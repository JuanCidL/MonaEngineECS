#include "MonaEngine.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Systems/StatsSystem.hpp"

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

class MinimalSetup : public Mona::Application
{
public:
	MinimalSetup() = default;
	~MinimalSetup() = default;
	virtual void UserStartUp(Mona::World &world) noexcept override
	{
		auto e1 = componentManager.CreateEntity();
		auto e2 = componentManager.CreateEntity();
		auto e3 = componentManager.CreateEntity();

		componentManager.DestroyEntity(e1);
		componentManager.DestroyEntity(e2);

		MonaECS::Stats &st = componentManager.AddComponent<MonaECS::Stats>(e3);

		eventManager.Subscribe<TestEvent, &func>();
		eventManager.Subscribe<TestEvent, ExampleClassUsingEvent, &ExampleClassUsingEvent::OnEvent>(exampleClass);
		systemManager.RegisterSystem<MonaECS::StatsSystem>();
		systemManager.StartUpSystems(componentManager, eventManager);
	}

	virtual void UserShutDown(Mona::World &world) noexcept override
	{
		systemManager.ShutDownSystems(componentManager, eventManager);
	}
	virtual void UserUpdate(Mona::World &world, float timeStep) noexcept override
	{
		systemManager.UpdateSystems(componentManager, eventManager, timeStep);
		time += timeStep;
		if (time > 1.0f)
		{
			TestEvent event{10};
			eventManager.Publish(event);
			time = 0.0f;
		}
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