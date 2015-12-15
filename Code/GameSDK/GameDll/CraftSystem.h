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


enum ECraftableItems
{
	Bush,
	Flintstone
};
enum EProducts
{
	Fire
};


class ICraftable
{
public:
	ICraftable(ECraftableItems type) : Type(type) {}
	ECraftableItems GetType() { return Type; }

private:
	ECraftableItems Type;
};

class IProduct
{
public:
	IProduct();
	virtual ~IProduct();

	typedef std::map<ECraftableItems,int> CraftingRules;
	virtual void SetRules() = 0;
	CraftingRules GetRequiredItemList();

	virtual IEntity* GetEntity() { return m_pEntity;}
	virtual void SetEntity() = 0;

	virtual void Spawn() = 0;
protected:
	CraftingRules m_vRequires;
	IEntity* m_pEntity;
};

namespace Products
{
	class Fire : public IProduct
	{
	public:
		Fire();
		~Fire() {}

		void SetRules();
		void SetEntity();
		void Spawn();
	};
}
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
	void RemoveItem(ICraftable*);

	void RemoveItems(IProduct*);

	void Reset();

	void UpdateUI();

	void TryCraft(EProducts product);
	bool HasRequiredItems(IProduct*);

	int GetCraftableCount(ECraftableItems c);

	std::vector<ICraftable*> m_vInventory;
	std::vector<CraftSystemListener*> m_vListeners;
};



//-------------------------------------------------------------------------
#endif