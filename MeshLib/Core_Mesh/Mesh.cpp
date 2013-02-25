#include "Mesh.h"
#include <algorithm>
#include "iterators.h"
#include <iostream>
#include <sstream>
#include <string>

#pragma warning (disable : 4996)
#pragma warning (disable : 4018)

 Mesh::~Mesh(){	clear();}

void Mesh::clear(){
	for (std::list<Face *>::iterator fiter = m_faces.begin(); fiter!=m_faces.end(); ++fiter)
	{
		Face * f = *fiter;
		delete f;
	}
	for (std::list<Edge *>::iterator eiter = m_edges.begin(); eiter!=m_edges.end(); ++eiter)
	{
		Edge * e = *eiter;
		Halfedge * he1 = e->he(0);
		Halfedge * he2 = e->he(1);
		delete he1;
		if (he2) delete he2;
		delete e;
	}
	for (std::list<Vertex *>::iterator viter = m_verts.begin(); viter!=m_verts.end(); ++viter)
	{
		Vertex * v = *viter;
		delete v;
	}
	m_verts.clear();
	m_edges.clear();
	m_faces.clear();
	maxVid=-1;
	maxFid = -1;
}

void Mesh::LabelBoundaries()
{	
	if (!boundaryLabeled) 
		boundaryLabeled=true;
	else 
		return;//only need to do the labeling once

	//Label boundary edges
	for( std::list<Edge *>::iterator eiter = m_edges.begin(); 	eiter!=m_edges.end(); ++eiter)
	{		
		Edge * edge = *eiter;
		Halfedge * he[2];
		he[0] = edge->he(0);
		he[1] = edge->he(1);
		if( !he[1] )
		{
			he[0]->target()->boundary() = true;
			he[0]->source()->boundary() = true;
		}
	}
}

bool Mesh::read( const char inputFile[])
{	
	std::cout << "Reading mesh " << inputFile << " ...";
	FILE * fp = fopen( inputFile, "r" );
	if( !fp ){
		std::cerr << "Can't open file " << inputFile << "!" <<std::endl;
		return false;
	}

	char line[1024];
	clear();

	while(!feof(fp)){	
		fgets(line, 1024, fp);
		if(!strlen(line)) continue;
		char * str = strtok( line, " \r\n");
		if (!str) continue;

		if( !strcmp(str, "Vertex" ) ){ //parsing a line of vertex element
			str = strtok(NULL," \r\n{");
			int id = atoi( str );
			Vertex * v  = createVertex( id ); // Important 1
			for( int i = 0 ; i < 3; i ++ ){
				str = strtok(NULL," \r\n{");
				v->point()[i] = atof( str );
			}		
			v->id() = id;
			std::vector<Halfedge *> heList; //storing the neighboring halfedges, for efficient edge searching in the initialization stage
			vertIndex[v] = v_adjHeList.size();
			v_adjHeList.push_back(heList);
			str = strtok( NULL, "\r\n");
			if(!str || strlen( str ) == 0) continue;
			std::string s(str);
			int sp = s.find("{");
			int ep = s.find("}");
			if( sp >= 0 && ep >= 0 )
				v->PropertyStr() = s.substr( sp+1, ep-sp-1 );
			continue;
		}
		
		if( !strcmp(str,"Face") ){ //parsing a line of face element
			str = strtok(NULL, " \r\n");
			if( !str || strlen( str ) == 0 ) continue;
			int id = atoi( str );
			int vid[3];
			for( int i = 0; i < 3; i ++ )
			{
				str = strtok(NULL," \r\n{");
				vid[i] = atoi(str);
			}
			Face * f = createFace( vid, id );  //Important 2
			if (!str) continue;
			str = strtok( NULL, "\r\n");
			if( !str || strlen( str ) == 0 ) continue;
			std::string s(str);
			int sp = s.find("{");
			int ep = s.find("}");
			if( sp >= 0 && ep >= 0 )
				f->PropertyStr() = s.substr( sp+1, ep-sp-1 );
			continue;
		}

		//Read in edge attributes, if there is any
		if( !strcmp(str,"Edge") )
		{
			LabelBoundaries();
			str = strtok(NULL, " \r\n");
			if (!str)	continue;
			int id0 = atoi( str );
			str = strtok(NULL, " \r\n");
			int id1 = atoi( str );
			Edge * edge = idEdge( id0, id1 );
			if (!edge)	continue;
			str = strtok(NULL, "\r\n");
			if (!str)	continue;
			std::string s(str);
			int sp = s.find("{");
			int ep = s.find("}");
			if( sp >= 0 && ep >= 0 )
				edge->PropertyStr() = s.substr( sp+1, ep-sp-1 );
			continue;
		}
	}
	LabelBoundaries();
	fclose(fp);

	//After initialization, the adjacent halfedge list is not necessary; remove them to save space
	for (int i=0;i<v_adjHeList.size();++i)
		v_adjHeList[i].clear();
	v_adjHeList.clear();

	printf("Done!\n");
	return true;
}



