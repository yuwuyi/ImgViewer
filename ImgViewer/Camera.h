#ifndef __CAMERA_H
#define __CAMERA_H

#include "Vector3.h"
#include "Quaternion.h"


// Initial field of view of the camera
#define DEFAULT_FOV 0.8284271f

#ifndef PI
#define	PI 3.14159265358979323846
#endif

class Camera  
{
private:
	typedef Vector3f vec3;
	typedef Quatf quat;
public:
	typedef enum {PANNING, ROTATING, ZOOMING, IDLING} mov;

public:
	void Init();
	Camera() { Init(); }
	virtual ~Camera() {}
	void SetHome(const vec3 &_pos, float _rot, const vec3 &_rotaxis, float _fov);
	void Reset();
	void GoHome() { Reset(); }

	void Projection(float _znear, float _zfar, float _width, float _height);
	void Modelview();
	void SetGL(float _znear, float _zfar, float _width, float _height);

	void GetViewCoord(vec3 &viewdir, vec3 &updir, vec3 &rightdir);

	void Move(float dx, float dy, float dz);
	void Rotate(const quat &q, float rotdist);
	void Zoom(float logzoom);

	void SaveConfig();
	void Undo();

	static void Point2TrackballPos(float x, float y, vec3 &v);
	static void Arc2Quaternion(float x1, float y1, float x2, float y2, quat &q);

public:
	vec3 pos;
	float rot;
	vec3 rotaxis;
	float fov;
	mov status;

private:
	vec3 homepos;
	float homerot;
	vec3 homerotaxis;
	float homefov;
	vec3 savepos;
	float saverot;
	vec3 saverotaxis;
	float savefov;

};



#endif // __CAMERA_H
