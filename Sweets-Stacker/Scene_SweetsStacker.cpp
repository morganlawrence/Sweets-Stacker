#include "Scene_SweetsStacker.h"
#include <fstream>
#include <iostream>

#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include <random>

namespace {
	std::random_device rd;
	std::mt19937 rng(rd());
}


Scene_SweetsStacker::Scene_SweetsStacker(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_worldView(gameEngine->window().getDefaultView())
{

	loadLevel(levelPath);
	checkGround();
	spawnPlayer();
	init(levelPath);
	registerActions();

}

void Scene_SweetsStacker::init(const std::string& path) {


}

void Scene_SweetsStacker::sMovement(sf::Time dt) {
	playerMovement();

	//	 move all objects
	for (auto e : m_entityManager.getEntities("iceCream")) {
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			tfm.pos += tfm.vel * dt.asSeconds();
			tfm.angle += tfm.angVel * dt.asSeconds();
		}
	}

	for (auto& e : m_entityManager.getEntities("player")) {
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			tfm.pos += tfm.vel * dt.asSeconds();

		}
	}

	for (auto& e : m_entityManager.getEntities("playerNose")) {
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			auto& playerTfm = m_player->getComponent<CTransform>();
			tfm.pos = playerTfm.pos + sf::Vector2f(78, -45.f);

			if (tfm.pos.y > 967.f)
				tfm.pos.y = 967.f;

			if (m_player->getComponent<CInput>().dir == 'L')
				tfm.pos.x -= 157.f;

			//std::cout << m_player->getComponent<CInput>().dir;
		}
	}
}

void Scene_SweetsStacker::registerActions() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::Q, "QUIT");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");

	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");

	registerAction(sf::Keyboard::W, "JUMP");
	registerAction(sf::Keyboard::Up, "JUMP");
}

void Scene_SweetsStacker::onEnd() {
	m_game->changeScene("MENU", nullptr, false);
}

void Scene_SweetsStacker::playerMovement() {

	// player movement
	auto& dir = m_player->getComponent<CInput>().dir;
	auto& pt = m_player->getComponent<CTransform>();
	auto& ptn = m_playerNose->getComponent<CTransform>();

	pt.vel.x = 0.f;

	if (m_player->getComponent<CInput>().LEFT) {
		pt.vel.x -= 1;
	}

	if (m_player->getComponent<CInput>().RIGHT) {

		pt.vel.x += 1;

	}

	if (m_player->getComponent<CInput>().UP) {
		m_player->getComponent<CInput>().UP = false;

		pt.vel.y = -m_playerConfig.JUMP;
	}

	// gravity
	pt.vel.y += 0.5;
	pt.vel.x = pt.vel.x * 10;

	for (auto e : m_entityManager.getEntities("player")) {
		auto& tx = e->getComponent<CTransform>();
		tx.prevPos = tx.pos;
		tx.pos += tx.vel;
	}

	ptn.pos = pt.pos + sf::Vector2f(80, -45.f);

}
//void Scene_SweetsStacker::checkPlayerState() {
//	auto& tx = m_player->getComponent<CTransform>();
//	auto& state = m_player->getComponent<CState>();
//
//	if (std::abs(tx.vel.x) > 0.1f)
//		tx.scale.x = (tx.vel.x > 0) ? 1 : -1;
//}

void Scene_SweetsStacker::sAnimation(sf::Time dt) {
	auto list = m_entityManager.getEntities();

	for (auto e : m_entityManager.getEntities()) {
		// update all animations
		if (e->hasComponent<CAnimation>()) {
			auto& anim = e->getComponent<CAnimation>();
			anim.animation.update(dt);

			auto& tfm = e->getComponent<CTransform>();
			anim.animation.getSprite().setPosition(tfm.pos);
			anim.animation.getSprite().setRotation(tfm.angle);
		}
	}
}

