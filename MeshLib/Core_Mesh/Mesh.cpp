#include "Mesh.h"
#include <algorithm>
#include "Trait.h"
#include "BasicTraits.h"
#include "iterators.h"
#include <iostream>
#include <sstream>
#include <string>

#pragma warning (disable : 4786)
#pragma warning (disable : 4996)
#pragma warning (disable : 4267)

using namespace XMeshLib;

Mesh::~Mesh()
{	
	std::vector<Face *>::iterator fiter = this->m_faces.begin();
	for (; fiter!=m_faces.end(); ++fiter)
	{
		Face * f = *fiter;
		//HalfEdge * he = f->halfedge();
		//for (int i=0;i<3;++i)
		//{
		//	HalfEdge * nhe = he->he_next();
		//	delete he;
		//	he = nhe;
		//}
		delete f;
	}
	std::vector<Edge *>::iterator eiter = this->m_edges.begin();
	for (; eiter!=m_edges.end(); ++eiter)
	{
		Edge * e = *eiter;
		HalfEdge * he1 = e->halfedge(0);
		HalfEdge * he2 = e->halfedge(1);
		
		delete he1;
		if (he2)
			delete he2;
		delete e;
	}
	std::vector<Vertex *>::iterator viter = this->m_verts.begin();
	for (; viter!=m_verts.end(); ++viter)
	{
		Vertex * v = *viter;
		delete v;
	}	
}

int Mesh::numVertices() const
{
	return m_verts.size();
}

int Mesh::numEdges() const
{
	return m_edges.size();
}

int Mesh::numFaces() const
{
	return m_faces.size();
}

bool Mesh::read( const char inputFile[])
{
	int i;
	FILE * fp = fopen( inputFile, "r" );
	if( !fp )
	{
		printf("Can't open file %s\n", inputFile );
		return false;
	}

	char line[1024];
	int id;
	this->maxid=-1;
	this->maxfid = -1;

	m_verts.clear();
	m_edges.clear();
	m_faces.clear();

	while( !feof(fp)  )
	{	
		fgets( line, 1024, fp );
		if( !strlen( line ) )
			continue;

		char * str = strtok( line, " \r\n");
		if (!str)
			continue;
		if( !strcmp(str, "Vertex" ) ) 
		{
//			m_allV = 0;
			str = strtok(NULL," \r\n{");
			id = atoi( str );
			Point p;
			for( int i = 0 ; i < 3; i ++ )
			{
				str = strtok(NULL," \r\n{");
				p[i] = atof( str );
			}		
			tVertex v  = createVertex( id );
			v->point() = p;
			v->id()    = id;

			str = strtok( NULL, "\r\n");

			if(!str) 
				continue;

			std::string s(str);
			int sp = s.find("{");
			int ep = s.find("}");

			if( sp >= 0 && ep >= 0 )
			{
				v->string() = s.substr( sp+1, ep-sp-1 );
			}
			continue;
		}
		
		if( !strcmp(str,"Face") ) 
		{
			assert(aux !=2);
			if (!aux)
			{
				setupAux();
			}
			str = strtok(NULL, " \r\n");
			if (!str)
				continue;
			id = atoi( str );
			//if (this->maxfid < id) //this is automatically done in createface
			//	this->maxfid = id;
			int vid[3];
			for( int i = 0; i < 3; i ++ )
			{
				str = strtok(NULL," \r\n{");
				vid[i] = atoi(str);
			}
			tFace f = createFace( vid, id );
			if (!str)
				continue;
			str = strtok( NULL, "\r\n");
			if( !str || strlen( str ) == 0 ) continue;
			std::string s(str);
			int sp = s.find("{");
			int ep = s.find("}");
			if( sp >= 0 && ep >= 0 )
			{
				f->string() = s.substr( sp+1, ep-sp-1 );
			}
			continue;
		}

		//read in edge attributes
		if( !strcmp(str,"Edge") )
		{
			if (aux!=2)
				removeAux();
			str = strtok(NULL, " \r\n");
			if (!str)
				continue;
			int id0 = atoi( str );
			str = strtok(NULL, " \r\n");
			int id1 = atoi( str );
			tEdge edge = idEdge( id0, id1 );
			if (!edge)
				continue;
			str = strtok(NULL, "\r\n");
			if (!str)
				continue;
			std::string s(str);
			int sp = s.find("{");
			int ep = s.find("}");
			if( sp >= 0 && ep >= 0 )
			{
				edge->string() = s.substr( sp+1, ep-sp-1 );
			}
			continue;
		}
	}
	
	//Label boundary edges
	for( i=0; i<m_edges.size(); ++i)
	{		
		Mesh::tEdge edge = m_edges[i];
		Mesh::tHalfEdge he[2];

		he[0] = edge->halfedge(0);
		he[1] = edge->halfedge(1);
		
		assert( he[0] );
		

		if( he[1] )
		{
			assert( he[0]->target() == he[1]->source() && he[0]->source() == he[1]->target() );

			if( he[0]->target()->id() < he[0]->source()->id() )
			{
				edge->halfedge(0) = he[1];
				edge->halfedge(1) = he[0];
			}

			assert( edgeVertex1(edge)->id() < edgeVertex2(edge)->id() );
		}
		else
		{
			he[0]->vertex()->boundary() = true;
			he[0]->he_prev()->vertex()->boundary()  = true;
		}
	}

	if (aux!=2)
		removeAux();
	fclose(fp);
	return true;
}



