#include "MonaEngine.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Systems.hpp"
#include "ECS/Components/CollisionComponents.hpp"
#include "ECS/Components/StatComponents.hpp"
#include "ECS/Components/InputComponents.hpp"
#include "ECS/Events/CollisionEvents.hpp"
#include "ECS/Events/ColorChangeEvents.hpp"
#include "ECS/Events/InputEvents.hpp"
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

void OnInputEvent(const MonaECS::MoveInputEvent &event)
{
    std::cout << "Move input event detected" << std::endl;
    // std::cout << "Move input event detected" << event.moveDir.x << " " << event.moveDir.y << " " << event.moveDir.z << std::endl;
}

class Wall : public Mona::GameObject
{
public:
    Wall(const glm::vec3 &position,
         const glm::vec3 &scale,
         MonaECS::ComponentManager *component,
         MonaECS::EventManager *event)
        : m_position(position), m_scale(scale), componentManager(component), eventManager(event) {};
    ~Wall() = default;
    virtual void UserStartUp(Mona::World &world) noexcept
    {
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
        componentManager->AddComponent<MonaECS::BodyComponent>(wallEntity, glm::vec3(0.0f), glm::vec3(0.0f), 10.0f);
        componentManager->AddComponent<MonaECS::ColliderComponent>(wallEntity, m_scale, false);
    }

private:
    const glm::vec3 &m_position;
    const glm::vec3 &m_scale;
    Mona::TransformHandle m_transform;
    Mona::TransformHandle m_wallTransform;
    MonaECS::ComponentManager *componentManager;
    MonaECS::EventManager *eventManager;
};

class Block : public Mona::GameObject
{
public:
    Block(const glm::vec3 &position, const glm::vec3 &scale, MonaECS::ComponentManager *component, MonaECS::EventManager *event) : m_position(position), m_scale(scale), componentManager(component), eventManager(event) {};
    ~Block() = default;
    virtual void UserStartUp(Mona::World &world) noexcept
    {
        m_transform = world.AddComponent<Mona::TransformComponent>(*this);
        auto block = world.CreateGameObject<Mona::GameObject>();
        auto &meshManager = Mona::MeshManager::GetInstance();

        m_blockTransform = world.AddComponent<Mona::TransformComponent>(block);
        m_blockTransform->SetTranslation(m_position);
        m_blockTransform->SetScale(m_scale);

        auto redBlockMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
        redBlockMaterial->SetDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
        auto greenBlockMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
        greenBlockMaterial->SetDiffuseColor(glm::vec3(0.0f, 1.0f, 0.0f));
        auto blueBlockMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
        blueBlockMaterial->SetDiffuseColor(glm::vec3(0.0f, 0.0f, 1.0f));

        m_materialVector.push_back(redBlockMaterial);
        m_materialVector.push_back(greenBlockMaterial);
        m_materialVector.push_back(blueBlockMaterial);

        m_meshHandle = world.AddComponent<Mona::StaticMeshComponent>(block, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), m_materialVector[0]);

        auto blockEntity = componentManager->CreateEntity();
        componentManager->AddComponent<MonaECS::TransformComponent>(blockEntity, &m_blockTransform);
        componentManager->AddComponent<MonaECS::BodyComponent>(blockEntity, glm::vec3(0.0f), glm::vec3(0.0f), 10.0f);
        componentManager->AddComponent<MonaECS::ColliderComponent>(blockEntity, m_scale, false);
        componentManager->AddComponent<MonaECS::Stats>(blockEntity);
        eventManager->Subscribe<MonaECS::ColorChangeEvent, Block, &Block::OnColorChange>(*this);
    }

    void OnColorChange(const MonaECS::ColorChangeEvent &event)
    {
        entt::registry &registry = componentManager->GetRegistry();
        auto ent = event.entity;
        auto stat = registry.get<MonaECS::Stats>(ent);
        m_meshHandle->SetMaterial(m_materialVector[static_cast<int>(stat.state)]);
    }

private:
    const glm::vec3 &m_position;
    const glm::vec3 &m_scale;
    Mona::TransformHandle m_transform;
    Mona::TransformHandle m_blockTransform;
    Mona::StaticMeshHandle m_meshHandle;
    std::vector<std::shared_ptr<Mona::DiffuseFlatMaterial>> m_materialVector;
    MonaECS::ComponentManager *componentManager;
    MonaECS::EventManager *eventManager;
};

