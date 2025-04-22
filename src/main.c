#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "game.h"
#include "track.h"

// Function prototypes
void display();
void reshape(int width, int height);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void cleanup();

int main(int argc, char** argv) {
    // ... (GLUT, GLEW, OpenGL Initialization remains the same) ...
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720); // Slightly wider aspect ratio common for racing
    glutInitWindowPosition(100, 100);
    glutCreateWindow("F1 Racer Prototype - V2");

    GLenum err = glewInit();
    if (GLEW_OK != err) { /* report error */ return 1; }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    fprintf(stdout, "Status: Using OpenGL %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.1f, 0.3f, 0.7f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND); // Enable blending for potential future transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutCloseFunc(cleanup);
    glutTimerFunc(FRAME_TIME_MS, updateGame, 0);

    glutMainLoop();
    return 0;
}

// Display callback
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- 3D Scene Rendering ---
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 500.0f); // Slightly wider FOV

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    setupCamera(); // Position the camera

    renderTrack();      // Draw the track surface and lines
    renderGuardrails(); // Draw the guardrails
    renderCar(&playerCar); // Draw the car

    // --- 2D HUD Rendering ---
    renderHUD(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)); // Draw the HUD on top

    glutSwapBuffers();
}

// Reshape, keyboardDown, keyboardUp, cleanup functions remain the same

void reshape(int width, int height) {
    if (height == 0) height = 1;
    float aspect = (float)width / (float)height;
    glViewport(0, 0, width, height);
    // Projection setup is now done primarily in display() before 3D rendering
    // but setting it here is still good practice for initial setup.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, aspect, 0.1f, 500.0f);
    glMatrixMode(GL_MODELVIEW); // Switch back
}

void keyboardDown(unsigned char key, int x, int y) {
    setCarControls(&playerCar, key, 1);
    if (key == 27) { // ESC
        glutLeaveMainLoop();
    }
     // Optional: Reset key 'r'
    if (key == 'r' || key == 'R') {
        printf("Resetting game state...\n");
        initGame(); // Re-initialize car position and timers
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    setCarControls(&playerCar, key, 0);
}

void cleanup() {
    printf("Exiting application...\n");
}