bool Mesh::write( const char outputFile[] )
{
	FILE * fp = fopen( outputFile,"w");
	if ( !fp )
	{
		printf("Cannot open file %s to write\n", outputFile);
		return false;
	}

	std::vector<Vertex *>::iterator vit;
	for (vit=m_verts.begin(); vit!=m_verts.end(); ++vit)
	{
		tVertex ver = *vit;
		std::ostringstream oss;
		oss.precision(12);
		oss.setf(std::ios::fixed,std::ios::floatfield);
		oss << "Vertex " << ver->id() << " " << ver->point()[0] << " " << ver->point()[1] << " " << ver->point()[2];
		fprintf(fp, "%s ", oss.str().c_str());
		//strcpy(vstr, oss.str().c_str());
		//fprintf(fp, "Vertex %d ", ver->id());
		//for (int i=0; i<3; ++i)
		//	fprintf(fp, "%lf ", ver->point()[i]);
		if (ver->string().size() > 0)
		{
			fprintf(fp, "{%s}", ver->string().c_str() );
		}
		fprintf(fp, "\n");
	}

	std::vector<Face *>::iterator fit;
	for (fit=m_faces.begin(); fit!=m_faces.end(); ++fit)
	{
		tFace face = *fit;
		tHalfEdge the0 = face->halfedge();
		tHalfEdge the1 = the0->he_next();
		int v0 = the0->source()->id();
		int v1 = the0->target()->id();
		int v2 = the1->target()->id();
		fprintf(fp, "Face %d %d %d %d ", face->id(), v0, v1, v2);
		if (face->string().size() > 0)
		{
			fprintf(fp, "{%s}", face->string().c_str() );
		}
		fprintf(fp, "\n");
	}

	std::vector<Edge *>::iterator eit;
	for (eit=m_edges.begin(); eit!=m_edges.end(); ++eit)
	{
		tEdge edge = *eit;		
		if (edge->string().size() > 0)
		{
			fprintf(fp, "Edge %d %d {%s} \n", edge->vid(0), edge->vid(1), edge->string().c_str() );
		}
	}
	fclose(fp);
	return true;
}

void Mesh::copyTo( Mesh & solid )
{
	int i;
	for(i=0; i<m_verts.size(); ++i)
	{
		Vertex * v = m_verts[i];
		Vertex * nv = solid.createVertex(v->id());
		nv->point() = v->point();
		nv->string() = v->string();		
		nv->boundary() = v->boundary();
	}
	solid.maxid = maxid;
	solid.setupAux();
	
	for (i=0; i<m_faces.size(); ++i)
	{
		Face * f = m_faces[i];
		HalfEdge * he = f->halfedge();
		int v[3];
		for (int j=0; j<3; ++j)
		{
			v[j] = he->vertex()->id();
			he = he->he_next();
		}
		Face * nf = solid.createFace(v, f->id());
		nf->string() = f->string();
	}
	solid.removeAux();

	solid.id2VInd.clear();
	solid.id2FInd.clear();
	
	stdext::hash_map<int, int>::iterator cur  = id2VInd.begin();
	for (; cur!=id2VInd.end(); ++cur)
	{
		solid.id2VInd[(*cur).first] = (*cur).second;
	}
		
	cur  = id2FInd.begin();	
	for (; cur!=id2FInd.end(); ++cur)
	{
		solid.id2FInd[(*cur).first] = (*cur).second;
	}
}

