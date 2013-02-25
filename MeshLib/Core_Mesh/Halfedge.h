#ifndef _XMESHLIB_CORE_HALFEDGE_H_
#define _XMESHLIB_CORE_HALFEDGE_H_

#include "Edge.h"
#include "Trait.h"
#include <assert.h>

namespace XMeshLib
{
	class Vertex;
	class Edge;
	class Face;
	class TraitList;

	class HalfEdge
	{
	public:
		HalfEdge(){ m_edge = NULL; m_vertex = NULL; m_prev = NULL; m_next = NULL; m_face = NULL; m_traits = NULL;}
		~HalfEdge()
		{
			if (m_traits)
			{
				m_traits->clear();
				delete m_traits;
			}
		}
		Edge     * & edge()    { return m_edge;   }
		Vertex   * & vertex()  { return m_vertex; }
		Vertex   * & target()  { return m_vertex; }
		Vertex   * & source()  { return m_prev->vertex();}
		HalfEdge * & he_prev() { return m_prev;}
		HalfEdge * & he_next() { return m_next;}
		HalfEdge * & he_sym()  { return m_edge->other(this); }
		Face     * & face()    { return m_face;}
		TraitList * & traits()  { return m_traits;}
		std::string & string() { return m_string; }
		HalfEdge * ccw_rotate_about_target();
		HalfEdge * clw_rotate_about_target();
		HalfEdge * ccw_rotate_about_source();
		HalfEdge * clw_rotate_about_source();
		void AddTrait(TraitNode * t);
		TraitNode * GetTrait(int tind);
		bool DelTrait(int tind);

	private:
		Edge     *     m_edge;
		Face     *     m_face;
		Vertex   *     m_vertex;		//target vertex
		HalfEdge *	   m_prev;
		HalfEdge *     m_next;
		std::string    m_string;
		TraitList *	   m_traits;
	};


}//name space XMeshLib

#endif //_XMESHLIB_CORE_HALFEDGE_H_ defined