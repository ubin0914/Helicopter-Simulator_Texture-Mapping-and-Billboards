#pragma once

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

#define L_SHIFT_INT 112
#define R_SHIFT_INT 113
#define L_CTRL_INT 114
#define R_CTRL_INT 115
#define L_ALT_INT 116
#define UPKEY_INT 101
#define DOWNKEY_INT 103
#define LEFTKEY_INT 100
#define RIGHTKEY_INT 102
#define TSIZE 64

int width = 1520, height = 790;
int lastSysTime;
float PI = 3.14159265;
float gravity = 9.80665;
bool keyPressing[128] = { 0 }, spKeyPressing[128] = { 0 };
float lastTimeSingleW = 0;
float lastTimeSingleLCtrl = 0;
float lastTimeSingleSpace = 0;
int lastMouseX = -999, lastMouseY = -999;
int sightMode = 2;
int focusMode = 0;
float sightLongitude = 135, sightLatitude = -5;
float lensDistance = 20;
float viewMagnification = 2;
float viewNear = 3, viewFar = 12000;
int blockNum = 15;
float clockTime = 50000;
float clockVarying = 3600;
float minaretlightMode = 1;
unsigned char buildingTex[TSIZE][TSIZE][4];
unsigned char LEDTex[TSIZE][TSIZE][4];
unsigned char mountainTex[TSIZE][TSIZE][4];
unsigned char buildingLightTex[TSIZE][TSIZE][4];
unsigned int textName[4];
float fogDensity = 0.0001;

class Coordinate {
public:
	float x, y, z;