bool Mesh::isBoundary( Mesh::tVertex  v ) const
{
	return v->boundary();
}

bool Mesh::isBoundary( Mesh::tEdge  e ) const
{
	if( e->halfedge(0) == NULL || e->halfedge(1) == NULL ) return true;
	return false;
}

bool Mesh::isBoundary( Mesh::tHalfEdge  he ) const
{
	if( he->he_sym() == NULL ) return true;
	return false;
}

Point & Mesh::verUV(tVertex v)
{
	return uv[id2VInd[v->id()]];
}

int Mesh::faceId( Mesh::tFace  f ) const
{
	return f->id();
}
int Mesh::vertexId( Mesh::tVertex  v ) const
{
	return v->id();
}

Mesh::tEdge Mesh::idEdge( int id0, int id1 )
{
	Vertex * v0 = idVertex(id0);
	Vertex * v1 = idVertex(id1);
	if (!v0 || !v1)
	{
		return NULL;
	}

	if (aux==1)
	{
		AdjHeTrait * adjHe = (AdjHeTrait *) v0->traits()->getTrait(ID_AdjHeTrait);
		int hesize = adjHe->NumAdjHE();		
		for (int j=0; j<hesize; ++j)
		{
			HalfEdge * he = adjHe->indAdjHe(j);
			assert(he->target()->id() == id0);
			if (he->source()->id() == id1)
			{
				return he->edge();
			}
		}
		
		adjHe = (AdjHeTrait *) v1->traits()->getTrait(ID_AdjHeTrait);
		hesize = adjHe->NumAdjHE();		
		for (int j=0; j<hesize; ++j)
		{
			HalfEdge * he = adjHe->indAdjHe(j);
			assert(he->target()->id() == id1);
			if (he->source()->id() == id0)
			{
				return he->edge();
			}
		}
	}
	else if (aux==2)
	{
		for (VertexEdgeIterator eviter(v0); !eviter.end(); ++eviter)
		{
			Edge * e = *eviter;
			if (e->vid(0) == id0 && e->vid(1) == id1)
				return e;
			else if (e->vid(1) == id0 && e->vid(0) == id1)
				return e;
		}
	}
	else
	{
		assert(aux==0);	
		//assert(0);
		for (int i=0; i<m_edges.size(); ++i)
		{
			Edge * te = m_edges[i];
			if (te->vid(0) == id0 && te->vid(1) == id1)
				return te;
			else if (te->vid(0) == id1 && te->vid(1) == id0)
				return te;
		}
		return NULL;
	}
	return NULL;
}

Mesh::tEdge Mesh::vertexEdge( tVertex v0, tVertex v1 )
{
	if (aux==2)
	{
		for (VertexEdgeIterator veiter(v0); !veiter.end(); ++veiter)
		{
			Edge * e = *veiter;
			if (e->vid(0) == v0->id() && e->vid(1) == v1->id())
				return e;
			else if (e->vid(1) == v0->id() && e->vid(0) == v1->id())
				return e;
		}
		return NULL;

		//HalfEdge * he0 = v0->halfedge();
		//HalfEdge * he1 = v1->halfedge();
		//if (!he0 && !he1)
		//	return NULL;
		//if (he0)
		//{

		//	if (he0->source() == v1)
		//		return he0->edge();
		//}
		//if (he1)
		//{
		//	if (he1->source() == v0)
		//		return he1->edge();
		//}
		//return NULL;
	}
	else if (!aux)
	{
		std::vector<Edge *>::iterator eit;
		for (eit = m_edges.begin(); 
			eit != m_edges.end(); 
			++eit)
		{
			Edge * te = *eit;
			HalfEdge * the0 = te->halfedge(0);
			Vertex * tv0 = the0->source();
			Vertex * tv1 = the0->target();		
			if (tv0 == v0 && tv1 == v1)
				return te;
			else if (tv0 == v1 && tv1 == v0)
				return te;
		}
		return NULL;
	}
	else
	{
		assert(aux==1);
		assert(0);
	}
	return NULL;

}

Mesh::tFace Mesh::edgeFace1( Mesh::tEdge  e )
{
	assert( e->halfedge(0) != NULL );
	return e->halfedge(0)->face();
}
Mesh::tFace Mesh::edgeFace2( Mesh::tEdge  e )
{
	assert( e->halfedge(1) != NULL );
	return e->halfedge(1)->face();
}

