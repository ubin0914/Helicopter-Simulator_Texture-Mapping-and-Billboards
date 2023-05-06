#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <ctime>

#include "PublicValue.h"

using namespace std;

struct LightSource {
	float* position;
	float* direction;
	float* diffuse;
	float* specular;
	float cutoff;
	float exponent;
	float attenuation;
};

LightSource sunlight, searchlight, minaretlight;
float* global_ambient;
Bulb sunBulb = Bulb(60);
Bulb minaretBulb = Bulb(0.5, 1, 0, 0);

Material atmosphereMat;

class Light {
public:
	static void initLights() {
		/*-----Define some global lighting status -----*/
		global_ambient = new float[] { 0.02, 0.02, 0.06 };
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); /* local viewer */
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient); /*global ambient*/

		sunlight.position = new float[]{ 0, 1, 0, 0 };
		sunlight.direction = new float[] { 0, -1, 0 };
		sunlight.diffuse = new float[]{ 1, 1, 1, 1.0 };
		sunlight.specular = new float[]{ 0.5, 0.5, 0.5, 1.0 };

		searchlight.position = new float[] { 0, 463, 0, 1 };
		searchlight.direction = new float[] { 1, 0, 0 };
		searchlight.diffuse = new float[] { 10, 10, 10, 1.0 };
		searchlight.specular = new float[] { 0.7, 0.7, 0.7, 1.0 };
		searchlight.cutoff = 60;
		searchlight.exponent = 10;
		searchlight.attenuation = 0.05;

		minaretlight.position = new float[] { 0, 509, 0, 1 };
		minaretlight.diffuse = new float[] { 5.0, 0.0, 0.0, 1.0 };
		minaretlight.specular = new float[] { 0.7, 0.7, 0.7, 1.0 };
		minaretlight.attenuation = 0.05;

		glLightfv(GL_LIGHT0, GL_POSITION, sunlight.position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, sunlight.diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, sunlight.specular);
		
		glLightfv(GL_LIGHT1, GL_POSITION, searchlight.position);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, searchlight.direction);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, searchlight.diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, searchlight.specular);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, searchlight.cutoff);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, searchlight.exponent);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, searchlight.attenuation);

		glLightfv(GL_LIGHT2, GL_POSITION, minaretlight.position);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, minaretlight.diffuse);
		glLightfv(GL_LIGHT2, GL_SPECULAR, minaretlight.specular);
		glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, minaretlight.attenuation);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);

		atmosphereMat.diffuse = new float[] { 0.4, 0.4, 0.4, 1 };
		atmosphereMat.ambient = new float[] { 1.5, 1.5, 1.7, 1 };
		atmosphereMat.specular = new float[] { 0, 0, 0, 1 };
		atmosphereMat.shininess = 8;
	}
	static void setLights(Helicopter& helicopter) {
		if (spKeyPressing[UPKEY_INT] && searchlight.exponent < 100) {
			for (int i = 0; i < 3; i++)
				searchlight.diffuse[i] *= 1.025;
			searchlight.exponent *= 1.05;
		}
		if (spKeyPressing[DOWNKEY_INT] && searchlight.exponent > 1) {
			for (int i = 0; i < 3; i++)
				searchlight.diffuse[i] /= 1.025;
			searchlight.exponent /= 1.05;
		}
		if (spKeyPressing[LEFTKEY_INT] && searchlight.cutoff < 75) {
			searchlight.cutoff += 1;
		}
		if (spKeyPressing[RIGHTKEY_INT] && searchlight.cutoff > 5) {
			searchlight.cutoff -= 1;
		}
		if (minaretlightMode) {
			minaretlight.diffuse[1] = minaretlight.diffuse[2] = 0;
		}
		else {
			minaretlight.diffuse[1] = minaretlight.diffuse[2] = 5;
		}

		float sunElevation = (clockTime - 21600) / 240;
		float narrowElevation = (clockTime > 43200 ? 270 - clockTime / 240 : clockTime / 240 - 90);
		if (narrowElevation < -5.5) {
			glDisable(GL_LIGHT0);

			global_ambient[0] = 0.03;
			global_ambient[1] = 0.03;
			global_ambient[2] = 0.05;
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
		}
		else {
			float sunElevation_cos = cos(sunElevation * PI / 180);
			float sunElevation_sin = sin(sunElevation * PI / 180);
			sunlight.position[0] = sunElevation_cos;
			sunlight.position[1] = sunElevation_sin;
			sunlight.diffuse[0] = 0.8 + sunElevation_sin * 0.1;
			sunlight.diffuse[1] = 0.4 + sunElevation_sin * 0.6;
			sunlight.diffuse[2] = 0.15 + sunElevation_sin * 0.85;
			if (narrowElevation < 0.5) {
				if(narrowElevation > -0.5)
					for (int i = 0; i < 3; i++)
						sunlight.diffuse[i] *= (narrowElevation * 0.6 + 0.7);
				else
					for (int i = 0; i < 3; i++)
						sunlight.diffuse[i] *= (narrowElevation + 5.5) * 0.06;
			}
			glEnable(GL_LIGHT0);
			glLightfv(GL_LIGHT0, GL_POSITION, sunlight.position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, sunlight.diffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, sunlight.specular);
			
			global_ambient[0] = sunlight.diffuse[0] * 0.3 + 0.05;
			global_ambient[1] = sunlight.diffuse[1] * 0.3 + 0.05;
			global_ambient[2] = sunlight.diffuse[2] * 0.32 + 0.08;
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

			sunBulb.setRGB(sunlight.diffuse[0] + 0.4, sunlight.diffuse[1] + 0.4, sunlight.diffuse[2] + 0.2);
			glPushMatrix();
				glTranslated(	helicopter.origin_r.x + sunlight.position[0] * 9900, 
								sunlight.position[1] * 9900, 
								helicopter.origin_r.z );
				sunBulb.rad = 100 - sunElevation_sin * 40;
				glFogf(GL_FOG_DENSITY, fogDensity / 50);
				sunBulb.draw();
				glFogf(GL_FOG_DENSITY, fogDensity);
			glPopMatrix();
		}
		searchlight.position[0] = helicopter.origin_r.x - helicopter.axisY_r.x * 1.2;
		searchlight.position[1] = helicopter.origin_r.y - helicopter.axisY_r.y * 1.2;
		searchlight.position[2] = helicopter.origin_r.z - helicopter.axisY_r.z * 1.2;
		searchlight.direction = helicopter.axisX_r.toArray();

		glLightfv(GL_LIGHT1, GL_POSITION, searchlight.position);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, searchlight.direction);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, searchlight.diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, searchlight.specular);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, searchlight.cutoff);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, searchlight.exponent);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, searchlight.attenuation);

		glLightfv(GL_LIGHT2, GL_DIFFUSE, minaretlight.diffuse);

		if (!minaretlightMode || lastSysTime % 2000 < 150) {
			glEnable(GL_LIGHT2);
			minaretBulb.setRGB(5, !minaretlightMode, !minaretlightMode);
			glPushMatrix();
				glTranslated(0, 509, 0);
				minaretBulb.draw();
			glPopMatrix();
		}
		else glDisable(GL_LIGHT2);

		glPushMatrix();
			glTranslated(helicopter.origin_r.x,	0, helicopter.origin_r.z);
			drawAtmosphere();
		glPopMatrix();

		if (keyPressing['g'] && fogDensity > 0.00001) fogDensity /= 1.07;
		if (keyPressing['h'] && fogDensity < 0.05) fogDensity *= 1.07;
		glFogf(GL_FOG_DENSITY, fogDensity);
		glFogfv(GL_FOG_COLOR, global_ambient);
	}
	static void drawAtmosphere() {
		atmosphereMat.select(GL_FRONT_AND_BACK);
		glCullFace(GL_FRONT);
		GLUquadricObj* sphere = gluNewQuadric();
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluQuadricNormals(sphere, GLU_SMOOTH);
		gluSphere(sphere, 10000, 32, 32);
		glCullFace(GL_BACK);
	}
};