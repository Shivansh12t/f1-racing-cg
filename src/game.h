#ifndef GAME_H
#define GAME_H

#include "car.h"

#define FRAME_RATE 60
#define FRAME_TIME_MS (1000 / FRAME_RATE) // Milliseconds per frame
#define FRAME_TIME_SEC (1.0f / FRAME_RATE) // Seconds per frame (for physics)

// Global car object (simple approach for prototype)
extern Car playerCar;

// Function declarations
void initGame();
void updateGame(int value); // Timer function callback
void setupCamera(); // Sets the view matrix based on car

#endif // GAME_H