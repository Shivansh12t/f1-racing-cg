#include "track.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

void renderTrack() {
    // Simple ground plane (e.g., grey)
    glColor3f(0.5f, 0.5f, 0.5f); // Grey color
    glBegin(GL_QUADS);
        // Make it large enough
        glVertex3f(-100.0f, 0.0f, -100.0f);
        glVertex3f(-100.0f, 0.0f,  100.0f);
        glVertex3f( 100.0f, 0.0f,  100.0f);
        glVertex3f( 100.0f, 0.0f, -100.0f);
    glEnd();

    // Add some simple track boundaries or markers (e.g., white lines)
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    glLineWidth(3.0f); // Make lines thicker

    // Example: A simple rectangular track outline
    float trackWidth = 10.0f;
    float trackLength = 60.0f;
    float halfWidth = trackWidth / 2.0f;

    // Outer boundary
    glBegin(GL_LINE_LOOP);
        glVertex3f(-halfWidth - 1, 0.01f, -trackLength / 2);
        glVertex3f(-halfWidth - 1, 0.01f,  trackLength / 2);
        glVertex3f( halfWidth + 1, 0.01f,  trackLength / 2);
        glVertex3f( halfWidth + 1, 0.01f, -trackLength / 2);
    glEnd();

    // Inner boundary
    glBegin(GL_LINE_LOOP);
        glVertex3f(-halfWidth, 0.01f, -trackLength / 2);
        glVertex3f(-halfWidth, 0.01f,  trackLength / 2);
        glVertex3f( halfWidth, 0.01f,  trackLength / 2);
        glVertex3f( halfWidth, 0.01f, -trackLength / 2);
    glEnd();

    // Start/Finish line (example)
    glColor3f(0.9f, 0.9f, 0.0f); // Yellowish
    glBegin(GL_QUADS);
        glVertex3f(-halfWidth, 0.02f, 0.0f);
        glVertex3f( halfWidth, 0.02f, 0.0f);
        glVertex3f( halfWidth, 0.02f, -1.0f);
        glVertex3f(-halfWidth, 0.02f, -1.0f);
    glEnd();

    glLineWidth(1.0f); // Reset line width
}