#include "car.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h> // For sin, cos
#include <stdio.h> // For debug prints (optional)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0)

void initCar(Car* car) {
    car->x = 0.0f;
    car->y = 0.25f; // Half the height of the cube, sitting on the ground (y=0)
    car->z = 5.0f;  // Start a bit forward on the track
    car->angle = 0.0f; // Facing positive Z initially
    car->speed = 0.0f;

    // --- Physics Parameters (Tune these!) ---
    car->acceleration_rate = 5.0f;  // Units per second^2
    car->braking_rate = 10.0f;      // Units per second^2
    car->friction = 1.5f;           // Coefficient (higher = more friction)
    car->turn_speed = 90.0f;        // Degrees per second
    car->max_speed = 25.0f;         // Units per second
    car->max_reverse_speed = -5.0f; // Units per second (negative)
    // --- --------------------------------- ---

    car->accelerating = 0;
    car->braking = 0;
    car->turning_left = 0;
    car->turning_right = 0;

    // Dimensions for rendering
    car->width = 1.0f;
    car->height = 0.5f;
    car->length = 2.0f;
}

void updateCar(Car* car, float deltaTime) {
    // 1. Apply Turning
    float turn_rate = car->turn_speed;
    // Optional: Reduce turn rate at higher speeds (simple model)
    if (fabs(car->speed) > car->max_speed * 0.5f) {
         turn_rate *= (car->max_speed - fabs(car->speed)) / (car->max_speed * 0.5f);
         if (turn_rate < car->turn_speed * 0.2f) turn_rate = car->turn_speed * 0.2f; // Minimum turn rate
    }

    if (car->turning_left && car->speed != 0) { // Only turn when moving
        car->angle += turn_rate * deltaTime;
    }
    if (car->turning_right && car->speed != 0) {
        car->angle -= turn_rate * deltaTime;
    }
    // Keep angle within 0-360
    if (car->angle > 360.0f) car->angle -= 360.0f;
    if (car->angle < 0.0f) car->angle += 360.0f;

    // 2. Apply Acceleration/Braking
    if (car->accelerating) {
        car->speed += car->acceleration_rate * deltaTime;
    }
    if (car->braking) {
        // Apply braking force relative to current direction
        if (car->speed > 0.0f) {
            car->speed -= car->braking_rate * deltaTime;
        } else if (car->speed < 0.0f) {
            car->speed += car->braking_rate * deltaTime; // Braking also works in reverse
        }
    }

    // 3. Apply Friction
    if (!car->accelerating) { // Only apply friction if not actively accelerating
         if (car->speed > 0.0f) {
            car->speed -= car->friction * deltaTime;
            if (car->speed < 0.0f) car->speed = 0.0f; // Stop if friction overcomes speed
        } else if (car->speed < 0.0f) {
            car->speed += car->friction * deltaTime;
            if (car->speed > 0.0f) car->speed = 0.0f; // Stop if friction overcomes speed
        }
    }

    // 4. Clamp Speed
    if (car->speed > car->max_speed) {
        car->speed = car->max_speed;
    }
    if (car->speed < car->max_reverse_speed) {
        car->speed = car->max_reverse_speed;
    }

    // Prevent jitter when almost stopped
    if (!car->accelerating && !car->braking && fabs(car->speed) < 0.1f) {
        car->speed = 0.0f;
    }

    // 5. Update Position based on Speed and Angle
    float angle_rad = DEG_TO_RAD(car->angle);
    car->x += car->speed * sin(angle_rad) * deltaTime;
    car->z += car->speed * cos(angle_rad) * deltaTime;

    // Debug print (optional)
    // printf("Pos: (%.2f, %.2f) Angle: %.1f Speed: %.2f\n", car->x, car->z, car->angle, car->speed);
}

void renderCar(const Car* car) {
    glPushMatrix(); // Save the current matrix state

    // Apply transformations for the car
    glTranslatef(car->x, car->y, car->z);
    glRotatef(car->angle, 0.0f, 1.0f, 0.0f); // Rotate around the Y-axis

    // Scale to represent F1 car dimensions (long and flat)
    glScalef(car->width, car->height, car->length);

    // Set car color (e.g., red)
    glColor3f(1.0f, 0.0f, 0.0f);

    // Draw a simple cube as the car body
    // glutSolidCube draws a cube centered at the origin
    glutSolidCube(1.0f);

    // Optional: Add a small white cube on top as a "driver helmet" indicator
    glPushMatrix();
    glTranslatef(0.0f, 0.6f, -0.2f); // Position relative to car center
    glScalef(0.3f / car->width, 0.3f / car->height, 0.3f / car->length); // Correct scale back
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();


    glPopMatrix(); // Restore the previous matrix state
}

void setCarControls(Car* car, int key, int state) {
    switch (key) {
        case 'w':
        case 'W':
            car->accelerating = state;
            if(state) car->braking = 0; // Cannot accelerate and brake same time
            break;
        case 's':
        case 'S':
            car->braking = state;
             if(state) car->accelerating = 0; // Cannot accelerate and brake same time
            break;
        case 'a':
        case 'A':
            car->turning_left = state;
            break;
        case 'd':
        case 'D':
            car->turning_right = state;
            break;
    }
}