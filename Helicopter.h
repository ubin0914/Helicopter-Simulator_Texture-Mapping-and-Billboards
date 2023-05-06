#include <iostream>
#include <vector>
#include <GL/glut.h>

#include "PublicValue.h"

using namespace std;

GLUquadricObj* sphere = NULL, * cylind = NULL;

class Helicopter {
public:
	//��m�P��V
	Coordinate origin_r;
	Coordinate axisX_r;
	Coordinate axisY_r;
	//�X�`���z�q
	Coordinate velocity;
	Coordinate acceleration;
	Coordinate angularVelocity;	//�����
	Coordinate angularAcceleration;
	//��(��)�[�t��(�H�U²�٬��O(�x))�P������έ�
	Coordinate bladeThrust;		//�D���l���O�A�笰�D���l�����V
	float limitBladeTiltAngle = 10;	//bladeThrust��V�۹�axisY_r���̤j�ɨ��A�N�q�Y�����ɱ׳t��
	Coordinate targetThrust;	//�D���l�ؼб��O
	float maxThrust = 1.3 * gravity;
	float minThrust = 0.7 * gravity;
	float thrustAccelerate = 0.8;	//�D���l�[�[�t�׭�
	Coordinate thrustTorque;	//�D���l���O�x
	Coordinate yawTorque;		//���z�W�]�t �����l���O�x + �D���l��������O�x
	float limitYawTorque = 0.15 * PI;	//yaw�ٲ����[�[�t�סA�|�������ܦ�limit�q��(���ΰf���O)
	//�ܩ� �����l���O �P �����l��������O�x �N���Q�D���l���O�۰ʥ��ű��F�A���t�~�p��
	//���O�P���O(�x)�o�ǳQ�ʤO�N���C��field

	//�H�U��targetThrust����V�]�w�̾�(�۹�@�ɮy��)�A�笰�����̲׶ɱפ�V
	int flightMode;
	int tiltMode;	//�� ����/���t/���� �T�q�Ҧ�
	float tiltDir[3][8] = {	//�N�۹��axisX_r
		{0, 45, 90, 135, 180, -135, -90, -45},
		{0, 45, 90, 135, 180, -135, -90, -45},
		{0, 35, 90, 135, 180, -135, -90, -35}
	};
	float limitTiltAngle[3][8] = {	//�N�۹��@��Y�b
		{25, 25, 25, 25, 25, 25, 25, 25},
		{40, 40, 40, 40, 40, 40, 40, 40},
		{70, 60, 40, 40, 40, 40, 40, 40}
	};
	
	float limitTiltAngleDeviation = 1;
	float limitTiltRateDeviation = 1 / 180 * PI;

	float bladeDir;
	float tailBladeDir;
	float tailBladeAngVelocity;

	float bottomY = -1.806;

	Material bodyMat, bladeMat;
	Polyhedron cabinObj, tailObj, bladeObj;

	Helicopter() {
		origin_r = { 0, 462 - bottomY, 0 };
		axisX_r = { 1, 0, 0 };
		axisY_r = { 0, 1, 0 };
		velocity = { 0, 0, 0 };
		acceleration = { 0, 0, 0 };
		angularVelocity = { 0, 0, 0 };
		angularAcceleration = { 0, 0, 0 };
		bladeThrust = { 0, 0, 0 };
		thrustTorque = { 0, 0, 0 };
		yawTorque = { 0, 0, 0 };
		tiltMode = 0;
		flightMode = 0;
		bladeDir = 0;
		tailBladeDir = 0;
		tailBladeAngVelocity = 500;
		
		/*bodyMat.diffuse = new float[] { 0.780392, 0.568627, 0.113725, 1 };
		bodyMat.ambient = new float[] { 0.329412, 0.223529, 0.027451, 1 };
		bodyMat.specular = new float[] { 0.992157, 0.941176, 0.807843, 1 };
		bodyMat.shininess = 0.21794872 * 128;*/
		
		bodyMat.diffuse = new float[] { 0.75164, 0.60648, 0.22648, 1 };
		bodyMat.ambient = new float[] { 0.24725, 0.1995, 0.0745, 1 };
		bodyMat.specular = new float[] { 0.628281, 0.555802, 0.366065, 1 };
		bodyMat.shininess = 0.4 * 128;

		bladeMat.diffuse = new float[] { 0.50754, 0.50754, 0.50754, 1 };
		bladeMat.ambient = new float[] { 0.19225, 0.19225, 0.19225, 1 };
		bladeMat.specular = new float[] { 0.508273, 0.508273, 0.508273, 1 };
		bladeMat.shininess = 0.4 * 128;

		cabinObj = Polyhedron(bodyMat, cabinVertex, cabinFace);
		tailObj = Polyhedron(bodyMat, tailVertex, tailFace);
		bladeObj = Polyhedron(bladeMat, bladeVertex, bladeFace);
	}