	Coordinate() {}
	Coordinate(const Coordinate& v) { x = v.x, y = v.y, z = v.z; }
	Coordinate(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	float getLength() const {
		return sqrtf(x * x + y * y + z * z);
	}
	void identify() {
		float length = sqrtf(x * x + y * y + z * z);
		if (length) { x /= length; y /= length; z /= length; }
	}
	Coordinate identity() const {
		Coordinate v(*this);
		v.identify();
		return v;
	}
	float* toArray() {
		float normal[] = { x, y, z };
		return normal;
	}
	void print() const {
		cout << x << " " << y << " " << z << "\n";
	}
};

Coordinate lightIncidence = { 0, -1, 0 };

Coordinate operator+ (const Coordinate& a, const Coordinate& b) { return Coordinate(a.x + b.x, a.y + b.y, a.z + b.z); }
Coordinate operator- (const Coordinate& a, const Coordinate& b) { return Coordinate(a.x - b.x, a.y - b.y, a.z - b.z); }
Coordinate operator- (const Coordinate& a) { return Coordinate(-a.x, -a.y, -a.z); }
Coordinate operator* (const float c, const Coordinate& v) { return Coordinate(c * v.x, c * v.y, c * v.z); }
Coordinate operator* (const Coordinate& v, const float c) { return Coordinate(c * v.x, c * v.y, c * v.z); }
bool operator== (const Coordinate& a, const Coordinate& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
bool operator!= (const Coordinate& a, const Coordinate& b) { return a.x != b.x || a.y != b.y || a.z != b.z; }
void operator+= (Coordinate& a, const Coordinate& b) { a.x += b.x, a.y += b.y, a.z += b.z; }
void operator-= (Coordinate& a, const Coordinate& b) { a.x -= b.x, a.y -= b.y, a.z -= b.z; }
void operator*= (Coordinate& a, const float b) { a.x *= b, a.y *= b, a.z *= b; }

float innerProduct(const Coordinate& a, const Coordinate& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Coordinate outerProduct(const Coordinate& a, const Coordinate& b) {
	return Coordinate(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

Coordinate project(const Coordinate& projectand, const Coordinate& projector) {
	return innerProduct(projectand, projector) / projector.getLength() * projector;
}

float includedAngle(const Coordinate& a, const Coordinate& b, const Coordinate& c = { 0, 0, 0 }) {
	if (a == b) return 0;
	int direct = 1;
	if (c.x || c.y || c.z) direct = (innerProduct(outerProduct(a, b), c) >= 0 ? 1 : -1);
	float angle_cos = innerProduct(a, b) / a.getLength() / b.getLength();
	if (angle_cos > 1) angle_cos = 1;
	else if (angle_cos < -1) angle_cos = -1;
	return direct * 180 / PI * acos(angle_cos);
}

class Face {
public:
	vector<int> order;
	Coordinate normal;
	//vector<int> texOrder;

	Face() {}
	Face(vector<int> order, Coordinate normal) {
		this->order = order;
		this->normal = normal;
		//texOrder = { 0, 1, 2, 3 };	//為啥加這個效率會變超慢
	}
	void draw(const vector<Coordinate>& cord) {
		glNormal3fv(normal.toArray());
		//glPolygonMode(GL_FRONT, GL_FILL);
		glBegin(GL_POLYGON);
		for each (int index in order) {
			glVertex3f(cord[index].x, cord[index].y, cord[index].z);
		}
		glEnd();
	}
	void drawBuildingSide(const vector<Coordinate>& cord, float levels) {
		glEnable(GL_TEXTURE_2D);
		glNormal3fv(normal.toArray());
		//glPolygonMode(GL_FRONT, GL_FILL);
		glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(cord[order[0]].x, cord[order[0]].y, cord[order[0]].z);
		glTexCoord2f(1, 0);
		glVertex3f(cord[order[1]].x, cord[order[1]].y, cord[order[1]].z);
		glTexCoord2f(1, levels);
		glVertex3f(cord[order[2]].x, cord[order[2]].y, cord[order[2]].z);
		glTexCoord2f(0, levels);
		glVertex3f(cord[order[3]].x, cord[order[3]].y, cord[order[3]].z);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	void draw101LED(const vector<Coordinate>& cord, float start) {
		glEnable(GL_TEXTURE_2D);
		glNormal3fv(normal.toArray());
		//glPolygonMode(GL_FRONT, GL_FILL);
		glBegin(GL_POLYGON);
		glTexCoord2f(0, start + 0.0175);
		glVertex3f(cord[order[0]].x, cord[order[0]].y, cord[order[0]].z);
		glTexCoord2f(0, start + 0.2325);
		glVertex3f(cord[order[1]].x, cord[order[1]].y, cord[order[1]].z);
		glTexCoord2f(1, start + 0.25);
		glVertex3f(cord[order[2]].x, cord[order[2]].y, cord[order[2]].z);
		glTexCoord2f(1, start);
		glVertex3f(cord[order[3]].x, cord[order[3]].y, cord[order[3]].z);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
};

class Material {
public:
	float* diffuse;
	float* ambient;
	float* specular;
	float shininess;

	void select(GLenum FACE = GL_FRONT) {
		glMaterialfv(FACE, GL_DIFFUSE, diffuse);
		glMaterialfv(FACE, GL_AMBIENT, ambient);
		glMaterialfv(FACE, GL_SPECULAR, specular);
		glMaterialf(FACE, GL_SHININESS, shininess);
	}
};

class Polyhedron {
public:
	Material material;
	vector<Coordinate> vertexCord;
	vector<Face> faces;

	Polyhedron() {};
	Polyhedron(Material& material, vector<Coordinate>& vertexCord, vector<vector<int>>& faceVertex) {
		this->material = material;
		this->vertexCord = vertexCord;
		for each (auto order in faceVertex) {
			faces.push_back(Face(order, outerProduct(vertexCord[order[1]] - vertexCord[order[0]], vertexCord[order[2]] - vertexCord[order[1]])));
		}
	}
	void draw() {
		material.select();
		for each (auto face in faces) {
			face.draw(vertexCord);
		}
	}
	void drawBuilding(float levels, bool turn_on) {
		material.select();
		if (turn_on) {
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, textName[3]);
		}
		else {
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, textName[0]);
		}
		for (int i = 0; i < 4; i++) {
			faces[i].drawBuildingSide(vertexCord, levels);
		}
		faces[4].draw(vertexCord);
	}
	void drawTaipei101() {
		material.select();
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, textName[1]);
		for (int i = 0; i < 180; i++)
			faces[i].draw(vertexCord);
		float offset = lastSysTime % 5000 / 5000.0;
		for (int i = 0; i < 4; i++) {
			faces[180 + i].draw101LED(vertexCord, offset + i * 0.25);
		}
		for (int i = 184; i < faces.size(); i++)
			faces[i].draw(vertexCord);
	}
};

class Bulb {
public:
	GLUquadricObj* sphere;
	float rad, r, g, b;

	Bulb(float rad, float r = 1, float g = 1, float b = 1) {
		this->rad = rad;
		this->r = r;
		this->g = g;
		this->b = b;
		sphere = gluNewQuadric();
	}
	void setRGB(float r, float g, float b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}
	void draw() {
		gluQuadricDrawStyle(sphere, GLU_FILL);
		glDisable(GL_LIGHTING);
		glColor3f(r, g, b);
		gluSphere(sphere, rad, 32, 32);
		glEnable(GL_LIGHTING);
	}
};

void rotateDir(Coordinate& dir, Coordinate axis, float angle) {
	axis.identify();
	float th = (float)angle / 180 * PI;
	dir = dir * cos(th) + outerProduct(axis, dir) * sin(th) + axis * innerProduct(axis, dir) * (1 - cos(th));
}

void rotateSysTo(const Coordinate& newAxisX, const Coordinate& newAxisY) {
	if (newAxisX.y || newAxisX.z) {
		if (newAxisY.x || newAxisY.z) {
			Coordinate temp = newAxisX - project(newAxisX, { 1,0,0 });
			float angle = includedAngle({ 0,0,1 }, temp, { 1,0,0 });
			glRotatef(angle, 1, 0, 0);					rotateDir(lightIncidence, { 1,0,0 }, -angle);
			temp = outerProduct(temp, { 1,0,0 });
			angle = includedAngle({ 1,0,0 }, newAxisX, temp);
			glRotatef(angle, 0, 1, 0);					rotateDir(lightIncidence, { 0,1,0 }, -angle);
			angle = includedAngle(temp, newAxisY, newAxisX);
			//if (angle * 0 != 0) cout << angle << "\n";
			glRotatef(angle, 1, 0, 0);					rotateDir(lightIncidence, { 1,0,0 }, -angle);
		}
		else {
			float angle = includedAngle({ 1,0,0 }, newAxisX, { 0,1,0 });
			glRotatef(angle, 0, 1, 0);					rotateDir(lightIncidence, { 0,1,0 }, -angle);
		}
	}
	else {
		float angle = includedAngle({ 0,1,0 }, newAxisY, { 1,0,0 });
		glRotatef(angle, 1, 0, 0);					rotateDir(lightIncidence, { 1,0,0 }, -angle);
	}
}

//直接返回世界座標系，沒有實作popMatrix時的處理
void resetLightIncidence() {
	lightIncidence = { 0, -1, 0 };
}