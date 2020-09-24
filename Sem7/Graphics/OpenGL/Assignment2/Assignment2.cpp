#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <bits/stdc++.h>
using namespace std;


//Point class for taking the points
class Point {
public:
    float x{}, y{};
    Point()= default;
    Point(float x, float y){this->x = x; this->y = y;}
    void setxy(float x2, float y2)
    {
        x = x2; y = y2;
    }
    //operator overloading for '=' sign
    const Point& operator=(const Point &rPoint)
    {
        x = rPoint.x;
        y = rPoint.y;
        return *this;
    }
    Point plus(float factor, Point ePointF) {
        return Point(x + factor * ePointF.x, y + factor * ePointF.y);
    }
    Point plus(Point ePointF) {
        return plus(1.0f, ePointF);
    }
    Point minus(float factor, Point ePointF) {
        return Point(x - factor * ePointF.x, y - factor * ePointF.y);
    }
    Point minus(Point ePointF) {
        return minus(1.0f, ePointF);
    }
    Point scaleBy(float factor) {
        return Point(factor * x, factor * y);
    }
};


class PolyBezierPathUtil {
    Path computePathThroughKnots(vector<Point> knots) {
        throwExceptionIfInputIsInvalid(knots);

        Path polyBezierPath = new Path();
        Point firstKnot = knots[0);
        polyBezierPath.moveTo(firstKnot.getX(), firstKnot.getY());

        /*
         * variable representing the number of Bezier curves we will join
         * together
         */
        int n = knots.size() - 1;

        if (n == 1) {
            Point lastKnot = knots[1);
            polyBezierPath.lineTo(lastKnot.getX(), lastKnot.getY());
        } else {
            vector<Point> controlPoints = computeControlPoints(n, knots);

            for (int i = 0; i < n; i++) {
                Point targetKnot = knots[i + 1);
                appendCurveToPath(polyBezierPath, controlPoints[i], controlPoints[n + i], targetKnot);
            }
        }

        return polyBezierPath;
    }

    vector<Point> computeControlPoints(int n, vector<Point> knots) {
        vector<Point> result(2 * n, Point());
        vector<Point> target = constructTargetVector(n, knots);
        vector<float> lowerDiag = constructLowerDiagonalVector(n - 1);
        vector<float> mainDiag = constructMainDiagonalVector(n);
        vector<float> upperDiag = constructUpperDiagonalVector(n - 1);

        vector<Point> newTarget(n, Point());
        vector<float> newUpperDiag(n-1, 0.0);

        // forward sweep for control points c_i,0:
        newUpperDiag[0] = upperDiag[0] / mainDiag[0];
        newTarget[0] = target[0].scaleBy(1 / mainDiag[0]);

        for (int i = 1; i < n - 1; i++) {
            newUpperDiag[i] = upperDiag[i] /
                              (mainDiag[i] - lowerDiag[i - 1] * newUpperDiag[i - 1]);
        }

        for (int i = 1; i < n; i++) {
            float targetScale = 1 /
                                      (mainDiag[i] - lowerDiag[i - 1] * newUpperDiag[i - 1]);

            newTarget[i] =
                    (target[i].minus(newTarget[i - 1].scaleBy(lowerDiag[i - 1]))).scaleBy(targetScale);
        }

        // backward sweep for control points c_i,0:
        result[n - 1] = newTarget[n - 1];

        for (int i = n - 2; i >= 0; i--) {
            result[i] = newTarget[i].minus(newUpperDiag[i], result[i + 1]);
        }

        // calculate remaining control points c_i,1 directly:
        for (int i = 0; i < n - 1; i++) {
            result[n + i] = knots[i + 1].scaleBy(2).minus(result[i + 1]);
        }

        result[2 * n - 1] = knots[n].plus(result[n - 1]).scaleBy(0.5f);

        return result;
    }
    vector<Point> constructTargetVector(int n, vector<Point> knots) {
        vector<Point> result (n, Point());

        result[0] = knots[0].plus(2, knots[1]);

        for (int i = 1; i < n - 1; i++) {
            result[i] = (knots[i].scaleBy(2).plus(knots[i + 1])).scaleBy(2);
        }

        result[result.size() - 1] = knots[n - 1].scaleBy(8).plus(knots[n]);

        return result;
    }
    vector<float> constructLowerDiagonalVector(int length) {
        vector<float> result(length, 0.0);

        for (int i = 0; i < result.size() - 1; i++) {
            result[i] = 1;
        }

        result[result.size() - 1] = 2;

        return result;
    }
    vector<float> constructMainDiagonalVector(int n) {
        vector<float> result(n, 0.0);

        result[0] = 2;

        for (int i = 1; i < result.size() - 1; i++) {
            result[i] = 4;
        }

        result[result.size() - 1] = 7;

        return result;
    }
    vector<float> constructUpperDiagonalVector(int length) {
        vector<float> result(length, 0.0);

        for (int i = 0; i < result.size(); i++) {
            result[i] = 1;
        }

        return result;
    }
    void appendCurveToPath(Path path, Point control1, Point control2, Point targetKnot) {
        path.cubicTo(
                control1.getX(),
                control1.getY(),
                control2.getX(),
                control2.getY(),
                targetKnot.getX(),
                targetKnot.getY()
        );
    }
//
//    void throwExceptionIfInputIsInvalid(Collection<Point> knots) {
//        if (knots.size() < 2) {
//            throw new IllegalArgumentException(
//                    "Collection must contain at least two knots"
//            );
//        }
//    }
};

