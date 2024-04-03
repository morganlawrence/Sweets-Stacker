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
	//for (auto e : m_entityManager.getEntities("enemies")) {
	//	if (e->hasComponent<CTransform>()) {
	//		auto& tfm = e->getComponent<CTransform>();
	//
	//		tfm.pos += tfm.vel * dt.asSeconds();
	//		tfm.angle += tfm.angVel * dt.asSeconds();
	//	}
	//}

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

			//std::cout << m_player->getComponent<CInput>().dir;
		}
	}


		//for (const std::string& flavour : { "caughtStrawberry", "caughtChocolate", "caughtVanilla" }) {
		for (int i = 0; i < m_entity.size(); i++) {
			std::shared_ptr<Entity> e = m_entity[i];

			//for (auto& e : m_entityManager.getEntities(flavour)) {
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
			//}
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

			//if (e->hasComponent<CLifespan>()) {
			//	auto& spriteComponent = e->getComponent<CSprite>();
			//	auto& sprite = spriteComponent.sprite; 
			//	auto& lifespan = e->getComponent<CLifespan>();

			//	sf::Color fillColor = sprite.getColor();
			//	fillColor.a = lifespan.remaining / lifespan.total * 255.0f;

			//	sprite.setColor(fillColor);
			//}

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
				m_game->window().draw(sprite);
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

		else if (action.name() == "LEFT" && !m_isPaused) {
			m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("ScoopyWalkLeft"));

			m_player->getComponent<CInput>().LEFT = true;
			m_player->getComponent<CInput>().dir = 'L';
		}
		else if (action.name() == "RIGHT" && !m_isPaused) {
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
		if (action.name() == "LEFT" && !m_isPaused) {
			m_player->removeComponent<CAnimation>();
			m_player->addComponent<CSprite>(Assets::getInstance().getTexture("DogL")).sprite;
			m_player->getComponent<CInput>().LEFT = false;

		}
		else if (action.name() == "RIGHT" && !m_isPaused) {
			m_player->removeComponent<CAnimation>();
			m_player->addComponent<CSprite>(Assets::getInstance().getTexture("DogR")).sprite;
			m_player->getComponent<CInput>().RIGHT = false;

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

	auto& sprite = m_player->addComponent<CSprite>().sprite;
	auto& spriteTex = m_player->addComponent<CSprite>(Assets::getInstance().getTexture("DogR")).sprite;
	sprite.setTexture(Assets::getInstance().getTexture("DogR"));

	auto sizey = sprite.getLocalBounds().getSize().y;
	auto sizex = sprite.getLocalBounds().getSize().x;

	//centerOrigin(sizex,sizey);

	m_player->addComponent<CBoundingBox>(sf::Vector2f(190.f, 125.f));
	
	//m_player->addComponent<CBoundingBox>(sf::Vector2f(120.f, 125.f));

	m_playerNose->addComponent<CBoundingBox>(sf::Vector2f(32.f, 38.f));

	std::cout << m_player->getComponent<CTransform>().pos.y;
}

void Scene_SweetsStacker::spawnEnemies(sf::Time dt) {
	static sf::Time spawnTimer = sf::Time::Zero;

	static const sf::Time spawnInterval = sf::seconds(6.0f);

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
		std::uniform_int_distribution<int> distDrop(0, 15);

		int enemy = distDrop(rng);
		//std::string spriteName;

		auto enemies = m_entityManager.addEntity("enemies");
		enemies->addComponent<CType>().DROPABLE = true;

		if (enemy < 5) {
			m_spriteName = "Pickle";
			std::cout << "Pickle Spawned\n";
		}
		else if (enemy <= 10) {
			m_spriteName = "Pepper";
			std::cout << "Pepper Spawned\n";
		}
		else {
			m_spriteName = "FishBones";
			std::cout << "Fish Bones Spawned\n";
		}

		auto& sprite = enemies->addComponent<CSprite>().sprite;
		auto [txtName, txtRect] = Assets::getInstance().getSprt(m_spriteName);
		enemies->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);
		enemies->addComponent<CBoundingBox>(sf::Vector2f(40.f, 57.f));

		auto size = sprite.getLocalBounds().getSize();
		centerOrigin(sprite);

		// position of spawned scoop
		enemies->addComponent<CTransform>(spawnPos);
		enemies->getComponent<CTransform>().vel = sf::Vector2f(0.f, 100.f);

		spawnTimer -= spawnInterval;
	}
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
		std::uniform_int_distribution<int> distDrop(0, 15);

		int scoopFlavour = distDrop(rng);
		//std::string spriteName;

		auto fallingScoops = m_entityManager.addEntity("iceCream");
		fallingScoops->addComponent<CType>().DROPABLE = true;

		if (scoopFlavour < 5) {
			m_spriteName = "StrawberryDrop";
			fallingScoops->addComponent<CFlavour>().STRAWBERRY = true;
			std::cout << "Stawberry Spawned\n";
		}
		else if (scoopFlavour <= 10) {
			m_spriteName = "ChocolateDrop";
			fallingScoops->addComponent<CFlavour>().CHOCOLATE = true;
			std::cout << "Chocolate Spawned\n";
		}
		else {
			m_spriteName = "VanillaDrop";
			fallingScoops->addComponent<CFlavour>().VANILLA = true;
			std::cout << "Vanilla Spawned\n";
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

		spawnTimer -= spawnInterval;
	}
	//auto check = m_entityManager.getEntities("iceCream");
	//auto newVel = check->getComponent<CTransform>().vel;
	//newVel *= 1.2;
}

