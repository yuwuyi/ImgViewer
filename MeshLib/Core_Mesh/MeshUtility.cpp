#include "MeshUtility.h"
#include "Curve.h"
#include <sstream>
#include <iostream>
#include <map>

using namespace XMeshLib;

void MeshUtility::MarkSharpEdges(Mesh * mesh)
{
	std::string str_sharp = "sharp";
	for (MeshEdgeIterator eiter(mesh); !eiter.end(); ++eiter)
	{
		Edge * e = *eiter;
		if (TraitParser::ReadTrait(e->string(), str_sharp, 0, NULL))
			e->sharp() = true;
	}
}

void MeshUtility::ClearSharpEdges(Mesh * mesh)
{
	std::string str_sharp = "sharp";
	for (MeshEdgeIterator eiter(mesh); !eiter.end(); ++eiter)
	{
		Edge * e = *eiter;
		TraitParser::RemoveTrait(e->string(), str_sharp);
		e->SetNotSharp();
	}
}

Mesh * MeshUtility::MeshInverse(Mesh * omesh)
{
	Mesh * nmesh= new Mesh;
	MeshDelegate cdelegate(nmesh);
	for (MeshVertexIterator viter(omesh); !viter.end(); ++viter)
	{
		Vertex * v= *viter;
		Vertex * nv = cdelegate.createVertex(v->id());
		nv->point() = v->point();
		nv->boundary() = v->boundary();
		//nv->normal() = -v->normal();
		nv->string() = v->string();
	}
	cdelegate.VertexDone();
	int find =0;
	for (MeshFaceIterator fiter(omesh); !fiter.end(); ++fiter)
	{
		Face * f = *fiter;
		HalfEdge * he = f->halfedge();
		int vid[3];
		vid[0] = he->target()->id();
		vid[1] = he->source()->id();
		vid[2] = he->he_next()->target()->id();
		cdelegate.createFace(vid, find);
		find++;
	}
	cdelegate.AllDone();
	return nmesh;
}

Mesh * MeshUtility::BuildMeshFromFaceList(std::vector<Face *> flist)
{
	Mesh * nmesh = new Mesh;
	std::vector<Vertex *> nvlist;
	for (int i=0;i<flist.size(); ++i)
	{
		Face * f = flist[i];
		HalfEdge * he = f->halfedge();
		nvlist.push_back(he->source());
		nvlist.push_back(he->target());
		nvlist.push_back(he->he_next()->target());				
	}

	std::stable_sort(nvlist.begin(), nvlist.end());
	std::vector<Vertex *>::iterator newend = std::unique(nvlist.begin(), nvlist.end());
	nvlist.erase(newend, nvlist.end());

	MeshDelegate sd(nmesh);
	for (int i=0;i<nvlist.size();++i)
	{
		Vertex * ov = nvlist[i];
		Vertex * nv = sd.createVertex(ov->id());
		nv->point() = ov->point();
		nv->string() = ov->string();				
	}
	sd.VertexDone();
	for (int i=0;i<flist.size();++i)
	{
		Face * f = flist[i];
		HalfEdge * he = f->halfedge();
		int vid[3];
		vid[0] = he->source()->id();
		vid[1] = he->target()->id();
		vid[2] = he->he_next()->target()->id();
		Face * nf = sd.createFace(vid, f->id());
		//if (!nf)
		//{
		//	int faint = 0;
		//	std::vector<Face *>::iterator fiter = std::find(flist.begin(), flist.end(), f);
		//	if (fiter!=flist.end())			
		//	{
		//		std::cout << faint <<std::endl;
		//	}
		//}
	}
	sd.AllDone();
	return nmesh;
}

void MeshUtility::Elongate(Mesh * mesh, double scalex, double scaley, double scalez)
{
	for (MeshVertexIterator viter(mesh); !viter.end(); ++viter)
	{
		Vertex * ver= *viter;		
		ver->point()[0] *= scalex;
		ver->point()[1] *= scaley;
		ver->point()[2] *= scalez;
	}
}

