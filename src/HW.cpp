/*---------------------------------------------------------------------------------
 * This file contains a program, drawing & animating a wind-mill.
 * This program is used to teach LCS and scene graph concepts.
 *
 * by S. K. Ueng, Nov.  2006
 */
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

enum MODE
{
    STOP,
    WALK,
    JUMP,
    GESTURE
};

enum WEAPON
{
    NONE,
    SWORD,
    LANCE
};

WEAPON weapon = NONE;
MODE mode = STOP;

/*-----Define a unit box--------*/
/* Vertices of the box */
float points[][3] = {{0.0, 0.0, 0.0}, {1.5, 0.0, 0.0}, {1.5, 1.5, 0.0}, {0.0, 1.5, 0.0}, {0.0, 0.0, 2.5}, {1.5, 0.0, 2.5}, {1.5, 1.5, 2.5}, {0.0, 1.5, 2.5}};
/* face of box, each face composing of 4 vertices */
int face[][4] = {{0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5}, {4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3}};
/* indices of the box faces */
int cube[6] = {0, 1, 2, 3, 4, 5};

int deltaTime = 0, lastFrame = 0;

/*-----Define GLU quadric objects, a sphere and a cylinder----*/
GLUquadricObj *sphere = NULL, *cylind = NULL;

/*-----Define window size----*/
int width = 800, height = 800;

// Global variables recording rotation angles of H-arm and blades.
float arm_ang = 45.0, bld_ang = 10.0;

double Pi = 3.1415926;
float view_x = 1.0, view_y = 1.0, view_z = 1.0;
float test_x = 0.0, test_y = 0.0, test_z = 0.0;
int ges_fore = 0, gesture = 0, spin = 0, ang_x = 0, ang_y = 0, ang_z = 0, r_x = 0, r_y = 0, r_z = 0, Lfa_ang = 0, Rfa_ang = 0, La_ang = 0, Ra_ang = 0, r_calf = 0, l_calf = 0, r_leg = 0, l_leg = 0, angle_speed = 1, counter = 0, turn_ang = 0;
bool ges = false, D = false, one_side_move = false, jump = false, stop = true, crouch = false, run = false, hold = false, right_hand = true;
float run_speed = 1.0;
float crouch_down = 0.0, calf_down = 0.0, leg_down = 0.0; // no use
int jump_s = 0;

/*-----Translation and rotations of eye coordinate system---*/
float eyeDx = 0.0, eyeDy = 0.0, eyeDz = 0.0;
float eyeAngx = 0.0, eyeAngy = 0.0, eyeAngz = 0.0;
double Eye[3] = {0.0, 30.0, 20.0}, Focus[3] = {0.0, 0.0, 0.0},
	   Vup[3] = {0.0, 1.0, 0.0};

