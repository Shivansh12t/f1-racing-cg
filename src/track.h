#ifndef TRACK_H
#define TRACK_H

// --- Track Dimensions (Constants) ---
#define TRACK_WIDTH 10.0f
#define STRAIGHT_LENGTH 60.0f
#define CURVE_RADIUS (TRACK_WIDTH * 1.5f) // Radius of the centerline of the curve

// Calculate inner/outer radii based on centerline and width
#define OUTER_RADIUS (CURVE_RADIUS + TRACK_WIDTH / 2.0f)
#define INNER_RADIUS (CURVE_RADIUS - TRACK_WIDTH / 2.0f)

// Z coordinates for the centers of the curved sections
#define CURVE_CENTER_Z_POS (STRAIGHT_LENGTH / 2.0f)
#define CURVE_CENTER_Z_NEG (-STRAIGHT_LENGTH / 2.0f)

// Define the finish line Z coordinate and width
#define FINISH_LINE_Z 0.0f
#define FINISH_LINE_WIDTH TRACK_WIDTH

// Number of segments for drawing curves smoothly
#define CURVE_SEGMENTS 40 // Increased for smoother curves
#define STRAIGHT_SEGMENTS 10 // Fewer needed for straights

// Collision Epsilon (small tolerance for boundary checks)
#define COLLISION_EPSILON 0.1f

// Function declarations
void renderTrack();
void renderGuardrails();

// Collision checking function (returns int: 1 for on track, 0 for off track)
int isPositionOnTrack(float x, float z);

// Optional helper (remains the same conceptually)
void getClosestPointOnCenterline(float x, float z, float* closestX, float* closestZ);

#endif // TRACK_H