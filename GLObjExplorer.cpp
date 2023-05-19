#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    float u, v;
} TexCoord;

typedef struct {
    float x, y, z;
} Normal;

int lastMouseX = 0;
int lastMouseY = 0;
float angleX = 0.0f;
float angleY = 0.0f;
int numFaces = 0;
float objectX = 0.0f;
float objectY = 0.0f;
bool isDoubleClick = false;
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 5.0f;


Vertex* vertices;
TexCoord* texCoords;
Normal* normals;
int numVertices = 0;
int numTexCoords = 0;
int numNormals = 0;
float zoomFactor = 1.0f;
bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255) 
typedef struct {
    int vertexIndex;
    int textureIndex;
    int normalIndex;
} FaceIndex;

FaceIndex* faces;
void renderOBJ(void) {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < numFaces; i++) {
        FaceIndex face = faces[i];
        glVertex3f(vertices[face.vertexIndex - 1].x, vertices[face.vertexIndex - 1].y, vertices[face.vertexIndex - 1].z);
        glTexCoord2f(texCoords[face.textureIndex - 1].u, texCoords[face.textureIndex - 1].v);
        glNormal3f(normals[face.normalIndex - 1].x, normals[face.normalIndex - 1].y, normals[face.normalIndex - 1].z);
    }
    glEnd();
}

void loadOBJ(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            Vertex vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            vertices = (Vertex*)realloc(vertices, (numVertices + 1) * sizeof(Vertex));
            vertices[numVertices] = vertex;
            numVertices++;
        } else if (line[0] == 'v' && line[1] == 't') {
            TexCoord texCoord;
            sscanf(line, "vt %f %f", &texCoord.u, &texCoord.v);
            texCoords = (TexCoord*)realloc(texCoords, (numTexCoords + 1) * sizeof(TexCoord));
            texCoords[numTexCoords] = texCoord;
            numTexCoords++;
        } else if (line[0] == 'v' && line[1] == 'n') {
            Normal normal;
            sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
            normals = (Normal*)realloc(normals, (numNormals + 1) * sizeof(Normal));
            normals[numNormals] = normal;
            numNormals++;
        } else if (line[0] == 'f') {
            FaceIndex face[3];
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &face[0].vertexIndex, &face[0].textureIndex, &face[0].normalIndex,
                &face[1].vertexIndex, &face[1].textureIndex, &face[1].normalIndex,
                &face[2].vertexIndex, &face[2].textureIndex, &face[2].normalIndex
            );
            faces =(FaceIndex*)realloc(faces, (numFaces + 3) * sizeof(FaceIndex));
            faces[numFaces] = face[0];
            faces[numFaces + 1] = face[1];
            faces[numFaces + 2] = face[2];
            numFaces += 3;
        }
    }

    fclose(file);
}
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / height, 0.1f, 100.0f);
}
void mouse(int button, int state, int x, int y) {
    if (button == 3 && state == GLUT_DOWN) {
        // Zoom in
        zoomFactor *= 0.9f;
        glutPostRedisplay();
    } else if (button == 4 && state == GLUT_DOWN) {
        // Zoom out
        zoomFactor *= 1.1f;
        glutPostRedisplay();
    }
     if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (isDoubleClick) {
            // Perform double-click action (e.g., rotation)
            objectX = 5.0f;
            objectY = 5.0f;
        }
        isDoubleClick = !isDoubleClick;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        lastMouseX = x;
        lastMouseY = y;
    }
    
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 5.0f / zoomFactor, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    gluLookAt(cameraX, cameraY, cameraZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glPushMatrix();
    glRotatef(objectX, 1.0f, 0.0f, 0.0f); // Rotate around x-axis
    glRotatef(objectY, 0.0f, 1.0f, 0.0f); // Rotate around y-axis
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
//light 1
    GLfloat light_position[] = { 1.0, 0.5, 1.0, 0.0 };
    GLfloat light_ambient[] = { 0.2, 0.4, 0.4, 1.0 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//light 2
    GLfloat light_position2[] = { 0.0, 1.0, 0.0, 0.0 };
    GLfloat light_ambient2[] = { 0.2, 0.4, 0.4, 1.0 };
    GLfloat light_diffuse2[] = { 0.8, 0.0, 0.0, 1.0 };
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);

    GLfloat material_ambient[] = { 0.1, 0.2, 0.4, 1.0 };
    GLfloat material_diffuse[] = { 0.5, 0.5, 0.8, 1.0 };
    GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat material_shininess[] = { 80.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
    
glPopMatrix();
    renderOBJ();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glutSwapBuffers();
    glutPostRedisplay();
}
void motion(int x, int y) {
    if (isDoubleClick) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;
        objectX += static_cast<float>(dx) * 0.01f;
        objectY -= static_cast<float>(dy) * 0.01f;  // Invert dy to match the coordinate system
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}
void passiveMotion(int x, int y) {
    if (!isDoubleClick) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;
        objectX += static_cast<float>(dy) * 0.01f; // Rotate around x-axis
        //make model to follow mouse more responsive
        //cameraX=objectX;
        //cameraY=objectY;
        objectY += static_cast<float>(dx) * 0.01f; // Rotate around y-axis
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'a': // Left key
            objectY -= 5.0f; // Rotate left
            break;
        case 'd': // Right key
            objectY += 5.0f; // Rotate right
            break;
        case 'w': // Top key
            objectX -= 5.0f; // Rotate top
            break;
        case 's': // Down key
            objectX += 5.0f; // Rotate down
            break;
        case 'q': // Move camera forward
            cameraZ -= 0.1f;
            break;
        case 'e': // Move camera backward
            cameraZ += 0.1f;
            break;
    }
    glutPostRedisplay(); // Trigger display update
}
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: // Move camera left
            cameraX -= 0.5f;
            break;
        case GLUT_KEY_RIGHT: // Move camera right
            cameraX += 0.5f;
            break;
        case GLUT_KEY_UP: // Move camera up
            cameraY += 0.5f;
            break;
        case GLUT_KEY_DOWN: // Move camera down
            cameraY -= 0.5f;
            break;
    }
    glutPostRedisplay(); // Trigger display update
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL .obj Viewer");
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);  
    glutPassiveMotionFunc(passiveMotion);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    loadOBJ("file2.obj");
    glutMainLoop();
    return 0;
}