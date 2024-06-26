#include "Scene_SweetsStacker.h"
#include <fstream>
#include <iostream>

#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
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
	m_score_text.setFont(Assets::getInstance().getFont("Score"));
	//m_score_text.setPosition(40.f, 5.f);
	m_score_text.setCharacterSize(60);
	m_score_text.setFillColor(sf::Color::White);

	m_end_score_text.setFont(Assets::getInstance().getFont("Score"));
	m_end_score_text.setCharacterSize(60);
	m_end_score_text.setFillColor(sf::Color::White);

	registerAction(sf::Mouse::Left, "MOUSE_CLICK");
	m_pause_menu.push_back(false);
	m_pause_menu.push_back(false);


}

void Scene_SweetsStacker::sMovement(sf::Time dt) {
	playerMovement();

	//	 move all objects
	for (auto e : m_entityManager.getEntities()) {
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

	float lastCaughtY = 967.f; // Initialize with the initial position of the first caught scoop

	for (auto& e : m_entityManager.getEntities("playerNose")) {
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();

			auto& playerTfm = m_player->getComponent<CTransform>();
			tfm.pos = playerTfm.pos + sf::Vector2f(78, -45.f);


			if (tfm.pos.y > 967.f)
				tfm.pos.y = 967.f;

			if (m_player->getComponent<CInput>().dir == 'L')
				tfm.pos.x -= 157.f;

		}
	}


		for (int i = 0; i < m_entity.size(); i++) {
			std::shared_ptr<Entity> e = m_entity[i];

				if (e->hasComponent<CTransform>()) {
					auto& tfm = e->getComponent<CTransform>();
					auto& playerTfm = m_player->getComponent<CTransform>();
					auto& playerNoseTfm = m_playerNose->getComponent<CTransform>();

					tfm.pos = playerTfm.pos + sf::Vector2f(78, -45.f);

					float dropBelow = lastCaughtY - 5.f; // position above last caught scoop
					playerNoseTfm.pos.y -= 15;

					if (tfm.pos.y > dropBelow) {
						tfm.pos.y = dropBelow;
					}

					if (m_player->getComponent<CInput>().dir == 'L')
						tfm.pos.x -= 157.f;

					lastCaughtY = tfm.pos.y - 10; // update the last caught scoop pos
				}
			
		}	
}

void Scene_SweetsStacker::registerActions() {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Q, "BACK");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");

	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");

	registerAction(sf::Keyboard::W, "JUMP");
	registerAction(sf::Keyboard::Up, "JUMP");
	registerAction(sf::Keyboard::Space, "JUMP");

}

void Scene_SweetsStacker::playerMovement() {
	// dont move if game over 
	if (m_player->hasComponent<CState>() && m_player->getComponent<CState>().state == "gameOver")
		return;

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


void Scene_SweetsStacker::checkPlayerState() {
	auto& tx = m_player->getComponent<CTransform>();
	auto& state = m_player->getComponent<CState>().state;

	auto prevPos = tx.pos;

	if (m_lives == 0 || m_finished == true) {
		state = "gameOver";
		m_player->removeComponent<CAnimation>();

		switch (m_player->getComponent<CInput>().dir) {
		case 'L':
			m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("scoopyDeathLeft"));
			break;
		case 'R':
			m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("scoopyDeathRight"));
			break;
		default:
			break;
		}

		clearStack();
		tx.pos.x = prevPos.x;
		tx.pos.y = 990;
		m_player->getComponent<CTransform>().vel = sf::Vector2f(0.f, 0.f); 
		m_player->removeComponent<CBoundingBox>(); 
		m_playerNose->removeComponent<CBoundingBox>();
		m_player->getComponent<CState>().state;
	}


}

