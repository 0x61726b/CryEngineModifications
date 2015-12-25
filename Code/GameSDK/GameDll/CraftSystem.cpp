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
#include "UI/ArkenUIController.h"
//-------------------------------------------------------------------------

CraftSystem::CraftSystem()
{
	if ( gEnv->pFlashUI )
	{
		IUIElement* pHUD = gEnv->pFlashUI->GetUIElement( "ArkenUI" );
		if ( pHUD )
		{
			pHUD->AddEventListener( this, "ArkenUI" );

		}
	}
}
CraftSystem::~CraftSystem()
{
	IUIElement* pHUD = gEnv->pFlashUI->GetUIElement( "ArkenUI" );
	pHUD->RemoveEventListener(this);
}

void CraftSystem::AddItem(ICraftable* item)
{
	/*
	Why creating a new instance?
	Because when the player picks the item,its instance will be deleted via entity system and the pointer will point to NULL
	which will lead to crash when trying to access it
	*/
	ICraftable* copy = new ICraftable;
	copy->SetItemDropType(item->GetItemDropType());
	copy->SetType(item->GetType());

	m_vInventory.push_back(copy);
	m_vInventory.push_back(copy);

	for(int i=0; i < m_vListeners.size(); ++i)
		m_vListeners.at(i)->OnPickup(copy);

	//Update UI
	UpdateUI();
}

bool CraftSystem::MakeItem1()
{
	//First item on the menu
	//Pie
	EItemDrops requiredItem1 = EItemDrops::Berry;
	int		   Quantity1	 = 1;
	int		   RegensFor	 = 20;

	int iOk = GetCraftableCount(requiredItem1);

	if(iOk >= Quantity1)
	{
		RemoveItems(requiredItem1,Quantity1);

		CHungerSanityController::Get()->SetHunger( CHungerSanityController::Get()->GetHunger() + RegensFor);
	}

	return iOk == Quantity1;
}

bool CraftSystem::MakeItem2()
{
	//First item on the menu
	//Pie
	EItemDrops requiredItem1 = EItemDrops::Banana;
	int		   Quantity1	 = 1;
	int		   RegensFor	 = 30;

	int iOk = GetCraftableCount(requiredItem1);

	if(iOk >= Quantity1)
	{
		RemoveItems(requiredItem1,Quantity1);

		CHungerSanityController::Get()->SetHunger( CHungerSanityController::Get()->GetHunger() + RegensFor);
	}

	return iOk == Quantity1;
}

bool CraftSystem::MakeItem3()
{
	//First item on the menu
	//Pie
	EItemDrops requiredItem1 = EItemDrops::Banana;
	EItemDrops requiredItem2 = EItemDrops::Berry;

	int		   Quantity1	 = 1;
	int		   Quantity2	 = 1;

	int		   RegensFor	 = 45;

	int iOk = GetCraftableCount(requiredItem1);
	int iOk2 = GetCraftableCount(requiredItem2);
	if(iOk >= Quantity1 && iOk2 >= Quantity2)
	{
		RemoveItems(requiredItem1,Quantity1);
		RemoveItems(requiredItem2,Quantity2);

		CHungerSanityController::Get()->SetHunger( CHungerSanityController::Get()->GetHunger() + RegensFor);
	}

	return iOk == Quantity1 && iOk2 == Quantity2;
}

void CraftSystem::Craft(const string& str)
{
	if(strcmp(str,"Pie") == 0)
	{

		int i = cry_random<int>(0,2);

		if(i == 0)
		{
			MakeItem1();
			
		}
		if(i == 1)
		{
			MakeItem2();
		}
		if(i == 2)
		{
			MakeItem3();
		}
	}
}


std::vector<ICraftable*> CraftSystem::GetCraftableOfType(EItemDrops e)
{
	m_vTempList.clear();
	for(int i=0; i < m_vInventory.size(); ++i)
	{
		if(m_vInventory.at(i)->GetItemDropType() == e)
		{
			m_vTempList.push_back(m_vInventory[i]);
		}
	}
	return m_vTempList;
}


int CraftSystem::GetCraftableCount(EItemDrops c)
{
	int count = 0;
	for(int i=0; i < m_vInventory.size(); ++i)
	{
		if(m_vInventory.at(i)->GetItemDropType() == c)
		{
			count++;
		}

	}
	return count;
}

void CraftSystem::AddListener(CraftSystemListener* listener)
{
	std::vector<CraftSystemListener*>::iterator It = std::find(m_vListeners.begin(),m_vListeners.end(),listener);

	if(It == m_vListeners.end())
		m_vListeners.push_back(listener);
}

void CraftSystem::RemoveListener(CraftSystemListener* listener)
{
	std::vector<CraftSystemListener*>::iterator It = std::find(m_vListeners.begin(),m_vListeners.end(),listener);

	if(It != m_vListeners.end())
	{
		m_vListeners.erase(It);
	}
}

void CraftSystem::RemoveItems(EItemDrops e,int count)
{
	std::vector<int> temp;

	std::vector<ICraftable*>::iterator It;

	int c = 0;
	for(It = m_vInventory.begin();It != m_vInventory.end();)
	{
		if(c >= count)
			break;;
		if((*It)->GetItemDropType() == e)
		{
			It = m_vInventory.erase(It);
			c++;
		}
		else
		{
			++It;
		}
	}


	UpdateUI();
}

void CraftSystem::OnUIEvent( IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args )
{
	if ( event.sDisplayName == "OnFireButton" )
	{

	}
	else
	{
		// event is not "OnButton1" event
	}
}

void CraftSystem::Reset()
{
	m_vListeners.clear();
	m_vInventory.clear();
}

void CraftSystem::UpdateUI()
{
	if( gEnv->pFlashUI )
	{
		//Basic UI of displaying quantity of items. See ArkenUI.xml
		IUIElement* pArkenUI = gEnv->pFlashUI->GetUIElement( "ArkenUI" );

		if( pArkenUI )
		{
			int qBerry = GetCraftableCount(EItemDrops::Berry);
			int qBanana = GetCraftableCount(EItemDrops::Banana);
			int qDragonFruit = GetCraftableCount(EItemDrops::DragonFruit);
			int qDurian = GetCraftableCount(EItemDrops::Durian);
			int qPomegranate = GetCraftableCount(EItemDrops::Pomegranate);
			int qWatermelon = GetCraftableCount(EItemDrops::Watermelon);

			CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
			ArkenUIController* pArkenUI = g_pGame->GetUI()->GetArkenUI();
			pArkenUI->BerryText(qBerry);
			pArkenUI->BananaText(qBanana);
			pArkenUI->DragonFruitText(qDragonFruit);
			pArkenUI->DurianText(qDurian);
			pArkenUI->PomegranateText(qPomegranate);
			pArkenUI->WatermelonText(qWatermelon);
		}
	}
}