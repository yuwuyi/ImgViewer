#ifndef SnapShot_h__
#define SnapShot_h__

#include <GL/GL.h>
#include <vector>
#include "Point.h"

class SnapShot {
public:
	int id;
	int slice_part;
	int slice_dir;
	GLuint texName;
	GLubyte* texture;

	SnapShot() {
		texName = 0;
		glGenTextures(1, &texName);
		texture = NULL;
	}

	~SnapShot() {
		delete[] texture;
	}

	void clear() {
		pts.clear();
		rgbs.clear();
		quads.clear();
		quad_rgbs.clear();
	}
	

	

	std::vector<XMeshLib::Point> pts;
	std::vector<XMeshLib::Point> rgbs;
	std::vector<XMeshLib::Point> quads;
	std::vector<XMeshLib::Point> quad_rgbs;
};

#endif // SnapShot_h__