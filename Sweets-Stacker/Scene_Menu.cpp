#include "Scene_Menu.h"
#include "Scene_SweetsStacker.h"
#include "Scene_Controls.h"
#include "Scene_Credits.h"

#include <memory>
#include "Physics.h"
#include "MusicPlayer.h"


void Scene_Menu::onEnd()
{
	m_game->window().close();
}

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	:Scene(gameEngine)
	, m_worldView(gameEngine->window().getDefaultView())
{
	loadLevel("../assetsNew/level0.txt");
	registerActions();
	init();

	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(20);
}

void Scene_Menu::registerActions() {
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::Enter, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");

}

void Scene_Menu::init()
{

	// Options Paths
	m_menuStrings.push_back("Start Game");
	m_levelPaths.push_back("../assetsNew/level1.txt");

	m_menuStrings.push_back("Controls");
	m_levelPaths.push_back("../assetsNew/level2.txt");

	m_menuStrings.push_back("Credits");
	m_levelPaths.push_back("../assetsNew/level3.txt");

	m_menuStrings.push_back("Exit");

	// Title Text
	m_menuTitle.setFont(Assets::getInstance().getFont("Menu"));
	m_menuTitle.setCharacterSize(100);
	m_menuTitle.setFillColor(sf::Color::White);
	m_menuTitle.setString("Sweets Stacker");

	centerOrigin(m_menuTitle);
	m_menuTitle.setPosition(m_game->window().getSize().x / 2, 333);

	// Options Text
	float textWidth = m_menuText.getLocalBounds().width;
	float textHeight = m_menuText.getLocalBounds().height;
	m_menuTextPosition.x = m_game->window().getSize().x  / 2.f;
	m_menuTextPosition.y = (m_game->window().getSize().y - m_menuStrings.size() * textHeight) / 2.f - 120;
	
	m_menuText.setFont(Assets::getInstance().getFont("Menu"));
	m_menuText.setCharacterSize(60);
	
	centerOrigin(m_menuText);
	m_menuText.setPosition(m_menuTextPosition);

	// Footer Text
	m_menuFooter.setFont(Assets::getInstance().getFont("main"));
	m_menuFooter.setCharacterSize(20);
	m_menuFooter.setFillColor(sf::Color::White);
	m_menuFooter.setString("UP: W    DOWN: S   PLAY:D/ENTER    QUIT: ESC");
	
	centerOrigin(m_menuFooter);
	m_menuFooter.setPosition(m_game->window().getSize().x / 2.f, 850);
}

void Scene_Menu::update(sf::Time dt)
{
	m_entityManager.update();
}


void Scene_Menu::sRender()
{
	m_game->window().setView(m_worldView);

	for (auto e : m_entityManager.getEntities("bkgMenu")) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->window().draw(sprite);
		}
	}

	m_game->window().draw(m_menuTitle);
	
	// set m_menuText fill colour
	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);

	for (size_t i = 0; i < m_menuStrings.size(); ++i)
	{
		m_menuText.setFillColor((i == m_menuIndex ? selectedColor : normalColor));
		m_menuText.setString(m_menuStrings.at(i));

		centerOrigin(m_menuText);
		m_menuText.setPosition(m_menuTextPosition.x, m_menuTextPosition.y + i * m_menuText.getCharacterSize() * 1.75);
		
		m_game->window().draw(m_menuText);
	}

	m_game->window().draw(m_menuFooter);

}


void Scene_Menu::sDoAction(const Command& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			m_menuIndex = (m_menuIndex + m_menuStrings.size() - 1) % m_menuStrings.size();
		}
		else if (action.name() == "DOWN")
		{
			m_menuIndex = (m_menuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "PLAY")
		{
			if (m_menuIndex == 0) {
				m_game->changeScene("PLAY", std::make_shared<Scene_SweetsStacker>(m_game, m_levelPaths[m_menuIndex]));
			}
			else if (m_menuIndex == 1) {
				m_game->changeScene("PLAY", std::make_shared<Scene_Controls>(m_game, m_levelPaths[m_menuIndex]));
			}
			else if (m_menuIndex == 2) {
				m_game->changeScene("PLAY", std::make_shared<Scene_Credits>(m_game, m_levelPaths[m_menuIndex]));
			}
			else if (m_menuIndex == 3) {
				onEnd();
			}
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}

}

void Scene_Menu::loadLevel(const std::string& path) {
	std::ifstream config(path);
	if (config.fail()) {
		std::cerr << "Open file " << path << " failed\n";
		config.close();
		exit(1);
	}

	std::string token{ "" };
	config >> token;
	while (!config.eof()) {
		if (token == "BkgMenu") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("bkgMenu");

			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);

		}
		else if (token[0] == '#') {
			std::cout << token;
		}

		config >> token;
	}

	config.close();
}
