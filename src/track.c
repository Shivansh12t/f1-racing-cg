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

// --- Rounded Corner Helpers (For Lines) ---
void renderCornerLineSegment(float center_x, float center_z, float radius, float start_angle_deg, int num_segments, float y_level) {
    float angle_step = DEG_TO_RAD(90.0f) / num_segments;
    float start_rad = DEG_TO_RAD(start_angle_deg);
    for (int i = 0; i <= num_segments; ++i) { // Include endpoint
        float current_angle = start_rad + i * angle_step;
        glVertex3f(center_x + radius * cosf(current_angle), y_level, center_z + radius * sinf(current_angle));
    }
}

// --- Rounded Corner Helpers (For Quad Strips - Surface/Rails) ---
void renderCornerQuadStripSegment(float center_x, float center_z, float inner_rad, float outer_rad, float start_angle_deg, int num_segments, float base_y, float top_y) {
     float angle_step = DEG_TO_RAD(90.0f) / num_segments;
     float start_rad = DEG_TO_RAD(start_angle_deg);
     for (int i = 0; i <= num_segments; ++i) {
         float current_angle = start_rad + i * angle_step;
         float cos_a = cosf(current_angle); float sin_a = sinf(current_angle);
         // Inner edge vertex pair (bottom, top)
         glVertex3f(center_x + inner_rad * cos_a, base_y, center_z + inner_rad * sin_a);
         if (top_y != base_y) glVertex3f(center_x + inner_rad * cos_a, top_y, center_z + inner_rad * sin_a);
         // Outer edge vertex pair (bottom, top)
         glVertex3f(center_x + outer_rad * cos_a, base_y, center_z + outer_rad * sin_a);
          if (top_y != base_y) glVertex3f(center_x + outer_rad * cos_a, top_y, center_z + outer_rad * sin_a);
     }
}