void Scene_SweetsStacker::sRender() {

	m_game->window().setView(m_worldView);

	// draw bkg first
	for (auto e : m_entityManager.getEntities("bkgtest")) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			m_game->window().draw(sprite);
		}
	}

	for (auto& e : m_entityManager.getEntities()) {
		if (e->hasComponent<CAnimation>()) {
			// draw animations
			if (e->hasComponent<CAnimation>()) {
				auto& anim = e->getComponent<CAnimation>().animation;
				auto& tfm = e->getComponent<CTransform>();
				anim.getSprite().setPosition(tfm.pos);
				anim.getSprite().setRotation(tfm.angle);
				m_game->window().draw(anim.getSprite());
			}

		}
		else {
			// draw sprite
			auto& sprite = e->getComponent<CSprite>().sprite;
			auto& tfm = e->getComponent<CTransform>();

			sprite.setPosition(tfm.pos);
			sprite.setRotation(tfm.angle);

			m_game->window().draw(sprite);
		}
		// draw bounding box
		if (m_drawAABB) {
			if (e->hasComponent<CBoundingBox>()) {
				auto box = e->getComponent<CBoundingBox>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f{ box.size.x, box.size.y });
				centerOrigin(rect);
				rect.setPosition(e->getComponent<CTransform>().pos);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color{ 0, 255, 0 });
				rect.setOutlineThickness(2.f);
				m_game->window().draw(rect);
			}
		}
	}
}

void Scene_SweetsStacker::update(sf::Time dt) {

	sUpdate(dt);
}

void Scene_SweetsStacker::sDoAction(const Command& action) {
	// On Key Press
	auto pos = m_player->getComponent<CTransform>().pos;

	if (action.type() == "START") {
		if (action.name() == "PAUSE") { setPaused(!m_isPaused); }
		else if (action.name() == "QUIT") { m_game->quitLevel(); }
		else if (action.name() == "BACK") { m_game->backLevel(); }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawAABB = !m_drawAABB; }

		else if (action.name() == "LEFT") {
			m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("ScoopyWalkLeft"));

			m_player->getComponent<CInput>().LEFT = true;
			m_player->getComponent<CInput>().dir = 'L';
		}
		else if (action.name() == "RIGHT") {
			m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("ScoopyWalkRight"));

			m_player->getComponent<CInput>().RIGHT = true;
			m_player->getComponent<CInput>().dir = 'R';
		}

		else if (action.name() == "JUMP") {
			if (m_player->getComponent<CInput>().JUMP) {
				m_player->getComponent<CInput>().UP = true;
				m_player->getComponent<CInput>().JUMP = false;
			}
		}
	}

	// on Key Release
	else if (action.type() == "END") {
		if (action.name() == "LEFT") {
			m_player->removeComponent<CAnimation>();
			m_player->getComponent<CInput>().LEFT = false;
			m_player->addComponent<CSprite>(Assets::getInstance().getTexture("DogL")).sprite;
		}
		else if (action.name() == "RIGHT") {
			m_player->removeComponent<CAnimation>();
			m_player->getComponent<CInput>().RIGHT = false;
			m_player->addComponent<CSprite>(Assets::getInstance().getTexture("DogR")).sprite;
		}
		else if (action.name() == "JUMP") {
			m_player->getComponent<CInput>().UP = false;
			std::cout << "player up is false" << std::endl;
		}
	}

}

void Scene_SweetsStacker::checkGround() {
	auto ground = m_entityManager.addEntity("ground");
	auto& sprite = ground->addComponent<CSprite>().sprite;

	sf::Vector2f pos{ 400.f,1138.f }; //400,1138
	auto& tfm = ground->addComponent<CTransform>(pos);
	ground->addComponent<CBoundingBox>(sf::Vector2f(800.f, 130.f)); // 800,129 // 136
}

void Scene_SweetsStacker::spawnPlayer() {
	auto pos = m_worldView.getSize();
	pos.x = pos.x / 2.f;
	pos.y -= 290.f;

	auto nosePosRight = m_worldView.getSize();
	nosePosRight.x = pos.x + 199.f / 2 - (32.f / 2 + 6.f);
	nosePosRight.y = pos.y - 60.f;

	m_player = m_entityManager.addEntity("player");
	m_playerNose = m_entityManager.addEntity("playerNose");

	m_player->addComponent<CTransform>(pos); // VEL?
	m_playerNose->addComponent<CTransform>(pos);

	m_player->addComponent<CInput>();
	m_player->getComponent<CInput>().dir = 'R';

	//m_playerNose->addComponent<CSprite>().sprite;

	//m_player->addComponent<CState>();
	//m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("ScoopyWalkRight"));
	//auto animation = Assets::getInstance().getAnimation("ScoopyWalkRight");
	//m_player->addComponent<CAnimation>(animation);
	//m_player->getComponent<CTransform>().vel = sf::Vector2f(0.f, 0.f); //??
	//m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("ScoopyWalkRight"));

	auto& sprite = m_player->addComponent<CSprite>().sprite;
	auto& spriteTex = m_player->addComponent<CSprite>(Assets::getInstance().getTexture("DogR")).sprite;
	sprite.setTexture(Assets::getInstance().getTexture("DogR"));

	auto size = sprite.getLocalBounds().getSize();

	m_player->addComponent<CBoundingBox>(sf::Vector2f(190.f, 125.f));
	m_playerNose->addComponent<CBoundingBox>(sf::Vector2f(32.f, 38.f));

	centerOrigin(sprite);
	std::cout << m_player->getComponent<CTransform>().pos.y;
}