int factorial(int n)
{
    if (n<=1)
        return(1);
    else
        n=n*factorial(n-1);
    return n;
}

float binomial_coff(float n,float k)
{
    float ans;
    ans = factorial(n) / (factorial(k)*factorial(n-k));
    return ans;
}

Point abc[20];
int SCREEN_HEIGHT = 500;
int points = 0;
int clicks = 7;

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
Point drawBezier(Point PT[], double t) {
    Point P;
    P.x = pow((1 - t), 3) * PT[0].x + 3 * t * pow((1 -t), 2) * PT[1].x + 3 * (1-t) * pow(t, 2)* PT[2].x + pow (t, 3)* PT[3].x;
    P.y = pow((1 - t), 3) * PT[0].y + 3 * t * pow((1 -t), 2) * PT[1].y + 3 * (1-t) * pow(t, 2)* PT[2].y + pow (t, 3)* PT[3].y;

    return P;
}
Point drawBezierGeneralized(Point PT[], double t) {
    Point P;
    P.x = 0; P.y = 0;
    for (int i = 0; i<clicks; i++)
    {
        P.x = P.x + binomial_coff((float)(clicks - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (clicks - 1 - i)) * PT[i].x;
        P.y = P.y + binomial_coff((float)(clicks - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (clicks - 1 - i)) * PT[i].y;
    }
    //cout<<P.x<<endl<<P.y;
    //cout<<endl<<endl;
    return P;
}

void myMouse(int button, int state, int x, int y) {
    // If left button was clicked
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Store where mouse was clicked, Y is backwards.
        abc[points].setxy((float)x,(float)(SCREEN_HEIGHT - y));
        points++;

        // Draw the red  dot.
        drawDot(x, SCREEN_HEIGHT - y);


        // If (click-amout) points are drawn do the curve.
        if(points == clicks)
        {
            glColor3f(0.2,1.0,0.0);
            // Drawing the control lines
            for(int k=0;k<clicks-1;k++)
                drawLine(abc[k], abc[k+1]);

            Point p1 = abc[0];
            /* Draw each segment of the curve.Make t increment in smaller amounts for a more detailed curve.*/
            for(double t = 0.0;t <= 1.0; t += 0.0005)
            {
                Point p2 = drawBezierGeneralized(abc,t);
                cout<<p1.x<<"  ,  "<<p1.y<<endl;
                cout<<p2.x<<"  ,  "<<p2.y<<endl;
                cout<<endl;
                drawLine(p1, p2);
                p1 = p2;
            }
            glColor3f(0.0,0.0,0.0);

            points = 0;
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