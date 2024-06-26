#pragma once


#include "Entity.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>


namespace Physics
{
	sf::Vector2f getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b, char d = 'D'); // d = default direction
	sf::Vector2f getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	sf::Vector2f getOverlapMouse(sf::Vector2f a, std::shared_ptr<Entity> b);
};