	void display() {
		if (sightMode == 1) return;

		glPushMatrix();
		glTranslatef(origin_r.x, origin_r.y, origin_r.z);
		rotateSysTo(axisX_r, axisY_r);
		
		glPushMatrix();
			glPolygonMode(GL_FRONT, GL_FILL);
			cabinObj.draw();
			/*
			glTranslatef(-0.1, -1.25, 0);
			glRotatef(90, 0, 1, 0);
			if (cylind == NULL) {
				cylind = gluNewQuadric();
				gluQuadricDrawStyle(cylind, GLU_FILL);
				gluQuadricNormals(cylind, GLU_SMOOTH);
			}
			gluCylinder(cylind, 0.08, 0.15, 0.3, 12, 3);*/
		glPopMatrix();

		glDisable(GL_CULL_FACE);

		glPushMatrix();
			glTranslatef(-2, 0.15, 0);
			glPolygonMode(GL_FRONT, GL_FILL);
			tailObj.draw();
			glPushMatrix();
				glTranslatef(-3.6, 1.3, 0);
				glRotatef(180, 1, 0, 0);
				if (cylind == NULL) {
					cylind = gluNewQuadric();
					gluQuadricDrawStyle(cylind, GLU_FILL);
					gluQuadricNormals(cylind, GLU_SMOOTH);
				}
				gluCylinder(cylind, 0.05, 0.05, 0.5, 12, 3);
				glRotatef(90, 1, 0, 0);

				glPushMatrix();
					glTranslatef(0, 0.5, 0);
					glRotatef(-tailBladeDir, 0, 1, 0);	//�M�D���l�ۤϡA�O���~�j
					glScaled(0.125, 1, 0.125);
					glPolygonMode(GL_FRONT, GL_FILL);
					for (int k = 0; k < 4; k++) {
						bladeObj.draw();
						glRotatef(90, 0, 1, 0);
					}
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
		
		glPushMatrix();
			glTranslatef(0, 1, 0);
			glRotatef(-90, 1, 0, 0);
			if (cylind == NULL) {
				cylind = gluNewQuadric();
				gluQuadricDrawStyle(cylind, GLU_FILL);
				gluQuadricNormals(cylind, GLU_SMOOTH);
			}
			gluCylinder(cylind, 0.1, 0.1, 1.0, 12, 3);
			glRotatef(90, 1, 0, 0);

			glPushMatrix();
				Coordinate bladeThrust_t = Coordinate(
					innerProduct(bladeThrust, axisX_r),
					innerProduct(bladeThrust, axisY_r),
					innerProduct(bladeThrust, outerProduct(axisX_r, axisY_r))
				);
				Coordinate rotateAxis = outerProduct({ 0, 1, 0 }, bladeThrust_t);
				glTranslatef(0, 1, 0);
				glRotatef(includedAngle({ 0, 1, 0 }, bladeThrust_t), rotateAxis.x, rotateAxis.y, rotateAxis.z);
				glRotatef(bladeDir, 0, 1, 0);
				glPolygonMode(GL_FRONT, GL_FILL);
				for (int k = 0; k < 4; k++) {
					bladeObj.draw();
					glRotatef(90, 0, 1, 0);
				}
			glPopMatrix();
		glPopMatrix();

		for (int i = 0; i < 4; i++) {
			glPushMatrix();
				glTranslated((i % 2 * 2 - 1) * 0.7, -0.9, (i / 2 * 2 - 1) * 0.9);
				glRotatef(120 - i / 2 * 60, 1, 0, 0);
				gluCylinder(cylind, 0.05, 0.05, 1.0, 12, 3);
			glPopMatrix();
		}
		for (int i = 0; i < 2; i++) {
			glPushMatrix();
				glTranslated(-1.5, -1.766, (i * 2 - 1) * 1.4);
				glRotatef(90, 0, 1, 0);
				gluCylinder(cylind, 0.08, 0.08, 3.0, 12, 3);
				glTranslated(0, 0, 2.98);
				glRotatef(-20, 1, 0, 0);
				gluCylinder(cylind, 0.08, 0.08, 0.7, 12, 3);
			glPopMatrix();
		}

		glPopMatrix();

		glEnable(GL_CULL_FACE);

		resetLightIncidence();
	}

