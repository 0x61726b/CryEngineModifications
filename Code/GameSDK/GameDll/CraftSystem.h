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
#ifndef __CraftSystem_h__
#define __CraftSystem_h__
//-------------------------------------------------------------------------

#include <IGameObject.h>
#include <IFlashUI.h>


//Move these to an xml file later
enum ECraftableItems
{
	Bush,
	Flintstone
};

enum EItemDrops
{
	Berry = 0,
	Banana,
	Durian,
	Pomegranate,
	DragonFruit,
	Watermelon
};


class ICraftable
{
public:
	ICraftable() {}
	ECraftableItems GetType() { return Type; }
	EItemDrops GetItemDropType() { return DropType;}

	void SetType(ECraftableItems type) { Type = type;}
	void SetItemDropType(EItemDrops drop) { DropType = drop;}

protected:
	ECraftableItems Type;
	EItemDrops DropType;
};


class CraftSystemListener
{
public:
	virtual void OnPickup(ICraftable* picked) = 0;
};
class CraftSystem : public IUIElementEventListener
{
public:
	CraftSystem();
	~CraftSystem();

	void OnUIEvent( IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args );

	void AddListener(CraftSystemListener* listener);
	void RemoveListener(CraftSystemListener* listener);

	void AddItem(ICraftable*);

	void RemoveItems(EItemDrops e,int cnt);

	void Reset();

	void UpdateUI();

	bool MakeItem1();

	void Craft(const string&);

	int GetCraftableCount(EItemDrops c);
	std::vector<ICraftable*> GetCraftableOfType(EItemDrops e);

	std::vector<ICraftable*> m_vTempList;

	std::vector<ICraftable*> m_vInventory;
	std::vector<CraftSystemListener*> m_vListeners;
};



//-------------------------------------------------------------------------
#endif