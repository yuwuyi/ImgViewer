#include "viewport.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Mesh.h"
#include "Iterators.h"
#include "SnapShot.h"
#include "imgviewer.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QString>
#include <QDir>
#include <iostream>

typedef Vector3f vec3;
typedef Vector3d vec3d;
typedef Quatf quat;

using namespace XMeshLib;

extern VerMap gVers;
extern Edges gEdges;
extern Mesh *gMesh;



ViewPort::ViewPort(QWidget *parent)
: QGLWidget(parent)
{
	setFocusPolicy(Qt::StrongFocus);

	slicedir = 2;
	slicepart = 0;
	z_threshold = 100;
	isShowFullMesh = false;
	stretch_factor = 1.0;
	isCtrl = false;
	slicemesh = new SliceMesh(gMesh);
	currentSnapshot = new SnapShot;
	currentSnapshot->id = 0;
	currentSnapshot->clear();
	pickingSnapShot = 0;
	snapshots.push_back(currentSnapshot);
	//slice_dist_factor = 50.0;
	slice_dist_factor = 1.0;
	
}

ViewPort::~ViewPort()
{
	for (size_t sid = 0; sid < snapshots.size(); ++sid) {
		delete snapshots[sid];
	}
}

void ViewPort::initializeGL () {
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	InitCamera();
}

void ViewPort::blackcolor() {
	background = Point(0, 0, 0);
	updateGL();
}

void ViewPort::whitecolor() {
	background = Point(1, 1, 1);
	updateGL();
}


void ViewPort::resizeGL ( int width , int height ) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat x = GLfloat(width) / height;
	glFrustum(-x, +x, -1.0, +1.0, 4.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
}