void Scene_SweetsStacker::sAnimation(sf::Time dt) {
	auto list = m_entityManager.getEntities();

	for (auto e : m_entityManager.getEntities()) {
		// update all animations
		if (e->hasComponent<CAnimation>()) {
			auto& anim = e->getComponent<CAnimation>();
			if (e->getComponent<CInput>().ISMOVING == true ||
				m_player->getComponent<CState>().state == "gameOver") {
				anim.animation.update(dt);
			}

			auto& tfm = e->getComponent<CTransform>();
			anim.animation.getSprite().setPosition(tfm.pos);
			anim.animation.getSprite().setRotation(tfm.angle);
			auto gameOver = m_player->getComponent<CState>().state;
			if (m_player->getComponent<CAnimation>().animation.hasEnded() &&
				gameOver == "gameOver") {
				std::this_thread::sleep_for(std::chrono::seconds(3));

				m_player->destroy();

				onEnd();
			}
		}
	}
}

void Scene_SweetsStacker::sRender() {

	m_game->window().setView(m_worldView);

	sf::Vector2i mousePos = sf::Mouse::getPosition(m_game->window());
	sf::Vector2f viewMousePos = m_game->window().mapPixelToCoords(mousePos, m_worldView);

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

		} else if(!e->getComponent<CType>().MENU) {
			// draw sprite
			auto& sprite = e->getComponent<CSprite>().sprite;
			auto& tfm = e->getComponent<CTransform>();

			sprite.setPosition(tfm.pos);
			sprite.setRotation(tfm.angle);

			m_game->window().draw(sprite);
		}

		// draw ui infront of dropables
		for (auto e : m_entityManager.getEntities("ui")) {
			if (e->getComponent<CSprite>().has) {
				auto& sprite = e->getComponent<CSprite>().sprite;
				auto& tfm = e->getComponent<CTransform>();

				sprite.setPosition(26.f,42.f);
				m_score_text.setPosition(180.f, 84.f);
				centerOrigin(m_score_text);
				std::string scoreString = std::to_string(m_score);
				m_score_text.setString(scoreString);

				m_game->window().draw(sprite);
				m_game->window().draw(m_score_text);

			}
		}

		for (auto e : m_entityManager.getEntities("life")) {
			if (e->getComponent<CSprite>().has) {
				auto& sprite = e->getComponent<CSprite>().sprite;
				auto& tfm = e->getComponent<CTransform>();

				sprite.setPosition(tfm.pos);
				sprite.setRotation(tfm.angle);
				m_game->window().draw(sprite);
			}
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

		if (m_isPaused) {
			for (auto e : m_entityManager.getEntities("pausescreen")) {
				if (e->getComponent<CSprite>().has) {
					auto& sprite = e->getComponent<CSprite>().sprite;
					auto& tfm = e->getComponent<CTransform>();

					sprite.setPosition(tfm.pos);

					m_game->window().draw(sprite);
				}
			}
			for (auto e : m_entityManager.getEntities("resume")) {
				if (e->getComponent<CSprite>().has) {
					auto& sprite = e->getComponent<CSprite>().sprite;
					auto& tfm = e->getComponent<CTransform>();

					auto overlap = Physics::getOverlapMouse(viewMousePos, e);
					if (overlap.x > 0 && overlap.y > 0) {
						m_pause_menu[0] = true;

						e->addComponent<CBoundingBox>(sprite.getLocalBounds().getSize());
					}
					else {
						m_pause_menu[0] = false;
					}
					sprite.setPosition(tfm.pos);

					m_game->window().draw(sprite);

				}
			}			
			
			for (auto e : m_entityManager.getEntities("quit")) {
				if (e->getComponent<CSprite>().has) {
					auto& sprite = e->getComponent<CSprite>().sprite;
					auto& tfm = e->getComponent<CTransform>();
					auto overlap = Physics::getOverlapMouse(viewMousePos, e);
					if (overlap.x > 0 && overlap.y > 0) {
						m_pause_menu[1] = true;
					}
					else {
						m_pause_menu[1] = false;
					}
					sprite.setPosition(tfm.pos);

					m_game->window().draw(sprite);
				}
			}
		}

		if (m_player->getComponent<CState>().state == "gameOver") {
			if (checkGameOverState() == true) {
				for (auto e : m_entityManager.getEntities("endscreen"))
				{
					if (e->getComponent<CSprite>().has) {
						auto& sprite = e->getComponent<CSprite>().sprite;
						auto& tfm = e->getComponent<CTransform>();

						if (m_level >= 1) {
							m_end_score = m_score * m_level;
						}
						else {
							m_end_score = m_score;
						}
						std::string endScoreString = std::to_string(m_end_score);
						m_end_score_text.setString(endScoreString);

						sprite.setPosition(tfm.pos);
						m_end_score_text.setPosition(400, 730);
						centerOrigin(m_end_score_text);

						m_game->window().draw(sprite);
						m_game->window().draw(m_end_score_text);

					}
				}
			}
			else {
				for (auto e : m_entityManager.getEntities("winscreen"))
				{
					if (e->getComponent<CSprite>().has) {
						auto& sprite = e->getComponent<CSprite>().sprite;
						auto& tfm = e->getComponent<CTransform>();

						if (m_level >= 1) {
							m_end_score = m_score * m_level;
						}
						else {
							m_end_score = m_score;
						}
						std::string endScoreString = std::to_string(m_end_score);
						m_end_score_text.setString(endScoreString);

						sprite.setPosition(tfm.pos);
						m_end_score_text.setPosition(400, 730);
						centerOrigin(m_end_score_text);

						m_game->window().draw(sprite);
						m_game->window().draw(m_end_score_text);

					}
				}
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

		else if (action.name() == "LEFT" && !m_isPaused && m_player->getComponent<CState>().state != "gameOver") {
			m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("ScoopyWalkLeft"));

			m_player->getComponent<CInput>().LEFT = true;
			m_player->getComponent<CInput>().dir = 'L';
			m_player->getComponent<CInput>().ISMOVING = true;
		}
		else if (action.name() == "RIGHT" && !m_isPaused && m_player->getComponent<CState>().state != "gameOver") {
			m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("ScoopyWalkRight"));

			m_player->getComponent<CInput>().RIGHT = true;
			m_player->getComponent<CInput>().dir = 'R';
			m_player->getComponent<CInput>().ISMOVING = true;

		}

		else if (action.name() == "JUMP") {
			if (m_player->getComponent<CInput>().JUMP) {
				m_player->getComponent<CInput>().UP = true;
				m_player->getComponent<CInput>().JUMP = false;
			}
		}
	}

	if (action.name() == "MOUSE_CLICK" && m_isPaused) {
		if (m_pause_menu[0] == true) {
			m_isPaused = false;
		}if (m_pause_menu[1] == true) {
			m_game->quitLevel();
		}
		
	}


	// on Key Release
	else if (action.type() == "END") {
		if (action.name() == "LEFT" && !m_isPaused && m_player->getComponent<CState>().state != "gameOver" ) {

			m_player->getComponent<CAnimation>().animation.m_currentFrame = 0;

			m_player->getComponent<CInput>().LEFT = false;
			m_player->getComponent<CInput>().ISMOVING = false;


		}
		else if (action.name() == "RIGHT" && !m_isPaused && m_player->getComponent<CState>().state != "gameOver") {

			m_player->getComponent<CAnimation>().animation.m_currentFrame = 0;

			m_player->getComponent<CInput>().RIGHT = false;
			m_player->getComponent<CInput>().ISMOVING = false;

		}
		else if (action.name() == "JUMP") {
			m_player->getComponent<CInput>().UP = false;
		}
	}

}