bool Mesh::write( const char outputFile[] )
{
	FILE * fp = fopen( outputFile,"w");
	if ( !fp ){
		std::cerr << "Cannot open file " << outputFile << "to write!" <<std::endl;
		return false;
	}

	std::cout << "Writing mesh "<< outputFile <<" ...";
	std::list<Vertex *>::iterator vit;
	for (vit=m_verts.begin(); vit!=m_verts.end(); ++vit){
		Vertex * ver = *vit;
		std::ostringstream oss;
		oss.precision(10); oss.setf(std::ios::fixed,std::ios::floatfield);  //setting the output precision: now 10
		oss << "Vertex " << ver->id() << " " << ver->point()[0] << " " << ver->point()[1] << " " << ver->point()[2];
		fprintf(fp, "%s ", oss.str().c_str());
		if (ver->PropertyStr().size() > 0)
			fprintf(fp, "{%s}", ver->PropertyStr().c_str() );
		fprintf(fp, "\n");
	}

	std::list<Face *>::iterator fit;
	for (fit=m_faces.begin(); fit!=m_faces.end(); ++fit)
	{
		Face * face = *fit;
		Halfedge * the0 = face->he();
		Halfedge * the1 = the0->next();
		int v0 = the0->source()->id();
		int v1 = the0->target()->id();
		int v2 = the1->target()->id();
		fprintf(fp, "Face %d %d %d %d ", face->id(), v0, v1, v2);
		if (face->PropertyStr().size() > 0)
			fprintf(fp, "{%s}", face->PropertyStr().c_str() );
		fprintf(fp, "\n");
	}

	std::list<Edge *>::iterator eit;
	for (eit=m_edges.begin(); eit!=m_edges.end(); ++eit)
	{
		Edge * edge = *eit;		
		if (edge->PropertyStr().size() > 0)
		{ //Only output edges when they have special traits
			fprintf(fp, "Edge %d %d {%s} \n", edge->he(0)->source()->id(), edge->he(0)->target()->id(), edge->PropertyStr().c_str() );
		}
	}
	fclose(fp);
	std::cout << "Done!" <<std::endl;
	return true;
}