Mesh::tFace Mesh::halfedgeFace( Mesh::tHalfEdge  he )
{
	return he->face();
}

Mesh::tFace Mesh::idFace(const int id )
{
	if (id2FInd.find(id) == id2FInd.end())
		return NULL;
	else
	{
		//const int ind = id2FInd[id];
		Face * f = m_faces[id2FInd[id]];
		assert(f->id()==id);
		return f;
	}

	//if (!id2FInd.size())
	//{
	//	id2FInd.resize(maxfid+1, -1);
	//	//for (int i=0;i<=maxfid; ++i)
	//	//{
	//	//	id2FInd.push_back(-1);
	//	//}
	//	for (int i=0;i<m_faces.size();++i)
	//	{
	//		Face * f = m_faces[i];
	//		id2FInd[f->id()] = i;
	//	}
	//}
	//if (id<0 || id>maxfid)
	//	return NULL;
	//int ind = id2FInd[id];
	//if (ind < 0 || ind >= m_faces.size())
	//	return NULL;
	//Face * f = m_faces[ind];
	//assert(f->id() == id);
	//return f;
	//for (int i=0; i<m_faces.size(); ++i)
	//{
	//	Face * tf = m_faces[i];
	//	if (tf->id() == id)
	//		return tf;
	//}
	//return NULL;
}

Mesh::tHalfEdge  Mesh::corner( tVertex v, tFace f)
{
	Mesh::tHalfEdge he = f->halfedge();
	do{
		if( he->vertex() == v )
			return he;
		he = he->he_next();
	}while( he != f->halfedge() );
	return NULL;
}

Mesh::tHalfEdge Mesh::idHalfedge( int id0, int id1 )
{
	tEdge e = idEdge( id0, id1 );
	if (!e)
		return NULL;
	tHalfEdge he = e->halfedge(0);
	if( he->source()->id() == id0 && he->target()->id() == id1 ) 
		return he;
	he = e->halfedge(1);
	if (!he)
		return NULL;
	if( he->source()->id() == id0 && he->target()->id() == id1 ) 
		return he;
	assert(0);
	return NULL;
}

Mesh::tHalfEdge Mesh::vertexHalfedge( tVertex v0, tVertex v1 )
{
	tEdge e = vertexEdge( v0, v1 );
	if (!e)
		return NULL;
	tHalfEdge he = e->halfedge(0);
	if( he->vertex() == v1 && he->he_prev()->vertex() == v0 ) 
		return he;
	he = e->halfedge(1);
	if (he)
	{
		assert( he->vertex() == v1 && he->he_prev()->vertex() == v0 );
	}
	return he;
}

Mesh::tVertex Mesh::edgeVertex1( Mesh::tEdge  e )
{
	assert(e->halfedge(0));
	return e->halfedge(0)->source();
}

Mesh::tVertex Mesh::edgeVertex2( Mesh::tEdge  e )
{
	assert(e->halfedge(0));
	return e->halfedge(0)->target();
}

Mesh::tVertex Mesh::halfedgeVertex( Mesh::tHalfEdge  he )
{
	return he->vertex();
}

Mesh::tVertex Mesh::idVertex( int id ) 
{
	if (id2VInd.find(id) == id2VInd.end())
		return NULL;
	else
	{
		Vertex * v = m_verts[id2VInd[id]];
		assert(v->id() == id);
		return v;
	}


//	return m_verts[id-1];
	//if (id<0 || id>maxid)
	//	return NULL;
	//int ind = id2VInd[id];
	//if (ind < 0 || ind >= m_verts.size())
	//{
	//	return NULL;
	//}
	//else
	//{
	//	assert(id == m_verts[ind]->m_id);
	//	return m_verts[ind];
	//}
/*
	if (m_allV)
	{
		return m_allV[id];
	}
	else
	{
		std::vector<Vertex *>::iterator vit;
		for (vit = m_verts.begin(); vit != m_verts.end(); vit++)
		{
			Vertex * tv = *vit;
			if (tv->id() == id)
				return tv;
		}
		return NULL;
	}
*/
}

