#include "track.h"
#include "game.h" // Need access to selectedTrackType
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0f)

// --- Rounded Corner Helpers (Brought back) ---
void renderCornerSegment(float center_x, float center_z, float radius, float start_angle_deg, int num_segments, float y_level) {
    float angle_step = DEG_TO_RAD(90.0f) / num_segments;
    float start_rad = DEG_TO_RAD(start_angle_deg);
    for (int i = 0; i <= num_segments; ++i) {
        float current_angle = start_rad + i * angle_step;
        glVertex3f(center_x + radius * cosf(current_angle), y_level, center_z + radius * sinf(current_angle));
    }
}
void renderCornerQuadStripSegment(float center_x, float center_z, float inner_rad, float outer_rad, float start_angle_deg, int num_segments, float base_y, float top_y) {
     float angle_step = DEG_TO_RAD(90.0f) / num_segments;
     float start_rad = DEG_TO_RAD(start_angle_deg);
     for (int i = 0; i <= num_segments; ++i) {
         float current_angle = start_rad + i * angle_step;
         float cos_a = cosf(current_angle); float sin_a = sinf(current_angle);
         // Inner edge vertex pair (bottom, top)
         glVertex3f(center_x + inner_rad * cos_a, base_y, center_z + inner_rad * sin_a);
         glVertex3f(center_x + inner_rad * cos_a, top_y, center_z + inner_rad * sin_a);
         // Outer edge vertex pair (bottom, top)
         glVertex3f(center_x + outer_rad * cos_a, base_y, center_z + outer_rad * sin_a);
         glVertex3f(center_x + outer_rad * cos_a, top_y, center_z + outer_rad * sin_a);
     }
}

// --- Rectangular Wall Helper (Remains the same) ---
void drawWall(float x1, float z1, float x2, float z2, float height, float thickness) {
    // ... (same as before) ...
    float dx=x2-x1; float dz=z2-z1; float len=sqrtf(dx*dx+dz*dz); if(len<0.001f) return;
    float nx=dx/len; float nz=dz/len; float px=-nz; float pz=nx; float half_thick=thickness/2.0f;
    float v[8][3]={ {x1-px*half_thick,0.0f,z1-pz*half_thick},{x1+px*half_thick,0.0f,z1+pz*half_thick},{x2+px*half_thick,0.0f,z2+pz*half_thick},{x2-px*half_thick,0.0f,z2-pz*half_thick}, {x1-px*half_thick,height,z1-pz*half_thick},{x1+px*half_thick,height,z1+pz*half_thick},{x2+px*half_thick,height,z2+pz*half_thick},{x2-px*half_thick,height,z2-pz*half_thick} };
    glBegin(GL_QUADS);
    glVertex3fv(v[4]);glVertex3fv(v[5]);glVertex3fv(v[6]);glVertex3fv(v[7]); // Top
    glVertex3fv(v[0]);glVertex3fv(v[3]);glVertex3fv(v[7]);glVertex3fv(v[4]); // Front
    glVertex3fv(v[1]);glVertex3fv(v[5]);glVertex3fv(v[6]);glVertex3fv(v[2]); // Back
    glVertex3fv(v[0]);glVertex3fv(v[4]);glVertex3fv(v[5]);glVertex3fv(v[1]); // Left
    glVertex3fv(v[3]);glVertex3fv(v[2]);glVertex3fv(v[6]);glVertex3fv(v[7]); // Right
    glEnd();
}