	void evolve(float dt) {
		//����L��J�M�wtargetThrust
		int dir = -1;
		if (keyPressing['w'] && !keyPressing['s'] && !keyPressing['a'] && !keyPressing['d']) dir = 0;
		else if (!keyPressing['w'] && keyPressing['s'] && !keyPressing['a'] && !keyPressing['d']) dir = 4;
		else if (!keyPressing['w'] && !keyPressing['s'] && keyPressing['a'] && !keyPressing['d']) dir = 2;
		else if (!keyPressing['w'] && !keyPressing['s'] && !keyPressing['a'] && keyPressing['d']) dir = 6;
		else if (keyPressing['w'] && !keyPressing['s'] && keyPressing['a'] && !keyPressing['d']) dir = 1;
		else if (keyPressing['w'] && !keyPressing['s'] && !keyPressing['a'] && keyPressing['d']) dir = 7;
		else if (!keyPressing['w'] && keyPressing['s'] && keyPressing['a'] && !keyPressing['d']) dir = 3;
		else if (!keyPressing['w'] && keyPressing['s'] && !keyPressing['a'] && keyPressing['d']) dir = 5;
		//�M�wtargetThrust��V
		targetThrust = { 0, 1, 0 };
		if (dir != -1) {
			Coordinate rotateAxis = outerProduct({ 0,1,0 }, axisX_r);
			rotateDir(rotateAxis, { 0,1,0 }, tiltDir[tiltMode][dir]);
			rotateDir(targetThrust, rotateAxis, limitTiltAngle[tiltMode][dir]);
		}
		//�M�wtargetThrust�q��
		if (!flightMode) targetThrust *= 0.01;
		else if (keyPressing[' '] && !spKeyPressing[L_CTRL_INT]) targetThrust *= maxThrust;
		else if (!keyPressing[' '] && spKeyPressing[L_CTRL_INT]) targetThrust *= minThrust;
		else targetThrust *= gravity / targetThrust.y < maxThrust ? gravity / targetThrust.y : maxThrust;
		//�վ�bladeThrust�q��
		float bladeThrustFixLength = bladeThrust.getLength();
		if (bladeThrustFixLength < targetThrust.getLength()) {
			bladeThrustFixLength += thrustAccelerate * dt;
			if (bladeThrustFixLength > targetThrust.getLength()) {
				bladeThrustFixLength = targetThrust.getLength();
			}
		}
		else if (bladeThrustFixLength > targetThrust.getLength()) {
			bladeThrustFixLength -= thrustAccelerate * dt;
			if (bladeThrustFixLength < targetThrust.getLength()) {
				bladeThrustFixLength = targetThrust.getLength();
			}
		}
		bladeThrust = bladeThrustFixLength * axisY_r;
		//�z�L�t��o�X�A�X��thrustTorque�æP�B�վ�bladeThrust��V (�]��̬O�@�@����)
		float remainAngle = includedAngle(axisY_r, targetThrust);
		if (remainAngle > limitTiltAngleDeviation || angularVelocity.getLength() > limitTiltRateDeviation) {
			Coordinate horizontalV = angularVelocity - project(angularVelocity, axisY_r);
			Coordinate idealV = sqrt(8 * sin(limitBladeTiltAngle * PI / 180) * remainAngle * PI / 180) * outerProduct(axisY_r, targetThrust).identity();
			float maxTorque = 0.2 * 2 * sin(limitBladeTiltAngle * PI / 180) * bladeThrust.getLength();	//��ʺD�q�˼Ƭ�0.2 �b�Z��2����
			Coordinate difV = idealV - horizontalV;
			thrustTorque = (5 * difV.getLength() > maxTorque ? maxTorque * difV.identity() : 5 * difV);
			float angle_sin = thrustTorque.getLength() / bladeThrust.getLength() / 0.2 / 2;
			if (angle_sin > 1) angle_sin = 1;
			else if (angle_sin < -1) angle_sin = -1;
			rotateDir(bladeThrust, thrustTorque, asin(angle_sin) * 180 / PI);
			/*�����X�{nan���רҡA�h�ìOasin���D�A���w���ʰj��
			if (0 * suitTorque == Coordinate(0, 0, 0)) thrustTorque = suitTorque;
			if (0 * suitThrust == Coordinate(0, 0, 0)) bladeThrust = suitThrust;*/
		}
		//����L��J�����M�wyaw����[�t��(�ھڤW�@������Y�b)
		if (keyPressing['q'] && !keyPressing['e']) {
			yawTorque = limitYawTorque * axisY_r;
			tailBladeAngVelocity = bladeThrust.getLength() * 80;
		}
		else if (!keyPressing['q'] && keyPressing['e']) {
			yawTorque = -limitYawTorque * axisY_r;
			tailBladeAngVelocity = bladeThrust.getLength() * 20;
		}
		else {
			yawTorque = { 0,0,0 };
			tailBladeAngVelocity = bladeThrust.getLength() * 50;
		}
		//���w�Q�ʤO�æX�`�Ҧ����O(�[�t��)
		if (!flightMode && bladeThrust.getLength() < 0.02) bladeThrust *= 0;
		Coordinate gravityAcceleration = { 0, -gravity, 0 };
		Coordinate panningResistance = 0.12 * -velocity;
		Coordinate rotateResistance = 0.2 * -angularVelocity;
		Coordinate crosswindTorque = 0.05 * outerProduct(Coordinate(panningResistance.x,0,panningResistance.z), axisX_r);	//panningResistance�y�������O�x(�Ф�y��z)
		acceleration = bladeThrust + gravityAcceleration + panningResistance;
		angularAcceleration = thrustTorque + yawTorque + rotateResistance + crosswindTorque;
		//����(��)�t��
		Coordinate avgVelocity = velocity + acceleration * dt * 0.5;
		Coordinate avgAngVelocity = angularVelocity + angularAcceleration * dt * 0.5;
		velocity += acceleration * dt;
		angularVelocity += angularAcceleration * dt;
		//�I���p��(�]����Ĳ�ɶ��L�k�w���A�G²�Ʊ��ɶ���ӥH�t�׮t���) �ثe����case3²�ƪ�
		//�ھڲ{���t�׹w���Y�U�@�������J�a���h�A�ɤW�^�u�t�׮t
		if (origin_r.x * origin_r.x + origin_r.z * origin_r.z < 25 && 
			origin_r.y + bottomY >= 462 && 
			origin_r.y + avgVelocity.y * dt + bottomY < 462) {
			Coordinate bounceDiffV = Coordinate( -avgVelocity.x * 0.7, -avgVelocity.y * 1.25, -avgVelocity.z * 0.7 );
			Coordinate bounceDiffAngV;	//AngV���ҥ�
			avgVelocity += bounceDiffV;
			velocity = avgVelocity;	//�������t�A�����Υ��t�@�����t
		}
		//�A���ܦ�m�P���жb�Y�����B��
		origin_r += avgVelocity * dt;
		if (origin_r.y < 0.1) origin_r.y = 0.1;	//���a���b
		if (angularVelocity.getLength()) {
			rotateDir(axisX_r, avgAngVelocity, 180 / PI * avgAngVelocity.getLength() * dt);
			rotateDir(axisY_r, avgAngVelocity, 180 / PI * avgAngVelocity.getLength() * dt);
			//�O�I�B��
			axisY_r.identify();
			axisX_r = outerProduct(axisY_r, outerProduct(axisX_r, axisY_r));
		}
		//�P�_�^�_����Ҧ�
		if (!keyPressing['w'] && includedAngle(axisY_r, { 0, 1, 0 }) <= 25) tiltMode = 0;

		//����
		bladeDir += 100 * bladeThrust.getLength() * dt;
		if (bladeDir > 10000) bladeDir -= 9720;
		tailBladeDir += tailBladeAngVelocity * dt;
		if (tailBladeDir > 10000) tailBladeDir -= 9720;
	}