Mesh::tEdge Mesh::edgeFlip(Edge * e)
{
	HalfEdge * he1 = e->halfedge(0);
	HalfEdge * he2 = e->halfedge(1);
	if (!he1 || !he2)
	{
		// this edge is on boundary, can not do flip
		return NULL;
	}
	Face * f1 = he1->face();
	Face * f2 = he2->face();
	HalfEdge * he3 = he1->he_next();
	HalfEdge * he4 = he3->he_next();
	HalfEdge * he5 = he2->he_next();
	HalfEdge * he6 = he5->he_next();
	e->vid(0) = he3->target()->id();
	e->vid(1) = he5->target()->id();
	he1->target() = he3->target();
	he2->target() = he5->target();
	he4->he_next() = he5; he5->he_prev() = he4;
	he5->he_next() = he1; he1->he_prev() = he5;
	he1->he_next() = he4; he4->he_prev() = he1;
	he3->he_next() = he2; he2->he_prev() = he3;
	he2->he_next() = he6; he6->he_prev() = he2;
	he6->he_next() = he3; he3->he_prev() = he6;
	he5->face() = f1;
	he3->face() = f2;
	f1->halfedge() = he4;
	f2->halfedge() = he6;
	HalfEdge * h = e->halfedge(0);
	if( h->target()->id() < h->source()->id() )
	{
		e->halfedge(0) = e->halfedge(1);
		e->halfedge(1) = h;
	}
	he6->target()->halfedge() = he6;
	he4->target()->halfedge() = he4;
	return e;
}

Vertex * Mesh::faceSplit(tFace f, double bary[3])
{
	if (bary[0]<0 || bary[1]<0 || bary[2]<0)
	{
		// the split point is not inside the face, can't do split
		return NULL;
	}
	// first we create the new vertex
	HalfEdge * he[3];
	he[0] = f->halfedge();
	Vertex * v[4];	
	v[0] = he[0]->target();
	he[1] = he[0]->he_next();
	v[1] = he[1]->target();
	he[2] = he[1]->he_next();
	v[2] = he[2]->target();
	v[3] = createVertex( ++this->maxid );
	v[3]->point() = (v[0]->point() * bary[0] + v[1]->point() * bary[1] + v[2]->point() * bary[2]) / (bary[0] + bary[1] + bary[2]);

	int i,j;

	//delete the original big face	
	int maxfid = -1;
	for (i=0; i<m_faces.size(); ++i)
	{
		if (m_faces[i]->id() > maxfid)
			maxfid = m_faces[i]->id();
	}

	tHalfEdge hes[3][2];
	for(i = 0; i < 3; ++i)
	{
		for (j=0; j<2; ++j)
		{
			hes[i][j] = new HalfEdge;
			assert(hes[i][j]);
		}
		hes[i][0]->vertex() = v[3];
		v[3]->halfedge() = hes[i][0];
		hes[i][1]->vertex() = v[i];
		he[i]->he_next() = hes[i][0];  hes[i][0]->he_prev() = he[i];
	}
	assert(aux == 2);

	//linking to each other for left 6 halfedges
	hes[0][0]->he_next() = hes[2][1];  hes[2][1]->he_prev() = hes[0][0];
	hes[1][0]->he_next() = hes[0][1];  hes[0][1]->he_prev() = hes[1][0];
	hes[2][0]->he_next() = hes[1][1];  hes[1][1]->he_prev() = hes[2][0];

	hes[2][1]->he_next() = he[0]; he[0]->he_prev() = hes[2][1];
	hes[0][1]->he_next() = he[1]; he[1]->he_prev() = hes[0][1];
	hes[1][1]->he_next() = he[2]; he[2]->he_prev() = hes[1][1];

	tFace f1 = createFace( ++maxfid );
	tFace f2 = createFace( ++maxfid );

	//linking to face
	hes[0][0]->face() = hes[2][1]->face() = he[0]->face() = f;
	hes[1][0]->face() = hes[0][1]->face() = he[1]->face() = f1;
	hes[2][0]->face() = hes[1][1]->face() = he[2]->face() = f2;
	f->halfedge() = he[0];
	f1->halfedge() = he[1];
	f2->halfedge() = he[2];

	//connecting with edge
	for( i = 0; i < 3; i ++ )
	{		
		int id0 = hes[i][0]->target()->id(); 
		int id1 = hes[i][1]->target()->id(); 
		tEdge e = new Edge(id0, id1);
		m_edges.push_back(e);
		hes[i][0]->edge() = hes[i][1]->edge() = e;
		e->halfedge(0) = hes[i][0];
		e->halfedge(1) = hes[i][1];
	}	

	return v[3];
}

