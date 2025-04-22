#include "game.h"
#include "track.h" // Includes updated track definitions
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>

// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0f)

// --- Global Variable Definitions ---
GameState currentGameState = STATE_MENU; // Start in the menu
TrackType selectedTrackType = TRACK_RECT; // Default track type
Car playerCar;
int lapStartTimeMs = 0;
int currentLapTimeMs = 0;
int lastLapTimeMs = 0;
int bestLapTimeMs = INT_MAX;
int crossedFinishLineMovingForwardState = 0;

// --- Initialization Function (for RACING state) ---
void initGame() {
    // Initialize the car based on the *selected* track type
    initCar(&playerCar); // initCar itself will check selectedTrackType

    // Initialize lap timing variables *when racing starts*
    lapStartTimeMs = glutGet(GLUT_ELAPSED_TIME);
    currentLapTimeMs = 0;
    lastLapTimeMs = 0;
    bestLapTimeMs = INT_MAX;

    // Set initial finish line state based on car start pos and selected track
    float finishLineXStart, finishLineXEnd;
    if (selectedTrackType == TRACK_RECT) {
        finishLineXStart = RECT_FINISH_LINE_X_START;
        finishLineXEnd = RECT_FINISH_LINE_X_END;
    } else { // TRACK_ROUNDED
        finishLineXStart = ROUND_FINISH_LINE_X_START;
        finishLineXEnd = ROUND_FINISH_LINE_X_END;
    }
    crossedFinishLineMovingForwardState = (playerCar.z >= FINISH_LINE_Z &&
                                           playerCar.x >= finishLineXStart &&
                                           playerCar.x <= finishLineXEnd);

    printf("Game Initialized for Track Type %d. Start time: %dms. Crossed Flag: %d\n",
           selectedTrackType, lapStartTimeMs, crossedFinishLineMovingForwardState);
}

// --- Function to start the game ---
void startGame(TrackType type) {
    printf("Starting game with Track Type %d\n", type);
    selectedTrackType = type;
    initGame(); // Initialize car position, timers for the selected track
    currentGameState = STATE_RACING; // Change state to racing
}

// --- Camera Setup Function --- (No changes needed)
void setupCamera() {
    // ... (same as before) ...
    float followDistance = 9.0f;
    float followHeight = 4.5f;
    float lookAtHeightOffset = 1.0f;
    float carAngleRad = DEG_TO_RAD(playerCar.angle);
    float camX = playerCar.x - followDistance * sinf(carAngleRad);
    float camY = playerCar.y + followHeight;
    float camZ = playerCar.z - followDistance * cosf(carAngleRad);
    float lookAtX = playerCar.x;
    float lookAtY = playerCar.y + lookAtHeightOffset;
    float lookAtZ = playerCar.z;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ, lookAtX, lookAtY, lookAtZ, 0.0f, 1.0f, 0.0f);
}

