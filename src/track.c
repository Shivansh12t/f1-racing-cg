#include "track.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h> // Optional: For debug prints

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0f)

// --- Main Rendering Functions (Rewritten for Continuity) ---

void renderTrack() {
    int total_segments = (CURVE_SEGMENTS + STRAIGHT_SEGMENTS) * 2;
    float surface_y = 0.0f;
    float line_y = 0.01f;
    float finish_y = 0.02f;

    // --- Render Ground Plane ---
    glColor3f(0.2f, 0.6f, 0.2f); // Grassy Green
    glBegin(GL_QUADS);
        float groundSize = fmaxf(STRAIGHT_LENGTH, OUTER_RADIUS * 2) * 1.5f;
        glVertex3f(-groundSize, -0.02f, -groundSize);
        glVertex3f(-groundSize, -0.02f,  groundSize);
        glVertex3f( groundSize, -0.02f,  groundSize);
        glVertex3f( groundSize, -0.02f, -groundSize);
    glEnd();

    // --- Render Track Surface (Asphalt Grey) ---
    glColor3f(0.4f, 0.4f, 0.45f);
    glBegin(GL_QUAD_STRIP);
    // Loop through one full circuit
    for (int i = 0; i <= total_segments; ++i) {
        float outer_x, outer_z, inner_x, inner_z;

        // Calculate position based on segment index 'i'
        // Segment mapping:
        // 0 to CURVE_SEGMENTS: Bottom curve (0 to 180 deg)
        // CURVE_SEGMENTS to CURVE_SEGMENTS + STRAIGHT_SEGMENTS: Left straight
        // C+S to 2*C+S: Top curve (180 to 360 deg)
        // 2*C+S to 2*C+2*S: Right straight

        int current_segment = i % total_segments; // Wrap around for the closing vertex

        if (current_segment < CURVE_SEGMENTS) { // Bottom Curve
            float angle_ratio = (float)current_segment / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(angle_ratio * 180.0f); // 0 to 180 deg
            outer_x = OUTER_RADIUS * sinf(angle_rad);
            outer_z = CURVE_CENTER_Z_NEG + OUTER_RADIUS * cosf(angle_rad);
            inner_x = INNER_RADIUS * sinf(angle_rad);
            inner_z = CURVE_CENTER_Z_NEG + INNER_RADIUS * cosf(angle_rad);
        } else if (current_segment < CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Left Straight
            float straight_ratio = (float)(current_segment - CURVE_SEGMENTS) / STRAIGHT_SEGMENTS;
            outer_x = -TRACK_WIDTH / 2.0f; // Constant X for outer edge
            outer_z = CURVE_CENTER_Z_NEG + (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
            inner_x = TRACK_WIDTH / 2.0f;  // Constant X for inner edge
            inner_z = outer_z;
        } else if (current_segment < 2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Top Curve
            float angle_ratio = (float)(current_segment - (CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(180.0f + angle_ratio * 180.0f); // 180 to 360 deg
            outer_x = OUTER_RADIUS * sinf(angle_rad);
            outer_z = CURVE_CENTER_Z_POS + OUTER_RADIUS * cosf(angle_rad);
            inner_x = INNER_RADIUS * sinf(angle_rad);
            inner_z = CURVE_CENTER_Z_POS + INNER_RADIUS * cosf(angle_rad);
        } else { // Right Straight
            float straight_ratio = (float)(current_segment - (2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / STRAIGHT_SEGMENTS;
            outer_x = TRACK_WIDTH / 2.0f; // Constant X for outer edge
             // Go from top Z back down to bottom Z
            outer_z = CURVE_CENTER_Z_POS - (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
            inner_x = -TRACK_WIDTH / 2.0f; // Constant X for inner edge
            inner_z = outer_z;
        }

        // Generate vertices for the quad strip
        glVertex3f(inner_x, surface_y, inner_z);
        glVertex3f(outer_x, surface_y, outer_z);
    }
    glEnd();


    // --- Render Track Markings (White Lines) ---
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    // Outer boundary
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= total_segments; ++i) {
        float x, z;
        int current_segment = i % total_segments;
         if (current_segment < CURVE_SEGMENTS) { // Bottom Curve
            float angle_ratio = (float)current_segment / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(angle_ratio * 180.0f);
            x = OUTER_RADIUS * sinf(angle_rad);
            z = CURVE_CENTER_Z_NEG + OUTER_RADIUS * cosf(angle_rad);
        } else if (current_segment < CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Left Straight
            float straight_ratio = (float)(current_segment - CURVE_SEGMENTS) / STRAIGHT_SEGMENTS;
            x = -TRACK_WIDTH / 2.0f; // Outer boundary is left side here
            z = CURVE_CENTER_Z_NEG + (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
        } else if (current_segment < 2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Top Curve
            float angle_ratio = (float)(current_segment - (CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(180.0f + angle_ratio * 180.0f);
            x = OUTER_RADIUS * sinf(angle_rad);
            z = CURVE_CENTER_Z_POS + OUTER_RADIUS * cosf(angle_rad);
        } else { // Right Straight
            float straight_ratio = (float)(current_segment - (2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / STRAIGHT_SEGMENTS;
            x = TRACK_WIDTH / 2.0f; // Outer boundary is right side here
            z = CURVE_CENTER_Z_POS - (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
        }
        glVertex3f(x, line_y, z);
    }
    glEnd();

    // Inner boundary
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= total_segments; ++i) {
        float x, z;
        int current_segment = i % total_segments;
        if (current_segment < CURVE_SEGMENTS) { // Bottom Curve
            float angle_ratio = (float)current_segment / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(angle_ratio * 180.0f);
            x = INNER_RADIUS * sinf(angle_rad);
            z = CURVE_CENTER_Z_NEG + INNER_RADIUS * cosf(angle_rad);
        } else if (current_segment < CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Left Straight
            float straight_ratio = (float)(current_segment - CURVE_SEGMENTS) / STRAIGHT_SEGMENTS;
            x = TRACK_WIDTH / 2.0f; // Inner boundary is right side here
            z = CURVE_CENTER_Z_NEG + (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
        } else if (current_segment < 2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Top Curve
            float angle_ratio = (float)(current_segment - (CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(180.0f + angle_ratio * 180.0f);
            x = INNER_RADIUS * sinf(angle_rad);
            z = CURVE_CENTER_Z_POS + INNER_RADIUS * cosf(angle_rad);
        } else { // Right Straight
            float straight_ratio = (float)(current_segment - (2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / STRAIGHT_SEGMENTS;
            x = -TRACK_WIDTH / 2.0f; // Inner boundary is left side here
            z = CURVE_CENTER_Z_POS - (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
        }
        glVertex3f(x, line_y, z);
    }
    glEnd();

    // --- Start/Finish line ---
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
        glVertex3f(-FINISH_LINE_WIDTH / 2.0f, finish_y, FINISH_LINE_Z);
        glVertex3f( FINISH_LINE_WIDTH / 2.0f, finish_y, FINISH_LINE_Z);
        glVertex3f( FINISH_LINE_WIDTH / 2.0f, finish_y, FINISH_LINE_Z - 1.5f);
        glVertex3f(-FINISH_LINE_WIDTH / 2.0f, finish_y, FINISH_LINE_Z - 1.5f);
    glEnd();

    glLineWidth(1.0f); // Reset line width
}


void renderGuardrails() {
    float railHeight = 0.8f;
    glColor3f(0.8f, 0.1f, 0.1f); // Red guardrails
    float margin = 0.2f;         // How far outside the track line the rail starts
    float base_y = 0.0f;
    float top_y = railHeight;
    int total_segments = (CURVE_SEGMENTS + STRAIGHT_SEGMENTS) * 2;


    // --- Outer Guardrail ---
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= total_segments; ++i) {
        float x, z;
        int current_segment = i % total_segments;
        float r = OUTER_RADIUS + margin; // Radius for outer rail

         if (current_segment < CURVE_SEGMENTS) { // Bottom Curve
            float angle_ratio = (float)current_segment / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(angle_ratio * 180.0f);
            x = r * sinf(angle_rad);
            z = CURVE_CENTER_Z_NEG + r * cosf(angle_rad);
        } else if (current_segment < CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Left Straight
            float straight_ratio = (float)(current_segment - CURVE_SEGMENTS) / STRAIGHT_SEGMENTS;
            x = -(TRACK_WIDTH / 2.0f) - margin; // Outer rail is left side here
            z = CURVE_CENTER_Z_NEG + (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
        } else if (current_segment < 2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Top Curve
            float angle_ratio = (float)(current_segment - (CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(180.0f + angle_ratio * 180.0f);
            x = r * sinf(angle_rad);
            z = CURVE_CENTER_Z_POS + r * cosf(angle_rad);
        } else { // Right Straight
            float straight_ratio = (float)(current_segment - (2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / STRAIGHT_SEGMENTS;
            x = (TRACK_WIDTH / 2.0f) + margin; // Outer rail is right side here
            z = CURVE_CENTER_Z_POS - (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
        }
        glVertex3f(x, base_y, z); // Base vertex
        glVertex3f(x, top_y, z);  // Top vertex
    }
    glEnd();

    // --- Inner Guardrail ---
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= total_segments; ++i) {
        float x, z;
        int current_segment = i % total_segments;
        float r = INNER_RADIUS - margin; // Radius for inner rail
        if (r < 0.1f) r = 0.1f; // Prevent negative radius

         if (current_segment < CURVE_SEGMENTS) { // Bottom Curve
            float angle_ratio = (float)current_segment / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(angle_ratio * 180.0f);
            x = r * sinf(angle_rad);
            z = CURVE_CENTER_Z_NEG + r * cosf(angle_rad);
        } else if (current_segment < CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Left Straight
            float straight_ratio = (float)(current_segment - CURVE_SEGMENTS) / STRAIGHT_SEGMENTS;
            x = (TRACK_WIDTH / 2.0f) + margin; // Inner rail is right side here
            z = CURVE_CENTER_Z_NEG + (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
        } else if (current_segment < 2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS) { // Top Curve
            float angle_ratio = (float)(current_segment - (CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / CURVE_SEGMENTS;
            float angle_rad = DEG_TO_RAD(180.0f + angle_ratio * 180.0f);
            x = r * sinf(angle_rad);
            z = CURVE_CENTER_Z_POS + r * cosf(angle_rad);
        } else { // Right Straight
            float straight_ratio = (float)(current_segment - (2 * CURVE_SEGMENTS + STRAIGHT_SEGMENTS)) / STRAIGHT_SEGMENTS;
            x = -(TRACK_WIDTH / 2.0f) - margin; // Inner rail is left side here
            z = CURVE_CENTER_Z_POS - (CURVE_CENTER_Z_POS - CURVE_CENTER_Z_NEG) * straight_ratio;
        }
        glVertex3f(x, base_y, z); // Base vertex
        glVertex3f(x, top_y, z);  // Top vertex
    }
    glEnd();
}


// --- Collision Detection ---

// Returns 1 if on track, 0 if off track.
int isPositionOnTrack(float x, float z) {
    // Add epsilon for tolerance
    float halfWidthPlusEps = (TRACK_WIDTH / 2.0f) + COLLISION_EPSILON;
    float innerRadiusMinusEpsSq = powf(fmaxf(0.0f, INNER_RADIUS - COLLISION_EPSILON), 2);
    float outerRadiusPlusEpsSq = powf(OUTER_RADIUS + COLLISION_EPSILON, 2);


    // Check straight sections
    if (z > CURVE_CENTER_Z_NEG && z < CURVE_CENTER_Z_POS) {
        return fabsf(x) <= halfWidthPlusEps; // Use tolerance
    }
    // Check curved sections
    else {
        float dist_sq;
        float center_z;

        if (z >= CURVE_CENTER_Z_POS) { // Top curve area
            center_z = CURVE_CENTER_Z_POS;
        } else { // Bottom curve area (z <= CURVE_CENTER_Z_NEG)
            center_z = CURVE_CENTER_Z_NEG;
        }

        float dx = x;
        float dz = z - center_z;
        dist_sq = dx * dx + dz * dz;

        // Check distance against radii with tolerance
        return dist_sq >= innerRadiusMinusEpsSq && dist_sq <= outerRadiusPlusEpsSq;
    }
}

// --- Optional Helper --- (No change needed)
void getClosestPointOnCenterline(float x, float z, float* closestX, float* closestZ) {
     // Check straight section
    if (z > CURVE_CENTER_Z_NEG && z < CURVE_CENTER_Z_POS) {
        *closestX = 0.0f;
        *closestZ = z;
    }
    // Check curved sections
    else {
        float center_z;
        if (z >= CURVE_CENTER_Z_POS) {
            center_z = CURVE_CENTER_Z_POS;
        } else {
             center_z = CURVE_CENTER_Z_NEG;
        }
        float dx = x;
        float dz = z - center_z;
        float dist = sqrtf(dx * dx + dz * dz);
        if (dist < 0.001f) {
            *closestX = 0.0f;
             *closestZ = center_z + ( (center_z == CURVE_CENTER_Z_POS) ? -CURVE_RADIUS : CURVE_RADIUS );
        } else {
            *closestX = (dx / dist) * CURVE_RADIUS;
            *closestZ = center_z + (dz / dist) * CURVE_RADIUS;
        }
    }
}