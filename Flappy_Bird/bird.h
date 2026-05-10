#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;

class Bird {
private:
    Sprite s_bird;
    Texture textures[3];
    int currentFrame;
    float animTimer;
    
    Vector2f position;
    float start_X, start_Y;
    
    float velocity;
    float gravity;
    float jumpStrength;

public:
    Bird(float x, float y);

    void resetPosition();
    FloatRect getBounds() const;
    Vector2f getPosition() const;
    
    void flap();
    void update(float dt);
    void draw(RenderWindow& win);
};