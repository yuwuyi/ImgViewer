#ifndef _XMESHLIB_CORE_TRAIT_H_
#define _XMESHLIB_CORE_TRAIT_H_

#pragma warning (disable : 4786)
#include <list>
#include <algorithm>

namespace XMeshLib
{
#define ID_AdjHeTrait						0000
#define ID_UVTrait							0001
#define ID_TheBoolTrait						0002
#define ID_TheDoubleTrait					0003

#define ID_CFKMTrait						1000

#define ID_DiLenEdgeTrait					1001
#define ID_DiLenFaceTrait					1002

#define ID_CCSEdgeTrait						1003
#define ID_CCSFaceTrait						1004
#define ID_CCSVerTrait						1005
#define ID_CCSHalfEdgeTrait					1006
#define ID_CCSUVTrait						1007

#define ID_DistFieldEdgeTrait				1008
#define ID_DistFieldVerTrait				1009
#define	ID_DFTriEdgeTrait					1010
#define ID_DFTriVerTrait					1011
//#define ID_MeshDFEdgeTrait				1012
#define ID_MeshDFVerTrait					1013


#define ID_G0CMapVerTrait					1016
#define ID_G0CMapEdgeTrait					1017
#define ID_G0CMapHETrait					1018

#define ID_SegmentFaceTrait					1020
#define ID_SegmentEdgeTrait					1021
#define ID_SegmentVerTrait					1022
#define ID_SegmentHETrait					1023
	
#define ID_VolumeMapTrait					1030

#define ID_GreedyHomotopyEdgeTrait			1040
#define ID_GreedyHomotopyVerTrait			1041
#define ID_SMapVerTrait						1046
#define ID_SMapFaceTrait					1047
#define ID_SMapEdgeTrait					1048
#define ID_ChartVertexTrait					1051	
#define ID_ChartFaceTrait					1052
#define ID_ChartEdgeTrait					1053
#define ID_GeodesicVertexTrait				1061
#define ID_GeodesicEdgeTrait				1062
#define ID_GeodesicFaceTrait				1063
#define ID_FlatMetricTilerVertexTrait		1071
#define ID_FlatMetricTilerEdgeTrait			1072
#define ID_FlatMetricTilerFaceTrait			1073
#define ID_FlatMetricTilerHalfEdgeTrait		1074
#define ID_HMetricTilerVertexTrait			1081
#define ID_HMetricTilerFaceTrait			1082
#define ID_HMetricTilerEdgeTrait			1083
#define ID_HMetricTilerHalfEdgeTrait		1084

#define ID_PantVertexTrait					1090

#define ID_MChartSegVerTrait				1100
#define ID_MChartSegFaceTrait				1101


#define ID_Map2Rect_ETrait					2001
#define ID_Map2Rect_VTrait					2002
#define ID_Map2Rect_HETrait					2003

	class TraitNode
	{
	public:
		TraitNode(){tId = -1;}
		virtual ~TraitNode(){tId = -1;}
		bool operator== (const TraitNode & t) const
		{
			int tid1 = t.tId;			
			if (tId == tid1)
				return true;
			else
				return false;			
		}
		int tId;
	};	

	class TraitList
	{
	public:
		TraitList(){;}
		~TraitList(){clear();}
		void clear()
		{
			for (std::list<TraitNode *>::iterator titer=tlist.begin(); titer!=tlist.end(); ++titer)
			{				
				TraitNode * tempTrait = * titer;				
				delete tempTrait;
			}
			tlist.clear();
		}
		void addTrait(TraitNode * t)
		{
			tlist.push_back(t);
		}
		bool delTrait(int id)
		{
			//TraitNode dummyTrait;
			//dummyTrait.tId = id;
			//std::list<TraitNode *>::iterator titer = std::find(tlist.begin(), tlist.end(), &dummyTrait);
			//if (titer!=tlist.end())
			//{
			//	TraitNode * tempTrait = *titer;				
			//	tlist.erase(titer);
			//	delete tempTrait;
			//	return true;
			//}
			//else
			//	return false;
			std::list<TraitNode *>::iterator titer;
			for (titer=tlist.begin(); titer!=tlist.end(); ++titer)
			{				
				TraitNode * tempTrait = * titer;
				if (id == tempTrait->tId)
				{
					tlist.erase(titer);
					delete tempTrait;
					return true;
				}
			}
			return false;
		}
		TraitNode * getTrait(int id)
		{
			std::list<TraitNode *>::iterator titer;
			for (titer=tlist.begin(); titer!=tlist.end(); ++titer)
			{				
				TraitNode * tempTrait = * titer;
				if (id == tempTrait->tId)
					return tempTrait;
			}
			return NULL;
		}
		std::list<TraitNode *> tlist;		
	};
}

#endif

