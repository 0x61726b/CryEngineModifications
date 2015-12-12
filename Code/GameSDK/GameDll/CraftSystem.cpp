/*************************************************************************

Copyright (C) 2015 Alperen Gezer

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
-------------------------------------------------------------------------
Description: Procedural Collectible Generation

-------------------------------------------------------------------------
History:
- 12.12.2015 - Created

*************************************************************************/

#include "StdAfx.h"
#include "CraftSystem.h"
//-------------------------------------------------------------------------


IProduct::IProduct()
	: m_pEntity(NULL)
{

}


IProduct::~IProduct()
{

}


std::map<ECraftableItems,int> IProduct::GetRequiredItemList()
{
	return m_vRequires;
}

namespace Products
{
	Fire::Fire()
	{

	}

	void Fire::SetRules()
	{
		m_vRequires.insert(std::make_pair(ECraftableItems::Bush,3));
		m_vRequires.insert(std::make_pair(ECraftableItems::Flintstone,1));
	}
	void Fire::SetEntity()
	{

	}
	void Fire::Spawn()
	{

	}
}

CraftSystem::CraftSystem()
{

}
CraftSystem::~CraftSystem()
{

}

void CraftSystem::AddItem(ICraftable* item)
{
	m_vInventory.push_back(item);
}

void CraftSystem::RemoveItem(ICraftable* item)
{
	std::vector<ICraftable*>::iterator It = std::find(m_vInventory.begin(),m_vInventory.end(),item);

	if(It != m_vInventory.end())
	{
		m_vInventory.erase(It);
	}
}
bool CraftSystem::HasRequiredItems(IProduct* p)
{
	IProduct::CraftingRules rules = p->GetRequiredItemList();

	IProduct::CraftingRules::iterator It = rules.begin();

	bool ok = true;
	for(It; It != rules.end(); ++It)
	{
		ECraftableItems item = It->first;
		int quantity		 = It->second;

		int count = 0;
		for(int i=0; i < m_vInventory.size(); ++i)
		{
			ICraftable* craftable = m_vInventory.at(i);

			if(craftable->GetType() == item)
				count++;
		}
		if(quantity != count)
			ok = false;
	}
	return ok;
}
void CraftSystem::TryCraft(EProducts product)
{
	switch(product)
	{
	case EProducts::Fire:
		{
			Products::Fire* fire = new Products::Fire;

			if(HasRequiredItems(fire))
				fire->Spawn();
			else
				delete fire;
		}
		break;
	default:
		break;
	}
}

int CraftSystem::GetCraftableCount(ECraftableItems c)
{
	int count = 0;
	for(int i=0; i < m_vInventory.size(); ++i)
	{
		if(m_vInventory.at(i)->GetType() == c)
			count++;
	}
	return count;
}