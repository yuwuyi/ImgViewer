#pragma warning (disable: 4267)

#ifndef _XMESHLIB_CURVE_H_
#define _XMESHLIB_CURVE_H_

#include "Mesh.h"

namespace XMeshLib
{

	class Curve
	{
	public:
		Curve():valid(true){}		

		//These constructors are used for input as already sorted halfedge lists. assuming the validity
		Curve(int length, HalfEdge * hes[]);
		Curve(std::vector<HalfEdge *> & inhelist);


		//This constructor is more general, given a starting vertex and a bunch of unlisted edges
		Curve(std::list<Edge *> edges, Vertex * startV);

		//This constructor is for closed curve, take any vertex as the start vertex
		Curve(std::list<Edge *> edges);

		~Curve();

		void BuildList(std::list<Edge *> edges, Vertex * startV);

		//Initiate a boundary curve started from the given vertex(tracing from either direction) or the given halfedge
		int CreateBoundaryLoop(Vertex * startV);
		int CreateBoundaryLoop(HalfEdge * startHe);
		bool RobustTraceBoundaryLoop(HalfEdge * startHe, int & loopSize);

		void TraceBoundary(HalfEdge * startHe, int step);

		//Trace the feature curve loop on the given mesh, back to the halfedge "heStart"
		int TraceFeatureCurve(Mesh * mesh, HalfEdge * heStart);

		void TraceBoundary(HalfEdge * startHe, Vertex * endV);

		Vertex * Head() const
		{
			if (valid)
				return helist[0]->source();
			else
				return NULL;
		}
		
		Vertex * Tail() const
		{
			if (valid)
				return helist[helist.size()-1]->target();
			else
				return NULL;
		}

		Vertex * VertexAt(int const ind);		
		
		HalfEdge * operator[] (int const ind) const
		{
			if (!valid || ind < 0 || (ind >= static_cast<int>(helist.size())))
				return NULL;
			else 
				return helist[ind];
		}

		bool AppendHalfEdge(HalfEdge * he)
		{
			helist.push_back(he);
			return true;
		}

		int verSize();
		
		int heSize() const
		{
			if (!valid)
				return -1;
			else
				return helist.size();
		}

		void Clear();

		//I/O Operations
		bool WriteToFile(const char file[]);
		bool ReadFromFile(Mesh * mesh, const char file[]);
		void MarkOnMesh() const;

		void BruteConcatenate(const Curve & c);
		bool Concatenate(const Curve & c);

		void RemoveHalfEdges(int ind);
		void SwapHalfEdge(int ind, HalfEdge * nhe);
		void ReplaceSections(int startInd, int endInd, std::vector<HalfEdge  *> & replaceList);
		void ReplaceSections(Vertex * startV, Vertex * endV, std::vector<HalfEdge  *> & replaceList);

	private:
		bool valid;
		std::vector<HalfEdge *> helist;
	};

}

#endif
