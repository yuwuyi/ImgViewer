#ifndef MouseHandle_h__
#define MouseHandle_h__

#include "Camera.h"
#include <QObject>
#include <QPoint>

class QMouseEvent;
class QWheelEvent;

class MouseHandle : public QObject {
	Q_OBJECT
public:

	MouseHandle(Camera& c, double &rd, double &vd);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent( QWheelEvent * event );

signals:
	void isClick(int x, int y);
	void signalUpdateSlice(double percent);

	public slots:
		void setCtrl(bool c);
		void slotSetScreenWidth(int w);
		void slotSetScreenHeight(int h);
private:
	void Mousei2f(int x, int y, float *xf, float *yf);
	void Rotate(float ox, float oy, float nx, float ny);
	void Translate(float dx, float dy, float dz);
	void UpdateDist(float dx, float dy, float dz);

	bool isCtrl;
	Camera &theCamera;
	double &rotate_dist;
	double &view_dist;
	QPointF lastPos;
	QPointF pressPos;

	int screenwidth;
	int screenheight;
};
#endif // MouseHandle_h__