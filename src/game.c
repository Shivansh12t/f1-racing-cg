#include "game.h"
#include "track.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h> // For INT_MAX

// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0f)

// --- Global Variable Definitions ---
Car playerCar;
int lapStartTimeMs = 0;
int currentLapTimeMs = 0;
int lastLapTimeMs = 0;
int bestLapTimeMs = INT_MAX;
int crossedFinishLineMovingForwardState = 0; // 0 = false, 1 = true


// --- Initialization Function ---
void initGame() {
    initCar(&playerCar);
    lapStartTimeMs = glutGet(GLUT_ELAPSED_TIME);
    currentLapTimeMs = 0;
    lastLapTimeMs = 0;
    bestLapTimeMs = INT_MAX;
    // Set initial state based on starting position (should be before line Z=0)
    crossedFinishLineMovingForwardState = (playerCar.z >= FINISH_LINE_Z &&
                                           playerCar.x >= FINISH_LINE_X_START &&
                                           playerCar.x <= FINISH_LINE_X_END);
    printf("Game Initialized. Car at (%.2f, %.2f). Start time: %dms. Crossed Flag: %d\n",
           playerCar.x, playerCar.z, lapStartTimeMs, crossedFinishLineMovingForwardState);
}

// --- Camera Setup Function --- (No changes needed)
void setupCamera() {
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

// --- Fixed Timestep Update Function --- (Updated Lap Logic)
void updateGame(int value) {
    (void)value; // Mark unused

    int timeNowMs = glutGet(GLUT_ELAPSED_TIME);
    updateCar(&playerCar, FRAME_TIME_SEC);

    // Update Lap Timers
    if (timeNowMs >= lapStartTimeMs) {
        currentLapTimeMs = timeNowMs - lapStartTimeMs;
    } else {
        // Handle potential timer wrap-around (less likely with GLUT_ELAPSED_TIME)
        // or reset during gameplay
        lapStartTimeMs = timeNowMs;
        currentLapTimeMs = 0;
    }

    // --- Lap Completion Logic (Rectangular Track Update) ---
    float carZ = playerCar.z;
    float carPrevZ = playerCar.prev_z;
    float carX = playerCar.x;

    // Check 1: Is the car within the X-bounds of the finish line?
    // Use the definitions from track.h
    int withinFinishLineX = (carX >= FINISH_LINE_X_START && carX <= FINISH_LINE_X_END);

    // Check 2: Is the car generally moving forward (positive Z direction)?
    // Check speed directly for slightly more robustness against Z jitter
    int movingForward = (playerCar.speed > 0.1f); // Check if speed is positive

    // --- Detect Crossing Finish Line FORWARD ---
    // Conditions:
    // a) Previous Z was *before* the line center (FINISH_LINE_Z).
    // b) Current Z is *at or after* the line center.
    // c) Car is generally moving forward.
    // d) Car is within the correct X range for the finish line.
    if (carPrevZ < FINISH_LINE_Z && carZ >= FINISH_LINE_Z && movingForward && withinFinishLineX) {
        // printf("DEBUG: Crossing Fwd Detected (Z:%.2f->%.2f, X:%.2f, State:%d)\n", carPrevZ, carZ, carX, crossedFinishLineMovingForwardState); // Debug

        if (crossedFinishLineMovingForwardState == 1) { // Already past the line once?
            // --- LAP COMPLETED ---
            lastLapTimeMs = currentLapTimeMs;
            // printf("DEBUG: Lap Completed! Time: %d ms\n", lastLapTimeMs); // Debug
            if (lastLapTimeMs > 0 && lastLapTimeMs < bestLapTimeMs) {
                bestLapTimeMs = lastLapTimeMs;
                 // printf("DEBUG: New Best Lap!\n"); // Debug
            }
            lapStartTimeMs = timeNowMs; // Reset timer for the new lap
            currentLapTimeMs = 0;
            // crossedFinishLineMovingForwardState remains 1

        } else {
            // First forward crossing in this cycle. Set flag, reset timer to start timing.
            crossedFinishLineMovingForwardState = 1; // Set flag to true
            lapStartTimeMs = timeNowMs;
            currentLapTimeMs = 0;
            // printf("DEBUG: First Forward Crossing - Timer Reset.\n"); // Debug
        }
    }
    // --- Detect Crossing Finish Line BACKWARD ---
    // Conditions:
    // a) Previous Z was *at or after* the line center.
    // b) Current Z is *before* the line center.
    // c) Car is within the correct X range for the finish line.
    else if (carPrevZ >= FINISH_LINE_Z && carZ < FINISH_LINE_Z && withinFinishLineX) {
        // Reset the flag if crossing backward.
        crossedFinishLineMovingForwardState = 0; // Set flag to false
        // printf("DEBUG: Crossing Backward Detected.\n"); // Debug
    }

    glutPostRedisplay();
    glutTimerFunc(FRAME_TIME_MS, updateGame, 0);
}


// --- Heads-Up Display (HUD) Rendering Function --- (No changes needed)
void renderHUD(int windowWidth, int windowHeight) {
    char hudText[100];

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_FOG_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);

    glColor3f(1.0f, 1.0f, 1.0f);

    int textX = 10;
    int textY = windowHeight - 30;
    int lineHeight = 20;

    // Current Lap Time
    int cur_mins = (currentLapTimeMs / 1000) / 60;
    int cur_secs = (currentLapTimeMs / 1000) % 60;
    int cur_ms = currentLapTimeMs % 1000;
    snprintf(hudText, sizeof(hudText), "Current: %02d:%02d.%03d", cur_mins, cur_secs, cur_ms);
    glRasterPos2i(textX, textY);
    for (char* c = hudText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    textY -= lineHeight;

    // Last Lap Time
    if (lastLapTimeMs > 0) {
        int last_mins = (lastLapTimeMs / 1000) / 60;
        int last_secs = (lastLapTimeMs / 1000) % 60;
        int last_ms = lastLapTimeMs % 1000;
        snprintf(hudText, sizeof(hudText), "Last:    %02d:%02d.%03d", last_mins, last_secs, last_ms);
    } else {
        snprintf(hudText, sizeof(hudText), "Last:    --:--.---");
    }
    glRasterPos2i(textX, textY);
    for (char* c = hudText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    textY -= lineHeight;

    // Best Lap Time
    if (bestLapTimeMs != INT_MAX) {
        int best_mins = (bestLapTimeMs / 1000) / 60;
        int best_secs = (bestLapTimeMs / 1000) % 60;
        int best_ms = bestLapTimeMs % 1000;
        snprintf(hudText, sizeof(hudText), "Best:    %02d:%02d.%03d", best_mins, best_secs, best_ms);
    } else {
        snprintf(hudText, sizeof(hudText), "Best:    --:--.---");
    }
    glRasterPos2i(textX, textY);
     for (char* c = hudText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glPopAttrib();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}