void Scene_SweetsStacker::checkGround() {
	auto ground = m_entityManager.addEntity("ground");
	auto& sprite = ground->addComponent<CSprite>().sprite;

	sf::Vector2f pos{ 400.f,1138.f };
	auto& tfm = ground->addComponent<CTransform>(pos);
	ground->addComponent<CBoundingBox>(sf::Vector2f(800.f, 130.f));
}

void Scene_SweetsStacker::spawnPlayer() {
	auto pos = m_worldView.getSize();
	pos.x = pos.x / 2.f;
	pos.y -= 199.f;

	auto nosePosRight = m_worldView.getSize();
	nosePosRight.x = pos.x + 199.f / 2 - (32.f / 2 + 6.f);
	nosePosRight.y = pos.y - 60.f;

	m_player = m_entityManager.addEntity("player");
	m_playerNose = m_entityManager.addEntity("playerNose");

	m_player->addComponent<CTransform>(pos);
	m_playerNose->addComponent<CTransform>(pos);

	m_player->addComponent<CInput>();
	m_player->addComponent<CState>();
	m_player->getComponent<CInput>().dir = 'R';

	auto& sprite = m_player->addComponent<CSprite>().sprite;
	auto& spriteTex = m_player->addComponent<CSprite>(Assets::getInstance().getTexture("DogR")).sprite;
	sprite.setTexture(Assets::getInstance().getTexture("DogR"));

	auto sizey = sprite.getLocalBounds().getSize().y;
	auto sizex = sprite.getLocalBounds().getSize().x;


	m_player->addComponent<CBoundingBox>(sf::Vector2f(190.f, 125.f));
	
	m_playerNose->addComponent<CBoundingBox>(sf::Vector2f(32.f, 38.f));

}