class Paddle : public Mona::GameObject
{
public:
    Paddle(MonaECS::ComponentManager *component, MonaECS::EventManager *event) : componentManager(component), eventManager(event) {};
    ~Paddle() = default;
    virtual void UserStartUp(Mona::World &world) noexcept
    {
        m_transform = world.AddComponent<Mona::TransformComponent>(*this);
        auto paddle = world.CreateGameObject<Mona::GameObject>();
        auto &meshManager = Mona::MeshManager::GetInstance();

        m_paddleTransform = world.AddComponent<Mona::TransformComponent>(paddle);
        m_paddleTransform->SetTranslation(m_transform->GetLocalTranslation());
        m_paddleTransform->SetScale(m_scale);

        auto paddleMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
        paddleMaterial->SetDiffuseColor(glm::vec3(0.2f, 0.2f, 0.2f));
        world.AddComponent<Mona::StaticMeshComponent>(paddle, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), paddleMaterial);

        auto paddleEntity = componentManager->CreateEntity();
        componentManager->AddComponent<MonaECS::TransformComponent>(paddleEntity, &m_paddleTransform);
        componentManager->AddComponent<MonaECS::BodyComponent>(paddleEntity, glm::vec3(0.0f), glm::vec3(0.0f), 10.0f);
        componentManager->AddComponent<MonaECS::ColliderComponent>(paddleEntity, m_scale, false);
        componentManager->AddComponent<MonaECS::Stats>(paddleEntity);
        componentManager->AddComponent<MonaECS::MoveInputComponent>(paddleEntity);
    }

private:
    const glm::vec3 &m_scale = glm::vec3(4.0f, 1.0f, 1.0f);
    Mona::TransformHandle m_transform;
    Mona::TransformHandle m_paddleTransform;
    MonaECS::ComponentManager *componentManager;
    MonaECS::EventManager *eventManager;
};

class Ball : public Mona::GameObject
{
public:
    Ball(MonaECS::ComponentManager *componentManager, MonaECS::EventManager *eventManager) : componentManager(componentManager), eventManager(eventManager) {}
    ~Ball() = default;
    virtual void UserStartUp(Mona::World &world) noexcept
    {
        m_transform = world.AddComponent<Mona::TransformComponent>(*this);
        auto &meshManager = Mona::MeshManager::GetInstance();
        auto ball = world.CreateGameObject<Mona::GameObject>();
        float ballRadius = 0.5f;

        m_ballTransform = world.AddComponent<Mona::TransformComponent>(ball);
        m_ballTransform->SetRotation(m_transform->GetLocalRotation());
        m_ballTransform->SetTranslation(m_transform->GetLocalTranslation() + glm::vec3(0.0f, 2.0f, 0.0f));
        m_ballTransform->SetScale(glm::vec3(ballRadius));

        auto ballMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
        ballMaterial->SetDiffuseColor(glm::vec3(0.75f, 0.3f, 0.3f));
        world.AddComponent<Mona::StaticMeshComponent>(ball, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Sphere), ballMaterial);

