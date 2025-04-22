#ifndef TRACK_H
#define TRACK_H

// --- Track Dimensions (Simple Rectangle) ---
#define TRACK_MAIN_WIDTH 80.0f        // Outer width of the track area (X-axis)
#define TRACK_MAIN_LENGTH 120.0f       // Outer length of the track area (Z-axis)
#define TRACK_ROAD_WIDTH 12.0f        // Width of the asphalt/road itself

// Calculate derived values
#define HALF_ROAD_WIDTH (TRACK_ROAD_WIDTH / 2.0f)

// Define outer boundaries
#define OUTER_X_POS (TRACK_MAIN_WIDTH / 2.0f)
#define OUTER_X_NEG (-TRACK_MAIN_WIDTH / 2.0f)
#define OUTER_Z_POS (TRACK_MAIN_LENGTH / 2.0f)
#define OUTER_Z_NEG (-TRACK_MAIN_LENGTH / 2.0f)

// Define inner boundaries (the hole in the middle)
#define INNER_X_POS (OUTER_X_POS - TRACK_ROAD_WIDTH)
#define INNER_X_NEG (OUTER_X_NEG + TRACK_ROAD_WIDTH)
#define INNER_Z_POS (OUTER_Z_POS - TRACK_ROAD_WIDTH)
#define INNER_Z_NEG (OUTER_Z_NEG + TRACK_ROAD_WIDTH)

// Define the finish line Z coordinate and width
#define FINISH_LINE_Z 0.0f
#define FINISH_LINE_X_START INNER_X_POS // Place finish line on right straight
#define FINISH_LINE_X_END   OUTER_X_POS
#define FINISH_LINE_THICKNESS 2.0f      // How thick the finish line is along Z

// Collision Epsilon (small tolerance for boundary checks)
#define COLLISION_EPSILON 0.2f // May need slight adjustment

// Function declarations
void renderTrack();
void renderGuardrails();

// Collision checking function (returns int: 1 for on track, 0 for off track)
int isPositionOnTrack(float x, float z);

#endif // TRACK_H