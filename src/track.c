#include "track.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h> // For fabsf
#include <stdio.h> // Optional: For debug prints

// --- Main Rendering Functions (Simple Rectangle) ---

void renderTrack() {
    float surface_y = 0.0f;
    float line_y = 0.01f;
    float finish_y = 0.02f;

    // --- Render Ground Plane ---
    glColor3f(0.2f, 0.6f, 0.2f); // Grassy Green
    glBegin(GL_QUADS);
        float groundSize = fmaxf(TRACK_MAIN_WIDTH, TRACK_MAIN_LENGTH) * 1.2f;
        glVertex3f(-groundSize, -0.02f, -groundSize);
        glVertex3f(-groundSize, -0.02f,  groundSize);
        glVertex3f( groundSize, -0.02f,  groundSize);
        glVertex3f( groundSize, -0.02f, -groundSize);
    glEnd();

    // --- Render Track Surface (Asphalt Grey) ---
    // Draw the surface as four rectangular strips (Top, Bottom, Left, Right)
    glColor3f(0.4f, 0.4f, 0.45f);

    // Top strip
    glBegin(GL_QUADS);
        glVertex3f(OUTER_X_NEG, surface_y, INNER_Z_POS);
        glVertex3f(OUTER_X_POS, surface_y, INNER_Z_POS);
        glVertex3f(OUTER_X_POS, surface_y, OUTER_Z_POS);
        glVertex3f(OUTER_X_NEG, surface_y, OUTER_Z_POS);
    glEnd();
    // Bottom strip
    glBegin(GL_QUADS);
        glVertex3f(OUTER_X_NEG, surface_y, OUTER_Z_NEG);
        glVertex3f(OUTER_X_POS, surface_y, OUTER_Z_NEG);
        glVertex3f(OUTER_X_POS, surface_y, INNER_Z_NEG);
        glVertex3f(OUTER_X_NEG, surface_y, INNER_Z_NEG);
    glEnd();
    // Left strip (excluding corners covered by top/bottom)
    glBegin(GL_QUADS);
        glVertex3f(OUTER_X_NEG, surface_y, INNER_Z_NEG);
        glVertex3f(INNER_X_NEG, surface_y, INNER_Z_NEG);
        glVertex3f(INNER_X_NEG, surface_y, INNER_Z_POS);
        glVertex3f(OUTER_X_NEG, surface_y, INNER_Z_POS);
    glEnd();
    // Right strip (excluding corners covered by top/bottom)
     glBegin(GL_QUADS);
        glVertex3f(INNER_X_POS, surface_y, INNER_Z_NEG);
        glVertex3f(OUTER_X_POS, surface_y, INNER_Z_NEG);
        glVertex3f(OUTER_X_POS, surface_y, INNER_Z_POS);
        glVertex3f(INNER_X_POS, surface_y, INNER_Z_POS);
    glEnd();


    // --- Render Track Markings (White Lines) ---
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    // Outer boundary
    glBegin(GL_LINE_LOOP);
        glVertex3f(OUTER_X_NEG, line_y, OUTER_Z_NEG);
        glVertex3f(OUTER_X_POS, line_y, OUTER_Z_NEG);
        glVertex3f(OUTER_X_POS, line_y, OUTER_Z_POS);
        glVertex3f(OUTER_X_NEG, line_y, OUTER_Z_POS);
    glEnd();

    // Inner boundary
    glBegin(GL_LINE_LOOP);
        glVertex3f(INNER_X_NEG, line_y, INNER_Z_NEG);
        glVertex3f(INNER_X_POS, line_y, INNER_Z_NEG);
        glVertex3f(INNER_X_POS, line_y, INNER_Z_POS);
        glVertex3f(INNER_X_NEG, line_y, INNER_Z_POS);
    glEnd();

    // --- Start/Finish line ---
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
        glVertex3f(FINISH_LINE_X_START, finish_y, FINISH_LINE_Z + FINISH_LINE_THICKNESS / 2.0f);
        glVertex3f(FINISH_LINE_X_END,   finish_y, FINISH_LINE_Z + FINISH_LINE_THICKNESS / 2.0f);
        glVertex3f(FINISH_LINE_X_END,   finish_y, FINISH_LINE_Z - FINISH_LINE_THICKNESS / 2.0f);
        glVertex3f(FINISH_LINE_X_START, finish_y, FINISH_LINE_Z - FINISH_LINE_THICKNESS / 2.0f);
    glEnd();

    glLineWidth(1.0f); // Reset line width
}

