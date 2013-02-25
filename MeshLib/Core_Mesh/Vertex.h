#ifndef _XMESHLIB_CORE_VERTEX_H_
#define _XMESHLIB_CORE_VERTEX_H_

#include "HalfEdge.h"
#include "Point.h"
#include "Trait.h"
#include <assert.h>

namespace XMeshLib
{
	class Vertex;
	class HalfEdge;
	class TraitList;
	
	class Vertex
	{
	public:		
		Vertex()
		{ 
			m_halfedge = NULL; 
			m_boundary = false; 
			m_sharp = false;
			m_traits = NULL; //new TraitList; 
		}
		~Vertex()
		{
			if (m_traits)
			{
				m_traits->clear();
				delete m_traits;
			}
		}
		Point & point() { return  m_point; }
	//	Point & normal(){ return  m_normal;}
		HalfEdge * & halfedge(){ return m_halfedge; }
		TraitList * & traits() { return m_traits; }
		void AddTrait(TraitNode * t);
		TraitNode * GetTrait(int tind);
		bool DelTrait(int tind);
		int & id() { return m_id; }		
		const int id() const  { return m_id; }
		bool & boundary() { return m_boundary; }
		bool & sharp() {return m_sharp;}
		std::string & string() { return m_string;}
		bool operator== (const Vertex & v) const
		{
			return m_id == v.id();
		}
		bool operator< (const Vertex & v) const
		{
			return v.id() >= m_id; 
		}
		HalfEdge *  most_ccw_in_halfedge();
		HalfEdge *  most_ccw_out_halfedge();
		HalfEdge *  most_clw_in_halfedge();
		HalfEdge *  most_clw_out_halfedge();

	protected:
		friend class Mesh;

		int	m_id;		
		Point m_point;
	//	Point m_normal;
		HalfEdge * m_halfedge;
		bool m_boundary;
		bool m_sharp;
		std::string m_string;
		TraitList * m_traits;
	};

}//name space MeshLib

#endif //_XMESHLIB_CORE_VERTEX_H_ defined
