#include "MonaEngine.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Systems/StatsSystem.hpp"
#include "ECS/Systems/CollisionSystem.hpp"
#include "ECS/Systems/MovementSystem.hpp"
#include "ECS/Systems/InputSystem.hpp"
#include "ECS/Events/CollisionEvents.hpp"
#include <ECS/Events/InputEvents.hpp>

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

void OnInputEvent(const MonaECS::MoveInputEvent &event)
{
	std::cout<<"Move input event detected"<<std::endl;
	// std::cout << "Move input event detected" << event.moveDir.x << " " << event.moveDir.y << " " << event.moveDir.z << std::endl;
}

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
	Box(float velocity, MonaECS::ComponentManager *componentManager, glm::vec3 vel=glm::vec3(0.0f, 0.0f, 1.0f)) : m_velocity(vel), componentManager(componentManager) {}
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
		boxEntity = e;
		componentManager->AddComponent<MonaECS::TransformComponent>(e, &m_boxTransform);
		componentManager->AddComponent<MonaECS::ColliderComponent>(e, glm::vec3(boxSize), true);
		componentManager->AddComponent<MonaECS::BodyComponent>(e, m_velocity, glm::vec3(0.0f), 1.0f);


		auto boxMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
		boxMaterial->SetDiffuseColor(glm::vec3(0.75f, 0.3f, 0.3f));
		world.AddComponent<Mona::StaticMeshComponent>(box, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), boxMaterial);

		Mona::BoxShapeInformation boxInfo(glm::vec3(boxSize));
	}

	virtual void UserUpdate(Mona::World &world, float timeStep) noexcept
	{
	}

	void SetTranslation(glm::vec3 translation)
	{
		m_transform->SetTranslation(translation);
		m_boxTransform->SetTranslation(translation + glm::vec3(0.0f, 2.0f, 0.0f));
	}

	entt::entity GetBoxEntity()
	{
		return boxEntity;
	}

private:
	Mona::TransformHandle m_transform;
	Mona::TransformHandle m_boxTransform;
	MonaECS::ComponentManager *componentManager;
	entt::entity boxEntity;

	glm::vec3 m_velocity;
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
		componentManager.SetWorld(&world);

		auto box1 = world.CreateGameObject<Box>(1.0f, &componentManager);
		auto box2 = world.CreateGameObject<Box>(-1.0f, &componentManager, glm::vec3(0.0f, 0.0f, -1.0f));

		auto b1e = box1->GetBoxEntity();
		componentManager.AddComponent<MonaECS::MoveInputComponent>(b1e);

		box2->SetTranslation(glm::vec3(0.0f, 0.0f, 20.0f));

		// now in x axis
		auto box3 = world.CreateGameObject<Box>(1.0f, &componentManager, glm::vec3(1.0f, 0.0f, 0.0f));
		auto box4 = world.CreateGameObject<Box>(-1.0f, &componentManager, glm::vec3(-1.0f, 0.0f, 0.0f));

		box3->SetTranslation(glm::vec3(0.0f, 0.0f, 5.0f));
		box4->SetTranslation(glm::vec3(20.0f, 0.0f, 5.0f));


		// now in y axis
		auto box5 = world.CreateGameObject<Box>(1.0f, &componentManager, glm::vec3(0.0f, 1.0f, 0.0f));
		auto box6 = world.CreateGameObject<Box>(-1.0f, &componentManager, glm::vec3(0.0f, -1.0f, 0.0f));

		box5->SetTranslation(glm::vec3(0.0f, 0.0f, 10.0f));
		box6->SetTranslation(glm::vec3(0.0f, 20.0f, 10.0f));





		systemManager.RegisterSystem<MonaECS::StatsSystem>();
		systemManager.RegisterSystem<MonaECS::CollisionSystem>();
		systemManager.RegisterSystem<MonaECS::MovementSystem>();
		systemManager.RegisterSystem<MonaECS::InputSystem>();

		systemManager.StartUpSystems(componentManager, eventManager);
		eventManager.Subscribe<MonaECS::CollisionEvent, MinimalSetup, &MinimalSetup::OnCollision>(*this);
		eventManager.Subscribe<MonaECS::MoveInputEvent, &OnInputEvent>();
	}

	virtual void UserShutDown(Mona::World &world) noexcept override
	{
		systemManager.ShutDownSystems(componentManager, eventManager);
	}
	virtual void UserUpdate(Mona::World &world, float timeStep) noexcept override
	{
		systemManager.UpdateSystems(componentManager, eventManager, timeStep);
	}

	void OnCollision(const MonaECS::CollisionEvent &event)
	{
		std::cout << "Collision detected" << std::endl;
		auto e1 = event.entity1;
		auto e2 = event.entity2;
		auto normal = event.normal;

		auto &transform1 = componentManager.GetComponent<MonaECS::TransformComponent>(e1);
		auto &transform2 = componentManager.GetComponent<MonaECS::TransformComponent>(e2);

		// Inverse the velocity of the body by the normal
		auto &body1 = componentManager.GetComponent<MonaECS::BodyComponent>(e1);
		auto &body2 = componentManager.GetComponent<MonaECS::BodyComponent>(e2);

		body1.velocity = glm::reflect(body1.velocity, normal);
		body2.velocity = glm::reflect(body2.velocity, normal);
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