// Helper function to draw a wall segment
void drawWall(float x1, float z1, float x2, float z2, float height, float thickness) {
    // Calculate normalized direction vector
    float dx = x2 - x1;
    float dz = z2 - z1;
    float len = sqrtf(dx*dx + dz*dz);
    if (len < 0.001f) return; // Avoid division by zero
    float nx = dx / len;
    float nz = dz / len;

    // Calculate perpendicular vector for thickness
    float px = -nz;
    float pz = nx;

    float half_thick = thickness / 2.0f;

    // Define the 8 vertices of the wall block
    float v[8][3] = {
        {x1 - px*half_thick, 0.0f, z1 - pz*half_thick}, // front bottom left
        {x1 + px*half_thick, 0.0f, z1 + pz*half_thick}, // back bottom left
        {x2 + px*half_thick, 0.0f, z2 + pz*half_thick}, // back bottom right
        {x2 - px*half_thick, 0.0f, z2 - pz*half_thick}, // front bottom right
        {x1 - px*half_thick, height, z1 - pz*half_thick}, // front top left
        {x1 + px*half_thick, height, z1 + pz*half_thick}, // back top left
        {x2 + px*half_thick, height, z2 + pz*half_thick}, // back top right
        {x2 - px*half_thick, height, z2 - pz*half_thick}  // front top right
    };

    // Draw the 6 faces of the wall
    glBegin(GL_QUADS);
        // Bottom face (optional, usually unseen)
        // glVertex3fv(v[0]); glVertex3fv(v[1]); glVertex3fv(v[2]); glVertex3fv(v[3]);
        // Top face
        glVertex3fv(v[4]); glVertex3fv(v[5]); glVertex3fv(v[6]); glVertex3fv(v[7]);
        // Front face
        glVertex3fv(v[0]); glVertex3fv(v[3]); glVertex3fv(v[7]); glVertex3fv(v[4]);
        // Back face
        glVertex3fv(v[1]); glVertex3fv(v[5]); glVertex3fv(v[6]); glVertex3fv(v[2]);
        // Left face
        glVertex3fv(v[0]); glVertex3fv(v[4]); glVertex3fv(v[5]); glVertex3fv(v[1]);
        // Right face
        glVertex3fv(v[3]); glVertex3fv(v[2]); glVertex3fv(v[6]); glVertex3fv(v[7]);
    glEnd();
}


void renderGuardrails() {
    float railHeight = 0.8f;
    float railThickness = 0.4f; // Give rails some thickness
    glColor3f(0.8f, 0.1f, 0.1f); // Red guardrails

    // Add a small margin so rails aren't exactly on the line
    float margin = 0.1f;

    // --- Outer Guardrail (Clockwise) ---
    float ox1 = OUTER_X_NEG - margin; float oz1 = OUTER_Z_NEG - margin;
    float ox2 = OUTER_X_POS + margin; float oz2 = OUTER_Z_NEG - margin;
    float ox3 = OUTER_X_POS + margin; float oz3 = OUTER_Z_POS + margin;
    float ox4 = OUTER_X_NEG - margin; float oz4 = OUTER_Z_POS + margin;

    drawWall(ox1, oz1, ox2, oz2, railHeight, railThickness); // Bottom
    drawWall(ox2, oz2, ox3, oz3, railHeight, railThickness); // Right
    drawWall(ox3, oz3, ox4, oz4, railHeight, railThickness); // Top
    drawWall(ox4, oz4, ox1, oz1, railHeight, railThickness); // Left


    // --- Inner Guardrail (Clockwise) ---
     float ix1 = INNER_X_NEG + margin; float iz1 = INNER_Z_NEG + margin;
     float ix2 = INNER_X_POS - margin; float iz2 = INNER_Z_NEG + margin;
     float ix3 = INNER_X_POS - margin; float iz3 = INNER_Z_POS - margin;
     float ix4 = INNER_X_NEG + margin; float iz4 = INNER_Z_POS - margin;

    drawWall(ix1, iz1, ix2, iz2, railHeight, railThickness); // Bottom
    drawWall(ix2, iz2, ix3, iz3, railHeight, railThickness); // Right
    drawWall(ix3, iz3, ix4, iz4, railHeight, railThickness); // Top
    drawWall(ix4, iz4, ix1, iz1, railHeight, railThickness); // Left
}


// --- Collision Detection (Simple Rectangle) ---
int isPositionOnTrack(float x, float z) {
    // Add tolerance to boundaries
    float outerXPosEps = OUTER_X_POS + COLLISION_EPSILON;
    float outerXNegEps = OUTER_X_NEG - COLLISION_EPSILON;
    float outerZPosEps = OUTER_Z_POS + COLLISION_EPSILON;
    float outerZNegEps = OUTER_Z_NEG - COLLISION_EPSILON;

    float innerXPosEps = INNER_X_POS - COLLISION_EPSILON;
    float innerXNegEps = INNER_X_NEG + COLLISION_EPSILON;
    float innerZPosEps = INNER_Z_POS - COLLISION_EPSILON;
    float innerZNegEps = INNER_Z_NEG + COLLISION_EPSILON;

    // 1. Check if OUTSIDE the Outer Boundary (definitely off-track)
    if (x > outerXPosEps || x < outerXNegEps || z > outerZPosEps || z < outerZNegEps) {
        return 0; // Off track (outside outer)
    }

    // 2. Check if INSIDE the Inner Boundary (definitely off-track)
    if (x < innerXPosEps && x > innerXNegEps && z < innerZPosEps && z > innerZNegEps) {
         return 0; // Off track (inside inner hole)
    }

    // 3. If not outside the outer box AND not inside the inner box, it must be on the track surface
    return 1; // On track
}