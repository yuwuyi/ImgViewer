#ifndef _XMESHLIB_ITERATORS_H_
#define _XMESHLIB_ITERATORS_H_

#include "Mesh.h"
#include <assert.h>

namespace XMeshLib
{
	class VertexOutHalfedgeIterator
	{
	public:
		VertexOutHalfedgeIterator( Mesh *  solid, Mesh::tVertex  v )
		{ m_solid = solid; m_vertex = v; m_halfedge = solid->vertexMostClwOutHalfEdge(v); };

		~VertexOutHalfedgeIterator(){};
		void operator++()
		{
			assert( m_halfedge != NULL ); 
			if( m_halfedge == m_solid->vertexMostCcwOutHalfEdge(m_vertex) ) 
				m_halfedge = NULL;
			else
				m_halfedge = m_solid->vertexNextCcwOutHalfEdge(m_halfedge); 
		}
		Mesh::tHalfEdge value() { return m_halfedge; }
		bool end(){ return m_halfedge == NULL; }
		Mesh::tHalfEdge operator*() { return value(); }
		//void reset() { m_solid->vertexMostClwOutHalfEdge(m_vertex); }

	private:
		Mesh *    m_solid;
		Mesh::tVertex   m_vertex;
		Mesh::tHalfEdge m_halfedge;		
	};

	class VertexInHalfedgeIterator
	{
	public:
		VertexInHalfedgeIterator( Mesh *  solid, Mesh::tVertex v )
		{ 
			m_solid = solid; m_vertex = v; m_halfedge = solid->vertexMostClwInHalfEdge(v); 
		}
		~VertexInHalfedgeIterator(){;}
		void operator++()
		{
			assert( m_halfedge != NULL ); 
			if( m_halfedge == m_solid->vertexMostCcwInHalfEdge(m_vertex) ) 
				m_halfedge = NULL; 
			else
				m_halfedge = m_solid->vertexNextCcwInHalfEdge(m_halfedge); 
		}
		Mesh::tHalfEdge value() { return m_halfedge; }
		bool end(){ return m_halfedge == NULL; }
		Mesh::tHalfEdge operator*() { return value(); }

	private:
		Mesh *    m_solid;
		Mesh::tVertex   m_vertex;
		Mesh::tHalfEdge m_halfedge;
	};

	class VertexVertexIterator
	{
	public:
		VertexVertexIterator( Mesh::tVertex  v )
		{ 
			m_vertex = v; 
			m_halfedge = m_vertex->most_clw_out_halfedge();
		}
		~VertexVertexIterator(){};
		void operator++()
		{
			assert( m_halfedge != NULL ); 
			if( !m_vertex->boundary() )
			{
				if( m_halfedge != m_vertex->most_ccw_out_halfedge() )
				{
					m_halfedge = m_halfedge->ccw_rotate_about_source();
				}
				else
				{
					m_halfedge = NULL;
				}
				return;
			}
			if( m_vertex->boundary() )
			{
				if( m_halfedge == m_vertex->most_ccw_in_halfedge() )
				{
					m_halfedge = NULL;
					return;
				}
				HalfEdge * he = m_halfedge->ccw_rotate_about_source();
				if( he == NULL )
				{
					m_halfedge = m_vertex->most_ccw_in_halfedge();
				}
				else
				{
					m_halfedge = he;
				}
			}
			return;
		}
		Mesh::tVertex value() 
		{ 
			if( !m_vertex->boundary() )
			{
				return m_halfedge->target(); 
			}
			if( m_halfedge != m_vertex->most_ccw_in_halfedge() )
			{
				return m_halfedge->target();
			}
			if( m_halfedge == m_vertex->most_ccw_in_halfedge() )
			{
				return m_halfedge->source();
			}
			return NULL;
		}
		
		Mesh::tVertex operator*() { return value(); }
		bool end(){ return m_halfedge == NULL; }
		void reset()	{ m_halfedge = m_vertex->most_clw_out_halfedge(); }
	private:
		Mesh::tVertex   m_vertex;
		Mesh::tHalfEdge m_halfedge;
	};
	
