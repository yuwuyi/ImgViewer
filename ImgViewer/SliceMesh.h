#ifndef SliceMesh_h__
#define SliceMesh_h__

#include <vector>
#include "Point.h"

namespace XMeshLib {
	class Mesh;
	class Face;
}



class SliceMesh {
public:
	enum DIRECTION {
		DIR_X = 0,
		DIR_Y = 1,
		DIR_Z = 2,
	};

	SliceMesh(XMeshLib::Mesh *mesh);
	void slice();

	std::vector<XMeshLib::Point> interpts;
	DIRECTION dir;
	double displacement;


private:
	XMeshLib::Mesh *mesh;
	std::vector<XMeshLib::Face*> candidate_faces;

private:
	void pick_candidate();
	void reorder();
	void cut();
	void dump();
};

#endif // SliceMesh_h__