void MeshUtility::GetMeshBoudaries(XMeshLib::Mesh *mesh, const char file[])
{
	bool hasBoundary = false;
		
	std::vector<bool> vOnBound;
	vOnBound.resize(mesh->numVertices(), false);

	MeshVertexIterator viter(mesh);
	for (; !viter.end(); ++viter)
	{
		Vertex * ver = *viter;
		if (ver->boundary())
		{
			vOnBound[mesh->id2VInd[ver->id()]] = true;
			hasBoundary = true;
		}
	}

	if (!hasBoundary)
	{
		std::cout << "There are no boundaries!" << std::endl;
		return;
	}

	int cind =0;
	while (hasBoundary)
	{
		hasBoundary = false;		
		for (viter.reset(); !viter.end(); ++viter)
		{
			Vertex * ver = *viter;
			if (vOnBound[mesh->id2VInd[ver->id()]])
			{
				hasBoundary = true;
				Curve nc;
				nc.CreateBoundaryLoop(ver);				
				vOnBound[mesh->id2VInd[ver->id()]] = false;
				for (int i=0;i<nc.heSize();++i)
				{
					Vertex * nv = nc[i]->vertex();
					vOnBound[mesh->id2VInd[nv->id()]] = false;
				}
				std::ostringstream oss;
				oss << file << "_" << cind++ << ".info";
				std::string filename(oss.str());
				nc.WriteToFile(filename.c_str());				
			}			
		}		
	}	
}

int MeshUtility::TraceAllBoundaries(Mesh * mesh, std::vector<Curve *> & bounds)
{
	std::map<Vertex *, bool> boundVer;
	for (MeshVertexIterator viter(mesh); !viter.end(); ++viter)
	{
		Vertex * ver = *viter;
		if (ver->boundary())
			boundVer[ver] = false;
	}
	bool done = false;
	while (!done)
	{
		done = true;
		std::map<Vertex *, bool>::iterator vit = boundVer.begin();
		for (; vit!=boundVer.end();++vit)
		{
			if (!vit->second)
			{
				Curve * nbound = new Curve;
				nbound->CreateBoundaryLoop(vit->first);
				bounds.push_back(nbound);
				for (int i=0;i<nbound->heSize(); ++i)
				{
					HalfEdge * he = (*nbound)[i];
					Vertex * sv = he->source();
					Vertex * tv = he->target();
					std::map<Vertex *,bool>::iterator cpos = boundVer.find(sv);
					assert(cpos != boundVer.end());
					cpos->second = true;
					cpos = boundVer.find(tv);
					assert(cpos != boundVer.end());
					cpos->second = true;
					done = false;					
				}
				break;
			}
		}
	}
	if (bounds.size()<2)
		std::cout << bounds.size() << " boundary curve has been traced!" << std::endl;
	else
		std::cout << bounds.size() << " boundary curves have been traced!" << std::endl;
	return bounds.size();
}

Point MeshUtility::ComputeMassCenter(Mesh * mesh)
{
	Point mc;
	int count=0;
	for (MeshVertexIterator viter(mesh); !viter.end(); ++viter)
	{
		Vertex * v = *viter;
		mc+= v->point();
		++count;
	}
	mc/=count;
	return mc;
}


void MeshUtility::ComputeFaceNormal(Mesh *mesh)
{
	for( MeshFaceIterator fiter( mesh ); !fiter.end(); ++ fiter )
	{
		Face * f = *fiter;
				
		Point p[3];
		int i = 0;

		for( FaceVertexIterator fviter(f); !fviter.end(); ++ fviter )
		{
			Vertex * v = *fviter;
			p[i++] = v->point();
		}
		Point n = (p[1]-p[0])^(p[2]-p[0]);
		double area = n.norm()/2;
		TraitParser::UpdateTrait(f->string(), std::string("area"), 1, &area);
		n /= n.norm();	
		TraitParser::UpdateTrait(f->string(), std::string("normal"), 3, n.v);
	}
}

void MeshUtility::ComputeVertexNormal(Mesh *mesh)
{
	for( MeshVertexIterator viter( mesh ); !viter.end(); ++ viter )
	{
		Vertex * v = *viter;

		// Calculate the vertex normal
		Point n(0,0,0);
		double area = 0;

		for(VertexFaceIterator vfiter(v); !vfiter.end(); ++ vfiter )
		{
			Face * f = *vfiter;
			Point fn;
			TraitParser::ReadTrait(f->string(), "normal", 3, fn.v);
			double s_area = 0;
			TraitParser::ReadTrait(f->string(), "area", 1, &s_area);
			n += fn * s_area;
			area += s_area;
		}

		n /= area;
		if( n.norm() > 1e-10 )
		{
			n /= n.norm();
		}

		n /= n.norm();	
		TraitParser::UpdateTrait(v->string(), std::string("normal"), 3, n.v);
	}
}