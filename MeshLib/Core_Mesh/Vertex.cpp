#include "Vertex.h"
#include "HalfEdge.h"


using namespace XMeshLib;

HalfEdge *  Vertex::most_ccw_in_halfedge()  
{ 
	if( !m_boundary )
	{
		return m_halfedge; //current half edge is the most ccw in halfedge 
	}

	HalfEdge * he = m_halfedge->ccw_rotate_about_target();
	
	while( he != NULL )
	{
		m_halfedge = he;
		he = m_halfedge->ccw_rotate_about_target();
	}

	return m_halfedge;
}
	
HalfEdge *  Vertex::most_clw_in_halfedge()  
{ 
	if( !m_boundary )
	{
		return most_ccw_in_halfedge()->ccw_rotate_about_target(); 
	}

	HalfEdge * he = m_halfedge->clw_rotate_about_target();
	
	while( he != NULL )
	{
		m_halfedge = he;
		he = m_halfedge->clw_rotate_about_target();
	}

	return m_halfedge;
}

HalfEdge *  Vertex::most_ccw_out_halfedge()  
{ 
	if( !m_boundary )
	{
		return most_ccw_in_halfedge()->he_sym(); 
	}

	HalfEdge * he = m_halfedge->he_next();
	HalfEdge * ne = he->ccw_rotate_about_source();

	while( ne != NULL )
	{
		he = ne;
		ne = he->ccw_rotate_about_source();
	}

	return he;
}
	
HalfEdge *  Vertex::most_clw_out_halfedge()  
{ 
	if( !m_boundary )
	{
		return most_ccw_out_halfedge()->ccw_rotate_about_source(); 
	}

	HalfEdge * he = m_halfedge->he_next();
	HalfEdge * ne = he->clw_rotate_about_source();
	
	while( ne != NULL )
	{
		he = ne;
		ne = he->clw_rotate_about_source();
	}

	return he;
}

void Vertex::AddTrait(TraitNode *t)
{
	TraitList * tl = this->traits();
	if (!tl)
	{
		tl = new TraitList;
		this->traits() = tl;
	}
	tl->addTrait(t);
}

TraitNode * Vertex::GetTrait(int tind)
{
	TraitList * tl = this->traits();
	if (!tl)
		return NULL;
	else
		return tl->getTrait(tind);
}

bool Vertex::DelTrait(int tind)
{
	TraitList * tl = this->traits();
	if (!tl)
		return false;
	else
		return tl->delTrait(tind);
}