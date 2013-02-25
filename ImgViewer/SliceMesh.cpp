#include "SliceMesh.h"
#include "Mesh.h"
#include "Iterators.h"

using namespace XMeshLib;

SliceMesh::SliceMesh(XMeshLib::Mesh *m) 
: mesh(m)
{

}

void SliceMesh::pick_candidate() {
	for (MeshFaceIterator mfit(mesh); !mfit.end(); ++mfit) {
		Face *face = *mfit;
		double bbmin = 1e6;
		double bbmax = -1e6;

		for (FaceVertexIterator fvit(face); !fvit.end(); ++fvit) {
			Vertex *v = *fvit;
			int ele = -1;
			if (dir == 2) {
				ele = 2;
			} else if (dir == 0) {
				ele = 1;
			} else if (dir == 1) {
				ele = 0;
			}
			
			double bbvalue = v->point()[ele];
			if (bbvalue < bbmin) {
				bbmin = bbvalue;
			}
			if (bbvalue > bbmax) {
				bbmax = bbvalue;
			}
		}

		//cut
		if (bbmin <= displacement && displacement <= bbmax) {
			candidate_faces.push_back(face);
		}
	}

}

void SliceMesh::reorder() {
	std::vector<Face*> order_faces;
	if (candidate_faces.empty()) {
		return;
	}
	Face *indicator = *candidate_faces.begin();
	candidate_faces.erase(candidate_faces.begin());
	
	while(!candidate_faces.empty()) {
		order_faces.push_back(indicator);

		bool found = false;
		for (FaceHalfedgeIterator fhit(indicator); !fhit.end(); ++fhit) {
			HalfEdge *he = *fhit;
			HalfEdge *ohe = he->edge()->other(he);
			if (ohe) {
				Face *nextFace = ohe->face();
				std::vector<Face*>::iterator it = std::find(candidate_faces.begin(), candidate_faces.end(), nextFace);
				if ( it != candidate_faces.end()) {
					indicator = *it;
					candidate_faces.erase(it);
					found = true;
					break;
				}
			}
		}

		if (!found && !candidate_faces.empty()) {
			indicator = *candidate_faces.begin(); 
			candidate_faces.erase(candidate_faces.begin());
		}
	}

	candidate_faces = order_faces;

}

int
intersect3D_SegmentPlane( Point SP0, Point SP1, Point Pn, Point PV0, Point& I )
{
	Point    u = SP1 - SP0;
	Point    w = SP0 - PV0;

	float     D = Pn * u;//dot(Pn.n, u);
	float     N = -Pn * w;//dot(Pn.n, w);

	if (fabs(D) < 1e-6) {          // segment is parallel to plane
		if (N == 0)                     // segment lies in plane
			return 2;
		else
			return 0;                   // no intersection
	}
	// they are not parallel
	// compute intersect param
	float sI = N / D;
	if (sI < 0 || sI > 1)
		return 0;                       // no intersection

	I = SP0 +  u * sI;                 // compute segment intersect point
	return 1;
}

void SliceMesh::cut() {
	interpts.clear();
	for (size_t fid = 0; fid < candidate_faces.size(); ++fid) {
		Face *face = candidate_faces[fid];
		std::vector<Point> newPts;
		for (FaceHalfedgeIterator fhit(face); !fhit.end(); ++fhit) {
			Edge *edge = (*fhit)->edge();
			Point s0 = mesh->idVertex(edge->vid(0))->point();
			Point s1 = mesh->idVertex(edge->vid(1))->point();

			Point pn(0, 0, 0);
			

			if (dir == DIR_X) {
				pn[1] = 1;
			} else if (dir == DIR_Y) {
				pn[0] = 1;
			} else if (dir == DIR_Z) {
				pn[2] = 1;
			}

			Point pv0(0, 0, 0);
			//pv0[dir] = displacement;
			if (dir == DIR_X) {
				pv0[1] = displacement;
			} else if (dir == DIR_Y) {
				pv0[0] = displacement;
			} else if (dir == DIR_Z) {
				pv0[2] = displacement;
			}


			Point pt;
			
			if (intersect3D_SegmentPlane(s0, s1, pn, pv0, pt) == 1) {
				interpts.push_back(pt);
				newPts.push_back(pt);
			}

			
			
		}

		if (dir == DIR_X || dir == DIR_Y) {
			if (newPts.size() == 2) {
				for (int i = 1; i < 29; ++i) {
					Point ptx = newPts[0] * i * (1/30.0) + newPts[1] * (1 - i * (1/30.0));
					interpts.push_back(ptx);
				}
			}
		}
	}
}

#include <iostream>
#include <fstream>

void SliceMesh::dump() {
	std::ofstream output("loop.cm");
	
	for (size_t pid = 0; pid < interpts.size(); ++pid) {
		Point pt = interpts[pid];
		output << "Vertex " << pid + 1 << " " << pt[0] << " " << pt[1] << " " << pt[2] << "\n";
	}

	output.close();
}

void SliceMesh::slice() {
	candidate_faces.clear();
	//first pick the faces
	std::cout << displacement << " ";
	pick_candidate();
	std::cout << "cand: " << candidate_faces.size() << "\n";
	//reorder
	if (dir == DIR_X || dir == DIR_Y) {
		reorder();
	}
	

	//cut
	cut();

//	dump();
}