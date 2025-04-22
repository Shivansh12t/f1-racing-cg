#ifndef CAR_H
#define CAR_H

// Basic struct to hold car state
typedef struct {
    // Position
    float x;
    float y; // Usually constant for a flat track
    float z;

    // Orientation (angle around Y-axis in degrees)
    float angle;

    // Physics
    float speed;
    float acceleration_rate;
    float braking_rate;
    float friction;
    float turn_speed;
    float max_speed;
    float max_reverse_speed;

    // Control state
    int accelerating;
    int braking;
    int turning_left;
    int turning_right;

    // Dimensions (for rendering)
    float width;
    float height;
    float length;

} Car;

// Function declarations
void initCar(Car* car);
void updateCar(Car* car, float deltaTime);
void renderCar(const Car* car);
void setCarControls(Car* car, int key, int state); // 1 for down, 0 for up

#endif // CAR_H