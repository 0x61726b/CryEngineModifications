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
- 3.12.2015 - Created

*************************************************************************/
#include "StdAfx.h"
#include "CraftableCollectible.h"
#include "Game.h"
#include "Actor.h"
//---------------------------------------------------------------------

//---------------------------------------------------------------------
void CCraftableCollectible::SProperties::InitFromScript(const IEntity& entity)
{
	IScriptTable* pScriptTable = entity.GetScriptTable();

	if(pScriptTable != NULL)
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			propertiesTable->GetValue("bEnabled",m_enabled);

			SmartScriptTable spawnTable;
			if(propertiesTable->GetValue("Spawn",spawnTable))
			{
				spawnTable->GetValue("fileModel",m_model);
			}
		}
	}

}
//---------------------------------------------------------------------
CCraftableCollectible::CCraftableCollectible()
{
}
//---------------------------------------------------------------------
CCraftableCollectible::~CCraftableCollectible()
{
}
//---------------------------------------------------------------------
void CCraftableCollectible::SpawnSingleEntity()
{
	SEntitySpawnParams ESP;
	ESP.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("BasicEntity");
	if(!ESP.pClass)
		return;

	Vec3 entityPos = GetEntity()->GetPos();

	ESP.nFlags = ENTITY_FLAG_MODIFIED_BY_PHYSICS | ENTITY_FLAG_SPAWNED | ENTITY_FLAG_CALC_PHYSICS | ENTITY_FLAG_CLIENT_ONLY; 
	ESP.vScale = Vec3(1,1,1);
	ESP.vPosition = entityPos;
	IEntity* pEnt = gEnv->pEntitySystem->SpawnEntity(ESP, false);
	pEnt->SetUpdatePolicy(ENTITY_UPDATE_ALWAYS);
	pEnt->LoadGeometry(0, m_ScriptsProps.m_model);

	IEntityPhysicalProxy* pPhysicalProxy = (IEntityPhysicalProxy *)pEnt->GetProxy(ENTITY_PROXY_PHYSICS);
	if (!pPhysicalProxy)
		pPhysicalProxy = (IEntityPhysicalProxy*)pEnt->CreateProxy(ENTITY_PROXY_PHYSICS).get();

	SEntityPhysicalizeParams pparams;
	pparams.type = PE_RIGID;//PE_RIGID;//PE_STATIC;
	pparams.nSlot = 0;
	pparams.mass = 5.f;
	pparams.density = 1.0f;

	pPhysicalProxy->Physicalize(pparams);

	pe_params_flags flags;
	flags.flagsOR = pef_monitor_collisions | pef_log_collisions | pef_traceable;


	pPhysicalProxy->EnablePhysics(true);

	IPhysicalEntity* pPE = pEnt->GetPhysics();
	if(pPE)
	{
		pPE->SetParams(&flags);
	}

	gEnv->pEntitySystem->InitEntity(pEnt, ESP);

	GetGameObject()->EnablePhysicsEvent(true, eEPE_OnCollisionLogged);
	GetGameObject()->EnablePhysicsEvent(true, eEPE_OnCollisionImmediate);

	m_vEntities.push_back(pEnt);
}
//---------------------------------------------------------------------
void CCraftableCollectible::RemoveEntites()
{
	EntityVector::iterator It = m_vEntities.begin();

	for(It;It != m_vEntities.end(); ++It)
	{
		gEnv->pEntitySystem->RemoveEntity((*It)->GetId());
	}
	m_vEntities.clear();
}
//---------------------------------------------------------------------
bool CCraftableCollectible::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);
	return true;
}
//---------------------------------------------------------------------
void CCraftableCollectible::PostInit(IGameObject *pGameObject)
{
	Reset();

	SpawnSingleEntity();
}
//--------------------------------------------------------------------
void CCraftableCollectible::ProcessEvent( SEntityEvent &event)
{
	if (event.event == ENTITY_EVENT_XFORM)
	{
		RemoveEntites();
		SpawnSingleEntity();
	}
	else if (gEnv->IsEditor() && (event.event == ENTITY_EVENT_RESET))
	{
		const bool leavingGameMode = (event.nParam[0] == 0);
		if (leavingGameMode)
		{
			Reset();
		}
	}
	if(event.event == ENTITY_EVENT_COLLISION)
	{
		EventPhysCollision *pCollision = (EventPhysCollision *)event.nParam[0];
	}
}
//--------------------------------------------------------------------
void CCraftableCollectible::HandleEvent( const SGameObjectEvent &event )
{
	if(event.event == eGFE_OnCollision)
	{
		EventPhysCollision *pCollision = (EventPhysCollision *)event.ptr;

		IEntity *pTarget0 = pCollision->iForeignData[0]==PHYS_FOREIGN_ID_ENTITY ? (IEntity*)pCollision->pForeignData[0]:nullptr;
		IEntity *pTarget1 = pCollision->iForeignData[1]==PHYS_FOREIGN_ID_ENTITY ? (IEntity*)pCollision->pForeignData[1]:nullptr;
	} else if (event.event == eEPE_OnCollisionImmediate)
	{
		EventPhysCollision *pCollision = (EventPhysCollision *)event.ptr;

	}
}
//--------------------------------------------------------------------
void CCraftableCollectible::Update( SEntityUpdateContext& ctx, int updateSlot )
{
}
//--------------------------------------------------------------------
bool CCraftableCollectible::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();

	return true;
}
//--------------------------------------------------------------------
void CCraftableCollectible::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{

}
//--------------------------------------------------------------------
bool CCraftableCollectible::GetEntityPoolSignature( TSerialize signature )
{
	return true;
}
//--------------------------------------------------------------------
void CCraftableCollectible::ActivateGeneration(bool activate)
{
	GetEntity()->Activate( activate );

	if (activate && (gEnv->IsEditor()))
	{
		if (GetGameObject()->GetUpdateSlotEnables( this, 0) == 0)
		{
			GetGameObject()->EnableUpdateSlot( this, 0 ) ;
		}
	}
	else
	{
		GetGameObject()->DisableUpdateSlot( this, 0 );
	}
}
//--------------------------------------------------------------------
void CCraftableCollectible::Reset()
{
	RemoveEntites();

	m_ScriptsProps.InitFromScript(*GetEntity());

	SpawnSingleEntity();

	ActivateGeneration(m_ScriptsProps.m_enabled);
}
//---------------------------------------------------------------------//
void CCraftableCollectible::FullSerialize( TSerialize ser )
{
}
//---------------------------------------------------------------------//
void CCraftableCollectible::PostSerialize()
{
}
//---------------------------------------------------------------------//

