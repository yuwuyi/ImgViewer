#ifndef _XMESHLIB_CORE_ATTRIBUTES_H_
#define _XMESHLIB_CORE_ATTRIBUTES_H_

#include <assert.h>
#include <vector>

//#pragma warning (disable : 4786)

namespace XMeshLib
{
	//enum AttributePrimitiveType {VER, HALFEDGE, EDGE, FACE};

	//template <class T, enum AttributePrimitiveType PType> class Attributes
	template <class T> class Attributes
	{
	public:
		Attributes()
		{
			//pType = PType;
		}

		//Attributes()
		//{
		//	pType = VER;
		//}

		~Attributes()
		{;}

		bool valid()
		{
			return (data.size())?true:false;
		}

		void clear()
		{
			data.clear();
		}

		void add(T & t)
		{
			data.push_back(t);
		}

		T & operator [] (int ind)
		{
			//assert(ind >= 0 && ind < (signed int)data.size());
			return data[ind];
		}
		
	private:
		//enum AttributePrimitiveType pType;		
		std::vector<T> data;
	};
}

#endif