// --- Rectangular Wall Helper (Remains the same) ---
void drawWall(float x1, float z1, float x2, float z2, float height, float thickness) {
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
        float groundSize = fmaxf(RECT_TRACK_MAIN_WIDTH, RECT_TRACK_MAIN_LENGTH) * 1.2f;
        glVertex3f(-groundSize, -0.02f, -groundSize); glVertex3f(-groundSize, -0.02f,  groundSize);
        glVertex3f( groundSize, -0.02f,  groundSize); glVertex3f( groundSize, -0.02f, -groundSize);
    glEnd();

    // --- Render Track Surface (Conditional) ---
    glColor3f(0.4f, 0.4f, 0.45f); // Asphalt color

    if (selectedTrackType == TRACK_RECT) {
        // --- Rectangular Surface --- (Same as before)
        glBegin(GL_QUADS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_INNER_Z_POS); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_INNER_Z_POS); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_OUTER_Z_POS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_OUTER_Z_POS); glEnd(); // Top
        glBegin(GL_QUADS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_OUTER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_OUTER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_INNER_Z_NEG); glEnd(); // Bottom
        glBegin(GL_QUADS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_INNER_X_NEG, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_INNER_X_NEG, surface_y, RECT_INNER_Z_POS); glVertex3f(RECT_OUTER_X_NEG, surface_y, RECT_INNER_Z_POS); glEnd(); // Left
        glBegin(GL_QUADS); glVertex3f(RECT_INNER_X_POS, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_INNER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, surface_y, RECT_INNER_Z_POS); glVertex3f(RECT_INNER_X_POS, surface_y, RECT_INNER_Z_POS); glEnd(); // Right
    } else { // TRACK_ROUNDED
        // --- Rounded Corner Surface (Revised Method) ---
        // Draw straights as simple quads
        // Top Straight
        glBegin(GL_QUADS);
            glVertex3f(-ROUND_STRAIGHT_X_LIMIT, surface_y, ROUND_TRACK_MAIN_LENGTH / 2.0f - ROUND_HALF_ROAD_WIDTH); // Inner TL
            glVertex3f( ROUND_STRAIGHT_X_LIMIT, surface_y, ROUND_TRACK_MAIN_LENGTH / 2.0f - ROUND_HALF_ROAD_WIDTH); // Inner TR
            glVertex3f( ROUND_STRAIGHT_X_LIMIT, surface_y, ROUND_TRACK_MAIN_LENGTH / 2.0f + ROUND_HALF_ROAD_WIDTH); // Outer TR
            glVertex3f(-ROUND_STRAIGHT_X_LIMIT, surface_y, ROUND_TRACK_MAIN_LENGTH / 2.0f + ROUND_HALF_ROAD_WIDTH); // Outer TL
        glEnd();
         // Bottom Straight
        glBegin(GL_QUADS);
            glVertex3f(-ROUND_STRAIGHT_X_LIMIT, surface_y, -ROUND_TRACK_MAIN_LENGTH / 2.0f + ROUND_HALF_ROAD_WIDTH); // Outer BL
            glVertex3f( ROUND_STRAIGHT_X_LIMIT, surface_y, -ROUND_TRACK_MAIN_LENGTH / 2.0f + ROUND_HALF_ROAD_WIDTH); // Outer BR
            glVertex3f( ROUND_STRAIGHT_X_LIMIT, surface_y, -ROUND_TRACK_MAIN_LENGTH / 2.0f - ROUND_HALF_ROAD_WIDTH); // Inner BR
            glVertex3f(-ROUND_STRAIGHT_X_LIMIT, surface_y, -ROUND_TRACK_MAIN_LENGTH / 2.0f - ROUND_HALF_ROAD_WIDTH); // Inner BL
        glEnd();
        // Left Straight
        glBegin(GL_QUADS);
             glVertex3f(-ROUND_TRACK_MAIN_WIDTH / 2.0f - ROUND_HALF_ROAD_WIDTH, surface_y, -ROUND_STRAIGHT_Z_LIMIT); // Inner BL
             glVertex3f(-ROUND_TRACK_MAIN_WIDTH / 2.0f + ROUND_HALF_ROAD_WIDTH, surface_y, -ROUND_STRAIGHT_Z_LIMIT); // Outer BL
             glVertex3f(-ROUND_TRACK_MAIN_WIDTH / 2.0f + ROUND_HALF_ROAD_WIDTH, surface_y,  ROUND_STRAIGHT_Z_LIMIT); // Outer TL
             glVertex3f(-ROUND_TRACK_MAIN_WIDTH / 2.0f - ROUND_HALF_ROAD_WIDTH, surface_y,  ROUND_STRAIGHT_Z_LIMIT); // Inner TL
        glEnd();
        // Right Straight
        glBegin(GL_QUADS);
             glVertex3f(ROUND_TRACK_MAIN_WIDTH / 2.0f + ROUND_HALF_ROAD_WIDTH, surface_y, -ROUND_STRAIGHT_Z_LIMIT); // Outer BR
             glVertex3f(ROUND_TRACK_MAIN_WIDTH / 2.0f - ROUND_HALF_ROAD_WIDTH, surface_y, -ROUND_STRAIGHT_Z_LIMIT); // Inner BR
             glVertex3f(ROUND_TRACK_MAIN_WIDTH / 2.0f - ROUND_HALF_ROAD_WIDTH, surface_y,  ROUND_STRAIGHT_Z_LIMIT); // Inner TR
             glVertex3f(ROUND_TRACK_MAIN_WIDTH / 2.0f + ROUND_HALF_ROAD_WIDTH, surface_y,  ROUND_STRAIGHT_Z_LIMIT); // Outer TR
        glEnd();

        // Draw corners using quad strips
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, ROUND_INNER_CORNER_RADIUS, ROUND_OUTER_CORNER_RADIUS, 270.0f, CORNER_SEGMENTS, surface_y, surface_y); glEnd();
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, ROUND_INNER_CORNER_RADIUS, ROUND_OUTER_CORNER_RADIUS, 180.0f, CORNER_SEGMENTS, surface_y, surface_y); glEnd();
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, ROUND_INNER_CORNER_RADIUS, ROUND_OUTER_CORNER_RADIUS, 90.0f, CORNER_SEGMENTS, surface_y, surface_y); glEnd();
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, ROUND_INNER_CORNER_RADIUS, ROUND_OUTER_CORNER_RADIUS, 0.0f, CORNER_SEGMENTS, surface_y, surface_y); glEnd();
    }

    // --- Render Track Markings (Conditional) ---
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    if (selectedTrackType == TRACK_RECT) {
        glBegin(GL_LINE_LOOP); glVertex3f(RECT_OUTER_X_NEG, line_y, RECT_OUTER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, line_y, RECT_OUTER_Z_NEG); glVertex3f(RECT_OUTER_X_POS, line_y, RECT_OUTER_Z_POS); glVertex3f(RECT_OUTER_X_NEG, line_y, RECT_OUTER_Z_POS); glEnd();
        glBegin(GL_LINE_LOOP); glVertex3f(RECT_INNER_X_NEG, line_y, RECT_INNER_Z_NEG); glVertex3f(RECT_INNER_X_POS, line_y, RECT_INNER_Z_NEG); glVertex3f(RECT_INNER_X_POS, line_y, RECT_INNER_Z_POS); glVertex3f(RECT_INNER_X_NEG, line_y, RECT_INNER_Z_POS); glEnd();
    } else { // TRACK_ROUNDED
        // Outer boundary
        glBegin(GL_LINE_STRIP); // Use STRIP and connect manually for clarity
            glVertex3f( ROUND_STRAIGHT_X_LIMIT, line_y, ROUND_TRACK_MAIN_LENGTH / 2.0f + ROUND_HALF_ROAD_WIDTH); // TR Straight End
            renderCornerLineSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, ROUND_OUTER_CORNER_RADIUS, 0.0f, CORNER_SEGMENTS, line_y); // TR Corner
            glVertex3f(-ROUND_STRAIGHT_X_LIMIT, line_y, ROUND_TRACK_MAIN_LENGTH / 2.0f + ROUND_HALF_ROAD_WIDTH); // TL Straight Start
            renderCornerLineSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, ROUND_OUTER_CORNER_RADIUS, 90.0f, CORNER_SEGMENTS, line_y); // TL Corner
            glVertex3f(-ROUND_TRACK_MAIN_WIDTH / 2.0f + ROUND_HALF_ROAD_WIDTH, line_y, -ROUND_STRAIGHT_Z_LIMIT); // BL Straight End
            renderCornerLineSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, ROUND_OUTER_CORNER_RADIUS, 180.0f, CORNER_SEGMENTS, line_y); // BL Corner
            glVertex3f( ROUND_STRAIGHT_X_LIMIT, line_y, -ROUND_TRACK_MAIN_LENGTH / 2.0f + ROUND_HALF_ROAD_WIDTH); // BR Straight Start
            renderCornerLineSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, ROUND_OUTER_CORNER_RADIUS, 270.0f, CORNER_SEGMENTS, line_y); // BR Corner
            glVertex3f(ROUND_TRACK_MAIN_WIDTH / 2.0f + ROUND_HALF_ROAD_WIDTH, line_y, ROUND_STRAIGHT_Z_LIMIT); // Connect back to start of TR straight
        glEnd();
         // Inner boundary
        glBegin(GL_LINE_STRIP);
            glVertex3f( ROUND_STRAIGHT_X_LIMIT, line_y, ROUND_TRACK_MAIN_LENGTH / 2.0f - ROUND_HALF_ROAD_WIDTH); // TR Straight End Inner
            renderCornerLineSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, ROUND_INNER_CORNER_RADIUS, 0.0f, CORNER_SEGMENTS, line_y); // TR Corner Inner
            glVertex3f(-ROUND_STRAIGHT_X_LIMIT, line_y, ROUND_TRACK_MAIN_LENGTH / 2.0f - ROUND_HALF_ROAD_WIDTH); // TL Straight Start Inner
            renderCornerLineSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, ROUND_INNER_CORNER_RADIUS, 90.0f, CORNER_SEGMENTS, line_y); // TL Corner Inner
            glVertex3f(-ROUND_TRACK_MAIN_WIDTH / 2.0f - ROUND_HALF_ROAD_WIDTH, line_y, -ROUND_STRAIGHT_Z_LIMIT); // BL Straight End Inner
            renderCornerLineSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, ROUND_INNER_CORNER_RADIUS, 180.0f, CORNER_SEGMENTS, line_y); // BL Corner Inner
             glVertex3f( ROUND_STRAIGHT_X_LIMIT, line_y, -ROUND_TRACK_MAIN_LENGTH / 2.0f - ROUND_HALF_ROAD_WIDTH); // BR Straight Start Inner
            renderCornerLineSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, ROUND_INNER_CORNER_RADIUS, 270.0f, CORNER_SEGMENTS, line_y); // BR Corner Inner
            glVertex3f(ROUND_TRACK_MAIN_WIDTH / 2.0f - ROUND_HALF_ROAD_WIDTH, line_y, ROUND_STRAIGHT_Z_LIMIT); // Connect back to start of TR straight Inner
        glEnd();
    }

    // --- Finish line (Common logic, different coords) ---
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
        float finishLineXStart, finishLineXEnd;
        if (selectedTrackType == TRACK_RECT) { finishLineXStart = RECT_FINISH_LINE_X_START; finishLineXEnd = RECT_FINISH_LINE_X_END; }
        else { finishLineXStart = ROUND_FINISH_LINE_X_START; finishLineXEnd = ROUND_FINISH_LINE_X_END; }
        glVertex3f(finishLineXStart, finish_y, FINISH_LINE_Z + FINISH_LINE_THICKNESS / 2.0f); glVertex3f(finishLineXEnd, finish_y, FINISH_LINE_Z + FINISH_LINE_THICKNESS / 2.0f);
        glVertex3f(finishLineXEnd, finish_y, FINISH_LINE_Z - FINISH_LINE_THICKNESS / 2.0f); glVertex3f(finishLineXStart, finish_y, FINISH_LINE_Z - FINISH_LINE_THICKNESS / 2.0f);
    glEnd();

    glLineWidth(1.0f);
}