Vertex * Mesh::EdgeSplit( Edge * e )
{
	aux = 0;

	// (1) Initialize: Get halfedge pointer
	tHalfEdge hes[2];
	hes[0] = e->halfedge(0);
	hes[1] = e->halfedge(1);
	int i,j,k;

	//(2) Remove edge
	std::vector<Edge *>::iterator eiter = std::find(m_edges.begin(), m_edges.end(), e);
	if (eiter == m_edges.end())
	{
		std::cerr << "Error: Given edge does not exists in current mesh or is not valid!" << std::endl;
		return NULL;
	}
	//assert(eiter != m_edges.end());
	m_edges.erase(eiter);	


	//(3) Create a new vertex
	tVertex nv = createVertex( ++maxid );	
	nv->boundary() = (!hes[1]);
	nv->point() = (hes[0]->source()->point() + hes[0]->target()->point())/2;
	
	//(4)
	std::vector<Face *> new_faces;
	for( j = 0; j < 2; ++j )
	{
		tVertex v[3];
		tHalfEdge he = hes[j];

		if( !he ) continue;

		//(4.1) Find and remove the face f connected to hes[j]
		tFace f = he->face();
		std::vector<Face *>::iterator fiter = std::find(m_faces.begin(), m_faces.end(), f);
		assert(fiter!=m_faces.end());
		m_faces.erase(fiter);
		delete f;

		//(4.2) Remove the linkings from f's halfedges to their original connected edges
		for(i = 0; i < 3; ++i )
		{
			v[i] = he->target();			
			tEdge te = he->edge();
			if( te->halfedge(0) == he )
			{
				te->halfedge(0) = te->halfedge(1);
			}
			te->halfedge(1) = NULL;
			if (!te->halfedge(0))
			{//both halfedges have been removed
				//assert(te == e);
				if (te!=e)
				{
					std::vector<Edge *>::iterator teiter = 
						std::find(m_edges.begin(), m_edges.end(), te);
					assert(teiter != m_edges.end());
					m_edges.erase(teiter);
					delete te;
				}
			}
			he = he->he_next();
		}

		//(4.3) Remove all halfedges in f
		for(k = 0; k < 3; k ++ )
		{
			tHalfEdge ne = he->he_next();
			delete he;
			he = ne;
		}
		hes[j] = NULL;

		//(4.4) Create new faces
		int vid[3];
		tVertex w[3];
		w[0] = nv; w[1] = v[0]; w[2] = v[1];
		for(k = 0; k < 3; k ++ )
		{
			vid[k] = w[k]->id(); 
		}
		Face * nf = createFace( vid, ++maxfid );
		if (!nf)
		{
			std::cerr << "Error: creating face " << maxfid << " fails!" <<std::endl;
			return NULL;
		}
		new_faces.push_back( nf );

		w[0] = nv; w[1] = v[1]; w[2] = v[2];
		for(k = 0; k < 3; k ++ )
		{
			vid[k] = w[k]->id(); 
		}	
		nf = createFace( vid, ++maxfid );
		if (!nf)
		{
			std::cerr << "Error: creating face " << maxfid << " fails!" <<std::endl;
			return NULL;
		}
		new_faces.push_back( nf );
	}

	//(5) Link newly created halfedges and edges
	for(i=0; i<new_faces.size(); i++ )
	{
		Face * f = new_faces[i];
		HalfEdge * he = f->halfedge();

		do{
			Edge     * e = he->edge();
			if( e->halfedge(1) )
			{
				HalfEdge * h = e->halfedge(0);
				if( h->target()->id() < h->source()->id() )
				{
					e->halfedge(0) = e->halfedge(1);
					e->halfedge(1) = h;
				}
			}
			he = he->he_next();
		}while( he != f->halfedge() );
	}

	delete e;
	
	return nv;	
}

void Mesh::setupAux()
{
	aux = 1;
	std::vector<Vertex *>::iterator vit;
	for (vit=m_verts.begin(); vit!=m_verts.end(); ++vit)
	{
		Vertex * ver = *vit;
		AdjHeTrait * t_he = (AdjHeTrait *)ver->GetTrait(ID_AdjHeTrait);
		if (!t_he)
		{
			t_he = new AdjHeTrait;
			ver->AddTrait(t_he);			
		}
	}
}

