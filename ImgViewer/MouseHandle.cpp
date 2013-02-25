#include "MouseHandle.h"
#include <QMouseEvent>
#include <QWheelEvent>

#include "Vector3.h"
#include "Quaternion.h"

typedef Vector3f vec3;
typedef Vector3d vec3d;
typedef Quatf quat;


MouseHandle::MouseHandle(Camera& c, double &rd, double &vd) 
: theCamera(c), rotate_dist(rd), view_dist(vd)
{
	
}

void MouseHandle::mousePressEvent(QMouseEvent *event)
{
	pressPos = event->pos();
	float nx, ny;
	Mousei2f(event->pos().x(), event->pos().y(), &nx, &ny);
	bool isClick = false;

	isClick = (QPointF(nx, ny) - lastPos).manhattanLength() < 0.01;

	lastPos = QPointF(nx, ny);

}

void MouseHandle::mouseReleaseEvent(QMouseEvent *event) {
	bool isClick = false;
	isClick = (event->pos() - pressPos).manhattanLength() < 5;
	if (isClick ) {
		//doPickingSnapShot(event->pos().x(), event->pos().y());
		
	}
}

void MouseHandle::Mousei2f(int x, int y, float *xf, float *yf)
{
// 	int screenwidth = width();
// 	int screenheight = height();

	float r = (float)screenwidth/screenheight;
	if (r > 1.0) {
		*xf = r * (2.0f * x / screenwidth - 1.0f);
		*yf = 1.0f - 2.0f * y / screenheight;
	} else {
		*xf = 2.0f * x / screenwidth - 1.0f;
		*yf = (1.0f - 2.0f * y / screenheight) / r;
	}
}

void MouseHandle::mouseMoveEvent(QMouseEvent *event)
{
	float cur_mouse_x, cur_mouse_y;
	float delta_x, delta_y;


	Mousei2f(event->pos().x(), event->pos().y(), &cur_mouse_x, &cur_mouse_y);

	delta_x = cur_mouse_x - lastPos.x();
	delta_y = cur_mouse_y - lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		Rotate(lastPos.x(), lastPos.y(), cur_mouse_x, cur_mouse_y);
	} else if (event->buttons() & Qt::RightButton) {
		Translate(delta_x, delta_y, 0);
	}

	lastPos = QPointF(cur_mouse_x, cur_mouse_y);
	
}    

void MouseHandle::setCtrl(bool c) {
	isCtrl = c;
}

void MouseHandle::wheelEvent( QWheelEvent * event )  {
	if (isCtrl) {
		//control press, adjust the slice_distance_factor
		double percent = 1 + (float)event->delta() / 720 ;

		if (percent < 0.1) {
			percent = 0.1;
		}

		if (percent > 2.0) {
			percent = 2.0;
		}

// 		slice_dist_factor *= percent;
// 		if (slice_dist_factor > 4) {
// 			slice_dist_factor = 4;
// 		}
// 		if (slice_dist_factor < 1.0) {
// 			slice_dist_factor = 1.0;
// 		}
// 
// 		updateSliceDistFact(percent);

		emit(signalUpdateSlice(percent));

	} else {
		Translate(0, 0, ((float)event->delta()) / 360);
	}
}


void MouseHandle::Rotate(float ox, float oy, float nx, float ny)
{
	quat q;
	Camera::Arc2Quaternion(ox, oy, nx, ny, q);

	if (rotate_dist == 0) {
		q[1] = -q[1];
		q[2] = -q[2];
	}

	theCamera.Rotate(q, rotate_dist);
}

void MouseHandle::Translate(float dx, float dy, float dz)
{
	// some auto param ajustment to be refined later
	float scalefactor = 0.5f*view_dist;
	dx *= scalefactor*theCamera.fov;
	dy *= scalefactor*theCamera.fov;
	dz = view_dist * (exp(-0.5f * dz) - 1.0f);

	// moving the camera in the opposite direction to
	// the direction we want the object to move
	theCamera.Move(-dx, -dy, -dz);
	UpdateDist(-dx, -dy, -dz);
}

// Mouse event helper function - update distance
void MouseHandle::UpdateDist(float dx, float dy, float dz)
{
	vec3 viewdir(0, 0, -view_dist);
	vec3 delta(-dx, -dy, -dz);

	vec3 newviewdir = viewdir + delta;

	view_dist = -newviewdir.z;
	// Always rotate around the center of the object
	// which is assumed to be place at the origin of the world coordinate
	rotate_dist = view_dist;
}

void MouseHandle::slotSetScreenWidth(int w) {
	screenwidth = w;
}

void MouseHandle::slotSetScreenHeight(int h) {
	screenheight = h;
}