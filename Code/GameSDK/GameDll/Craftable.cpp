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
			propertiesTable->GetValue("audioTriggerPlayTrigger",m_ItemCollectSoundTriggerID);
			propertiesTable->GetValue("iDrops",m_Drop);
			propertiesTable->GetValue("iType",m_Type);
			propertiesTable->GetValue("iHealthBonus",m_HealthBonus);
			propertiesTable->GetValue("particle_Fx",m_ParticleFx);
			propertiesTable->GetValue("vParticleRot",m_ParticleRot);
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
	IEntity* pEntity = GetEntity();

	pEntity->SetScale(Vec3(m_ScriptsProps.m_Scale,m_ScriptsProps.m_Scale,m_ScriptsProps.m_Scale));
	pEntity->LoadGeometry(0, m_ScriptsProps.m_Model); 


	IAudioSystem* pAudioSystem = gEnv->pAudioSystem;
	TAudioControlID id = pAudioSystem->GetAudioTriggerID(m_ScriptsProps.m_ItemCollectSoundTriggerID,m_audioControlIDs[eSID_Collect]);

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

	//Create particle effect
	CItemParticleEffectCache& particleCache = g_pGame->GetGameSharedParametersStorage()->GetItemResourceCache().GetParticleEffectCache();
	particleCache.CacheParticle(m_ScriptsProps.m_ParticleFx);



	//Get the effect from the cache
	IParticleEffect* pParticleEffect = particleCache.GetCachedParticle(m_ScriptsProps.m_ParticleFx);

	if(GetEntity()->GetParticleEmitter(0))
	{
		gEnv->pParticleManager->DeleteEmitter(GetEntity()->GetParticleEmitter(0));
	}

	if (pParticleEffect)
	{
		//Matrix34 loc;
		//loc.SetIdentity();
		//loc.SetTranslation(entityPos);
		////spawn the effect
		//m_pParticleEffect = pParticleEffect->Spawn(false, loc);

		GetEntity()->LoadParticleEmitter(1,pParticleEffect);
		
		IParticleEmitter* pEmitter = GetEntity()->GetParticleEmitter(1);

		
		if(pEmitter)
		{
			Matrix34 loc;
			loc.SetRotationX( (float)g_PI * ( m_ScriptsProps.m_ParticleRot.x / 180.0f ));
			loc.SetRotationY( (float)g_PI * ( m_ScriptsProps.m_ParticleRot.y / 180.0f ));
			loc.SetRotationZ( (float)g_PI * ( m_ScriptsProps.m_ParticleRot.z / 180.0f ));
			GetEntity()->SetSlotLocalTM(1,loc);
		}
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

				pPlayer->GetCraftSystem()->AddItem(this);

				//We're done with this entity
				gEnv->pEntitySystem->RemoveEntity(GetEntity()->GetId());

				m_pEntityAudioProxy->ExecuteTrigger(m_audioControlIDs[eSID_Collect],eLSM_None,m_pAudioProxyId);

				/*pPlayer->SetThirdPerson(false,true);
				pPlayer->Kill();

				CryFixedStringT<256> command;
				command.Format("map %s nb", gEnv->pGame->GetIGameFramework()->GetLevelName());
				gEnv->pConsole->ExecuteString(command.c_str());*/

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

	SetItemDropType((EItemDrops)m_ScriptsProps.m_Drop);
	SetType((ECraftableItems)m_ScriptsProps.m_Type);



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




//-------------------------------------------------------------------------
