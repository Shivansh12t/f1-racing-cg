#include "car.h"      // Defines the Car struct and function prototypes
#include "track.h"    // Defines track boundaries and isPositionOnTrack()
#include "game.h"     // Defines selectedTrackType and TrackType enum

#include <GL/glew.h>     // For OpenGL types (indirectly used via GLUT)
#include <GL/freeglut.h> // For rendering primitives like glutSolidCube
#include <math.h>        // For sinf, cosf, fabsf, fmodf, fmaxf, fminf, powf, sqrtf
#include <stdio.h>       // For optional debugging printf statements

// Define M_PI if not already defined by math.h
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// Macro for converting degrees to radians
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0f)


// --- Car Initialization ---
// Sets the initial state of the car based on the selected track.
void initCar(Car* car) {
    // Common initial state
    car->y = 0.25f;      // Half height, sitting on y=0 plane
    car->angle = 0.0f;     // Facing positive Z (generally 'up' the track initially)
    car->speed = 0.0f;

    // --- Set start position based on track type ---
    // This ensures the car starts on a valid part of the chosen track,
    // typically on the starting straight behind the finish line.
    if (selectedTrackType == TRACK_RECT) {
        // Start on the right straight for the rectangular track
        car->x = (RECT_INNER_X_POS + RECT_OUTER_X_POS) / 2.0f; // Center of the right road lane
        car->z = FINISH_LINE_Z - 20.0f; // Start back from the finish line Z coordinate
    } else { // TRACK_ROUNDED
        // Start on the right straight for the rounded track as well
        car->x = ROUND_TRACK_MAIN_WIDTH / 2.0f; // Center X of the right straight section
        car->z = FINISH_LINE_Z - 20.0f; // Start back from the finish line Z coordinate
    }

    // Initialize previous position to the starting position
    car->prev_x = car->x;
    car->prev_z = car->z;

    // --- Physics Parameters ---
    // These values can be tuned to change the car's handling characteristics.
    car->acceleration_rate = 7.0f;  // Units per second^2
    car->braking_rate = 12.0f;      // Units per second^2 (force opposing motion)
    car->friction = 1.8f;           // Drag factor applied when not accelerating/braking
    // Adjust turning speed slightly based on track type for better feel
    car->turn_speed = (selectedTrackType == TRACK_RECT) ? 120.0f : 150.0f; // Slower turn for sharp corners
    car->max_speed = 35.0f;         // Maximum forward speed in units per second
    car->max_reverse_speed = -8.0f; // Maximum reverse speed

    // --- Control State Initialization ---
    // Ensure control flags start as 'off'.
    car->accelerating = 0; // 0 = false, 1 = true
    car->braking = 0;
    car->turning_left = 0;
    car->turning_right = 0;

    // --- Dimensions for Rendering ---
    // Used for scaling the car model.
    car->width = 1.0f;
    car->height = 0.5f;
    car->length = 2.2f;
}


// --- Car Update Logic ---
// Called every frame by updateGame() to calculate physics and collisions.
void updateCar(Car* car, float deltaTime) {
    // Store previous valid position *before* any updates. Used for collision response.
    car->prev_x = car->x;
    car->prev_z = car->z;

    // --- 1. Apply Turning ---
    // Adjust turning rate based on speed (less turning control at high speeds).
    float current_turn_speed = car->turn_speed;
    if (fabsf(car->speed) > 1.0f) { // Only adjust if moving significantly
        // Example of speed-sensitive turning: decrease linearly above 30% max speed
        float speed_factor = 1.0f - (fmaxf(0.0f, fabsf(car->speed) - car->max_speed * 0.3f) / (car->max_speed * 0.7f));
        current_turn_speed *= fmaxf(0.15f, speed_factor); // Ensure a minimum turn rate
    }

    // Apply rotation if turning keys are pressed and the car is moving.
    if (car->turning_left && fabsf(car->speed) > 0.1f) {
        car->angle += current_turn_speed * deltaTime;
    }
    if (car->turning_right && fabsf(car->speed) > 0.1f) {
        car->angle -= current_turn_speed * deltaTime;
    }
    // Keep angle within 0-360 degrees using fmodf.
    car->angle = fmodf(car->angle + 360.0f, 360.0f);


    // --- 2. Apply Acceleration/Braking ---
    // Calculate the net acceleration/deceleration for this frame.
    float effective_accel = 0.0f;
    if (car->accelerating) {
        effective_accel = car->acceleration_rate;
    }
    if (car->braking) {
        // Braking force opposes the current direction of motion.
        if (car->speed > 0.01f) effective_accel -= car->braking_rate;
        else if (car->speed < -0.01f) effective_accel += car->braking_rate; // Braking works in reverse too
    }

    // Update speed based on effective acceleration and time step.
    car->speed += effective_accel * deltaTime;


    // --- 3. Apply Friction ---
    // Apply friction only if the car is coasting (no acceleration or braking input).
    if (!car->accelerating && !car->braking && fabsf(car->speed) > 0.01f) {
        float friction_force = car->friction * deltaTime;
        // Apply friction opposing the direction of motion.
        if (car->speed > 0.0f) {
            car->speed -= friction_force;
            if (car->speed < 0.0f) car->speed = 0.0f; // Prevent friction from reversing direction
        } else {
            car->speed += friction_force;
            if (car->speed > 0.0f) car->speed = 0.0f; // Prevent friction from reversing direction
        }
    }


    // --- 4. Clamp Speed ---
    // Ensure speed stays within the defined maximum forward and reverse limits.
    car->speed = fmaxf(car->max_reverse_speed, fminf(car->max_speed, car->speed));


    // --- 5. Calculate Potential New Position ---
    // Only calculate movement if speed is significant enough to avoid floating point issues.
    if (fabsf(car->speed) > 0.001f) {
        float angle_rad = DEG_TO_RAD(car->angle); // Convert angle to radians for trig functions
        // Calculate change in position based on speed, angle, and time step.
        float dx = car->speed * sinf(angle_rad) * deltaTime;
        float dz = car->speed * cosf(angle_rad) * deltaTime;

        // Determine the potential position for the next frame.
        float potential_x = car->x + dx;
        float potential_z = car->z + dz;

        // --- 6. Collision Detection and Response ---
        // Check if the potential new position is on the track using the
        // track-type-aware function from track.c.
        if (isPositionOnTrack(potential_x, potential_z)) {
            // Position is valid: Update the car's actual position.
            car->x = potential_x;
            car->z = potential_z;
        } else {
            // Collision Occurred!
            // Simple Response: Revert to the last known valid position and stop the car.
            car->x = car->prev_x;
            car->z = car->prev_z;
            car->speed = 0.0f; // Bring car to a complete halt

            // Optional: Add sound effect or visual feedback here later.
            // printf("Collision! Pos:(%.2f, %.2f) Speed set to 0.\n", car->x, car->z); // Debug output
        }
    } else {
         // If speed is near zero, explicitly set it to zero to prevent potential drift.
         car->speed = 0.0f;
    }
}


