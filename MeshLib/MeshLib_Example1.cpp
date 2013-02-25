#include "Core_Mesh/Mesh.h"
#include "Core_Mesh/Iterators.h"
#include <iostream>


void help()
{
	std::cout << "Please provide an input mesh name (e.g.torus.m). \n";
	exit(0);
}


int main(int argc, char ** argv)
{
	if (argc!=2) 
		help();

	Mesh * mesh1 = new Mesh;
	bool rSucc = mesh1->read(argv[1]);
	if (!rSucc)	return 0;
	Mesh cmesh;
	mesh1->copyTo(cmesh);

	bool wSucc = cmesh.write("test_output.m");
	if (!wSucc)	return 0;

	std::cout << "The following shows the usage of some iterators... \n";
	int testVerNum=0;
	for (MeshVertexIterator viter(&cmesh); !viter.end(); ++viter)
	{
		Vertex * v = *viter;
		testVerNum++;
	}
	std::cout << "total vertex # = " << testVerNum << std::endl;

	int testVerNum2=0;
	for (MeshFaceIterator fiter(&cmesh); !fiter.end(); ++fiter)
	{
		Face * f = * fiter;
		for (FaceVertexIterator fvit(f); !fvit.end(); ++fvit)
		{
			Vertex * v= *fvit;
			testVerNum2++;
		}
	}
	std::cout << "total corner # = " << testVerNum2 << " (Why?) " << std::endl;

	int testFaceNum=0;
	for (MeshFaceIterator fit(&cmesh); !fit.end(); ++fit)
	{
		testFaceNum++;
	}
	std::cout << "total face # = " << testFaceNum << std::endl;


	int testFNum=0;
	for (MeshVertexIterator vit(&cmesh); !vit.end(); ++vit)
	{
		Vertex * v = *vit;
		for (VertexFaceIterator vfit(v); !vfit.end(); ++vfit)
		{
			testFNum++;
		}
	}
	std::cout << "total face # (computed by another method): " << testFNum << " (is this # right?) " << std::endl;

	std::cout << "The following shows the usage of indexing hash table in handling properties defined on elements. \n";
	//suppose we want to compute and store on each vertex: how many neighboring edges it has
	int * nbEdges = new int[mesh1->numVertices()];
	MeshIndex m_ind(mesh1);
	if (!m_ind.vi)
	{
		//when vi=0, the index hasn't been set up yet
		m_ind.setVIndex();
		std::cout << "Now all " << m_ind.vi << " vertices are indexed. \n";
	}
	for (MeshVertexIterator vit(mesh1); !vit.end();++vit)
	{
		Vertex * v = *vit;
		int nc=0;
		for (VertexEdgeIterator veit(v); !veit.end();++veit)
		{
			Edge * e = *veit;
			nc++;
		}
		int vind = m_ind.vindex[v];
		nbEdges[vind]=nc;		
	}
	//now suppose you have a vertex * vert, its neiboring edge # = nbEdges[m_ind->vindex[vert]]
	int testprint = 10;
	int cnt=0;
	for (MeshVertexIterator vit(mesh1); !vit.end();++vit){
		Vertex * v =*vit;
		int vind = m_ind.vindex[v];
		std::cout << "This vertex (vertex id= " << v->id() << ") has " << nbEdges[vind] << " neighboring edges. \n";
		cnt++;
		if (cnt==testprint)
			break;
	}
	

	delete [] nbEdges;

	delete mesh1;
	return 1;

}