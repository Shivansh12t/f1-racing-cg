#include <stdio.h>
#include <GL/glew.h>      // Must be included before freeglut.h
#include <GL/freeglut.h>
#include "game.h"         // Includes car.h indirectly
#include "track.h"

// Function prototypes for GLUT callbacks
void display();
void reshape(int width, int height);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void cleanup();

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Double buffered, RGB color, Depth testing
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Basic F1 Racer Prototype");

    // Initialize GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
     fprintf(stdout, "Status: Using OpenGL %s\n", glGetString(GL_VERSION));

    // --- Basic OpenGL Setup ---
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.1f, 0.3f, 0.7f, 1.0f); // Set background color (sky blue)
    glEnable(GL_CULL_FACE); // Optional: Improve performance by not drawing back faces
    glCullFace(GL_BACK);

    // Initialize Game State
    initGame();

    // Register GLUT callbacks
    glutDisplayFunc(display);         // Drawing function
    glutReshapeFunc(reshape);         // Window resize function
    glutKeyboardFunc(keyboardDown);   // Key press function
    glutKeyboardUpFunc(keyboardUp);     // Key release function
    glutCloseFunc(cleanup);           // Function called when window is closed

    // Register the fixed-update timer
    glutTimerFunc(FRAME_TIME_MS, updateGame, 0);

    // Enter the GLUT main event loop
    glutMainLoop();

    return 0; // Should not be reached if glutMainLoop runs indefinitely
}

// --- GLUT Callback Implementations ---

void display() {
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the projection matrix (perspective) - usually done in reshape, but safe here too
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // Reset projection matrix
    gluPerspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 500.0f); // FOV, Aspect Ratio, Near plane, Far plane

    // Set up the view matrix (camera)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Reset modelview matrix
    setupCamera();    // Position the camera based on the car

    // Render the track
    renderTrack();

    // Render the car
    renderCar(&playerCar); // Use the global playerCar

    // Swap the front and back buffers (show the rendered frame)
    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) height = 1; // Prevent divide by zero
    float aspect = (float)width / (float)height;

    // Set the viewport to cover the new window size
    glViewport(0, 0, width, height);

    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1f, 500.0f);

    // Switch back to modelview matrix
    glMatrixMode(GL_MODELVIEW);
}

void keyboardDown(unsigned char key, int x, int y) {
    // Pass key press events to the car controller
    setCarControls(&playerCar, key, 1); // 1 means key is down

    // Exit on ESC key
    if (key == 27) { // 27 is the ASCII code for ESC
        glutLeaveMainLoop();
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    // Pass key release events to the car controller
    setCarControls(&playerCar, key, 0); // 0 means key is up
}

void cleanup() {
    // Add any necessary cleanup code here (e.g., freeing memory if dynamically allocated)
    printf("Exiting application...\n");
}