#include "Scene_Credits.h"
#include "GameEngine.h"
#include <memory>
#include "Physics.h"
#include "MusicPlayer.h"

Scene_Credits::Scene_Credits(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_worldView(gameEngine->window().getView())
{

	loadLevel(levelPath);
	registerActions();

	init();
}

void Scene_Credits::registerActions() {
	registerAction(sf::Keyboard::Escape, "QUIT");
}

void Scene_Credits::init() {

}

void Scene_Credits::sRender()
{
	m_game->window().setView(m_worldView);

	for (auto e : m_entityManager.getEntities("bkgCredits")) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->window().draw(sprite);
		}
	}


}

void Scene_Credits::update(sf::Time dt) {
	m_entityManager.update();
}


void Scene_Credits::onEnd() {
	m_game->quitLevel();

}

void Scene_Credits::sDoAction(const Command& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Credits::loadLevel(const std::string& path) {
	std::ifstream config(path);
	if (config.fail()) {
		std::cerr << "Open file " << path << " failed\n";
		config.close();
		exit(1);
	}

	std::string token{ "" };
	config >> token;
	while (!config.eof()) {
		if (token == "BkgCredits") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("bkgCredits");

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