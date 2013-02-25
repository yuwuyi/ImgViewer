#include "imgviewer.h"
#include <QFileDialog>
#include "viewport.h"
#include "SliceMesh.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Iterators.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace XMeshLib;
extern VerMap gVers;
extern Edges gEdges;
extern Mesh *gMesh;

ImgViewer::ImgViewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	connect(ui.widget, SIGNAL(triggerUpdateSliceInfo()), this, SLOT(updateSliceInfo()));
}

void ImgViewer::updateSliceInfo() {
	//ui.horizontalSlider->setValue(ui.widget->slicepart);
	
	sliceDir(ui.widget->slicedir);
	slice(ui.widget->slicepart);
	//ui.comboBox->setCurrentIndex(ui.widget->slicedir);
	
}

void ImgViewer::loadImages() {

	QString outputDir_ = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		"",	QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	ui.widget->loadImages(outputDir_);
	//ReadImageArray(outputDir_);

	sliceDir(2);
	slice(0);
	ui.widget->slice(0);
}

ImgViewer::~ImgViewer()
{

}

void ImgViewer::loadCM() {
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "~", tr("Contour Files (*.cm)"));
	
	if (fileName.isEmpty()) {
		return;
	}

	std::ifstream input(fileName.toStdString().c_str());
	while(input.good()) {
		std::string line;
		getline(input, line);
		if (line.empty()) {
			continue;
		}

		std::string title;
		std::stringstream ss(line);
		ss >> title;
		if (title == "Vertex") {
			int id = -1;
			ss >> id;

			if (id > 0) {

				Vertex *vertex = new Vertex();
				vertex->id() = id + gMesh->maxid;
				ss >> vertex->point().v[1] >> vertex->point().v[0]>> vertex->point().v[2];
				vertex->point() /= 512;
				vertex->point() -= Point(0.5, 0.5, 0);
				gVers[vertex->id()] = vertex;
			}
		}
		if (title == "Edge") {
			int id0, id1;
			Edge *edge = new Edge;
			ss >> id0 >> id1;
			edge->vid(0) = id0 + gMesh->maxid;
			edge->vid(1) = id1 + gMesh->maxid;
			gEdges.push_back(edge);
		}
	}
	input.close();

}


void ImgViewer::sliceDir(int sd) {
	ui.comboBox->setCurrentIndex(sd);
	if (sd == 2) {
		if (ui.widget->slicepart > ui.widget->rgbCube.size() - 1) {
			ui.widget->slicepart = ui.widget->rgbCube.size() - 1;
		}
		ui.horizontalSlider->setValue(ui.widget->slicepart);

		ui.horizontalSlider->setMaximum(ui.widget->rgbCube.size() - 1);
		//ui.horizontalSlider->setValue(ui.widget)
	
	} else {

		ui.horizontalSlider->setMaximum(ui.widget->rgbCube[0][0].size() - 1) ;

		if (ui.widget->slicepart > ui.widget->rgbCube[0][0].size() - 1) {
			ui.widget->slicepart = ui.widget->rgbCube[0][0].size() - 1;
		}
		ui.horizontalSlider->setValue(ui.widget->slicepart);

		
	
		
	}
}

void ImgViewer::slice(int s) {
// 	if (s > ui.horizontalSlider->maximum()) {
// 		s = ui.horizontalSlider->maximum() - 1;
// 	}
	ui.indicator->setText(QString("%1").arg(s));
	//ui.widget->slice(s);
}

void ImgViewer::LoadMesh() {
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "~", tr("Mesh Files (*.m)"));

	if (fileName.isEmpty()) {
		return;
	}

	gMesh->read(fileName.toStdString().c_str());

	//process gMesh;
	for (MeshVertexIterator mvit(gMesh); !mvit.end(); ++mvit) {
		Vertex *vertex = *mvit;
		vertex->point()[0] /= 512;
		vertex->point()[1] /= 512;
		vertex->point()[2] *= ui.widget->slice_dist_factor;

		if (!ui.widget->rgbCube.empty()) {
			vertex->point()[2] /= ui.widget->rgbCube.size();
		}
		
		vertex->point() -= Point(0.5, 0.5, 0);
		std::swap(vertex->point()[0], vertex->point()[1]);
	}
}