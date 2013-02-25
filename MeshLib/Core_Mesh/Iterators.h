#ifndef _ITERATORS_H_
#define _ITERATORS_H_

#include "Vertex.h"
#include "Mesh.h"

//// A list of iterator classes for usage
//////MeshVertexIterator
//////MeshFaceIterator
//////MeshEdgeIterator
//////MeshHalfedgeIterator
//////FaceVertexIterator
//////FaceHalfedgeIterator
//////FaceEdgeIterator
//////VertexVertexIterator
//////VertexEdgeIterator
//////VertexOutHalfedgeIterator
//////VertexInHalfedgeIterator


//// MeshIndex: Indexing all elements following their order in the mesh iterators

// All vertices
class MeshVertexIterator
{
public:
	MeshVertexIterator(Mesh * cmesh ):m_Mesh(cmesh){ m_iter = m_Mesh->m_verts.begin(); }
	Vertex * value() { return *m_iter;}
	void operator++() { ++m_iter; }
	bool end() { return m_iter == m_Mesh->m_verts.end();}
	Vertex * operator*(){ return value(); }
	void reset() { m_iter = m_Mesh->m_verts.begin(); }
private:
	std::list<Vertex *>::iterator m_iter;
	Mesh * m_Mesh;
};

// All faces
class MeshFaceIterator
{
public:
	MeshFaceIterator(Mesh * cmesh ):m_Mesh(cmesh){ m_iter = m_Mesh->m_faces.begin(); }
	Face * value() { return *m_iter; }
	void operator++() { ++m_iter;}
	bool end() { return m_iter == m_Mesh->m_faces.end(); }
	Face * operator*(){ return value(); }
	void reset() { m_iter = m_Mesh->m_faces.begin();}
private:	
	Mesh * m_Mesh;
	std::list<Face *>::iterator m_iter;
};

// All edges
class MeshEdgeIterator
{
public:
	MeshEdgeIterator(Mesh * cmesh ):m_Mesh(cmesh){m_iter = m_Mesh->m_edges.begin();}
	Edge * value() {  return *m_iter; };
	void operator++() { ++m_iter;}
	bool end() { return m_iter == m_Mesh->m_edges.end(); }
	Edge * operator*(){ return value(); }
	void reset() { m_iter = m_Mesh->m_edges.begin();}
private:		
	Mesh * m_Mesh;
	std::list<Edge *>::iterator m_iter;
};

// All halfedges
class MeshHalfedgeIterator
{
public:
	MeshHalfedgeIterator( Mesh * cmesh ):m_Mesh(cmesh){ m_id = 0; m_iter = m_Mesh->m_edges.begin(); }
	Halfedge * value() {
		Edge * e = *m_iter; 
		return e->he(m_id); 
	};
	void operator++() 
	{ 
		++m_id;
		if (m_id==1){
			Edge * e = *m_iter;
			if( !e->he(m_id) ){
				m_id = 0;
				++m_iter;}
		}
		else {//m_id==2
			m_id = 0;
			++m_iter;
		}	
	};
	bool end() { return (m_iter == m_Mesh->m_edges.end()) ;}
	Halfedge * operator*(){ return value(); };
	void reset() { m_id = 0; m_iter = m_Mesh->m_edges.begin();};
private:		
	Mesh * m_Mesh;
	std::list<Edge *>::iterator m_iter;
	int  m_id;
};


// f -> vertex
class FaceVertexIterator
{
public:

	FaceVertexIterator( Face * f ){ m_face = f; m_halfedge = f->he(); }
	~FaceVertexIterator(){;}
	void operator++()	{
		m_halfedge = m_halfedge->next();
		if( m_halfedge == m_face->he() )
			m_halfedge = NULL;
	}
	Vertex * value() { return m_halfedge->target(); }
	Vertex * operator*() { return value(); };
	bool end(){ return (!m_halfedge); };
private:
	Face *     m_face;
	Halfedge * m_halfedge;
};