void Mesh::resetAux()
{
	std::vector<Edge *>::iterator eit;
	for (eit=m_edges.begin(); eit!=m_edges.end(); ++eit)
	{
		Edge * e = *eit;
		HalfEdge * he0 = e->halfedge(0);
		if (he0)
		{
			Vertex * v0 = he0->target();
			AdjHeTrait * tv0 = (AdjHeTrait *)v0->GetTrait(ID_AdjHeTrait);
			tv0->AddHE(he0);
		}
		HalfEdge * he1 = e->halfedge(1);
		if (he1)
		{
			Vertex * v1 = he1->target();
			AdjHeTrait * tv1 = (AdjHeTrait *)v1->GetTrait(ID_AdjHeTrait);
			tv1->AddHE(he1);
		}		
	}
}

void Mesh::removeAux()
{
	for (int i=0; i<m_edges.size(); ++i)
	{
		Edge * edge = m_edges[i];
		Mesh::tHalfEdge he[2];
		he[0] = edge->halfedge(0);
		he[1] = edge->halfedge(1);
		assert(he[0]);
		if (he[1])
		{
			assert(he[0]->target() == he[1]->source() && he[0]->source() == he[1]->target());
			if (he[0]->target()->id() < he[0]->source()->id())
			{
				edge->halfedge(0) = he[1];
				edge->halfedge(1) = he[0];
			}			
		}
		else
		{
			he[0]->vertex()->boundary() = true;
			he[0]->he_prev()->vertex()->boundary() = true;
		}		
	}
	std::vector<Vertex *>::iterator vit;
	for (vit=m_verts.begin(); vit!=m_verts.end(); ++vit)
	{
		Vertex * ver = *vit;
		ver->DelTrait(ID_AdjHeTrait);
	}
	aux = 2;
//	delete [] m_allV;
//	this->m_allV = NULL;
}


//create new geometric simplexes

Mesh::tVertex Mesh::createVertex( int id )
{
	Vertex * v = new Vertex;	
	v->id() = id;	
	if (id > this->maxid)
	{
		this->maxid = id;
	}	
	id2VInd[id] = m_verts.size();
	m_verts.push_back( v );
	if (aux==1)
	{ // an aux trait has been attached to vertex, do the samething to this new vertex		
		AdjHeTrait * t_he = new AdjHeTrait;	
		v->AddTrait(t_he);
		//v->traits()->addTrait((TraitNode *)t_he);		
	}
	return v;
}

Mesh::tEdge Mesh::createEdge( Vertex * start, Vertex * end )
{
	tEdge e = idEdge(start->id(), end->id());
	if (!e)
	{
		e = new Edge(start->id(), end->id());
		assert(e);
		m_edges.push_back( e );	
	}
	return e;
}

Mesh::tFace Mesh::createFace( int id )
{
	tFace f = new Face();
	assert(f);
	f->id() = id;
	if (maxfid < id)
		maxfid = id;
	m_faces.push_back(f);	
	return f;
}

Mesh::tFace Mesh::createFace( Vertex * ver[3] , int id )
{	
	if (id2FInd.find(id) != id2FInd.end())
	{//Return NULL if this id already exits
		std::cerr << "Try to duplicate create a face with existing id: " << id << " !" << std::endl;
		return NULL;
	}

	id2FInd[id] = m_faces.size();
	int i,j;
	tFace f = createFace( id );	
	//create halfedges
	tHalfEdge hes[3];
	for(i = 0; i < 3; i ++ )
	{
		hes[i] = new HalfEdge;
		assert( hes[i] );
		hes[i]->vertex() = ver[i];
		ver[i]->halfedge() = hes[i];
		if (aux==1)
		{			
			//TraitList * tl = ver[i]->traits();
			//AdjHeTrait * adjHe = (AdjHeTrait *) tl->getTrait(ID_AdjHeTrait);
			AdjHeTrait * adjHe = (AdjHeTrait *) ver[i]->GetTrait(ID_AdjHeTrait);
			adjHe->AddHE(hes[i]);
		}
	}
	//linking to each other
	for( i = 0; i < 3; i ++ )
	{
		hes[i]->he_next() = hes[(i+1)%3];
		hes[i]->he_prev() = hes[(i+2)%3];
	}
	//linking to face
	for( i = 0; i < 3; i ++ )
	{
		hes[i]->face()   = f;
		f->halfedge()    = hes[i];
	}
	//connecting with edge
	for( i = 0; i < 3; i ++ )
	{
		//tEdge e = createEdge( ver[i], ver[(i+2)%3] );
		int id0 = ver[i]->id(); // target
		int id1 = ver[(i+2)%3]->id(); // source
		tEdge e;
		bool existE=false;
		if (aux==1)
		{
			AdjHeTrait * adjHe = (AdjHeTrait *) ver[(i+2)%3]->traits()->getTrait(ID_AdjHeTrait);
			int hesize = adjHe->NumAdjHE();		
			for (j=0; j<hesize; ++j)
			{
				HalfEdge * he = adjHe->indAdjHe(j);
				assert(he->target()->id() == id1);
				if (he->source()->id() == id0)
				{
					e = he->edge();
					existE = true;
					break;
				}
			}
		}
		else
		{
			Edge * tte = idEdge(id0, id1);
			if (tte)
			{
				existE = true;
				e = tte;
			}
			else 
				existE = false;
		}
		if (!existE)
		{
			e = new Edge(id0, id1);
			m_edges.push_back(e);
		}
		if(!e->halfedge(0))
		{
			e->halfedge(0) = hes[i];
		}
		else
		{
			assert(!e->halfedge(1));
			e->halfedge(1) = hes[i];
		}
		hes[i]->edge() = e;
	}
	return f;
}

