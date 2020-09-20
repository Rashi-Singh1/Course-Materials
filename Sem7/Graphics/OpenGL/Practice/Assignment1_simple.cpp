#include <iostream>
#include <GL/glut.h>
#include <bits/stdc++.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

bool spin = true;
void keyboard( unsigned char key, int x, int y ){
    if(key == ' ') spin = !spin;
}

float angle = 0;
void timer(int value) {
    if(spin) angle += 0.5;
    glutTimerFunc( 15, timer, 0 );
    glutPostRedisplay();
}

struct Triangle{
    fvec3 p1{}, p2{}, p3{}, normal{};
    Triangle()= default;
    Triangle(fvec3 p1, fvec3 p2, fvec3 p3){
        this->p1 = p1;
        this->p2 = p2;
        this->p3 = p3;
        this->calculate_normal();
    }
    void calculate_normal() {
        /* calculate Vector1 and Vector2 */
        fvec3 vector1, vector2, crossProduct;
        float val;
        vector1.x = this->p1.x - this->p2.x;
        vector1.y = this->p1.y - this->p2.y;
        vector1.z = this->p1.z - this->p2.z;

        vector2.x = this->p1.x - this->p3.x;
        vector2.y = this->p1.y - this->p3.y;
        vector2.z = this->p1.z - this->p3.z;

        /* cross product */
        crossProduct.x = vector1.y * vector2.z - vector2.y * vector1.z;
        crossProduct.y = vector2.x * vector1.z - vector1.x * vector2.z;
        crossProduct.z = vector1.x * vector2.y - vector2.x * vector1.y;

        /* normalization factor */
        val = sqrt( crossProduct.x*crossProduct.x + crossProduct.y*crossProduct.y + crossProduct.z*crossProduct.z );

        this->normal.x = crossProduct.x/val;
        this->normal.y = crossProduct.y/val;
        this->normal.z = crossProduct.z/val;
    }
};

vector<Triangle> triangles;
void readObjFile(const string &file) {
    ifstream objFile(file);
    if(!objFile.is_open()) return;

    vector<fvec3> vertex;

    string line, prefix;
    stringstream stream;

    fvec3 temp3;

    while(getline(objFile, line)) {
        stream.clear();
        stream.str(line);
        stream >> prefix;
        if(prefix == "v") {
            stream >> temp3.x >> temp3.y >> temp3.z;
            vertex.push_back(temp3);
        }
        else if(prefix == "f") {
            string face = stream.str();
            int count = 0;
            string index;
            vector<int> indices;
            for(int i = 0; i < face.size(); i++){
                if(face[i] == 'f') continue;
                else if(face[i] == '/' || face[i] == ' ') {
                    if(!index.empty() && count == 0) indices.push_back(stof(index));
                    count = (face[i] == '/' ? (count+1)%3 : 0);
                    index = "";
                }
                else index.push_back(face[i]);
            }
            if(!index.empty() && count == 0) indices.push_back(stof(index));
            if(indices.size() >= 3) triangles.emplace_back(vertex[indices[0]-1], vertex[indices[1]-1], vertex[indices[2]-1]);
        }
        else ;
    }
}

void initGL() {
    unsigned long uColor = 0x262626;
//    unsigned long uColor = 0x404040;
    float fAlpha = (float)(uColor >> 24) / 0xFF;
    float fRed = (float)((uColor >> 16) & 0xFF) / 0xFF;
    float fGreen = (float)((uColor >> 8) & 0xFF) / 0xFF;
    float fBlue = (float)(uColor & 0xFF) / 0xFF;
    glClearColor(fRed, fGreen, fBlue, fAlpha);
//    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); //Enable lighting
    glShadeModel(GL_SMOOTH); //Enable smooth shading
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Automatically normalize normals
}

void addLighting(){
    //Add ambient light
    //sh that shines everywhere in our scene by the same amount
    //every face gets the same amount
    GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2) and intensity //can be greater than 1 so not like color
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    //Add positioned light
    GLfloat lightColor0[] = {0.3f, 0.3f, 0.3f, 1.0f}; //Color (0.5, 0.5, 0.5)
//    GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
    GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    //Add directed light
    GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
    GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
}

void draw() {
    double w = glutGet( GLUT_WINDOW_WIDTH );
    double h = glutGet( GLUT_WINDOW_HEIGHT );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45, w / h, 0.1, 10 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt( 2, 2, 2, 0, 0, 0, 0, 0, 1 );

    addLighting();

    glTranslatef(0,0,-0.25);
    glRotatef( angle, 0, 0, 1 );
    glRotatef(90, 1, 0, 0);
    glScalef(0.3,0.3,0.3);

    glBegin(GL_TRIANGLES);
    for(auto cur : triangles){
        glVertex3f(cur.p1.x, cur.p1.y, cur.p1.z);
        glVertex3f(cur.p2.x, cur.p2.y, cur.p2.z);
        glVertex3f(cur.p3.x, cur.p3.y, cur.p3.z);
        glNormal3f(cur.normal.x, cur.normal.y, cur.normal.z);
    }
    glEnd();
    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

int main( int argc, char **argv )
{
    if(argc < 2) cout << "Please give the obj input file" <<endl;
    else {
        readObjFile(argv[1]);
        glutInit( &argc, argv );
        glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
        glutInitWindowSize( 640, 480 );
        glutCreateWindow("170101052 - Assignment 1");
        glutDisplayFunc(draw);
        glutReshapeFunc(reshape);
        glutKeyboardFunc(keyboard);
        glutTimerFunc(0, timer, 0);
        initGL();
        glutMainLoop();
    }
    return 0;
}
//g++ main.cpp -lglut -lGLU -lGL -lm