void Scene_SweetsStacker::spawnEnemies(sf::Time dt) {
	static sf::Time spawnTimer = sf::Time::Zero;

	spawnTimer += dt;
	sf::FloatRect spawningBounds = getOutOfBounds();

	while (spawnTimer >= m_enemyInterval) {
		// positions
		std::uniform_real_distribution<float> distX(spawningBounds.left, spawningBounds.left + spawningBounds.width);
		// range
		std::uniform_real_distribution<float> distY(spawningBounds.top, spawningBounds.top + spawningBounds.height);
		// random positions
		sf::Vector2f spawnPos{ distX(rng), distY(rng) };
		// random sprite
		std::uniform_int_distribution<int> distDrop(0, 15);

		int enemy = distDrop(rng);
		//std::string spriteName;

		auto enemies = m_entityManager.addEntity("enemies");
		enemies->addComponent<CType>().DROPABLE = true;

		if (enemy < 5) {
			m_spriteName = "Pickle";
		}
		else if (enemy <= 10) {
			m_spriteName = "Pepper";
		}
		else {
			m_spriteName = "FishBones";
		}

		auto& sprite = enemies->addComponent<CSprite>().sprite;
		auto [txtName, txtRect] = Assets::getInstance().getSprt(m_spriteName);
		enemies->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);
		enemies->addComponent<CBoundingBox>(sf::Vector2f(40.f, 57.f));

		auto size = sprite.getLocalBounds().getSize();
		centerOrigin(sprite);

		// position of spawned enemy
		enemies->addComponent<CTransform>(spawnPos);
		enemies->getComponent<CTransform>().vel = m_levelSpeed;

		spawnTimer -= m_enemyInterval;
	}
}

void Scene_SweetsStacker::spawnScoops(sf::Time dt) {

	static sf::Time spawnTimer = sf::Time::Zero;

	spawnTimer += dt;
	sf::FloatRect spawningBounds = getOutOfBounds();

	while (spawnTimer >= m_scoopInterval) {
		// positions
		std::uniform_real_distribution<float> distX(spawningBounds.left, spawningBounds.left + spawningBounds.width);
		// range
		std::uniform_real_distribution<float> distY(spawningBounds.top, spawningBounds.top + spawningBounds.height);
		// random positions
		sf::Vector2f spawnPos{ distX(rng), distY(rng) };
		// random sprite
		std::uniform_int_distribution<int> distDrop(0, 15);

		int scoopFlavour = distDrop(rng);
		//std::string spriteName;

		auto fallingScoops = m_entityManager.addEntity("iceCream");
		fallingScoops->addComponent<CType>().DROPABLE = true;

		if (scoopFlavour < 5) {
			m_spriteName = "StrawberryDrop";
			fallingScoops->addComponent<CFlavour>().STRAWBERRY = true;
		}
		else if (scoopFlavour <= 10) {
			m_spriteName = "ChocolateDrop";
			fallingScoops->addComponent<CFlavour>().CHOCOLATE = true;
		}
		else {
			m_spriteName = "VanillaDrop";
			fallingScoops->addComponent<CFlavour>().VANILLA = true;
		}

		auto& sprite = fallingScoops->addComponent<CSprite>().sprite;
		auto [txtName, txtRect] = Assets::getInstance().getSprt(m_spriteName);
		fallingScoops->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);
		fallingScoops->addComponent<CBoundingBox>(sf::Vector2f(40.f, 57.f));

		auto size = sprite.getLocalBounds().getSize();
		centerOrigin(sprite);

		// position of spawned scoop
		fallingScoops->addComponent<CTransform>(spawnPos);
		fallingScoops->getComponent<CTransform>().vel = m_levelSpeed;

		spawnTimer -= m_scoopInterval;
	}

}

