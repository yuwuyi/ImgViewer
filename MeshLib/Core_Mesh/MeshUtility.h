#ifndef _XMESHLIB_MESHUTILITY_H_
#define _XMESHLIB_MESHUTILITY_H_

#include "Mesh.h"
#include "Iterators.h"
#include "MeshDelegate.h"
#include "TraitParser.h"
#include "Curve.h"

namespace XMeshLib
{
	class MeshUtility
	{
	public:
		static void MarkSharpEdges(Mesh * mesh);

		static void ClearSharpEdges(Mesh * mesh);

		static Mesh * MeshInverse(Mesh * omesh);

		// Input: a list of faces that compose the new mesh
		// Output: the newly created mesh, with original vertices ids
		static Mesh * BuildMeshFromFaceList(std::vector<Face *> flist);

		static void Elongate(Mesh * mesh, double scalex, double scaley, double scalez);

		static void GetMeshBoudaries(Mesh * mesh, const char file[]);

		//Trace all the boundary loops, return the boundary number
		static int TraceAllBoundaries(Mesh * mesh, std::vector<Curve *> & bounds);

		static Point ComputeMassCenter(Mesh * mesh);

		static void ComputeFaceNormal(Mesh *mesh);

		static void ComputeVertexNormal(Mesh *mesh);
	};
}

#endif
