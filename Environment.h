#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <ctime>

#include "PublicValue.h"

using namespace std;

class Environment {
private:
	int building[25][25][16] = { 0 };
	vector<Coordinate> taipei_101_vertex;
	vector<vector<int>> taipei_101_face;
	float buildingHeight[19] = { 9, 9, 9, 9, 12, 12, 12, 12, 15, 15, 15, 15, 18, 18, 18, 18, 27, 36, 60 };
	int centerBlockX = 0, centerBlockZ = 0;	//以block為單位的x與z
	pair<float, float> mountainLoc[30];
	Coordinate mountainXAxis[30];
	float mountainWidth[30];
	float mountainHeight[30];

	Material taipei_101_Mat, buildingsMat, groundMat, coating, lightningRod_Mat;
	Polyhedron taipei_101_Obj, cubeObj;

	float buildingLightPreiod[7][2] = {
		{18.5 * 3600, 1 * 3600},
		{18.25 * 3600, 2 * 3600},
		{18 * 3600, 3 * 3600},
		{17.75 * 3600, 23 * 3600},
		{9 * 3600, 21 * 3600},
		{7 * 3600, 1.5 * 3600},
		{0 * 3600, 24 * 3600}
	};

public:
	Environment() {
		pushSquareVertex(64, 0, 64);
		pushSquareVertex(47, 110, 47);
		pushSquareVertex(48, 113.5, 48);
		vector<pair<float, float>> dxdz = {
			make_pair(36, 41),
			make_pair(41, 41),
			make_pair(41, 36),
			make_pair(46, 36),
			make_pair(46, -36)
		};
		pushMultiSquareVertex(dxdz, 113.5);
		pushMultiSquareVertex(dxdz, 117);
		for (int i = 0; i < 8; i++) {
			vector<pair<float, float>> dxdz = {
				make_pair(33, 38),
				make_pair(38, 38),
				make_pair(38, 33),
				make_pair(43, 33),
				make_pair(43, -33)
			};
			pushMultiSquareVertex(dxdz, 117 + i * 34);
			dxdz = {
				make_pair(38, 43),
				make_pair(43, 43),
				make_pair(43, 38),
				make_pair(48, 38),
				make_pair(48, -38)
			};
			pushMultiSquareVertex(dxdz, 117 + (i + 1) * 34);
		}
		pushSquareVertex(24, 389, 24);
		pushSquareVertex(26, 401, 26);
		pushSquareVertex(24, 401, 24);
		pushSquareVertex(24, 407, 24);
		pushSquareVertex(12, 407, 12);
		pushSquareVertex(16, 434, 16);
		pushSquareVertex(15.5, 434, 15.5);
		pushSquareVertex(16.5, 448.5, 16.5);
		pushSquareVertex(9, 448.5, 9);
		pushSquareVertex(6, 457.5, 6);

		pushFourFace(0, 1);
		pushFourFace(4, 1);
		for (int k = 0; k < 5; k++)
			pushFourFace(12 + k, 5, (k == 4));
		for (int i = 0; i < 8; i++)
			for (int k = 0; k < 5; k++)
				pushFourFace(52 + i * 40 + k, 5, (k == 4));
		pushFourFace(372, 1);
		pushFourFace(380, 1);
		pushFourFace(388, 1);
		pushFourFace(396, 1);
		pushFourFace(404, 1);

		pushCeilingFace(8, 4);
		pushCeilingFace(32, 20);
		for(int i = 0; i < 8; i++)
			pushCeilingFace(72 + i * 40, 20);
		pushCeilingFace(376, 4);
		pushCeilingFace(384, 4);
		pushCeilingFace(392, 4);
		pushCeilingFace(400, 4);
		pushCeilingFace(408, 4);

		initialBuildings();
		initialMountains();

		taipei_101_Mat.diffuse = new float[] { 0.4, 0.4, 0.4, 1 };
		taipei_101_Mat.ambient = new float[] { 0.25, 0.25, 0.25, 1 };
		taipei_101_Mat.specular = new float[] { 0.7746, 0.7746, 0.7746, 1 };
		taipei_101_Mat.shininess = 0.6 * 128;

		buildingsMat.diffuse = new float[] { 0.7, 0.6, 0.54, 1 };
		buildingsMat.ambient = new float[] { 0.7, 0.6, 0.54, 1 };
		buildingsMat.specular = new float[] { 0.5, 0.5, 0.5, 1 };
		buildingsMat.shininess = 0.25 * 128;

		groundMat.diffuse = new float[] { 0.1, 0.1, 0.1, 1 };
		groundMat.ambient = new float[] { 0.1, 0.1, 0.1, 1 };
		groundMat.specular = new float[] { 0, 0, 0, 1 };
		groundMat.shininess = 8;

		coating.diffuse = new float[] { 1, 1, 0.2, 1 };
		coating.ambient = new float[] { 5, 5, 1, 1 };
		coating.specular = new float[] { 0, 0, 0, 1 };
		coating.shininess = 8;

		lightningRod_Mat.diffuse = new float[] { 0.7, 0.7, 0.7, 1 };
		lightningRod_Mat.ambient = new float[] { 0.4, 0.4, 0.4, 1 };
		lightningRod_Mat.specular = new float[] { 0.7746, 0.7746, 0.7746, 1 };
		lightningRod_Mat.shininess = 0.05 * 128;

		taipei_101_Obj = Polyhedron(taipei_101_Mat, taipei_101_vertex, taipei_101_face);
		cubeObj = Polyhedron(buildingsMat, cubeVertex, cubeFace);

		make_buildingTexture();
		make_LEDTexture();
		make_mountainTexture();
		make_buildingLightTexture();
	}
	void initialBuildings() {
		srand(time(NULL));
		for (int i = 0; i < blockNum; i++)
			for (int j = 0; j < blockNum; j++)
				for (int k = 0; k < 16; k++)
					building[i][j][k] = rand() % 19;
	}
	void initialMountains() {
		srand(time(NULL));
		for (int i = 0; i < 30; i++) {
			float dir = rand() % 360;
			float dst = 4000 + rand() % 5000;
			mountainLoc[i] = make_pair(dst * cos(dir * PI / 180), dst * sin(dir * PI / 180));
			mountainXAxis[i] = Coordinate(cos((dir + 90) * PI / 180), 0, sin((dir + 90) * PI / 180));
			mountainWidth[i] = 2000 + rand() % 3000;
			mountainHeight[i] = 200 + rand() % 600;
		}
	}
	void display() {
		drawGround();
		drawTapei_101();
		
		/*int i = 5, j = 5, k = 0;
		glPushMatrix();
		glTranslated((j - 4) * 100 + (k % 4) * 25 - 37.5, 0, (i - 4) * 100 + (k / 4) * 25 - 37.5);
		glScaled(1, buildingHeight[building[i][j][k]], 1);
		glPolygonMode(GL_FRONT, GL_FILL);
		for each (Face face in buildingFace) {
			face.drawAsRGB(buildingVertex);
		}
		glPopMatrix();*/

		buildingsMat.select();
		glPolygonMode(GL_FRONT, GL_FILL);
		for (int i = -blockNum / 2; i <= blockNum / 2; i++) {
			for (int j = -blockNum / 2; j <= blockNum / 2; j++) {
				float offsetX = (centerBlockX + j) * 110;
				float offsetZ = (centerBlockZ + i) * 110;
				if (!offsetX && !offsetZ) continue;
				int indexX = (centerBlockX + j + 100000 * blockNum) % blockNum;
				int indexZ = (centerBlockZ + i + 100000 * blockNum) % blockNum;
				for (int k = 0; k < 16; k++) {
					float quarterX = (k < 8 ? 25 : -25) + (k % 4 < 2 ? 10 : -10);
					float quarterZ = (k / 4 % 2 ? 20.5 : -20.5) + (k % 2 ? 10 : -10);
					glPushMatrix();
					glTranslated(offsetX + quarterX, 0, offsetZ + quarterZ);
					glScaled(1, buildingHeight[building[indexX][indexZ][k]], 1);

					float levels = buildingHeight[building[indexX][indexZ][k]] / 3;
					int index;
					if (levels == 3) index = 0;
					else if (levels == 4) index = 1;
					else if (levels == 5) index = 2;
					else if (levels == 6) index = 3;
					else if (levels == 9) index = 4;
					else if (levels == 12) index = 5;
					else if (levels == 20) index = 6;
					if(buildingLightPreiod[index][0] < buildingLightPreiod[index][1])
						cubeObj.drawBuilding(levels, (clockTime >= buildingLightPreiod[index][0] && clockTime < buildingLightPreiod[index][1]));
					else
						cubeObj.drawBuilding(levels, (clockTime >= buildingLightPreiod[index][0] || clockTime < buildingLightPreiod[index][1]));
					glPopMatrix();
				}
			}
		}

		for (int i = 0; i < 30; i++) {
			draw_mountainBillboard(mountainLoc[i].first, mountainLoc[i].second, mountainWidth[i], mountainHeight[i], mountainXAxis[i]);
		}
	}
	void updateBlock(int newX, int newZ) {
		if (newX != centerBlockX) {
			int updateX = (centerBlockX + 100000 * blockNum + (newX - centerBlockX == 1 ? -1 : 1) * (blockNum / 2)) % blockNum;
			for (int i = 0; i < blockNum; i++)
				for (int k = 0; k < 16; k++)
					building[updateX][i][k] = rand() % 19;
			centerBlockX = newX;
		}
		if (newZ != centerBlockZ) {
			int updateZ = (centerBlockZ + 100000 * blockNum + (newZ - centerBlockZ == 1 ? -1 : 1) * (blockNum / 2)) % blockNum;
			for (int i = 0; i < blockNum; i++)
				for (int k = 0; k < 16; k++)
					building[i][updateZ][k] = rand() % 19;
			centerBlockZ = newZ;
		}
	}
private:
	void drawGround() {
		groundMat.select();
		glNormal3f(0, 1, 0);
		glBegin(GL_POLYGON);
		glVertex3f(centerBlockX * 110 + 9999, 0, centerBlockZ * 110 - 9999);
		glVertex3f(centerBlockX * 110 - 9999, 0, centerBlockZ * 110 - 9999);
		glVertex3f(centerBlockX * 110 - 9999, 0, centerBlockZ * 110 + 9999);
		glVertex3f(centerBlockX * 110 + 9999, 0, centerBlockZ * 110 + 9999);
		glEnd();
	}
	void drawTapei_101() {
		glPolygonMode(GL_FRONT, GL_FILL);
		taipei_101_Obj.drawTaipei101();
		//雷達平臺
		glPushMatrix();
			glTranslated(0, 457.5, 0);
			glRotatef(-90, 1, 0, 0);
			GLUquadricObj* cylind = gluNewQuadric();
			gluQuadricDrawStyle(cylind, GLU_FILL);
			gluQuadricNormals(cylind, GLU_SMOOTH);
			gluCylinder(cylind, 3, 5, 4.5, 24, 3);

			glTranslated(0, 0, 4.5);
			GLUquadricObj* ceiling = gluNewQuadric();
			gluQuadricDrawStyle(ceiling, GLU_FILL);
			gluDisk(ceiling, 0, 5, 24, 3);

			coating.select();
			glTranslated(0, 0, 0.02);
			gluDisk(ceiling, 4, 4.2, 24, 3);

			glDisable(GL_CULL_FACE);

			glRotatef(90, 1, 0, 0);
			glNormal3f(0, 1, 0);
			glBegin(GL_POLYGON);
			glVertex3f(-1.9, 0, 2.5);
			glVertex3f(-1.9, 0, -2.5);
			glVertex3f(-1.4, 0, -2.5);
			glVertex3f(-1.4, 0, 2.5);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex3f(1.4, 0, 2.5);
			glVertex3f(1.4, 0, -2.5);
			glVertex3f(1.9, 0, -2.5);
			glVertex3f(1.9, 0, 2.5);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex3f(-1.4, 0, 0.25);
			glVertex3f(-1.4, 0, -0.25);
			glVertex3f(1.4, 0, -0.25);
			glVertex3f(1.4, 0, 0.25);
			glEnd();

			glEnable(GL_CULL_FACE);
		glPopMatrix();
	}
	void pushSquareVertex(float dx, float y, float dz) {
		taipei_101_vertex.push_back({ dx / 2, y, dz / 2 });
		taipei_101_vertex.push_back({ dz / 2, y, -dx /2 });
		taipei_101_vertex.push_back({ -dx / 2, y, -dz / 2 });
		taipei_101_vertex.push_back({ -dz / 2, y, dx / 2 });
	}
	void pushMultiSquareVertex(vector<pair<float, float>> dxdz, float y) {
		for each (pair<float, float> vertex in dxdz)
			taipei_101_vertex.push_back({ vertex.first / 2, y, vertex.second / 2 });
		for each (pair<float, float> vertex in dxdz)
			taipei_101_vertex.push_back({ vertex.second / 2, y, -vertex.first / 2 });
		for each (pair<float, float> vertex in dxdz)
			taipei_101_vertex.push_back({ -vertex.first / 2, y, -vertex.second / 2 });
		for each (pair<float, float> vertex in dxdz)
			taipei_101_vertex.push_back({ -vertex.second / 2, y, vertex.first / 2 });
	}
	void pushFourFace(int start, int interval, bool cross = 1) {
		for (int i = 0; i < 4; i++) {
			int fin = start + 4 * interval;
			taipei_101_face.push_back(	{i * interval + start, 
										(i == 3 && cross ? -1 : i) * interval + start + 1,
										(i == 3 && cross ? -1 : i) * interval + fin + 1,
										i * interval + fin} );
		}
	}
	void pushCeilingFace(int start, int cycle) {
		vector<int> order;
		for (int i = 0; i < cycle; i++)
			order.push_back(start + i);
		taipei_101_face.push_back(order);
	}

