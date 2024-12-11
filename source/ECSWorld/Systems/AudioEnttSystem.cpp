#include "AudioEnttSystem.hpp"

namespace Mona
{
	void EnttAudioSystem::StartUp(EnTTComponentManager &componentManager, EnTTEventManager &eventManager) noexcept
	{
		Config &config = Config::GetInstance();
		const int channels = config.getValueOrDefault<int>("N_OPENAL_SOURCES", 32);
		MONA_ASSERT(channels > 0, "EnttAudioSystem Error: please request more than zero channels");
		// Creaci�n de una instancia de ALCdevice y ALCcontext, y posterior chequeo.
		m_audioDevice = alcOpenDevice(nullptr);

		if (!m_audioDevice)
		{
			MONA_LOG_ERROR("EnttAudioSystem Error: Failed to open audio device.");
			return;
		}
		m_audioContext = alcCreateContext(m_audioDevice, NULL);
		if (!alcMakeContextCurrent(m_audioContext))
		{
			MONA_LOG_ERROR("EnttAudioSystem Error: Failed to make audio context current.");
			return;
		}

		ALCALL(alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED));
		m_masterVolume = 1.0f;
		ALCALL(alListenerf(AL_GAIN, m_masterVolume));

		// Se crean una fuente de OpenAL por cada canal solicitado
		m_channels = channels;
		m_openALSources.reserve(channels);
		for (unsigned int i = 0; i < channels; i++)
		{
			ALuint source = 0;
			ALCALL(alGenSources(1, &source));
			m_openALSources.emplace_back(source, i + 1);
		}
		m_firstFreeOpenALSourceIndex = 0;
	}

	void EnttAudioSystem::ShutDown(EnTTComponentManager &componentManager, EnTTEventManager &eventManager) noexcept
	{
		alcMakeContextCurrent(NULL);
		alcDestroyContext(m_audioContext);
		alcCloseDevice(m_audioDevice);
	}

	void EnttAudioSystem::Update(EnTTComponentManager &componentManager, EnTTEventManager &eventManager, float timeStep) noexcept
	{
		entt::registry &registry = componentManager.GetRegistry();
		auto viewEnTTWorld = registry.view<Mona::EnTTWorld>();

		glm::fquat audioListenerOffsetRotation;
		for (auto entity : viewEnTTWorld)
		{
			// Es solo uno, pero el for es necesario.
			Mona::EnTTWorld &world = viewEnTTWorld.get<Mona::EnTTWorld>(entity);
			audioListenerOffsetRotation = world.m_audioListenerOffsetRotation;
		}

		auto viewAudioTransform = registry.view<AudioSourceComponent, TransformComponent>();
		glm::vec3 listenerPosition = glm::vec3(0.0f);
		for (auto [entity, audio, transform] : viewAudioTransform.each())
		{
			listenerPosition = transform.GetLocalTranslation();
			glm::vec3 frontVector = glm::rotate(audioListenerOffsetRotation, transform.GetFrontVector());
			glm::vec3 upVector = glm::rotate(audioListenerOffsetRotation, transform.GetUpVector());
			UpdateListener(listenerPosition, frontVector, upVector);
			UpdateAudioSourceComponentsTimers(timeStep, audio);
		}

		auto viewAudioNoTransform = registry.view<AudioSourceComponent>(entt::exclude<TransformComponent>);
		viewAudioNoTransform.each([&](auto entity, AudioSourceComponent &audio)
								  {this->UpdateListener(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); });

		// Se remueven las fuentes libres que ya terminaron de reproducir su clip de audio
		RemoveCompletedFreeAudioSources();

		// Ambos tipos de fuentes avanzan sus timers en timeStep segundos
		UpdateFreeAudioSourcesTimers(timeStep);

		int audioDataCount = componentManager.GetComponentCount<AudioSourceComponent>();
		// Comienza la asignaci�n de fuentes de OpenAL a las fuentes del motor.
		//  Hay que hacer el filtro dentro de la propia función
		if (m_freeAudioSources.size() + audioDataCount <= m_openALSources.size())
		{
			// Si la suma de fuentes libres y fuentes ligadas a GameObjects es menor que la cantidad de fuentes de OpenAL disponibles
			// Asignamos recursos a todas.
			AssignOpenALSourceToFreeAudioSources(m_freeAudioSources.begin(), m_freeAudioSources.end());
			AssignOpenALSourceToAudioSourceComponents(registry);
		}
		else
		{

			// Remover recursos de OpenAL de fuentes libres 3D fuera  de rango y obtener iterador a la primera que quedo fuera
			auto firstOutFreeSource = PartitionAndRemoveOpenALSourceFromFreeAudioSources(listenerPosition);
			// Remover recursos de OpenAL de fuentes 3D fuera de rango o que terminaron de reproducir su audio clip
			//  y obtener iterador a la primera que quedo fuera
			uint32_t firstOutAudioComponent = PartitionAndRemoveOpenALSourceFromAudioSourceComponents(registry, listenerPosition);

			if (std::distance(m_freeAudioSources.begin(), firstOutFreeSource) + firstOutAudioComponent <= m_openALSources.size())
			{
				// Si descontando las fuentes recien descartadas los recursos de OpenAL son sufientes estos son asignados.
				AssignOpenALSourceToFreeAudioSources(m_freeAudioSources.begin(), firstOutFreeSource);
				AssignOpenALSourceToAudioSourceComponents(registry);
			}
			else
			{
				// Dado que aun los recursos de openAL no son suficientes para la cantidad de fuentes, se procede a ordenarlas por prioridad
				uint32_t countFreeSources[static_cast<unsigned int>(AudioSourcePriority::PriorityCount)] = {};
				uint32_t countAudioComponents[static_cast<unsigned int>(AudioSourcePriority::PriorityCount)] = {};
				SortFreeAudioSourcesByPriority(firstOutFreeSource, countFreeSources);
				SortAudioSourceComponentsByPriority(registry, countAudioComponents);

				// Una vez ordenadas tanto las fuentes libres como las unidas como componentes se procede a otorgar y quitar los recursos de OpenAL a estas
				uint32_t firstToRemoveFreeSource = 0;
				uint32_t firstToRemoveSourceComponent = 0;
				uint32_t sourceCount = m_openALSources.size();
				for (int i = 0; i < static_cast<unsigned int>(AudioSourcePriority::PriorityCount); i++)
				{
					if (sourceCount <= countAudioComponents[i])
					{
						firstToRemoveSourceComponent += sourceCount;
						break;
					}
					firstToRemoveSourceComponent += countAudioComponents[i];
					sourceCount -= countAudioComponents[i];

					if (sourceCount <= countFreeSources[i])
					{
						firstToRemoveFreeSource += sourceCount;
						break;
					}
					firstToRemoveFreeSource += countFreeSources[i];
					sourceCount -= countFreeSources[i];
				}
				RemoveOpenALSourceFromFreeAudioSources(m_freeAudioSources.begin() + firstToRemoveFreeSource, m_freeAudioSources.end());
				RemoveOpenALSourceFromAudioSourceComponents(registry);
				AssignOpenALSourceToFreeAudioSources(m_freeAudioSources.begin(), m_freeAudioSources.begin() + firstToRemoveFreeSource);
				AssignOpenALSourceToAudioSourceComponents(registry);
			}
		}
	}

	float EnttAudioSystem::GetMasterVolume() const noexcept
	{
		return m_masterVolume;
	}

	void EnttAudioSystem::SetMasterVolume(float volume) noexcept
	{
		// Para que el cambio de volumen sea global se le cambia esta propiedad al unico receptor
		m_masterVolume = std::clamp(volume, 0.0f, 1.0f);
		ALCALL(alListenerf(AL_GAIN, m_masterVolume));
	}

	void EnttAudioSystem::PlayAudioClip3D(std::shared_ptr<AudioClip> audioClip,
										  const glm::vec3 &position,
										  float volume,
										  float pitch,
										  float radius,
										  AudioSourcePriority priority)
	{
		if (audioClip == nullptr)
			return;
		m_freeAudioSources.emplace_back(audioClip,
										std::clamp(volume, 0.0f, 1.0f),
										std::max(0.0f, pitch),
										std::max(0.0f, radius),
										priority,
										SourceType::Source3D,
										position);
	}

	void EnttAudioSystem::PlayAudioClip2D(std::shared_ptr<AudioClip> audioClip,
										  float volume,
										  float pitch,
										  AudioSourcePriority priority)
	{
		if (audioClip == nullptr)
			return;
		m_freeAudioSources.emplace_back(audioClip,
										std::clamp(volume, 0.0f, 1.0f),
										std::max(0.0f, pitch),
										1.0f,
										priority,
										SourceType::Source2D);
	}

	void EnttAudioSystem::ClearSources() noexcept
	{
		for (auto &openALSource : m_openALSources)
		{
			ALCALL(alDeleteSources(1, &(openALSource.m_sourceID)));
		}
	}

	void EnttAudioSystem::RemoveOpenALSource(uint32_t index) noexcept
	{
		auto &alSource = m_openALSources[index];
		ALCALL(alSourcef(alSource.m_sourceID, AL_GAIN, 0.0f));
		ALCALL(alSourceStop(alSource.m_sourceID));
		ALCALL(alSourcei(alSource.m_sourceID, AL_BUFFER, 0));
		FreeOpenALSource(index);
	}

	void EnttAudioSystem::UpdateListener(const glm::vec3 &position, const glm::vec3 &frontVector, const glm::vec3 &upVector)
	{
		ALCALL(alListener3f(AL_POSITION, position.x, position.y, position.z));
		ALfloat forwardAndUpVectors[] = {
			frontVector.x, frontVector.y, frontVector.z,
			upVector.x, upVector.y, upVector.z};
		ALCALL(alListenerfv(AL_ORIENTATION, forwardAndUpVectors));
	}

	void EnttAudioSystem::RemoveCompletedFreeAudioSources()
	{
		// Se particiona el arregle de fuentes libres dejando al final las fuentes que terminaron de reproducir su audio clip (timeLeft <0)
		auto beginRemove = std::partition(m_freeAudioSources.begin(), m_freeAudioSources.end(),
										  [](const FreeAudioSource &audioSource)
										  {
											  return audioSource.m_timeLeft > 0.0f;
										  });

		// Se remueven los recursos asignados a las fuentes previo a su elimination.
		for (auto it = beginRemove; it != m_freeAudioSources.end(); it++)
		{
			if (it->m_openALsource)
			{
				AudioSource::OpenALSource openALSource = it->m_openALsource.value();
				ALCALL(alSourceStop(openALSource.m_sourceID));
				ALCALL(alSourcei(openALSource.m_sourceID, AL_BUFFER, 0));
				FreeOpenALSource(openALSource.m_sourceIndex);
			}
		}

		m_freeAudioSources.resize(std::distance(m_freeAudioSources.begin(), beginRemove));
	}

	void EnttAudioSystem::UpdateFreeAudioSourcesTimers(float timeStep)
	{
		// Se actualiza el timer de toas las fuentes libres en timeStep segundos ponderados por el tono de la fuente.
		for (auto &freeAudioSource : m_freeAudioSources)
		{
			freeAudioSource.m_timeLeft -= timeStep * freeAudioSource.m_pitch;
		}
	}

	void EnttAudioSystem::UpdateAudioSourceComponentsTimers(float timeStep, AudioSourceComponent &audioComponent)
	{
		// El proceso de actualizar las fuentes de audio usadas como componentes es un poco mas complejo.
		// Ya que estas pueden estar en repetici�n, en pausa o detenidas.
		if (audioComponent.m_sourceState == AudioSourceState::Paused || audioComponent.m_sourceState == AudioSourceState::Stopped)
			return;
		if (audioComponent.m_timeLeft < 0)
			audioComponent.m_sourceState = AudioSourceState::Stopped;
		audioComponent.m_timeLeft -= timeStep * audioComponent.m_pitch;
		if (audioComponent.m_isLooping)
		{
			while (audioComponent.m_timeLeft < 0)
			{
				audioComponent.m_timeLeft += audioComponent.m_audioClip->GetTotalTime();
			}
		}
	}

	std::vector<FreeAudioSource>::iterator EnttAudioSystem::PartitionAndRemoveOpenALSourceFromFreeAudioSources(const glm::vec3 &listenerPosition)
	{
		auto firstOutOfRange = std::partition(m_freeAudioSources.begin(), m_freeAudioSources.end(),
											  [&listenerPosition](const FreeAudioSource &audioSource)
											  {
												  return audioSource.m_sourceType == SourceType::Source2D || audioSource.m_radius * audioSource.m_radius > glm::length2(audioSource.m_position - listenerPosition);
											  });

		for (auto it = firstOutOfRange; it != m_freeAudioSources.end(); it++)
		{
			if (it->m_openALsource)
			{
				AudioSource::OpenALSource openALSource = it->m_openALsource.value();
				ALCALL(alSourcef(openALSource.m_sourceID, AL_GAIN, 0.0f));
				ALCALL(alSourceStop(openALSource.m_sourceID));
				ALCALL(alSourcei(openALSource.m_sourceID, AL_BUFFER, 0));
				FreeOpenALSource(openALSource.m_sourceIndex);
				it->m_openALsource = std::nullopt;
			}
		}

		return firstOutOfRange;
	}

	uint32_t EnttAudioSystem::PartitionAndRemoveOpenALSourceFromAudioSourceComponents(entt::registry &registry,
																					  const glm::vec3 &listenerPosition)
	{
		auto view = registry.view<AudioSourceComponent, TransformComponent>();
		std::vector<entt::entity> removableEnttites;
		uint32_t count = 0;

		for (auto entity : view)
		{
			auto &audioSource = view.get<AudioSourceComponent>(entity);
			auto &transform = view.get<TransformComponent>(entity);
			const glm::vec3 &position = transform.GetLocalTranslation();
			float squareRadius = audioSource.m_radius * audioSource.m_radius;
			float squareDistance = glm::distance2(position, listenerPosition);
			if (audioSource.m_sourceState == AudioSourceState::Playing &&
				squareDistance <= squareRadius)
			{
				count++;
			}
			else
			{
				removableEnttites.push_back(entity);
			}
		}

		for (auto entity : removableEnttites)
		{
			auto &audioSource = view.get<AudioSourceComponent>(entity);
			if (audioSource.m_openALsource)
			{
				AudioSource::OpenALSource openALSource = audioSource.m_openALsource.value();
				ALCALL(alSourcef(openALSource.m_sourceID, AL_GAIN, 0.0f));
				ALCALL(alSourceStop(openALSource.m_sourceID));
				ALCALL(alSourcei(openALSource.m_sourceID, AL_BUFFER, 0));
				FreeOpenALSource(openALSource.m_sourceIndex);
				audioSource.m_openALsource = std::nullopt;
			}
			registry.remove<AudioSourceComponent>(entity);
		}

		return count;
	}

	void EnttAudioSystem::AssignOpenALSourceToFreeAudioSources(std::vector<FreeAudioSource>::iterator begin, std::vector<FreeAudioSource>::iterator end)
	{
		for (auto it = begin; it != end; it++)
		{
			// Solo se asignan recursos si esta fuente no tiene uno asignado
			if (!it->m_openALsource)
			{
				auto unusedOpenALSource = GetNextFreeSource();
				it->m_openALsource = unusedOpenALSource;
				// Se actualiza los datos de la fuente de OpenAL con los datos nuevos
				if (it->m_sourceType == SourceType::Source2D)
				{
					ALCALL(alSourcei(unusedOpenALSource.m_sourceID, AL_SOURCE_RELATIVE, AL_TRUE));
					ALCALL(alSource3f(unusedOpenALSource.m_sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f));
				}
				else
				{
					const glm::vec3 &position = it->m_position;
					ALCALL(alSourcei(unusedOpenALSource.m_sourceID, AL_SOURCE_RELATIVE, AL_FALSE));
					ALCALL(alSource3f(unusedOpenALSource.m_sourceID, AL_POSITION, position.x, position.y, position.z));
				}

				ALCALL(alSourcei(unusedOpenALSource.m_sourceID, AL_LOOPING, AL_FALSE));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_PITCH, it->m_pitch));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_GAIN, it->m_volume));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_MAX_DISTANCE, it->m_radius));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_REFERENCE_DISTANCE, it->m_radius * 0.2f));
				ALCALL(alSourcei(unusedOpenALSource.m_sourceID, AL_BUFFER, it->m_audioClip->GetBufferID()));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_SEC_OFFSET, it->m_audioClip->GetTotalTime() - it->m_timeLeft));
				ALCALL(alSourcePlay(unusedOpenALSource.m_sourceID));
			}
		}
	}

	void EnttAudioSystem::AssignOpenALSourceToAudioSourceComponents(entt::registry &registry)
	{
		auto view = registry.view<AudioSourceComponent, TransformComponent>();

		for (auto [entity, audioSource, transform] : view.each())
		{
			// Solo se asignan recursos si esta fuente no tiene uno asignado
			if (!audioSource.m_openALsource)
			{
				auto unusedOpenALSource = GetNextFreeSource();
				audioSource.m_openALsource = unusedOpenALSource;

				if (audioSource.m_sourceType == SourceType::Source2D)
				{
					// En caso de que la fuente sea 2D se le asigna una posici�n relativa de (0.0,0.0,0.0)
					ALCALL(alSourcei(unusedOpenALSource.m_sourceID, AL_SOURCE_RELATIVE, AL_TRUE));
					ALCALL(alSource3f(unusedOpenALSource.m_sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f));
				}
				else
				{
					ALCALL(alSourcei(unusedOpenALSource.m_sourceID, AL_SOURCE_RELATIVE, AL_FALSE));
				}
				ALCALL(alSourcei(unusedOpenALSource.m_sourceID, AL_LOOPING, audioSource.m_isLooping));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_PITCH, audioSource.m_pitch));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_GAIN, audioSource.m_volume));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_MAX_DISTANCE, audioSource.m_radius));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_REFERENCE_DISTANCE, audioSource.m_radius * 0.2f));
				ALCALL(alSourcei(unusedOpenALSource.m_sourceID, AL_BUFFER, audioSource.m_audioClip->GetBufferID()));
				ALCALL(alSourcef(unusedOpenALSource.m_sourceID, AL_SEC_OFFSET, audioSource.m_audioClip->GetTotalTime() - audioSource.m_timeLeft));

				if (audioSource.m_sourceState == AudioSourceState::Playing)
				{
					ALCALL(alSourcePlay(unusedOpenALSource.m_sourceID));
				}
			}

			if (audioSource.m_sourceType == SourceType::Source3D)
			{
				// Si la fuente es 3D se actualizan las posiciones
				const glm::vec3 &position = transform.GetLocalTranslation();
				ALCALL(alSource3f(audioSource.m_openALsource.value().m_sourceID, AL_POSITION, position.x, position.y, position.z));
			}
		}
	}

	void EnttAudioSystem::RemoveOpenALSourceFromFreeAudioSources(std::vector<FreeAudioSource>::iterator begin,
																 std::vector<FreeAudioSource>::iterator end)
	{
		for (auto it = begin; it != end; it++)
		{
			if (it->m_openALsource)
			{
				AudioSource::OpenALSource openALSource = it->m_openALsource.value();
				ALCALL(alSourcef(openALSource.m_sourceID, AL_GAIN, 0.0f));
				ALCALL(alSourceStop(openALSource.m_sourceID));
				ALCALL(alSourcei(openALSource.m_sourceID, AL_BUFFER, 0));
				FreeOpenALSource(openALSource.m_sourceIndex);
				it->m_openALsource = std::nullopt;
			}
		}
	}

	void EnttAudioSystem::RemoveOpenALSourceFromAudioSourceComponents(entt::registry &registry)
	{
		auto view = registry.view<AudioSourceComponent>();
		std::vector<entt::entity> removableEntities;

		for (auto entity : view)
		{
			AudioSourceComponent &audioSource = view.get<AudioSourceComponent>(entity);

			if (audioSource.m_openALsource)
			{
				AudioSource::OpenALSource openALSource = audioSource.m_openALsource.value();

				ALCALL(alSourcef(openALSource.m_sourceID, AL_GAIN, 0.0f));
				ALCALL(alSourceStop(openALSource.m_sourceID));
				ALCALL(alSourcei(openALSource.m_sourceID, AL_BUFFER, 0));
				FreeOpenALSource(openALSource.m_sourceIndex);
				audioSource.m_openALsource = std::nullopt;
			}

			removableEntities.push_back(entity);
		}

		for (auto entity : removableEntities)
		{
			registry.remove<AudioSourceComponent>(entity);
		}
	}

	void EnttAudioSystem::SortFreeAudioSourcesByPriority(std::vector<FreeAudioSource>::iterator end,
														 uint32_t (&outCount)[static_cast<unsigned int>(AudioSourcePriority::PriorityCount)])
	{
		// La lista se ordena usando una implementaci�n inplace de count sort
		//  La implementaci�n esta basada en el mensaje de Sebastian Mestre en la siguiente pregunta en stackoverflow
		//  https://stackoverflow.com/questions/30547452/is-stdsort-the-best-choice-to-do-in-place-sort-for-a-huge-array-with-limited-i
		unsigned int counts[static_cast<unsigned int>(AudioSourcePriority::PriorityCount)] = {};
		unsigned int offsetCounts[static_cast<unsigned int>(AudioSourcePriority::PriorityCount)] = {};

		for (auto it = m_freeAudioSources.begin(); it != end; it++)
		{
			counts[static_cast<unsigned int>(it->m_priority)]++;
			outCount[static_cast<unsigned int>(it->m_priority)]++;
		}
		for (int i = 1; i < static_cast<unsigned int>(AudioSourcePriority::PriorityCount); i++)
		{
			counts[i] += counts[i - 1];
			offsetCounts[i] = counts[i - 1];
		}
		auto begin = m_freeAudioSources.begin();
		for (auto it = begin; it != end; it++)
		{
			unsigned int priorityIndex = static_cast<unsigned int>(it->m_priority);

			while (std::distance(begin, it) != offsetCounts[priorityIndex] &&
				   counts[priorityIndex] != offsetCounts[priorityIndex])
			{
				if (it->m_priority != (begin + offsetCounts[priorityIndex])->m_priority)
					std::iter_swap(it, begin + offsetCounts[priorityIndex]);
				++offsetCounts[priorityIndex];
				unsigned int priorityIndex = static_cast<unsigned int>(it->m_priority);
			}
		}
	}

	void EnttAudioSystem::SortAudioSourceComponentsByPriority(entt::registry &registry, uint32_t (&outCount)[static_cast<unsigned int>(AudioSourcePriority::PriorityCount)])
	{
		for (unsigned int &count : outCount)
		{
			count = 0;
		}

		std::vector<std::pair<entt::entity, AudioSourcePriority>> audioSources;

		auto view = registry.view<AudioSourceComponent>();
		for (auto entity : view)
		{
			auto &audioSource = view.get<AudioSourceComponent>(entity);
			unsigned int priorityIndex = static_cast<unsigned int>(audioSource.m_priority);
			audioSources.emplace_back(entity, audioSource.m_priority);
			outCount[priorityIndex]++;
		}

		std::stable_sort(audioSources.begin(), audioSources.end(), [](const auto &lhs, const auto &rhs)
						 { return static_cast<unsigned int>(lhs.second) < static_cast<unsigned int>(rhs.second); });

		for (size_t i = 0; i < audioSources.size(); ++i)
		{
			entt::entity entity = audioSources[i].first;
			auto audioSource = view.get<AudioSourceComponent>(entity);
			registry.replace<AudioSourceComponent>(entity, audioSource);
		}
	}

	void EnttAudioSystem::FreeOpenALSource(uint32_t index)
	{
		auto &freeEntry = m_openALSources[index];
		if (m_firstFreeOpenALSourceIndex == m_channels)
		{
			m_firstFreeOpenALSourceIndex = index;
			freeEntry.m_nextFreeIndex = m_channels;
		}
		else
		{
			auto &firstFreeEntry = m_openALSources[m_firstFreeOpenALSourceIndex];
			freeEntry.m_nextFreeIndex = m_firstFreeOpenALSourceIndex;
			m_firstFreeOpenALSourceIndex = index;
		}
	}

	AudioSource::OpenALSource EnttAudioSystem::GetNextFreeSource()
	{
		// Dado que m_firstFreeOpenALSourceIndex siempre tiene el indice con la primera fuente de OpenAL libre
		// basta con retornar la fuente en dicho indice y luego actualizar como corresponde el valor de m_firstFreeOpenALSourceIndex
		MONA_ASSERT(m_firstFreeOpenALSourceIndex != m_channels, "EnttAudioSystem Error: Not enough openal sources available");
		auto &entry = m_openALSources[m_firstFreeOpenALSourceIndex];
		uint32_t index = m_firstFreeOpenALSourceIndex;
		m_firstFreeOpenALSourceIndex = entry.m_nextFreeIndex;
		return AudioSource::OpenALSource(entry.m_sourceID, index);
	}

}
