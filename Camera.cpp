#include "Camera.h"


Camera::Camera(Vector3 pos, Vector3 c, Vector3 u, float cSpeed, GLuint UBO_BP) {
	position = pos;
	center = c;
	up = u;
	ortho = false;
	camSpeed = cSpeed;
	mForward = 0;
	mRight = 0;
	mUp = 0;
	oldX = 0;
	oldY = 0;
	glGenBuffers(1, vbo);
	glBindBuffer(GL_UNIFORM_BUFFER, vbo[0]);
	{
		glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4) * 2, 0, GL_STREAM_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, UBO_BP, vbo[0]);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Camera::moveCam() {
	Vector3 facing = (center - position).normalized();
	Vector3 right = (facing % up).normalized();
	position = position + mForward * camSpeed * facing + mUp * camSpeed * up + mRight * camSpeed * right;
	center = center + mForward * camSpeed * facing + mUp * camSpeed * up + mRight * camSpeed * right;


}

void Camera::rotateCamera(double x, double y)
{
	if (b)
	{
		oldX = x;
		oldY = y;
		b = false;
	}
	Vector3 facing = (center - position).normalized();
	Vector3 right = (facing % up).normalized();
	double xdif = x - oldX;
	double ydif = y - oldY;
	oldX = x;
	oldY = y;
	facing = Matrix4::rotation(up, -xdif * 0.00314) * facing;
	facing = Matrix4::rotation(right, -ydif * 0.00314) * facing;
	facing = facing.normalized();
	center = position + facing;
	//change to rotate around camera axis;
}

Matrix4 Camera::CamView()
{
	Matrix4 m = Matrix4::view(position, center, up);
	return m;
}

Matrix4 Camera::CamPerpective()
{
	if (ortho)
	{
		return Matrix4::orthographic(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 10.0f);
	}
	else {
		return Matrix4::perspective(30, 640.f / 480.f, 1, 10);
	}
}

void Camera::camBinds()
{
	Matrix4 ViewMatrix = CamView();
	Matrix4 ProjectionMatrix = CamPerpective();

	glBindBuffer(GL_UNIFORM_BUFFER, vbo[0]);
	{
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4), ViewMatrix.inner);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4), sizeof(Matrix4), ProjectionMatrix.inner);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}