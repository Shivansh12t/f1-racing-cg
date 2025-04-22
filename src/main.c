#include <stdio.h>       // Standard Input/Output functions (printf)
#include <GL/glew.h>     // OpenGL Extension Wrangler Library (must be included before freeglut)
#include <GL/freeglut.h> // FreeGLUT library for windowing, input, and basic shapes

#include "game.h"       // Includes GameState, TrackType, global variables, core functions (initGame, updateGame, renderMenu, renderHUD, setupCamera, input handlers)
#include "track.h"      // Includes track rendering and collision functions (renderTrack, renderGuardrails, isPositionOnTrack)

// --- Function Prototypes for GLUT Callbacks ---
void display();                          // Main drawing function
void reshape(int width, int height);     // Window resize handler
void keyboardDown(unsigned char key, int x, int y); // Regular key press handler
void keyboardUp(unsigned char key, int x, int y);   // Regular key release handler
void specialKeyDown(int key, int x, int y); // Special key press handler (arrows, etc.)
// void specialKeyUp(int key, int x, int y); // Optional: Special key release handler (not needed currently)
void cleanup();                          // Function called when the GLUT window is closed

// --- Main Application Entry Point ---
int main(int argc, char** argv) {
    // 1. Initialize GLUT
    glutInit(&argc, argv); // Initialize the GLUT library
    // Configure the display mode: Double buffering, RGB color, Depth buffer enabled
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);     // Set initial window dimensions
    glutInitWindowPosition(100, 100);  // Set initial window position
    glutCreateWindow("F1 Racer Prototype"); // Create window with title

    // 2. Initialize GLEW
    // Must be done *after* GLUT window creation and *before* using OpenGL extensions
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW failed to initialize, report error and exit
        fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
        return 1; // Indicate failure
    }
    // Print versions for debugging/information
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    fprintf(stdout, "Status: Using OpenGL %s\n", glGetString(GL_VERSION));


    // 3. Basic OpenGL Setup (Fixed-Function Pipeline settings)
    glEnable(GL_DEPTH_TEST); // Enable depth testing for correct 3D rendering order
    glDepthFunc(GL_LEQUAL);  // Pixels with equal or lesser depth pass the test
    glClearColor(0.1f, 0.3f, 0.7f, 1.0f); // Set the background clear color (sky blue)
    glEnable(GL_CULL_FACE); // Enable face culling for potential performance improvement
    glCullFace(GL_BACK);    // Cull back-facing polygons (polygons facing away from the camera)
    // Optional: Enable alpha blending if needed later for transparency effects
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // 4. Initialize Game State (No longer call initGame here, start in MENU)
    // The game starts in STATE_MENU by default (see game.c global definition).


    // 5. Register GLUT Callback Functions
    glutDisplayFunc(display);           // Set the main drawing function
    glutReshapeFunc(reshape);           // Set the window resize handler
    glutKeyboardFunc(keyboardDown);     // Set the handler for regular ASCII key presses
    glutKeyboardUpFunc(keyboardUp);       // Set the handler for regular key releases
    glutSpecialFunc(specialKeyDown);    // Set the handler for special keys (arrows, F-keys, etc.)
    // glutSpecialUpFunc(specialKeyUp); // Optional: Handler for special key releases
    glutCloseFunc(cleanup);             // Set the function to call when the window is closed


    // 6. Register the Fixed-Update Timer
    // Calls updateGame() after FRAME_TIME_MS milliseconds. updateGame() then reschedules itself.
    glutTimerFunc(FRAME_TIME_MS, updateGame, 0);


    // 7. Enter the GLUT Main Event Loop
    // This starts processing events (input, drawing, timers) and runs until glutLeaveMainLoop() is called.
    glutMainLoop();


    return 0; // Should not be reached if glutMainLoop runs indefinitely
}


// --- GLUT Callback Implementations ---

// Main Drawing Function (Called by GLUT when redraw is needed)
void display() {
    // Clear the color and depth buffers before drawing the new frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Check the current game state and render accordingly
    if (currentGameState == STATE_MENU) {
        // Render the 2D track selection menu
        renderMenu(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    } else { // STATE_RACING
        // --- Render the 3D Racing Scene ---

        // Set up the Projection Matrix (defines the camera's lens/field of view)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity(); // Reset matrix
        // Define perspective projection: FOV angle, aspect ratio, near clip plane, far clip plane
        gluPerspective(50.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 600.0f);

        // Set up the Modelview Matrix (defines camera position/orientation and object transformations)
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity(); // Reset matrix
        setupCamera();    // Position and orient the camera based on the player car

        // Render the 3D track elements (surface, lines, guardrails)
        renderTrack();      // Renders the currently selected track geometry
        renderGuardrails(); // Renders guardrails for the selected track

        // Render the player's car
        renderCar(&playerCar);

        // --- Render the 2D HUD (Heads-Up Display) over the 3D scene ---
        renderHUD(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    }

    // Swap the front and back buffers to display the newly rendered frame
    glutSwapBuffers();
}


// Window Resize Handler (Called by GLUT when the window is resized)
void reshape(int width, int height) {
    // Prevent division by zero if window height becomes zero
    if (height == 0) height = 1;
    // Calculate the new aspect ratio
    float aspect = (float)width / (float)height;

    // Set the OpenGL viewport to cover the entire new window area
    glViewport(0, 0, width, height);

    // Re-apply the perspective projection matrix with the new aspect ratio
    // (This is also done in display(), but good practice to set it here too)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, aspect, 0.1f, 600.0f);

    // Switch back to the Modelview matrix mode for subsequent operations
    glMatrixMode(GL_MODELVIEW);
}


// Regular Key Press Handler (Delegates based on Game State)
void keyboardDown(unsigned char key, int x, int y) {
    (void)x; (void)y; // Mark GLUT mouse coordinates as unused for now

    // Call the appropriate handler function based on the current game state
    if (currentGameState == STATE_MENU) {
        handleMenuKeyPress(key); // Defined in game.c
    } else { // STATE_RACING
        handleRacingKeyPress(key); // Defined in game.c
    }
}


// Regular Key Release Handler (Only relevant for Racing State)
void keyboardUp(unsigned char key, int x, int y) {
    (void)x; (void)y; // Mark unused

    // Only the racing state needs key release information (for car controls)
    if (currentGameState == STATE_RACING) {
        setCarControls(&playerCar, key, 0); // 0 = key up
    }
}


// Special Key Press Handler (Arrows, F-keys, etc. - Delegates based on Game State)
void specialKeyDown(int key, int x, int y) {
    (void)x; (void)y; // Mark unused

    // Call the appropriate handler function based on the current game state
    if (currentGameState == STATE_MENU) {
        handleMenuSpecialKey(key); // Defined in game.c
    } else { // STATE_RACING
        handleRacingSpecialKey(key); // Defined in game.c (currently does nothing)
    }
}


// Cleanup Function (Called when the GLUT window is closed)
void cleanup() {
    // Add any necessary cleanup code here (e.g., freeing dynamically allocated memory)
    // For this simple prototype, just printing a message is sufficient.
    printf("Exiting application...\n");
}