// f -> halfedge
class FaceHalfedgeIterator
{
public:
	FaceHalfedgeIterator( Face * f ){ m_face = f; m_halfedge = f->he(); }
	~FaceHalfedgeIterator(){;}
	void operator++(){
		m_halfedge = m_halfedge->next();
		if( m_halfedge == m_face->he() )
			m_halfedge = NULL;
	}
	Halfedge * value() { return m_halfedge; };
	Halfedge * operator*() { return value(); };
	bool end(){ return (!m_halfedge); };
private:
	Face *     m_face;
	Halfedge * m_halfedge;
};


// f -> edge
class FaceEdgeIterator
{
public:
	FaceEdgeIterator( Face * f ){ m_face = f; m_halfedge = f->he(); }
	~FaceEdgeIterator(){;}

	void operator++(){
		m_halfedge = m_halfedge->next();
		if( m_halfedge == m_face->he() )
			m_halfedge = NULL;
	}

	Edge * value() { return m_halfedge->edge(); };
	Edge * operator*() { return value(); };
	bool end(){ return (!m_halfedge); };
private:
	Face *     m_face;
	Halfedge * m_halfedge;
};


class VertexVertexIterator
{
public:
	VertexVertexIterator( Vertex *  v ){ 
		m_vertex = v; 
		m_halfedge = m_vertex->most_clw_out_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_source(); 
		else 
			end_he= m_vertex->most_ccw_in_halfedge();
	}
	~VertexVertexIterator(){;}
	void operator++(){
		if (m_halfedge == end_he )
		{
			m_halfedge = NULL;
			return;
		}
		m_halfedge = m_halfedge->ccw_rotate_about_source();
		if( m_halfedge == NULL )
			m_halfedge = end_he; 
	}
	Vertex * value() 
	{ 
		if( m_vertex->boundary() && m_halfedge == end_he )
			return end_he->source();
		else
			return m_halfedge->target(); 
	}
	Vertex * operator*() { return value(); }
	bool end(){ return !m_halfedge; }
	void reset() { 
		m_halfedge = m_vertex->most_clw_out_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_source(); 
		else 
			end_he= m_vertex->most_ccw_in_halfedge();
	}
private:
	Vertex	 * m_vertex;
	Halfedge * m_halfedge;
	Halfedge * end_he;
};

class VertexEdgeIterator
{
public:
	VertexEdgeIterator( Vertex *  v ){ 
		m_vertex = v; 
		m_halfedge = m_vertex->most_clw_out_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_source(); 
		else 
			end_he= m_vertex->most_ccw_in_halfedge();
	}
	~VertexEdgeIterator(){;}
	void operator++()
	{
		if (m_halfedge == end_he )
		{
			m_halfedge = NULL;
			return;
		}
		m_halfedge = m_halfedge->ccw_rotate_about_source();
		if( m_halfedge == NULL )
			m_halfedge = end_he; 
	}
	Edge * value() 
	{ 
		return m_halfedge->edge();
	}

	Edge * operator*() { return value(); }

	bool end(){ return (!m_halfedge); }
	void reset()	{ 
		m_halfedge = m_vertex->most_clw_out_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_source(); 
		else 
			end_he= m_vertex->most_ccw_in_halfedge();	
	}
private:
	Vertex	 * m_vertex;
	Halfedge * m_halfedge;
	Halfedge * end_he;

};

class VertexFaceIterator
{
public:
	VertexFaceIterator( Vertex * v )
	{ 
		m_vertex = v; 
		m_halfedge = m_vertex->most_clw_out_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_source(); 
		else 
			end_he= m_vertex->most_ccw_out_halfedge();
	}
	~VertexFaceIterator(){;}
	void operator++(){
		if (m_halfedge == end_he )
		{
			m_halfedge = NULL;
			return;
		}
		m_halfedge = m_halfedge->ccw_rotate_about_source();
	}
	Face * value() { return m_halfedge->face(); };
	Face * operator*() { return value(); };
	bool end(){ return (!m_halfedge); };
	void reset()	{ 
		m_halfedge = m_vertex->most_clw_out_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_source(); 
		else 
			end_he= m_vertex->most_ccw_out_halfedge(); }
private:
	Vertex	 * m_vertex;
	Halfedge * m_halfedge;
	Halfedge * end_he;
};