// --- Track Rendering (Conditional) ---
void renderTrack() {
    float surface_y = 0.0f;
    float line_y = 0.01f;
    float finish_y = 0.02f;

    // --- Render Ground Plane (Common) ---
    glColor3f(0.2f, 0.6f, 0.2f);
    glBegin(GL_QUADS);
        float groundSize = fmaxf(RECT_TRACK_MAIN_WIDTH, RECT_TRACK_MAIN_LENGTH) * 1.2f; // Use rect size for ground
        glVertex3f(-groundSize, -0.02f, -groundSize); glVertex3f(-groundSize, -0.02f,  groundSize);
        glVertex3f( groundSize, -0.02f,  groundSize); glVertex3f( groundSize, -0.02f, -groundSize);
    glEnd();

    // --- Render Track Surface (Conditional) ---
    glColor3f(0.4f, 0.4f, 0.45f); // Asphalt color

    if (selectedTrackType == TRACK_RECT) {
        // --- Rectangular Surface ---
        // Top strip
        glBegin(GL_QUADS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_INNER_Z_POS); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_INNER_Z_POS); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_OUTER_Z_POS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_OUTER_Z_POS); glEnd();
        // Bottom strip
        glBegin(GL_QUADS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_OUTER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_OUTER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_INNER_Z_NEG); glEnd();
        // Left strip
        glBegin(GL_QUADS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_INNER_X_NEG, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_INNER_X_NEG, surface_y, RECT_INNER_Z_POS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_INNER_Z_POS); glEnd();
        // Right strip
        glBegin(GL_QUADS); glVertex3f(RECT_INNER_X_POS, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_INNER_Z_POS); glVertex3f(RECT_INNER_X_POS, surface_y, RECT_INNER_Z_POS); glEnd();
    } else { // TRACK_ROUNDED
        // --- Rounded Corner Surface ---
        glBegin(GL_QUAD_STRIP);
            // BR Corner -> Bottom Straight Start
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, ROUND_INNER_CORNER_RADIUS, ROUND_OUTER_CORNER_RADIUS, 270.0f, CORNER_SEGMENTS, surface_y, surface_y);
            // BL Corner -> Left Straight Start
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, ROUND_INNER_CORNER_RADIUS, ROUND_OUTER_CORNER_RADIUS, 180.0f, CORNER_SEGMENTS, surface_y, surface_y);
            // TL Corner -> Top Straight Start
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, ROUND_INNER_CORNER_RADIUS, ROUND_OUTER_CORNER_RADIUS, 90.0f, CORNER_SEGMENTS, surface_y, surface_y);
            // TR Corner -> Right Straight Start
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, ROUND_INNER_CORNER_RADIUS, ROUND_OUTER_CORNER_RADIUS, 0.0f, CORNER_SEGMENTS, surface_y, surface_y);
            // Close Loop - Repeat first vertex pair
            float first_angle=DEG_TO_RAD(270.0f); float cos_a=cosf(first_angle); float sin_a=sinf(first_angle);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+ROUND_INNER_CORNER_RADIUS*cos_a, surface_y, ROUND_CORNER_CENTER_BR_Z+ROUND_INNER_CORNER_RADIUS*sin_a);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+ROUND_OUTER_CORNER_RADIUS*cos_a, surface_y, ROUND_CORNER_CENTER_BR_Z+ROUND_OUTER_CORNER_RADIUS*sin_a);
        glEnd();
    }

    // --- Render Track Markings (Conditional) ---
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    if (selectedTrackType == TRACK_RECT) {
        // Outer boundary
        glBegin(GL_LINE_LOOP); glVertex3f(RECT_OUTER_X_NEG, line_y, RECT_OUTER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, line_y, RECT_OUTER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, line_y, RECT_OUTER_Z_POS); glVertex3f(RECT_OUTER_X_NEG, line_y, RECT_OUTER_Z_POS); glEnd();
        // Inner boundary
        glBegin(GL_LINE_LOOP); glVertex3f(RECT_INNER_X_NEG, line_y, RECT_INNER_Z_NEG); glVertex3f(RECT_INNER_X_POS, line_y, RECT_INNER_Z_NEG); glVertex3f(RECT_INNER_X_POS, line_y, RECT_INNER_Z_POS); glVertex3f(RECT_INNER_X_NEG, line_y, RECT_INNER_Z_POS); glEnd();
    } else { // TRACK_ROUNDED
         // Outer boundary
        glBegin(GL_LINE_LOOP);
            renderCornerSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, ROUND_OUTER_CORNER_RADIUS, 270.0f, CORNER_SEGMENTS, line_y);
            renderCornerSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, ROUND_OUTER_CORNER_RADIUS, 180.0f, CORNER_SEGMENTS, line_y);
            renderCornerSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, ROUND_OUTER_CORNER_RADIUS, 90.0f, CORNER_SEGMENTS, line_y);
            renderCornerSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, ROUND_OUTER_CORNER_RADIUS, 0.0f, CORNER_SEGMENTS, line_y);
        glEnd();
        // Inner boundary
        glBegin(GL_LINE_LOOP);
            renderCornerSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, ROUND_INNER_CORNER_RADIUS, 270.0f, CORNER_SEGMENTS, line_y);
            renderCornerSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, ROUND_INNER_CORNER_RADIUS, 180.0f, CORNER_SEGMENTS, line_y);
            renderCornerSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, ROUND_INNER_CORNER_RADIUS, 90.0f, CORNER_SEGMENTS, line_y);
            renderCornerSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, ROUND_INNER_CORNER_RADIUS, 0.0f, CORNER_SEGMENTS, line_y);
        glEnd();
    }

    // --- Finish line (Common logic, different coords) ---
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
        float finishLineXStart, finishLineXEnd;
        if (selectedTrackType == TRACK_RECT) {
            finishLineXStart = RECT_FINISH_LINE_X_START; finishLineXEnd = RECT_FINISH_LINE_X_END;
        } else {
            finishLineXStart = ROUND_FINISH_LINE_X_START; finishLineXEnd = ROUND_FINISH_LINE_X_END;
        }
        glVertex3f(finishLineXStart, finish_y, FINISH_LINE_Z + FINISH_LINE_THICKNESS / 2.0f);
        glVertex3f(finishLineXEnd,   finish_y, FINISH_LINE_Z + FINISH_LINE_THICKNESS / 2.0f);
        glVertex3f(finishLineXEnd,   finish_y, FINISH_LINE_Z - FINISH_LINE_THICKNESS / 2.0f);
        glVertex3f(finishLineXStart, finish_y, FINISH_LINE_Z - FINISH_LINE_THICKNESS / 2.0f);
    glEnd();

    glLineWidth(1.0f);
}


