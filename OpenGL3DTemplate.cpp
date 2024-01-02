#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <windows.h>
#include <glut.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

const int screenWidth = 1200;
const int screenHeight = 600;
int timer = 120;
bool animationsActive = true;
int ticketSoundDelay = 1;
int animationSoundDelay = -1;
GLboolean win = false;
GLboolean lose = false;
bool soundPlayed = false;

class Vector3f {
public:
    float x, y, z;

    Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f operator+(const Vector3f& v) const {
        return Vector3f(x + v.x, y + v.y, z + v.z);
    }

    Vector3f operator-(const Vector3f& v) const {
        return Vector3f(x - v.x, y - v.y, z - v.z);
    }

    Vector3f operator*(float n) const {
        return Vector3f(x * n, y * n, z * n);
    }

    Vector3f operator/(float n) const {
        return Vector3f(x / n, y / n, z / n);
    }

    Vector3f unit() const {
        float magnitude = sqrt(x * x + y * y + z * z);
        return (*this) / magnitude;
    }

    Vector3f cross(const Vector3f& v) const {
        return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};


class Camera {
public:
    Vector3f eye, center, up;

    Camera(float eyeX = 0.021192, float eyeY = 0.353662, float eyeZ = 1.06366, float centerX = 0.0163718, float centerY = 0.0163718, float centerZ = 0.0163718, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
        eye = Vector3f(eyeX, eyeY, eyeZ);
        center = Vector3f(centerX, centerY, centerZ);
        up = Vector3f(upX, upY, upZ);
    }


    void moveX(float d) {
        Vector3f right = up.cross(center - eye).unit();
        eye = eye + right * d;
        center = center + right * d;
    }

    void moveY(float d) {
        eye = eye + up.unit() * d;
        center = center + up.unit() * d;
    }

    void moveZ(float d) {
        Vector3f view = (center - eye).unit();
        eye = eye + view * d;
        center = center + view * d;
    }

    void rotateX(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
        up = view.cross(right);
        center = eye + view;
    }

    void rotateY(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
        right = view.cross(up);
        center = eye + view;
    }

    void look() {
        gluLookAt(
            eye.x, eye.y, eye.z,
            center.x, center.y, center.z,
            up.x, up.y, up.z
        );
    }
};

class Fence {
public:
    float r, g, b;
    float colorChangeSpeed;

    Fence() : r(0.5f), g(0.3f), b(0.0f), colorChangeSpeed(0.03f) {}

    void animate() {
        r += colorChangeSpeed;
        g += colorChangeSpeed * 0.6f;
        b -= colorChangeSpeed * 0.2f;

        if (r > 0.8f || r < 0.2f) {
            colorChangeSpeed = -colorChangeSpeed;
        }
    }
};

class Player {
public:
    float posX, posY, posZ;
    float rotY;

    Player() : posX(0.0f), posY(0.0f), posZ(0.0f), rotY(0.0f) {}

    void moveX(float dx) {
        posX += dx;
    }

    void moveZ(float dz) {
        posZ += dz;
    }

    void rotateY(float angle) {
        rotY = angle;
    }
};

class FerrisWheel {
public:
    float rotationAngle;
    FerrisWheel() : rotationAngle(0.0f) {}

    void animate() {
        rotationAngle += 3.0f;
        if (rotationAngle > 360.0f) {
            rotationAngle -= 360.0f;
        }
    }
};

class HotAirBalloon {
public:
    float translationY;
    float translationSpeed;
    float minHeight;
    float maxHeight;

    HotAirBalloon() : translationY(0.0f), translationSpeed(0.01f), minHeight(-0.03f), maxHeight(0.03f) {}

    void animate() {
        translationY += translationSpeed;
        if (translationY > maxHeight || translationY < minHeight) {
            translationSpeed = -translationSpeed;
        }
    }
};

class Swing {
public:
    float rotationAngle;
    float rotationSpeed;
    float maxRotationAngle;
    bool swingForward;

    Swing() : rotationAngle(0.0f), rotationSpeed(3.0f), maxRotationAngle(20.0f), swingForward(true) {}

    void animate() {
        if (swingForward) {
            rotationAngle += rotationSpeed;
            if (rotationAngle >= maxRotationAngle) {
                swingForward = false;
            }
        }
        else {
            rotationAngle -= rotationSpeed;
            if (rotationAngle <= -maxRotationAngle) {
                swingForward = true;
            }
        }
    }
};

class Tree {
public:
    float scale;
    float scaleSpeed;
    float minScale;
    float maxScale;