	vector<Coordinate> cubeVertex = {
		{9, 0, 9},
		{9, 0, -9},
		{-9, 0, -9},
		{-9, 0, 9},
		{9, 1, 9},
		{9, 1, -9},
		{-9, 1, -9},
		{-9, 1, 9},
	};
	vector<vector<int>> cubeFace = {
		{ 0, 1, 5, 4 },
		{ 1, 2, 6, 5 },
		{ 2, 3, 7, 6 },
		{ 3, 0, 4, 7 },
		{ 4, 5, 6, 7 }
	};

	void make_buildingTexture() {
		for (int i = 0; i < 64; i++)
			for (int j = 0; j < 64; j++) {
				buildingTex[i][j][0] = 170;
				buildingTex[i][j][1] = 153;
				buildingTex[i][j][2] = 138;
				buildingTex[i][j][3] = 255;
			}
		int start[10] = { 3, 10, 17, 24, 28, 32, 36, 43, 50, 57 };
		for (int s = 0; s < 10; s++)
			for (int i = 15; i < 50; i++)
				for (int j = 0; j < 4; j++)
					buildingTex[i][start[s] + j][0] = buildingTex[i][start[s] + j][1] = buildingTex[i][start[s] + j][2] = 70;
	}
	void make_LEDTexture() {
		for (int i = 0; i < 64; i++)
			for (int j = 0; j < 64; j++) {
				LEDTex[i][j][0] = 255;
				LEDTex[i][j][1] = 255;
				LEDTex[i][j][2] = 255;
				LEDTex[i][j][3] = 0;
			}
		setLEDLine(1, 13, 8, 0);
		setLEDLine(4, 11, 9, 1);
		setLEDLine(12, 13, 2, 0);
		setLEDLine(11, 12, 4, 0);
		setLEDLine(10, 11, 2, 0);
		setLEDLine(14, 11, 2, 0);
		setLEDLine(9, 9, 7, 1);
		setLEDLine(15, 9, 7, 1);
		setLEDLine(11, 7, 4, 0);
		setLEDLine(18, 13, 4, 0, 1);
		setLEDLine(18, 3, 4, 0, 1);
		setLEDLine(19, 12, 9, 1);
		setLEDLine(23, 13, 11, 1);
		setLEDLine(25, 13, 4, 0);
		setLEDLine(25, 8, 5, 0);
		setLEDLine(29, 11, 4, 1);
		setLEDLine(28, 11, 1, 0, 1);
		setLEDLine(29, 12, 1, 0, 1);
		setLEDLine(32, 13, 11, 1);
		setLEDLine(34, 13, 5, 0);
		setLEDLine(34, 8, 4, 0, 1);
		setLEDLine(34, 4, 5, 0);
		setLEDLine(40, 13, 4, 0, 1);
		setLEDLine(40, 3, 4, 0, 1);
		setLEDLine(41, 12, 9, 1);
		setLEDLine(46, 12, 9, 1);
		setLEDLine(49, 11, 7, 1);
		setLEDLine(53, 11, 7, 1);
		setLEDLine(51, 11, 2, 0);
		setLEDLine(51, 6, 2, 0);
		setLEDLine(56, 12, 9, 1);
	}
	void setLEDLine(int si, int sj, int len, int dir, int thick = 2) {
		if (dir)
			for (int i = 0; i < thick; i++)
				for (int j = 0; j < len; j++)
					for (int k = 0; k < 4; k++)
						LEDTex[si + i][(sj - j) * 4 - k][3] = 255;
		else
			for (int i = 0; i < len; i++)
				for (int j = 0; j < thick; j++)
					for (int k = 0; k < 4; k++)
						LEDTex[si + i][(sj - j) * 4 - k][3] = 255;
	}
	void make_mountainTexture() {
		for (int i = 0; i < 64; i++)
			for (int j = 0; j < 64; j++) {
				mountainTex[i][j][0] = 30;
				mountainTex[i][j][1] = 50;
				mountainTex[i][j][2] = 30;
				float y = i / 32.0, x = j / 64.0 * 6 - 3;
				mountainTex[i][j][3] = 255 * (y <= -2.0 / 9 * x * x + 2);
			}
	}
	void getBilAxes(float a[3], float b[3]) {
		float mtx[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
		float w0, w2;
		double len;

		/*----Get w0 and w2 from the modelview matrix mtx[] ---*/
		w0 = mtx[2];
		w2 = mtx[10];

		len = sqrt(w0 * w0 + w2 * w2);
		/*---- Define the a and b axes for billboards ----*/
		b[0] = 0.0;
		b[1] = 1.0;
		b[2] = 0.0;
		a[0] = w2 / len;
		a[1] = 0.0;
		a[2] = -w0 / len;
	}
	void make_buildingLightTexture() {
		for (int i = 0; i < 64; i++)
			for (int j = 0; j < 64; j++) {
				buildingLightTex[i][j][0] = 255;
				buildingLightTex[i][j][1] = 255;
				buildingLightTex[i][j][2] = 200;
				buildingLightTex[i][j][3] = 0;
			}
		int start[10] = { 3, 10, 17, 24, 28, 32, 36, 43, 50, 57 };
		for (int s = 0; s < 10; s++)
			for (int i = 15; i < 50; i++)
				for (int j = 0; j < 4; j++)
					buildingLightTex[i][start[s] + j][3] = (s < 3 || s > 6 ? 150 : 20);
	}
	void draw_mountainBillboard (float x, float z, float w, float h, Coordinate dir) {
		float v0[3], v1[3], v2[3], v3[3], a[3], b[3];
		getBilAxes(a, b);

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textName[2]);

		/*----Compute the 4 vertices of the billboard ----*/
		v0[0] = x - (w / 2) * dir.x;
		v0[1] = 0.0;
		v0[2] = z - (w / 2) * dir.z;
		v1[0] = x + (w / 2) * dir.x;
		v1[1] = 0.0;
		v1[2] = z + (w / 2) * dir.z;
		v2[0] = x + (w / 2) * dir.x;
		v2[1] = h;
		v2[2] = z + (w / 2) * dir.z;
		v3[0] = x - (w / 2) * dir.x;
		v3[1] = h;
		v3[2] = z - (w / 2) * dir.z;

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
		glDisable(GL_TEXTURE_2D);
	}
public:
	void drawLightningRod(const Helicopter& helicopter) {
		float dist = Coordinate(helicopter.origin_r.x, 0, helicopter.origin_r.z).getLength();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		lightningRod_Mat.diffuse[3] = (dist > 5) * (dist - 5 > 25 ? 1 : (dist - 5) / 25);
		lightningRod_Mat.select(GL_FRONT_AND_BACK);
		glPushMatrix();
		glTranslated(0, 462, 0);
		glRotatef(-90, 1, 0, 0);
		GLUquadricObj* rod = gluNewQuadric();
		gluQuadricDrawStyle(rod, GLU_FILL);
		gluQuadricNormals(rod, GLU_SMOOTH);
		gluCylinder(rod, 1.5, 0.5, 46.5, 24, 3);
		glPopMatrix();
		glDisable(GL_BLEND);
	}
};