void ViewPort::paintGL () {
	glDisable(GL_LIGHT0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(background[0], background[1], background[2], 0.5); 
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	theCamera.SetGL(view_dist/5.0f, view_dist*5.0f, width(), height());

	for (size_t sid = 0; sid < snapshots.size(); ++sid) {
		render_snapshot(snapshots[sid]);
	}

	glEnable (GL_BLEND);
	glColor3f(1.0, 0, 0.0);
	glPointSize(5.0);
	glLineWidth(5.0);
	glBegin(GL_LINES);
	for (size_t eid = 0; eid < gEdges.size(); ++eid) {
		Edge *edge = gEdges[eid];
		Vertex *v1 = gVers[edge->vid(0)];
		Vertex *v2 = gVers[edge->vid(1)];
		Point pt1 = v1->point();
		Point pt2 = v2->point();

		if (slicedir == 2 && (pt1[2] > z_threshold || pt2[2] > z_threshold)) {
			continue;
		} else if (slicedir !=2 && (pt1[slicedir] > xy_threshold || pt2[slicedir] > xy_threshold)) {
			continue;
		}

		glVertex3f(pt1[0], pt1[1], pt1[2]);
		glVertex3f(pt2[0], pt2[1], pt2[2]);
	}
	glEnd();
	glPointSize(1.0);
	
	renderMesh();
	glPopMatrix();
}


void ViewPort::renderMesh() {
	// 	The Mesh;
	if (isShowFullMesh) {
		glColor4f(1.0, 0.3, 0, 0.5);
		glPointSize(1.0);
		glLineWidth(1.0);
		glBegin(GL_TRIANGLES);
		for (MeshFaceIterator mfit(gMesh); !mfit.end(); ++mfit) {
			Face *face = *mfit;
			Vertex *vers[3];
			int vid = 0;
			for (FaceVertexIterator fvit(face); !fvit.end(); ++fvit, ++vid) {
				vers[vid] = *fvit;
			}
			glVertex3f(vers[0]->point()[0], vers[0]->point()[1], vers[0]->point()[2]);
			glVertex3f(vers[1]->point()[0], vers[1]->point()[1], vers[1]->point()[2]);
			glVertex3f(vers[2]->point()[0], vers[2]->point()[1], vers[2]->point()[2]);
		}
		glEnd();
	}

}
//Camera
void ViewPort::InitCamera()
{
	vec3 homepos, homerotaxis;

	// Since the model is always normalized, constants suffice
	homepos.x = 0;
	homepos.y = 0;
	homepos.z = 6.0;

	homerotaxis = vec3(0, 0, 1);

	theCamera.SetHome(homepos, 0, homerotaxis, DEFAULT_FOV);
	view_dist = 6.0;
	rotate_dist = 6.0;

	theCamera.GoHome();
}

void ViewPort::Mousei2f(int x, int y, float *xf, float *yf)
{
	int screenwidth = width();
	int screenheight = height();

	float r = (float)screenwidth/screenheight;
	if (r > 1.0) {
		*xf = r * (2.0f * x / screenwidth - 1.0f);
		*yf = 1.0f - 2.0f * y / screenheight;
	} else {
		*xf = 2.0f * x / screenwidth - 1.0f;
		*yf = (1.0f - 2.0f * y / screenheight) / r;
	}
}



void ViewPort::mousePressEvent(QMouseEvent *event)
{
	pressPos = event->pos();
	float nx, ny;
	Mousei2f(event->pos().x(), event->pos().y(), &nx, &ny);
	bool isClick = false;

	isClick = (QPointF(nx, ny) - lastPos).manhattanLength() < 0.01;

	lastPos = QPointF(nx, ny);

	updateGL();



	

}

void ViewPort::mouseReleaseEvent(QMouseEvent *event) {
	bool isClick = false;
	isClick = (event->pos() - pressPos).manhattanLength() < 5;
	updateGL();

	if (isClick ) {
		doPickingSnapShot(event->pos().x(), event->pos().y());
	}
}


void ViewPort::mouseMoveEvent(QMouseEvent *event)
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
	updateGL();
}    
void ViewPort::updateSliceDistFact(double percent) {
	for (size_t sid = 0; sid < snapshots.size(); ++sid) {
		SnapShot *ss = snapshots[sid];
		for (size_t pid = 0; pid < ss->pts.size(); ++pid) {
			ss->pts[pid][2] *= percent;
		}

		for (size_t pid = 0; pid < ss->quads.size(); ++pid) {
			ss->quads[pid][2] *= percent;
		}
	}

	//update mesh;
	for (MeshVertexIterator mvit(gMesh); !mvit.end(); ++mvit) {
		(*mvit)->point()[2] *= percent;
	}
}


void ViewPort::wheelEvent( QWheelEvent * event )  {
 	if (isCtrl) {
 		//control press, adjust the slice_distance_factor
 		double percent = 1 + (GLfloat)event->delta() / 720 ;
	
		if (percent < 0.1) {
 			percent = 0.1;
 		}

		if (percent > 2.0) {
			percent = 2.0;
		}
		
		slice_dist_factor *= percent;
		if (slice_dist_factor > 4) {
			slice_dist_factor = 4;
		}
		if (slice_dist_factor < 1.0) {
			slice_dist_factor = 1.0;
		}
		
 		updateSliceDistFact(percent);
 
 	} else {
		Translate(0, 0, ((GLfloat)event->delta()) / 360);
	}
	updateGL();
}

void ViewPort::keyPressEvent ( QKeyEvent * event ) {
	if (event->modifiers() & Qt::ControlModifier) {
		isCtrl = true;
	}
	std::cout << isCtrl << "\n";

	if (snapshots.size() > 1) {
		if (event->key() == Qt::Key_Delete) {
			currentSnapshot->clear();
			snapshots.erase(snapshots.begin() + currentSnapshot->id);
			for (int sid = 0; sid < snapshots.size(); ++sid) {
				snapshots[sid]->id = sid;
			}
			pickingSnapShot = currentSnapshot->id -1;
			if (pickingSnapShot < 0) {
				pickingSnapShot = 0;
			}
			delete currentSnapshot;
			currentSnapshot = snapshots[pickingSnapShot];


			slicepart = currentSnapshot->slice_part;
			slicedir = currentSnapshot->slice_dir;

			emit (triggerUpdateSliceInfo());
		}
	}

	updateGL();

}

