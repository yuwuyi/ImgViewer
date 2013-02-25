#ifndef _XMESHLIB_CORE_BASICTRAIT_H_
#define _XMESHLIB_CORE_BASICTRAIT_H_

#pragma warning (disable : 4786)
#pragma warning (disable : 4018)
#pragma warning (disable : 4267)

#include <list>
#include <algorithm>
#include "Mesh.h"
#include "HalfEdge.h"
#include "Point.h"
#include "Trait.h"

namespace XMeshLib
{
	class AdjHeTrait:public TraitNode
	{
	public:
		AdjHeTrait()
		{
			tId = ID_AdjHeTrait;
		}
		~AdjHeTrait()
		{
			;
		}		
		void AddHE(HalfEdge * he)
		{
			adjhelist.push_back(he);
		}
		int NumAdjHE()
		{
			return adjhelist.size();
		}
		HalfEdge * indAdjHe(int i)
		{
			if (i<0 || i>= adjhelist.size())
				return NULL;
			return adjhelist[i];
		}
	public:
		std::vector<HalfEdge *> adjhelist;
	};

	class UVTrait:public TraitNode
	{
	public:
		UVTrait(){tId = ID_UVTrait;}
		Point & uv(){return m_uv; }
		Point & conformal() {return m_conformal;}
		Point m_uv;
		Point m_conformal;
	};

	class TheBoolTrait:public TraitNode
	{
	public:
		TheBoolTrait(){tId = ID_TheBoolTrait;}
		bool & value()
		{
			return m_value;
		}
		bool m_value;
	};

	class TheDoubleTrait:public TraitNode
	{
	public:
		TheDoubleTrait(){tId = ID_TheDoubleTrait;}
		double & value()
		{
			return m_value;
		}
		double m_value;
	};
}

#endif