void Scene_SweetsStacker::spawnCones(sf::Time dt) {
	static sf::Time spawnTimer = sf::Time::Zero;

	spawnTimer += dt;
	sf::FloatRect spawningBounds = getOutOfBounds();

	while (spawnTimer >= m_coneInterval) {
		// positions
		std::uniform_real_distribution<float> distX(spawningBounds.left, spawningBounds.left + spawningBounds.width);
		// range
		std::uniform_real_distribution<float> distY(spawningBounds.top, spawningBounds.top + spawningBounds.height);
		// random positions
		sf::Vector2f spawnPos{ distX(rng), distY(rng) };
		// random sprite
		std::uniform_int_distribution<int> distDrop(0, 15);

		int cone = distDrop(rng);

		auto fallingCone = m_entityManager.addEntity("cone");
		fallingCone->addComponent<CType>().DROPABLE = true;
		fallingCone->addComponent<CLifespan>(6); // add lifespan component

		auto& sprite = fallingCone->addComponent<CSprite>().sprite;
		auto [txtName, txtRect] = Assets::getInstance().getSprt("ConeDrop");
		fallingCone->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);
		fallingCone->addComponent<CBoundingBox>(sf::Vector2f(40.f, 57.f));

		auto size = sprite.getLocalBounds().getSize();
		centerOrigin(sprite);

		// position of spawned cone
		fallingCone->addComponent<CTransform>(spawnPos);
		fallingCone->getComponent<CTransform>().vel = m_levelSpeed;

		spawnTimer -= m_coneInterval;
	}
}

void Scene_SweetsStacker::spawnFinishline() {

		auto finshline = m_entityManager.addEntity("finshline");
		auto& sprite = finshline->addComponent<CSprite>().sprite;

		auto& spriteTex = finshline->addComponent<CSprite>(Assets::getInstance().getTexture("FinishLine")).sprite;
		sprite.setTexture(Assets::getInstance().getTexture("FinishLine"));

		auto& tfm = finshline->getComponent<CTransform>();

		// Set position and rotation
		tfm.pos = sf::Vector2f(m_worldView.getSize().x / 2, 300.f);
		tfm.angle = 0.f;

		// Add bounding box component
		auto spriteSize = sprite.getLocalBounds().getSize();
		finshline->addComponent<CBoundingBox>(spriteSize);
	

}

void Scene_SweetsStacker::spawnStrawberry() {
	auto playerNose = m_entityManager.getEntities("playerNose");
	for (auto pN : playerNose) {
		auto caughtScoop = m_entityManager.addEntity("caughtStrawberry");
		auto& sprite = caughtScoop->addComponent<CSprite>().sprite;

		auto [txtName, txtRect] = Assets::getInstance().getSprt("StrawberryScoop");
		caughtScoop->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);

		auto& pntfm = pN->getComponent<CTransform>();
		auto& cstfm = caughtScoop->getComponent<CTransform>();
		caughtScoop->addComponent<CTransform>(pntfm);
		
		m_entity.push_back(caughtScoop);
	}
}

