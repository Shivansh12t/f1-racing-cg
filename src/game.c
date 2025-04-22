#include "game.h"
#include "track.h" // Needed for rendering within display
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h> // For sin/cos in camera

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0)

// Define the global car object
Car playerCar;

void initGame() {
    initCar(&playerCar);
    // Any other game-specific initialization
}

// Function to set up the third-person camera
void setupCamera() {
    // Camera parameters
    float followDistance = 8.0f;
    float followHeight = 4.0f;
    float lookAtHeightOffset = 1.0f; // Look slightly above the car's base

    // Calculate camera position based on car angle and position
    float camX = playerCar.x - followDistance * sin(DEG_TO_RAD(playerCar.angle));
    float camY = playerCar.y + followHeight;
    float camZ = playerCar.z - followDistance * cos(DEG_TO_RAD(playerCar.angle));

    // Calculate the point the camera looks at (the car)
    float lookAtX = playerCar.x;
    float lookAtY = playerCar.y + lookAtHeightOffset;
    float lookAtZ = playerCar.z;

    // Set the view matrix
    gluLookAt(camX, camY, camZ,       // Camera position (eye)
              lookAtX, lookAtY, lookAtZ, // Point to look at (center)
              0.0f, 1.0f, 0.0f);      // Up vector
}


// Fixed timestep update function
void updateGame(int value) {
    // Update car physics and position
    updateCar(&playerCar, FRAME_TIME_SEC);

    // --- Add other game logic updates here (collision detection, AI, etc.) ---

    // Request a redraw of the scene
    glutPostRedisplay();

    // Reschedule the timer for the next frame
    glutTimerFunc(FRAME_TIME_MS, updateGame, 0);
}