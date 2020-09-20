#include <iostream>
#include <GL/glut.h>
#include <bits/stdc++.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

bool spin = true;
float size = 1.0;
float angle = 0;

//functions to take care of keyboard key presses for increasing, decreasing size and pausing/rotating the 3D object
void keyboard( unsigned char key, int x, int y ){
    if(key == ' ') spin = !spin;
    if(key == '<') size-=0.1;
    if(key == '>') size+=0.1;
}
void keyPress(int key, int x, int y){
    if(key == GLUT_KEY_LEFT) angle-=90;
    if(key == GLUT_KEY_DOWN) size-=0.1;
    if(key == GLUT_KEY_RIGHT) angle+=90;
    if(key == GLUT_KEY_UP) size+=0.1;
}
void timer(int value) {
    if(spin) angle += 0.5;
    glScalef(size, size, size);
    glutTimerFunc( 15, timer, 0 );
    glutPostRedisplay();
}

//structure to store the points and the normal of a triangle
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

//structure to convert hex color code to the required rgba format
struct Color{
    unsigned long hex;
    float r,g,b,a;
    Color(unsigned long hex){
        this->hex = hex;
        this->convert();
    }
    void convert(){
        this->a = (float)(this->hex >> 24) / 0xFF;
        this->r = (float)((this->hex >> 16) & 0xFF) / 0xFF;
        this->g = (float)((this->hex >> 8) & 0xFF) / 0xFF;
        this->b = (float)(this->hex & 0xFF) / 0xFF;
    }
};

//read the object file line by line to populate "vector<Triangle> triangles"
void readObjFile(const string &file) {
    ifstream objFile(file);
    if(!objFile.is_open()) {
        cout << "File: " << file << " cannot be opened" <<endl;
        return;
    }
    vector<fvec3> vertices;
    fvec3 curVertex;
    stringstream stream;
    string line, prefix;
    while(getline(objFile, line)) {
        stream.clear();
        stream.str(line);
        stream >> prefix;
        if(prefix == "v") {
            stream >> curVertex.x >> curVertex.y >> curVertex.z;
            vertices.push_back(curVertex);
        }
        else if(prefix == "f") {
            string face = stream.str();
            vector<int> indices;
            string index;
            int count = 0;
            for(auto x : face){
                if(x == 'f') continue;
                else if(x == '/' || x == ' ') {
                    if(!index.empty() && count == 0) indices.push_back(stof(index));
                    count = (x == '/' ? (count+1)%3 : 0);
                    index = "";
                }
                else index.push_back(x);
            }
            if(!index.empty() && count == 0) indices.push_back(stof(index));
            if(indices.size() >= 3) triangles.emplace_back(Triangle(vertices[indices[0]-1], vertices[indices[1]-1], vertices[indices[2]-1]));
        }
        else ;
    }
}

//specify culling, lighting, background color and shadeModel etc.
void initGL() {
    Color bg(0x252525);
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glEnable(GL_DEPTH_TEST);                                                                                // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);                                                                                 // Set the type of depth-test
    glShadeModel(GL_SMOOTH);                                                                                //Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);                                                      // Nice perspective corrections
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);                                                                                  //Enable lighting
    glEnable(GL_LIGHT0);                                                                                    //Enable light #0
    glEnable(GL_LIGHT1);                                                                                    //Enable light #1
    glEnable(GL_NORMALIZE);                                                                                 //Automatically normalize normals
}

//specify the light source and color of the light
void addLighting(){
    GLfloat ambientColor[] = {0.18f, 0.18f, 0.18f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    //Add positioned light
    GLfloat lightColor0[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightPos0[] = {8.0f, 4.0f, 8.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    //Add directed light
    GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f};
    GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
}

//reshape the object according to "float size"
void reshape(GLsizei width, GLsizei height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

//draw/plot the triangles stored in "vector<Triangle> triangles"
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
    glScalef(size,size,size);

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

int main( int argc, char **argv )
{
    if(argc < 2) cout << "Please give the obj input file" <<endl;
    else {
        readObjFile(argv[1]);
        glutInit( &argc, argv );
        glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
        glutInitWindowSize( 1000, 600 );
        glutCreateWindow("170101052 - Assignment 1");
        glutDisplayFunc(draw);
        glutReshapeFunc(reshape);
        glutKeyboardFunc(keyboard);
        glutSpecialFunc(keyPress);
        glutTimerFunc(0, timer, 0);
        initGL();
        glutMainLoop();
    }
    return 0;
}