class VertexOutHalfedgeIterator
{
public:
	VertexOutHalfedgeIterator(Vertex * v ){ 
		m_vertex = v; 
		m_halfedge = m_vertex->most_clw_out_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_source(); 
		else 
			end_he= m_vertex->most_ccw_out_halfedge();
	}
	~VertexOutHalfedgeIterator(){;}
	void operator++(){
		if (m_halfedge == end_he )
			m_halfedge = NULL;
		else
			m_halfedge = m_halfedge->ccw_rotate_about_source();
	}
	Halfedge * value() { return m_halfedge; }
	bool end(){ return (!m_halfedge); }
	Halfedge * operator*() { return value(); }
	void reset()	{ 
		m_halfedge = m_vertex->most_clw_out_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_source(); 
		else 
			end_he= m_vertex->most_ccw_out_halfedge(); }
private:
	Vertex *   m_vertex;
	Halfedge * m_halfedge;	
	Halfedge * end_he;
};

class VertexInHalfedgeIterator
{
public:
	VertexInHalfedgeIterator(Vertex * v ){ 
		m_vertex = v; 
		m_halfedge = m_vertex->most_clw_in_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_target(); 
		else 
			end_he= m_vertex->most_ccw_in_halfedge();
	}
	~VertexInHalfedgeIterator(){;}
	void operator++()	{
		if (m_halfedge == end_he )
			m_halfedge = NULL;
		else
			m_halfedge = m_halfedge->ccw_rotate_about_target();
	}
	Halfedge * value() { return m_halfedge; }
	bool end(){ return (!m_halfedge); }
	Halfedge * operator*() { return value(); }
	void reset()	{ 
		m_halfedge = m_vertex->most_clw_in_halfedge();
		if (!m_vertex->boundary())
			end_he= m_halfedge->clw_rotate_about_target(); 
		else 
			end_he= m_vertex->most_ccw_in_halfedge(); }
private:
	Vertex *   m_vertex;
	Halfedge * m_halfedge;
	Halfedge * end_he;
};

#include <hash_map>
#include <iostream>

class MeshIndex
{
public:
	MeshIndex(Mesh * cmesh):m_mesh(cmesh){vi=fi=ei=hei=0;}
	~MeshIndex(){;}
	void setVIndex(){
		std::cout << "Indexing mesh vertices ...";
		vindex.clear();
		vi=0;
		for (MeshVertexIterator it(m_mesh); !it.end();++it)	{
			Vertex * v = *it;
			vindex[v]=vi++;
		}
		//vindex.rehash(vi);
		std::cout << "Done\n";
	}
	void removeVIndex(){ vindex.clear(); vi=0;}
	void setFIndex(){
		std::cout << "Indexing mesh faces ...";
		findex.clear();
		fi=0;
		for (MeshFaceIterator it(m_mesh); !it.end();++it)	{
			Face * f = *it;
			findex[f]=fi++;
		}		
		//findex.rehash(fi);
		std::cout<< "Done\n";
	}
	void removeFIndex(){ findex.clear(); fi=0;}
	void setHEIndex(){
		std::cout << "Indexing mesh halfedges...";
		heindex.clear();
		hei=0;
		for (MeshHalfedgeIterator it(m_mesh); !it.end();++it)	{
			Halfedge * he = *it;
			heindex[he]=hei++;
		}
		//heindex.rehash(hei);
		std::cout<<"Done\n";
	}
	void removeHEIndex(){ heindex.clear(); hei=0;}
	void setEIndex(){
		std::cout<< "Indexing mesh edges";
		eindex.clear();
		ei=0;
		for (MeshEdgeIterator it(m_mesh); !it.end();++it)	{
			Edge * e = *it;
			eindex[e]=ei++;
		}
		//eindex.rehash(ei);
		std::cout<<"Done\n";
	}
	void removeEIndex(){ eindex.clear(); ei=0;}
	stdext::hash_map<Vertex *, int> vindex;
	stdext::hash_map<Face *, int> findex;
	stdext::hash_map<Edge *, int> eindex;
	stdext::hash_map<Halfedge *, int> heindex;	
	Mesh * m_mesh;
	int vi, fi, ei, hei;

};

#endif