Mesh::tFace Mesh::createFace( int v[3] , int id )
{
	Vertex * ver[3];
	for(int i = 0; i < 3; i ++ )
	{
		ver[i] =  idVertex( v[i] );
		if (!ver[i])
		{
			std::cerr << "Error: invalid vertex id: " << v[i] << " provided when creating face "
				<< id << " !" << std::endl;
			return NULL;
		}
	}
	Face * f = createFace(ver, id);
	return f;
}

Mesh::tHalfEdge Mesh::vertexMostClwOutHalfEdge( Mesh::tVertex  v )
{
	return v->most_clw_out_halfedge();
};

Mesh::tHalfEdge Mesh::vertexMostCcwOutHalfEdge( Mesh::tVertex  v )
{
	return v->most_ccw_out_halfedge();
};

Mesh::tHalfEdge Mesh::vertexNextCcwOutHalfEdge( Mesh::tHalfEdge  he )
{
	return he->ccw_rotate_about_source();
};

Mesh::tHalfEdge Mesh::vertexNextClwOutHalfEdge( Mesh::tHalfEdge  he )
{
	assert( he->he_sym() != NULL );
	return he->clw_rotate_about_source();
};

Mesh::tHalfEdge Mesh::vertexMostClwInHalfEdge( Mesh::tVertex  v )
{
	return v->most_clw_in_halfedge();
};

Mesh::tHalfEdge Mesh::vertexMostCcwInHalfEdge( Mesh::tVertex  v )
{
	return v->most_ccw_in_halfedge();
};

Mesh::tHalfEdge Mesh::vertexNextCcwInHalfEdge( Mesh::tHalfEdge  he )
{
	assert( he->he_sym() != NULL );
	return he->ccw_rotate_about_target();
};

Mesh::tHalfEdge Mesh::vertexNextClwInHalfEdge( Mesh::tHalfEdge  he )
{
	return he->clw_rotate_about_target();
};

Mesh::tHalfEdge Mesh::faceNextClwHalfEdge( Mesh::tHalfEdge  he )
{
	return he->he_prev();
};

Mesh::tHalfEdge Mesh::faceNextCcwHalfEdge( Mesh::tHalfEdge  he )
{
	return he->he_next();
};

Mesh::tHalfEdge Mesh::faceMostCcwHalfEdge( Mesh::tFace  face )
{
	return face->halfedge();
};

Mesh::tHalfEdge Mesh::faceMostClwHalfEdge( Mesh::tFace  face )
{
	return face->halfedge()->he_next();
};

int Mesh::sid(int vid)
{
	//if (vid < 0 || vid > maxid)
	//{
	//	return -1;
	//}
	//else
	return id2VInd[vid];
}

int Mesh::sid2vid(int sid)
{
	if (sid>=0 && sid <m_verts.size())
	{
		return (m_verts[sid])->id();
	}
	else
		return -1;
}

Vertex * Mesh::sidVertex(int sid)
{
	if (sid>=0 && sid <m_verts.size())
	{
		return m_verts[sid];
	}
	else
		return NULL;	
}