void Scene_SweetsStacker::spawnChocolate() {
	auto playerNose = m_entityManager.getEntities("playerNose");
	for (auto pN : playerNose) {
		auto caughtScoop = m_entityManager.addEntity("caughtChocolate");
		auto& sprite = caughtScoop->addComponent<CSprite>().sprite;

		auto [txtName, txtRect] = Assets::getInstance().getSprt("ChocolateScoop");
		caughtScoop->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);

		auto& pntfm = pN->getComponent<CTransform>();
		auto& cstfm = caughtScoop->getComponent<CTransform>();
		caughtScoop->addComponent<CTransform>(pntfm);
		caughtScoop->addComponent<CType>().CAUGHT = true;
		
		m_entity.push_back(caughtScoop);
	}
	
}

void Scene_SweetsStacker::spawnVanilla() {
	auto playerNose = m_entityManager.getEntities("playerNose");
	for (auto pN : playerNose) {
		auto caughtScoop = m_entityManager.addEntity("caughtVanilla");
		auto& sprite = caughtScoop->addComponent<CSprite>().sprite;

		auto [txtName, txtRect] = Assets::getInstance().getSprt("VanillaScoop");
		caughtScoop->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);

		auto& pntfm = pN->getComponent<CTransform>();
		auto& cstfm = caughtScoop->getComponent<CTransform>();
		caughtScoop->addComponent<CTransform>(pntfm);

		m_entity.push_back(caughtScoop);
	}
}