// --- Guardrail Rendering (Conditional) ---
void renderGuardrails() {
    float railHeight = 0.8f;
    float railThickness = 0.4f;
    float margin = 0.15f; // Adjusted margin slightly
    float base_y = 0.0f;
    float top_y = railHeight;
    glColor3f(0.8f, 0.1f, 0.1f);

    if (selectedTrackType == TRACK_RECT) {
        // --- Rectangular Guardrails ---
        float ox1=RECT_OUTER_X_NEG-margin; float oz1=RECT_OUTER_Z_NEG-margin; float ox2=RECT_OUTER_X_POS+margin; float oz2=RECT_OUTER_Z_NEG-margin;
        float ox3=RECT_OUTER_X_POS+margin; float oz3=RECT_OUTER_Z_POS+margin; float ox4=RECT_OUTER_X_NEG-margin; float oz4=RECT_OUTER_Z_POS+margin;
        drawWall(ox1, oz1, ox2, oz2, railHeight, railThickness); drawWall(ox2, oz2, ox3, oz3, railHeight, railThickness);
        drawWall(ox3, oz3, ox4, oz4, railHeight, railThickness); drawWall(ox4, oz4, ox1, oz1, railHeight, railThickness);

        float ix1=RECT_INNER_X_NEG+margin; float iz1=RECT_INNER_Z_NEG+margin; float ix2=RECT_INNER_X_POS-margin; float iz2=RECT_INNER_Z_NEG+margin;
        float ix3=RECT_INNER_X_POS-margin; float iz3=RECT_INNER_Z_POS-margin; float ix4=RECT_INNER_X_NEG+margin; float iz4=RECT_INNER_Z_POS-margin;
        drawWall(ix1, iz1, ix2, iz2, railHeight, railThickness); drawWall(ix2, iz2, ix3, iz3, railHeight, railThickness);
        drawWall(ix3, iz3, ix4, iz4, railHeight, railThickness); drawWall(ix4, iz4, ix1, iz1, railHeight, railThickness);
    } else { // TRACK_ROUNDED
        // --- Rounded Guardrails ---
        float outerRailInnerRadius = ROUND_OUTER_CORNER_RADIUS + margin - railThickness/2.0f; // Adjust radii for thickness
        float outerRailOuterRadius = ROUND_OUTER_CORNER_RADIUS + margin + railThickness/2.0f;
        float innerRailInnerRadius = fmaxf(0.1f, ROUND_INNER_CORNER_RADIUS - margin - railThickness/2.0f);
        float innerRailOuterRadius = fmaxf(0.1f, ROUND_INNER_CORNER_RADIUS - margin + railThickness/2.0f);

        // Outer Guardrail (using QUAD_STRIP centered on the target line)
        glBegin(GL_QUAD_STRIP);
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, outerRailInnerRadius, outerRailOuterRadius, 270.0f, CORNER_SEGMENTS, base_y, top_y);
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, outerRailInnerRadius, outerRailOuterRadius, 180.0f, CORNER_SEGMENTS, base_y, top_y);
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, outerRailInnerRadius, outerRailOuterRadius, 90.0f, CORNER_SEGMENTS, base_y, top_y);
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, outerRailInnerRadius, outerRailOuterRadius, 0.0f, CORNER_SEGMENTS, base_y, top_y);
            // Close Loop
            float first_angle=DEG_TO_RAD(270.0f); float cos_a=cosf(first_angle); float sin_a=sinf(first_angle);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+outerRailInnerRadius*cos_a, base_y, ROUND_CORNER_CENTER_BR_Z+outerRailInnerRadius*sin_a);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+outerRailInnerRadius*cos_a, top_y, ROUND_CORNER_CENTER_BR_Z+outerRailInnerRadius*sin_a);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+outerRailOuterRadius*cos_a, base_y, ROUND_CORNER_CENTER_BR_Z+outerRailOuterRadius*sin_a);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+outerRailOuterRadius*cos_a, top_y, ROUND_CORNER_CENTER_BR_Z+outerRailOuterRadius*sin_a);
        glEnd();

        // Inner Guardrail
         glBegin(GL_QUAD_STRIP);
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, innerRailInnerRadius, innerRailOuterRadius, 270.0f, CORNER_SEGMENTS, base_y, top_y);
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, innerRailInnerRadius, innerRailOuterRadius, 180.0f, CORNER_SEGMENTS, base_y, top_y);
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, innerRailInnerRadius, innerRailOuterRadius, 90.0f, CORNER_SEGMENTS, base_y, top_y);
            renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, innerRailInnerRadius, innerRailOuterRadius, 0.0f, CORNER_SEGMENTS, base_y, top_y);
            // Close Loop
            first_angle=DEG_TO_RAD(270.0f); cos_a=cosf(first_angle); sin_a=sinf(first_angle);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+innerRailInnerRadius*cos_a, base_y, ROUND_CORNER_CENTER_BR_Z+innerRailInnerRadius*sin_a);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+innerRailInnerRadius*cos_a, top_y, ROUND_CORNER_CENTER_BR_Z+innerRailInnerRadius*sin_a);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+innerRailOuterRadius*cos_a, base_y, ROUND_CORNER_CENTER_BR_Z+innerRailOuterRadius*sin_a);
            glVertex3f(ROUND_CORNER_CENTER_BR_X+innerRailOuterRadius*cos_a, top_y, ROUND_CORNER_CENTER_BR_Z+innerRailOuterRadius*sin_a);
        glEnd();
    }
}

