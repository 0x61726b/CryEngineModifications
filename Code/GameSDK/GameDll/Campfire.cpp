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
Description: Custom Actor with particle system and proxy

-------------------------------------------------------------------------
History:
- 11.12.2015 - Created

*************************************************************************/

#include "StdAfx.h"
#include "Campfire.h"
#include "Player.h"
#include "IEntityProxy.h"
#include "UI/ArkenUIController.h"
#include "HungerSanityController.h"

void CCampfire::SProperties::InitFromScript(const IEntity& entity)
{
	IScriptTable* pScriptTable = entity.GetScriptTable();

	if(pScriptTable != NULL)
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			propertiesTable->GetValue("object_Particle",particleEffect);
			propertiesTable->GetValue("fScale",m_Scale);
			propertiesTable->GetValue("fTriggerRad",triggerRadius);
		}
	}

}

CCampfire::CCampfire()
	: m_pParticleEffect(NULL),
	m_bGenerating(false),
	m_fTime(0.0f)
{

}
//---------------------------------------------------------------------
CCampfire::~CCampfire()
{

}
//---------------------------------------------------------------------
void CCampfire::Spawn()
{

	CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

	Vec3 entityPos = GetEntity()->GetPos();

	//ESP.nFlags = ENTITY_FLAG_MODIFIED_BY_PHYSICS | ENTITY_FLAG_SPAWNED | ENTITY_FLAG_CALC_PHYSICS | ENTITY_FLAG_CLIENT_ONLY; 
	//ESP.vScale = Vec3(1,1,1);
	//ESP.vPosition = entityPos;

	/*string effect = "smoke_and_fire.fire_small.fire1";*/
	string effect = string(m_ScriptsProps.particleEffect);

	//First, cache the effect - you would normally do this elsewhere in your code
	CItemParticleEffectCache& particleCache = g_pGame->GetGameSharedParametersStorage()->GetItemResourceCache().GetParticleEffectCache();
	particleCache.CacheParticle(effect);



	//Get the effect from the cache
	IParticleEffect* pParticleEffect = particleCache.GetCachedParticle(effect);

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
		GetEntity()->LoadParticleEmitter(0,pParticleEffect);

	}



	//Create trigger area
	IEntityTriggerProxy *pTriggerProxy = (IEntityTriggerProxy*)(GetEntity()->GetProxy(ENTITY_PROXY_TRIGGER));

	if(!pTriggerProxy)
	{
		GetEntity()->CreateProxy(ENTITY_PROXY_TRIGGER);
		pTriggerProxy = (IEntityTriggerProxy*)GetEntity()->GetProxy(ENTITY_PROXY_TRIGGER);
	}
	if(pTriggerProxy)
	{
		AABB aabb(m_ScriptsProps.triggerRadius);
		pTriggerProxy->SetTriggerBounds(aabb);
	}
	else
	{
		gEnv->pLog->Log("%s: Warning: Trigger Area Has Bad Params", GetEntity()->GetName());
	}




}
//---------------------------------------------------------------------
bool CCampfire::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);

	GetGameObject()->EnablePhysicsEvent(true, eEPE_OnCollisionLogged);
	return true;
}
//---------------------------------------------------------------------
void CCampfire::PostInit(IGameObject *pGameObject)
{
	Reset();
}
//--------------------------------------------------------------------
void CCampfire::HandleEvent( const SGameObjectEvent& e)
{

}
//--------------------------------------------------------------------
void CCampfire::ProcessEvent( SEntityEvent &event)
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
				m_bGenerating = true;
				GetGameObject()->EnableUpdateSlot( this, 0 ) ;

				CryLog("%s: Player can now interact with the entity.", GetEntity()->GetName());    
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
				m_bGenerating = false;
				GetGameObject()->DisableUpdateSlot( this, 0 ) ;
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
	if(event.event == ENTITY_EVENT_START_LEVEL)
	{
		Spawn();
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
void CCampfire::Update( SEntityUpdateContext& ctx, int updateSlot )
{
	if(m_bGenerating)
	{
		m_fTime += ctx.fFrameTime;
		CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

		if(m_fTime >= 1.9f)
		{
			
			

			int oldSanity = CHungerSanityController::Get()->GetSanity();
			int newSanity = oldSanity;
			newSanity += 6;

			CHungerSanityController::Get()->SetSanity(newSanity);
			ArkenUIController::Get()->SetManaOrb(newSanity);

			CryLog("Regenareted player's Sanity from %f to %f",(float)oldSanity,(float)newSanity);

			m_fTime = 0.0f;
		}
	}
}
//--------------------------------------------------------------------
void CCampfire::PostUpdate(float frameTime ) //Not getting called
{



}
//--------------------------------------------------------------------
bool CCampfire::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();

	return true;
}
//--------------------------------------------------------------------
void CCampfire::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{

}
//--------------------------------------------------------------------
bool CCampfire::GetEntityPoolSignature( TSerialize signature )
{
	return true;
}
//--------------------------------------------------------------------
void CCampfire::Reset()
{
	m_ScriptsProps.InitFromScript(*GetEntity());

	GetEntity()->SetScale(Vec3(m_ScriptsProps.m_Scale,m_ScriptsProps.m_Scale,m_ScriptsProps.m_Scale)); //for some reason..

	Spawn();
}
//---------------------------------------------------------------------//
void CCampfire::FullSerialize( TSerialize ser )
{
}
//---------------------------------------------------------------------//
void CCampfire::PostSerialize()
{
}
//---------------------------------------------------------------------//
