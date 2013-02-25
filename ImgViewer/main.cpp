#include "imgviewer.h"
#include "viewport.h"
#include "Mesh.h"
#include "SliceMesh.h"
#include <QtGui/QApplication>


using namespace XMeshLib;

VerMap gVers;
Edges gEdges;
Mesh *gMesh;


int main(int argc, char *argv[])
{
// 	if (argc != 3) {
// 		exit(-1);
// 	}

	gMesh = new Mesh;
	//gMesh->read(argv[1]);

	//for (int slice_part = 10; slice_part < 1000; slice_part +=10) {
// 		SliceMesh slice(gMesh, SliceMesh::DIR_X, 240);
// 		slice.slice();
// 	//}
	
	
//	exit(-1);
// 	gMesh->read(argv[1]);
// 	loadCM(argv[2], gVers, gEdges);

	QApplication a(argc, argv);
	ImgViewer w;
	w.show();
	return a.exec();
}
