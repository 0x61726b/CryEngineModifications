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
#include "SpellSystem.h"
#include "ItemResourceCache.h"
#include "Game.h"
#include "GameParameters.h"
#include "PlayerInput.h"

#include "CraftSystem.h"
#include "UI/ArkenUIController.h"
#include "HungerSanityController.h"

SpellSystem::SpellSystem()
	: lastSpell(NULL),
	m_mouseX(0),
	m_mouseY(0)
{
	gEnv->pHardwareMouse->AddListener(this);
	CHungerSanityController::Get()->AddListener(this);
}

SpellSystem::~SpellSystem()
{
	gEnv->pHardwareMouse->RemoveListener(this);
	CHungerSanityController::Get()->RemoveListener(this);
}

void SpellSystem::Reset()
{
	m_vActiveSpells.clear();
	lastSpell = NULL;
	m_mouseX = 0;
	m_mouseY = 0;
}

void SpellSystem::OnSpellPressed(int SpellId)
{
	/*if(SpellId == Spells::ESpells::BuildFireCamp)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

		Products::Fire* f = new Products::Fire;
		f->SetRules();
		if(pPlayer->GetCraftSystem()->HasRequiredItems(f))
		{
			BuildFireCamp* fireCamp = new BuildFireCamp(this);
			lastSpell = fireCamp;

			fireCamp->OnPress();

			pPlayer->GetCraftSystem()->RemoveItems(f);

			
		}
		delete f;


	}*/
}

void SpellSystem::OnSpellReleased(int SpellId)
{
	if(lastSpell)
		lastSpell->OnRelease();
}

void SpellSystem::OnRightClick()
{
	if(lastSpell)
	{
		lastSpell->OnSpellRelease();
	}
}

bool SpellSystem::OnLeftClick()
{
	if(lastSpell)
	{
		return lastSpell->OnSpellActivate();
	}
	return false;
}

BuildFireCamp::BuildFireCamp(SpellSystem* s)
	:
	mouseWorldPos(Vec3(0,0,0)),
	localEntity(NULL),
	m_SpellSystem(s),
	m_bPlacing(false)
{
	m_pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
}

BuildFireCamp::~BuildFireCamp()
{

}
void BuildFireCamp::OnPress()
{
	if(CHungerSanityController::Get()->GetSanity() < FIRECAMP_REQUIRED_SANITY)
	{
		g_pGame->GetUI()->GetArkenUI()->ShowNotEnough(true);
		return;
	}

	m_bPlacing = true;



	BuildEntity();
}

void BuildFireCamp::OnRelease()
{

}

void BuildFireCamp::OnSpellRelease()
{
	if(m_bPlacing)
	{
		m_bPlacing = false;
		if(localEntity)
			gEnv->pEntitySystem->RemoveEntity( localEntity->GetId() );
	}

}

bool BuildFireCamp::OnSpellActivate()
{
	if(m_bPlacing)
	{
		m_bPlacing = false;

		m_SpellSystem->m_vActiveSpells.push_back(this);

		CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());


		int currentSanity = CHungerSanityController::Get()->GetSanity();
		if(currentSanity < FIRECAMP_REQUIRED_SANITY )
		{
			m_bPlacing = false;
			return false;
		}
		currentSanity -= FIRECAMP_REQUIRED_SANITY;
		CHungerSanityController::Get()->SetSanity( currentSanity );
		g_pGame->GetUI()->GetArkenUI()->SetManaOrb(currentSanity);

		return true;
	}
	return false;
}
void BuildFireCamp::PostUpdate(float dt)
{
	if(m_bPlacing)
	{
		if(localEntity)
		{
			Vec3 pos = m_pPlayer->GetPlayerInput()->GetMouseWorldPosition();
			localEntity->SetPos( m_pPlayer->GetPlayerInput()->GetMouseWorldPosition() );
			CryLog("Moving to %f %f %f",pos.x,pos.y,pos.z);
		}
	}
}

void SpellSystem::PostUpdate(float dt)
{
	if(lastSpell)
		lastSpell->PostUpdate(dt);
}

void BuildFireCamp::BuildEntity()
{
	SEntitySpawnParams ESP;
	ESP.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Campfire");
	if(!ESP.pClass)
		return;

	CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

	Vec3 entityPos = pPlayer->GetPlayerInput()->GetMouseWorldPosition();

	ESP.nFlags = ENTITY_FLAG_MODIFIED_BY_PHYSICS | ENTITY_FLAG_SPAWNED | ENTITY_FLAG_CALC_PHYSICS | ENTITY_FLAG_CLIENT_ONLY; 
	ESP.vScale = Vec3(1,1,1);
	ESP.vPosition = entityPos;
	localEntity = gEnv->pEntitySystem->SpawnEntity(ESP, false);
	gEnv->pEntitySystem->InitEntity(localEntity,ESP);
}

void SpellSystem::OnHardwareMouseEvent(int iX,int iY,EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta )
{
	m_mouseX = iX;
	m_mouseY = iY;
}
Vec3 SpellSystem::GetMouseCoordinates()
{
	return Vec3((float)m_mouseX,(float)m_mouseY,0);
}

void SpellSystem::OnHungerChanged()
{

}

void SpellSystem::OnSanityChanged()
{
	int currentSanity = CHungerSanityController::Get()->GetSanity();

	CPlayer* pPlayer =  static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
	if( currentSanity < FIRECAMP_REQUIRED_SANITY)
	{
		
	}
	else
	{
		
	}
}