void ViewPort::keyReleaseEvent ( QKeyEvent * event ) {
	//if (event->modifiers() & Qt::ControlModifier) {
		isCtrl = false;
	//}
	std::cout << isCtrl << "\n";
}



void ViewPort::Rotate(float ox, float oy, float nx, float ny)
{
	quat q;
	Camera::Arc2Quaternion(ox, oy, nx, ny, q);

	if (rotate_dist == 0) {
		q[1] = -q[1];
		q[2] = -q[2];
	}

	theCamera.Rotate(q, rotate_dist);
}

void ViewPort::Translate(float dx, float dy, float dz)
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
void ViewPort::UpdateDist(float dx, float dy, float dz)
{
	vec3 viewdir(0, 0, -view_dist);
	vec3 delta(-dx, -dy, -dz);

	vec3 newviewdir = viewdir + delta;

	view_dist = -newviewdir.z;
	// Always rotate around the center of the object
	// which is assumed to be place at the origin of the world coordinate
	rotate_dist = view_dist;
}

void ViewPort::loadImages(const QString & dirName) {
	QDir dir(dirName);

	QStringList filters;
	filters << "*.bmp";
	dir.setNameFilters(filters);

	QStringList lst = dir.entryList( );
	int slotid = 0;
	rgbCube.resize(lst.size());

	foreach (QString fileName, dir.entryList(QDir::Files)) {

		std::cout << "loading " << dir.absoluteFilePath(fileName).toStdString().c_str() << "\n";
		BMP input;
		input.ReadFromFile(dir.absoluteFilePath(fileName).toStdString().c_str());

		size_t height_dim = input.TellHeight();
		size_t width_dim  = input.TellWidth();

		rgbCube[slotid].resize(height_dim);
		for (size_t i = 0; i < height_dim; ++i) {
			rgbCube[slotid][i].resize(width_dim);
		}

		for (size_t i = 0; i < height_dim; ++i) {
			for (size_t j = 0; j < width_dim; ++j) {
				rgbCube[slotid][i][j] = *input(i, j);
			}
		}
		++slotid;
	}
	std::cout << rgbCube.size() << "\n";
}

void ViewPort::sliceDir(int dir) {
	slicedir = dir;
	//std::cout << slicedir << "\n";
	if (dir == 2) {
		if(slicepart >= rgbCube.size()) {
			slicepart = rgbCube.size() - 1;
		}
	} else { if (slicepart >= rgbCube[0].size()) {slicepart = rgbCube[0].size() - 1;}
	}
 	
 	
	slicemesh->dir = (SliceMesh::DIRECTION)dir;
	
	if (dir == 2) {
		if (!rgbCube.empty()) {
			slicemesh->displacement = slicepart / rgbCube.size();
		}
	} else {
		slicemesh->displacement = slicepart / 512.0;
	}
	
	//slicemesh->slice();
	
	slice(slicepart);
	updateGL();
}

void ViewPort::slice(int s) {

	if (s < 0) {
		s = 0;
	}
	if (rgbCube.empty()) {
		return ;
	}

	if (slicedir == 2 && s < rgbCube.size()) {
		slicepart = s;
		z_threshold = (float)(slicepart ) * 1.0 / rgbCube.size() * slice_dist_factor ;
	} else if (s < rgbCube[0].size()) {
		slicepart = s;
		xy_threshold = (float)slicepart / rgbCube[0][0].size() * slice_dist_factor;
	}
	
	currentSnapshot->clear();
	compute_image_snapshot();
	//add to current slice
	add_mesh_contour();

	updateGL();
}


