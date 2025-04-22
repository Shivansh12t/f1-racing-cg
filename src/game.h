#ifndef GAME_H
#define GAME_H

#include "car.h"

// --- Game States ---
typedef enum {
    STATE_MENU,
    STATE_RACING
} GameState;

// --- Track Types ---
typedef enum {
    TRACK_RECT,      // The sharp-cornered rectangle
    TRACK_ROUNDED    // The rectangle with rounded corners
} TrackType;


#define FRAME_RATE 60
#define FRAME_TIME_MS (1000 / FRAME_RATE)
#define FRAME_TIME_SEC (1.0f / FRAME_RATE)

// --- Global Variables ---
extern GameState currentGameState; // Current state of the game
extern TrackType selectedTrackType; // Which track geometry to use
extern Car playerCar;

// Timer variables (using int for booleans)
extern int lapStartTimeMs;
extern int currentLapTimeMs;
extern int lastLapTimeMs;
extern int bestLapTimeMs;
extern int crossedFinishLineMovingForwardState; // Renamed for clarity

// --- Function Declarations ---
void initGame();          // Initializes variables for the RACING state
void updateGame(int value); // Main update loop
void setupCamera();       // Sets up the 3D camera
void renderMenu(int windowWidth, int windowHeight); // Renders the start menu
void renderHUD(int windowWidth, int windowHeight);  // Renders the racing HUD
void startGame(TrackType type); // Transitions from MENU to RACING

#endif // GAME_H