        auto ballEntity = componentManager->CreateEntity();
        componentManager->AddComponent<MonaECS::TransformComponent>(ballEntity, &m_ballTransform);
        componentManager->AddComponent<MonaECS::BodyComponent>(ballEntity, glm::vec3(2.0f, 10.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), 10.0f);
        componentManager->AddComponent<MonaECS::ColliderComponent>(ballEntity, glm::vec3(ballRadius), false);
        eventManager->Subscribe<MonaECS::CollisionEvent, Ball, &Ball::OnCollision>(*this);
    }
    void OnCollision(const MonaECS::CollisionEvent &event)
    {
        auto e1 = event.entity1;
        auto e2 = event.entity2;
        entt::registry &registry = componentManager->GetRegistry();

        auto &transform1 = componentManager->GetComponent<MonaECS::TransformComponent>(e1);
        auto &transform2 = componentManager->GetComponent<MonaECS::TransformComponent>(e2);

        glm::vec3 pos1 = (*transform1.tHandle)->GetLocalTranslation();
        glm::vec3 pos2 = (*transform2.tHandle)->GetLocalTranslation();

        auto &body1 = componentManager->GetComponent<MonaECS::BodyComponent>(e1);
        auto &body2 = componentManager->GetComponent<MonaECS::BodyComponent>(e2);

        if (registry.any_of<MonaECS::Stats>(e1))
        {
            auto &stats = componentManager->GetComponent<MonaECS::Stats>(e1);
            stats.health -= 40;
            glm::vec3 new_direction = glm::normalize(glm::vec3((pos2.x - pos1.x), -body2.velocity.y, 0.0f));
            float v_mod = std::sqrt(std::pow(body2.velocity.x, 2) + std::pow(body2.velocity.y, 2));
            float a_mod = std::sqrt(std::pow(body2.acceleration.x, 2) + std::pow(body2.acceleration.y, 2));
            body2.velocity = new_direction * v_mod;
            body2.acceleration = new_direction * a_mod;
            return;
        }
        else if (registry.any_of<MonaECS::Stats>(e2))
        {
            auto &stats = componentManager->GetComponent<MonaECS::Stats>(e2);
            stats.health -= 40;
            glm::vec3 new_direction = glm::normalize(glm::vec3((pos1.x - pos2.x), -body1.velocity.y, 0.0f));
            float v_mod = std::sqrt(std::pow(body1.velocity.x, 2) + std::pow(body1.velocity.y, 2));
            float a_mod = std::sqrt(std::pow(body1.acceleration.x, 2) + std::pow(body1.acceleration.y, 2));
            body1.velocity = new_direction * v_mod;
            body1.acceleration = new_direction * a_mod;
            return;
        }

        body1.acceleration = glm::reflect(body1.acceleration, GetNormals(pos1));
        body2.acceleration = glm::reflect(body2.acceleration, GetNormals(pos2));
        body1.velocity = glm::reflect(body1.velocity, GetNormals(pos1));
        body2.velocity = glm::reflect(body2.velocity, GetNormals(pos2));
    }

private:
    glm::vec3 GetNormals(glm::vec3 pos)
    {
        if (pos.x < -17.5f)
        {
            return glm::vec3(-1.0f, 0.0f, 0.0f);
        }
        else if (pos.x > 17.5f)
        {
            return glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else if (pos.y > 25.0f)
        {
            return glm::vec3(0.0f, -1.0f, 0.0f);
        }
        else if (pos.y < -4.0f)
        {
            return glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else
        {
            return glm::vec3(0.0f);
        }
    }
    Mona::TransformHandle m_transform;
    Mona::TransformHandle m_ballTransform;
    Mona::RigidBodyHandle m_ballRigidBody;
    std::shared_ptr<Mona::AudioClip> m_ballBounceSound;
    MonaECS::ComponentManager *componentManager;
    MonaECS::EventManager *eventManager;
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
        world.CreateGameObject<Ball>(&componentManager, &eventManager);

        world.CreateGameObject<Wall>(glm::vec3(0.0f, 26.0f, 0.0f), glm::vec3(18.0f, 1.0f, 1.0f), &componentManager, &eventManager);
        glm::vec3 sideWallScale(1.0f, 27.0f, 1.0f);
        float sideWallOffset = 19.0f;
        world.CreateGameObject<Wall>(glm::vec3(-sideWallOffset, 0.0f, 0.0f), sideWallScale, &componentManager, &eventManager);
        world.CreateGameObject<Wall>(glm::vec3(sideWallOffset, 0.0f, 0.0f), sideWallScale, &componentManager, &eventManager);

        world.CreateGameObject<Block>(glm::vec3(2.0f, 15.0f, 0.0f), glm::vec3(3.0f, 1.0f, 1.0f), &componentManager, &eventManager);
        world.CreateGameObject<Paddle>(&componentManager, &eventManager);

        systemManager.RegisterSystem<MonaECS::StatsSystem>();
        systemManager.RegisterSystem<MonaECS::CollisionSystem>();
        systemManager.RegisterSystem<MonaECS::MovementSystem>();
        systemManager.RegisterSystem<MonaECS::InputSystem>();
        systemManager.StartUpSystems(componentManager, eventManager);
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