void ViewPort::add_mesh_contour() {
	if (slicedir == 2) {
		if (!rgbCube.empty()) {
			slicemesh->displacement =  1.0 * slicepart / rgbCube.size() * slice_dist_factor;
		}

	} else {
		slicemesh->displacement =  slicepart / 512.0  - 0.5;
	}

	slicemesh->slice();

	currentSnapshot->slice_part = slicepart;
	currentSnapshot->slice_dir = slicedir;


	Point trans_dir(0, 0, 0);
	if (slicedir == 1) {
		trans_dir[0] = -0.01;
	} else if (slicedir == 0) {
		trans_dir[1] = -0.01;
	} else if (slicedir == 2) {
		trans_dir[2] = 0.01;
	}


	size_t slicePtSize = slicemesh->interpts.size();
	for(size_t pid = 0; pid < slicePtSize; ++pid) {
		Point pt1 = slicemesh->interpts[pid] + trans_dir;
		currentSnapshot->pts.push_back(pt1);
		currentSnapshot->rgbs.push_back(Point(1, 0, 0));
	}

}


void ViewPort::render_snapshot(SnapShot *shot) {

	if (shot->pts.empty() && shot->quads.empty()) {
		return;
	}
	glDisable(GL_LIGHTING);
	glPointSize(2.0);
	
 	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
 	glEnable(GL_BLEND);		// Turn Blending On
 	glDisable(GL_DEPTH_TEST);	// Turn Depth Testing Off

	double minx = 1e10, miny = 1e10, maxx = -1e10, maxy = -1e10, maxz = -1e10, minz = 1e10;

	const double OPACITY = 0.8;

	glLoadName(shot->id + 1);

	
	glBegin(GL_POINTS);
	for (size_t sid = 0; sid < shot->pts.size(); ++sid) {
		Point pt = shot->pts[sid];
		Point rgb = shot->rgbs[sid];
 
 		glColor4f(rgb[0], rgb[1], rgb[2], OPACITY);
 		glVertex3f(pt[0], pt[1], pt[2]);
		
		if (pt[0] < minx) {
			minx = pt[0];
		}
		if (pt[1] < miny) {
			miny = pt[1];
		}
		if (pt[0] > maxx) {
			maxx = pt[0];
		}
		if (pt[1] > maxy) {
			maxy = pt[1];
		}

		if (pt[2] > maxz) {
			maxz = pt[2];
		}
		if (pt[2] < minz) {
			minz = pt[2];
		}
	}
	glEnd();
	
	glLineWidth(4.0);	
	for (size_t sid = 0; sid < shot->quads.size(); ) {
		glBegin(GL_QUADS);
		for (int i = 0; i < 4; ++i, ++sid) {
				
			Point pt = shot->quads[sid];
			Point rgb = shot->quad_rgbs[sid];

			if (pt[0] < minx) {
				minx = pt[0];
			}
			if (pt[1] < miny) {
				miny = pt[1];
			}
			if (pt[0] > maxx) {
				maxx = pt[0];
			}
			if (pt[1] > maxy) {
				maxy = pt[1];
			}

			if (pt[2] > maxz) {
				maxz = pt[2];
			}
			if (pt[2] < minz) {
				minz = pt[2];
			}
			
			glColor4f(rgb[0], rgb[1], rgb[2], OPACITY);
			glVertex3f(pt[0], pt[1], pt[2]);
			
		}
		glEnd();
	}
	
	glDisable(GL_BLEND);		// Turn Blending Off
	glEnable(GL_DEPTH_TEST);	// Turn Depth Testing On

	//render for picking
	//glLoadName(shot->id + 1);
	//background
	glLineWidth(0.1);


	//the yellow boundary
	if (shot == currentSnapshot) {
		glColor3f(1.0, 1.0, 0);
	} else {
		glColor3f(0.7, 0.7, 0.7);
	}

		double z_value = maxz;
		glLineWidth(1.0);
		
		glBegin(GL_LINES);
		if (shot->slice_dir == 2){
			glVertex3f(0.6, 0.6, z_value);
			glVertex3f(0.6, -0.6, z_value);

			glVertex3f(-0.6, -0.6, z_value);
			glVertex3f(-0.6, 0.6, z_value);

			glVertex3f(0.6, -0.6, z_value);
			glVertex3f(-0.6, -0.6, z_value);

			glVertex3f(-0.6, 0.6, z_value);
			glVertex3f(0.6, 0.6, z_value);
		} else if (shot->slice_dir == 1) {
			glVertex3f(minx, miny - 0.1, maxz + 0.1);
			glVertex3f(minx, maxy + 0.1, maxz + 0.1);

			glVertex3f(minx, miny - 0.1, minz - 0.1);
			glVertex3f(minx, maxy + 0.1, minz - 0.1);

			glVertex3f(minx, miny - 0.1, maxz + 0.1);
			glVertex3f(minx, miny - 0.1, minz - 0.1);

			glVertex3f(minx, maxy + 0.1, maxz + 0.1);
			glVertex3f(minx, maxy + 0.1, minz - 0.1);
		} else if (shot->slice_dir == 0) {
			glVertex3f(minx - 0.1, miny, maxz + 0.1);
			glVertex3f(maxx + 0.1, miny, maxz + 0.1);

			glVertex3f(minx - 0.1, miny, minz - 0.1);
			glVertex3f(maxx + 0.1, miny, minz - 0.1);

			glVertex3f(minx - 0.1, miny, maxz + 0.1);
			glVertex3f(minx - 0.1, miny, minz - 0.1);

			glVertex3f(maxx + 0.1, miny, maxz + 0.1);
			glVertex3f(maxx + 0.1, miny, minz - 0.1);
		}

		glEnd();


}

