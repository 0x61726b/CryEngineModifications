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
- 11.12.2015 - Created

*************************************************************************/

#include "StdAfx.h"
#include "Craftable.h"
#include "Game.h"
#include "Actor.h"

#include "IEntityProxy.h"

void CCraftable::SProperties::InitFromScript(const IEntity& entity)
{
	IScriptTable* pScriptTable = entity.GetScriptTable();

	if(pScriptTable != NULL)
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			propertiesTable->GetValue("object_Model",m_Model);
			propertiesTable->GetValue("fScale",m_Scale);
		}
	}

}
//---------------------------------------------------------------------
namespace Craftable
{
	void RegisterEvents( IGameObjectExtension& goExt, IGameObject& gameObject )
	{
		const int eventID = eGFE_OnCollision;
		gameObject.UnRegisterExtForEvents( &goExt, NULL, 0 );
		gameObject.RegisterExtForEvents( &goExt, &eventID, 1 );
	}
}
CCraftable::CCraftable()
{

}
//---------------------------------------------------------------------
CCraftable::~CCraftable()
{

}
//---------------------------------------------------------------------
void CCraftable::Spawn()
{
	GetEntity()->SetScale(Vec3(m_ScriptsProps.m_Scale,m_ScriptsProps.m_Scale,m_ScriptsProps.m_Scale));
	GetEntity()->LoadGeometry(0, m_ScriptsProps.m_Model); 



	//Create trigger area
	IEntityTriggerProxy *pTriggerProxy = (IEntityTriggerProxy*)(GetEntity()->GetProxy(ENTITY_PROXY_TRIGGER));

	if(!pTriggerProxy)
	{
		GetEntity()->CreateProxy(ENTITY_PROXY_TRIGGER);
		pTriggerProxy = (IEntityTriggerProxy*)GetEntity()->GetProxy(ENTITY_PROXY_TRIGGER);
	}
	if(pTriggerProxy)
	{
		AABB boundingBox;
		GetEntity()->GetLocalBounds(boundingBox);
		pTriggerProxy->SetTriggerBounds(boundingBox);
	}
	else
	{
		gEnv->pLog->Log("%s: Warning: Trigger Area Has Bad Params", GetEntity()->GetName());
	}




}
//---------------------------------------------------------------------
bool CCraftable::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);
	Craftable::RegisterEvents(*this,*pGameObject);
	GetGameObject()->EnablePhysicsEvent(true, eEPE_OnCollisionLogged);
	return true;
}
//---------------------------------------------------------------------
void CCraftable::PostInit(IGameObject *pGameObject)
{
	Reset();

	Spawn();
}
//--------------------------------------------------------------------
void CCraftable::HandleEvent( const SGameObjectEvent& e)
{

}
//--------------------------------------------------------------------
void CCraftable::ProcessEvent( SEntityEvent &event)
{
	switch(event.event)
	{
	case ENTITY_EVENT_ENTERAREA:
		{
			IEntity * pEntity = gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0]);
			IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor((EntityId)event.nParam[0]);
			EntityId PlayerId =  gEnv->pGame->GetIGameFramework()->GetClientActorId();
			IActor* pPlayerAc = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(PlayerId);


			if(pActor == pPlayerAc)
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

				if(!pPlayer)
					return;
				CCraftableBush* bush = static_cast<CCraftableBush*>(this);
				CCraftableFlintstone* flintstone = static_cast<CCraftableFlintstone*>(this);

				bool collected = false;

				if(bush)
				{
					pPlayer->GetCraftSystem()->AddItem(bush);
					collected = true;
				} else if(flintstone)
				{
					pPlayer->GetCraftSystem()->AddItem(flintstone);
					collected = true;
				}


				if(collected)
				{
					//We're done with this entity
					gEnv->pEntitySystem->RemoveEntity(GetEntity()->GetId());
				}
			}

			break;
		}

	case ENTITY_EVENT_LEAVEAREA:
		{
			IEntity * pEntity = gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0]);
			IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor((EntityId)event.nParam[0]);
			EntityId PlayerId =  gEnv->pGame->GetIGameFramework()->GetClientActorId();
			IActor* pPlayerAc = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(PlayerId);
			if(pActor == pPlayerAc)
			{
				gEnv->pLog->Log("%s: Player Left the area", GetEntity()->GetName());         
			}

			if(pActor != pPlayerAc)
			{
				gEnv->pLog->Log("%s: Something left the area", GetEntity()->GetName());
			}
			break;
		}
	default:
		break;
	}
	if (event.event == ENTITY_EVENT_XFORM)
	{
		Reset();
		/*Spawn();*/
	}
	if(event.event == ENTITY_EVENT_RESET)
	{
		Reset();
	}
	else if (gEnv->IsEditor() && (event.event == ENTITY_EVENT_RESET))
	{
		const bool leavingGameMode = (event.nParam[0] == 0);
		if (leavingGameMode)
		{
			Reset();
		}
	}
}
//--------------------------------------------------------------------
void CCraftable::Update( SEntityUpdateContext& ctx, int updateSlot )
{

}
//--------------------------------------------------------------------
bool CCraftable::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();

	Craftable::RegisterEvents(*this,*pGameObject);

	return true;
}
//--------------------------------------------------------------------
void CCraftable::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	Craftable::RegisterEvents(*this,*pGameObject);
}
//--------------------------------------------------------------------
bool CCraftable::GetEntityPoolSignature( TSerialize signature )
{
	return true;
}
//--------------------------------------------------------------------
void CCraftable::Reset()
{
	m_ScriptsProps.InitFromScript(*GetEntity());

	GetEntity()->SetScale(Vec3(m_ScriptsProps.m_Scale,m_ScriptsProps.m_Scale,m_ScriptsProps.m_Scale)); //for some reason..
}
//---------------------------------------------------------------------//
void CCraftable::FullSerialize( TSerialize ser )
{
}
//---------------------------------------------------------------------//
void CCraftable::PostSerialize()
{
}
//---------------------------------------------------------------------//

CCraftableBush::CCraftableBush()
	: ICraftable(ECraftableItems::Bush)
{

}
CCraftableBush::~CCraftableBush()
{

}

CCraftableFlintstone::CCraftableFlintstone()
	: ICraftable(ECraftableItems::Flintstone)
{

}
CCraftableFlintstone::~CCraftableFlintstone()
{

}




//-------------------------------------------------------------------------
