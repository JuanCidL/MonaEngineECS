#include "MonaEngine.hpp"
#include "ECS/ECS.hpp"
#include "ECS/Systems/StatsSystem.hpp"
#include "ECS/Systems/CollisionSystem.hpp"
#include "ECS/Systems/MovementSystem.hpp"
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

class Block : public Mona::GameObject{
public:
    Block(const glm::vec3 &position, const glm::vec3 &scale, MonaECS::ComponentManager* component, MonaECS::EventManager *event) : m_position(position), m_scale(scale), componentManager(component), eventManager(event){};
    ~Block() = default;
    virtual void UserStartUp(Mona::World &world) noexcept{
        m_transform = world.AddComponent<Mona::TransformComponent>(*this);  
        auto wall = world.CreateGameObject<Mona::GameObject>();
        auto &meshManager = Mona::MeshManager::GetInstance();
        
        m_blockTransform = world.AddComponent<Mona::TransformComponent>(wall);
        m_blockTransform->SetTranslation(m_position);
        m_blockTransform->SetScale(m_scale);

        auto blockMaterial = std::static_pointer_cast<Mona::DiffuseFlatMaterial>(world.CreateMaterial(Mona::MaterialType::DiffuseFlat));
        MonaECS::Stats init;
        blockMaterial->SetDiffuseColor(init.GetColor());
        world.AddComponent<Mona::StaticMeshComponent>(wall, meshManager.LoadMesh(Mona::Mesh::PrimitiveType::Cube), blockMaterial);

        auto wallEntity = componentManager->CreateEntity();
        componentManager->AddComponent<MonaECS::TransformComponent>(wallEntity, &m_blockTransform);
        componentManager->AddComponent<MonaECS::BodyComponent>(wallEntity, glm::vec3(0.0f), glm::vec3(0.0f), 10.0f);
        componentManager->AddComponent<MonaECS::ColliderComponent>(wallEntity, m_scale, false);
        componentManager->AddComponent<MonaECS::Stats>(wallEntity, glm::vec3(0.0f));
        eventManager->Subscribe<MonaECS::CollisionEvent, Block, &Block::OnCollision>(*this);
    }
    
    void OnCollision(const MonaECS::CollisionEvent &event){
        /* auto e1 = event.entity1;
        auto e2 = event.entity2;

        try
        {
            auto &stat = componentManager->GetComponent<MonaECS::Stats>(e1);
            stat.NextState();
        }
        catch(const std::exception& e)
        {
            auto &stat = componentManager->GetComponent<MonaECS::Stats>(e2);
            stat.NextState();
        } */
        


    }
private:
    const glm::vec3& m_position;
    const glm::vec3& m_scale;
    Mona::TransformHandle m_transform;
    Mona::TransformHandle m_blockTransform;
    MonaECS::ComponentManager * componentManager;
    MonaECS::EventManager * eventManager;

};

class Wall : public Mona::GameObject{
public:
    Wall(const glm::vec3 &position, const glm::vec3 &scale, MonaECS::ComponentManager* component, MonaECS::EventManager *event) : m_position(position), m_scale(scale), componentManager(component), eventManager(event){};
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
        componentManager->AddComponent<MonaECS::BodyComponent>(wallEntity, glm::vec3(0.0f), glm::vec3(0.0f), 10.0f);
        componentManager->AddComponent<MonaECS::ColliderComponent>(wallEntity, m_scale, false);
        
    }

    
private:
    const glm::vec3& m_position;
    const glm::vec3& m_scale;
    Mona::TransformHandle m_transform;
    Mona::TransformHandle m_wallTransform;
    MonaECS::ComponentManager * componentManager;
    MonaECS::EventManager * eventManager;

};

class Ball : public Mona::GameObject
{
public:
    Ball(MonaECS::ComponentManager *componentManager, MonaECS::EventManager *eventManager) :  componentManager(componentManager), eventManager(eventManager) {}
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
        componentManager->AddComponent<MonaECS::BodyComponent>(ballEntity, glm::vec3(15.0f, 10.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), 10.0f); // Fue necesario ponerle 1 en x e y
        componentManager->AddComponent<MonaECS::ColliderComponent>(ballEntity, glm::vec3(ballRadius), false);
        eventManager->Subscribe<MonaECS::CollisionEvent, Ball, &Ball::OnCollision>(*this);
    }
    void OnCollision(const MonaECS::CollisionEvent &event)
    {
        auto e1 = event.entity1;
        auto e2 = event.entity2;

        auto &transform1 = componentManager->GetComponent<MonaECS::TransformComponent>(e1);
        auto &transform2 = componentManager->GetComponent<MonaECS::TransformComponent>(e2);

        glm::vec3 pos1 = (*transform1.tHandle)->GetLocalTranslation(); 
        glm::vec3 pos2 = (*transform2.tHandle)->GetLocalTranslation(); 

        auto &body1 = componentManager->GetComponent<MonaECS::BodyComponent>(e1);
        auto &body2 = componentManager->GetComponent<MonaECS::BodyComponent>(e2);

    
        body1.acceleration = glm::reflect(body1.acceleration, GetNormals(pos1));
        body2.acceleration = glm::reflect(body2.acceleration, GetNormals(pos2));
        body1.velocity = glm::reflect(body1.velocity, GetNormals(pos1));
        body2.velocity = glm::reflect(body2.velocity, GetNormals(pos2));
    }

private:
    glm::vec3 GetNormals(glm::vec3 pos){
        if(pos.x < -17.5f){
            return glm::vec3(-1.0f, 0.0f, 0.0f);
        }
        else if(pos.x > 17.5f){
            return glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else if(pos.y > 25.0f){
            return glm::vec3(0.0f, -1.0f, 0.0f);
        }
        else if(pos.y < -4.0f){
            return glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else{
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
        
        //world.CreateGameObject<Block>(glm::vec3(2.0f, 15.0f, 0.0f), glm::vec3(5.0f, 1.0f, 1.0f), &componentManager, &eventManager);

        systemManager.RegisterSystem<MonaECS::StatsSystem>();
        systemManager.RegisterSystem<MonaECS::MovementSystem>();
        systemManager.RegisterSystem<MonaECS::CollisionSystem>();
		//systemManager.RegisterSystem<MonaECS::InputSystem>();
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