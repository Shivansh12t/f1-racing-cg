#ifndef TRACK_H
#define TRACK_H

// Include game state definitions to potentially use selectedTrackType here if needed
// #include "game.h" // Or manage dependencies differently

// --- Track Dimensions (RECTANGULAR - Keep these) ---
#define RECT_TRACK_MAIN_WIDTH 80.0f
#define RECT_TRACK_MAIN_LENGTH 120.0f
#define RECT_TRACK_ROAD_WIDTH 12.0f
#define RECT_HALF_ROAD_WIDTH (RECT_TRACK_ROAD_WIDTH / 2.0f)
#define RECT_OUTER_X_POS (RECT_TRACK_MAIN_WIDTH / 2.0f)
#define RECT_OUTER_X_NEG (-RECT_TRACK_MAIN_WIDTH / 2.0f)
#define RECT_OUTER_Z_POS (RECT_TRACK_MAIN_LENGTH / 2.0f)
#define RECT_OUTER_Z_NEG (-RECT_TRACK_MAIN_LENGTH / 2.0f)
#define RECT_INNER_X_POS (RECT_OUTER_X_POS - RECT_TRACK_ROAD_WIDTH)
#define RECT_INNER_X_NEG (RECT_OUTER_X_NEG + RECT_TRACK_ROAD_WIDTH)
#define RECT_INNER_Z_POS (RECT_OUTER_Z_POS - RECT_TRACK_ROAD_WIDTH)
#define RECT_INNER_Z_NEG (RECT_OUTER_Z_NEG + RECT_TRACK_ROAD_WIDTH)
#define RECT_FINISH_LINE_X_START RECT_INNER_X_POS
#define RECT_FINISH_LINE_X_END   RECT_OUTER_X_POS

// --- Track Dimensions (ROUNDED CORNERS - Bring these back/adjust) ---
#define ROUND_TRACK_MAIN_WIDTH 80.0f        // Keep overall size similar
#define ROUND_TRACK_MAIN_LENGTH 120.0f
#define ROUND_TRACK_ROAD_WIDTH 12.0f        // Keep road width similar
#define ROUND_HALF_ROAD_WIDTH (ROUND_TRACK_ROAD_WIDTH / 2.0f)
#define ROUND_CORNER_RADIUS (ROUND_TRACK_ROAD_WIDTH * 1.8f) // Centerline radius
#define ROUND_INNER_CORNER_RADIUS (ROUND_CORNER_RADIUS - ROUND_HALF_ROAD_WIDTH)
#define ROUND_OUTER_CORNER_RADIUS (ROUND_CORNER_RADIUS + ROUND_HALF_ROAD_WIDTH)
#define ROUND_STRAIGHT_X_LIMIT (ROUND_TRACK_MAIN_WIDTH / 2.0f - ROUND_CORNER_RADIUS)
#define ROUND_STRAIGHT_Z_LIMIT (ROUND_TRACK_MAIN_LENGTH / 2.0f - ROUND_CORNER_RADIUS)
#define ROUND_CORNER_CENTER_TR_X (ROUND_STRAIGHT_X_LIMIT)
#define ROUND_CORNER_CENTER_TR_Z (ROUND_STRAIGHT_Z_LIMIT)
#define ROUND_CORNER_CENTER_TL_X (-ROUND_STRAIGHT_X_LIMIT)
#define ROUND_CORNER_CENTER_TL_Z (ROUND_STRAIGHT_Z_LIMIT)
#define ROUND_CORNER_CENTER_BL_X (-ROUND_STRAIGHT_X_LIMIT)
#define ROUND_CORNER_CENTER_BL_Z (-ROUND_STRAIGHT_Z_LIMIT)
#define ROUND_CORNER_CENTER_BR_X (ROUND_STRAIGHT_X_LIMIT)
#define ROUND_CORNER_CENTER_BR_Z (-ROUND_STRAIGHT_Z_LIMIT)
#define ROUND_FINISH_LINE_X_START (ROUND_TRACK_MAIN_WIDTH/2.0f - ROUND_HALF_ROAD_WIDTH) // Place on right straight
#define ROUND_FINISH_LINE_X_END   (ROUND_TRACK_MAIN_WIDTH/2.0f + ROUND_HALF_ROAD_WIDTH)

// --- Common Settings ---
#define FINISH_LINE_Z 0.0f // Common Z coordinate for the finish line center
#define FINISH_LINE_THICKNESS 2.0f
#define CORNER_SEGMENTS 25 // Segments per 90-degree corner for rounded track
#define COLLISION_EPSILON 0.2f

// --- Function Declarations ---
void renderTrack(); // Will now depend on selectedTrackType
void renderGuardrails(); // Will now depend on selectedTrackType
int isPositionOnTrack(float x, float z); // Will now depend on selectedTrackType

#endif // TRACK_H