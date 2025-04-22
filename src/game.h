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
    // Add more track types here if needed
} TrackType;

// --- Menu Selection ---
#define NUM_TRACK_OPTIONS 2 // Number of selectable tracks

#define FRAME_RATE 60
#define FRAME_TIME_MS (1000 / FRAME_RATE)
#define FRAME_TIME_SEC (1.0f / FRAME_RATE)

// --- Global Variables ---
extern GameState currentGameState;
extern TrackType selectedTrackType; // Track type for the *current* race
extern int menuSelectionIndex; // Which track is highlighted in the menu (0-based)
extern Car playerCar;

// Timer variables
extern int lapStartTimeMs;
extern int currentLapTimeMs;
extern int lastLapTimeMs;
extern int bestLapTimeMs;
extern int crossedFinishLineMovingForwardState;

// --- Function Declarations ---
void initGame();
void updateGame(int value);
void setupCamera();
void renderMenu(int windowWidth, int windowHeight);
void renderHUD(int windowWidth, int windowHeight);
void startGame(TrackType type);
void handleMenuKeyPress(unsigned char key); // Specific handlers
void handleMenuSpecialKey(int key);
void handleRacingKeyPress(unsigned char key);
void handleRacingSpecialKey(int key);


#endif // GAME_H