	vector<Coordinate> cabinVertex = {
		{ -2.25, -0.2, 0.25 },
		{ -2.25, 0.5, 0.25 },
		{ -0.75, -1, 1 },
		{ -0.75, 1, 1 },
		{ 1.5, 1, 1 },
		{ 2.0, -1, 1 },
		{ 2.5, -0.4, 1 },
		{ 3.0, -0.4, 0.6 },

		{ -2.25, -0.2, -0.25 },
		{ -2.25, 0.5, -0.25 },
		{ -0.75, -1, -1 },
		{ -0.75, 1, -1 },
		{ 1.5, 1, -1 },
		{ 2.0, -1, -1 },
		{ 2.5, -0.4, -1 },
		{ 3.0, -0.4, -0.6 },
	};
	vector<vector<int>> cabinFace = {
		{ 0, 2, 3, 1 },
		{ 1, 3, 11, 9 },
		{ 9, 11, 10, 8 },
		{ 8, 10, 2, 0 },
		{ 2, 5, 6, 4, 3 },
		{ 3, 4, 12, 11 },
		{ 11, 12, 14, 13, 10 },
		{ 10, 13, 5, 2 },
		{ 4, 6, 7 },
		{ 6, 5, 7 },
		{ 12, 15, 14 },
		{ 14, 15, 13 },
		{ 4, 7, 15, 12 },
		{ 7, 5, 13, 15 },
		{ 0, 1, 8, 9 }
	};