// --- Fixed Timestep Update Function --- (Add state check)
void updateGame(int value) {
    // --- Only update game logic if in RACING state ---
    if (currentGameState != STATE_RACING) {
        // Reschedule the timer even in menu state to keep it alive
        glutTimerFunc(FRAME_TIME_MS, updateGame, 0);
        // Request redraw to potentially show menu updates/animations later
        glutPostRedisplay();
        return; // Don't process physics, laps etc.
    }
    // --- End of state check ---

    (void)value; // Mark unused

    int timeNowMs = glutGet(GLUT_ELAPSED_TIME);
    updateCar(&playerCar, FRAME_TIME_SEC); // updateCar handles physics and collision

    // Update Lap Timers
    if (timeNowMs >= lapStartTimeMs) {
        currentLapTimeMs = timeNowMs - lapStartTimeMs;
    } else {
        lapStartTimeMs = timeNowMs;
        currentLapTimeMs = 0;
    }

    // Lap Completion Logic (Check selected track type for finish line bounds)
    float carZ = playerCar.z;
    float carPrevZ = playerCar.prev_z;
    float carX = playerCar.x;
    int movingForward = (playerCar.speed > 0.1f);

    float finishLineXStart, finishLineXEnd;
    if (selectedTrackType == TRACK_RECT) {
        finishLineXStart = RECT_FINISH_LINE_X_START;
        finishLineXEnd = RECT_FINISH_LINE_X_END;
    } else { // TRACK_ROUNDED
        finishLineXStart = ROUND_FINISH_LINE_X_START;
        finishLineXEnd = ROUND_FINISH_LINE_X_END;
    }
    int withinFinishLineX = (carX >= finishLineXStart && carX <= finishLineXEnd);


    // Detect Crossing Finish Line FORWARD
    if (carPrevZ < FINISH_LINE_Z && carZ >= FINISH_LINE_Z && movingForward && withinFinishLineX) {
        if (crossedFinishLineMovingForwardState == 1) {
            lastLapTimeMs = currentLapTimeMs;
            if (lastLapTimeMs > 0 && lastLapTimeMs < bestLapTimeMs) {
                bestLapTimeMs = lastLapTimeMs;
            }
            lapStartTimeMs = timeNowMs;
            currentLapTimeMs = 0;
        } else {
            crossedFinishLineMovingForwardState = 1;
            lapStartTimeMs = timeNowMs;
            currentLapTimeMs = 0;
        }
    }
    // Detect Crossing Finish Line BACKWARD
    else if (carPrevZ >= FINISH_LINE_Z && carZ < FINISH_LINE_Z && withinFinishLineX) {
        crossedFinishLineMovingForwardState = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(FRAME_TIME_MS, updateGame, 0);
}

// --- Menu Rendering Function ---
void renderMenu(int windowWidth, int windowHeight) {
    char menuText[100];

    // Switch to Ortho projection (same as HUD)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable 3D effects
    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_FOG_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);

    // Set text color
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow text for menu

    // --- Render Menu Text ---
    int textX = windowWidth / 2 - 150; // Center text roughly
    int textY = windowHeight / 2 + 50;
    int lineHeight = 24;

    snprintf(menuText, sizeof(menuText), "F1 RACER PROTOTYPE");
    glRasterPos2i(textX, textY);
    for (char* c = menuText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c); // Bigger font
    }
    textY -= lineHeight * 2; // Extra space

    glColor3f(1.0f, 1.0f, 1.0f); // White text for options

    snprintf(menuText, sizeof(menuText), "Select Track:");
    glRasterPos2i(textX, textY);
     for (char* c = menuText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    textY -= lineHeight;

    snprintf(menuText, sizeof(menuText), "1: Rectangular Circuit");
    glRasterPos2i(textX + 20, textY); // Indent options
     for (char* c = menuText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    textY -= lineHeight;

    snprintf(menuText, sizeof(menuText), "2: Rounded Circuit");
     glRasterPos2i(textX + 20, textY); // Indent options
     for (char* c = menuText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    textY -= lineHeight * 2;

    snprintf(menuText, sizeof(menuText), "ESC to Exit");
     glRasterPos2i(textX, textY);
     for (char* c = menuText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Restore OpenGL States
    glPopAttrib();

    // Restore Matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


// --- Heads-Up Display (HUD) Rendering Function --- (No changes needed)
void renderHUD(int windowWidth, int windowHeight) {
    // ... (same as before) ...
    char hudText[100];

    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_FOG_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D); glDisable(GL_FOG);
    glColor3f(1.0f, 1.0f, 1.0f);
    int textX = 10; int textY = windowHeight - 30; int lineHeight = 20;

    int cur_mins=(currentLapTimeMs/1000)/60; int cur_secs=(currentLapTimeMs/1000)%60; int cur_ms=currentLapTimeMs%1000;
    snprintf(hudText, sizeof(hudText), "Current: %02d:%02d.%03d", cur_mins, cur_secs, cur_ms);
    glRasterPos2i(textX, textY); for (char* c = hudText; *c != '\0'; c++) { glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c); } textY -= lineHeight;

    if (lastLapTimeMs > 0) { int last_mins=(lastLapTimeMs/1000)/60; int last_secs=(lastLapTimeMs/1000)%60; int last_ms=lastLapTimeMs%1000; snprintf(hudText, sizeof(hudText), "Last:    %02d:%02d.%03d", last_mins, last_secs, last_ms); } else { snprintf(hudText, sizeof(hudText), "Last:    --:--.---"); }
    glRasterPos2i(textX, textY); for (char* c = hudText; *c != '\0'; c++) { glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c); } textY -= lineHeight;

    if (bestLapTimeMs != INT_MAX) { int best_mins=(bestLapTimeMs/1000)/60; int best_secs=(bestLapTimeMs/1000)%60; int best_ms=bestLapTimeMs%1000; snprintf(hudText, sizeof(hudText), "Best:    %02d:%02d.%03d", best_mins, best_secs, best_ms); } else { snprintf(hudText, sizeof(hudText), "Best:    --:--.---"); }
    glRasterPos2i(textX, textY); for (char* c = hudText; *c != '\0'; c++) { glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c); }

    glPopAttrib();
    glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW); glPopMatrix();
}