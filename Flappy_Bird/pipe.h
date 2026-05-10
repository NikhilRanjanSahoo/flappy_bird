#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

enum class PipeType { Top, Bottom };

class Pipe {
private:
    Sprite sprite;
    PipeType type;
    float speed;
    
    float anchorY;   
    float offsetY;   
    bool isOscillating;
    float lifeTime; 

public:
    bool isPassed;

    Pipe(); 
    void spawn(float startX, float anchorY, float offsetY, PipeType pipeType, const Texture& texture, bool oscillate);
    void respawn(float newX, float newAnchorY, bool oscillate); 

    void update(float dt);
    void draw(RenderWindow& win) const;
    bool collides(const FloatRect& birdBounds) const;
    
    float getX() const;
    float getWidth() const;
};