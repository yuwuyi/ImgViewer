#include "MeshDelegate.h"
#include "Iterators.h"

using namespace XMeshLib;

Vertex * MeshDelegate::createVertex( int id )
{
	return mesh->createVertex(id);
}

Face * MeshDelegate::createFace( int * v, int id)
{
//	if (!aux)
//	{
//		aux = true;
//		mesh->setupAux();
//	}
	return mesh->createFace(v,id);
}

void MeshDelegate::VertexDone()
{
	if (!mesh->aux)
	{
		mesh->setupAux();
		gotAux = true;
	}
}

void MeshDelegate::AllDone()
{
	mesh->removeAux();
	gotAux = false;

	for (MeshVertexIterator viter(mesh); !viter.end(); ++viter)
	{
		Vertex * v = *viter;
		v->boundary() = false;
	}

	for(MeshEdgeIterator eiter(mesh); !eiter.end(); ++eiter)
	{
		Edge * e = *eiter;
		if (!e->halfedge(1))
		{
			assert(e->halfedge(0));
			e->halfedge(0)->source()->boundary() = true;
			e->halfedge(0)->target()->boundary() = true;
		}
		
	}
}
/*
void MeshDelegate::resetAux()
{
	for (MeshHalfEdgeIterator heiter(mesh); !heiter.end(); ++heiter)
	{
		HalfEdge * he = *heiter;
		Vertex * ver = he->target();
		AdjHeTrait * t_he = (AdjHeTrait *)ver->GetTrait(ID_AdjHeTrait);
		t_he->AddHE(he);

	}
	

	std::vector<Vertex *>::iterator vit;
	for (vit=m_verts.begin(); vit!=m_verts.end(); ++vit)
	{
		Vertex * ver = *vit;
		AdjHeTrait * t_he = (AdjHeTrait *)ver->GetTrait(ID_AdjHeTrait);
		t_he->AddHE(he);
		ver->AddTrait(t_he);
	//	m_allV[ver->id()] = ver;			
	}
}*/
