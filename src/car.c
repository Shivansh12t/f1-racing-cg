#include "car.h"
#include "track.h" // Need this for collision checking (isPositionOnTrack)
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h> // Optional for debugging

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0f)

void initCar(Car* car) {
    car->x = 0.0f;
    car->y = 0.25f; // Half height, sitting on y=0 plane
    car->z = FINISH_LINE_Z - 5.0f; // Start behind the finish line
    car->angle = 0.0f; // Facing positive Z
    car->speed = 0.0f;

    car->prev_x = car->x;
    car->prev_z = car->z;

    // --- Physics Parameters (Tune these!) ---
    car->acceleration_rate = 7.0f;
    car->braking_rate = 12.0f;
    car->friction = 1.8f; // Slightly adjusted friction
    car->turn_speed = 100.0f; // Slightly adjusted base turn speed
    car->max_speed = 35.0f;
    car->max_reverse_speed = -8.0f;
    // Removed collision_speed_loss_factor, now we just stop.
    // --- --------------------------------- ---

    car->accelerating = 0;
    car->braking = 0;
    car->turning_left = 0;
    car->turning_right = 0;

    // Dimensions for rendering
    car->width = 1.0f;
    car->height = 0.5f;
    car->length = 2.2f;
}

void updateCar(Car* car, float deltaTime) {
    // Store previous valid position *before* any updates
    car->prev_x = car->x;
    car->prev_z = car->z;

    // --- 1. Apply Turning ---
    float current_turn_speed = car->turn_speed;
    if (fabsf(car->speed) > 1.0f) {
        float speed_factor = 1.0f - (fmaxf(0.0f, fabsf(car->speed) - car->max_speed * 0.3f) / (car->max_speed * 0.7f));
        current_turn_speed *= fmaxf(0.15f, speed_factor);
    }

    if (car->turning_left && fabsf(car->speed) > 0.1f) {
        car->angle += current_turn_speed * deltaTime;
    }
    if (car->turning_right && fabsf(car->speed) > 0.1f) {
        car->angle -= current_turn_speed * deltaTime;
    }
    car->angle = fmodf(car->angle + 360.0f, 360.0f);


    // --- 2. Apply Acceleration/Braking ---
    float effective_accel = 0.0f;
    if (car->accelerating) {
        effective_accel = car->acceleration_rate;
    }
    if (car->braking) {
        // Braking opposes current speed direction
        if (car->speed > 0.01f) effective_accel -= car->braking_rate;
        else if (car->speed < -0.01f) effective_accel += car->braking_rate; // Brake works in reverse too
    }

    car->speed += effective_accel * deltaTime;


    // --- 3. Apply Friction ---
    if (!car->accelerating && !car->braking && fabsf(car->speed) > 0.01f) {
        float friction_force = car->friction * deltaTime;
        if (car->speed > 0.0f) {
            car->speed -= friction_force;
            if (car->speed < 0.0f) car->speed = 0.0f; // Don't overshoot zero
        } else {
            car->speed += friction_force;
            if (car->speed > 0.0f) car->speed = 0.0f; // Don't overshoot zero
        }
    }


    // --- 4. Clamp Speed ---
    car->speed = fmaxf(car->max_reverse_speed, fminf(car->max_speed, car->speed));


    // --- 5. Calculate Potential New Position ---
    // Only calculate movement if speed is significant
    if (fabsf(car->speed) > 0.001f) {
        float angle_rad = DEG_TO_RAD(car->angle);
        float dx = car->speed * sinf(angle_rad) * deltaTime;
        float dz = car->speed * cosf(angle_rad) * deltaTime;

        float potential_x = car->x + dx;
        float potential_z = car->z + dz;

        // --- 6. Collision Detection and Response ---
        if (isPositionOnTrack(potential_x, potential_z)) {
            // Move is valid, update position
            car->x = potential_x;
            car->z = potential_z;
        } else {
            // Collision occurred!
            // --- RESPONSE: Revert to previous position AND stop the car ---
            car->x = car->prev_x;
            car->z = car->prev_z;
            car->speed = 0.0f; // Bring car to a halt immediately

             // printf("Collision! Pos:(%.2f, %.2f), Speed: %.2f -> 0\n", car->x, car->z, car->speed); // Debug
        }
    } else {
         // If speed is effectively zero, ensure it *is* zero to prevent drift
         car->speed = 0.0f;
    }
}


void renderCar(const Car* car) {
    glPushMatrix(); // Save the current matrix state

    glTranslatef(car->x, car->y, car->z);
    glRotatef(car->angle, 0.0f, 1.0f, 0.0f);

    // --- Car Body (Red) ---
    // Scale only the body here if wheels are drawn relative to the *unscaled* body space later
    glPushMatrix();
    glScalef(car->width, car->height, car->length);
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidCube(1.0f);
    glPopMatrix(); // Restore scale after drawing body


    // --- Wheels (Smaller Cubes) ---
    // Use absolute sizes or sizes relative to *initial* car dimensions, not the scaled ones.
    float wheelRadius = 0.35f * car->height; // Make radius relative to car height
    float wheelWidth = 0.15f * car->width;  // Make width relative to car width
    float wheelDistX = (car->width / 2.0f) + wheelWidth * 0.5f; // Position wheels outside body width
    float wheelDistZ = (car->length / 2.0f) * 0.7f; // Position wheels along length

    glColor3f(0.1f, 0.1f, 0.1f); // Dark grey/black

    // Front Left
    glPushMatrix();
    glTranslatef(-wheelDistX, 0.0f, wheelDistZ); // Position relative to car center
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Rotate cube to simulate cylinder orientation
    glScalef(wheelWidth, wheelRadius * 2.0f, wheelRadius * 2.0f); // Scale cube to approximate wheel dims
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


    // --- Driver Helmet (White) ---
    glPushMatrix();
    // Position relative to car center (slightly up and back)
    glTranslatef(0.0f, car->height * 0.6f, -car->length * 0.1f);
    float helmetSize = 0.15f; // Define an absolute size or smaller relative size
    glScalef(helmetSize, helmetSize, helmetSize);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();


    glPopMatrix(); // Restore the matrix state from before car transformations
}

// setCarControls remains the same
void setCarControls(Car* car, int key, int state) {
    switch (key) {
        case 'w': case 'W':
            car->accelerating = state;
            if(state) car->braking = 0;
            break;
        case 's': case 'S':
            car->braking = state;
             if(state) car->accelerating = 0;
            break;
        case 'a': case 'A':
            car->turning_left = state;
            break;
        case 'd': case 'D':
            car->turning_right = state;
            break;
    }
}