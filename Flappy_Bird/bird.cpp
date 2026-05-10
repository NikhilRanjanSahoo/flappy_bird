#include "bird.h"

Bird::Bird(float x, float y) {
    start_X = x;
    start_Y = y;
    position.x = x;
    position.y = y;
    
    velocity = 0.0f;
    gravity = 2000.0f;       
    jumpStrength = -600.0f;  

    textures[0].loadFromFile("graphics/yellowbird-downflap.png");
    textures[1].loadFromFile("graphics/yellowbird-midflap.png");
    textures[2].loadFromFile("graphics/yellowbird-upflap.png");
    
    currentFrame = 1; 
    animTimer = 0.0f;

    s_bird.setTexture(textures[currentFrame]);
    s_bird.setOrigin(textures[0].getSize().x / 2.0f, textures[0].getSize().y / 2.0f);
    s_bird.setPosition(position);
}

void Bird::resetPosition() {
    position.x = start_X;
    position.y = start_Y;
    velocity = 0.0f;
    s_bird.setPosition(position);
}

Vector2f Bird::getPosition() const {
    return position;
}

FloatRect Bird::getBounds() const {
    FloatRect bounds = s_bird.getGlobalBounds();
    // FIXED: += pushes the left/top edges INWARD to shrink the box
    bounds.left += 5.0f;
    bounds.width -= 10.0f;
    bounds.top += 5.0f;
    bounds.height -= 10.0f;
    return bounds;
}

void Bird::flap() {
    velocity = jumpStrength;
}

void Bird::update(float dt) {
    velocity += gravity * dt;
    position.y += velocity * dt;
    s_bird.setPosition(position);
    
    animTimer += dt;
    if (animTimer > 0.1f) {
        animTimer = 0.0f;
        currentFrame++;
        if (currentFrame > 2) currentFrame = 0; 
        s_bird.setTexture(textures[currentFrame]);
    }
    
    float angle = velocity * 0.05f;
    if (angle < -25.0f) angle = -25.0f; 
    if (angle > 90.0f) angle = 90.0f;   
    s_bird.setRotation(angle);
}

void Bird::draw(RenderWindow& win) {
    win.draw(s_bird);
}