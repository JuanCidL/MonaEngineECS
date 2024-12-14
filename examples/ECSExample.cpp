#include "MonaEngine.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Systems/StatsEnTTSystem.hpp"
class MinimalSetup : public Mona::Application
{
public:
	MinimalSetup() = default;
	~MinimalSetup() = default;
	virtual void UserStartUp(Mona::World &world) noexcept override{
		statsSystem.StartUp(componentManager, eventManager);
		auto e1 = componentManager.CreateEntity();
		auto e2 = componentManager.CreateEntity();
		auto e3 = componentManager.CreateEntity();

		componentManager.DestroyEntity(e1);
		componentManager.DestroyEntity(e2);
		componentManager.DestroyEntity(e3);

	}

	virtual void UserShutDown(Mona::World& world) noexcept override {
		statsSystem.ShutDown(componentManager, eventManager);
	}
	virtual void UserUpdate(Mona::World& world, float timeStep) noexcept override {
		statsSystem.Update(componentManager, eventManager, timeStep);
	}

private:
	Mona::EnTTComponentManager componentManager;
	Mona::EnTTEventManager eventManager;
	Mona::StatsEnTTSystem statsSystem;
};
int main()
{	
	MinimalSetup app;
	Mona::Engine engine(app);
	engine.StartMainLoop();
	
}