void ViewPort::compute_image_snapshot() {
	size_t stack_size = rgbCube.size();
	if (stack_size == 0) {
		return;
	}

	if (slicedir == 2) {

		bool using_pts_drawing = false;
		if (using_pts_drawing) {
			size_t height_dim = rgbCube[slicepart].size();
			size_t width_dim  = rgbCube[slicepart][0].size();

			for (size_t i = 0; i < height_dim; ++i) {
				for (size_t j = 0; j < width_dim; ++j) {
					currentSnapshot->pts.push_back(Point((float)i / height_dim - 0.5, 
						(float)j/ width_dim - 0.5, z_threshold - 0.001));
					currentSnapshot->rgbs.push_back(Point(rgbCube[slicepart][i][j].Red /255.0, rgbCube[slicepart][i][j].Green /255.0, rgbCube[slicepart][i][j].Blue/255.0));
				}
			}
		} else {

			size_t height_dim = rgbCube[slicepart].size();
			size_t width_dim  = rgbCube[slicepart][0].size();

			for (size_t i = 0; i < height_dim - 1; ++i) {
				for (size_t j = 0; j < width_dim - 1; ++j) {
					currentSnapshot->quads.push_back(Point((float)i / height_dim - 0.5, 
						(float)j/ width_dim - 0.5, z_threshold - 0.001));
					currentSnapshot->quad_rgbs.push_back(Point(rgbCube[slicepart][i][j].Red /255.0, rgbCube[slicepart][i][j].Green /255.0, rgbCube[slicepart][i][j].Blue/255.0));

					currentSnapshot->quads.push_back(Point((float)(i + 1) / height_dim - 0.5, 
						(float)j/ width_dim - 0.5, z_threshold - 0.001));
					currentSnapshot->quad_rgbs.push_back(Point(rgbCube[slicepart][i + 1][j].Red /255.0, rgbCube[slicepart][i + 1][j].Green /255.0, rgbCube[slicepart][i + 1][j].Blue/255.0));

					currentSnapshot->quads.push_back(Point((float)(i + 1) / height_dim - 0.5, 
						(float)(j + 1)/ width_dim - 0.5, z_threshold - 0.001));
					currentSnapshot->quad_rgbs.push_back(Point(rgbCube[slicepart][i + 1][j + 1].Red /255.0, rgbCube[slicepart][i + 1][j + 1].Green /255.0, rgbCube[slicepart][i + 1][j + 1].Blue/255.0));

					currentSnapshot->quads.push_back(Point((float)(i) / height_dim - 0.5, 
						(float)(j + 1)/ width_dim - 0.5, z_threshold - 0.001));
					currentSnapshot->quad_rgbs.push_back(Point(rgbCube[slicepart][i][j + 1].Red /255.0, rgbCube[slicepart][i][j + 1].Green /255.0, rgbCube[slicepart][i][j + 1].Blue/255.0));

				}
			}

		}
	} else if (slicedir == 1) {
		size_t height_dim = rgbCube.size();
		size_t width_dim  = rgbCube[0].size();
		size_t stack_dim = rgbCube[0][0].size();
		//glPointSize(5.0);
		//glBegin(GL_QUADS);
		for (size_t i = 0; i < height_dim - 1; ++i) {
			for (size_t j = 0; j < width_dim - 1; ++j) {
				currentSnapshot->quads.push_back(Point((float)slicepart / stack_dim - 0.001 - 0.5, ((float)j / width_dim - 0.5) * stretch_factor, 
						((float)i / height_dim) * stretch_factor * slice_dist_factor));
				currentSnapshot->quad_rgbs.push_back(Point(rgbCube[i][slicepart][j].Red /255.0, rgbCube[i][slicepart][j].Green /255.0, rgbCube[i][slicepart][j].Blue/255.0));

				currentSnapshot->quads.push_back(Point((float)slicepart / stack_dim - 0.001 - 0.5, ((float)(j + 1) / width_dim - 0.5) * stretch_factor,  
						((float)i / height_dim ) * stretch_factor * slice_dist_factor));
				currentSnapshot->quad_rgbs.push_back(Point(rgbCube[i][slicepart][j + 1].Red /255.0, rgbCube[i][slicepart][j + 1].Green /255.0, rgbCube[i][slicepart][j +1 ].Blue/255.0));

				currentSnapshot->quads.push_back(Point((float)slicepart / stack_dim - 0.001 - 0.5, ((float)(j + 1) / width_dim - 0.5) * stretch_factor,  
						((float)(i + 1) / height_dim) * stretch_factor * slice_dist_factor));
				currentSnapshot->quad_rgbs.push_back(Point(rgbCube[i + 1][slicepart][j + 1].Red /255.0, rgbCube[i + 1][slicepart][j + 1].Green /255.0, rgbCube[i + 1][slicepart][j +1 ].Blue/255.0));

				currentSnapshot->quads.push_back(Point((float)slicepart / stack_dim - 0.001 - 0.5,  ((float)(j) / width_dim - 0.5) * stretch_factor, 
						((float)(i + 1) / height_dim) * stretch_factor * slice_dist_factor));
				currentSnapshot->quad_rgbs.push_back(Point(rgbCube[i + 1][slicepart][j ].Red / 255.0, rgbCube[i + 1][slicepart][j].Green /255.0, rgbCube[i + 1][slicepart][j].Blue/255.0));
			}
		}
		//glEnd();

	} else if (slicedir == 0) {
		size_t height_dim = rgbCube.size();
		size_t width_dim  = rgbCube[0].size();
		size_t stack_dim = rgbCube[0][0].size();
		//glPointSize(10.0);
		//glBegin(GL_QUADS);
		for (size_t i = 0; i < height_dim - 1; ++i) {
			for (size_t j = 0; j < width_dim - 1; ++j) {
				//glColor3f(rgbCube[i][j][slicepart].Red /255.0, rgbCube[i][j][slicepart].Green /255.0, rgbCube[i][j][slicepart].Blue/255.0);
				//glVertex3f(((float)j  / width_dim - 0.5) * stretch_factor, (float)slicepart / stack_dim - 0.001, 
					//((float)i / height_dim - 0.5) * stretch_factor );
				currentSnapshot->quad_rgbs.push_back(Point(rgbCube[i][j][slicepart].Red /255.0, rgbCube[i][j][slicepart].Green /255.0, rgbCube[i][j][slicepart].Blue/255.0));
				currentSnapshot->quads.push_back(Point(((float)j  / width_dim - 0.5) * stretch_factor, (float)slicepart / stack_dim - 0.001 - 0.5, 
					((float)i / height_dim) * stretch_factor * slice_dist_factor));

				//glColor3f(rgbCube[i][j + 1][slicepart].Red /255.0, rgbCube[i][j + 1][slicepart].Green /255.0, rgbCube[i][j + 1][slicepart].Blue/255.0);
				//glVertex3f(((float)(j + 1)/ width_dim - 0.5) * stretch_factor ,  (float)slicepart / stack_dim - 0.001,
				//	((float)(i) / height_dim - 0.5) * stretch_factor);

				currentSnapshot->quad_rgbs.push_back(Point(rgbCube[i][j + 1][slicepart].Red /255.0, rgbCube[i][j + 1][slicepart].Green /255.0, rgbCube[i][j + 1][slicepart].Blue/255.0));
				currentSnapshot->quads.push_back(Point(((float)(j + 1)/ width_dim - 0.5) * stretch_factor ,  (float)slicepart / stack_dim - 0.001 - 0.5,
						((float)(i) / height_dim ) * stretch_factor * slice_dist_factor));

				//glColor3f(rgbCube[i + 1][j + 1][slicepart].Red /255.0, rgbCube[i + 1][j + 1][slicepart].Green /255.0, rgbCube[i + 1][j + 1][slicepart].Blue/255.0);
				//glVertex3f(((float)(j + 1) / width_dim - 0.5) * stretch_factor ,  (float)slicepart / stack_dim - 0.001,
				//((float)(i + 1) / height_dim - 0.5) * stretch_factor);

				currentSnapshot->quad_rgbs.push_back(Point(rgbCube[i + 1][j + 1][slicepart].Red /255.0, rgbCube[i + 1][j + 1][slicepart].Green /255.0, rgbCube[i + 1][j + 1][slicepart].Blue/255.0));
				currentSnapshot->quads.push_back(Point(((float)(j + 1) / width_dim - 0.5) * stretch_factor ,  (float)slicepart / stack_dim - 0.001 - 0.5,
					((float)(i + 1) / height_dim ) * stretch_factor * slice_dist_factor));

				//glColor3f(rgbCube[i + 1][j][slicepart].Red /255.0, rgbCube[i + 1][j][slicepart].Green /255.0, rgbCube[i + 1][j][slicepart].Blue/255.0);
				//glVertex3f(((float)(j)  / width_dim - 0.5) * stretch_factor , (float)slicepart / stack_dim - 0.001,
				//	((float)(i + 1)  / height_dim - 0.5) * stretch_factor );
				currentSnapshot->quad_rgbs.push_back(Point(rgbCube[i + 1][j][slicepart].Red /255.0, rgbCube[i + 1][j][slicepart].Green /255.0, rgbCube[i + 1][j][slicepart].Blue/255.0));
				currentSnapshot->quads.push_back(Point(((float)(j)  / width_dim - 0.5) * stretch_factor , (float)slicepart / stack_dim - 0.001 - 0.5,
					((float)(i + 1)  / height_dim ) * stretch_factor * slice_dist_factor));
			}
		}
		//glEnd();
	}

}

