/*****************************************************************************
 *  curves.cpp                                                               *
 *                                                                           *
 *     This program fits a set of data with three different curve fitting    *
 *  algorithms:  Lagrange interpolation, Bezier curves, and uniform cubic    *
 *  B-splines.                                                               *
 *                                                                           *
 *  Author:  Cary Laxer                                                      *
 *                                                                           *
 *  Modification History:                                                    *
 *     February 16, 1992 - Creation.  Program reads data from the provided   *
 *			   data file and determines the range of values in               *
 *			   file.  The data points are plotted in with 'x'                *
 *			   marks in each third of the screen.                            *
 *     February 18, 1992 - Added the routines to do Lagrange interpolation.  *
 *     February 19, 1992 - Added the routine to do Bezier curve approxima-   *
 *			   tion.  Enlarged the window in the y direction so the inter-   *
 *			   polated curve would be completely visible.                    *
 *     February 20, 1992 - Added the routines to do uniform cubic B-spline   *
 *			   approximation of the data.                                    *
 *     February 6, 1994  - Modified to work with GraphX.                     *
 *     December 14, 1997 - Modified to work with OpenGL and glut.            *
 *     November 2, 2000  - Modified to use C++ I/O instead of C I/O.         *
 *****************************************************************************/

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <GL/glut.h>

/* Drawing constants. */
#define WINDOW_SIZE  540  /* initial size of window                             */
#define BORDER        10  /* border width in each viewport                      */
#define MAX_POINTS   100  /* maximum number of control points                   */
#define MARK_FACTOR 0.35  /* scale factor for 'x' that marks each control point */
#define DELTA_T     0.05  /* time step factor for drawing each curve            */

/* Display list constants. */
#define MARK_LIST           1
#define LAGRANGE_LIST       2
#define BEZIER_LIST         3
#define SPLINE_LIST         4
#define LAGRANGE_TITLE_LIST 5
#define BEZIER_TITLE_LIST   6
#define SPLINE_TITLE_LIST   7

double px[MAX_POINTS], py[MAX_POINTS], minx, maxx, miny, maxy, markd;
int number_of_points = 0;
int WindowSizeX = WINDOW_SIZE, WindowSizeY = WINDOW_SIZE;
int WindowSizeY3 = WINDOW_SIZE / 3;

void interact ();
void mark_points ();
void spline ();
void gfxinit ();
void display (void);
void reshape (int width, int height);

using namespace std;

int main (int argc, char** argv)
{
    /* Get input data. */

    interact ();

    /* Set graphics window parameters. */

    glutInit (&argc, argv);
    glutInitWindowSize (WindowSizeX, WindowSizeY);
    glutInitWindowPosition (100, 0);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow ("Curve Fitting");
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    gfxinit ();
    glutMainLoop ();
    return 0;
}

//done
void gfxinit ()
/* This is the routine that generates the image to be displayed. */
{
    glClearColor (1.0, 1.0, 1.0, 0.0); /* Make the background white. */

    /* Generate the three different curves for displaying. */

    mark_points ();          /* Generate the data marks display list.            */
    spline ();               /* Generate the lines for the spline approximation. */
}

//done
void reshape (int width, int height)
/* This is the callback function that gets executed every time the display
   size has changed. */
{
    WindowSizeX = width;
    WindowSizeY = height;
    WindowSizeY3 = height / 3;
}

void display (void)
/* This is the callback function that gets executed every time the display
   needs to be updated.
*/
{
    glClear (GL_COLOR_BUFFER_BIT);
    glViewport (0, 0, WindowSizeX, WindowSizeY);
    glLoadIdentity ();
    gluOrtho2D (minx-markd, maxx+markd, miny-2.0*markd, maxy+2.0*markd);
    glViewport (BORDER, BORDER, WindowSizeX-2*BORDER, WindowSizeY3-2*BORDER);
    glCallList (MARK_LIST);
    glCallList (SPLINE_LIST);

    glutSwapBuffers ();
}

void interact (void)
/* This function gets the input data for the program to process. */
{
    ifstream points_file;

    /* Open data file. */

    points_file.open ("points.dat", ios::in);
    if (!points_file.is_open())
    {
        cerr << "Data file 'points.dat' not found." << endl;
        exit (EXIT_FAILURE);
    }

    /* Read file into arrays, determining maximum and minimum values. */

    maxx = maxy = -1.0e38;
    minx = miny = 1.0e38;
    while (points_file >> px[number_of_points] >> py[number_of_points])
    {
        if (px[number_of_points] < minx) minx = px[number_of_points];
        if (px[number_of_points] > maxx) maxx = px[number_of_points];
        if (py[number_of_points] < miny) miny = py[number_of_points];
        if (py[number_of_points] > maxy) maxy = py[number_of_points];
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

void mark_points ()
/* This routine makes a mark for each data point in the arrays. */
{
    int i;

    glNewList(MARK_LIST, GL_COMPILE);
    glColor3f (1.0, 0.0, 0.0);  /* Draw the marks in red. */
    glBegin (GL_LINES);
    for (i=0; i<number_of_points; i++)
    {
        glVertex2d (px[i] - markd, py[i] - markd);
        glVertex2d (px[i] + markd, py[i] + markd);
        glVertex2d (px[i] - markd, py[i] + markd);
        glVertex2d (px[i] + markd, py[i] - markd);
    }
    glEnd ();
    glEndList ();
}

double b (double t)
/* This function evaluates the uniform cubic B-spline. */
{
    double tp2, tp1, tm2, tm1;

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

void spline ()
/* This function approximates the data with spline curves. */
{
    double xs[MAX_POINTS+4], ys[MAX_POINTS+4];
    double x, y, t, bt1, bt2, bt3, bt4;
    int i;
    char title[] = "Spline Approximation";

    /* Load local arrays with data and make the two endpoints multiple so that
     * they are interpolated. */

    xs[0] = xs[1] = px[0];
    ys[0] = ys[1] = py[0];
    for (i=0; i<number_of_points; i++)
    {
        xs[i+2] = px[i];
        ys[i+2] = py[i];
    }
    xs[number_of_points+2] = xs[number_of_points+3] = px[number_of_points-1];
    ys[number_of_points+2] = ys[number_of_points+3] = py[number_of_points-1];

    /* Compute the values to plot. */

    glNewList (SPLINE_LIST, GL_COMPILE);
    glColor3f (0.0, 0.0, 0.0);  /* Draw curve in black. */
    glBegin (GL_LINE_STRIP);
    glVertex2d (px[0], py[0]);
    for (i=0; i<=number_of_points; i++)
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

    /* Render the title into a display list. */

    glNewList (SPLINE_TITLE_LIST, GL_COMPILE);
    glColor3f (0.0, 0.0, 0.0);  /* Draw title in black. */
    for (i=0; i<(int)strlen(title); i++)
        glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, title[i]);
    glEndList();
}