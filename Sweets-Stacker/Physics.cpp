#include "Physics.h"
#include <cmath>

sf::Vector2f Physics::getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b, char d)
{
    sf::Vector2f overlap(0.f, 0.f);
    if (!a->hasComponent<CBoundingBox>() or !b->hasComponent<CBoundingBox>())
        return overlap;

    auto atx = a->getComponent<CTransform>();
    auto abb = a->getComponent<CBoundingBox>();
    auto btx = b->getComponent<CTransform>();
    auto bbb = b->getComponent<CBoundingBox>();

    if (abb.has && bbb.has)
    {
        if (d == 'D') // default
        {
            float dx = std::abs(atx.pos.x - btx.pos.x);
            float dy = std::abs(atx.pos.y - btx.pos.y);
            overlap = sf::Vector2f(abb.halfSize.x + bbb.halfSize.x - dx, abb.halfSize.y + bbb.halfSize.y - dy);
        }
        else
        {
            if (d == 'R') {
                if (abb.has && bbb.has)
                {
                    float dx = std::abs(atx.pos.x - btx.pos.x);
                    float dy = std::abs(atx.pos.y - btx.pos.y);

                    // Check if entities are close enough for potential collision
                    if (dx < abb.halfSize.x + bbb.halfSize.x && dy < abb.halfSize.y + bbb.halfSize.y)
                    {
                        float effectiveLeftA = atx.pos.x - abb.halfSize.x + 56.f;  // Adjusted left boundary for A
                        if (effectiveLeftA < atx.pos.x - 56.f) {
                            effectiveLeftA = atx.pos.x - 56.f;  // If left boundary exceeds the adjustment, use the adjustment
                        }

                        float overlapLeft = std::max(0.f, std::min(atx.pos.x + abb.halfSize.x, btx.pos.x + bbb.halfSize.x) - effectiveLeftA);
                        float overlapHeight = abb.halfSize.y + bbb.halfSize.y - dy;
                        overlap = sf::Vector2f(overlapLeft, overlapHeight);
                    }
                }
            }

            if (d == 'L') {
                if (abb.has && bbb.has) {
                    float dx = std::abs(atx.pos.x - btx.pos.x);
                    float dy = std::abs(atx.pos.y - btx.pos.y);

                    // Check if entities are close enough for potential collision
                    if (dx < abb.halfSize.x + bbb.halfSize.x && dy < abb.halfSize.y + bbb.halfSize.y) {
                        float effectiveRightA = atx.pos.x + abb.halfSize.x - 56.f;  // Adjusted right boundary for A
                        if (effectiveRightA > atx.pos.x + 56.f) {
                            effectiveRightA = atx.pos.x + 56.f;  // If right boundary exceeds the adjustment, use the adjustment
                        }

                        float overlapRight = std::max(0.f, effectiveRightA - std::max(atx.pos.x - abb.halfSize.x, btx.pos.x - bbb.halfSize.x));
                        float overlapHeight = abb.halfSize.y + bbb.halfSize.y - dy;
                        overlap = sf::Vector2f(overlapRight, overlapHeight);
                    }
                }

            }
           
            
        }

    }
    return overlap;
}

sf::Vector2f Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    sf::Vector2f overlap(0.f, 0.f);
    if (!a->hasComponent<CBoundingBox>() or !b->hasComponent<CBoundingBox>())
        return overlap;

    auto atx = a->getComponent<CTransform>();
    auto abb = a->getComponent<CBoundingBox>();
    auto btx = b->getComponent<CTransform>();
    auto bbb = b->getComponent<CBoundingBox>();

    if (abb.has && bbb.has)
    {
        float dx = std::abs(atx.prevPos.x - btx.prevPos.x);
        float dy = std::abs(atx.prevPos.y - btx.prevPos.y);
        overlap = sf::Vector2f(abb.halfSize.x + bbb.halfSize.x - dx, abb.halfSize.y + bbb.halfSize.y - dy);
    }
    return overlap;
}
