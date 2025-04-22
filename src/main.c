#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "game.h" // Includes game state, track type, functions etc.
#include "track.h"

// Function prototypes
void display();
void reshape(int width, int height);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void cleanup();

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("F1 Racer Prototype - Track Select"); // New title

    GLenum err = glewInit();
    if (GLEW_OK != err) { fprintf(stderr, "Error: %s\n", glewGetErrorString(err)); return 1; }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    fprintf(stdout, "Status: Using OpenGL %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
    glClearColor(0.1f, 0.3f, 0.7f, 1.0f);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);

    // Don't call initGame() here, start in menu state
    // initGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown); // Handles input based on state
    glutKeyboardUpFunc(keyboardUp);   // Only relevant for racing state
    glutCloseFunc(cleanup);
    glutTimerFunc(FRAME_TIME_MS, updateGame, 0); // Update loop runs always

    glutMainLoop();
    return 0;
}

// --- Display Callback (Checks Game State) ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentGameState == STATE_MENU) {
        renderMenu(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    } else { // STATE_RACING
        // --- 3D Scene Rendering ---
        glMatrixMode(GL_PROJECTION); glLoadIdentity();
        gluPerspective(50.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 600.0f); // Increased far plane slightly

        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        setupCamera();

        renderTrack();      // Renders the selected track
        renderGuardrails(); // Renders guardrails for the selected track
        renderCar(&playerCar);

        // --- 2D HUD Rendering ---
        renderHUD(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    }

    glutSwapBuffers();
}

// --- Reshape Callback --- (No change needed)
void reshape(int width, int height) {
    // ... (same as before) ...
    if (height == 0) height = 1; float aspect = (float)width / (float)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(50.0f, aspect, 0.1f, 600.0f); // Match far plane
    glMatrixMode(GL_MODELVIEW);
}

// --- Keyboard Down Callback (Handles States) ---
void keyboardDown(unsigned char key, int x, int y) {
    if (currentGameState == STATE_MENU) {
        // --- Menu Input Handling ---
        switch (key) {
            case '1':
                startGame(TRACK_RECT); // Select Rectangular, init game, change state
                break;
            case '2':
                startGame(TRACK_ROUNDED); // Select Rounded, init game, change state
                break;
            case 27: // ESC exits from menu too
                printf("ESC pressed in menu. Exiting.\n");
                glutLeaveMainLoop();
                break;
        }
    } else { // STATE_RACING
        // --- Racing Input Handling ---
        setCarControls(&playerCar, key, 1); // Handle W, A, S, D

        switch (key) {
            case 'r':
            case 'R':
                printf("'R' pressed. Resetting game state.\n");
                initGame(); // Re-initialize car and timers for current track
                break;
            case 27: // ESC
                printf("ESC pressed in racing. Returning to Menu.\n");
                // Optionally reset car/timers here, or just switch state
                currentGameState = STATE_MENU;
                // Reset timers explicitly when returning to menu maybe?
                lastLapTimeMs = 0;
                bestLapTimeMs = INT_MAX;
                currentLapTimeMs = 0;
                glutPostRedisplay(); // Make sure menu redraws
                break;
        }
    }
}

// --- Keyboard Up Callback (Only affects Racing State) ---
void keyboardUp(unsigned char key, int x, int y) {
    if (currentGameState == STATE_RACING) {
        setCarControls(&playerCar, key, 0);
    }
}

// --- Cleanup Callback --- (No change needed)
void cleanup() {
    printf("Exiting application...\n");
}