void Scene_SweetsStacker::spawnScoops(sf::Time dt) {

	static sf::Time spawnTimer = sf::Time::Zero;

	static const sf::Time spawnInterval = sf::seconds(3.0f);

	spawnTimer += dt;
	sf::FloatRect spawningBounds = getOutOfBounds();

	while (spawnTimer >= spawnInterval) {
		// positions
		std::uniform_real_distribution<float> distX(spawningBounds.left, spawningBounds.left + spawningBounds.width);
		// range
		std::uniform_real_distribution<float> distY(spawningBounds.top, spawningBounds.top + spawningBounds.height);
		// random positions
		sf::Vector2f spawnPos{ distX(rng), distY(rng) };
		// random sprite
		std::uniform_int_distribution<int> distDrop(0, 10);

		int scoopFlavour = distDrop(rng);
		std::string spriteName;

		if (scoopFlavour < 5) {
			spriteName = "StrawberryDrop";
			std::cout << "Stawberry Spawned\n";
		}
		else if (scoopFlavour >= 5) {
			spriteName = "ChocolateDrop";
			std::cout << "Chocolate Spawned\n";
		}

		auto fallingScoops = m_entityManager.addEntity("iceCream");

		auto& sprite = fallingScoops->addComponent<CSprite>().sprite;
		auto [txtName, txtRect] = Assets::getInstance().getSprt(spriteName);
		fallingScoops->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);
		fallingScoops->addComponent<CBoundingBox>(sf::Vector2f(40.f, 57.f));

		auto size = sprite.getLocalBounds().getSize();
		centerOrigin(sprite);

		// position of spawned scoop
		fallingScoops->addComponent<CTransform>(spawnPos);
		fallingScoops->getComponent<CTransform>().vel = sf::Vector2f(0.f, 150.f);

		spawnTimer -= spawnInterval;
	}
}

sf::FloatRect Scene_SweetsStacker::getOutOfBounds() const
{
	auto center = m_worldView.getCenter();
	auto size = m_worldView.getSize();
	auto leftTop = center - size / 2.f;

	size.y -= size.y;

	return sf::FloatRect(leftTop, size);
}

sf::FloatRect Scene_SweetsStacker::getViewBounds() {
	auto view = m_game->window().getView();
	return sf::FloatRect(
		(view.getCenter().x - view.getSize().x / 2.f), (view.getCenter().y - view.getSize().y / 2.f),
		view.getSize().x, view.getSize().y);
}

void Scene_SweetsStacker::adjustPlayerPosition() {
	auto center = m_worldView.getCenter();
	sf::Vector2f viewHalfSize = m_worldView.getSize() / 2.f;


	auto left = center.x - viewHalfSize.x;
	auto right = center.x + viewHalfSize.x;
	auto top = center.y - viewHalfSize.y;
	auto bot = center.y + viewHalfSize.y;

	// nose bb
	auto& nose_pos = m_playerNose->getComponent<CTransform>().pos;
	auto noseHalfSize = m_playerNose->getComponent<CBoundingBox>().halfSize;

	nose_pos.x = std::max(nose_pos.x, left + noseHalfSize.x);
	nose_pos.x = std::min(nose_pos.x, right - noseHalfSize.x);
	nose_pos.y = std::max(nose_pos.y, top + noseHalfSize.y);
	nose_pos.y = std::min(nose_pos.y, bot - noseHalfSize.y);

	//player bb
	auto& player_pos = m_player->getComponent<CTransform>().pos;
	auto halfSize = m_player->getComponent<CBoundingBox>().halfSize;

	player_pos.x = std::max(player_pos.x, left + halfSize.x);
	player_pos.x = std::min(player_pos.x, right - halfSize.x);
	player_pos.y = std::max(player_pos.y, top + halfSize.y);
	player_pos.y = std::min(player_pos.y, bot - halfSize.y);


}