void Scene_SweetsStacker::spawnCones(sf::Time dt) {
	static sf::Time spawnTimer = sf::Time::Zero;

	static const sf::Time spawnInterval = sf::seconds(10.0f);

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
		std::uniform_int_distribution<int> distDrop(0, 15);

		int cone = distDrop(rng);
		//std::string spriteName;

		auto fallingCone = m_entityManager.addEntity("cone");
		fallingCone->addComponent<CType>().DROPABLE = true;
		fallingCone->addComponent<CLifespan>(6); // add lifespan component
		//fallingCone->addComponent<CType>().GROUNDED;
		auto& sprite = fallingCone->addComponent<CSprite>().sprite;
		auto [txtName, txtRect] = Assets::getInstance().getSprt("ConeDrop");
		fallingCone->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);
		fallingCone->addComponent<CBoundingBox>(sf::Vector2f(40.f, 57.f));

		auto size = sprite.getLocalBounds().getSize();
		centerOrigin(sprite);
		std::cout << "Cone Spawned\n";

		// position of spawned cone
		fallingCone->addComponent<CTransform>(spawnPos);
		fallingCone->getComponent<CTransform>().vel = sf::Vector2f(0.f, 100.f);

		spawnTimer -= spawnInterval;
	}
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
		//tfm.getSprite().setPosition(tfm.pos);
		//sprite.setPosition(tfm.pos);
	}
}




