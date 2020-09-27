#include <iostream>
#include <GL/glut.h>
#include <bits/stdc++.h>
#define WINDOW_SIZE  540  /* initial size of window                             */
#define MAX_POINTS   100  /* maximum number of control points                   */
#define MARK_FACTOR 0.05  /* scale factor for 'x' that marks each control point */
#define DELTA_T     0.005  /* time step factor for drawing each curve            */
#define MARK_LIST      1
#define clicks         4
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

vector<Point> points(MAX_POINTS, Point());
double minx, maxx, miny, maxy, markd;
int number_of_points = 0;
int WindowSizeX = WINDOW_SIZE, WindowSizeY = WINDOW_SIZE;

void mark_points ();
void spline ();
void init (){
    glClearColor (1.0, 1.0, 1.0, 0.0); /* Make the background white. */
    mark_points ();          /* Generate the data marks display list.            */
    spline ();               /* Generate the lines for the spline approximation. */
}
void reshape (int width, int height){
    WindowSizeX = width;
    WindowSizeY = height;
}
void display (){
    glClear (GL_COLOR_BUFFER_BIT);
    glViewport (0, 0, WindowSizeX, WindowSizeY);

    glLoadIdentity ();
    gluOrtho2D (minx-markd, maxx+markd, miny-2.0*markd, maxy+2.0*markd);

    glCallList (MARK_LIST);
    glCallList (clicks);

    glutSwapBuffers ();
}
void readInput (){
    ifstream points_file;
    points_file.open ("points.dat", ios::in);
    if (!points_file.is_open())
    {
        cerr << "Data file 'points.dat' not found." << endl;
        exit (EXIT_FAILURE);
    }
    /* Read file into arrays, determining maximum and minimum values. */

    maxx = maxy = -1.0e38;
    minx = miny = 1.0e38;
    while (points_file >> points[number_of_points].x >> points[number_of_points].y)
    {
        if (points[number_of_points].x < minx) minx = points[number_of_points].x;
        if (points[number_of_points].x > maxx) maxx = points[number_of_points].x;
        if (points[number_of_points].y < miny) miny = points[number_of_points].y;
        if (points[number_of_points].y > maxy) maxy = points[number_of_points].y;
        number_of_points++;
        if (number_of_points == MAX_POINTS)
        {
            cout << "Data arrays are full. If any more data is present it will not be plotted."
                 << endl;
            break;
        }
    }
    points_file.close ();

    /* Determine length of line segments for making 'x' marks. */

    if (maxx-minx > maxy-miny)
        markd = (maxx-minx)/number_of_points * MARK_FACTOR;
    else
        markd = (maxy-miny)/number_of_points * MARK_FACTOR;
}
void mark_points (){
    glNewList(MARK_LIST, GL_COMPILE);
    glColor3f (1.0, 0.0, 0.0);  /* Draw the marks in red. */
    glBegin (GL_LINES);
    for (int i=0; i<number_of_points; i++)
    {
        glVertex2d (points[i].x - markd, points[i].y - markd);
        glVertex2d (points[i].x + markd, points[i].y + markd);
        glVertex2d (points[i].x - markd, points[i].y + markd);
        glVertex2d (points[i].x + markd, points[i].y - markd);
//        drawDot(points[i])
    }
    glEnd ();
    glEndList ();
}

double b (double t){
    double tp2, tp1, tm2, tm1;

    if (t <= -2.0) return 0.0;
    else if (t <= -1.0){
        tp2 = t + 2.0;
        return tp2 * tp2 * tp2 / 6.0;
    }
    else if (t <= 0.0) {
        tp2 = t + 2.0;
        tp1 = t + 1.0;
        tp2 = tp2 * tp2 * tp2 / 6.0;
        tp1 = 2.0 * tp1 * tp1 * tp1 /3.0;
        return tp2 - tp1;
    }
    else if (t <= 1.0) {
        tm1 = 1.0 - t;
        tm2 = 2.0 - t;
        tm1 = 2.0 * tm1 * tm1 * tm1 / 3.0;
        tm2 = tm2 * tm2 * tm2 / 6.0;
        return tm2 - tm1;
    }
    else if (t <= 2.0){
        tm2 = 2.0 - t;
        return tm2 * tm2 * tm2 / 6.0;
    }
    return 0.0;
}
void spline (){
    double xs[MAX_POINTS+4], ys[MAX_POINTS+4];
    double x, y, t, bt1, bt2, bt3, bt4;
    xs[0] = xs[1] = points[0].x;
    ys[0] = ys[1] = points[0].y;
    for (int i=0; i<number_of_points; i++)
    {
        xs[i+2] = points[i].x;
        ys[i+2] = points[i].y;
    }
    xs[number_of_points+2] = xs[number_of_points+3] = points[number_of_points-1].x;
    ys[number_of_points+2] = ys[number_of_points+3] = points[number_of_points-1].y;

    glNewList (clicks, GL_COMPILE);
    glColor3f (0.0, 0.0, 0.0);  /* Draw curve in black. */
    glBegin (GL_LINE_STRIP);
    glVertex2d (points[0].x, points[0].y);
    for (int i=0; i<=number_of_points; i++)
        for (t=DELTA_T; t<1.0+DELTA_T/2.0; t+=DELTA_T)
        {
            bt1 = b(t - 2.0);
            bt2 = b(t - 1.0);
            bt3 = b(t);
            bt4 = b(t + 1.0);
            x = xs[i]*bt4 + xs[i+1]*bt3 + xs[i+2]*bt2 + xs[i+3]*bt1;
            y = ys[i]*bt4 + ys[i+1]*bt3 + ys[i+2]*bt2 + ys[i+3]*bt1;
            glVertex2d (x, y);
        }
    glEnd ();
    glEndList ();
}

int main (int argc, char** argv)
{
    readInput ();
    glutInit (&argc, argv);
    glutInitWindowSize (WindowSizeX, WindowSizeY);
    glutInitWindowPosition (100, 0);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow ("Curve Fitting");
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    init ();
    glutMainLoop ();
    return 0;
}