void ViewPort::adjustContour(int adj) {
	isShowFullMesh = adj;
	updateGL();
}

void ViewPort::snapshot() {
	Point tmp_pt;
	if (!currentSnapshot->pts.empty()) {
		tmp_pt = currentSnapshot->pts[0];
	} else if (!currentSnapshot->quads.empty()) {
		tmp_pt = currentSnapshot->quads[0];
	}


	currentSnapshot = new SnapShot;
	currentSnapshot->id = snapshots.size();
	
	currentSnapshot->clear();
	currentSnapshot->pts.push_back(tmp_pt);
	currentSnapshot->rgbs.push_back(Point(0, 0, 0));		
	pickingSnapShot = currentSnapshot->id;
	snapshots.push_back(currentSnapshot);
}


#define SELBUF_SIZE 1024
#define DOF			5.0f

void ViewPort::doPickingSnapShot(int x, int y) {
	GLint hits;
	GLint viewport[4];
	GLuint selectBuf[SELBUF_SIZE];
	
	glGetIntegerv(GL_VIEWPORT, viewport);
	pickingSnapShot = currentSnapshot->id;
	glSelectBuffer(SELBUF_SIZE, selectBuf);
	glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(0);

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5, 5, viewport);
	theCamera.Projection(view_dist/DOF, view_dist*DOF, width(), height());
	glMatrixMode(GL_MODELVIEW);
	theCamera.Modelview();

	// currently only picking on point set supported
	//Renderer::renderTetMeshforPicking();
	for (size_t sid = 0; sid < snapshots.size(); ++sid) {
		render_snapshot(snapshots[sid]);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glFlush();

	hits = glRenderMode(GL_RENDER);
	if (hits < 0)
	{
		fprintf(stderr, "Error: Selection Buffer Overflow!\n");
		return;
	}

#if DEBUG_DOPICKING
	printf("hits: %d\n", hits);
#endif

	ProcessHits(hits, selectBuf);
}
using namespace std;
void ViewPort::ProcessHits(GLint hits, GLuint buffer[])
{
#define DEBUG_PROCESSHITS 1

	GLint i;
	GLuint names, j, *ptr;
	vector<pair<GLfloat, GLuint> > name_array;
	vector<pair<GLfloat, GLuint> >::iterator niter;
	vector<pair<GLfloat, pair<GLuint, GLuint> > > edge_array;
	vector<pair<GLfloat, pair<GLuint, GLuint> > >::iterator eiter;

	pair<GLfloat, GLuint> name_pair;
	pair<GLfloat, GLuint> other_pair;
	pair<GLfloat, pair<GLuint, GLuint> > edge_pair;

	ptr = (GLuint *) buffer;

	for (i = 0; i < hits; i++)
	{
		names = *ptr;
		ptr += 3;

		for (j = 0; j < names; j++)
		{
			// In vertex picking mode, we will have to eliminate duplicates
			// Duplicates don't happen in face/edge mode
			// This is related to the way we render the model for picking
			name_pair = std::make_pair((GLfloat)*(ptr-2)/0x7fffffff, (*ptr));
			name_array.push_back(name_pair);

#if DEBUG_PROCESSHITS
			//			fprintf(stderr, "name: %u z1: %g z2: %g\n", *ptr, (float)*(ptr-2)/0x7fffffff, (float)*(ptr-1)/0x7fffffff);
#endif				
			ptr++;
		}
	}

	// A minor problem: if two points are collinear they may both be picked
	// we need to throw away the one that is farther away
	// A tricky problem: the point which we don't actually see (hidden further away)
	// may gets picked if the picking is not accurate enough and does not actually 
	// pick the point in the front which we DO see
	// Solution: sort the names according to their z1 value

	if (name_array.size() > 1)
	{
		std::sort(name_array.begin(), name_array.end());
	}

	if (name_array.empty())
	{
#if DEBUG_PROCESSHITS
		//fprintf(stderr, "Name array empty!\n");
#endif

		return;
	}

	for (niter = name_array.begin(); niter != name_array.end(); ++niter)
	{
		name_pair = *niter;
		
		if (name_pair.second - 1>= snapshots.size()) {
			continue;
		}
		SnapShot *snapshot = snapshots[name_pair.second - 1];
		if (snapshot)
		{
#if DEBUG_PROCESSHITS
			fprintf(stdout, "Snapshot %d picked!\n", snapshot->id);
#endif			
			pickingSnapShot = snapshot->id;
			currentSnapshot->slice_part = slicepart;
			currentSnapshot->slice_dir = slicedir;

			

			
			
			slicepart = snapshot->slice_part;
			slicedir = snapshot->slice_dir;
			
			currentSnapshot = snapshot;

			emit (triggerUpdateSliceInfo());
		}
#if DEBUG_PROCESSHITS
		else
			fprintf(stderr, "SnapShot not found!\n");
#endif
		break;
	}		

#undef DEBUG_PROCESSHITS

	updateGL();
}
