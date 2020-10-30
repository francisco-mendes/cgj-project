#pragma once
#include <GL/glew.h>

#include "Math/Matrix.h"
#include "Camera.h"

struct Camera {
	Vector3 position;
	Vector3 center;
	Vector3 up;
	
	bool ortho;
	
	float camSpeed;
	int mForward;
	int mRight;
	int mUp;
	
	double oldX, oldY;
	bool b = true;
	
	GLuint vbo[1];
	
	Camera() = default;
	Camera(Vector3 pos, Vector3 c, Vector3 u, float cSpeed, GLuint viewId);

	void moveCam();
	void rotateCamera(double x, double y);
	Matrix4 CamView();
	Matrix4 CamPerpective();
	void camBinds();
};