void Scene_SweetsStacker::sCollisions() {
	//adjustPlayerPosition();

	auto& iceCream = m_entityManager.getEntities("iceCream");
	auto& ground = m_entityManager.getEntities("ground");
	auto& player = m_entityManager.getEntities("player");
	auto& playerNose = m_entityManager.getEntities("playerNose");

	for (auto p : player) {
		for (auto g : ground) {
			auto overlap = Physics::getOverlap(p, g);
			if (overlap.x > 0 and overlap.y > 0) {
				p->getComponent<CTransform>().pos.y -= overlap.y;

				p->getComponent<CInput>().JUMP = true;
			}
		}
	}

	for (auto e : ground) {
		for (auto i : iceCream) {
			auto overlap = Physics::getOverlap(e, i);
			if (overlap.x > 0 and overlap.y > 0) {

				i->destroy();
			}
		}
	}

}

void Scene_SweetsStacker::sUpdate(sf::Time dt) {
	SoundPlayer::getInstance().removeStoppedSounds();

	if (m_isPaused)
		return;

	m_entityManager.update();
	sAnimation(dt);
	sMovement(dt);
	adjustPlayerPosition();
	sCollisions();
	//sRespawnEntities();
	spawnScoops(dt);
}

//void Scene_SweetsStacker::sRespawnEntities() {
//	auto worldViewBounds = getViewBounds();
//
//	for (auto e : m_entityManager.getEntities()) { //icecream?
//		if (e->hasComponent<CTransform>() && e->hasComponent<CBoundingBox>()) {
//			auto& transform = e->getComponent<CTransform>();
//			auto& boundingBox = e->getComponent<CBoundingBox>();
//
//			sf::FloatRect entityBounds(
//				transform.pos.x - boundingBox.size.x / 2.0f,
//				transform.pos.y - boundingBox.size.y / 2.0f,
//				boundingBox.size.x,
//				boundingBox.size.y
//			);
//
//			if (!worldViewBounds.intersects(entityBounds)) {
//				if (transform.pos.x < worldViewBounds.left) {
//					transform.pos.x = worldViewBounds.left + worldViewBounds.width + boundingBox.size.x / 2.0f;
//				}
//				else if (transform.pos.x > worldViewBounds.left + worldViewBounds.width) {
//					transform.pos.x = worldViewBounds.left - boundingBox.size.x / 2.0f;
//				}
//
//				if (transform.pos.y < worldViewBounds.top) {
//					transform.pos.y = worldViewBounds.top + worldViewBounds.height + boundingBox.size.y / 2.0f;
//				}
//				else if (transform.pos.y > worldViewBounds.top + worldViewBounds.height) {
//					transform.pos.y = worldViewBounds.top - boundingBox.size.y / 2.0f;
//				}
//			}
//		}
//	}
//
//}


void Scene_SweetsStacker::loadLevel(const std::string& path) {
	std::ifstream config(path);
	if (config.fail()) {
		std::cerr << "Open file " << path << " failed\n";
		config.close();
		exit(1);
	}

	std::string token{ "" };
	config >> token;
	while (!config.eof()) {
		if (token == "Bkg") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("bkgtest");

			// for background, no textureRect its just the whole texture
			// and no center origin, position by top left corner
			// stationary so no CTransfrom required.
			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token == "Ground") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("groundTest");
		}
		//else if (token == "Player") {
		//	config >>
		//		m_playerConfig.X >>
		//		m_playerConfig.Y >>
		//		m_playerConfig.CW >>
		//		m_playerConfig.CH >>
		//		m_playerConfig.SPEED >>
		//		m_playerConfig.JUMP >>
		//		m_playerConfig.MAXSPEED >>
		//		m_playerConfig.GRAVITY;
		//}
		else if (token[0] == '#') {
			std::cout << token;
		}

		config >> token;
	}

	config.close();
}

