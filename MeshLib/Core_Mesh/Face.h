#ifndef _XMESHLIB_CORE_FACE_H_
#define _XMESHLIB_CORE_FACE_H_

#include "Trait.h"
#include <assert.h>

namespace XMeshLib
{
	class HalfEdge;
	class TraitList;
	class Face
	{
	public:
		Face(){ m_halfedge = NULL; m_traits=NULL; m_sharp = false;}
		~Face()
		{
			if (m_traits)
			{
				m_traits->clear();
				delete m_traits;			
			}
		}
		HalfEdge    * & halfedge() { return m_halfedge; };
		TraitList	* & traits()    { return m_traits;};
		int		      & id()       { return m_id; };
		const int       id() const { return m_id; };
		std::string &   string()   { return m_string;};
		bool & sharp() {return m_sharp;}
		bool operator== (const Face & f) const
		{
			return m_id == f.id();
		};
		bool operator< (const Face & f) const
		{
			return f.id() >= m_id; 
		};

		void AddTrait(TraitNode * t);
		TraitNode * GetTrait(int tind);
		bool DelTrait(int tind);

	private:
		int			  m_id;
		HalfEdge    * m_halfedge;
		std::string   m_string;   //string
		bool m_sharp;
		TraitList   * m_traits;
	};

}//name space XMeshLib

#endif //_XMESHLIB_CORE_FACE_H_ defined