# 🐦 Flapping Bird (C++ / SFML)

A fully featured, object-oriented Flappy Bird clone built from scratch using **C++** and the **SFML** (Simple and Fast Multimedia Library).

This project demonstrates core game development concepts including physics simulation, collision detection, state machines, and file handling. It features dynamic difficulty scaling, persistent high scores, and a robust audio system.

## ✨ Key Features

* **Physics & Mechanics:** Smooth gravity, velocity, and jump mechanics with a responsive, tilt-animated bird sprite.
* **Dynamic Difficulty:** Three selectable difficulty modes (Easy, Medium, Hard). Hard mode introduces vertical sine-wave oscillation to the pipes for an extra challenge!
* **Persistent High Scores:** Utilizes C++ `<fstream>` to save and load high scores locally (`highscore.txt`), ensuring your best runs are never lost.
* **State Management:** Seamless transitions between Start, Playing, Paused, and Game Over states.
* **Audio System:** Fully synchronized SFML audio for flapping, scoring, crashing, and falling, including circuit-breaker logic to prevent audio channel overlapping.
* **Advanced Camera/Views:** Utilizes separate `sf::View` layers to keep the HUD (Score, High Score, UI) perfectly anchored to the screen regardless of world movement.

## 🎮 Controls

* **[ 1, 2, 3 ]** : Select Difficulty (Easy, Medium, Hard) on the start screen.
* **[ Spacebar ]** : Start game / Flap wings.
* **[ Enter / Return ]** : Pause game (while playing) / Restart game (on Game Over screen).
* **[ Esc ]** : Quit the game.

## 🛠️ Build & Run (Linux / Ubuntu)

**Prerequisites:** Ensure you have the SFML library installed on your system.

```bash
sudo apt-get install libsfml-dev

```

**Compilation:**
Navigate to the project directory and compile using `g++`:

```bash
g++ main.cpp bird.cpp pipe.cpp -o play -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

```

**Running the Game:**
*(Note: If you are on Ubuntu 24.04 and experience PipeWire audio delays, force the PulseAudio driver)*

```bash
env ALSOFT_DRIVERS=pulse ./play

```