// --- Collision Detection (Conditional) ---
int isPositionOnTrack(float x, float z) {
    if (selectedTrackType == TRACK_RECT) {
        // --- Rectangular Collision ---
        float outerXPosEps = RECT_OUTER_X_POS + COLLISION_EPSILON; float outerXNegEps = RECT_OUTER_X_NEG - COLLISION_EPSILON;
        float outerZPosEps = RECT_OUTER_Z_POS + COLLISION_EPSILON; float outerZNegEps = RECT_OUTER_Z_NEG - COLLISION_EPSILON;
        float innerXPosEps = RECT_INNER_X_POS - COLLISION_EPSILON; float innerXNegEps = RECT_INNER_X_NEG + COLLISION_EPSILON;
        float innerZPosEps = RECT_INNER_Z_POS - COLLISION_EPSILON; float innerZNegEps = RECT_INNER_Z_NEG + COLLISION_EPSILON;

        if (x > outerXPosEps || x < outerXNegEps || z > outerZPosEps || z < outerZNegEps) return 0; // Outside outer
        if (x < innerXPosEps && x > innerXNegEps && z < innerZPosEps && z > innerZNegEps) return 0; // Inside inner
        return 1; // On track
    } else { // TRACK_ROUNDED
        // --- Rounded Corner Collision ---
        float absX = fabsf(x); float absZ = fabsf(z);
        float innerRadiusSq = powf(fmaxf(0.0f, ROUND_INNER_CORNER_RADIUS - COLLISION_EPSILON), 2);
        float outerRadiusSq = powf(ROUND_OUTER_CORNER_RADIUS + COLLISION_EPSILON, 2);
        float halfRoadWidthWithEps = ROUND_HALF_ROAD_WIDTH + COLLISION_EPSILON;

        // Check Straight Sections
        if (absX <= ROUND_STRAIGHT_X_LIMIT) { // Horizontal straight zone
            if (absZ <= (ROUND_TRACK_MAIN_LENGTH / 2.0f + halfRoadWidthWithEps) &&
                absZ >= (ROUND_TRACK_MAIN_LENGTH / 2.0f - halfRoadWidthWithEps))
                return 1;
        }
        if (absZ <= ROUND_STRAIGHT_Z_LIMIT) { // Vertical straight zone
             if (absX <= (ROUND_TRACK_MAIN_WIDTH / 2.0f + halfRoadWidthWithEps) &&
                 absX >= (ROUND_TRACK_MAIN_WIDTH / 2.0f - halfRoadWidthWithEps))
                 return 1;
        }

        // Check Corner Sections
        float corner_center_x = 0.0f, corner_center_z = 0.0f; int in_corner_zone = 0;
        if (x > ROUND_STRAIGHT_X_LIMIT && z > ROUND_STRAIGHT_Z_LIMIT) { corner_center_x = ROUND_CORNER_CENTER_TR_X; corner_center_z = ROUND_CORNER_CENTER_TR_Z; in_corner_zone = 1; }
        else if (x < -ROUND_STRAIGHT_X_LIMIT && z > ROUND_STRAIGHT_Z_LIMIT) { corner_center_x = ROUND_CORNER_CENTER_TL_X; corner_center_z = ROUND_CORNER_CENTER_TL_Z; in_corner_zone = 1; }
        else if (x < -ROUND_STRAIGHT_X_LIMIT && z < -ROUND_STRAIGHT_Z_LIMIT) { corner_center_x = ROUND_CORNER_CENTER_BL_X; corner_center_z = ROUND_CORNER_CENTER_BL_Z; in_corner_zone = 1; }
        else if (x > ROUND_STRAIGHT_X_LIMIT && z < -ROUND_STRAIGHT_Z_LIMIT) { corner_center_x = ROUND_CORNER_CENTER_BR_X; corner_center_z = ROUND_CORNER_CENTER_BR_Z; in_corner_zone = 1; }

        if (in_corner_zone) {
            float dx = x - corner_center_x; float dz = z - corner_center_z;
            float dist_sq = dx * dx + dz * dz;
            if (dist_sq >= innerRadiusSq && dist_sq <= outerRadiusSq) return 1;
        }
        return 0; // Off track
    }
}