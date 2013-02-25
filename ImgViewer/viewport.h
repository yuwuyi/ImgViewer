#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QString>
#include <QGLWidget>
#include <vector>
#include <map>

#include "Camera.h"
#include "EasyBMP.h"
#include "SliceMesh.h"
#include "Point.h"

class SnapShot;
namespace XMeshLib {
	class Vertex;
	class Edge;
}

class KeyHandle;
class MouseHandle;

typedef std::map<int, XMeshLib::Vertex*> VerMap;
typedef std::vector<XMeshLib::Edge*> Edges;
typedef std::vector< std::vector < std::vector <RGBApixel> > > RGBCube;

class ViewPort : public QGLWidget
{
	Q_OBJECT

public:
	ViewPort(QWidget *parent);
	~ViewPort();
	void loadImages(const QString & dirName);

	public slots:
		void sliceDir(int slicedir);
		void slice(int s);
		void adjustContour(int);
		void snapshot();
		void blackcolor();
		void whitecolor();
		void click(int x, int y);
		void slotUpdateSlice(double percent);
signals:
		void ctrlPress(bool b);
protected:
	void initializeGL ();
	void initTexture();
	void resizeGL ( int width , int height );
	void paintGL ();

 	void mousePressEvent(QMouseEvent *event);
 	void mouseMoveEvent(QMouseEvent *event);
 	void mouseReleaseEvent(QMouseEvent *event);
 	void wheelEvent( QWheelEvent * event );

	//key handles
	void keyPressEvent ( QKeyEvent * event );
	void keyReleaseEvent ( QKeyEvent * event );

private:
// 	void Mousei2f(int x, int y, float *xf, float *yf);
// 	void Rotate(float ox, float oy, float nx, float ny);
// 	void Translate(float dx, float dy, float dz);
// 	void UpdateDist(float dx, float dy, float dz);
	Camera theCamera;
	double rotate_dist;							// How far the center of the trackball is from the camera
	double view_dist;							// How far the center of the world is from the camera

// 	QPointF lastPos;
// 	QPointF pressPos;

	void InitCamera();
	void compute_image_snapshot();
	void add_mesh_contour();
	//image rgbs
	//CubeRGB: [slotid][height][weight]
	
	
	double z_threshold;
	double xy_threshold;
	bool isShowFullMesh;
	double stretch_factor;
	
	std::vector<SnapShot*> snapshots;
	
	void render_snapshot(SnapShot *shot);
	void renderMesh();
	bool isCtrl;
	//texture
	//GLuint texName;
	//GLubyte* texture;
	
public :
	
	RGBCube rgbCube;
	SliceMesh *slicemesh;
	SnapShot *currentSnapshot;
	int slicedir;
	int slicepart;

	XMeshLib::Point background;
signals:
	void triggerUpdateSliceInfo();

private:
	void doPickingSnapShot(int x, int y);
	void ProcessHits(GLint hits, GLuint buffer[]);
	int pickingSnapShot;
	void updateSliceDistFact(double percent);

	MouseHandle *mouseHandle;
	KeyHandle *keyHandle;

public:
	double slice_dist_factor;
	
};

#endif // VIEWPORT_H