	class VertexEdgeIterator
	{
	public:
		VertexEdgeIterator( Mesh::tVertex  v )
		{ 
			m_vertex = v; 
			m_halfedge = m_vertex->most_clw_out_halfedge();
		};
		~VertexEdgeIterator(){};
		void operator++()
		{
			assert( m_halfedge != NULL ); 
			
			if( !m_vertex->boundary() )
			{
				if( m_halfedge != m_vertex->most_ccw_out_halfedge() )
				{
					m_halfedge = m_halfedge->ccw_rotate_about_source();
				}
				else
				{
					m_halfedge = NULL;
				}
				return;
			}

			if( m_vertex->boundary() )
			{
				if( m_halfedge == m_vertex->most_ccw_in_halfedge() )
				{
					m_halfedge = NULL;
					return;
				}

				HalfEdge * he = m_halfedge->ccw_rotate_about_source();

				if( he == NULL )
				{
					m_halfedge = m_vertex->most_ccw_in_halfedge();
				}
				else
				{
					m_halfedge = he;
				}
			}

			return;
		}
		Mesh::tEdge value() 
		{ 
			assert( m_halfedge != NULL );
			return m_halfedge->edge();
		}

		Mesh::tEdge operator*() { return value(); }

		bool end(){ return m_halfedge == NULL; }
		void reset()	{ m_halfedge = m_vertex->most_clw_out_halfedge(); }
	private:
		Mesh::tVertex   m_vertex;
		Mesh::tHalfEdge m_halfedge;
	};

	class VertexFaceIterator
	{
	public:
		VertexFaceIterator( Mesh::tVertex v )
		{ 
			m_vertex = v; 
			m_halfedge = m_vertex->most_clw_out_halfedge(); 
		};

		~VertexFaceIterator(){};

		void operator++()
		{
			assert( m_halfedge != NULL );  

			if( m_halfedge == m_vertex->most_ccw_out_halfedge() ) 
			{
				m_halfedge = NULL;
				return;
			}
			m_halfedge = m_halfedge->ccw_rotate_about_source();
		};

		Mesh::tFace value() { return m_halfedge->face(); };
		Mesh::tFace operator*() { return value(); };
		bool end(){ return m_halfedge == NULL; };
		void reset()	{ m_halfedge = m_vertex->most_clw_out_halfedge(); };

	private:
		Mesh::tVertex   m_vertex;
		Mesh::tHalfEdge m_halfedge;
	};

	// f -> halfedge
	class FaceHalfedgeIterator
	{
	public:

		FaceHalfedgeIterator( Mesh::tFace f )
		{ 
			m_face = f; 
			m_halfedge = f->halfedge(); 
		};

		~FaceHalfedgeIterator(){};

		void operator++()
		{
			assert( m_halfedge != NULL );
			m_halfedge = m_halfedge->he_next();

			if( m_halfedge == m_face->halfedge() )
			{
				 m_halfedge = NULL;
				return;
			};
		}

		Mesh::tHalfEdge value() { return m_halfedge; };
		Mesh::tHalfEdge operator*() { return value(); };

		bool end(){ return m_halfedge == NULL; };

	private:
		Mesh::tFace     m_face;
		Mesh::tHalfEdge m_halfedge;
	};


	// f -> edge
	class FaceEdgeIterator
	{
	public:

		FaceEdgeIterator( Mesh::tFace f )
		{ 
			m_face = f; 
			m_halfedge = f->halfedge(); 
		};

		~FaceEdgeIterator(){};

		void operator++()
		{
			assert( m_halfedge != NULL );
			m_halfedge = m_halfedge->he_next();

			if( m_halfedge == m_face->halfedge() )
			{
				 m_halfedge = NULL;
				return;
			};
		}

		Mesh::tEdge value() { return m_halfedge->edge(); };
		Mesh::tEdge operator*() { return value(); };

		bool end(){ return m_halfedge == NULL; };

	private:
		Mesh::tFace     m_face;
		Mesh::tHalfEdge m_halfedge;
	};


