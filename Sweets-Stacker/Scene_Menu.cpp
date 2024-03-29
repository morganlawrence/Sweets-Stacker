#include "Scene_Menu.h"
#include "Scene_SweetsStacker.h"
#include <memory>
#include "Physics.h"

void Scene_Menu::onEnd()
{
	m_game->window().close();
}

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	:Scene(gameEngine)
{
	loadLevel("../assetsNew/level1.txt");
	//registerActions();
	init();
}

void Scene_Menu::registerActions() {

}

void Scene_Menu::init()
{
	/*m_title = "Frogger";
	m_menuStrings.push_back("Play Game");

	m_levelPaths.push_back("../assets/level1.txt");

	m_menuText.setFont(Assets::getInstance().getFont("main"));

	const size_t CHAR_SIZE{ 64 };
	m_menuText.setCharacterSize(CHAR_SIZE);*/

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_menuStrings.push_back("Start Game");
	m_levelPaths.push_back("../assets/level1.txt");

	m_menuStrings.push_back("Settings");
	m_levelPaths.push_back("../assets/level1.txt");

	m_menuStrings.push_back("Credits");
	m_levelPaths.push_back("../assets/level1.txt");


	m_menuTitle.setFont(Assets::getInstance().getFont("Menu"));
	centerOrigin(m_menuTitle);
	m_menuTitle.setPosition(/*m_game->window().getSize().x */ 300 / 2.f, 250);

	m_menuTitle.setCharacterSize(80);
	m_menuTitle.setFillColor(sf::Color::White);


	m_menuText.setFont(Assets::getInstance().getFont("Menu"));

	const size_t CHAR_SIZE{ 64 };
	m_menuText.setCharacterSize(CHAR_SIZE);

}

void Scene_Menu::update(sf::Time dt)
{
	m_entityManager.update();
}


void Scene_Menu::sRender()
{
	sf::View view = m_game->window().getView();
	view.setCenter(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f);
	m_game->window().setView(view);

	for (auto e : m_entityManager.getEntities("bkgMenu")) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->window().draw(sprite);
		}
	}

	m_menuTitle.setString("Sweets Stacker\n");
	m_game->window().draw(m_menuTitle);


	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);

	//static const sf::Color backgroundColor(100, 100, 255);

	sf::Text footer("UP: W    DOWN: S   PLAY:D    QUIT: ESC",
		Assets::getInstance().getFont("main"), 20);
	footer.setFillColor(normalColor);
	footer.setPosition(32, 700);

	//m_game->window().clear(backgroundColor);

	//m_menuText.setFillColor(normalColor);
	//m_menuText.setString(m_title);
	//m_menuText.setPosition(10, 10);
	//m_game->window().draw(m_menuText);

	// total height of menu strings
	float totalMenuHeight = m_menuStrings.size() * m_menuText.getCharacterSize();

	// starting position to center vertically
	float startY = (m_game->window().getSize().y - totalMenuHeight) / 3.f;

	for (size_t i{ 0 }; i < m_menuStrings.size(); ++i)
	{
		m_menuText.setFillColor((i == m_menuIndex ? selectedColor : normalColor));
		//m_menuText.setPosition(275, 150 + (i+1) * 150);
		m_menuText.setString(m_menuStrings.at(i));
		m_menuText.setPosition((m_game->window().getSize().x - m_menuText.getLocalBounds().width) / 2.f,
			startY + i * m_menuText.getCharacterSize() + (i + 1) * 40);

		m_game->window().draw(m_menuText);
	}

	m_game->window().draw(footer);
	//m_game->window().display();

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
			m_game->changeScene("PLAY", std::make_shared<Scene_SweetsStacker>(m_game, m_levelPaths[m_menuIndex]));
			//m_game->changeScene("PLAY", std::make_shared<Scene_Frogger>(m_game));
			//loadLevel(".. / assets / level1.txt");
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