// --- Car Rendering ---
// Draws the car model (currently a composite cube structure) at its current position and orientation.
void renderCar(const Car* car) {
    glPushMatrix(); // Save the current OpenGL matrix state

    // Apply transformations: Move to car's position and rotate to its angle.
    glTranslatef(car->x, car->y, car->z);
    glRotatef(car->angle, 0.0f, 1.0f, 0.0f); // Rotate around the Y-axis (vertical)

    // --- Car Body (Red) ---
    // Scale and draw the main body cube. Encapsulate in push/pop to isolate scaling.
    glPushMatrix();
    glScalef(car->width, car->height, car->length);
    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    glutSolidCube(1.0f); // Draw a unit cube, scaled by glScalef
    glPopMatrix(); // Restore scale after drawing body


    // --- Wheels (Dark Grey Cubes) ---
    // Define wheel dimensions relative to the car's base dimensions for consistency.
    float wheelRadius = 0.35f * car->height; // Adjust for visual size
    float wheelWidth = 0.15f * car->width;   // Adjust for visual size
    // Calculate wheel positions relative to the car's center.
    float wheelDistX = (car->width / 2.0f) + wheelWidth * 0.5f; // Position slightly outside the body width
    float wheelDistZ = (car->length / 2.0f) * 0.7f; // Position along the car's length

    glColor3f(0.1f, 0.1f, 0.1f); // Dark grey/black color for wheels

    // Draw each wheel as a scaled cube, positioned and rotated appropriately.
    // Front Left
    glPushMatrix();
    glTranslatef(-wheelDistX, 0.0f, wheelDistZ); // Position
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Rotate cube to align with axle direction
    glScalef(wheelWidth, wheelRadius * 2.0f, wheelRadius * 2.0f); // Scale cube to wheel dimensions
    glutSolidCube(1.0f);
    glPopMatrix();

    // Front Right
    glPushMatrix();
    glTranslatef(wheelDistX, 0.0f, wheelDistZ);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(wheelWidth, wheelRadius * 2.0f, wheelRadius * 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Rear Left
    glPushMatrix();
    glTranslatef(-wheelDistX, 0.0f, -wheelDistZ);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(wheelWidth, wheelRadius * 2.0f, wheelRadius * 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Rear Right
    glPushMatrix();
    glTranslatef(wheelDistX, 0.0f, -wheelDistZ);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(wheelWidth, wheelRadius * 2.0f, wheelRadius * 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();


    // --- Driver Helmet Indicator (White Cube) ---
    // Draw a small cube to represent the driver's head area.
    glPushMatrix();
    // Position relative to the car's center (adjust Y and Z offsets as needed).
    glTranslatef(0.0f, car->height * 0.6f, -car->length * 0.1f);
    float helmetSize = 0.15f; // Define size for the helmet cube
    glScalef(helmetSize, helmetSize, helmetSize); // Scale the unit cube
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    glutSolidCube(1.0f);
    glPopMatrix();


    glPopMatrix(); // Restore the matrix state from before car transformations
}


// --- Car Control Input ---
// Updates the car's control state flags based on keyboard input.
void setCarControls(Car* car, int key, int state) {
    // 'state' is 1 if the key is pressed down, 0 if released.
    switch (key) {
        case 'w': // Forward acceleration
        case 'W':
            car->accelerating = state;
            // Prevent simultaneous acceleration and braking.
            if(state) car->braking = 0;
            break;
        case 's': // Braking / Reverse
        case 'S':
            car->braking = state;
            // Prevent simultaneous acceleration and braking.
             if(state) car->accelerating = 0;
            break;
        case 'a': // Turn Left
        case 'A':
            car->turning_left = state;
            break;
        case 'd': // Turn Right
        case 'D':
            car->turning_right = state;
            break;
    }
}