void Scene_SweetsStacker::clearStack() {
	auto& c = m_entityManager.getEntities("caughtChocolate");
	auto& s = m_entityManager.getEntities("caughtStrawberry");
	auto& v = m_entityManager.getEntities("caughtVanilla");

	for (auto& entity : c) {
		entity->destroy();
	}
	for (auto& entity : s) {
		entity->destroy();
	}
	for (auto& entity : v) {
		entity->destroy();
	}

	m_entity.clear();

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

void Scene_SweetsStacker::sUpdateLifeSprites() {
	sf::Vector2f position(732.f , 85.f);
	for (auto& e : m_entityManager.getEntities("life")) {
		e->destroy();
	}
	for (int i = 0; i < m_lives; ++i) {
		auto lives = m_entityManager.addEntity("life");
		lives->addComponent<CType>().MENU = true;
		
		auto& tfm = lives->addComponent<CTransform>(position);

		auto& sprite = lives->addComponent<CSprite>().sprite;
		auto& spriteTex = lives->addComponent<CSprite>(Assets::getInstance().getTexture("lives")).sprite;
		
		sprite.setTexture(Assets::getInstance().getTexture("lives"));
		position.x -= 97.f;
	}
}




void Scene_SweetsStacker::sCollisions() {

	auto& iceCream = m_entityManager.getEntities("iceCream");
	auto& ground = m_entityManager.getEntities("ground");
	auto& player = m_entityManager.getEntities("player");
	auto& playerNose = m_entityManager.getEntities("playerNose");
	auto& enemies = m_entityManager.getEntities("enemies");
	auto& cone = m_entityManager.getEntities("cone");
	auto& finishline = m_entityManager.getEntities("finshline");

	auto& jump = m_player->getComponent<CInput>().JUMP;

	for (auto& pN : playerNose) {
		for (auto& f : finishline) {
			auto overlap = Physics::getOverlap(pN, f);
			if (overlap.x > 0 && overlap.y > 0 && jump == true) {
				m_finished = true;
				checkPlayerState();

			}
		}
	}


	for (auto p : player) {
		for (auto g : ground) {
			auto overlap = Physics::getOverlap(p, g);
			if (overlap.x > 0 and overlap.y > 0) {
				p->getComponent<CTransform>().pos.y -= overlap.y;

				p->getComponent<CInput>().JUMP = true;
			}
		}
	}

	for (auto p : player) {
		for (auto c : cone) {
			if (c->getComponent<CType>().GROUNDED == true) {
				auto overlap = Physics::getOverlap(p, c, p->getComponent<CInput>().dir); // changes bb collisons so it doesnt collide with the tail
				if (overlap.x > 0 and overlap.y > 0)
				{
					playRandomBark();
					c->destroy();
					m_lives--;
					checkPlayerState();

				}
			}
		}
	}

	for (auto pN : playerNose) {
		for (auto i : iceCream) {
			auto overlap = Physics::getOverlap(pN, i);
			if (overlap.x > 0 and overlap.y > 0) {
				SoundPlayer::getInstance().play("splat", m_player->getComponent<CTransform>().pos);
				i->destroy();
				m_score += 50;

				if (i->getComponent<CFlavour>().STRAWBERRY == true) {
					spawnStrawberry();
				}
				else if (i->getComponent<CFlavour>().CHOCOLATE == true) {
					spawnChocolate();

				}
				else if (i->getComponent<CFlavour>().VANILLA == true) {
					spawnVanilla();
				}

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

	for (auto g : ground) {
		for (auto e : enemies) {
			auto overlap = Physics::getOverlap(g, e);
			if (overlap.x > 0 and overlap.y > 0) {

				e->destroy();
			}
		}
	}

	for (auto c : cone) {
		for (auto g : ground) {
			auto overlap = Physics::getOverlap(c, g);
			if (overlap.x > 0 and overlap.y > 0) {
				c->getComponent<CType>().GROUNDED = true;
				c->getComponent<CTransform>().pos.y -= overlap.y;

				auto& spriteComponent = c->getComponent<CSprite>();
				auto& sprite = spriteComponent.sprite;
				auto& lifespan = c->getComponent<CLifespan>();
				lifespan.remaining -= sf::seconds(1.0f / 60.0f);

				sf::Color fillColor = sprite.getColor();
				fillColor.a = lifespan.remaining / lifespan.total * 255.0f;

				sprite.setColor(fillColor);
			}
		}
	}

	if (!m_entity.empty()) {
		for (auto pN : playerNose) {
			for (auto c : cone) {
				auto overlap = Physics::getOverlap(pN, c);
				if (overlap.x > 0 and overlap.y > 0) {
					SoundPlayer::getInstance().play("lick", m_player->getComponent<CTransform>().pos);
					m_score += 100;

					clearStack();
					c->destroy();
					m_level++;


				}
			}
		}
	}

	for (auto pN : playerNose) {
		for (auto e : enemies) {
			auto overlap = Physics::getOverlap(pN, e);
			if (overlap.x > 0 and overlap.y > 0) {
				playRandomBark(); 

				e->destroy();
				m_lives--;
				checkPlayerState();

			}
		}
	}


}

void Scene_SweetsStacker::playRandomBark() {
	std::uniform_int_distribution<> dis(0, 2);
	int randomBark = dis(rng);

	std::string soundName;
	switch (randomBark) {
	case 0:
		soundName = "whine";
		break;
	case 1:
		soundName = "whine2";
		break;
	case 2:
	default:
		soundName = "whine3";
		break;
	}

	SoundPlayer::getInstance().play(soundName, m_player->getComponent<CTransform>().pos);
}


bool Scene_SweetsStacker::checkGameOverState() {
	if (m_player->getComponent<CState>().state == "gameOver" 
		&& m_lives == 0) {
		return true;
	}
	else {
		return false;
	}
}

void Scene_SweetsStacker::sCheckLevel() {
	if(m_level > m_prevLevel) {
		auto& iceCream = m_entityManager.getEntities("iceCream");
		auto& enemies = m_entityManager.getEntities("enemies");
		auto& cone = m_entityManager.getEntities("cone");

		//ice cream scoops
		m_scoopInterval -= sf::seconds(0.2f);

		if (m_scoopInterval < sf::seconds(0.8f)) {
			m_scoopInterval = sf::seconds(0.8f); // limit the interval to a minimum of 0.5 seconds
		}

		for (auto& i : iceCream) {
			auto& transformComponent = i->getComponent<CTransform>();
			m_levelSpeed =	transformComponent.vel += sf::Vector2f(0.f,50.f);

		}

		// enemies
		m_enemyInterval -= sf::seconds(0.2f);

		if (m_enemyInterval < sf::seconds(0.8f)) {
			m_enemyInterval = sf::seconds(0.8f); // limit the interval to a minimum of 0.5 seconds
		}

		for (auto& e : enemies) {
			auto& transformComponent = e->getComponent<CTransform>();
			m_levelSpeed = transformComponent.vel += sf::Vector2f(0.f, 50.f);

		}

		// cones
		m_coneInterval -= sf::seconds(0.15f);

		if (m_coneInterval < sf::seconds(0.8f)) {
			m_coneInterval = sf::seconds(0.8f); // limit the interval to a minimum of 0.5 seconds
		}

		for (auto& c : cone) {
			auto& transformComponent = c->getComponent<CTransform>();
			m_levelSpeed = transformComponent.vel += sf::Vector2f(0.f, 50.f);

		}

		m_prevLevel = m_level; // Update previousLevel after processing the increase
	}

	if (m_level == 7 && m_finishLineSpawned == false) {
		spawnFinishline();
		m_finishLineSpawned = true;
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
	sLifespan(dt);
	sRespawnEntities(dt);
	sUpdateLifeSprites();
	sCheckLevel();

}

void Scene_SweetsStacker::onEnd() {
	m_game->quitLevel();
	m_lives = 3;
	m_level = 0;
}


void Scene_SweetsStacker::sRespawnEntities(sf::Time dt){
	spawnScoops(dt);
	spawnEnemies(dt);
	spawnCones(dt);
}

void Scene_SweetsStacker::sLifespan(sf::Time dt) {

	auto entities = m_entityManager.getEntities();

	for (auto& eLS : entities) {
		// for all entities that have a CLifespan compnent
		if (eLS->hasComponent<CLifespan>() && eLS->getComponent<CType>().GROUNDED) {

			auto& lifespan = eLS->getComponent<CLifespan>();
			lifespan.remaining -= dt;

			if (lifespan.remaining <= sf::seconds(2.0f)) {
				eLS->getComponent<CBoundingBox>().size = sf::Vector2f(0,0);
				eLS->getComponent<CType>().GROUNDED = false;
			}
			if (lifespan.remaining <= sf::Time::Zero) {
				eLS->destroy();	// if entity lifespan runs out destroy the entity
			}
		}
	}
}

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
			e->addComponent<CType>().MENU = true;

			// for background, no textureRect its just the whole texture
			// and no center origin, position by top left corner
			// stationary so no CTransfrom required.
			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token == "Ui") {
			std::string name; 
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("ui");
			e->addComponent<CType>().MENU = true;
			e->addComponent<CTransform>(pos);

			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite; 
			sprite.setOrigin(0.f, 0.f); 
			sprite.setPosition(pos); 
		}
		else if (token == "Paused") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("pausescreen");
			e->addComponent<CType>().MENU = true;
			e->addComponent<CTransform>(pos);

			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token == "GameOver") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("endscreen");
			e->addComponent<CType>().MENU = true;
			e->addComponent<CTransform>(pos);

			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token == "GameOverWin") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("winscreen");
			e->addComponent<CType>().MENU = true;
			e->addComponent<CTransform>(pos);

			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token == "Resume") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("resume");
			e->addComponent<CType>().MENU = true;

			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;

			auto spriteSize = sprite.getLocalBounds().getSize();

			e->addComponent<CBoundingBox>(spriteSize);
			sf::Vector2f spriteOrigin = sprite.getOrigin();

			sf::Vector2f boundingBoxPosition = pos + spriteOrigin;

			e->addComponent<CTransform>(boundingBoxPosition);
		}
		else if (token == "Quit") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = m_entityManager.addEntity("quit");
			e->addComponent<CType>().MENU = true;

			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			auto spriteSize = sprite.getLocalBounds().getSize();

			e->addComponent<CBoundingBox>(spriteSize);
			sf::Vector2f spriteOrigin = sprite.getOrigin();

			sf::Vector2f boundingBoxPosition = pos + spriteOrigin;

			e->addComponent<CTransform>(boundingBoxPosition);

		}
		else if (token[0] == '#') {
			std::cout << token;
		}
		config >> token;
	}

	config.close();
}

