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
#include "Trap.h"
#include "Game.h"
#include "Actor.h"
//---------------------------------------------------------------------
void CTrap::SProperties::InitFromScript(const IEntity& entity)
{
	IScriptTable* pScriptTable = entity.GetScriptTable();

	if(pScriptTable != NULL)
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			propertiesTable->GetValue("fTriggerRad",m_fTriggerRad);
			propertiesTable->GetValue("object_TrapModel",m_TrapModel);
			propertiesTable->GetValue("audioTriggerSound",m_AudioTrigger);
			propertiesTable->GetValue("vImpulseOnImpact",m_Impulse);
		}
	}

}
//---------------------------------------------------------------------
CTrap::CTrap()
	: m_TriggerEntity(NULL)
{

}
//---------------------------------------------------------------------
CTrap::~CTrap()
{

}
//---------------------------------------------------------------------
void CTrap::Spawn()
{
	IEntity* pEntity = GetEntity();

	//Load geometry
	pEntity->LoadGeometry(0,m_ScriptsProps.m_TrapModel);


	//Create trigger area
	IEntityTriggerProxy *pTriggerProxy = (IEntityTriggerProxy*)(pEntity->GetProxy(ENTITY_PROXY_TRIGGER));

	if(!pTriggerProxy)
	{
		pEntity->CreateProxy(ENTITY_PROXY_TRIGGER);
		pTriggerProxy = (IEntityTriggerProxy*)GetEntity()->GetProxy(ENTITY_PROXY_TRIGGER);
	}
	if(pTriggerProxy)
	{
		AABB aabb(m_ScriptsProps.m_fTriggerRad);
		pTriggerProxy->SetTriggerBounds(aabb);

		m_TriggerEntity = pTriggerProxy;
	}
	else
	{
		CryLog("%s: Warning: Trigger Area Has Bad Params", GetEntity()->GetName());
	}

	//Create audio proxy
	IAudioSystem* pAudioSystem = gEnv->pAudioSystem;
	TAudioControlID id = pAudioSystem->GetAudioTriggerID(m_ScriptsProps.m_AudioTrigger,m_audioControlIDs[eSID_SwitchOff]);

	IEntityProxy* pProxy = pEntity->GetProxy(ENTITY_PROXY_AUDIO);
	if (!pProxy)
	{
		if (pEntity->CreateProxy(ENTITY_PROXY_AUDIO))
			pProxy = pEntity->GetProxy(ENTITY_PROXY_AUDIO);
	}

	m_pEntityAudioProxy = (IEntityAudioProxy*)pProxy;

	if (m_pEntityAudioProxy == 0)
		return;


	m_pAudioProxyId = m_pEntityAudioProxy->CreateAuxAudioProxy();


}
//---------------------------------------------------------------------
bool CTrap::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);





	return true;
}
//---------------------------------------------------------------------
void CTrap::PostInit(IGameObject *pGameObject)
{
	Reset();

	Spawn();



}
//--------------------------------------------------------------------
void CTrap::ProcessEvent( SEntityEvent &event)
{
	switch(event.event)
	{
	case ENTITY_EVENT_LINK:
		{
			IEntityLink* link = GetEntity()->GetEntityLinks();

			if( link != NULL)
			{
				m_pLinkedEntity = gEnv->pEntitySystem->GetEntity( link->entityId );
			}
			break;
		}
	case ENTITY_EVENT_ENTERAREA:
		{
			IEntity * pEntity = gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0]);
			IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor((EntityId)event.nParam[0]);
			EntityId PlayerId =  gEnv->pGame->GetIGameFramework()->GetClientActorId();
			IActor* pPlayerAc = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(PlayerId);

			EntityId triggerId = (EntityId)event.nParam[2];
			IEntity* pTargetEntity = gEnv->pEntitySystem->GetEntity(triggerId);

			if(pTargetEntity->GetProxy(ENTITY_PROXY_TRIGGER) != m_TriggerEntity)
			{
				//Boulder hit the player

				IEntityTriggerProxy* boulderTrigger = (IEntityTriggerProxy*)pTargetEntity->GetProxy(ENTITY_PROXY_TRIGGER);
				boulderTrigger->ForwardEventsTo( triggerId );
				
				CHungerSanityController::Get()->SetHunger(-10);
				return;
			}
			if(pActor == pPlayerAc)
			{
				if(m_pLinkedEntity)
				{
					CryLog("%s:Triggering %s",GetEntity()->GetName(),m_pLinkedEntity->GetName());

					IEntityPhysicalProxy *pPhysicsProxy = (IEntityPhysicalProxy*)m_pLinkedEntity->GetProxy(ENTITY_PROXY_PHYSICS);

					if(pPhysicsProxy)
					{
						m_pLinkedEntity->Activate(true);
						m_pLinkedEntity->Hide(false);

						m_pLinkedEntity->CreateProxy(ENTITY_PROXY_TRIGGER);
						IEntityTriggerProxy* pTriggerProxy = (IEntityTriggerProxy*)m_pLinkedEntity->GetProxy(ENTITY_PROXY_TRIGGER);

						if(pTriggerProxy)
						{
							AABB aabb(1);
							pTriggerProxy->SetTriggerBounds(aabb);
						}

						pTriggerProxy->ForwardEventsTo( GetEntity()->GetId() );

						AABB bbox;
						pPhysicsProxy->GetWorldBounds(bbox);

						Vec3 entityPos = m_pLinkedEntity->GetWorldPos();
						Vec3 trapPos   = GetEntity()->GetWorldPos();

						Vec3 dir = (entityPos - trapPos).GetNormalizedSafe();

						IPhysicalEntity* pPhysical = pPhysicsProxy->GetPhysicalEntity();



						pPhysicsProxy->AddImpulse(0,bbox.GetCenter(),m_ScriptsProps.m_Impulse,false,1);
					}
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
				CryLog("%s: Player Left the area", GetEntity()->GetName());         

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
	if (gEnv->IsEditor() && (event.event == ENTITY_EVENT_RESET))
	{
		const bool leavingGameMode = (event.nParam[0] == 0);
		if (leavingGameMode)
		{
			Reset();
		}
	}
}

//--------------------------------------------------------------------
void CTrap::Update( SEntityUpdateContext& ctx, int updateSlot )
{

}
//--------------------------------------------------------------------
bool CTrap::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();

	return true;
}
//--------------------------------------------------------------------
void CTrap::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{

}
//--------------------------------------------------------------------
bool CTrap::GetEntityPoolSignature( TSerialize signature )
{
	return true;
}
//--------------------------------------------------------------------
void CTrap::Reset()
{
	m_ScriptsProps.InitFromScript(*GetEntity());
}
//---------------------------------------------------------------------//
void CTrap::FullSerialize( TSerialize ser )
{
}
//---------------------------------------------------------------------//
void CTrap::PostSerialize()
{
}
//---------------------------------------------------------------------//