	vector<Coordinate> tailVertex = {
		{ -3.5, -0.15, 0.1 },
		{ -3.5, 0.15, 0.1 },
		{ 0, -0.35, 0.25 },
		{ 0, 0.35, 0.25 },
		{ -3.85, 1.5, 0.05 },
		{ -3.5, 1.5, 0.05 },
		{ -3.45, -0.5, 0.05 },
		{ -2.95, -0.45, 0.05 },
		{ -3.25, 0.0, 0.05 },
		{ -2.45, 0.0, 0.05 },

		{ -3.5, -0.15, -0.1 },
		{ -3.5, 0.15, -0.1 },
		{ 0, -0.35, -0.25 },
		{ 0, 0.35, -0.25 },
		{ -3.85, 1.5, -0.05 },
		{ -3.5, 1.5, -0.05 },
		{ -3.45, -0.5, -0.05 },
		{ -2.95, -0.45, -0.05 },
		{ -3.25, 0.0, -0.05 },
		{ -2.45, 0.0, -0.05 },
	};
	vector<vector<int>> tailFace = {
		{ 0, 2, 3, 1 },
		{ 1, 3, 13, 11 },
		{ 10, 11, 13, 12 },
		{ 2, 0, 10, 12 },
		{ 4, 5, 15, 14 },
		{ 5, 9, 19, 15 },
		{ 9, 7, 17, 19 },
		{ 7, 6, 16, 17 },
		{ 6, 8, 18, 16 },
		{ 8, 4, 14, 18 },
		{ 4, 8, 9, 5 },
		{ 8, 6, 7, 9 },
		{ 15, 19, 18, 14 },
		{ 19, 17, 18, 16 },
		{ 1, 0, 10, 11 }
	};

	vector<Coordinate> bladeVertex = {
		{ 0.0, 0.04, -0.05 },
		{ 0.0, -0.04, 0.05 },
		{ 4.5, -0.04, 0.15 },
		{ 4.5, 0.04, -0.15 }
	};
	vector<vector<int>> bladeFace = {
		{ 0, 1, 2, 3 }
	};
};