void Mesh::copyTo( Mesh & tMesh )
{
	std::cout << "Copying the mesh...";
	int i=0;
	for(std::list<Vertex *>::iterator viter = m_verts.begin();
		viter!=m_verts.end(); ++viter)
	{
		Vertex * v = *viter;
		Vertex * nv = tMesh.createVertex(v->id());
		nv->point() = v->point();
		nv->PropertyStr() = v->PropertyStr();		
		nv->boundary() = v->boundary();
		tMesh.id2Ver[v->id()] = nv;
	}
	tMesh.maxVid = maxVid;
	tMesh.boundaryLabeled = boundaryLabeled;
	
	std::list<Face *>::iterator fiter = m_faces.begin();
	for(;fiter!=m_faces.end(); ++fiter)
	{
		Face * f = *fiter;
		Face * nf = tMesh.createFace(f->id());
		Halfedge * he[3];
		Halfedge * nhe[3];
		he[0] = f->he();		he[1] = he[0]->next();		he[2] = he[1]->next();
		for (int j=0; j<3; ++j)
		{
			nhe[j] = new Halfedge();
			Vertex * v1 = he[j]->target();
			Vertex * nv1 = tMesh.idVertex(v1->id());
			nv1->he()=nhe[j];
			nhe[j]->target() = nv1;
			nhe[j]->face() = nf;
		}
		for (int j=0;j<3;++j)
		{
			nhe[j]->next()=nhe[(j+1)%3];
			nhe[j]->prev()=nhe[(j+2)%3];
		}
		nf->he()=nhe[0];
		nf->PropertyStr() = f->PropertyStr();
		tMesh.id2Face[nf->id()]=nf;
	}

	for(fiter = m_faces.begin();fiter!=m_faces.end(); ++fiter)
	{
		Face * f = *fiter;
		Face * nf = tMesh.idFace(f->id());
		Halfedge * he[3];
		Halfedge * nhe[3];
		he[0] = f->he();		he[1] = he[0]->next();		he[2] = he[1]->next();
		nhe[0] = nf->he();		nhe[1] = nhe[0]->next();		nhe[2] = nhe[1]->next();
		for (int i=0;i<3;++i)
		{
			Edge * e = he[i]->edge();
			if (he[i]==e->he(0))
			{
				if (e->boundary())
				{
					Edge * ne = tMesh.createEdge(nhe[i], NULL);
					nhe[i]->edge()=ne;
					ne->PropertyStr() = e->PropertyStr();
					continue;
				}
				//get its twin: twin_he
				Halfedge * twin_he, * twin_nhe;
				Face * tf = he[i]->twin()->face();
				Face * tnf = tMesh.idFace(tf->id());
				twin_he = tf->he();
				twin_nhe = tnf->he();
				for (int j=0;j<3;++j)
				{
					if (twin_he->edge()==e)	break;
					twin_he = twin_he->next();
					twin_nhe = twin_nhe->next();
				}
				Edge * ne = tMesh.createEdge(nhe[i], twin_nhe);
				nhe[i]->edge() = ne;
				twin_nhe->edge() = ne;
				ne->PropertyStr() = e->PropertyStr();
			}
		}
	}
	std::cout<< "Done!" <<std::endl;
}

Edge * Mesh::idEdge( int id0, int id1 )
{
	Vertex * v0 = idVertex(id0);
	Vertex * v1 = idVertex(id1);
	if (!v0 || !v1)
		return NULL;
	else
		return vertexEdge(v0, v1);
}

Edge * Mesh::vertexEdge( Vertex * v0, Vertex * v1 )
{
	Halfedge * he0 = v0->most_clw_out_halfedge();
	if(he0->target() == v1)
		return he0->edge();

	Halfedge * he=he0->ccw_rotate_about_source();
	while (he!=he0 && he){
		if (he->target()==v1)
			return he->edge();
		he = he->ccw_rotate_about_source();
	}

	if (!v0->boundary())
		return NULL;  //the entire one-ring of v0 has been checked
	he = v0->most_ccw_in_halfedge();
	if (he->source()==v1)
		return he->edge();
	else
		return NULL;
}

Face * Mesh::idFace(const int id )
{
	stdext::hash_map<int, Face *>::iterator cpos = id2Face.find(id);
	if (cpos == id2Face.end())
		return NULL;
	else
	{
		Face * f = cpos->second;
		return f;
	}
}

Halfedge * Mesh::idHalfedge( int srcVID, int trgVID )
{

	Vertex * v0 = idVertex(srcVID);
	Vertex * v1 = idVertex(trgVID);

	Halfedge * he0 = v0->most_clw_out_halfedge();
	if(he0->target() == v1)
		return he0;

	Halfedge * he=he0->ccw_rotate_about_source();
	while (he!=he0 && he){
		if (he->target()==v1)
			return he;
		he = he->ccw_rotate_about_source();
	}

	return NULL;  //the entire one-ring of v0 has been checked
}

