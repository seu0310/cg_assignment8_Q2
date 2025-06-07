#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include "frame_timer.cpp"
#include "load_mesh.cpp"

float scale = 10.0f;
float tx = 0.1f, ty = -1.0f, tz = -1.5f;

GLuint bunnyVAO;
GLuint vboPositions, vboNormals, eboIndices;

void set_normalized_light_position()
{
    float x = 1.0f, y = 1.0f, z = 1.0f;
    float len = sqrtf(x * x + y * y + z * z);
    GLfloat light_position[] = { x / len, y / len, z / len, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void init_lighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    GLfloat global_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    set_normalized_light_position();
    GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    GLfloat ka[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat kd[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat ks[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
}

void setup_buffers()
{
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<unsigned int> indices;

    for (auto& v : gPositions) {
        positions.push_back(v.x);
        positions.push_back(v.y);
        positions.push_back(v.z);
    }

    for (auto& n : gNormals) {
        normals.push_back(n.x);
        normals.push_back(n.y);
        normals.push_back(n.z);
    }

    for (auto& tri : gTriangles) {
        indices.push_back(tri.indices[0]);
        indices.push_back(tri.indices[1]);
        indices.push_back(tri.indices[2]);
    }

    glGenVertexArrays(1, &bunnyVAO);
    glBindVertexArray(bunnyVAO);

    glGenBuffers(1, &vboPositions);
    glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glNormalPointer(GL_FLOAT, 0, 0);
    glEnableClientState(GL_NORMAL_ARRAY);

    glGenBuffers(1, &eboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);

    init_lighting();
    init_timer();

    load_mesh("bunny.obj");
    setup_buffers();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    start_timing();

    glPushMatrix();

    glTranslatef(tx, ty, tz);
    glScalef(scale, scale, scale);

    glBindVertexArray(bunnyVAO);
    glDrawElements(GL_TRIANGLES, gTriangles.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glPopMatrix();

    float timeElapsed = stop_timing();
    gTotalFrames++;
    gTotalTimeElapsed += timeElapsed;
    float fps = gTotalFrames / gTotalTimeElapsed;
    char string[1024];
    sprintf(string, "OpenGL Bunny (VAO): %.2f FPS", fps);
    glutSetWindowTitle(string);

    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 1280);
    glutCreateWindow("OpenGL Bunny (VAO)");

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        printf("GLEW initialization failed: %s\n", glewGetErrorString(err));
        return -1;
    }

    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}