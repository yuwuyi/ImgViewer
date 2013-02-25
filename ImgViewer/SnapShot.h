#ifndef SnapShot_h__
#define SnapShot_h__

#include <vector>
#include "Point.h"

class SnapShot {
public:
	int id;
	int slice_part;
	int slice_dir;
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