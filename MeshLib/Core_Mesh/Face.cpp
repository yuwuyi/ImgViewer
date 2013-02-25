#include "Face.h"
#include "Trait.h"

using namespace XMeshLib;

void Face::AddTrait(TraitNode *t)
{
	TraitList * tl = this->traits();
	if (!tl)
	{
		tl = new TraitList;
		this->traits() = tl;
	}
	tl->addTrait(t);
}

TraitNode * Face::GetTrait(int tind)
{
	TraitList * tl = this->traits();
	if (!tl)
		return NULL;
	else
		return tl->getTrait(tind);
}

bool Face::DelTrait(int tind)
{
	TraitList * tl = this->traits();
	if (!tl)
		return false;
	else
		return tl->delTrait(tind);
}