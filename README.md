# Computer Graphics Project - F1 Racing Game

## Features
- Immersive F1-style racing game.
- Two unique tracks to choose from and race.
- Heads-Up Display (HUD) showing:
    - Current lap time.
    - Best lap time.
    - Current lap number.
- Controls:
    - Press `R` to reset after a crash.
    - Press `Esc` to navigate menus or exit the game.

## Quickstart

### Prerequisites
Ensure the following are installed on your system:
1. GCC or any compatible C compiler.
2. OpenGL (For Windows installation, refer to [this guide](https://github.com/Shivansh12t/opengl-installation-windows)).
3. `make` build tool.

### Clone the Repository
```bash
git clone https://github.com/Shivansh12t/f1-racing-cg racing-game
cd racing-game
```

### Build the Project
```bash
make
```

### Run the Game
```bash
.\bin\game.exe
```

Here's your **F1 Racing Game Codebase Functionality** organized into a clear **Markdown table**:

---

## Function Overview

| **Category**                  | **Function**                                    | **Description**                                                                                           |
|------------------------------|--------------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| **Main (main.c)**            | `main(int argc, char** argv)`                   | Entry point. Initializes GLUT/GLEW, sets callbacks, enters main loop.                                     |
|                              | `display()`                                     | Renders menu or racing scene. Sets camera and overlays HUD.                                               |
|                              | `reshape(int width, int height)`               | Updates viewport and perspective when window is resized.                                                  |
|                              | `keyboardDown(unsigned char key, int x, int y)` | Handles key presses, routes based on state.                                                               |
|                              | `keyboardUp(unsigned char key, int x, int y)`   | Handles key releases for car controls.                                                                    |
|                              | `specialKeyDown(int key, int x, int y)`         | Handles special key presses (e.g., arrow keys).                                                            |
|                              | `cleanup()`                                     | Cleans up on exit.                                                                                         |
| **Game Logic (game.c)**      | `initGame()`                                    | Initializes car and lap state for selected track.                                                          |
|                              | `startGame(TrackType type)`                     | Starts the game with selected track.                                                                      |
|                              | `setupCamera()`                                 | Configures third-person camera behind the car.                                                             |
|                              | `updateGame(int value)`                         | Game loop callback. Updates car, timers, and lap detection.                                                |
|                              | `switchTrack(TrackType newType)`               | Switches between different track types.                                                                   |
|                              | `renderMenu(int w, int h)`                      | Draws the main menu with options and instructions.                                                         |
|                              | `renderHUD(int w, int h)`                       | Draws lap time and other race metrics as HUD.                                                              |
|                              | `handleMenuKeyPress(unsigned char key)`         | Handles menu input (Enter to start, Esc to quit).                                                          |
|                              | `handleMenuSpecialKey(int key)`                 | Allows track switching with arrow keys in menu.                                                            |
|                              | `handleRacingKeyPress(unsigned char key)`       | Handles in-race controls like WASD, reset, or exit.                                                        |
|                              | `handleRacingSpecialKey(int key)`               | Placeholder for special keys during racing.                                                                |
| **Car Physics (car.c)**      | `initCar(Car* car)`                             | Sets car position, speed, orientation, and control flags.                                                  |
|                              | `calculateCarCorners(...)`                      | Computes corner coordinates for collision detection.                                                       |
|                              | `updateCar(Car* car, float deltaTime)`          | Applies physics: movement, turning, and friction. Checks track collision.                                 |
|                              | `isPositionOnTrack(float x, float z)`           | Verifies if car is within allowed track bounds.                                                            |
|                              | `renderCar(const Car* car)`                     | Draws the car at its current location and rotation.                                                        |
|                              | `setCarControls(Car* car, int key, int state)`  | Updates movement flags based on input.                                                                    |
| **Track Rendering (rect/round)** | `renderRectTrack()` / `renderRoundTrack()`          | Renders full track surface and decorations.                                                        |
|                              | `renderRectGuardrails()` / `renderRoundGuardrails()` | Renders guardrails on the sides of the track.                                                       |
|                              | `isPositionOnRectTrack(float x, float z)`       | Validates car position on rectangular track.                                                               |
|                              | `isPositionOnRoundTrack(float x, float z)`      | Validates car position on circular/rounded track.                                                          |
|                              | `drawWallRect(...)` / `drawWallRound(...)`      | Helper to draw straight or curved wall sections.                                                           |
|                              | `renderCornerLineSegmentRound(...)`             | Draws curved lines for round tracks (edges, markings).                                                     |
|                              | `renderCornerSurfaceSegmentRound(...)`          | Draws curved track surfaces (e.g., corners).                                                               |


## Potential Improvements
1. Fix graphic rendering issues on rounded tracks.
2. Add support for uploading custom maps using a markdown-like format.
3. Introduce multiple cars and AI-controlled racers for enhanced gameplay.
