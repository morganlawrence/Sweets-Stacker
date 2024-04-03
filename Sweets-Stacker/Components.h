//
// Created by David Burchill on 2023-09-27.
//

#ifndef BREAKOUT_COMPONENTS_H
#define BREAKOUT_COMPONENTS_H


#include <memory>
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "Animation.h"
#include <bitset>


struct Component
{
    bool		has{ false };
    Component() = default;
};


struct CAnimation : public Component {
    Animation   animation;

    CAnimation() = default;
    CAnimation(const Animation& a) : animation(a) {}

};

struct CSprite : public Component {
    sf::Sprite sprite;

    CSprite() = default;

    CSprite(const sf::Texture& t)
        : sprite(t) {
        centerOrigin(sprite);
    }

    CSprite(const sf::Texture& t, sf::IntRect r)
        : sprite(t, r) {
        centerOrigin(sprite);
    }
};


struct CTransform : public Component
{

    sf::Transformable  tfm;
    sf::Vector2f	pos{ 0.f, 0.f };
    sf::Vector2f	prevPos{ 0.f, 0.f };
    sf::Vector2f	vel{ 0.f, 0.f };
    sf::Vector2f	scale{ 1.f, 1.f };

    float           angVel{ 0 };
    float	        angle{ 0.f };

    CTransform() = default;
    CTransform(const sf::Vector2f& p) : pos(p) {}
    CTransform(const sf::Vector2f& p, const sf::Vector2f& v)
        : pos(p), prevPos(p), vel(v) {}

};



struct CBoundingBox : public Component
{
    sf::Vector2f size{ 0.f, 0.f };
    sf::Vector2f halfSize{ 0.f, 0.f };

    CBoundingBox() = default;
    CBoundingBox(const sf::Vector2f& s) : size(s), halfSize(0.5f * s)
    {}
};

struct CState : public Component
{
    //enum State {
    //    isGrounded = 1,
    //    isFacingLeft = 1 << 1,
    //    isRunning = 1 << 2
    //};
    //unsigned int  state{ 0 };

    //CState() = default;
    //CState(unsigned int s) : state(s) {}
    //bool test(unsigned int x) { return (state & x); }
    //void set(unsigned int x) { state |= x; }
    //void unSet(unsigned int x) { state &= ~x; }
    std::string state{ "none" };

    CState() = default;
    CState(const std::string& s) : state(s) {}
};

struct CFlavour : public Component {
    bool STRAWBERRY{ false };
    bool CHOCOLATE{ false };
    bool VANILLA{ false };

    bool ICECREAM{ false };


    CFlavour() = default;
};

struct CInput : public Component
{
    bool UP{ false };
    bool LEFT{ false };
    bool RIGHT{ false };
    bool DOWN{ false };
    bool JUMP{ true };

    unsigned char dir{ 0 };

    CInput() = default;
};

struct CLifespan : public Component
{
    sf::Time total{ sf::Time::Zero };
    sf::Time remaining{ sf::Time::Zero };

    CLifespan() = default;
    CLifespan(float t) : total(sf::seconds(t)), remaining{ sf::seconds(t) } {}

};

struct CType : public Component {
    bool DROPABLE{ false };
    bool MENU{ false };
    bool GROUNDED{ false };
    bool CAUGHT{ false };

    CType() = default;
};

#endif //BREAKOUT_COMPONENTS_H