Vertex * Mesh::idVertex( int id ) 
{
	stdext::hash_map<int, Vertex *>::iterator cpos = id2Ver.find(id);
	return (cpos == id2Ver.end())?(NULL):(cpos->second);
}

//create new geometric simplexes

Vertex * Mesh::createVertex( int id )
{
	Vertex * v = new Vertex;	
	v->id() = id;	
	if (id > maxVid)
		maxVid = id;
	id2Ver[id] = v;
	m_verts.push_back( v );
	return v;
}

Edge * Mesh::createEdge(Halfedge * he0, Halfedge * he1)
{
	Edge * e = new Edge(he0, he1);
	m_edges.push_back( e );
	return e;
}

Face * Mesh::createFace( int id )
{
	Face * f = new Face();
	f->id() = id;
	if (maxFid < id)
		maxFid = id;
	m_faces.push_back(f);	
	return f;
}

Face * Mesh::createFace( int v[3] , int id )
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

Face * Mesh::createFace( Vertex * ver[3] , int id )
{	
	stdext::hash_map<int, Face *>::iterator cpos = id2Face.find(id);
	if (cpos != id2Face.end())
	{//Return NULL if this id already exits
		std::cerr << "Try to duplicate create a face with existing id: " << id << " !" << std::endl;
		return NULL;
	}
	
	int i;
	Face * f = createFace( id );	
	id2Face[id] = f;
	//create halfedges
	Halfedge * hes[3];
	for(i = 0; i < 3; i ++ )
	{
		hes[i] = new Halfedge;
		hes[i]->target() = ver[i];
		ver[i]->he() = hes[i];
		std::vector<Halfedge *> & adjHeList = v_adjHeList[vertIndex[ver[i]]];
		adjHeList.push_back(hes[i]);
	}
	//linking to each other, and linking to face
	for( i = 0; i < 3; i ++ )
	{
		hes[i]->next() = hes[(i+1)%3];
		hes[i]->prev() = hes[(i+2)%3];
		hes[i]->face() = f;
		f->he() = hes[i];
	}

	//connecting with edge
	for( i = 0; i < 3; i ++ )
	{
		Vertex * ev0 = ver[i]; // target
		Vertex * ev1 = ver[(i+2)%3]; // source
		Edge * e = NULL;
		std::vector<Halfedge *> & adjHeList = v_adjHeList[vertIndex[ev1]];
		for (int j=0; j<adjHeList.size(); ++j)
		{
			Halfedge * he = adjHeList[j];
			if (he->source() == ev0)
			{
				e = he->edge();
				break;
			}
		}
		if (!e)
		{
				e = new Edge();				
				m_edges.push_back(e);				
		}
		if (e->he(0))
		{
			if (e->he(1))
			{
				std::cerr << "Error: an edge appears more than twice. Non-manifold surfaces!" << std::endl;
				exit(0);
			}
			e->he(1) = hes[i];
		}
		else
			e->he(0) = hes[i];
		hes[i]->edge() = e;
	}
	return f;
}

