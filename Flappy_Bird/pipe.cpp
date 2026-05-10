#include "pipe.h"
#include <cmath> 

Pipe::Pipe() {
    speed = 200.0f;
    isPassed = false;
    isOscillating = false;
    lifeTime = 0.0f;
}

void Pipe::spawn(float startX, float anchorY, float offsetY, PipeType pipeType, const Texture& texture, bool oscillate) {
    this->type = pipeType;
    this->anchorY = anchorY;
    this->offsetY = offsetY;
    this->isOscillating = oscillate;
    this->lifeTime = 0.0f;
    this->isPassed = false;

    sprite.setTexture(texture);
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
    sprite.setScale(1.5f, 1.5f);

    if (type == PipeType::Top) sprite.setRotation(180.0f); 
    else sprite.setRotation(0.0f);
    
    sprite.setPosition(startX, anchorY + offsetY);
}

void Pipe::respawn(float newX, float newAnchorY, bool oscillate) {
    anchorY = newAnchorY;
    isOscillating = oscillate;
    lifeTime = 0.0f;
    isPassed = false; 
    
    sprite.setPosition(newX, anchorY + offsetY);
}

void Pipe::update(float dt) {
    sprite.move(-speed * dt, 0); 
    
    if (isOscillating) {
        lifeTime += dt;
        float dynamicAnchor = anchorY + std::sin(lifeTime * 3.0f) * 60.0f; 
        sprite.setPosition(sprite.getPosition().x, dynamicAnchor + offsetY);
    }
}

bool Pipe::collides(const FloatRect& birdBounds) const {
    FloatRect bounds = sprite.getGlobalBounds();
    bounds.left += 5.0f;
    bounds.width -= 10.0f;
    return bounds.intersects(birdBounds);
}

void Pipe::draw(RenderWindow& win) const {
    win.draw(sprite);
}

float Pipe::getX() const { return sprite.getPosition().x; }
float Pipe::getWidth() const { return sprite.getGlobalBounds().width; }