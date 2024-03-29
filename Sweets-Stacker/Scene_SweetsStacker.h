#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"
#include "GameEngine.h"

class Scene_SweetsStacker : public Scene
{

    struct PlayerConfig
    {
        float X{ 0.f }, Y{ 0.f }, CW{ 0.f }, CH{ 0.f };
        float SPEED{ 0.f }, MAXSPEED{ 0.f }, JUMP{ 12 }, GRAVITY{ 0.f };
    };

    struct PlayerNoseConfig {
        float X{ 0.f }, Y{ 0.f }, CW{ 0.f }, CH{ 0.f };
        float SPEED{ 0.f }, MAXSPEED{ 0.f }, JUMP{ 0.f }, GRAVITY{ 0.f };
    };
private:
    PlayerConfig	m_playerConfig;
    PlayerNoseConfig	m_playerNoseConfig;

    std::shared_ptr<Entity>		m_player;
    std::shared_ptr<Entity>		m_playerNose;

    sf::View        m_worldView;
    sf::FloatRect   m_worldBounds;

    bool			m_drawTextures{ true };
    bool			m_drawAABB{ false };
    bool			m_drawGrid{ false };

    //systems
    void            sMovement(sf::Time dt);
    void            sCollisions();
    void            sUpdate(sf::Time dt);
    void            sAnimation(sf::Time dt);
    void            sRespawnEntities();

    void	        onEnd() override;

    // helper functions
    void            playerMovement();
    void            adjustPlayerPosition();
    void            checkGround();
    void            checkPlayerState();
    void	        registerActions();

    void            spawnPlayer();
    void            spawnScoops(sf::Time dt);

    void            init(const std::string& path);
    void            loadLevel(const std::string& path);
    sf::FloatRect   getViewBounds();
    sf::FloatRect   getOutOfBounds() const;
    // sf::View   getOutOfBounds() const;


public:

    Scene_SweetsStacker(GameEngine* gameEngine, const std::string& levelPath);

    void		  update(sf::Time dt) override;
    void		  sDoAction(const Command& command) override;
    void		  sRender() override;

};
