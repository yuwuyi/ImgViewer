#include "Edge.h"
#include "Trait.h"

using namespace XMeshLib;

void Edge::SetSharp()
{
	m_sharp = true;
	if (m_string.size())
	{
		std::string strsharp = "sharp";		
		if (m_string.find(strsharp)==std::string::npos)
			m_string += " sharp";
	}
	else
		m_string = "sharp";
}

void Edge::SetNotSharp()
{
	m_sharp = false;
	if (!m_string.size())
		return;
	std::string strsharp = "sharp";		
	size_t pos = m_string.find(strsharp);
	if (pos !=std::string::npos)
		m_string.erase(pos, pos+5);

}

void Edge::AddTrait(TraitNode *t)
{
	TraitList * tl = this->traits();
	if (!tl)
	{
		tl = new TraitList;
		this->traits() = tl;
	}
	tl->addTrait(t);
}

TraitNode * Edge::GetTrait(int tind)
{
	TraitList * tl = this->traits();
	if (!tl)
		return NULL;
	else
		return tl->getTrait(tind);
}

bool Edge::DelTrait(int tind)
{
	TraitList * tl = this->traits();
	if (!tl)
		return false;
	else
		return tl->delTrait(tind);
}