#include <iostream>
#include <GL/glut.h>
#include <bits/stdc++.h>
#define MAX_POINTS 100
#define SPLINE_LIST 4
#define DELTA_T 0.005  /* time step factor for drawing each curve            */
typedef double d;
using namespace std;

class Point {
public:
    d x{}, y{};
    Point()= default;
    Point(d x, d y){
        this->x = x;
        this->y = y;
    }
    Point& operator=(const Point &rPoint){
        x = rPoint.x;
        y = rPoint.y;
        return *this;
    }
};

vector<Point> points;
int SCREEN_HEIGHT = 500;
int clicks = 4;

void drawDot(Point p) {
    glBegin(GL_POINTS);
    glVertex2i(p.x,p.y);
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
d smoothen (d t) {
    d tp2, tp1, tm2, tm1;

    if (t <= -2.0)
        return 0.0;
    else if (t <= -1.0)
    {
        tp2 = t + 2.0;
        return tp2 * tp2 * tp2 / 6.0;
    }
    else if (t <= 0.0)
    {
        tp2 = t + 2.0;
        tp1 = t + 1.0;
        tp2 = tp2 * tp2 * tp2 / 6.0;
        tp1 = 2.0 * tp1 * tp1 * tp1 /3.0;
        return tp2 - tp1;
    }
    else if (t <= 1.0)
    {
        tm1 = 1.0 - t;
        tm2 = 2.0 - t;
        tm1 = 2.0 * tm1 * tm1 * tm1 / 3.0;
        tm2 = tm2 * tm2 * tm2 / 6.0;
        return tm2 - tm1;
    }
    else if (t <= 2.0)
    {
        tm2 = 2.0 - t;
        return tm2 * tm2 * tm2 / 6.0;
    }
    else
        return 0.0;
}
void spline (){                  /* This function approximates the data with spline curves. */
    d xs[MAX_POINTS+4], ys[MAX_POINTS+4];
    d x, y, t, bt1, bt2, bt3, bt4;
    int i;

    xs[0] = xs[1] = points[0].x;
    ys[0] = ys[1] = points[0].y;
    for (i=0; i<points.size(); i++)
    {
        xs[i+2] = points[i].x;
        ys[i+2] = points[i].y;
    }
    xs[points.size()+2] = xs[points.size()+3] = points[points.size()-1].x;
    ys[points.size()+2] = ys[points.size()+3] = points[points.size()-1].y;

    glNewList (SPLINE_LIST, GL_COMPILE);
    glColor3f (0.0, 0.0, 0.0);  /* Draw curve in black. */
    glBegin (GL_LINE_STRIP);
    glVertex2d (points[0].x, points[0].y);
    for (i=0; i<=points.size(); i++)
        for (t=DELTA_T; t<1.0+DELTA_T/2.0; t+=DELTA_T)
        {
            bt1 = smoothen(t - 2.0);
            bt2 = smoothen(t - 1.0);
            bt3 = smoothen(t);
            bt4 = smoothen(t + 1.0);
            x = xs[i]*bt4 + xs[i+1]*bt3 + xs[i+2]*bt2 + xs[i+3]*bt1;
            y = ys[i]*bt4 + ys[i+1]*bt3 + ys[i+2]*bt2 + ys[i+3]*bt1;
            glVertex2d (x, y);
        }
    glEnd ();
    glEndList ();
}

void myInit() {
    glClearColor(1.0,1.0,1.0,0.0);
    glColor3f(0.0,0.0,0.0);
    glPointSize(3);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,640.0,0.0,500.0);
}
void mouse(int button, int state, int x, int y) {
    // If left button was clicked
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Store where mouse was clicked, Y is backwards.
        points.emplace_back(Point((d)x,(d)((d)SCREEN_HEIGHT - y)));

        for(auto p : points) drawDot(p);
        for(int i = 0 ; i < points.size() - 1; i++) drawLine(points[i], points[i+1]);
        spline();
    }
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(640,500);
    glutInitWindowPosition(100,150);
    glutCreateWindow("rashi170101052 - Assignment 2");
    glutMouseFunc(mouse);
    glutDisplayFunc(display);
    myInit();
    glutMainLoop();
    return 0;
}