	// f -> vertex
	class FaceVertexIterator
	{
	public:

		FaceVertexIterator( Mesh::tFace f )
		{ 
			m_face = f; 
			m_halfedge = f->halfedge(); 
		};

		~FaceVertexIterator(){};

		void operator++()
		{
			assert( m_halfedge != NULL );
			m_halfedge = m_halfedge->he_next();

			if( m_halfedge == m_face->halfedge() )
			{
				 m_halfedge = NULL;
				return;
			};
		}

		Mesh::tVertex value() { return m_halfedge->target(); };
		Mesh::tVertex operator*() { return value(); };

		bool end(){ return m_halfedge == NULL; };

	private:
		Mesh::tFace     m_face;
		Mesh::tHalfEdge m_halfedge;
	};


	// soild vertices
	class MeshVertexIterator
	{
	public:
		MeshVertexIterator(const Mesh * solid ):
		  m_solid(solid), ind(0)
		{
			//m_iter = (m_solid->m_verts)[ind];			
		}

		Mesh::tVertex value() { return m_solid->m_verts.at(ind); }

		void operator++() 
		{ 
			++ind; 
		}

		void operator++(int) 
		{ 
			++ind; 
		}
		
		bool end() { return ind == m_solid->m_verts.size(); }

		Mesh::tVertex operator*(){ return value(); }

		void reset() { ind = 0; }

	private:
		int ind;
		const Mesh * m_solid;
	};

	// soild faces
	class MeshFaceIterator
	{
	public:
		MeshFaceIterator(const Mesh * solid ):
		  m_solid(solid), ind(0)
		{
			//m_solid = solid;
			//ind = 0;
			//m_iter = m_solid->m_faces[ind];			
		}

		Mesh::tFace value() { return m_solid->m_faces[ind]; }

		void operator++() { ++ind;}

		bool end() { return ind == m_solid->m_faces.size(); }

		Mesh::tFace operator*(){ return value(); }

		void reset() { ind = 0; }//m_iter = (m_solid->m_faces)[ind];}

	private:
		int ind;
		const Mesh * m_solid;
		//Mesh::tFace m_iter;
	};

	// soild edges
	class MeshEdgeIterator
	{
	public:
		MeshEdgeIterator(Mesh * solid )
		{
			m_solid = solid;
			ind = 0;
			m_iter = m_solid->m_edges[ind];			
		}

		Mesh::tEdge value() {  m_iter = m_solid->m_edges.at(ind); return m_iter; };

		void operator++() { ++ind;}

		bool end() { return ind == m_solid->m_edges.size(); }

		Mesh::tEdge operator*(){ return value(); }

		void reset() { ind = 0; m_iter = (m_solid->m_edges)[ind];}

	private:
		int ind;
		Mesh * m_solid;
		Mesh::tEdge m_iter;
	};

	// soild halfedges
	class MeshHalfEdgeIterator
	{
	public:
		MeshHalfEdgeIterator( Mesh * solid )
		{
			m_solid = solid;
			ind = 0;
			m_id = 0;
			m_iter = m_solid->m_edges[ind];
		}

		Mesh::tHalfEdge value() { 
			m_iter = m_solid->m_edges.at(ind);
			Mesh::tEdge e = m_iter; 
			return e->halfedge(m_id); 
		};

		void operator++() 
		{ 
			++m_id;

			switch( m_id )
			{
			case 1:
				{
					Mesh::tEdge e = m_iter;
					if( e->halfedge(m_id) == NULL )
					{
						m_id = 0;
						ind++;						
					}
				}
				break;
			case 2:
				m_id = 0;
				ind++;				
				break;
			}
		};

		bool end() { return (ind == m_solid->m_edges.size()); }

		Mesh::tHalfEdge operator*(){ return value(); };

		void reset() { ind = 0; m_id = 0; m_iter = (m_solid->m_edges)[ind];};

	private:
		int ind;
		Mesh * m_solid;
		Mesh::tEdge m_iter;
		int  m_id;
	};
}

#endif
