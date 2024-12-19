#include "MonaEngine.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Systems/StatsSystem.hpp"
#include "ECS/Systems/CollisionSystem.hpp"
#include "ECS/Components/CollisionComponents.hpp"
#include "Rendering/DiffuseFlatMaterial.hpp"

void CreateBasicCameraWithMusicAndLight(Mona::World &world)
{
    auto camera = world.CreateGameObject<Mona::GameObject>();
    auto transform = world.AddComponent<Mona::TransformComponent>(camera, glm::vec3(0.0f, -15.0f, 15.0f));
    transform->Rotate(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f);
    world.SetMainCamera(world.AddComponent<Mona::CameraComponent>(camera));
    world.SetAudioListenerTransform(transform);
    auto &config = Mona::Config::GetInstance();
    auto &audioClipManager = Mona::AudioClipManager::GetInstance();
    auto audioClipPtr = audioClipManager.LoadAudioClip(config.getPathOfApplicationAsset("AudioFiles/music.wav"));
    auto audioSource = world.AddComponent<Mona::AudioSourceComponent>(camera, audioClipPtr);
    audioSource->SetIsLooping(true);
    audioSource->SetVolume(0.3f);
    audioSource->Play();

    world.AddComponent<Mona::DirectionalLightComponent>(camera, glm::vec3(1.0f));
}

class Wall : public Mona::GameObject{
public:
    Wall(const glm::vec3 &position, const glm::vec3 &scale, MonaECS::ComponentManager* component) : m_position(position), m_scale(scale), componentManager(component){};
    ~Wall() = default;
    virtual void UserStartUp(Mona::World &world) noexcept{
        m_transform = world.AddComponent<Mona::TransformComponent>(*this);  
        auto wall = world.CreateGameObject<Mona::GameObject>();
        auto &meshManager = Mona::MeshManager::GetInstance();
        
        m_wallTransform = world.AddComponent<Mona::TransformComponent>(wall);
        m_wallTransform->SetTranslation(m_position);
        m_wallTransform->SetScale(m_scale);

        auto wallMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
        world.AddComponent<Mona::StaticMeshComponent>(wall, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), wallMaterial);

        auto wallEntity = componentManager->CreateEntity();
        componentManager->AddComponent<MonaECS::TransformComponent>(wallEntity, &m_wallTransform);
        componentManager->AddComponent<MonaECS::ColliderComponent>(wallEntity, m_scale, false);
    }


private:
    const glm::vec3& m_position;
    const glm::vec3& m_scale;
    Mona::TransformHandle m_transform;
    Mona::TransformHandle m_wallTransform;
    MonaECS::ComponentManager * componentManager;

};

class Ball : public Mona::GameObject
{
public:
    Ball(MonaECS::ComponentManager *componentManager) :  componentManager(componentManager) {}
    ~Ball() = default;
    virtual void UserStartUp(Mona::World &world) noexcept
    {
        m_transform = world.AddComponent<Mona::TransformComponent>(*this);
        auto &meshManager = Mona::MeshManager::GetInstance();
        auto ball = world.CreateGameObject<Mona::GameObject>();
        float ballRadius = 0.5f;

        auto ballEntity = componentManager->CreateEntity();
        componentManager->AddComponent<MonaECS::TransformComponent>(ballEntity, &m_ballTransform);
        componentManager->AddComponent<MonaECS::BodyComponent>(ballEntity, glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);
        componentManager->AddComponent<MonaECS::ColliderComponent>(ballEntity, glm::vec3(ballRadius), false);

        m_ballTransform = world.AddComponent<Mona::TransformComponent>(ball);
        m_ballTransform->SetRotation(m_transform->GetLocalRotation());
        m_ballTransform->SetTranslation(m_transform->GetLocalTranslation() + glm::vec3(0.0f, 2.0f, 0.0f));
        m_ballTransform->SetScale(glm::vec3(ballRadius));

        auto ballMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
        ballMaterial->SetDiffuseColor(glm::vec3(0.75f, 0.3f, 0.3f));
        world.AddComponent<Mona::StaticMeshComponent>(ball, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Sphere), ballMaterial);
    }

    virtual void UserUpdate(Mona::World &world, float timeStep) noexcept
    {
        // m_ballRigidBody->SetLinearVelocity(glm::vec3(0.0f, 15.0f, 0.0f));
    }

private:
    Mona::TransformHandle m_transform;
    Mona::TransformHandle m_ballTransform;
    Mona::RigidBodyHandle m_ballRigidBody;
    std::shared_ptr<Mona::AudioClip> m_ballBounceSound;
    MonaECS::ComponentManager *componentManager;
};

class Breakout : public Mona::Application
{
public:
    Breakout() = default;
    ~Breakout() = default;
    virtual void UserStartUp(Mona::World &world) noexcept override
    {
        world.SetGravity(glm::vec3(0.0f, 0.0f, 0.0f));
        world.SetAmbientLight(glm::vec3(0.3f));
        CreateBasicCameraWithMusicAndLight(world);
        world.CreateGameObject<Ball>(&componentManager);

        world.CreateGameObject<Wall>(glm::vec3(0.0f, 26.0f, 0.0f), glm::vec3(18.0f, 1.0f, 1.0f), &componentManager);
        glm::vec3 sideWallScale(1.0f, 27.0f, 1.0f);
		float sideWallOffset = 19.0f;
        world.CreateGameObject<Wall>(glm::vec3(-sideWallOffset, 0.0f, 0.0f), sideWallScale, &componentManager);
        world.CreateGameObject<Wall>(glm::vec3(sideWallOffset, 0.0f, 0.0f), sideWallScale, &componentManager);

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
    }
    std::shared_ptr<Mona::AudioClip> m_blockBreakingSound;

private:
    MonaECS::ComponentManager componentManager;
    MonaECS::EventManager eventManager;
    MonaECS::SystemManager systemManager;
    MonaECS::StatsSystem statsSystem;
};
int main()
{
    Breakout breakout;
    Mona::Engine engine(breakout);
    engine.StartMainLoop();
}