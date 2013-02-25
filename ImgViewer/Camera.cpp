#include <cmath>
#include <cstdlib>
#include <QtOpenGL>
#include "Camera.h"

// Radius of trackball
#define TRACKBALL_R 0.85f

// Initialize parameters of the camera to the usual
// (i.e. at origin, looking down negative Z axis, etc.)
void Camera::Init()
{
	pos[0] = homepos[0] = savepos[0] = 0;
	pos[1] = homepos[1] = savepos[1] = 0;
	pos[2] = homepos[2] = savepos[2] = 0;

	rot = homerot = saverot = 0;

	rotaxis[0] = homerotaxis[0] = saverotaxis[0] = 0;
	rotaxis[1] = homerotaxis[1] = saverotaxis[1] = 0;
	rotaxis[2] = homerotaxis[2] = saverotaxis[2] = 1;

	fov = homefov = savefov = DEFAULT_FOV;

	status = IDLING;
}


// Set the "home" position of the camera
void Camera::SetHome(const vec3 &_pos,
		     float _rot,
		     const vec3 &_rotaxis,
		     float _fov)
{
	homepos = _pos;
	homerot = _rot;
	homerotaxis = _rotaxis;
	homefov = _fov;
}


// Reset the camera to the "home" position
void Camera::Reset()
{
	SaveConfig();
	pos = homepos;
	rot = homerot;
	rotaxis = homerotaxis;
	fov = homefov;
}


// Save the current position of the camera for a possible future undo
void Camera::SaveConfig()
{
	savepos = pos;
	saverot = rot;
	saverotaxis = rotaxis;
	savefov = fov;
}


// Undo the last camera motion
void Camera::Undo()
{
	vec3 vt;
	float t;
	vt = pos;
	pos = savepos;
	savepos = vt;

	vt = rotaxis;
	rotaxis = saverotaxis;
	saverotaxis = rotaxis;

	t = rot;
	rot = saverot;
	saverot = t;

	t = fov;
	fov = savefov;
	savefov = fov;
}


// Set up the OpenGL projection for the current
// camera position, given screen width and height (in pixels) and
// distance to near and far planes (in world coordinates)
// { Note, this is mainly for picking }
void Camera::Projection(float _znear, float _zfar, float _width, float _height)
{
	float diag = sqrt(_width*_width + _height*_height);
	float top = _height/diag * 0.5f*::tanf(fov) * _znear;
	float bottom = -top;
	float right = _width/diag * 0.5f*::tanf(fov) * _znear;
	float left = -right;
	float neardist = _znear;
	float fardist = _zfar;
	glFrustum(left, right, bottom, top, neardist, fardist);
}

void Camera::Modelview()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180.0f/PI*rot, rotaxis.x, rotaxis.y, rotaxis.z);
	glTranslatef(-pos.x, -pos.y, -pos.z);
}


// Set up the OpenGL projection and modelview matrices for the current
// camera position, given screen width and height (in pixels) and
// distance to near and far planes (in world coordinates)
void Camera::SetGL(float _znear, float _zfar, float _width, float _height)
{
	float diag = sqrt(_width*_width + _height*_height);
	float top = _height/diag * 0.5f*::tanf(fov) * _znear;
	float bottom = -top;
	float right = _width/diag * 0.5f*::tanf(fov) * _znear;
	float left = -right;
	float neardist = _znear;
	float fardist = _zfar;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left, right, bottom, top, neardist, fardist);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180.0f/PI*rot, rotaxis.x, rotaxis.y, rotaxis.z);
	//glRotatef(0, rotaxis.x, rotaxis.y, rotaxis.z);
	glTranslatef(-pos.x, -pos.y, -pos.z);
}



// Find the vector that gives the view direction of the camera, as well as
// vectors pointing "up" and "right"
void Camera::GetViewCoord(vec3 &viewdir, vec3 &updir, vec3 &rightdir)
{
	quat q;
	q.FromAxisAngle(rotaxis, rot);
	// negate w of the quaternion is equivalent to
	// changing the encoded rotation from theta to 2PI - theta
	// (i.e. rotate back theta)
	q[3] = -q[3];

	viewdir[0] = 0.0f; viewdir[1] = 0.0f; viewdir[2] = -1.0f;

	q.RotateVec(viewdir);

	updir[0] = updir[2] = 0.0f;  updir[1] = 1.0f;
	
	q.RotateVec(updir);

	rightdir = viewdir ^ updir;
}


// Translate the camera by (dx, dy, dz).  Note that these are specified
// *relative to the camera's frame*
void Camera::Move(float dx, float dy, float dz)
{
	SaveConfig();

	vec3 xaxis(1, 0, 0);
	vec3 yaxis(0, 1, 0);
	vec3 zaxis(0, 0, 1);

	quat currq;
	currq.FromAxisAngle(rotaxis, rot);

	currq[3] = -currq[3];

	currq.RotateVec(xaxis);
	currq.RotateVec(yaxis);
	currq.RotateVec(zaxis);

	pos[0] += xaxis[0]*dx + yaxis[0]*dy + zaxis[0]*dz;
	pos[1] += xaxis[1]*dx + yaxis[1]*dy + zaxis[1]*dz;
	pos[2] += xaxis[2]*dx + yaxis[2]*dy + zaxis[2]*dz;
}



// Rotate the camera by quaternion q, about a point lying along the camera's
// view direction, a distance "rotdist" away from the camera
void Camera::Rotate(const quat &q, float rotdist)
{
	SaveConfig();

	vec3 viewdir, updir, rightdir;
	GetViewCoord(viewdir, updir, rightdir);
	vec3 rotcenter(pos[0] + rotdist*viewdir[0],
			    pos[1] + rotdist*viewdir[1],
			    pos[2] + rotdist*viewdir[2]);

	quat currq;
	currq.FromAxisAngle(rotaxis, rot);

	currq = q * currq;
	currq.ToAxisAngle(rotaxis, rot);

	GetViewCoord(viewdir, updir, rightdir);

	pos[0] = rotcenter[0] - rotdist*viewdir[0];
	pos[1] = rotcenter[1] - rotdist*viewdir[1];
	pos[2] = rotcenter[2] - rotdist*viewdir[2];
}



// Zoom the camera (i.e. change field of view)
void Camera::Zoom(float logzoom)
{
	SaveConfig();
	fov /= exp(logzoom);
}


// Convert an (x,y) normalized mouse position to a position on the trackball
void Camera::Point2TrackballPos(float x, float y, vec3 &v)
{
	float r2 = x*x + y*y;
	float t = 0.5f * TRACKBALL_R * TRACKBALL_R;

	v[0] = x;
	v[1] = y;
	if (r2 < t)
		v[2] = sqrtf(2.0f*t - r2);
	else
		v[2] = t / sqrtf(r2);
        
	v.normalize();
}


// Takes normalized mouse positions (x1, y1) and (x2, y2) and returns a
// quaternion representing a rotation on the trackball
void Camera::Arc2Quaternion(float x1, float y1, float x2, float y2, quat &q)
{
	if ((x1 == x2) && (y1 == y2)) 
	{
		q.w = 1.0;
		q.x = 0; q.y = 0; q.z = 0;	
		return;
	}

	vec3 pos1, pos2;
	Point2TrackballPos(x1, y1, pos1);
	Point2TrackballPos(x2, y2, pos2);

	vec3 rotaxis;
	rotaxis = pos1 ^ pos2;
	rotaxis.normalize();

	float rotangle = TRACKBALL_R * sqrtf((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)) ;

	q.FromAxisAngle(rotaxis, rotangle);
}