    Tree() : scale(1.0f), scaleSpeed(0.02f), minScale(0.8f), maxScale(1.2f) {}

    void animate() {
        scale += scaleSpeed;
        if (scale > maxScale || scale < minScale) {
            scaleSpeed = -scaleSpeed;
        }
    }
};

class TicketStand {
public:
    float scale;
    float scaleSpeed;
    float minScale;
    float maxScale;

    TicketStand() : scale(1.0f), scaleSpeed(0.01f), minScale(1.0f), maxScale(1.2f) {}

    void animate() {
        scale += scaleSpeed;
        if (scale > maxScale || scale < minScale) {
            scaleSpeed = -scaleSpeed;
        }
    }
};

class Ticket {
public:
    float posX, posY, posZ;
    bool isHit;
    float translationX;
    float translationSpeed;
    float minX;
    float maxX;

    Ticket() : posX(0.3f), posY(0.0f), posZ(0.3f), isHit(false), translationX(0.0f), translationSpeed(0.01f), minX(-0.03f), maxX(0.03f) {}

    void animate() {
        translationX += translationSpeed;
        if (translationX > maxX || translationX < minX) {
            translationSpeed = -translationSpeed;
        }
    }
};

Camera camera;
Fence fence;
Player player;
FerrisWheel ferrisWheel;
HotAirBalloon hotAirBalloon;
Swing swing;
Tree tree;
TicketStand ticketStand;
Ticket ticket;

void anim(int value) {
    if (animationsActive) {
        fence.animate();
        ferrisWheel.animate();
        hotAirBalloon.animate();
        swing.animate();
        tree.animate();
        ticketStand.animate();
        ticket.animate();
    }

    glutPostRedisplay();

    glutTimerFunc(1000 / 60, anim, 0);
}

void drawGround(double thickness) {
    glPushMatrix();
    glColor3f(0.4, 0.6, 0.2);
    glScaled(1.0, thickness, 1.0);
    glutSolidCube(1);
    glPopMatrix();
}

void drawSky() {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(0.6 * 0.9, 0.8 * 0.9, 1 * 0.9);
    GLUquadricObj* qobj;
    qobj = gluNewQuadric();
    glTranslated(50, 0, 0);
    glRotated(90, 1, 0, 1);
    gluQuadricTexture(qobj, true);
    gluQuadricNormals(qobj, GL_SMOOTH);
    gluSphere(qobj, 100, 100, 100);
    gluDeleteQuadric(qobj);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawFence(double legThick, double legLen) {
    glPushMatrix();

    glColor3f(fence.r, fence.g, fence.b);

    for (int i = -6; i < 7; i++) {
        glPushMatrix();
        glTranslated(i * 0.08, legLen / 2, 0);
        glScaled(legThick, legLen, legThick);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    glPushMatrix();
    glTranslated(0, 0.25, 0);
    glScaled(1, 0.02, 0.02);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawPlayer() {
    glPushMatrix();

    glTranslatef(player.posX, player.posY, player.posZ);
    glRotatef(player.rotY, 0.0f, 1.0f, 0.0f);

    // head
    glColor3f(0.9765, 0.8784, 0.7529);
    glPushMatrix();
    glScaled(0.5, 0.5, 0.5);
    glutSolidSphere(0.1, 100, 100);
    glPopMatrix();

    // eyes
    glColor3f(0.0, 0.3, 0.0);
    glPushMatrix();
    glTranslated(0.015, 0.03, 0.04);
    glScaled(0.05, 0.05, 0.05);
    glutSolidSphere(0.1, 100, 100);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.015, 0.03, 0.04);
    glScaled(0.05, 0.05, 0.05);
    glutSolidSphere(0.1, 100, 100);
    glPopMatrix();

    // mouth
    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
    glTranslated(0, 0.047, 0.01);
    glScaled(0.8, 1, 0.8);
    glBegin(GL_LINE_STRIP);

    GLfloat ctrlPoints[4][3] = {
        {0.03, -0.05, 0.05},
        {0.01, -0.065, 0.05},
        {-0.01, -0.065, 0.05},
        {-0.03, -0.05, 0.05}
    };

    for (float t = 0.0; t <= 1.0; t += 0.01) {
        float x = (1 - t) * (1 - t) * (1 - t) * ctrlPoints[0][0] +
            3 * (1 - t) * (1 - t) * t * ctrlPoints[1][0] +
            3 * (1 - t) * t * t * ctrlPoints[2][0] +
            t * t * t * ctrlPoints[3][0];

        float y = (1 - t) * (1 - t) * (1 - t) * ctrlPoints[0][1] +
            3 * (1 - t) * (1 - t) * t * ctrlPoints[1][1] +
            3 * (1 - t) * t * t * ctrlPoints[2][1] +
            t * t * t * ctrlPoints[3][1];

        float z = (1 - t) * (1 - t) * (1 - t) * ctrlPoints[0][2] +
            3 * (1 - t) * (1 - t) * t * ctrlPoints[1][2] +
            3 * (1 - t) * t * t * ctrlPoints[2][2] +
            t * t * t * ctrlPoints[3][2];

        glVertex3f(x, y, z);
    }
    glEnd();
    glPopMatrix();

    // t-shirt
    glColor3f(0.5, 0.7, 1);
    glPushMatrix();
    glTranslated(0, -0.15, 0);
    glRotated(-90, 1, 0, 0);
    glScaled(0.1, 0.1, 0.1);
    glutSolidCone(0.5, 1.5, 50, 50);
    glPopMatrix();

    // sleeves
    glPushMatrix();
    glTranslated(0.04, -0.085, 0.0);
    glRotated(-90, 1, 0, 0);
    glRotated(-30, 0, 1, 0);
    glScaled(0.03, 0.055, 0.03);
    glutSolidCone(0.6, 1.6, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.04, -0.085, 0.0);
    glRotated(-90, 1, 0, 0);
    glRotated(30, 0, 1, 0);
    glScaled(0.03, 0.055, 0.03);
    glutSolidCone(0.6, 1.6, 50, 50);
    glPopMatrix();

    // shorts
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslated(0.02, -0.15, 0);
    glScaled(0.025, 0.08, 0.025);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.02, -0.15, 0);
    glScaled(0.025, 0.08, 0.025);
    glutSolidCube(1.0);
    glPopMatrix();

    // legs
    glColor3f(0.9765, 0.8784, 0.7529);
    glPushMatrix();
    glTranslated(0.02, -0.21, 0);
    glScaled(0.025, 0.04, 0.025);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.02, -0.21, 0);
    glScaled(0.025, 0.04, 0.025);
    glutSolidCube(1.0);
    glPopMatrix();

    // arms
    glColor3f(0.9765, 0.8784, 0.7529);
    glPushMatrix();
    glTranslated(0.05, -0.09, 0);
    glRotated(36, 0, 0, 1);
    glScaled(0.015, 0.045, 0.015);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.9765, 0.8784, 0.7529);
    glPushMatrix();
    glTranslated(-0.05, -0.09, 0);
    glRotated(-36, 0, 0, 1);
    glScaled(0.015, 0.045, 0.015);
    glutSolidCube(1.0);
    glPopMatrix();