// --- Guardrail Rendering (Conditional) ---
void renderGuardrails() {
    float railHeight = 0.8f;
    float railThickness = 0.4f;
    float margin = 0.15f;
    float base_y = 0.0f;
    float top_y = railHeight;
    glColor3f(0.8f, 0.1f, 0.1f);

    if (selectedTrackType == TRACK_RECT) {
        // --- Rectangular Guardrails --- (Same as before)
        float ox1=RECT_OUTER_X_NEG-margin; float oz1=RECT_OUTER_Z_NEG-margin; float ox2=RECT_OUTER_X_POS+margin; float oz2=RECT_OUTER_Z_NEG-margin; float ox3=RECT_OUTER_X_POS+margin; float oz3=RECT_OUTER_Z_POS+margin; float ox4=RECT_OUTER_X_NEG-margin; float oz4=RECT_OUTER_Z_POS+margin;
        drawWall(ox1, oz1, ox2, oz2, railHeight, railThickness); drawWall(ox2, oz2, ox3, oz3, railHeight, railThickness); drawWall(ox3, oz3, ox4, oz4, railHeight, railThickness); drawWall(ox4, oz4, ox1, oz1, railHeight, railThickness);
        float ix1=RECT_INNER_X_NEG+margin; float iz1=RECT_INNER_Z_NEG+margin; float ix2=RECT_INNER_X_POS-margin; float iz2=RECT_INNER_Z_NEG+margin; float ix3=RECT_INNER_X_POS-margin; float iz3=RECT_INNER_Z_POS-margin; float ix4=RECT_INNER_X_NEG+margin; float iz4=RECT_INNER_Z_POS-margin;
        drawWall(ix1, iz1, ix2, iz2, railHeight, railThickness); drawWall(ix2, iz2, ix3, iz3, railHeight, railThickness); drawWall(ix3, iz3, ix4, iz4, railHeight, railThickness); drawWall(ix4, iz4, ix1, iz1, railHeight, railThickness);
    } else { // TRACK_ROUNDED
        // --- Rounded Guardrails ---
        // Draw straight sections using drawWall
        float outerRailY = ROUND_TRACK_MAIN_LENGTH / 2.0f + ROUND_HALF_ROAD_WIDTH + margin;
        float innerRailY = ROUND_TRACK_MAIN_LENGTH / 2.0f - ROUND_HALF_ROAD_WIDTH - margin;
        float outerRailX = ROUND_TRACK_MAIN_WIDTH / 2.0f + ROUND_HALF_ROAD_WIDTH + margin;
        float innerRailX = ROUND_TRACK_MAIN_WIDTH / 2.0f - ROUND_HALF_ROAD_WIDTH - margin;

        // Outer walls
        drawWall(-ROUND_STRAIGHT_X_LIMIT, outerRailY,  ROUND_STRAIGHT_X_LIMIT, outerRailY, railHeight, railThickness); // Top
        drawWall(-ROUND_STRAIGHT_X_LIMIT, -outerRailY, ROUND_STRAIGHT_X_LIMIT, -outerRailY, railHeight, railThickness); // Bottom
        drawWall(outerRailX, -ROUND_STRAIGHT_Z_LIMIT, outerRailX,  ROUND_STRAIGHT_Z_LIMIT, railHeight, railThickness); // Right
        drawWall(-outerRailX, -ROUND_STRAIGHT_Z_LIMIT,-outerRailX,  ROUND_STRAIGHT_Z_LIMIT, railHeight, railThickness); // Left
        // Inner walls
        drawWall(-ROUND_STRAIGHT_X_LIMIT, innerRailY,  ROUND_STRAIGHT_X_LIMIT, innerRailY, railHeight, railThickness); // Top
        drawWall(-ROUND_STRAIGHT_X_LIMIT, -innerRailY, ROUND_STRAIGHT_X_LIMIT, -innerRailY, railHeight, railThickness); // Bottom
        drawWall(innerRailX, -ROUND_STRAIGHT_Z_LIMIT, innerRailX,  ROUND_STRAIGHT_Z_LIMIT, railHeight, railThickness); // Right
        drawWall(-innerRailX, -ROUND_STRAIGHT_Z_LIMIT,-innerRailX,  ROUND_STRAIGHT_Z_LIMIT, railHeight, railThickness); // Left


        // Draw curved sections using QUAD_STRIP helper
        float outerRailInnerRadius = ROUND_OUTER_CORNER_RADIUS + margin - railThickness/2.0f;
        float outerRailOuterRadius = ROUND_OUTER_CORNER_RADIUS + margin + railThickness/2.0f;
        float innerRailInnerRadius = fmaxf(0.1f, ROUND_INNER_CORNER_RADIUS - margin - railThickness/2.0f);
        float innerRailOuterRadius = fmaxf(0.1f, ROUND_INNER_CORNER_RADIUS - margin + railThickness/2.0f);

        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, outerRailInnerRadius, outerRailOuterRadius, 270.0f, CORNER_SEGMENTS, base_y, top_y); glEnd();
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, outerRailInnerRadius, outerRailOuterRadius, 180.0f, CORNER_SEGMENTS, base_y, top_y); glEnd();
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, outerRailInnerRadius, outerRailOuterRadius, 90.0f, CORNER_SEGMENTS, base_y, top_y); glEnd();
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, outerRailInnerRadius, outerRailOuterRadius, 0.0f, CORNER_SEGMENTS, base_y, top_y); glEnd();

        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BR_X, ROUND_CORNER_CENTER_BR_Z, innerRailInnerRadius, innerRailOuterRadius, 270.0f, CORNER_SEGMENTS, base_y, top_y); glEnd();
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_BL_X, ROUND_CORNER_CENTER_BL_Z, innerRailInnerRadius, innerRailOuterRadius, 180.0f, CORNER_SEGMENTS, base_y, top_y); glEnd();
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TL_X, ROUND_CORNER_CENTER_TL_Z, innerRailInnerRadius, innerRailOuterRadius, 90.0f, CORNER_SEGMENTS, base_y, top_y); glEnd();
        glBegin(GL_QUAD_STRIP); renderCornerQuadStripSegment(ROUND_CORNER_CENTER_TR_X, ROUND_CORNER_CENTER_TR_Z, innerRailInnerRadius, innerRailOuterRadius, 0.0f, CORNER_SEGMENTS, base_y, top_y); glEnd();
    }
}


