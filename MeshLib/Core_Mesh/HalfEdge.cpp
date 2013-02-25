#include "HalfEdge.h"
#include "Trait.h"

using namespace XMeshLib;

HalfEdge * HalfEdge::ccw_rotate_about_target()
{
	HalfEdge * he_dual = he_sym();
	if( he_dual == NULL ) return NULL;

	return he_dual->he_prev();
}

HalfEdge * HalfEdge::clw_rotate_about_target()
{
	HalfEdge * he = he_next()->he_sym();
	return he;
}

HalfEdge * HalfEdge::ccw_rotate_about_source()
{

	HalfEdge * he = he_prev()->he_sym();
	return he;
}

HalfEdge * HalfEdge::clw_rotate_about_source()
{
	HalfEdge * he = he_sym();
	if( he == NULL ) return NULL;
	return he->he_next();
}

void HalfEdge::AddTrait(TraitNode *t)
{
	TraitList * tl = this->traits();
	if (!tl)
	{
		tl = new TraitList;
		this->traits() = tl;
	}
	tl->addTrait(t);
}

TraitNode * HalfEdge::GetTrait(int tind)
{
	TraitList * tl = this->traits();
	if (!tl)
		return NULL;
	else
		return tl->getTrait(tind);
}

bool HalfEdge::DelTrait(int tind)
{
	TraitList * tl = this->traits();
	if (!tl)
		return false;
	else
		return tl->delTrait(tind);
}