#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <cstdlib>
#include <ctime>
#include <iostream> 
#include <string> 
#include <fstream>
#include <array>
#include <algorithm> 
#include "bird.h"
#include "pipe.h"

using namespace sf;
using namespace std;

const float WINDOW_WIDTH = 910.0f;
const float WINDOW_HEIGHT = 512.0f;

// ==========================================
// --- FINITE STATE MACHINE ---
// ==========================================
enum class GameState {
    Start,
    Playing,
    Paused,
    Falling, 
    GameOver 
};

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    RenderWindow win(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flapping Bird");
    win.setKeyRepeatEnabled(false); 
    
    View gameView(FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    View hudView(FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    
    Bird bird(100, 256);
    
    // NEW: We replace all 4 booleans with a single state tracker!
    GameState gameState = GameState::Start; 
    
    int difficultyLevel = 1; 
    int score = 0; 
    int highScore = 0; 
    
    ifstream loadFile("highscore.txt");
    if (loadFile.is_open()) {
        loadFile >> highScore; 
        loadFile.close();
    }
    
    Clock clock;

    // --- AUDIO & TEXT SETUP ---
    SoundBuffer buf_wing, buf_swoosh, buf_hit, buf_die, buf_point;
    if (!buf_wing.loadFromFile("audio/wing.ogg") || !buf_swoosh.loadFromFile("audio/swoosh.ogg") ||
        !buf_hit.loadFromFile("audio/hit.ogg") || !buf_die.loadFromFile("audio/die.ogg") ||
        !buf_point.loadFromFile("audio/point.ogg")) {
        cout << "Error loading audio files!" << endl;
    }

    Sound s_wing(buf_wing), s_swoosh(buf_swoosh), s_hit(buf_hit), s_die(buf_die), s_point(buf_point);

    Font font;
    if (!font.loadFromFile("fonts/04b_19.ttf")) cout << "Error loading font!" << endl;
    
    Text scoreText, highScoreText, diffText;
    scoreText.setFont(font);
    scoreText.setString("0"); 
    scoreText.setCharacterSize(50); 
    scoreText.setFillColor(Color::White);
    scoreText.setOutlineColor(Color::Black);
    scoreText.setOutlineThickness(4.0f); 
    scoreText.setPosition(20.0f, 20.0f); 

    highScoreText = scoreText; 
    highScoreText.setString("BEST: " + to_string(highScore)); 
    highScoreText.setCharacterSize(30); 
    highScoreText.setFillColor(Color::Yellow); 
    highScoreText.setPosition(20.0f, 80.0f); 

    diffText = scoreText;
    diffText.setCharacterSize(25);
    diffText.setFillColor(Color::Cyan);
    diffText.setPosition(WINDOW_WIDTH / 2.0f - 180.0f, WINDOW_HEIGHT - 100.0f); 

    // --- TEXTURES ---
    Texture t_bg, t_game_over, t_message, t_pipe;
    t_bg.loadFromFile("graphics/background-day16_9.png");
    t_game_over.loadFromFile("graphics/gameover.png");
    t_message.loadFromFile("graphics/start.png"); 
    t_pipe.loadFromFile("graphics/pipe-green.png");

    Sprite s_bg(t_bg), s_game_over(t_game_over), s_message(t_message);
    s_game_over.setOrigin(t_game_over.getSize().x / 2.0f, t_game_over.getSize().y / 2.0f);
    s_game_over.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
    s_message.setOrigin(t_message.getSize().x / 2.0f, t_message.getSize().y / 2.0f);
    s_message.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

    // --- SETUP PIPE OBJECT POOL ---
    array<Pipe, 6> pipes; 
    float pipeSpacing = 350.0f; 

    auto resetPipes = [&]() {
        float gapSize = (difficultyLevel == 0) ? 220.0f : 140.0f;
        bool oscillate = (difficultyLevel == 2);
        float offset = (gapSize / 2.0f) + ((t_pipe.getSize().y * 1.5f) / 2.0f);

        for (int pair = 0; pair < 3; pair++) {
            float gapY = 150.0f + (rand() % 151); 
            float startX = WINDOW_WIDTH + 100.0f + (pair * pipeSpacing);
            pipes[pair * 2].spawn(startX, gapY, -offset, PipeType::Top, t_pipe, oscillate);
            pipes[pair * 2 + 1].spawn(startX, gapY, offset, PipeType::Bottom, t_pipe, oscillate);
        }
    };
    resetPipes();

    auto checkAndSaveHighScore = [&]() {
        if (score > highScore) {
            highScore = score;
            highScoreText.setString("BEST: " + to_string(highScore));
            ofstream saveFile("highscore.txt");
            if (saveFile.is_open()) {
                saveFile << highScore; 
                saveFile.close();
            }
        }
    };

    // ==========================================
    // --- MAIN GAME LOOP ---
    // ==========================================
    while (win.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        Event event;
        while (win.pollEvent(event)) {
            if (event.type == Event::Closed) win.close();
            
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) win.close();
                
                if (gameState == GameState::Start) {
                    if (event.key.code == Keyboard::Num1 || event.key.code == Keyboard::Numpad1) difficultyLevel = 0;
                    if (event.key.code == Keyboard::Num2 || event.key.code == Keyboard::Numpad2) difficultyLevel = 1;
                    if (event.key.code == Keyboard::Num3 || event.key.code == Keyboard::Numpad3) difficultyLevel = 2;
                }

                if (event.key.code == Keyboard::Space) {
                    if (gameState == GameState::Start || gameState == GameState::Playing) {
                        gameState = GameState::Playing; // Ensure state moves to playing
                        bird.flap();
                        s_wing.play(); 
                    }
                }
                
                if (event.key.code == Keyboard::Return) {
                    // Only allow restart if the bird has hit the floor
                    if (gameState == GameState::GameOver) {
                        gameState = GameState::Start; 
                        score = 0; 
                        scoreText.setString("0"); 
                        bird.resetPosition();
                        resetPipes(); 
                        
                        s_wing.stop();
                        s_swoosh.stop();
                        s_hit.stop();
                        s_die.stop();
                        s_point.stop();
                        s_swoosh.play();
                    } else if (gameState == GameState::Playing) {
                        gameState = GameState::Paused;
                    } else if (gameState == GameState::Paused) {
                        gameState = GameState::Playing;
                    }
                }
            }
        }

        if (difficultyLevel == 0) diffText.setString("MODE: EASY (PRESS 1,2,3)");
        else if (difficultyLevel == 1) diffText.setString("MODE: MEDIUM (PRESS 1,2,3)");
        else if (difficultyLevel == 2) diffText.setString("MODE: HARD (PRESS 1,2,3)");

        if (gameState == GameState::Playing) {
            
            bool oscillate = (difficultyLevel == 2);

            for (int i = 0; i < 6; i++) {
                pipes[i].update(dt);
            }

            if (pipes[0].getX() + pipes[0].getWidth() < 0) {
                float lastX = pipes[4].getX();
                float gapY = 150.0f + (rand() % 151);

                std::rotate(pipes.begin(), pipes.begin() + 2, pipes.end());

                pipes[4].respawn(lastX + pipeSpacing, gapY, oscillate);
                pipes[5].respawn(lastX + pipeSpacing, gapY, oscillate);
            }

            for (int i = 0; i < 6; i += 2) {
                if (!pipes[i].isPassed && bird.getPosition().x > pipes[i].getX() + pipes[i].getWidth()) {
                    pipes[i].isPassed = true;
                    score++;
                    s_point.play();
                    scoreText.setString(to_string(score));
                }

                if (pipes[i].collides(bird.getBounds()) || pipes[i+1].collides(bird.getBounds())) {
                    gameState = GameState::Falling; // Change state to falling
                    
                    s_wing.stop();
                    s_swoosh.stop();
                    s_point.stop();
                    cout << "BOOM! COLLISION DETECTED!" << endl;
                    s_hit.play();      
                    s_die.play();
                    
                    checkAndSaveHighScore(); 
                }
            }
        }

        // The bird only updates gravity if playing or falling
        if (gameState == GameState::Playing || gameState == GameState::Falling) {
            bird.update(dt);
        }

        // Check for floor impact
        if (bird.getPosition().y >= WINDOW_HEIGHT - 20.0f) {
            if (gameState != GameState::GameOver) {
                gameState = GameState::GameOver; // Officially dead
                s_swoosh.play(); 
                
                checkAndSaveHighScore(); 
            }
        }

        // --- RENDERING ---
        win.clear();
        win.setView(gameView);
        win.draw(s_bg);
        for (int i = 0; i < 6; i++) pipes[i].draw(win);
        bird.draw(win);
        
        win.setView(hudView);
        
        // Render UI based strictly on current state
        if (gameState == GameState::GameOver || gameState == GameState::Falling) {
            win.draw(s_game_over);
        } else if (gameState == GameState::Start) {
            win.draw(s_message); 
            win.draw(diffText); 
        } 
        
        win.draw(scoreText);
        win.draw(highScoreText); 
        win.display();
    }
    return 0;
}