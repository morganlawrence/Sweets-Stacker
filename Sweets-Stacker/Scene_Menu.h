#pragma once

#include "Scene.h"
#include "GameEngine.h"


class Scene_Menu : public Scene
{
private:
	std::vector<std::string>	m_menuStrings;
	sf::Text					m_menuText;
	sf::Text					m_menuTitle;
	sf::Text					m_menuFooter;
	std::vector<std::string>	m_levelPaths;
	int							m_menuIndex{ 0 };
	std::string					m_title;
	sf::View					m_worldView;
	sf::Vector2f				m_menuTextPosition;


	void						registerActions();
	void						init();
	void						loadLevel(const std::string& path);
	void						onEnd() override;
public:

	Scene_Menu(GameEngine* gameEngine);

	void						update(sf::Time dt) override;
	void						sRender() override;
	void						sDoAction(const Command& action) override;


};

