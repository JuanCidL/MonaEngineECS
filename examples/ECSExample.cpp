#include "MonaEngine.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Systems/StatsSystem.hpp"
#include "ECS/Systems/CollisionSystem.hpp"

struct TestEvent
{
	int value;
};
void func(const TestEvent &event)
{
	std::cout << "Event value: " << event.value << std::endl;
}

class ExampleClassUsingEvent
{
public:
	ExampleClassUsingEvent() = default;
	~ExampleClassUsingEvent() = default;
	void OnEvent(const TestEvent &event)
	{
		std::cout << "Event value: " << event.value << std::endl;
	}
};

void CreateCamera(Mona::World &world)
{
	auto camera = world.CreateGameObject<Mona::GameObject>();
	auto transform = world.AddComponent<Mona::TransformComponent>(camera, glm::vec3(0.0f, -15.0f, 15.0f));
	transform->Rotate(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f);
	world.SetMainCamera(world.AddComponent<Mona::CameraComponent>(camera));
}

class Box : public Mona::GameObject
{
public:
	Box(float velocity, MonaECS::ComponentManager *componentManager) : m_paddleVelocity(velocity), componentManager(componentManager) {}
	~Box() = default;
	virtual void UserStartUp(Mona::World &world) noexcept
	{
		m_transform = world.AddComponent<Mona::TransformComponent>(*this);

		auto boxTransformEntity = componentManager->CreateEntity();

		auto &meshManager = Mona::MeshManager::GetInstance();
		auto box = world.CreateGameObject<Mona::GameObject>();
		float boxSize = 0.5f;
		m_boxTransform = world.AddComponent<Mona::TransformComponent>(box);
		m_boxTransform->SetRotation(m_transform->GetLocalRotation());
		m_boxTransform->SetTranslation(m_transform->GetLocalTranslation() + glm::vec3(0.0f, 2.0f, 0.0f));
		m_boxTransform->SetScale(glm::vec3(boxSize));
		componentManager->AddComponent<MonaECS::TransformComponent>(boxTransformEntity, &m_boxTransform);

		// Entity for the box
		auto e = componentManager->CreateEntity();
		componentManager->AddComponent<MonaECS::TransformComponent>(e, &m_boxTransform);
		componentManager->AddComponent<MonaECS::ColliderComponent>(e, glm::vec3(boxSize), true);
		componentManager->AddComponent<MonaECS::BodyComponent>(e, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), 1.0f);


		auto boxMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
		boxMaterial->SetDiffuseColor(glm::vec3(0.75f, 0.3f, 0.3f));
		world.AddComponent<Mona::StaticMeshComponent>(box, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), boxMaterial);

		Mona::BoxShapeInformation boxInfo(glm::vec3(boxSize));
	}

	virtual void UserUpdate(Mona::World &world, float timeStep) noexcept
	{
	}

private:
	Mona::TransformHandle m_transform;
	Mona::TransformHandle m_boxTransform;
	MonaECS::ComponentManager *componentManager;

	float m_paddleVelocity;
};

class MinimalSetup : public Mona::Application
{
public:
	MinimalSetup() = default;
	~MinimalSetup() = default;
	virtual void UserStartUp(Mona::World &world) noexcept override
	{
		// auto e1 = componentManager.CreateEntity();
		// auto e2 = componentManager.CreateEntity();
		// auto e3 = componentManager.CreateEntity();

		// componentManager.DestroyEntity(e1);

		// MonaECS::Stats &st = componentManager.AddComponent<MonaECS::Stats>(e3);
		// componentManager.AddComponent<MonaECS::TransformComponent>(e3);

		// eventManager.Subscribe<TestEvent, &func>();
		//eventManager.Subscribe<TestEvent, ExampleClassUsingEvent, &ExampleClassUsingEvent::OnEvent>(exampleClass);
		world.SetAmbientLight(glm::vec3(0.3f));
		CreateCamera(world);
		world.CreateGameObject<Box>(1.0f, &componentManager);

		systemManager.RegisterSystem<MonaECS::StatsSystem>();
		systemManager.RegisterSystem<MonaECS::CollisionSystem>();

		systemManager.StartUpSystems(componentManager, eventManager);
	}

	virtual void UserShutDown(Mona::World &world) noexcept override
	{
		systemManager.ShutDownSystems(componentManager, eventManager);
	}
	virtual void UserUpdate(Mona::World &world, float timeStep) noexcept override
	{
		systemManager.UpdateSystems(componentManager, eventManager, timeStep);
		// time += timeStep;
		// if (time > 1.0f)
		// {
		// 	TestEvent event{10};
		// 	eventManager.Publish(event);
		// 	time = 0.0f;
		// }
	}

private:
	MonaECS::ComponentManager componentManager;
	MonaECS::EventManager eventManager;
	MonaECS::SystemManager systemManager;
	MonaECS::StatsSystem statsSystem;
	ExampleClassUsingEvent exampleClass;
	float time = 0.0f;
};
int main()
{
	MinimalSetup app;
	Mona::Engine engine(app);
	engine.StartMainLoop();
}