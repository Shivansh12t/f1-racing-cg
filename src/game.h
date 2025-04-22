#ifndef GAME_H
#define GAME_H

#include "car.h" // Includes necessary type definitions

#define FRAME_RATE 60
#define FRAME_TIME_MS (1000 / FRAME_RATE)
#define FRAME_TIME_SEC (1.0f / FRAME_RATE)

// Global car object
extern Car playerCar;

// Global timer variables (using int for booleans)
extern int lapStartTimeMs;
extern int currentLapTimeMs;
extern int lastLapTimeMs;
extern int bestLapTimeMs;
// Use int for boolean flag: 0 = false, 1 = true
extern int crossedFinishLineMovingForwardState; // Renamed for clarity

// Function declarations
void initGame();
void updateGame(int value);
void setupCamera();
void renderHUD(int windowWidth, int windowHeight);

#endif // GAME_H