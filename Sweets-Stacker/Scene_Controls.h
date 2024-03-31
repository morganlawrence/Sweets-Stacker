#pragma once

#include "Scene.h"
#include "GameEngine.h"

class Scene_Controls : public Scene
{
private:
	sf::View					m_worldView;
	sf::Text					m_menuOptions;
	sf::Text					m_optionsFooter;

	void						registerActions();
	void						init();
	void						loadLevel(const std::string& path);
	void						onEnd() override;
public:

	Scene_Controls(GameEngine* gameEngine, const std::string& levelPath);

	void						update(sf::Time dt) override;
	void						sRender() override;
	void						sDoAction(const Command& action) override;

};

