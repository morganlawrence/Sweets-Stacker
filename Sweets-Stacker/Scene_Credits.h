#pragma once

#include "Scene.h"
#include "GameEngine.h"
class Scene_Credits : public Scene
{
private:
	sf::View					m_worldView;
	sf::Text					m_menuCredits;
	sf::Text					m_creditsFooter;

	void						registerActions();
	void						init();
	void						loadLevel(const std::string& path);
	void						onEnd() override;
public:

	Scene_Credits(GameEngine* gameEngine, const std::string& levelPath);

	void						update(sf::Time dt) override;
	void						sRender() override;
	void						sDoAction(const Command& action) override;
};

