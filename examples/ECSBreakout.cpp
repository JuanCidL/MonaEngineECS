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

class Ball : public Mona::GameObject
{
public:
    Ball(float velocity, MonaECS::ComponentManager *componentManager) : m_paddleVelocity(velocity), componentManager(componentManager) {}
    ~Ball() = default;
    virtual void UserStartUp(Mona::World &world) noexcept
    {
        m_transform = world.AddComponent<Mona::TransformComponent>(*this);

        auto ballTransformEntity = componentManager->CreateEntity();

        auto &meshManager = Mona::MeshManager::GetInstance();
        auto ball = world.CreateGameObject<Mona::GameObject>();
        float ballRadius = 0.5f;
        m_ballTransform = world.AddComponent<Mona::TransformComponent>(ball);
        m_ballTransform->SetRotation(m_transform->GetLocalRotation());
        m_ballTransform->SetTranslation(m_transform->GetLocalTranslation() + glm::vec3(0.0f, 2.0f, 0.0f));
        m_ballTransform->SetScale(glm::vec3(ballRadius));
        componentManager->AddComponent<MonaECS::TransformComponent>(ballTransformEntity, &m_ballTransform);

        auto ballMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
        ballMaterial->SetDiffuseColor(glm::vec3(0.75f, 0.3f, 0.3f));
        world.AddComponent<Mona::StaticMeshComponent>(ball, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Sphere), ballMaterial);

        Mona::SphereShapeInformation sphereInfo(ballRadius);
        m_ballRigidBody = world.AddComponent<Mona::RigidBodyComponent>(ball, sphereInfo, Mona::RigidBodyType::DynamicBody);
        m_ballRigidBody->SetRestitution(1.0f);
        m_ballRigidBody->SetFriction(0.0f);
        auto callback = [ballTransform = m_ballTransform, ballSound = m_ballBounceSound](Mona::World &world, Mona::RigidBodyHandle &otherRigidBody, bool isSwaped, Mona::CollisionInformation &colInfo) mutable
        {
            world.PlayAudioClip3D(ballSound, ballTransform->GetLocalTranslation(), 0.3f);
        };
        m_ballRigidBody->SetStartCollisionCallback(callback);
    }

    virtual void UserUpdate(Mona::World &world, float timeStep) noexcept
    {
        m_ballRigidBody->SetLinearVelocity(glm::vec3(0.0f, 15.0f, 0.0f));
    }

private:
    Mona::TransformHandle m_transform;
    Mona::TransformHandle m_ballTransform;
    Mona::RigidBodyHandle m_ballRigidBody;
    std::shared_ptr<Mona::AudioClip> m_ballBounceSound;
    MonaECS::ComponentManager *componentManager;

    float m_paddleVelocity;
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
        world.CreateGameObject<Ball>(20.0f, &componentManager);
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