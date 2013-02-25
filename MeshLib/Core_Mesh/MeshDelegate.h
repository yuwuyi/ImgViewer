#ifndef _XMESHLIB_SOLID_DELEGATE_H_
#define _XMESHLIB_SOLID_DELEGATE_H_

#include "Mesh.h"

namespace XMeshLib{

class MeshDelegate
{
	public:
		MeshDelegate(Mesh * pS) 
		{
			mesh = pS;
			gotAux = false;
		}
		~MeshDelegate()
		{
			//if (gotAux)
			//	AllDone();
		}

		void VertexDone();
		void AllDone();

		Vertex *  createVertex( int id  );
		Face   *  createFace( int * v, int id ); 

		bool gotAux;
		Mesh * mesh;

};		

}//xmeshlib 
#endif
