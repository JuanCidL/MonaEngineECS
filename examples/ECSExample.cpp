#include "MonaEngine.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Systems/StatsEnTTSystem.hpp"
class MinimalSetup : public Mona::Application
{
public:
	MinimalSetup() = default;
	~MinimalSetup() = default;
	virtual void UserStartUp(Mona::World &world) noexcept override
	{
		// statsSystem.StartUp(componentManager, eventManager);
		auto e1 = componentManager.CreateEntity();
		auto e2 = componentManager.CreateEntity();
		auto e3 = componentManager.CreateEntity();

		componentManager.DestroyEntity(e1);
		componentManager.DestroyEntity(e2);
		// componentManager.DestroyEntity(e3);
		// Mona::Comp &uw = componentManager.AddComponent<Mona::Comp>(e3, 10.0f);
		// Mona::Comp2 &uw2 = componentManager.AddComponent<Mona::Comp2>(e3, 10.0f);

		// Mona::Comp &uw4 = componentManager.AddComponent<Mona::Comp>(e2, 10.0f);

		Mona::Stats &st = componentManager.AddComponent<Mona::Stats>(e3);

		systemManager.RegisterSystem<Mona::StatsEnTTSystem>();
		systemManager.StartUpSystems(componentManager, eventManager);
	}

	virtual void UserShutDown(Mona::World &world) noexcept override
	{
		// statsSystem.ShutDown(componentManager, eventManager);
		systemManager.ShutDownSystems(componentManager, eventManager);
	}
	virtual void UserUpdate(Mona::World &world, float timeStep) noexcept override
	{
		// statsSystem.Update(componentManager, eventManager, timeStep);
		// auto count = componentManager.GetComponentCount<Mona::Comp>();
		// auto count2 = componentManager.GetComponentCount<Mona::Comp2>();
		// std::cout << "Comp count: " << count << std::endl;
		// std::cout << "Comp2 count: " << count2 << std::endl;
		systemManager.UpdateSystems(componentManager, eventManager, timeStep);
	}

private:
	Mona::EnTTComponentManager componentManager;
	Mona::EnTTEventManager eventManager;
	Mona::EnTTSystemManager systemManager;
	Mona::StatsEnTTSystem statsSystem;
};
int main()
{
	MinimalSetup app;
	Mona::Engine engine(app);
	engine.StartMainLoop();
}