// --- Collision Detection (Conditional) ---
int isPositionOnTrack(float x, float z) {
    if (selectedTrackType == TRACK_RECT) {
        // --- Rectangular Collision --- (Same as before)
        float outerXPosEps = RECT_OUTER_X_POS + COLLISION_EPSILON; float outerXNegEps = RECT_OUTER_X_NEG - COLLISION_EPSILON; float outerZPosEps = RECT_OUTER_Z_POS + COLLISION_EPSILON; float outerZNegEps = RECT_OUTER_Z_NEG - COLLISION_EPSILON;
        float innerXPosEps = RECT_INNER_X_POS - COLLISION_EPSILON; float innerXNegEps = RECT_INNER_X_NEG + COLLISION_EPSILON; float innerZPosEps = RECT_INNER_Z_POS - COLLISION_EPSILON; float innerZNegEps = RECT_INNER_Z_NEG + COLLISION_EPSILON;
        if (x > outerXPosEps || x < outerXNegEps || z > outerZPosEps || z < outerZNegEps) return 0;
        if (x < innerXPosEps && x > innerXNegEps && z < innerZPosEps && z > innerZNegEps) return 0;
        return 1;
    } else { // TRACK_ROUNDED
        // --- Rounded Corner Collision --- (Revised Straight Check) ---
        float absX = fabsf(x); float absZ = fabsf(z);
        float innerRadiusSq = powf(fmaxf(0.0f, ROUND_INNER_CORNER_RADIUS - COLLISION_EPSILON), 2);
        float outerRadiusSq = powf(ROUND_OUTER_CORNER_RADIUS + COLLISION_EPSILON, 2);
        float halfRoadWidthWithEps = ROUND_HALF_ROAD_WIDTH + COLLISION_EPSILON; // Use half width for checks

        // Check Straight Sections (more precise)
        if (absX <= ROUND_STRAIGHT_X_LIMIT) { // Potential Horizontal straight zone
             if (absZ >= (ROUND_TRACK_MAIN_LENGTH / 2.0f - halfRoadWidthWithEps) && // Check Z within top/bottom road width
                 absZ <= (ROUND_TRACK_MAIN_LENGTH / 2.0f + halfRoadWidthWithEps))
                 return 1; // On top or bottom straight
        }
        if (absZ <= ROUND_STRAIGHT_Z_LIMIT) { // Potential Vertical straight zone
             if (absX >= (ROUND_TRACK_MAIN_WIDTH / 2.0f - halfRoadWidthWithEps) && // Check X within left/right road width
                 absX <= (ROUND_TRACK_MAIN_WIDTH / 2.0f + halfRoadWidthWithEps))
                 return 1; // On left or right straight
        }

        // Check Corner Sections (Same as before)
        float corner_center_x = 0.0f, corner_center_z = 0.0f; int in_corner_zone = 0;
        if (x > ROUND_STRAIGHT_X_LIMIT && z > ROUND_STRAIGHT_Z_LIMIT) { corner_center_x = ROUND_CORNER_CENTER_TR_X; corner_center_z = ROUND_CORNER_CENTER_TR_Z; in_corner_zone = 1; }
        else if (x < -ROUND_STRAIGHT_X_LIMIT && z > ROUND_STRAIGHT_Z_LIMIT) { corner_center_x = ROUND_CORNER_CENTER_TL_X; corner_center_z = ROUND_CORNER_CENTER_TL_Z; in_corner_zone = 1; }
        else if (x < -ROUND_STRAIGHT_X_LIMIT && z < -ROUND_STRAIGHT_Z_LIMIT) { corner_center_x = ROUND_CORNER_CENTER_BL_X; corner_center_z = ROUND_CORNER_CENTER_BL_Z; in_corner_zone = 1; }
        else if (x > ROUND_STRAIGHT_X_LIMIT && z < -ROUND_STRAIGHT_Z_LIMIT) { corner_center_x = ROUND_CORNER_CENTER_BR_X; corner_center_z = ROUND_CORNER_CENTER_BR_Z; in_corner_zone = 1; }

        if (in_corner_zone) {
            float dx = x - corner_center_x; float dz = z - corner_center_z;
            float dist_sq = dx * dx + dz * dz;
            if (dist_sq >= innerRadiusSq && dist_sq <= outerRadiusSq) return 1; // In corner bounds
        }
        return 0; // Off track
    }
}