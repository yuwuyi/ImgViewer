#ifndef _HALFEDGE_H_
#define _HALFEDGE_H_

#include "Edge.h"

class Vertex;
class Face;

class Halfedge
{
public:
	Halfedge(){ m_edge = NULL; m_vertex = NULL; m_prev = NULL; m_next = NULL; m_face = NULL;}
	~Halfedge()	{;}

	//Referenced elements
	Face     * & face()    { return m_face;}
	Edge     * & edge()    { return m_edge;}
	Vertex   * & target()  { return m_vertex;}	
	Halfedge * & prev() { return m_prev;}
	Halfedge * & next() { return m_next;}

	//Derived by halfedge data structure
	Halfedge * & twin()  { return m_edge->twin(this); }
	Vertex   * & source()  { return m_prev->target();}

	//Rotation operations
	Halfedge * clw_rotate_about_target() {return next()->twin();}
	Halfedge * ccw_rotate_about_source() {return prev()->twin();}
	Halfedge * clw_rotate_about_source() {
		Halfedge * he = twin();
		if (he) return he->next();
		else return NULL;
	}
	Halfedge * ccw_rotate_about_target() {
		Halfedge * he_dual = twin();
		if (he_dual) return he_dual->prev();
		else return NULL;
	}


protected:
	Edge     *     m_edge;
    Face     *     m_face;
	Vertex   *     m_vertex;		//target vertex
	Halfedge *	   m_prev;
	Halfedge *     m_next;
};

#endif 