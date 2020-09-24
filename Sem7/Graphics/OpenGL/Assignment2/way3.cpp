#include <iostream>
#include <GL/glut.h>
#include <bits/stdc++.h>
typedef float f;
using namespace std;

class Point {
public:
    float x{}, y{};
    Point()= default;
    Point(float x, float y){
        this->x = x;
        this->y = y;
    }
    const Point& operator=(const Point &rPoint){
        x = rPoint.x;
        y = rPoint.y;
        return *this;
    }
};

vector<Point> points;
int SCREEN_HEIGHT = 500;
int pCount = 0;
int clicks = 4;

void myInit() {
    glClearColor(1.0,1.0,1.0,0.0);
    glColor3f(0.0,0.0,0.0);
    glPointSize(3);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,640.0,0.0,500.0);
}

void drawDot(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x,y);
    glEnd();
    glFlush();
}
void drawLine(Point p1, Point p2) {
    glBegin(GL_LINES);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();
    glFlush();
}

void myMouse(int button, int state, int x, int y) {
    // If left button was clicked
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Store where mouse was clicked, Y is backwards.
        points.emplace_back(Point((f)x,(f)(SCREEN_HEIGHT - y)));
        pCount++;

        // Draw the red  dot.
        drawDot(x, SCREEN_HEIGHT - y);


        // If (click-amout) points are drawn do the curve.
        if(pCount == clicks)
        {
            glColor3f(0.2,1.0,0.0);
            // Drawing the control lines
            for(int k=0;k<clicks-1;k++)
                drawLine(points[k], points[k+1]);

            Point p1 = points[0];
            glColor3f(0.0,0.0,0.0);

            pCount = 0;
        }
    }
}


void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(640,500);
    glutInitWindowPosition(100,150);
    glutCreateWindow("Bezier Curve");
    glutMouseFunc(myMouse);
    glutDisplayFunc(myDisplay);
    myInit();
    glutMainLoop();

    return 0;
}