////////////////////////////////////////////////////////////////////////
//Edge * Mesh::EdgeFlip(Edge * e)
//{
//	Halfedge * he1 = e->he(0);
//	Halfedge * he2 = e->he(1);
//	if (!he1 || !he2)
//	{
//		// this edge is on boundary, can not do flip
//		return NULL;
//	}
//	Face * f1 = he1->face();
//	Face * f2 = he2->face();
//	Halfedge * he3 = he1->next();
//	Halfedge * he4 = he3->next();
//	Halfedge * he5 = he2->next();
//	Halfedge * he6 = he5->next();
//	e->vid(0) = he3->target()->id();
//	e->vid(1) = he5->target()->id();
//	he1->target() = he3->target();
//	he2->target() = he5->target();
//	he4->next() = he5; he5->prev() = he4;
//	he5->next() = he1; he1->prev() = he5;
//	he1->next() = he4; he4->prev() = he1;
//	he3->next() = he2; he2->prev() = he3;
//	he2->next() = he6; he6->prev() = he2;
//	he6->next() = he3; he3->prev() = he6;
//	he5->face() = f1;
//	he3->face() = f2;
//	f1->halfedge() = he4;
//	f2->halfedge() = he6;
//	Halfedge * h = e->he(0);
//	if( h->target()->id() < h->source()->id() )
//	{
//		e->he(0) = e->he(1);
//		e->he(1) = h;
//	}
//	he6->target()->halfedge() = he6;
//	he4->target()->halfedge() = he4;
//	return e;
//}
//
//Vertex * Mesh::FaceSplit(Face * f, double bary[3])
//{
//	if (bary[0]<0 || bary[1]<0 || bary[2]<0)
//	{
//		// the split point is not inside the face, can't do split
//		return NULL;
//	}
//	// first we create the new vertex
//	Halfedge * he[3];
//	he[0] = f->he();
//	Vertex * v[4];	
//	v[0] = he[0]->target();
//	he[1] = he[0]->next();
//	v[1] = he[1]->target();
//	he[2] = he[1]->next();
//	v[2] = he[2]->target();
//	v[3] = createVertex( ++this->maxid );
//	v[3]->point() = (v[0]->point() * bary[0] + v[1]->point() * bary[1] + v[2]->point() * bary[2]) / (bary[0] + bary[1] + bary[2]);
//
//	int i,j;
//
//	//delete the original big face	
//	int maxfid = -1;
//	for (std::list<Face *>::iterator fiter=m_faces.begin();
//		fiter!=m_faces.end(); ++fiter)
//	{		
//		Face *f=*fiter;
//		if (f->id() > maxfid)
//			maxfid = f->id();
//	}
//
//	Halfedge * hes[3][2];
//	for(i = 0; i < 3; ++i)
//	{
//		for (j=0; j<2; ++j)
//		{
//			hes[i][j] = new Halfedge;
//		}
//		hes[i][0]->vertex() = v[3];
//		v[3]->halfedge() = hes[i][0];
//		hes[i][1]->vertex() = v[i];
//		he[i]->next() = hes[i][0];  hes[i][0]->prev() = he[i];
//	}
//
//	//linking to each other for left 6 halfedges
//	hes[0][0]->next() = hes[2][1];  hes[2][1]->prev() = hes[0][0];
//	hes[1][0]->next() = hes[0][1];  hes[0][1]->prev() = hes[1][0];
//	hes[2][0]->next() = hes[1][1];  hes[1][1]->prev() = hes[2][0];
//
//	hes[2][1]->next() = he[0]; he[0]->prev() = hes[2][1];
//	hes[0][1]->next() = he[1]; he[1]->prev() = hes[0][1];
//	hes[1][1]->next() = he[2]; he[2]->prev() = hes[1][1];
//
//	Face * f1 = createFace( ++maxfid );
//	Face * f2 = createFace( ++maxfid );
//
//	//linking to face
//	hes[0][0]->face() = hes[2][1]->face() = he[0]->face() = f;
//	hes[1][0]->face() = hes[0][1]->face() = he[1]->face() = f1;
//	hes[2][0]->face() = hes[1][1]->face() = he[2]->face() = f2;
//	f->he() = he[0];
//	f1->halfedge() = he[1];
//	f2->halfedge() = he[2];
//
//	//connecting with edge
//	for( i = 0; i < 3; i ++ )
//	{		
//		int id0 = hes[i][0]->target()->id(); 
//		int id1 = hes[i][1]->target()->id(); 
//		Edge * e = new Edge(id0, id1);
//		m_edges.push_back(e);
//		hes[i][0]->edge() = hes[i][1]->edge() = e;
//		e->he(0) = hes[i][0];
//		e->he(1) = hes[i][1];
//	}	
//
//	return v[3];
//}
//
//Vertex * Mesh::EdgeSplit( Edge * e )
//{
//
//	// (1) Initialize: Get halfedge pointer
//	Halfedge * hes[2];
//	hes[0] = e->he(0);
//	hes[1] = e->he(1);
//	int i,j,k;
//
//	//(2) Remove edge
//	std::list<Edge *>::iterator eiter = std::find(m_edges.begin(), m_edges.end(), e);
//	if (eiter == m_edges.end())
//	{
//		std::cerr << "Error: Given edge does not exists in current mesh or is not valid!" << std::endl;
//		return NULL;
//	}
//	//assert(eiter != m_edges.end());
//	m_edges.erase(eiter);	
//
//
//	//(3) Create a new vertex
//	Vertex * nv = createVertex( ++maxid );	
//	nv->boundary() = (!hes[1]);
//	nv->point() = (hes[0]->source()->point() + hes[0]->target()->point())/2;
//	
//	//(4)
//	std::list<Face *> new_faces;
//	for( j = 0; j < 2; ++j )
//	{
//		Vertex * v[3];
//		Halfedge * he = hes[j];
//
//		if( !he ) continue;
//
//		//(4.1) Find and remove the face f connected to hes[j]
//		Face * f = he->face();
//		std::list<Face *>::iterator fiter = std::find(m_faces.begin(), m_faces.end(), f);
//		m_faces.erase(fiter);
//		delete f;
//
//		//(4.2) Remove the linkings from f's halfedges to their original connected edges
//		for(i = 0; i < 3; ++i )
//		{
//			v[i] = he->target();			
//			Edge * te = he->edge();
//			if( te->he(0) == he )
//			{
//				te->he(0) = te->he(1);
//			}
//			te->he(1) = NULL;
//			if (!te->he(0))
//			{//both halfedges have been removed
//				//assert(te == e);
//				if (te!=e)
//				{
//					std::list<Edge *>::iterator teiter = 
//						std::find(m_edges.begin(), m_edges.end(), te);
//					m_edges.erase(teiter);
//					delete te;
//				}
//			}
//			he = he->next();
//		}
//
//		//(4.3) Remove all halfedges in f
//		for(k = 0; k < 3; k ++ )
//		{
//			Halfedge * ne = he->next();
//			delete he;
//			he = ne;
//		}
//		hes[j] = NULL;
//
//		//(4.4) Create new faces
//		int vid[3];
//		Vertex * w[3];
//		w[0] = nv; w[1] = v[0]; w[2] = v[1];
//		for(k = 0; k < 3; k ++ )
//		{
//			vid[k] = w[k]->id(); 
//		}
//		Face * nf = createFace( vid, ++maxfid );
//		if (!nf)
//		{
//			std::cerr << "Error: creating face " << maxfid << " fails!" <<std::endl;
//			return NULL;
//		}
//		new_faces.push_back( nf );
//
//		w[0] = nv; w[1] = v[1]; w[2] = v[2];
//		for(k = 0; k < 3; k ++ )
//		{
//			vid[k] = w[k]->id(); 
//		}	
//		nf = createFace( vid, ++maxfid );
//		if (!nf)
//		{
//			std::cerr << "Error: creating face " << maxfid << " fails!" <<std::endl;
//			return NULL;
//		}
//		new_faces.push_back( nf );
//	}
//
//	//(5) Link newly created halfedges and edges
//	std::list<Face *>::iterator nfit = new_faces.begin();
//	for(; nfit!=new_faces.end(); ++nfit)
//	{
//		Face * f = *nfit;
//		Halfedge * he = f->he();
//
//		do{
//			Edge     * e = he->edge();
//			if( e->he(1) )
//			{
//				Halfedge * h = e->he(0);
//				if( h->target()->id() < h->source()->id() )
//				{
//					e->he(0) = e->he(1);
//					e->he(1) = h;
//				}
//			}
//			he = he->next();
//		}while( he != f->he() );
//	}
//
//	delete e;
//	
//	return nv;	
//}