float u[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
float eye[3];
float cv, sv; /* cos(5.0) and sin(5.0) */
float my_near = 1.5, my_far = 1000.0;
float ntop, nleft, nright, nbottom;
float nzoom[4][4] = {{-40.0, 40.0, -40.0, 40}, {-40.0, 40.0, -40.0, 40}, {-40.0, 40.0, -40.0, 40}, {-40.0, 40.0, -40.0, 40}};
int zoom_mode = 1;

/* ----Drawing syle----*/
int style = 4;
int sty_flag = 0;

/*----Define light source properties -------*/
int isLightOn0 = -1, isLightOn1 = -1, isLightOn2 = -1, isLightOn3 = -1, isLightOn4 = 1;
float lit_position[] = {4.0, 0.0, 0.0, 1.0};  // spotlight
float lit_direction[] = {-1.0, -1.0, 1.0, 0.0};
float lit_diffuse[] = {1.0, 1.0, 0.1, 1.0};
float lit_specular[] = {0.7, 0.7, 0.7, 1.0};
float lit_cutoff = 60.0;
float lit_exponent = 8.0;

float lit1_position[] = {-1.5, 15.0, -15.0, 1.0}; // point light
float lit1_ambient[] = {0.7, 0.7, 0.7, 1.0};
float lit1_diffuse[] = {0.3, 0.3, 0.3, 1.0};
float lit1_specular[] = {1.0, 1.0, 1.0, 1.0};
// float lit1_diffuse[] = {0.7, 0.7, 0.0, 1.0};

float lit2_position[] = {1.5, 20.0, -15.0, 0.0}; // directional light
float lit2_diffuse[] = {1.0, 1.0, 1.0, 1.0};
float lit2_ambient[] = {0.7, 0.7, 0.7, 1.0};
bool isFancy = false;
int Light2_ang = 0;

float lit3_position[] = {0.0, 0.0, 0.0, 1.0}; // test
float lit3_diffuse[] = {0.3, 0.3, 0.1, 1.0};
float lit3_ambient[] = {0.3, 0.3, 0.1, 1.0};
float lit3_specular[] = {0.7, 0.7, 0.7, 1.0};
float robot_emission[] = {1.0, 1.0, 1.0, 1.0};

float lit4_position[] = {0.0, 0.0, 1.0, 1.0}; // emission
float lit4_diffuse[] = {0.5, 0.5, 0.5, 1.0};
float lit4_ambient[] = {0.6, 0.6, 0.6, 1.0};
float lit4_specular[] = {0.3, 0.3, 0.3, 1.0};
float light_emission[] = {0.3, 0.1, 0.1, 1.0};
float no_emission[] = {0.0, 0.0, 0.0, 1.0};

float global_ambient[] = {0.2, 0.2, 0.2, 1.0};

float no_specular[] = {0.0, 0.0, 0.0, 1.0};

float flr_diffuse[] = {0.0, 0.0, 0.0, 1.0};
float flr_ambient[] = {0.0, 0.0, 0.0, 1.0};
float flr_specular[] = {0.0, 0.0, 0.0, 1.0};

float obs_diffuse[] = {0.35, 0.3, 0.25, 1.0};
float obs_ambient[] = {0.35, 0.3, 0.25, 1.0};
float obs_shininess = 64.0;

float ruby_ambient[] = {0.1745, 0.01175, 0.01175, 1.0};
float ruby_diffuse[] = {0.61424, 0.04136, 0.04136, 1.0};
float ruby_specular[] = {0.727811, 0.626959, 0.626959, 1.0};
float ruby_shininess = 76.8;

float emerald_ambient[] = {0.0215, 0.1745, 0.0215, 1.0};
float emerald_diffuse[] = {0.07568, 0.61424, 0.07568, 1.0};
float emerald_specular[] = {0.633, 0.727811, 0.633, 1.0};
float emerald_shininess = 76.8;

float silver_ambient[] = {0.19225, 0.19225, 0.19225, 1.0};
float silver_diffuse[] = {0.50754, 0.50754, 0.50754, 1.0};
float silver_specular[] = {0.508273, 0.508273, 0.508273, 1.0};
float silver_shininess = 51.2;

float gold_ambient[] = {0.24725, 0.1995, 0.0745, 1.0};
float gold_diffuse[] = {0.75164, 0.60648, 0.22648, 1.0};
float gold_specular[] = {0.628281, 0.555802, 0.366065, 1.0};
float gold_shininess = 51.2;

float purl_ambient[] = {0.25, 0.20725, 0.20725, 1.0};
float purl_diffuse[] = {1.0, 0.829, 0.829, 1.0};
float purl_specular[] = {0.296648, 0.296648, 0.296648, 1.0};
float purl_shininess = 11.264;

float obsidian_ambient[] = {0.05375, 0.05, 0.06625, 1.0};
float obsidian_diffuse[] = {0.18275, 0.17, 0.22525, 1.0};
float obsidian_specular[] = {0.332741, 0.328634, 0.346435, 1.0};
float obsidian_shininess = 38.4;

float jade_ambient[] = {0.135, 0.2225, 0.1575, 1.0};
float jade_diffuse[] = {0.54, 0.89, 0.63, 1.0};
float jade_specular[] = {0.316228, 0.316228, 0.316228, 1.0};
float jade_shininess = 12.8;

float copper_ambient[] = {0.19125, 0.0735, 0.0225, 1.0};
float copper_diffuse[] = {0.7038, 0.27048, 0.0828, 1.0};
float copper_specular[] = {0.256777, 0.137622, 0.086014, 1.0};
float copper_shininess = 12.8;

float redplastic_ambient[] = {0.0, 0.0, 0.0, 1.0};
float redplastic_diffuse[] = {0.5, 0.0, 0.0, 1.0};
float redplastic_specular[] = {0.7, 0.6, 0.6, 1.0};
float redplastic_shininess = 32.0;

float whiteplastic_ambient[] = {0.0, 0.0, 0.0, 1.0};
float whiteplastic_diffuse[] = {0.55, 0.55, 0.55, 1.0};
float whiteplastic_specular[] = {0.7, 0.7, 0.7, 1.0};
float whiteplastic_shininess = 32.0;

float mat_diffuse[] = {0.8, 0.8, 0.8, 1.0};

/*----Some global variables for transformation------*/
float lit_angle = 0.0;

/*texture mapping*/
unsigned int texture[10];
int t_width, t_height, t_nrC;
bool obs = false;

/*billboard*/
float mtx[16];
float a[3], b[3];//the axes of the billboard

void draw_sword();
void draw_lance();
void draw_stuff();
void CheckLight();

void compute_ab_axes(){
    float w0, w2;
    double len;

    w0 = mtx[2];
    w2 = mtx[10];

    len = sqrt(w0 * w0 + w2 * w2);

    b[0] = 0.0;
    b[1] = 1.0;
    b[2] = 0.0;
    a[0] = w2 / len;
    a[1] = 0.0;
    a[2] = -w0 / len;
}

void FindNormal(float *normals, float *p1, float *p2, float *p3){
    float v1[3], v2[3];
    for (int i = 0; i < 3; i++){
        v1[i] = p2[i] - p1[i];
        v2[i] = p3[i] - p1[i];
    }
    normals[0] = v1[1] * v2[2] - v1[2] * v2[1];
    normals[1] = v1[2] * v2[0] - v1[0] * v2[2];
    normals[2] = v1[0] * v2[1] - v1[1] * v2[0];
    normals[3] = 1.0;
    return;
}

unsigned char *GetTexture(char *filename){
    unsigned char *data;
    data = stbi_load(filename, &t_width, &t_height, &t_nrC, 0);
    return data;
}

void TextureInit(){
    unsigned char *floor = GetTexture("assets/pictures/container.jpg");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(10, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, floor);

    unsigned char *earth = GetTexture("assets/pictures/earth.jpg");
    glBindTexture(GL_TEXTURE_2D, texture[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, earth);

    unsigned char *tnt = GetTexture("assets/pictures/tnt.png");
    glBindTexture(GL_TEXTURE_2D, texture[2]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tnt);

    unsigned char *tntface = GetTexture("assets/pictures/tntface.png");
    glBindTexture(GL_TEXTURE_2D, texture[3]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tntface);

    unsigned char *wall = GetTexture("assets/pictures/wall.jpg");
    glBindTexture(GL_TEXTURE_2D, texture[4]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, wall);

    unsigned char *sky = GetTexture("assets/pictures/sky.jpg");
    glBindTexture(GL_TEXTURE_2D, texture[5]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, sky);

    unsigned char *gun = GetTexture("assets/pictures/gun.jpg");
    glBindTexture(GL_TEXTURE_2D, texture[6]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, gun);

    unsigned char *picture = GetTexture("assets/pictures/picture.jpg");
    glBindTexture(GL_TEXTURE_2D, texture[7]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, picture);

    unsigned char *paint = GetTexture("assets/pictures/paint.jpg");
    glBindTexture(GL_TEXTURE_2D, texture[8]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, paint);
}

/*--------------------------------------------------------
 * Procedure to draw a billboard, center=[x,z], width=w,
 * height = h;
 */
void draw_billboard(float x, float z, float w, float h) {
    float v0[3], v1[3], v2[3], v3[3];

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    /*----Compute the 4 vertices of the billboard ----*/
    v0[0] = x - (w / 2) * a[0];
    v0[1] = 0.0;
    v0[2] = z - (w / 2) * a[2];
    v1[0] = x + (w / 2) * a[0];
    v1[1] = 0.0;
    v1[2] = z + (w / 2) * a[2];
    v2[0] = x + (w / 2) * a[0];
    v2[1] = h;
    v2[2] = z + (w / 2) * a[2];
    v3[0] = x - (w / 2) * a[0];
    v3[1] = h;
    v3[2] = z - (w / 2) * a[2];

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3fv(v0);
    glTexCoord2f(1.0, 0.0);
    glVertex3fv(v1);
    glTexCoord2f(1.0, 1.0);
    glVertex3fv(v2);
    glTexCoord2f(0.0, 1.0);
    glVertex3fv(v3);
    glEnd();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
}

void MyBillboard(){
    glEnable(GL_TEXTURE_2D);
    glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
    compute_ab_axes();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();
    glTranslatef(0.0, 100.0, 0.0);
    glRotatef(180.0, 0.0, 0.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    draw_billboard((0.0) * 4.0, (-5.0) * 4.0, 200.0, 100.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 50.0, 0.0);
    glRotatef(180.0, 0.0, 0.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    draw_billboard((0.0) * 4.0, (-4.0) * 4.0, 50.0, 30.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-60.0, 50.0, 0.0);
    glRotatef(180.0, 0.0, 0.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    draw_billboard((0.0) * 4.0, (-4.0) * 4.0, 15.0, 20.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(50.0, 20.0, 0.0);
    glRotatef(180.0, 0.0, 0.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    draw_billboard((0.0) * 4.0, (-4.0) * 4.0, 15.0, 20.0);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void draw_quad(){
    glNormal3f(0.0, 1.0, 0.0);
    for (int i = -10; i < 10; i++){
        for (int j = -10; j < 10; j++){
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(i * 40.0, 0.0, j * 40.0);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(i * 40.0, 0.0, (j + 1) * 40.0);
            glTexCoord2f(1.0, 1.0);
            glVertex3f((i + 1) * 40.0, 0.0, (j + 1) * 40.0);
            glTexCoord2f(1.0, 0.0);
            glVertex3f((i + 1) * 40.0, 0.0, j * 40.0);
            glEnd();
        }
    }
}

void skybox(){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[5]);

    glPushMatrix();
    glTranslatef(0.0, 70.0, 0.0);
    glRotatef(180, 1.0, 0.0, 0.0);
    draw_quad();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-100.0, 0.0, 0.0);
    glRotatef(-90, 0.0, 0.0, 1.0);
    draw_quad();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(100.0, 0.0, 0.0);
    glRotatef(90, 0.0, 0.0, 1.0);
    draw_quad();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, 100.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    draw_quad();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

/*----------------------------------------------------------
 * Procedure to initialize the working environment.
 */
void myinit()
{
    glClearColor(0.0, 0.0, 0.0, 1.0); /*set the background color BLACK */
    /*Clear the Depth & Color Buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glShadeModel(GL_SMOOTH); /*Enable smooth shading */
    glEnable(GL_DEPTH_TEST); /*Enable depth buffer for shading computing */
    glEnable(GL_NORMALIZE);  /*Enable automatic normalization of normal vectors */
    glEnable(GL_LIGHTING);   /*Enable lighting */
    CheckLight();            /*Enable light sources */
    TextureInit();

    if (cylind == NULL)
    { /* allocate a quadric object, if necessary */
        cylind = gluNewQuadric();
        gluQuadricDrawStyle(cylind, GLU_FILL);
        gluQuadricNormals(cylind, GLU_SMOOTH);
    }

    if (sphere == NULL)
    {
        sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
    }

    cv = cos(5.0 * Pi / 180.0);
    sv = sin(5.0 * Pi / 180.0);

    eye[0] = Eye[0];
    eye[1] = Eye[1];
    eye[2] = Eye[2];

    float h_near, w_near;
    h_near = 2 * tan(90.0 / 2 * Pi / 180.0) * my_near;
    w_near = h_near * width / height;
    nleft = -w_near / 2;
    nright = w_near / 2;
    nbottom = -h_near / 2;
    ntop = h_near / 2;


    glFlush(); /*Enforce window system display the results*/
}

void draw_viewvolume2(){
    float fleft, fright, ftop, fbottom, nl, nr, nt, nb;

    nl = nleft;
    nr = nright;
    nt = ntop ;
    nb = nbottom ;
    fleft = nleft * (my_far / my_near) ;
    fright = nright * (my_far / my_near) ;
    ftop = ntop * (my_far / my_near) ;
    fbottom = nbottom * (my_far / my_near) ;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.0, 0.7, 0.7);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(nl, nt, - my_near);
    glVertex3f(nr, nt, - my_near);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(nl, nb, - my_near);
    glVertex3f(nl, nt, - my_near);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(nr, nb, - my_near);
    glVertex3f(nl, nb, - my_near);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(nr, nb, - my_near);
    glVertex3f(nr, nt, - my_near);
    glEnd();
    

    glColor3f(0.0, 0.7, 0.7);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(fleft, ftop, - my_far);
    glVertex3f(fright, ftop, - my_far);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(fleft, fbottom, - my_far);
    glVertex3f(fleft, ftop, - my_far);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(fright, fbottom, - my_far);
    glVertex3f(fleft, fbottom, - my_far);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(fright, fbottom, - my_far);
    glVertex3f(fright, ftop, - my_far);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


}

/*-------------------------------------------------------
 * Make viewing matrix
 */
void make_view(int x)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (x)
	{
	case 4: /* Perspective */

		/* In this sample program, eye position and Xe, Ye, Ze are computed
		   by ourselves. Therefore, use them directly; no trabsform is
		   applied upon eye coordinate system
		   */
        sty_flag = 1;
		gluLookAt(eye[0], eye[1], eye[2],
				  eye[0] - u[2][0], eye[1] - u[2][1], eye[2] - u[2][2],
				  u[1][0], u[1][1], u[1][2]);
		break;

	case 1: /* X direction parallel viewing */
        sty_flag = 0;
		gluLookAt(30.0, 0.0, 0.0, Focus[0], Focus[1], Focus[2], 0.0, 1.0, 0.0);
		break;
	case 2: /* Y direction parallel viewing */
        sty_flag = 0;
		gluLookAt(0.0, 30.0, 0.0, Focus[0], Focus[1], Focus[2], 1.0, 0.0, 0.0);
		break;
	case 3:
        sty_flag = 0;
		gluLookAt(0.0, 0.0, 30.0, Focus[0], Focus[1], Focus[2], 0.0, 1.0, 0.0);
		break;
	}
}

/*------------------------------------------------------
 * Procedure to make projection matrix
 */
void make_projection(int x)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (x == 4)
	{
            glFrustum(nleft, nright, nbottom, ntop, my_near, my_far);
	}
	else
	{
        if (width > height)
            glOrtho(nzoom[x][0], nzoom[x][1], nzoom[x][2] * (float)height / (float)width,
                    nzoom[x][3] * (float)height / (float)width,
                    -0.0, 100.0);
        else
            glOrtho(nzoom[x][0] * (float)width / (float)height,
                    nzoom[x][1] * (float)width / (float)height, nzoom[x][2], nzoom[x][3],
                    -0.0, 100.0);
    }
	glMatrixMode(GL_MODELVIEW);
}

/*---------------------------------------------------------
 * Procedure to draw view volume, eye position, focus ,...
 * for perspective projection
 */
void draw_view()
{

	glMatrixMode(GL_MODELVIEW);

	/*----Draw Eye position-----*/
	glPushMatrix();
	glTranslatef(eye[0], eye[1], eye[2]);
	glColor3f(0.0, 1.0, 0.0);
	glutWireSphere(1.0, 10, 10);
	glPopMatrix();

	/*----Draw eye coord. axes -----*/
	glColor3f(1.0, 1.0, 0.0); /* Draw Xe */ //yellow
	glBegin(GL_LINES);
	glVertex3f(eye[0], eye[1], eye[2]);
	glVertex3f(eye[0] + 20.0 * u[0][0], eye[1] + 20.0 * u[0][1], eye[2] + 20.0 * u[0][2]);
	glEnd();

	glColor3f(1.0, 0.0, 1.0); /* Draw Ye */ // purple
	glBegin(GL_LINES);
	glVertex3f(eye[0], eye[1], eye[2]);
	glVertex3f(eye[0] + 20.0 * u[1][0], eye[1] + 20.0 * u[1][1], eye[2] + 20.0 * u[1][2]);
	glEnd();

	glColor3f(0.0, 1.0, 1.0); /* Draw Ze */  // cyan
	glBegin(GL_LINES);
	glVertex3f(eye[0], eye[1], eye[2]);
	glVertex3f(eye[0] + 20.0 * u[2][0], eye[1] + 20.0 * u[2][1], eye[2] + 20.0 * u[2][2]);
	glEnd();

    glPushMatrix();
    glTranslatef(eye[0], 0.0, 0.0);
    glTranslatef(0.0, eye[1], 0.0);
    glTranslatef(0.0, 0.0, eye[2]);
    glRotatef(-eyeAngx, 1.0, 0.0, 0.0);
    glRotatef(eyeAngy, 0.0, 1.0, 0.0);
    glRotatef(-eyeAngz, 0.0, 0.0, 1.0);

    if(sty_flag){
        glPushMatrix();
        glLoadIdentity();
        /* position light1 in eye coord sys */
        lit1_position[0] = eye[0];
        lit1_position[1] = eye[1];
        lit1_position[2] = eye[2];
        glLightfv(GL_LIGHT1, GL_POSITION, lit1_position);

        /* Draw light1 position */
        glTranslatef(lit1_position[0], lit1_position[1], lit1_position[2]);
        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
        glColor3f(1.0, 1.0, 0.5);
        // glutSolidSphere(3.5, 12, 12);
        glPopMatrix();
        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
    }
    else{
        draw_viewvolume2();
    }
    glPopMatrix();
}

/*-------------------------------------------------------
 * Procedure to draw three axes and the orign
 */
void draw_axes()
{

    /*----Draw a white sphere to represent the original-----*/
    glColor3f(0.9, 0.9, 0.9);

    gluSphere(sphere, 2.0, /* radius=2.0 */
              12,          /* composing of 12 slices*/
              12);         /* composing of 8 stacks */

    /*----Draw three axes in colors, yellow, meginta, and cyan--*/
    /* Draw Z axis  */
    glColor3f(0.0, 0.95, 0.95);
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
                10.0,             /* height of the cylinder */
                12,               /* use 12-side polygon approximating circle*/
                3);               /* Divide it into 3 sections */

    /* Draw Y axis */
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0); /*Rotate about x by -90', z becomes y */
    glColor3f(0.95, 0.0, 0.95);
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
                10.0,             /* height of the cylinder */
                12,               /* use 12-side polygon approximating circle*/
                3);               /* Divide it into 3 sections */
    glPopMatrix();

    /* Draw X axis */
    glColor3f(0.95, 0.95, 0.0);
    glPushMatrix();
    glRotatef(90.0, 0.0, 1.0, 0.0); /*Rotate about y  by 90', x becomes z */
    gluCylinder(cylind, 0.5, 0.5,   /* radius of top and bottom circle */
                10.0,               /* height of the cylinder */
                12,                 /* use 12-side polygon approximating circle*/
                3);                 /* Divide it into 3 sections */
    glPopMatrix();
    /*-- Restore the original modelview matrix --*/
    glPopMatrix();
}

/*--------------------------------------------------------
 * Procedure to draw a cube. The geometrical data of the cube
 * are defined above.
 */
void draw_ret()
{
    if(obs)
        glEnable(GL_TEXTURE_2D);
    int i;

    float normals[4];
    for (i = 0; i < 6; i++)
    {
        FindNormal(normals, points[face[i][0]], points[face[i][1]], points[face[i][2]]);
        glNormal3fv(normals);

        if((normals[0] == 0.0) && (normals[1] == 0.0 && obs))
            glBindTexture(GL_TEXTURE_2D, texture[3]);
        else
            glBindTexture(GL_TEXTURE_2D, texture[2]);
        
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glBegin(GL_POLYGON); /* Draw the face */
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(points[face[i][0]]);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(points[face[i][1]]);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(points[face[i][2]]);
        glTexCoord2f(1.0, 0.0);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

/*------------------------------------------------------------
 * Procedure to draw a floor of black-and-white tiles.
 */
void draw_floor()
{
    int i, j;

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, flr_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, flr_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, flr_specular);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glNormal3f(0.0, 1.0, 0.0);

    for (i = -1; i < 1; i++){
        for (j = -1; j < 1; j++)
        {
            // if ((i + j) % 2 == 0) glColor3f(0.9, 0.9, 0.9);
            /*else*/ 
            glColor3f(0.9, 0.9, 0.9);
            glBegin(GL_POLYGON);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(i * 100.0, 0.0, j * 100.0);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(i * 100.0, 0.0, (j + 1) * 100.0);
            glTexCoord2f(1.0, 1.0);
            glVertex3f((i + 1) * 100.0, 0.0, (j + 1) * 100.0);
            glTexCoord2f(1.0, 0.0);
            glVertex3f((i + 1) * 100.0, 0.0, j * 100.0);
            glEnd();
        }
    }
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
}

// Draw robot
void draw_robo()
{
    draw_floor();
    glPushMatrix();
    // draw obstacle
    glTranslatef(-5.0, 0.0, 36.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glScalef(10.0, 6.0, 6.0);

    //material properties
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_AMBIENT, obs_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, obs_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, obs_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    obs = true;
    glColor3f(0.35, 0.3, 0.25); //gray like rock
    draw_ret();
    obs = false;

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();

    glTranslatef(test_x, test_y, test_z);

    glPushMatrix();
    glRotatef(turn_ang, 0.0, 1.0, 0.0);

    glTranslatef(0.0, 5.0, 0.0);

    // draw body
    glPushMatrix(); //(0, 5, 0)
    glTranslatef(0.0, 5.0, 0.0);
    glRotatef(-90, 1.0, 0.0, 0.0); // 把z軸座標轉到-y方向
    if (run)
    {
        glRotatef(10, 1.0, 0.0, 0.0);
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, emerald_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emerald_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, emerald_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, emerald_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(0.0, 1.0, 0.0); /* Green colored */
    gluCylinder(cylind, 4, 4, // 沿著z軸畫圓柱
                6,
                12,
                3);

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix(); // 回到(0, 5, 0)

    // draw right leg
    glPushMatrix();
    glTranslatef(-2.0, 5.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    if (run)
    {
        glTranslatef(0.0, 0.0, 1.0);
        glRotatef(-30, 1.0, 0.0, 0.0);
    }
    glRotatef(r_leg, 1.0, 0.0, 0.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, gold_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gold_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, gold_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(1.0, 1.0, 0.0); // Yello color
    gluCylinder(cylind, 1.5, 1.5, 2.5, 12, 3);

    glDisable(GL_COLOR_MATERIAL);

    // draw right knee
    glTranslatef(0.0, 0.5, 3.0);
    glPushMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ruby_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ruby_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ruby_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, ruby_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(1.0, 0.0, 0.0); // Red color
    gluSphere(sphere, 1.5,    /* radius=2.0 */
              12,             /* composing of 12 slices*/
              12);            /* composing of 12 stacks */

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();

    glRotatef(r_calf, 1.0, 0.0, 0.0);

    // draw calf
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    if (run)
    {
        glRotatef(30, 1.0, 0.0, 0.0);
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, purl_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, purl_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, purl_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, purl_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(0.0, 0.0, 1.0); // Blue color
    gluCylinder(cylind, 1.5, 1.5, 6.0, 12, 3);

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();
    glPopMatrix();

    // draw left leg
    glPushMatrix();
    glTranslatef(2.0, 5.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    if (run)
    {
        glTranslatef(0.0, 0.0, 1.0);
        glRotatef(-30, 1.0, 0.0, 0.0);
    }
    glRotatef(l_leg, 1.0, 0.0, 0.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, gold_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gold_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, gold_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(2.0, 1.0, 0.0); // Yello color
    gluCylinder(cylind, 1.5, 1.5, 2.5, 12, 3);

    glDisable(GL_COLOR_MATERIAL);

    // draw left knee
    glTranslatef(0.0, 0.5, 3.0);
    glPushMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ruby_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ruby_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ruby_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, ruby_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(1.0, 0.0, 0.0); // Red color
    gluSphere(sphere, 1.5,    /* radius=2.0 */
              12,             /* composing of 12 slices*/
              12);            /* composing of 12 stacks */

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();

    glRotatef(l_calf, 1.0, 0.0, 0.0);

    // draw calf
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    if (run)
    {
        glRotatef(30, 1.0, 0.0, 0.0);
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, purl_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, purl_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, purl_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, purl_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(0.0, 0.0, 1.0); // Blue color
    gluCylinder(cylind, 1.5, 1.5, 6.0, 12, 3);

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();
    glPopMatrix();

    // draw head
    glPushMatrix();
    glTranslatef(0.0, 13.0, 0.0);

    if (run)
    {
        glTranslatef(0.0, 0.0, 1.0);
        glRotatef(10, 1.0, 0.0, 0.0);
    }

    glLightfv(GL_LIGHT3, GL_POSITION, lit3_position);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, silver_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, silver_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, silver_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, silver_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(1.0, 1.0, 1.0);
    glutSolidCube(6);

    glDisable(GL_COLOR_MATERIAL);

    glTranslatef(-1.5, 0.0, 3.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, obsidian_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, obsidian_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, obsidian_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, obsidian_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(1.0, 6, 6);

    glTranslatef(3.0, 0.0, 0.0);
    glutSolidSphere(1.0, 6, 6);

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();

    // draw left shoulder
    glPushMatrix();
    glTranslatef(4.9, 10.0, 0.0);
    if (run)
    {
        glRotatef(10, 1.0, 0.0, 0.0);
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, silver_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, silver_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, silver_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, silver_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(0.32, 0.32, 0.32);
    glutSolidCube(3);

    glDisable(GL_COLOR_MATERIAL);

    glRotatef(La_ang, 1.0, 0.0, 0.0);
    glRotatef(gesture, 0.0, 0.0, 1.0);

    // draw left upper arm
    glPushMatrix();
    glTranslatef(-0.5, -1.4, -0.5);
    glRotatef(90, 1.0, 0.0, 0.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, jade_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, jade_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, jade_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, jade_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    draw_ret();

    glDisable(GL_COLOR_MATERIAL);

    // draw left elbow
    glTranslatef(0.5, 0.4, 3.0);
    glPushMatrix();
    glScalef(2.0, 2.0, 2.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, silver_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, silver_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, silver_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, silver_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    glColor3f(0.0, 1.0, 1.0);
    gluSphere(sphere, 0.5, 12, 12);

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();

    glRotatef(Lfa_ang, 1.0, 0.0, 0.0);
    glRotatef(ges_fore, 0.0, 0.0, 1.0);

    // draw left forearm
    glPushMatrix();
    glTranslatef(-0.6, -0.4, 0.125);
    if (run)
    {
        glTranslatef(0.0, 0.0, 0.375);
        glRotatef(-60, 1.0, 0.0, 0.0);
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, jade_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, jade_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, jade_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, jade_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);

    draw_ret();

    glDisable(GL_COLOR_MATERIAL);

    glPushMatrix();///////////////////////////////////////////////////light
    glTranslatef(0.75, 2.0, 4.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, obsidian_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, obsidian_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, obsidian_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, obsidian_shininess);
    // glMaterialfv(GL_FRONT, GL_EMISSION, robot_emission);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, light_emission);

    glScalef(1.0, 1.0, 3.0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidCube(1.0);
    glDisable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_POSITION, lit_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lit_direction);

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.75, 1.0, 4.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, 3.0);

    glLightfv(GL_LIGHT4, GL_POSITION, lit4_position);/////////////////////////////////RRRRRRRRRRRRRRRRRRRRRRRRR
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, redplastic_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redplastic_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, redplastic_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, redplastic_shininess);
    if(isLightOn4 == 1){
        glMaterialfv(GL_FRONT, GL_EMISSION, light_emission);
    }
    else if(isLightOn4 == -1){
        glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
    }

    glColor3f(1.0, 0.0, 0.0);
    glutSolidCube(1.0);
    // gluSphere(sphere, 2.0, 12, 12);
    // glutSolidSphere(2.0, 12, 12);


    glDisable(GL_COLOR_MATERIAL);
    
    glPopMatrix();
    if (hold && !right_hand)
    {
        glTranslatef(0.75, 0.0, 2.5);

        // if (weapon == SWORD)
        //     draw_sword();
        // if (weapon == LANCE)
        //     draw_lance();
    }

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // draw right shoulder
    glPushMatrix();
    glTranslatef(-4.9, 10.0, 0.0);
    if (run)
    {
        glRotatef(10, 1.0, 0.0, 0.0);
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, silver_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, silver_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, silver_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, silver_shininess);

    glColor3f(0.32, 0.32, 0.32);
    glutSolidCube(3);

    glDisable(GL_COLOR_MATERIAL);

    glRotatef(Ra_ang, 1.0, 0.0, 0.0);
    glRotatef((-1) * gesture, 0.0, 0.0, 1.0);

    // draw right upper arm
    glPushMatrix();
    glTranslatef(-1.4, -1.3, -0.8);
    glRotatef(90, 1.0, 0.0, 0.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, jade_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, jade_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, jade_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, jade_shininess);
    
    draw_ret();

    glDisable(GL_COLOR_MATERIAL);

    // draw right elbow
    glTranslatef(0.5, 0.4, 3.0);
    glPushMatrix();
    glScalef(2.0, 2.0, 2.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, silver_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, silver_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, silver_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, silver_shininess);

    glColor3f(0.0, 1.0, 1.0);
    gluSphere(sphere, 0.5, 12, 12);

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();

    // rotate forearm
    glRotatef(Rfa_ang, 1.0, 0.0, 0.0);
    glRotatef((-1) * ges_fore, 0.0, 1.0, 0.0);

    // draw right forearm
    glPushMatrix();
    glTranslatef(-0.5, -0.4, 0.125);
    if (run)
    {
        glTranslatef(0.0, 0.0, 0.375);
        glRotatef(-60, 1.0, 0.0, 0.0);
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, jade_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, jade_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, jade_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, jade_shininess);

    draw_ret();

    glDisable(GL_COLOR_MATERIAL);

    if (hold && right_hand)
    {
        glTranslatef(0.75, 0.0, 2.5);
        if (weapon == SWORD)
            draw_sword();
        if (weapon == LANCE)
            draw_lance();
    }

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

// draw sword
void draw_sword()
{

    if (!hold || weapon != SWORD)
    {
        glRotatef(spin, 0.0, 1.0, 0.0);
        glScalef(2.0, 2.0, 2.0);
    }

    glPushMatrix();
    glRotatef(-90, 1.0, 0.0, 0.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ruby_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ruby_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ruby_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, ruby_shininess);

    glColor3f(1.0, 0.0, 0.0);

    gluCylinder(cylind, 0.5, 0.5, // 沿著z軸畫圓柱
                2,
                12,
                3);

    glDisable(GL_COLOR_MATERIAL);

    glPushMatrix();
    glTranslatef(0.0, 0.0, 2.5);
    glScalef(3.0, 1.0, 1.0);

    glutWireCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, 5.5);
    glScalef(2.0, 0.1, 5.0);

    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, 8.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(/*sqrt(3) - 1.0*/ 0.0, 0.0, 1.0);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

// draw lance
void draw_lance()
{

    if (!hold || weapon != LANCE)
    {
        // glRotatef(spin, 0.0, 1.0, 0.0);
        glScalef(1.5, 1.5, 1.5);
    }

    glPushMatrix();
    glRotatef(-90, 1.0, 0.0, 0.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, copper_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, copper_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, copper_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, copper_shininess);

    glColor3f(0.5, 0.0, 1.0);
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);

    glDisable(GL_COLOR_MATERIAL);

    float p[][4] = {
        {-1.0, -1.0, 0.0, 1.0},
        {1.0, -1.0, 0.0, 1.0},
        {0.0, 0.0, 3.0, 1.0},
        {1.0, 1.0, 0.0, 1.0},
        {-1.0, 1.0, 0.0, 1.0}
    };
    float normals[4];

    glTranslatef(0.0, 0.0, 10.0);

    glPushMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, copper_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, copper_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, copper_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, copper_shininess);

    FindNormal(normals, p[0], p[1], p[2]);


    glNormal3fv(normals);

    glColor3f(1.0, 0.5, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex4fv(p[0]);
    glVertex4fv(p[1]);
    glVertex4fv(p[2]);
    glEnd();

    FindNormal(normals, p[1], p[3], p[2]);
    glNormal3fv(normals);

    glBegin(GL_TRIANGLES);
    glVertex4fv(p[1]);
    glVertex4fv(p[3]);
    glVertex4fv(p[2]);
    glEnd();

    FindNormal(normals, p[3], p[4], p[2]);
    glNormal3fv(normals);

    glBegin(GL_TRIANGLES);
    glVertex4fv(p[3]);
    glVertex4fv(p[4]);
    glVertex4fv(p[2]);
    glEnd();

    FindNormal(normals, p[0], p[4], p[2]);
    glNormal3fv(normals);

    glBegin(GL_TRIANGLES);
    glVertex4fv(p[0]);
    glVertex4fv(p[4]);
    glVertex4fv(p[2]);
    glEnd();

    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();

    glPopMatrix();
}

void draw_stuff(){
    draw_axes();
    glPushMatrix();
    glTranslatef(-30.0, 0.0, 0.0);
    draw_sword();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-30.0, 0.0, 10.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-10.0, 0.0, 0.0);
    glVertex3f(10.0, 0.0, 0.0);
    glEnd();
    glTranslatef(10.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, -20.0);
    glVertex3f(0.0, 0.0, 20.0);
    glEnd();
    glTranslatef(0.0, 0.0, -20.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-20.0, 0.0, 0.0);
    glEnd();
    glTranslatef(0.0, 0.0, 40.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-20.0, 0.0, 0.0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glTranslatef(-30.0, 5.0, 20.0);
    glRotatef(spin, 0.0, 1.0, 0.0);

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glMaterialfv(GL_FRONT, GL_AMBIENT, whiteplastic_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteplastic_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, whiteplastic_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, whiteplastic_shininess);

    gluQuadricTexture(sphere, GL_TRUE);
    glColor3f(1.0, 1.0, 1.0);
    gluSphere(sphere, 5.0, 12, 12);
    // glutSolidSphere(5.0, 12, 12);

    // draw_lance();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}
/*draw scene*/
void draw_scene(){
    draw_stuff();

    glPushMatrix();
    draw_robo();
    glPopMatrix();
}
/*-------------------------------------------------------
 * Display and Idle callback func. This func. draws three
 * cubes at proper places to simulate a solar system.
 */
void display()
{
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;
    /*Clear previous frame and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*----Define the current eye position and the eye-coordinate system---*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); /* Initialize modelview matrix */

    /*-------Draw the base of the windmill which is a cube----*/
    switch (style)
    {
    case 0:
        make_view(4);
        make_projection(4);
        glViewport(width / 2, 0, width / 2, height / 2);
        draw_scene();
        make_view(4);
        draw_view();

        make_view(1);
        make_projection(1);
        glViewport(0, height / 2, width / 2, height / 2);
        draw_scene();
        make_view(1);
        draw_view();

        make_view(2);
        make_projection(2);
        glViewport(width / 2, height / 2, width / 2, height / 2);
        draw_scene();
        make_view(2);
        draw_view();

        make_view(3);
        make_projection(3);
        glViewport(0, 0, width / 2, height / 2);
        draw_scene();
        make_view(3);
        draw_view();
        break;
    case 1:
        make_view(1);
        make_projection(1);
        glViewport(0, 0, width, height);
        draw_scene();
        make_view(1);
        draw_view();
        break;
    case 2:
        make_view(2);
        make_projection(2);
        glViewport(0, 0, width, height);
        draw_scene();
        make_view(2);
        draw_view();
        break;
    case 3:
        make_view(3);
        make_projection(3);
        glViewport(0, 0, width, height);
        draw_scene();
        make_view(3);
        draw_view();
        break;
    case 4:
        make_view(4);
        make_projection(4);
        glViewport(0, 0, width, height);
        draw_scene();
        make_view(4);
        draw_view();
        break;
    }
    CheckLight();
    MyBillboard();
    skybox();
    /*-------Swap the back buffer to the front --------*/
    glutSwapBuffers();
    glFlush(); /*--Display the results----*/
}

void zoom(unsigned char key){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch(style){
        case 0:
            if(key == 'k'){
                float factor = 1.2;
                nleft *= factor;
                nright *= factor;
                ntop *= factor;
                nbottom *= factor;
            }
            else{
                float factor = 5.0 / 6.0;
                nleft *= factor;
                nright *= factor;
                ntop *= factor;
                nbottom *= factor;
            }
            break;
        case 1:
            if(key == 'k'){
                float factor = 1.2;
                for (int i = 0; i < 4; i++){
                    nzoom[1][i] *= factor;
                }
            }
            else{
                float factor = 5.0 / 6.0;
                for (int i = 0; i < 4; i++){
                    nzoom[1][i] *= factor;
                }
            }
            break;
        case 2:
            if(key == 'k'){
                float factor = 1.2;
                for (int i = 0; i < 4; i++){
                    nzoom[2][i] *= factor;
                }
            }
            else{
                float factor = 5.0 / 6.0;
                for (int i = 0; i < 4; i++){
                    nzoom[2][i] *= factor;
                }
            }
            break;
        case 3:
            if(key == 'k'){
                float factor = 1.2;
                for (int i = 0; i < 4; i++){
                    nzoom[3][i] *= factor;
                }
            }
            else{
                float factor = 5.0 / 6.0;
                for (int i = 0; i < 4; i++){
                    nzoom[3][i] *= factor;
                }
            }
            break;
        case 4:
            if(key == 'k'){
                float factor = 1.2;
                nleft *= factor;
                nright *= factor;
                ntop *= factor;
                nbottom *= factor;
            }
            else{
                float factor = 5.0 / 6.0;
                nleft *= factor;
                nright *= factor;
                ntop *= factor;
                nbottom *= factor;
            }
            break;
    }
    glMatrixMode(GL_MODELVIEW);
}

/*--------------------------------------------------
 * Reshape callback function which defines the size
 * of the main window when a reshape event occurrs.
 */
void my_reshape(int w, int h)
{
    width = w;
    height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (double)w / (double)h, my_near, my_far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*--------------------------------------------------
 * Keyboard callback func. When a 'q' key is pressed,
 * the program is aborted.
 */
void my_keyboard(unsigned char key, int ix, int iy)
{
    if (key == ' ')
    {
        mode = JUMP;
        jump = true;
        if (jump_s < 5)
        {
            jump_s++;
        }
    }
    if (key == 27)
        exit(0);
    if (key == '.')
    {
        if (!crouch)
        {
            test_y = -2.3;
            r_leg = l_leg = -45;
            r_calf = l_calf = 90;
            crouch = true;
        }
        else
        {
            test_y = 0.0;
            r_leg = l_leg = 0;
            r_calf = l_calf = 0;
            crouch = false;
        }
    }
    if (key == 'p')
    {
        hold = false;
        weapon = NONE;
    }
    if (key == 'g')
    {
        mode = GESTURE;
        ges = true;
    }
    if (key == 'o')
        right_hand = !right_hand;
    if (key == 'w')
        mode = WALK;
    if (key == 's')
        mode = STOP;
    if (key == 'r')
    {
        run = true;
        if (run_speed < 3.0)
        {
            run_speed += 0.4;
        }
    }
    // if (key == 'd' || key == 'D')
    // {
    //     D = (D == true) ? false : true;
    //     cout << "status: " << D << endl;
    // }
    // if (D)
    // {
    //     if (key == 'q')
    //     {
    //         if ((La_ang - 1) < -180)
    //             return;
    //         La_ang = (La_ang - 1) % 360;
    //     }
    //     if (key == 'e')
    //     {
    //         if ((Ra_ang - 1) < -180)
    //             return;
    //         Ra_ang = (Ra_ang - 1) % 360;
    //     }
    //     if (key == 'z')
    //     {
    //         if ((Lfa_ang - 1) < -120)
    //             return;
    //         Lfa_ang = (Lfa_ang - 1) % 360;
    //     }
    //     if (key == 'c')
    //     {
    //         if ((Rfa_ang - 1) < -120)
    //             return;
    //         Rfa_ang = (Rfa_ang - 1) % 360;
    //     }
    //     if (key == '1')
    //         test_x -= 0.1;
    //     if (key == '2')
    //         test_y -= 0.1;
    //     if (key == '3')
    //         test_z -= 0.1;
    //     if (key == '4')
    //         ang_x = (ang_x - 1) % 360;
    //     if (key == '5')
    //         ang_y = (ang_y - 1) % 360;
    //     if (key == '6')
    //         ang_z = (ang_z - 1) % 360;
    //     if (key == '7')
    //         r_x = (r_x - 5) % 360;
    //     if (key == '8')
    //         r_y = (r_y - 5) % 360;
    //     if (key == '9')
    //         r_z = (r_z - 5) % 360;
    // }
    // else
    // {
    //     if (key == 'q')
    //     {
    //         if ((La_ang + 1) > 30)
    //             return;
    //         La_ang = (La_ang + 1) % 360;
    //     }
    //     if (key == 'e')
    //     {
    //         if ((Ra_ang + 1) > 30)
    //             return;
    //         Ra_ang = (Ra_ang + 1) % 360;
    //     }
    //     if (key == 'z')
    //     {
    //         if ((Lfa_ang + 1) > 0)
    //             return;
    //         Lfa_ang = (Lfa_ang + 1) % 360;
    //     }
    //     if (key == 'c')
    //     {
    //         if ((Rfa_ang + 1) > 0)
    //             return;
    //         Rfa_ang = (Rfa_ang + 1) % 360;
    //     }
    if (key == '0')
        style = 0;
    if (key == '1')
        style = 1;
    if (key == '2')
        style = 2;
    if (key == '3')
        style = 3;
    if (key == '4')
        style = 4;
    // Transform the EYE coordinate system
    int i;
    float x[3], y[3], z[3];

    if (key == 'u')
	{
		eyeDy += 0.5; /* move up */
		for (i = 0; i < 3; i++)
			eye[i] -= 0.5 * u[1][i];
	}
	else if (key == 'd')
	{
		eyeDy += -0.5; /* move down */
		for (i = 0; i < 3; i++)
			eye[i] += 0.5 * u[1][i];
	}
	else if (key == 'W')
	{
		eyeDx += -0.5; /* move left */
		for (i = 0; i < 3; i++)
			eye[i] += 0.5 * u[0][i];
	}
	else if (key == 'e')
	{
		eyeDx += 0.5; /* move right */
		for (i = 0; i < 3; i++)
			eye[i] -= 0.5 * u[0][i];
	}
	else if (key == 'n')
	{
		eyeDz += 0.5; /* zoom in */
		for (i = 0; i < 3; i++)
			eye[i] -= 0.5 * u[2][i];
	}
	else if (key == 'f')
	{
		eyeDz += -0.5; /* zoom out */
		for (i = 0; i < 3; i++)
			eye[i] += 0.5 * u[2][i];
	}
	else if (key == 'p')
	{ /* pitching 對X軸旋轉 */ 
		eyeAngx += 5.0;
		if (eyeAngx > 360.0)
			eyeAngx -= 360.0;
		y[0] = u[1][0] * cv - u[2][0] * sv;
		y[1] = u[1][1] * cv - u[2][1] * sv;
		y[2] = u[1][2] * cv - u[2][2] * sv;

		z[0] = u[2][0] * cv + u[1][0] * sv;
		z[1] = u[2][1] * cv + u[1][1] * sv;
		z[2] = u[2][2] * cv + u[1][2] * sv;

		for (i = 0; i < 3; i++)
		{
			u[1][i] = y[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'P')
	{
		eyeAngx += -5.0;
		if (eyeAngx < 0.0)
			eyeAngx += 360.0;
		y[0] = u[1][0] * cv + u[2][0] * sv;
		y[1] = u[1][1] * cv + u[2][1] * sv;
		y[2] = u[1][2] * cv + u[2][2] * sv;

		z[0] = u[2][0] * cv - u[1][0] * sv;
		z[1] = u[2][1] * cv - u[1][1] * sv;
		z[2] = u[2][2] * cv - u[1][2] * sv;

		for (i = 0; i < 3; i++)
		{
			u[1][i] = y[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'h')
	{ /* heading 對Y軸旋轉 */
		eyeAngy += 5.0;
		if (eyeAngy > 360.0)
			eyeAngy -= 360.0;
		for (i = 0; i < 3; i++)
		{
			x[i] = cv * u[0][i] - sv * u[2][i];
			z[i] = sv * u[0][i] + cv * u[2][i];
		}
		for (i = 0; i < 3; i++)
		{
			u[0][i] = x[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'H')
	{
		eyeAngy += -5.0;
		if (eyeAngy < 0.0)
			eyeAngy += 360.0;
		for (i = 0; i < 3; i++)
		{
			x[i] = cv * u[0][i] + sv * u[2][i];
			z[i] = -sv * u[0][i] + cv * u[2][i];
		}
		for (i = 0; i < 3; i++)
		{
			u[0][i] = x[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'r')
	{ /* rolling 對z軸旋轉 */
		eyeAngz += 5.0;
		if (eyeAngz > 360.0)
			eyeAngz -= 360.0;
		for (i = 0; i < 3; i++)
		{
			x[i] = cv * u[0][i] - sv * u[1][i];
			y[i] = sv * u[0][i] + cv * u[1][i];
		}
		for (i = 0; i < 3; i++)
		{
			u[0][i] = x[i];
			u[1][i] = y[i];
		}
	}
	else if (key == 'R')
	{
		eyeAngz += -5.0;
		if (eyeAngz < 0.0)
			eyeAngz += 360.0;
		for (i = 0; i < 3; i++)
		{
			x[i] = cv * u[0][i] + sv * u[1][i];
			y[i] = -sv * u[0][i] + cv * u[1][i];
		}
		for (i = 0; i < 3; i++)
		{
			u[0][i] = x[i];
			u[1][i] = y[i];
		}
	}
    else if(key == '+'){
        if (zoom_mode)
            zoom_mode = 0;
        else
            zoom_mode = 1;
    }
    else if(key == 'k' || key == 'l')
    {
        zoom(key);
    }
    display();
}

void CheckLight(){
    if (isLightOn0 == 1)
    {
        glEnable(GL_LIGHT0);
    }
    else
    {
        glDisable(GL_LIGHT0);
    }
    if (isLightOn1 == 1)
    {
        glEnable(GL_LIGHT1);
    }
    else
    {
        glDisable(GL_LIGHT1);
    }
    if (isLightOn2 == 1)
    {
        glEnable(GL_LIGHT2);
    }
    else
    {
        glDisable(GL_LIGHT2);
    }
    if(isLightOn3 == 1){
        glEnable(GL_LIGHT3);
    }
    else{
        glDisable(GL_LIGHT3);
    }
    if(isLightOn4 == 1){
        glEnable(GL_LIGHT4);
    }
    else{
        glDisable(GL_LIGHT4);
    }
    if(isFancy){
        srand(time(NULL));
        lit2_diffuse[0] = (float)rand() / (float)(RAND_MAX / 1.0);
        lit2_diffuse[1] = (float)rand() / (float)(RAND_MAX / 1.0);
        lit2_diffuse[2] = (float)rand() / (float)(RAND_MAX / 1.0);
        lit2_ambient[0] = 0.7;
        lit2_ambient[1] = 0.7;
        lit2_ambient[2] = 0.7;
    }

    // /*define light0 spot light */
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, lit_cutoff);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, lit_exponent);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lit_specular);

    // /*define light1 point light */
    glLightfv(GL_LIGHT1, GL_AMBIENT, lit1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lit1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lit1_specular);
    // glLightfv(GL_LIGHT1, GL_POSITION, lit1_position);

    // /*define light2 directional light */
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lit2_diffuse);
    glLightfv(GL_LIGHT2, GL_AMBIENT, lit2_ambient);
    // glLightfv(GL_LIGHT2, GL_SPECULAR, lit_specular);
    glPushMatrix();
    glRotatef(Light2_ang, 0.0, 0.0, 1.0);
    glLightfv(GL_LIGHT2, GL_POSITION, lit2_position);
    glPopMatrix();

    // /*define light3 ambient light */
    glLightfv(GL_LIGHT3, GL_AMBIENT, lit3_ambient);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lit3_diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, lit3_specular);

    // /*define light4 spot light */
    glLightfv(GL_LIGHT4, GL_AMBIENT, lit4_ambient);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, lit4_diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, lit4_specular);


    // /*define global ambient light */
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
}

void MainMenu(int value){
    switch(value){
        case 0:
            glShadeModel(GL_FLAT);
            break;
        case 1:
            glShadeModel(GL_SMOOTH);
            break;
        case 2:
            exit(0);
            break;
        case 3:
            isLightOn3 *= -1;
            break;
        case 4:
            isLightOn4 *= -1;
            break;
    }
}

void Light0Menu(int value){
    switch(value){
        case 0: //TURN ON/OFF
            isLightOn0 *= -1;
            break;
        case 1: // add up cut off
            if(lit_cutoff < 90){
                lit_cutoff += 5;
            }
            break;
        case 2:
            if(lit_cutoff > 15){
                lit_cutoff -= 5;
            }
            break;
        case 3:
            for (int i = 0; i < 3; i++){
                if(lit_diffuse[i] > 0.0)
                    lit_diffuse[i] -= 0.1;
                if(lit_specular[i] > 0.0)
                    lit_specular[i] -= 0.1;
            }
            break;
        case 4:
            for (int i = 0; i < 3; i++){
                if (lit_diffuse[i] < 1.0)
                    lit_diffuse[i] += 0.1;
                if (lit_specular[i] < 1.0)
                    lit_specular[i] += 0.1;
            }
            break;
        case 5: //yellow
            lit_diffuse[0] = 1.0;
            lit_diffuse[1] = 1.0;
            lit_diffuse[2] = 0.1;
            lit_specular[0] = 0.7;
            lit_specular[1] = 0.7;
            lit_specular[2] = 0.7;
            break;
        case 6: //red
            lit_diffuse[0] = 1.0;
            lit_diffuse[1] = 0.1;
            lit_diffuse[2] = 0.1;
            lit_specular[0] = 0.7;
            lit_specular[1] = 0.7;
            lit_specular[2] = 0.7;
            break;
        case 7: //green
            lit_diffuse[0] = 0.1;
            lit_diffuse[1] = 1.0;
            lit_diffuse[2] = 0.1;
            lit_specular[0] = 0.7;
            lit_specular[1] = 0.7;
            lit_specular[2] = 0.7;
            break;
        case 8: //blue
            lit_diffuse[0] = 0.1;
            lit_diffuse[1] = 0.1;
            lit_diffuse[2] = 1.0;
            lit_specular[0] = 0.7;
            lit_specular[1] = 0.7;
            lit_specular[2] = 0.7;
            break;
        case 9: //white
            lit_diffuse[0] = 1.0;
            lit_diffuse[1] = 1.0;
            lit_diffuse[2] = 1.0;
            lit_specular[0] = 0.7;
            lit_specular[1] = 0.7;
            lit_specular[2] = 0.7;
            break;
    }
}

void Light1Menu(int value){
    switch(value){
        case 0: //TURN ON/OFF
            isLightOn1 *= -1;
            break;
        case 1: //default
            lit1_diffuse[0] = 0.3;
            lit1_diffuse[1] = 0.3;
            lit1_diffuse[2] = 0.3;
            lit1_ambient[0] = 0.7;
            lit1_ambient[1] = 0.7;
            lit1_ambient[2] = 0.7;
            lit1_specular[0] = 1.0;
            lit1_specular[1] = 1.0;
            lit1_specular[2] = 1.0;
            break;
        case 2: //white
            lit1_diffuse[0] = 1.0;
            lit1_diffuse[1] = 1.0;
            lit1_diffuse[2] = 1.0;
            lit1_ambient[0] = 0.7;
            lit1_ambient[1] = 0.7;
            lit1_ambient[2] = 0.7;
            lit1_specular[0] = 1.0;
            lit1_specular[1] = 1.0;
            lit1_specular[2] = 1.0;
            break;
        case 3: //red
            lit1_diffuse[0] = 1.0;
            lit1_diffuse[1] = 0.1;
            lit1_diffuse[2] = 0.1;
            lit1_ambient[0] = 0.7;
            lit1_ambient[1] = 0.7;
            lit1_ambient[2] = 0.7;
            lit1_specular[0] = 1.0;
            lit1_specular[1] = 1.0;
            lit1_specular[2] = 1.0;
            break;
        case 4: //green
            lit1_diffuse[0] = 0.1;
            lit1_diffuse[1] = 1.0;
            lit1_diffuse[2] = 0.1;
            lit1_ambient[0] = 0.7;
            lit1_ambient[1] = 0.7;
            lit1_ambient[2] = 0.7;
            lit1_specular[0] = 1.0;
            lit1_specular[1] = 1.0;
            lit1_specular[2] = 1.0;
            break;
        case 5: //blue
            lit1_diffuse[0] = 0.1;
            lit1_diffuse[1] = 0.1;
            lit1_diffuse[2] = 1.0;
            lit1_ambient[0] = 0.7;
            lit1_ambient[1] = 0.7;
            lit1_ambient[2] = 0.7;
            lit1_specular[0] = 1.0;
            lit1_specular[1] = 1.0;
            lit1_specular[2] = 1.0;
            break;
        case 6: //increase the intensity
            if (lit1_diffuse[0] < 1.0)
                lit1_diffuse[0] += 0.1;
            if (lit1_diffuse[1] < 1.0)
                lit1_diffuse[1] += 0.1;
            if (lit1_diffuse[2] < 1.0)
                lit1_diffuse[2] += 0.1;
            break;
        case 7: //decrease the intensity
            if (lit1_diffuse[0] > 0.0)
                lit1_diffuse[0] -= 0.1;
            if (lit1_diffuse[1] > 0.0)
                lit1_diffuse[1] -= 0.1;
            if (lit1_diffuse[2] > 0.0)
                lit1_diffuse[2] -= 0.1;
            break;
    }
}

void Light2Menu(int value){
    switch(value){
        case 0: //TURN ON/OFF
            isLightOn2 *= -1;
            break;
        case 1: //default
            isFancy = false;
            lit2_diffuse[0] = 0.0;
            lit2_diffuse[1] = 0.7;
            lit2_diffuse[2] = 0.7;
            lit2_ambient[0] = 0.0;
            lit2_ambient[1] = 0.7;
            lit2_ambient[2] = 0.7;
            break;
        case 2://white
            isFancy = false;
            lit2_diffuse[0] = 1.0;
            lit2_diffuse[1] = 1.0;
            lit2_diffuse[2] = 1.0;
            lit2_ambient[0] = 0.7;
            lit2_ambient[1] = 0.7;
            lit2_ambient[2] = 0.7;
            break;
        case 3://red
            isFancy = false;
            lit2_diffuse[0] = 1.0;
            lit2_diffuse[1] = 0.1;
            lit2_diffuse[2] = 0.1;
            lit2_ambient[0] = 0.7;
            lit2_ambient[1] = 0.7;
            lit2_ambient[2] = 0.7;
            break;
        case 4://green
            isFancy = false;
            lit2_diffuse[0] = 0.1;
            lit2_diffuse[1] = 1.0;
            lit2_diffuse[2] = 0.1;
            lit2_ambient[0] = 0.7;
            lit2_ambient[1] = 0.7;
            lit2_ambient[2] = 0.7;
            break;
        case 5://blue
            isFancy = false;
            lit2_diffuse[0] = 0.1;
            lit2_diffuse[1] = 0.1;
            lit2_diffuse[2] = 1.0;
            lit2_ambient[0] = 0.7;
            lit2_ambient[1] = 0.7;
            lit2_ambient[2] = 0.7;
            break;
        case 6: //fancy
            isFancy = true;
            break;
    }
}

void createMenu(){
    int SpotLightMenu = glutCreateMenu(Light0Menu);
    glutAddMenuEntry("Turn On/Off", 0);
    glutAddMenuEntry("Add up cut off", 1);
    glutAddMenuEntry("Add down cut off", 2);
    glutAddMenuEntry("low down the intensity", 3);
    glutAddMenuEntry("add up the intensity", 4);
    glutAddMenuEntry("yellow (default)", 5);
    glutAddMenuEntry("red", 6);
    glutAddMenuEntry("green", 7);
    glutAddMenuEntry("blue", 8);
    glutAddMenuEntry("white", 9);

    int PointLightMenu = glutCreateMenu(Light1Menu);
    glutAddMenuEntry("Turn On/Off", 0);
    glutAddMenuEntry("default color", 1);
    glutAddMenuEntry("white", 2);
    glutAddMenuEntry("red", 3);
    glutAddMenuEntry("green", 4);
    glutAddMenuEntry("blue", 5);
    glutAddMenuEntry("increase the intensity", 6);
    glutAddMenuEntry("decrease the intensity", 7);

    int DirectionalLightMenu = glutCreateMenu(Light2Menu);
    glutAddMenuEntry("Turn On/Off", 0);
    glutAddMenuEntry("default color", 1);
    glutAddMenuEntry("white", 2);
    glutAddMenuEntry("red", 3);
    glutAddMenuEntry("green", 4);
    glutAddMenuEntry("blue", 5);
    glutAddMenuEntry("fancy", 6);

    int LightMenu = glutCreateMenu(MainMenu);
    glutAddSubMenu("Spot Light", SpotLightMenu);
    glutAddSubMenu("Point Light", PointLightMenu);
    glutAddSubMenu("Directional Light", DirectionalLightMenu);
    glutAddMenuEntry("Flat Shading", 0);
    glutAddMenuEntry("Smooth Shading", 1);
    glutAddMenuEntry("Exit", 2);
    glutAddMenuEntry("fire Emission Turn On/Off", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Check boundary condition
bool CheckBoundary()
{
    cout << "(" << test_x << "," << test_y << "," << test_z << ")\n";
    float distance = 0.0;
    if (!crouch && !run)
    {
        distance = 0.6;
    }
    if (crouch && !run)
    {
        distance = 0.3;
    }
    if (!crouch && run)
    {
        distance = 0.6 * run_speed;
    }

    if (turn_ang == 0)
    {
        if ((test_z + distance > 23.4 && test_z < 39.6) && (test_x >= -8.6 && test_x <= 13.6))
            return false;
        if ((test_z + distance > -10.0 && test_z < 10.0) && (test_x >= -40.0 && test_x <= -20.0))
        {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_z + distance > 10.0 && test_z < 30.0) && (test_x >= -40.0 && test_x <= -20.0))
        {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }
    if (turn_ang == 90 || turn_ang == -270)
    {
        if ((test_x + distance > -8.6 && test_x < 13.6) && (test_z >= 23.4 && test_z <= 39.6))
            return false;
        if ((test_x + distance > -40.0 && test_x < -20.0) && (test_z >= -10 && test_z <= 10))
        {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_x + distance > -40.0 && test_x < -20.0) && (test_z >= 10 && test_z <= 30))
        {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }
    if (turn_ang == 180 || turn_ang == -180)
    {
        if ((test_z - distance < 39.6 && test_z > 23.4) && (test_x >= -8.6 && test_x <= 13.6))
            return false;
        if ((test_z - distance < 10.0 && test_z > -10.0) && (test_x >= -40.0 && test_x <= -20.0))
        {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_z - distance < 30.0 && test_z > 10.0) && (test_x >= -40.0 && test_x <= -20.0))
        {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }
    if (turn_ang == -90 || turn_ang == 270)
    {
        if ((test_x - distance < 13.6 && test_x > -8.6) && (test_z >= 23.4 && test_z <= 39.6))
            return false;
        if ((test_x - distance < -20.0 && test_x > -40.0) && (test_z >= -10 && test_z <= 10))
        {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_x - distance < -20.0 && test_x > -40.0) && (test_z >= 10 && test_z <= 30))
        {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }

    return true;
}

// stop func
void Stop()
{
    if (counter % angle_speed == 0 && !crouch)
    {
        if (La_ang < 0)
        {
            La_ang += 1;
        }
        if (r_leg < 0 /*&& La_ang % 2 == 0*/)
        {
            r_leg += 1;
        }
        if (Ra_ang < 0)
        {
            Ra_ang += 1;
        }
        if (l_leg < 0 /*&& Ra_ang % 2 == 0*/)
        {
            l_leg += 1;
        }
        if ((La_ang == r_leg && r_leg == Ra_ang) && (Ra_ang == l_leg && l_leg == 0))
            stop = true;
    }
    else if (counter % angle_speed == 0 && crouch)
    {
        if (r_calf < 90)
        {
            r_calf++;
        }
        if (l_calf < 90)
        {
            l_calf++;
            if (l_calf == 90)
                stop = true;
        }
    }
    counter--;
    if (counter == 0)
        stop = true;
}

// moveforward func
void Move()
{
    float distance = 0.0;
    if (!crouch && !run)
    {
        distance = 0.6;
    }
    if (crouch && !run)
    {
        distance = 0.3;
    }
    if (!crouch && run)
    {
        distance = 0.6 * run_speed;
    }
    distance *= deltaTime;
    if (turn_ang == 0 && CheckBoundary())
        test_z += distance;
    if ((turn_ang == 90 || turn_ang == -270) && CheckBoundary())
        test_x += distance;
    if ((turn_ang == 180 || turn_ang == -180) && CheckBoundary())
        test_z -= distance;
    if ((turn_ang == -90 || turn_ang == 270) && CheckBoundary())
        test_x -= distance;
}

void movement()
{
    spin = (spin + 1) % 360;
    if (mode == STOP)
    {
        if (!stop)
        {
            run = false;
            Stop();
        }
    }
    else if (mode == WALK)
    {
        counter++;
        stop = false;
        if ((counter % angle_speed == 0 && !crouch) && !run)
        {
            if (one_side_move)
            {
                if (Ra_ang > -30)
                {
                    Ra_ang -= 1;
                }
                if (l_leg > -15 && Ra_ang % 2 == 0)
                {
                    l_leg -= 1;
                }
                if (La_ang < 0)
                {
                    La_ang += 1;
                }
                if (r_leg < 0 && La_ang % 2 == 0)
                {
                    r_leg += 1;
                }
                if (Ra_ang == -30 && l_leg == -15)
                {
                    Move();
                    one_side_move = false;
                }
            }
            else
            {
                if (La_ang > -30)
                {
                    La_ang -= 1;
                }
                if (r_leg > -15 && La_ang % 2 == 0)
                {
                    r_leg -= 1;
                }
                if (Ra_ang < 0)
                {
                    Ra_ang += 1;
                }
                if (l_leg < 0 && Ra_ang % 2 == 0)
                {
                    l_leg += 1;
                }
                if (La_ang == -30 && r_leg == -15)
                {
                    Move();
                    one_side_move = true;
                }
            }
        }
        else if ((counter % angle_speed == 0 && crouch) && !run)
        {
            if (one_side_move)
            {
                if (l_calf > 45)
                {
                    l_calf -= 1;
                }
                if (r_calf < 90)
                {
                    r_calf += 1;
                }
                if (La_ang < 0)
                {
                    La_ang += 1;
                }
                if (Ra_ang > -30)
                {
                    Ra_ang -= 1;
                }
                if (l_calf == 45)
                {
                    Move();
                    one_side_move = false;
                }
            }
            else
            {
                if (r_calf > 45)
                {
                    r_calf -= 1;
                }
                if (l_calf < 90)
                {
                    l_calf += 1;
                }
                if (Ra_ang < 0)
                {
                    Ra_ang += 1;
                }
                if (La_ang > -30)
                {
                    La_ang -= 1;
                }
                if (r_calf == 45)
                {
                    Move();
                    one_side_move = true;
                }
            }
        }
        else if (counter % angle_speed == 0 && run)
        {
            if (one_side_move)
            {
                if (Ra_ang > -30)
                {
                    Ra_ang -= 1;
                }
                if (La_ang < 15)
                {
                    La_ang += 1;
                }
                if (l_leg > -15)
                {
                    l_leg -= 1;
                }
                if (r_leg < 15)
                {
                    r_leg += 1;
                }
                if (Ra_ang == -30 && l_leg == -15)
                {
                    Move();
                    one_side_move = false;
                }
            }
            else
            {
                if (La_ang > -30)
                {
                    La_ang -= 1;
                }
                if (Ra_ang < 15)
                {
                    Ra_ang += 1;
                }
                if (r_leg > -15)
                {
                    r_leg -= 1;
                }
                if (l_leg < 15)
                {
                    l_leg += 1;
                }
                if (La_ang == -30 && r_leg == -15)
                {
                    Move();
                    one_side_move = true;
                }
            }
        }
    }
    else if (mode == JUMP)
    {
        if (test_y < 1.0 * jump_s && jump)
        {
            test_y += 0.1;
        }
        else
        {
            jump = false;
            jump_s = 0;
        }
        if (!jump && test_y > 0.0)
        {
            test_y -= 0.1;
        }
    }
    else if (mode == GESTURE)
    {
        Stop();
        if (gesture < 120)
        {
            gesture++;
        }
        else if (ges_fore < 90)
        {
            ges_fore++;
        }
        else
        {
            ges = false;
        }
        if (!ges)
        {
            if (ges_fore > 0)
            {
                ges_fore--;
            }
            else if (gesture > 0)
            {
                gesture--;
            }
        }
    }
    glutPostRedisplay();
}

/*---------------------------------------------
 * Idle event callback function, increase blade and arm
 * rotation angles.
 */
void idle_func()
{
    bld_ang += 5.0;
    if (bld_ang > 360.0)
    {
        bld_ang -= 360.0;
        arm_ang += 3.0;
        if (arm_ang > 360.0)
            arm_ang -= 360.0;
    }
    display(); /* show the scene again */
}

void mouse(int button, int state, int x, int y)
{
    // Wheel reports as button 3(scroll up) and button 4(scroll down)
    if ((button == 3) || (button == 4)) // It's a wheel event
    {
        // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
        if (state == GLUT_UP)
            return; // Disregard redundant GLUT_UP events
        printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", x, y);
        if (button == 3)
        {
            view_x += 0.1;
            view_y += 0.1;
            view_z += 0.1;
        }
        else
        {
            if (view_x == 1.0)
                return;
            view_x -= 0.1;
            view_y -= 0.1;
            view_z -= 0.1;
        }
    }
    else
    { // normal button event
        printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
    }
}

void motion(int x, int y)
{
}

void specialKey(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        turn_ang = (turn_ang + 90) % 360;
        break;
    case GLUT_KEY_RIGHT:
        turn_ang = (turn_ang - 90) % 360;
        break;
    case GLUT_KEY_UP:
        Light2_ang = (Light2_ang + 10) % 360;
        break;
    case GLUT_KEY_DOWN:
        Light2_ang = (Light2_ang - 10) % 360;
        break;
    }
}

/*---------------------------------------------------
 * Main procedure which defines the graphics environment,
 * the operation style, and the callback func's.
 */
int main(int argc, char **argv)
{
    /*-----Initialize the GLUT environment-------*/
    glutInit(&argc, argv);

    /*-----Depth buffer is used, be careful !!!----*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("robot");

    myinit(); /*---Initialize other state varibales----*/

    /*----Associate callback func's whith events------*/
    glutDisplayFunc(display);
    // glutIdleFunc(idle_func);    /* Add idle event */
    glutReshapeFunc(my_reshape);
    glutKeyboardFunc(my_keyboard);
    glutMouseFunc(mouse);
    glutIdleFunc(movement);
    glutSpecialFunc(specialKey);
    createMenu();

    glutMainLoop();
    return 1;
}

/*
    視野直視角色
    glRotatef(20.0, 0.0, 1.0, 0.0);
    glRotatef(-50, 1.0, 0.0, 0.0);

    Debug Obj
    glTranslatef(test_x, 0.0, 0.0);
    glTranslatef(0.0, test_y, 0.0);
    glTranslatef(0.0, 0.0, test_z);

    glRotatef(r_x, 1.0, 0.0, 0.0);
    glRotatef(r_y, 0.0, 1.0, 0.0);
    glRotatef(r_z, 0.0, 0.0, 1.0);
*/