void Scene_SweetsStacker::sCollisions() {
	//adjustPlayerPosition();

	auto& iceCream = m_entityManager.getEntities("iceCream");
	auto& ground = m_entityManager.getEntities("ground");
	auto& player = m_entityManager.getEntities("player");
	auto& playerNose = m_entityManager.getEntities("playerNose");
	auto& enemies = m_entityManager.getEntities("enemies");
	auto& cone = m_entityManager.getEntities("cone");


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
					SoundPlayer::getInstance().play("whine3", m_player->getComponent<CTransform>().pos);
					c->destroy();
					m_lives--;

				}
			}
		}
	}

	for (auto pN : playerNose) {
		for (auto i : iceCream) {
			auto overlap = Physics::getOverlap(pN, i);
			if (overlap.x > 0 and overlap.y > 0) {
				SoundPlayer::getInstance().play("splat", m_player->getComponent<CTransform>().pos); // pos??
				i->destroy();
				
				if (i->getComponent<CFlavour>().STRAWBERRY == true) {
					//auto caughtScoop = m_entityManager.addEntity("caughtStrawberry");
					//auto& sprite = caughtScoop->addComponent<CSprite>().sprite;
					//
					//auto [txtName, txtRect] = Assets::getInstance().getSprt("StrawberryScoop");
					//caughtScoop->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);
					//
					//auto& pntfm = pN->getComponent<CTransform>();
					//auto& cstfm = caughtScoop->getComponent<CTransform>();
					//caughtScoop->addComponent<CTransform>(pntfm);
					//
					//m_entity.push_back(caughtScoop);
					spawnStrawberry();
				}
				else if (i->getComponent<CFlavour>().CHOCOLATE == true) {
					//auto caughtScoop = m_entityManager.addEntity("caughtChocolate");
					//auto& sprite = caughtScoop->addComponent<CSprite>().sprite;
					//
					//auto [txtName, txtRect] = Assets::getInstance().getSprt("ChocolateScoop");
					//caughtScoop->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);
					//
					//auto& pntfm = pN->getComponent<CTransform>();
					//auto& cstfm = caughtScoop->getComponent<CTransform>();
					//caughtScoop->addComponent<CTransform>(pntfm);
					//
					//m_entity.push_back(caughtScoop);
					spawnChocolate();

				}
				else if (i->getComponent<CFlavour>().VANILLA == true) {
					//auto caughtScoop = m_entityManager.addEntity("caughtVanilla");
					//auto& sprite = caughtScoop->addComponent<CSprite>().sprite;
					//
					//auto [txtName, txtRect] = Assets::getInstance().getSprt("VanillaScoop");
					//caughtScoop->addComponent<CSprite>(Assets::getInstance().getTexture(txtName), txtRect);
					//
					//auto& pntfm = pN->getComponent<CTransform>();
					//auto& cstfm = caughtScoop->getComponent<CTransform>();
					//caughtScoop->addComponent<CTransform>(pntfm);
					//
					//m_entity.push_back(caughtScoop);
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

	for (auto pN : playerNose) {
		for (auto c : cone) {
			auto overlap = Physics::getOverlap(pN, c);
			if (overlap.x > 0 and overlap.y > 0) {
				SoundPlayer::getInstance().play("lick", m_player->getComponent<CTransform>().pos);

				clearStack();
				c->destroy();
				std::cout << "collision level before: ";
				std::cout << m_level;
				std::cout << "\n";
				std::cout << m_levelSpeed;
				m_level++;
			

			}
		}
	}

	for (auto pN : playerNose) {
		for (auto e : enemies) {
			auto overlap = Physics::getOverlap(pN, e);
			if (overlap.x > 0 and overlap.y > 0) {

			/*	std::uniform_int_distribution<> dis(0, 2);
				int randomBark = dis(rng);

				if (randomBark == 0) {
					SoundPlayer::getInstance().play("whine",  m_player->getComponent<CTransform>().pos);
				}
				else if(randomBark == 1) {
					SoundPlayer::getInstance().play("whine2", m_player->getComponent<CTransform>().pos);
				}
				else {*/
					SoundPlayer::getInstance().play("whine3", m_player->getComponent<CTransform>().pos);
				//}

				e->destroy();
				m_lives--;
			}
		}
	}


}

void Scene_SweetsStacker::sCheckLevel() {
	if(m_level > m_prevLevel) {
		auto iceCream = m_entityManager.getEntities("iceCream");

		for (auto& i : iceCream) {
			auto& transformComponent = i->getComponent<CTransform>();
			m_levelSpeed =	transformComponent.vel += sf::Vector2f(0.f,50.f); // Assuming vel is a vector type
		}

		m_prevLevel = m_level; // Update previousLevel after processing the increase
	}
}

void Scene_SweetsStacker::sUpdate(sf::Time dt) {
	SoundPlayer::getInstance().removeStoppedSounds();

	if (m_isPaused)
		return;

	m_entityManager.update();

	sRespawnLifeSprites();

	sAnimation(dt);
	sMovement(dt);
	adjustPlayerPosition();
	sCollisions();
	sLifespan(dt);
	sRespawnEntities(dt);
	sUpdateLifeSprites();
	sCheckLevel();

	//spawnScoops(dt);
	//spawnEnemies(dt);
}

void Scene_SweetsStacker::onEnd() {
	//MusicPlayer::getInstance().setPaused("gameTheme");
	m_game->quitLevel();
	//m_game->changeScene("MENU", nullptr, false);
	m_lives = 3;
}

void Scene_SweetsStacker::sRespawnLifeSprites() {
	if (m_lives == 0) {
		onEnd();
	}
}

void Scene_SweetsStacker::sRespawnEntities(sf::Time dt){
	spawnScoops(dt);
	spawnEnemies(dt);
	spawnCones(dt);
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