    // shoes
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslated(0.02, -0.23, 0.005);
    glScaled(0.03, 0.007, 0.05);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.02, -0.23, 0.005);
    glScaled(0.03, 0.007, 0.05);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void darwFerrisWheel() {
    glPushMatrix();

    glRotatef(ferrisWheel.rotationAngle, 0, 0, 1);

    // wheel
    glPushMatrix();
    glColor3f(1.0, 0.75, 0.5);
    glutWireTorus(0.02, 0.2, 1000, 1000);
    glPopMatrix();

    glPushMatrix();
    glutWireTorus(0.009, 0.1, 1000, 1000);
    glPopMatrix();

    // rods
    glPushMatrix();
    glColor3f(1.0 * 0.65, 0.0, 0.0);
    glScaled(0.012, 0.4, 0.012);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 1.0 * 0.65, 0.0);
    glRotated(45, 0, 0, 1);
    glScaled(0.012, 0.4, 0.012);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.0, 1.0 * 0.65);
    glRotated(-45, 0, 0, 1);
    glScaled(0.012, 0.4, 0.012);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0 * 0.65, 1.0 * 0.65, 0.0);
    glRotated(90, 0, 0, 1);
    glScaled(0.012, 0.4, 0.012);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawFerrisWheelStructure() {
    darwFerrisWheel();

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslated(-0.08, -0.2, 0.0);
    glRotated(-22.5, 0, 0, 1);
    glScaled(0.015, 0.4, 0.015);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslated(0.08, -0.2, 0.0);
    glRotated(22.5, 0, 0, 1);
    glScaled(0.015, 0.4, 0.015);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslated(0.0, -0.38, 0.0);
    glRotated(90, 0, 0, 1);
    glScaled(0.015, 0.4, 0.015);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawHotAirBalloon() {
    glPushMatrix();

    glTranslated(0.0, hotAirBalloon.translationY, 0.0);

    // balloon
    glPushMatrix();
    glScaled(0.0024, 0.0036, 0.0024);
    glutSolidSphere(40.0, 100, 100);
    glPopMatrix();

    // basket
    glPushMatrix();
    glColor3f(0.8, 0.6, 0.4);
    glTranslated(0.0, -0.25, 0.0);
    glScaled(0.1, 0.05, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();

    // basket details
    glColor3f(0.5, 0.3, 0.0);
    for (int i = -3; i < 4; i++) {
        glPushMatrix();
        glTranslated(i * 0.016, -0.25, 0.053);
        glScaled(0.0025, 0.05, 0.0025);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // support
    glPushMatrix();
    glRotated(10, 0, 0, 1);
    glTranslated(-0.05, -0.18, 0);
    glScaled(0.01, 0.15, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glRotated(-10, 0, 0, 1);
    glTranslated(0.05, -0.18, 0);
    glScaled(0.01, 0.15, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawSwing() {
    glPushMatrix();

    // translate to the top rod
    glTranslatef(0, 0.2, -0.05);
    // rotate about x
    glRotatef(-swing.rotationAngle, 1, 0, 0);
    // translate back to the original position
    glTranslatef(0, -0.2, 0.05);

    // chair
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glScaled(0.18, 0.01, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glTranslated(0.0, 0.05, -0.05);
    glScaled(0.18, 0.1, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    // chair details
    glColor3f(0.0, 0.3, 0.5);
    for (int i = -2; i < 3; i++) {
        glPushMatrix();
        glTranslated(i * 0.04, 0.05, -0.045);
        glScaled(0.005, 0.1, 0.005);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // rods
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.2);
    glTranslated(-0.08, 0.15, -0.05);
    glScaled(0.01, 0.1, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.08, 0.15, -0.05);
    glScaled(0.01, 0.1, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawSwingStructure() {
    drawSwing();

    // top rod
    glPushMatrix();
    glTranslated(0, 0.2, -0.05);
    glScaled(0.3, 0.01, 0.02);
    glutSolidCube(1.0);
    glPopMatrix();

    // side rods
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.2);
    glTranslated(-0.13, 0.05, -0.05);
    glScaled(0.01, 0.3, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.2);
    glTranslated(0.13, 0.05, -0.05);
    glScaled(0.01, 0.3, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    // base rods
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.2);
    glTranslated(0.13, -0.1, -0.05);
    glRotated(90, 1, 0, 0);
    glScaled(0.02, 0.15, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.2);
    glTranslated(-0.13, -0.1, -0.05);
    glRotated(90, 1, 0, 0);
    glScaled(0.02, 0.15, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

}

void drawTree() {
    glPushMatrix();

    glScalef(tree.scale, tree.scale, tree.scale);

    // tree body
    glPushMatrix();
    glColor3f(0.4, 0.6, 0.2);
    glRotated(-90, 1, 0, 0);
    glScaled(0.1, 0.1, 0.1);
    glutSolidCone(0.5, 1.5, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, 0.06, 0);
    glRotated(-90, 1, 0, 0);
    glScaled(0.1 * 0.9, 0.1 * 0.9, 0.1 * 0.9);
    glutSolidCone(0.5, 1.5, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, 0.12, 0);
    glRotated(-90, 1, 0, 0);
    glScaled(0.1 * 0.8, 0.1 * 0.8, 0.1 * 0.8);
    glutSolidCone(0.5, 1.5, 50, 50);
    glPopMatrix();

    // trunk
    glPushMatrix();
    glColor3f(0.5, 0.3, 0.0);
    glTranslated(0, -0.02, 0);
    glScaled(0.02, 0.07, 0.02);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawTicketStand() {
    glPushMatrix();

    glScaled(ticketStand.scale, ticketStand.scale, ticketStand.scale);

    // body
    for (int i = -3; i < 4; i++) {
        glPushMatrix();
        if (i % 2 == 0)
            glColor3f(1.0, 0.0, 0.0);
        else
            glColor3f(1.0, 1.0, 1.0);
        glTranslated(i * 0.04, 0, 0);
        glScaled(0.04, 0.3, 0.1);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // window
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);
    glTranslated(0, 0.05, 0.055);
    glScaled(0.7, 0.8, 0);
    GLUquadric* quadObj = gluNewQuadric();
    gluDisk(quadObj, 0, 0.1, 50, 50);
    glPopMatrix();

    // sign
    glPushMatrix();
    glColor3f(1.0 * 0.8, 1.0 * 0.8, 0.0);
    glTranslated(0, 0.23, 0);
    glScaled(0.25, 0.07, 0.025);
    glutSolidCube(1.0);
    glPopMatrix();

    // rods
    glPushMatrix();
    glTranslated(0.05, 0.15, 0);
    glScaled(0.02, 0.1, 0.008);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.05, 0.15, 0);
    glScaled(0.02, 0.1, 0.008);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawTicket() {
    glPushMatrix();
    glTranslatef(ticket.translationX, 0, 0);

    // body
    glPushMatrix();
    glScalef(0.2, 0.1, 0.01);
    glutSolidCube(1.0);
    glPopMatrix();

    // decoration
    glColor3f(1.0, 1.0, 1.0);

    float positions[4] = { 0.025, -0.025, 0.075, -0.075 };

    // left spheres
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(-0.1, positions[i] * 0.5, 0);
        glScaled(0.5, 0.5, 0.5);
        glutSolidSphere(0.02, 20, 20);
        glPopMatrix();
    }

    // right spheres
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(0.1, positions[i] * 0.5, 0);
        glScaled(0.5, 0.5, 0.5);
        glutSolidSphere(0.02, 20, 20);
        glPopMatrix();
    }

    glPopMatrix();
}

bool checkCollision(const Ticket& ticket) {
    float collisionDistanceX = 0.09f;
    float collisionDistanceZ = 0.09f;

    return (abs(player.posX - ticket.posX) < collisionDistanceX &&
        abs(player.posZ - ticket.posZ) < collisionDistanceZ);
}

void setupLights() {
    GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat shininess[] = { 50 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
    //    GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, screenWidth / screenHeight, 0.001, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.look();
}

void Keyboard(unsigned char key, int x, int y) {
    if (timer == 0)
        return;

    if (key == ' ') {
        animationsActive = !animationsActive;
        PlaySound(TEXT("anim"), NULL, SND_ASYNC);
        animationSoundDelay = 1;
    }

    float d = 0.03;
    float moveDistance = 0.03f;

    switch (key) {
    case 'w':
        camera.moveY(d);
        break;
    case 's':
        camera.moveY(-d);
        break;
    case 'a':
        camera.moveX(d);
        break;
    case 'd':
        camera.moveX(-d);
        break;
    case 'q':
        camera.moveZ(d);
        break;
    case 'e':
        camera.moveZ(-d);
        break;
    case 't': // top view
        camera.eye.x = 0.0160754;
        camera.eye.y = 1.27918;
        camera.eye.z = -0.048015;
        camera.center.x = 0.016228;
        camera.center.y = 0.016228;
        camera.center.z = 0.016228;
        break;
    case 'f': // front view
        camera.eye.x = 0.0212869;
        camera.eye.y = 0.205086;
        camera.eye.z = 1.08428;
        camera.center.x = 0.0167281;
        camera.center.y = 0.0167281;
        camera.center.z = 0.0167281;
        break;
    case 'c': // side view
        camera.eye.x = -0.992256;
        camera.eye.y = 0.227585;
        camera.eye.z = 0.0032941;
        camera.center.x = 0.00435436;
        camera.center.y = 0.00435436;
        camera.center.z = 0.00435436;
        break;
    case 'j': // move left (-x)
        if (player.posX - moveDistance >= -0.45) {
            player.moveX(-moveDistance);
            player.rotateY(270);
        }
        break;
    case 'l': // move right (x)
        if (player.posX + moveDistance <= 0.45) {
            player.moveX(moveDistance);
            player.rotateY(90);
        }
        break;
    case 'k': // move forward (+z)
        if (player.posZ + moveDistance <= 0.45) {
            player.moveZ(moveDistance);
            player.rotateY(0);
        }
        break;
    case 'i': // move backward (-z)
        if (player.posZ - moveDistance >= -0.5) {
            player.moveZ(-moveDistance);
            player.rotateY(180);
        }
        break;
    case GLUT_KEY_ESCAPE:
        exit(EXIT_SUCCESS);
    }

    glutPostRedisplay();

}

void Special(int key, int x, int y) {

    float a = 1.0;

    switch (key) {
    case GLUT_KEY_UP:
        camera.rotateX(a);
        break;
    case GLUT_KEY_DOWN:
        camera.rotateX(-a);
        break;
    case GLUT_KEY_LEFT:
        camera.rotateY(a);
        break;
    case GLUT_KEY_RIGHT:
        camera.rotateY(-a);
        break;
    }

    glutPostRedisplay();
}

void gameOver() {
    animationsActive = false;
    glDisable(GL_LIGHTING);
    glColor3f(1.0 * 0.8, 0.0, 0.0);
    glRasterPos2i(10, screenHeight - 30);
    char overText[] = "Game Over!";
    for (char* c = overText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glEnable(GL_LIGHTING);
}

void youWin() {
    animationsActive = false;
    glDisable(GL_LIGHTING);
    glColor3f(0.0, 1.0, 0.0);
    glRasterPos2i(10, screenHeight - 30);
    char overText[] = "You Win!";
    for (char* c = overText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glEnable(GL_LIGHTING);
    if (!soundPlayed) {
        PlaySound(TEXT("ticket"), NULL, SND_ASYNC);
        soundPlayed = true;
    }
}

void update(int value) {
    if (animationSoundDelay == 0)
    {
        PlaySound(TEXT("backGround"), NULL, SND_ASYNC);
        animationSoundDelay = -1;
    }
    else
        animationSoundDelay--;

    if (ticketSoundDelay == 0 && ticket.isHit)
    {
        PlaySound(TEXT("backGround"), NULL, SND_ASYNC);
        ticketSoundDelay = -1;
    }
    else
    {
        if (ticket.isHit)
            ticketSoundDelay--;
    }


    if (timer == 0) {
        if (ticket.isHit)
        {
            win = true;
            PlaySound(TEXT("win"), NULL, SND_ASYNC);
        }
        else
        {
            lose = true;
            PlaySound(TEXT("lose"), NULL, SND_ASYNC);
        }
    }
    else {
        timer--;
        glutTimerFunc(1000, update, 0);
    }
    glutPostRedisplay();
}

void Display() {
    setupCamera();
    setupLights();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (timer == 0 && !ticket.isHit) {
        gameOver();
    }
    else if (checkCollision(ticket)) {
        ticket.isHit = true;
        youWin();
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    drawSky();
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, 0.0, -0.5);
    drawFence(0.02, 0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.5, 0, 0);
    glRotated(90, 0, 1, 0);
    drawFence(0.02, 0.3);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.5, 0, 0);
    glRotated(90, 0, 1, 0);
    drawFence(0.02, 0.3);
    glPopMatrix();

    glPushMatrix();
    drawGround(0.02);
    glPopMatrix();

    glPushMatrix();
    glScaled(0.8, 0.8, 0.8);
    glTranslated(0, 0.25, 0);
    drawPlayer();
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.0, 0.37, -0.42);
    glScaled(0.9, 0.9, 0.9);
    drawFerrisWheelStructure();
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.5, 0.4, 0.2);
    glScaled(0.3, 0.3, 0.3);
    glColor3f(1.0, 0.0, 0.0);
    drawHotAirBalloon();
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.6, 0.43, 0.3);
    glScaled(0.35, 0.35, 0.35);
    glColor3f(0.0, 0.0, 1.0);
    drawHotAirBalloon();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.4, 0.43, -0.6);
    glScaled(0.35, 0.35, 0.35);
    glColor3f(0.0, 1.0, 0.0);
    drawHotAirBalloon();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.35, 0.12, 0);
    glRotated(90, 0, 1, 0);
    glScaled(0.8, 0.8, 0.8);
    drawSwingStructure();
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.3, 0.06, -0.2);
    glScaled(0.85, 0.85, 0.85);
    drawTree();
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.42, 0.06, 0.1);
    glScaled(0.7, 0.7, 0.7);
    drawTree();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.42, 0.08, 0.35);
    glRotated(90, 0, 1, 0);
    glScaled(0.5, 0.5, 0.4);
    drawTicketStand();
    glPopMatrix();

    if (!ticket.isHit) {
        glPushMatrix();
        glTranslated(0.3, 0.03, 0.3);
        glScaled(0.3, 0.3, 0.3);
        drawTicket();
        glPopMatrix();
    }

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("Dream Park");
    //glEnable(GL_MULTISAMPLE);

    if (!win && !lose)
        PlaySound(TEXT("backGround"), NULL, SND_ASYNC);

    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Special);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glutTimerFunc(0, anim, 0);
    glutTimerFunc(1000, update, 0);

    glutMainLoop();
    return 0;
}
