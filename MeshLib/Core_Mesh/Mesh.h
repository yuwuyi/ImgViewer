#ifndef _XMESHLIB_CORE_MESH_H_
#define _XMESHLIB_CORE_MESH_H_

#pragma warning (disable : 4786)
#include "Edge.h"
#include "Face.h"
#include "HalfEdge.h"
#include "Vertex.h"
#include "Point.h"
#include "Trait.h"
#include "Attributes.h"
#include <hash_map>
//#include <map>

namespace XMeshLib{

	class Vertex;
	class HalfEdge;
	class Edge;
	class Face;

	class Mesh// : public Data
	{
	public:
		typedef Vertex   *	tVertex;
		typedef HalfEdge *	tHalfEdge;
		typedef Edge     *	tEdge;
		typedef Face     *	tFace;
		typedef Mesh    *	tMesh;

		Mesh()
		{		
			aux = false;			
			maxid = -1;
			maxfid = -1;
		}
		~Mesh();

		int numVertices() const;
		int numEdges() const ;
		int numFaces() const;
		
		void copyTo( Mesh & solid );
		bool read( const char inFile[]);
		bool write( const char outFile[]);

		bool isBoundary( tVertex  v ) const;
		bool isBoundary( tEdge    e ) const;
		bool isBoundary( tHalfEdge  he ) const;
		
		bool hasNormal() {return normals.valid();}
		bool hasUV() {return uv.valid();}

		//check whether the given vertex is on current mesh
		bool hasVertex(Vertex * ver) 
		{
			return (idVertex(ver->id())==ver);
		}

		bool hasHalfEdge(HalfEdge * he)
		{
			return (idVertex(he->vertex()->id())==he->vertex());
		}

		Point & verNormal(tVertex v){return normals[id2VInd[v->id()]];}
		Point & verUV(tVertex v);


		int				faceId( tFace  f ) const;
		int				vertexId( tVertex  v ) const;
		tEdge			idEdge( int id0, int id1 );
		tEdge			vertexEdge( tVertex v0, tVertex v1 );
		tFace			edgeFace1( tEdge  e );
		tFace			edgeFace2( tEdge  e );
		tFace			halfedgeFace( tHalfEdge he );
		tFace			idFace(const int id );
		tHalfEdge		corner( tVertex v, tFace f);
		tHalfEdge		idHalfedge( int id0, int id1 );
		tHalfEdge		vertexHalfedge( tVertex v0, tVertex v1 );
		tVertex			edgeVertex1( tEdge  e );
		tVertex			edgeVertex2( tEdge  e );
		tVertex			halfedgeVertex( tHalfEdge he );		
		tVertex			idVertex( int id );

		/*
			Flip the input edge, and return it.
			if the flip works, return the resultant edge, otherwise return NULL
		*/
		tEdge			edgeFlip(tEdge e);

		// Split the given edge(input) into two. 
		//	  return :  newly created vertex or NULL(when the split fails);
		tVertex			EdgeSplit(tEdge e);

		tVertex			faceSplit(tFace f, double bary[3]);

		/*
			aux :  represent the linking status of the current solid
				=0  --> nothing has been created, the only thing we have is vertex, edges, faces lists
				=1  --> aux traits has been created, from each vertex we can track all halfedges around it
				=2  --> aux traits has been removed, but the half-edge structure has been created
		*/
		int aux;
		void setupAux();
		void resetAux();
		void removeAux();

		tHalfEdge vertexMostClwOutHalfEdge( tVertex  v );
		tHalfEdge vertexNextCcwOutHalfEdge( tHalfEdge  he );
		tHalfEdge vertexMostCcwOutHalfEdge( tVertex  v );
		tHalfEdge vertexNextClwOutHalfEdge( tHalfEdge  he );
		tHalfEdge vertexMostClwInHalfEdge( tVertex  v );
		tHalfEdge vertexNextCcwInHalfEdge( tHalfEdge  he );
		tHalfEdge vertexMostCcwInHalfEdge( tVertex  v );
		tHalfEdge vertexNextClwInHalfEdge( tHalfEdge  he );
		tHalfEdge faceMostClwHalfEdge( tFace  f );
		tHalfEdge faceMostCcwHalfEdge( tFace  f );
		tHalfEdge faceNextCcwHalfEdge( tHalfEdge  he );
		tHalfEdge faceNextClwHalfEdge( tHalfEdge  he );		

	public:
		int maxid;
		int maxfid;
		Attributes<Point> normals;
		Attributes<Point> uv;
		int sid(int vid);
		int sid2vid(int sid);
		Vertex * sidVertex(int sid);

		stdext::hash_map<int, int>		id2VInd;
		stdext::hash_map<int, int>		id2FInd;
		//std::map<int, int>			id2VInd;		
		//std::map<int, int>			id2FInd;		

	protected:
		std::vector<Edge *>				m_edges;
		std::vector<Vertex *>			m_verts;	
		std::vector<Face *>				m_faces;		
		
		
	protected:
		friend class MeshVertexIterator;
		friend class MeshEdgeIterator;
		friend class MeshFaceIterator;
		friend class MeshHalfEdgeIterator;
		friend class MeshDelegate;


		tVertex createVertex(int id);
		tEdge createEdge(tVertex start, tVertex end);
		tFace createFace(int id);
		tFace createFace(int v[], int id);
		tFace createFace(Vertex * v[], int id);
	};

}//name space XMeshLib

#endif //_XMESHLIB_CORE_SOLID_H_ defined
