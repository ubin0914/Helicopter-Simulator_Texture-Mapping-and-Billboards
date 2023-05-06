//Author: 00857036 資工3A 楊育斌

#include <iostream>
#include <math.h>
#include <vector>
#include <chrono>
#include <ctime>
#include <GL/glut.h>

#include "PublicValue.h"
#include "Helicopter.h"
#include "Environment.h"
#include "Light.h"

using namespace std;
using namespace std::chrono;

/*-----Define GLU quadric objects, a sphere and a cylinder----*/
//GLUquadricObj* sphere = NULL, * cylind = NULL;

Helicopter helicopter = Helicopter();
Environment environment = Environment();
Coordinate lookFrom;
Coordinate lookTo;

void display() {
    /*Clear previous frame and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*----Define the current eye position and the eye-coordinate system---*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    if (focusMode == 1) {
        Coordinate& levelProject = sightMode == 3 ? Coordinate(helicopter.origin_r.x-lookFrom.x,0,helicopter.origin_r.z-lookFrom.z) : Coordinate(helicopter.axisX_r.x,0,helicopter.axisX_r.z);
        if (sightMode == 1) {
            sightLongitude = includedAngle({ 1,0,0 }, levelProject, { 0,1,0 });
            sightLatitude = includedAngle(helicopter.axisX_r, levelProject) * (helicopter.axisX_r.y >= 0 ? 1 : -1);
        }
        else if (sightMode == 2) {
            sightLongitude = includedAngle({ 1,0,0 }, levelProject, { 0,1,0 });
            sightLatitude = includedAngle(helicopter.axisX_r, levelProject) * (helicopter.axisX_r.y >= 0 ? 1 : -1);
        }
        else if (sightMode == 3) {
            sightLongitude = includedAngle({ 1,0,0 }, levelProject, { 0,1,0 });
            sightLatitude = includedAngle(helicopter.origin_r - lookFrom, levelProject) * (helicopter.origin_r.y >= lookFrom.y ? 1 : -1);
        }
    }
    Coordinate sight = { 1, 0, 0 };
    rotateDir(sight, { 0,1,0 }, sightLongitude);
    rotateDir(sight, outerProduct(sight, { 0,1,0 }), sightLatitude);
    if (sightMode == 1) {
        lookFrom = { helicopter.origin_r.x, helicopter.origin_r.y, helicopter.origin_r.z };
        lookTo = lookFrom + sight;
    }
    else if (sightMode == 2) {
        lookTo = { helicopter.origin_r.x, helicopter.origin_r.y, helicopter.origin_r.z };
        lookFrom = lookTo - lensDistance * sight;
        if (lookFrom.y < 0.1) lookFrom.y = 0.1;
        if (spKeyPressing[R_SHIFT_INT]) {
            viewMagnification *= 1.01;
            if (viewMagnification > 8) viewMagnification = 8;
        }
        if (spKeyPressing[R_CTRL_INT]) {
            viewMagnification /= 1.01;
            if (viewMagnification < 1) viewMagnification = 1;
        }
    }
    else if (sightMode == 3) {
        if (spKeyPressing[UPKEY_INT]) lookFrom.y += 1;
        if (spKeyPressing[DOWNKEY_INT]) lookFrom.y -= 1;
        if (spKeyPressing[LEFTKEY_INT]) lookFrom -= outerProduct(sight, { 0,1,0 }).identity() * 1;
        if (spKeyPressing[RIGHTKEY_INT]) lookFrom += outerProduct(sight, { 0,1,0 }).identity() * 1;
        if (spKeyPressing[R_SHIFT_INT]) lookFrom += sight.identity() * 1;
        if (spKeyPressing[R_CTRL_INT]) lookFrom -= sight.identity() * 1;
        lookTo = lookFrom + sight;
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120 / viewMagnification, (double)width / (double)height, viewNear, viewFar);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(lookFrom.x, lookFrom.y, lookFrom.z, lookTo.x, lookTo.y, lookTo.z, 0.0, 1.0, 0.0);

    //---------------------------------------------------------------

    Light::setLights(helicopter);
    environment.updateBlock(floor(helicopter.origin_r.x / 110 + 0.5), floor(helicopter.origin_r.z / 110 + 0.5));
    environment.display();
    helicopter.display();
    environment.drawLightningRod(helicopter);

    glutSwapBuffers();
    glFlush();
}

void myinit() {
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, 499, 499);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);  /*Enable depth buffer for shading computing */
    glEnable(GL_NORMALIZE);   /*Enable mornalization  */

    glEnable(GL_LIGHTING);    /*Enable lighting effects */
    
    Light::initLights();

    /*-----Enable face culling -----*/
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    /*-----Create Texture -----*/
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(4, textName);

    glBindTexture(GL_TEXTURE_2D, textName[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, buildingTex);
    
    glBindTexture(GL_TEXTURE_2D, textName[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, LEDTex);

    glBindTexture(GL_TEXTURE_2D, textName[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, mountainTex);

    glBindTexture(GL_TEXTURE_2D, textName[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, buildingLightTex);

    /*----Set up fog conditions ---*/

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogf(GL_FOG_DENSITY, 0.0001);
    glFogf(GL_FOG_START, 1.0);
    glFogf(GL_FOG_END, 10000.0);

    glFlush();/*Enforce window system display the results*/
}

void my_reshape(int w, int h) {
    width = w;
    height = h;
    
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-15.0, 15.0, -15.0 * (float)h / (float)w, 15.0 * (float)h / (float)w,
            -10.5, 30.0);
    else
        glOrtho(-15.0 * (float)w / (float)h, 15.0 * (float)w / (float)h, -15.0, 15.0,
            -10.5, 30.0);
}

void keyboard_down(unsigned char key, int x, int y) {
    if (key == 'w') {
        if (lastSysTime - lastTimeSingleW < 250) helicopter.tiltMode = 2;
        lastTimeSingleW = lastSysTime;
    }
    if (key == ' ') {
        if (lastSysTime - lastTimeSingleSpace < 250) helicopter.flightMode = 1;
        lastTimeSingleSpace = lastSysTime;
    }
    if (key == 'v') {
        sightMode = sightMode % 3 + 1;
        focusMode = sightMode == 3;
        if (sightMode == 2) viewMagnification = 1.5;
    }
    if (key == 'f') focusMode = !focusMode;
    if (key == '[') {
        blockNum -= 2;
        if(blockNum < 3) blockNum = 3;
        environment.initialBuildings();
    }
    if (key == ']') {
        blockNum += 2;
        if (blockNum > 25) blockNum = 25;
        environment.initialBuildings();
    }
    if (key == 't') {
        clockVarying = (clockVarying == 3600 ? 600 : 3600);
    }
    if (key == 'l') {
        minaretlightMode = !minaretlightMode;
    }
    keyPressing[key] = 1;
    if (key >= 'A' && key <= 'Z') keyPressing[key + 32] = 1;
    if (keyPressing['z']) keyPressing['q'] = 1;
    if (keyPressing['c']) keyPressing['e'] = 1;
}

void keyboard_up(unsigned char key, int x, int y) {
    keyPressing[key] = 0;
    if (key >= 'A' && key <= 'Z') keyPressing[key + 32] = 0;
    if (!keyPressing['z']) keyPressing['q'] = 0;
    if (!keyPressing['c']) keyPressing['e'] = 0;
}

void spKeyboard_down(int key, int x, int y) {
    spKeyPressing[key] = 1;
    //cout << key << "\n";
    if (key == L_SHIFT_INT) {
        helicopter.tiltMode = 1;
    }
    if (key == L_CTRL_INT) {
        if (lastSysTime - lastTimeSingleLCtrl < 250) helicopter.flightMode = 0;
        lastTimeSingleLCtrl = lastSysTime;
    }
}

void spKeyboard_up(int key, int x, int y) {
    spKeyPressing[key] = 0;
}

void mouse_func(int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        if (button == 2 && focusMode == 2) {
            focusMode = 1;
        }
    }
    if (state == GLUT_DOWN) {
        if (button == 1) {
            focusMode = !focusMode;
        }
        if (button == 2 && focusMode == 1) {
            focusMode = 2;
        }
        if (button == 3) {
            if (sightMode == 2) {
                lensDistance /= 1.1;
                if (lensDistance < 8) lensDistance = 8;
            }
            else {
                viewMagnification *= 1.1;
                if (viewMagnification > 20) viewMagnification = 20;
            }
        }
        else if (button == 4) {
            if (sightMode == 2) {
                lensDistance *= 1.1;
                if (lensDistance > 80) lensDistance = 80;
            }
            else {
                viewMagnification /= 1.1;
                if (viewMagnification < 1) viewMagnification = 1;
            }
        }
    }
}

void passive_motion(int x, int y) {
    if (abs(x - lastMouseX) < 100 && abs(y - lastMouseY) < 100) {
        sightLongitude -= (x - lastMouseX) / 3.0;
        sightLatitude -= (y - lastMouseY) / 3.0;

        if (sightLatitude > 89.9) sightLatitude = 89.9; //因為到90度會不知道上方在哪
        else if (sightLatitude < -89.9) sightLatitude = -89.9;
    }
    lastMouseX = x, lastMouseY = y;
}

void mouse_motion(int x, int y) {
    if (focusMode != 2) return;
    if (abs(x - lastMouseX) < 100 && abs(y - lastMouseY) < 100) {
        sightLongitude -= (x - lastMouseX) / 3.0;
        sightLatitude -= (y - lastMouseY) / 3.0;

        if (sightLatitude > 89.9) sightLatitude = 89.9; //因為到90度會不知道上方在哪
        else if (sightLatitude < -89.9) sightLatitude = -89.9;
    }
    lastMouseX = x, lastMouseY = y;
}

void idle_func() {
    //int sec_since_epoch = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    int curSysTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() % 100000000;
    float dt = (curSysTime - lastSysTime) / 1000.0; cout << dt << "\n";
    lastSysTime = curSysTime;
    clockTime += clockVarying * dt;
    if (clockTime > 86400) clockTime -= 86400;

    helicopter.evolve(dt);
    
    if (lastMouseX < 30 && lastMouseX != -999) sightLongitude += 120 * dt;
    else if (lastMouseX > width - 31) sightLongitude -= 120 * dt;
    if (lastMouseY < 30 && lastMouseY != -999) sightLatitude += 120 * dt;
    else if (lastMouseY > height - 31) sightLatitude -= 120 * dt;

    if (sightLatitude > 89.9) sightLatitude = 89.9; //因為到90度會不知道上方在哪
    else if (sightLatitude < -89.9) sightLatitude = -89.9;

    /*if (spKeyPressing[R_SHIFT_INT]) lensDistance -= 30 * dt;
    if (spKeyPressing[R_CTRL_INT]) lensDistance += 30 * dt;
    if (lensDistance < 12) lensDistance = 12;
    else if(lensDistance > 100) lensDistance = 100;*/

    display();
}

void main(int argc, char** argv) {

    /*-----Initialize the GLUT environment-------*/
    glutInit(&argc, argv);

    /*-----Depth buffer is used, be careful !!!----*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow("A Flying Helicopter");

    myinit();      /*---Initialize other state varibales----*/
    
    glutDisplayFunc(display);
    glutIdleFunc(idle_func);
    glutReshapeFunc(my_reshape);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(keyboard_down);
    glutKeyboardUpFunc(keyboard_up);
    glutSpecialFunc(spKeyboard_down);
    glutSpecialUpFunc(spKeyboard_up);
    glutMouseFunc(mouse_func);
    glutPassiveMotionFunc(passive_motion);
    glutMotionFunc(mouse_motion);
    //---------------------------------------------------------------
    
    Coordinate a(0.972809, 0, -0.231604), b(0.972809, 0, -0.231604);
    /*rotateDir(a, b, -45);
    a.print();
    cout << includedAngle(a, b) << "\n";
    cout << acos(innerProduct(a, b) / a.getLength() / b.getLength()) << "\n";
    cout << innerProduct(a, b) / a.getLength() / b.getLength() << "\n";*/

    lastSysTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() % 100000000;
    
    glutMainLoop();
}