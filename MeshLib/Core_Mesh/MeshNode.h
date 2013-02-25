#ifndef _XMESHNODE_H_
#define _XMESHNODE_H_

#include "Mesh.h"

namespace XMeshLib{

	struct MeshNode
	{
		Mesh * mesh;
		std::string meshName;
	};

	typedef std::vector<MeshNode> MeshListVec;

};
#endif