#ifndef _XMESHLIB_CORE_EDGE_H_
#define _XMESHLIB_CORE_EDGE_H_

#include <assert.h>
#include "Trait.h"

namespace XMeshLib{

class HalfEdge;
class Vertex;
class TraitList;

class Edge
{
public:
	Edge()
		:m_sharp(false), m_traits(NULL)
	{
		m_halfedge[0]=NULL; 
		m_halfedge[1]=NULL;
		m_vertices[0] = -1;
		m_vertices[1] = -1;
	}

	Edge(int s, int e)
		:m_sharp(false), m_traits(NULL)
	{
		m_halfedge[0]=NULL; 
		m_halfedge[1]=NULL;
		m_vertices[0] = s;
		m_vertices[1] = e;
	}
	
	~Edge()
	{
		if (m_traits)
		{
			m_traits->clear();
			delete m_traits;
		}
	}
	HalfEdge * & halfedge( int i ) { assert( 0<=i && i < 2 ); return m_halfedge[i];}
	int & vid(int i) {assert(0<=i && i <2); return m_vertices[i]; }
	TraitList * & traits() { return m_traits;};
	bool boundary() { return (m_halfedge[0] && !(m_halfedge[1])) || (!(m_halfedge[0]) && m_halfedge[1] ); }
	bool & sharp() { return m_sharp; }
	std::string & string()  { return m_string;}
	HalfEdge * & other( HalfEdge * he ) { return (he != m_halfedge[0] )?m_halfedge[0]:m_halfedge[1]; }
	
	int otherVid(const int cvid)
	{
		return ((m_vertices[0]==cvid)?(m_vertices[1]):(m_vertices[0]));		
	}
	
	void SetSharp();
	void SetNotSharp();
	void AddTrait(TraitNode * t);
	TraitNode * GetTrait(int tind);
	bool DelTrait(int tind);
	
	
	bool operator== (const Edge & e) const
    {
		int rid0 = e.m_vertices[0];
		int rid1 = e.m_vertices[1];
		if (m_vertices[0] == rid0 && m_vertices[1] == rid1)
			return true;
		else if (m_vertices[1] == rid0 && m_vertices[0] == rid1)
			return true;
		return false;
    }

protected:	
	HalfEdge  * m_halfedge[2];
	int m_vertices[2];
	std::string m_string;
	bool m_sharp;
	TraitList * m_traits;
};



}//name space XMeshLib

#endif //_MESHLIB_EDGE_H_ defined