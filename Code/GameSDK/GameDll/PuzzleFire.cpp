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
#include "PuzzleFire.h"
#include "Game.h"
#include "Actor.h"
//---------------------------------------------------------------------
void CPuzzleFire::SProperties::InitFromScript(const IEntity& entity)
{
	IScriptTable* pScriptTable = entity.GetScriptTable();

	if(pScriptTable != NULL)
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			propertiesTable->GetValue("colorLight",m_Color);
			propertiesTable->GetValue("object_Model",m_Model);

			propertiesTable->GetValue("material_LightMaterial",m_Mat);
			propertiesTable->GetValue("texture_ProjTex",m_Tex);

			propertiesTable->GetValue("fZRot",m_zrot);
			propertiesTable->GetValue("fXRot",m_xrot);
			propertiesTable->GetValue("fYRot",m_yrot);

			propertiesTable->GetValue("fDiffuseMul",m_fDiffuseMul);
			propertiesTable->GetValue("fPointLightRad",m_fPointLightRad);

			propertiesTable->GetValue("fProjectorRadius",m_fradius);
			propertiesTable->GetValue("vOffset",m_Offset);

			propertiesTable->GetValue("fTriggerRad",m_fTriggerRad);
			propertiesTable->GetValue("audioTriggerSound",m_AudioTrigger);

			propertiesTable->GetValue("fProjFov",m_fProjFov);
			propertiesTable->GetValue("fProjNearPlane",m_fProjNear);

			propertiesTable->GetValue("bActive",m_bActive);
		}
	}

}
//---------------------------------------------------------------------
CPuzzleFire::CPuzzleFire()
	: m_bCanInteract(false),
	m_pPuzzleController(NULL)
{
	gEnv->pInput->AddEventListener(this);
}
//---------------------------------------------------------------------
CPuzzleFire::~CPuzzleFire()
{
	if(m_pLight)m_pLight->ReleaseNode();
	if(m_pPointLight)m_pPointLight->ReleaseNode();
	gEnv->pInput->RemoveEventListener(this);
}
//---------------------------------------------------------------------
void CPuzzleFire::Spawn()
{
	IEntity* pEntity = GetEntity();

	//Load geometry
	pEntity->LoadGeometry(0,m_ScriptsProps.m_Model);


	//Projector
	CDLight lightProps;
	lightProps.SetLightColor(ColorF(m_ScriptsProps.m_Color.x,m_ScriptsProps.m_Color.y,m_ScriptsProps.m_Color.z,0));

	int flags = FT_DONT_STREAM;
	IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(m_ScriptsProps.m_Mat,false);
	m_pLight = gEnv->p3DEngine->CreateLightSource();

	lightProps.m_pLightImage = gEnv->pRenderer->EF_LoadTexture(m_ScriptsProps.m_Tex,flags);
	lightProps.m_fProjectorNearPlane = m_ScriptsProps.m_fProjNear;
	lightProps.m_fLightFrustumAngle = m_ScriptsProps.m_fProjFov * 0.5f;
	lightProps.SetSpecularMult(3);

	lightProps.m_Flags |= DLF_PROJECT;
	lightProps.m_sName = "Hue";

	Matrix34 rotation;
	rotation.SetIdentity();
	rotation.SetRotationX(g_PI * (m_ScriptsProps.m_xrot / 180.0f) );
	rotation.SetRotationZ( g_PI * ( m_ScriptsProps.m_zrot / 180.0f));
	rotation.SetRotationY(g_PI * ( m_ScriptsProps.m_yrot / 180.0f));


	Matrix34 translation;
	translation.SetIdentity();
	translation.SetTranslation( GetEntity()->GetWorldPos()  + m_ScriptsProps.m_Offset );

	Vec3 origin = translation.GetTranslation();
	lightProps.SetPosition( origin );

	lightProps.SetMatrix(translation*rotation);
	m_pLight->SetMatrix(translation*rotation);
	lightProps.m_fRadius = m_ScriptsProps.m_fradius;
	f32 r = lightProps.m_fRadius;
	m_pLight->SetBBox(AABB(Vec3(origin.x-r, origin.y-r, origin.z-r), Vec3(origin.x+r,origin.y+r,origin.z+r)));


	m_pLight->SetLightProperties( lightProps );
	m_pLight->SetMaterial(pMaterial);


	gEnv->p3DEngine->RegisterEntity(m_pLight);




	//Second light

	CDLight pointLight;
	pointLight.SetLightColor(ColorF(m_ScriptsProps.m_Color.x,m_ScriptsProps.m_Color.y,m_ScriptsProps.m_Color.z,0));
	pointLight.SetSpecularMult(m_ScriptsProps.m_fDiffuseMul);
	pointLight.SetPosition(origin);
	pointLight.SetMatrix(translation);
	pointLight.m_fRadius = m_ScriptsProps.m_fPointLightRad;


	m_pPointLight = gEnv->p3DEngine->CreateLightSource();
	m_pPointLight->SetMatrix(translation);
	r = m_ScriptsProps.m_fPointLightRad;
	m_pPointLight->SetBBox(AABB(Vec3(origin.x-r, origin.y-r, origin.z-r), Vec3(origin.x+r,origin.y+r,origin.z+r)));

	m_pPointLight->SetLightProperties( pointLight );

	gEnv->p3DEngine->RegisterEntity(m_pPointLight);



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

	m_pPointLight->Hide( !m_ScriptsProps.m_bActive );
	m_pLight->Hide( !m_ScriptsProps.m_bActive );


	//Particle FX
	//Create particle effect
	CItemParticleEffectCache& particleCache = g_pGame->GetGameSharedParametersStorage()->GetItemResourceCache().GetParticleEffectCache();
	particleCache.CacheParticle("ArkenParticles.FX.ItemHighlight");



	//Get the effect from the cache
	IParticleEffect* pParticleEffect = particleCache.GetCachedParticle("ArkenParticles.FX.ItemHighlight");

	if(GetEntity()->GetParticleEmitter(0))
	{
		gEnv->pParticleManager->DeleteEmitter(GetEntity()->GetParticleEmitter(1));
	}

	if (pParticleEffect)
	{
		//Matrix34 loc;
		//loc.SetIdentity();
		//loc.SetTranslation(entityPos);
		////spawn the effect
		//m_pParticleEffect = pParticleEffect->Spawn(false, loc);

		GetEntity()->LoadParticleEmitter(1,pParticleEffect);
	}
}
//---------------------------------------------------------------------
bool CPuzzleFire::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);





	return true;
}
//---------------------------------------------------------------------
void CPuzzleFire::PostInit(IGameObject *pGameObject)
{
	Reset();

	Spawn();



}
//--------------------------------------------------------------------
void CPuzzleFire::ProcessEvent( SEntityEvent &event)
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

				CryLog("%s: Player can now interact with the entity.", GetEntity()->GetName());    

				m_bCanInteract = true;


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

				m_bCanInteract = false;
			}

			if(pActor != pPlayerAc)
			{
				gEnv->pLog->Log("%s: Something left the area", GetEntity()->GetName());
			}
			break;
		}
	case ENTITY_EVENT_CUSTOM_TURNOFFLIGHTS:
		{
			SwitchLights(false);
			break;
		}
	case ENTITY_EVENT_CUSTOM_TURNONLIGHTS:
		{
			SwitchLights(true);
			break;
		}
	case ENTITY_EVENT_CUSTOM_SETOWNER:
		{
			m_pPuzzleController = (IEntity*)event.nParam[0];
			if(m_pPuzzleController)
			{
				SEntityEvent event1;
				event1.event = ENTITY_EVENT_CUSTOM_NOTIFYOWNER;
				event1.nParam[0] = (INT_PTR)this->GetEntity()->GetId();
				event1.nParam[1] = (INT_PTR)m_ScriptsProps.m_bActive;
				m_pPuzzleController->SendEvent(event1);
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
void CPuzzleFire::SwitchLights(bool b)
{
	if(!b)
	{
		m_pEntityAudioProxy->ExecuteTrigger(m_audioControlIDs[eSID_SwitchOff],eLSM_None,m_pAudioProxyId);

		m_pPointLight->Hide(true);
		m_pLight->Hide(true);
		SEntityEvent evt;
		evt.event = ENTITY_EVENT_CUSTOM_NOTIFYOWNER;
		evt.nParam[0] = (INT_PTR)this->GetEntity()->GetId();
		evt.nParam[1] = (INT_PTR)false;
		if(m_pPuzzleController)m_pPuzzleController->SendEvent( evt );
	}
	else
	{
		m_pEntityAudioProxy->ExecuteTrigger(m_audioControlIDs[eSID_SwitchOff],eLSM_None,m_pAudioProxyId);

		m_pPointLight->Hide(false);
		m_pLight->Hide(false);

		SEntityEvent evt;
		evt.event = ENTITY_EVENT_CUSTOM_NOTIFYOWNER;
		evt.nParam[0] = (INT_PTR)this->GetEntity()->GetId();
		evt.nParam[1] =(INT_PTR)true;
		if(m_pPuzzleController)m_pPuzzleController->SendEvent( evt );
	}
	//CHungerSanityController::Get()->SetSanity( CHungerSanityController::Get()->GetSanity() - 10 );
}
//--------------------------------------------------------------------
void CPuzzleFire::Update( SEntityUpdateContext& ctx, int updateSlot )
{

}
//--------------------------------------------------------------------
bool CPuzzleFire::OnInputEvent( const SInputEvent &event )
{
	if(event.keyName == "F")
	{
		if(event.state  == EInputState::eIS_Released)
		{
			if(m_bCanInteract)
			{
				bool bOk = m_ScriptsProps.m_bActive;
				if(m_ScriptsProps.m_bActive)
				{
					SwitchLights(false);
					bOk = false;
				}
				else
				{
					SwitchLights(true);
					bOk = true;
				}
				m_ScriptsProps.m_bActive = bOk;
			}
		}
	}


	return false;
}
//--------------------------------------------------------------------
bool CPuzzleFire::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();

	return true;
}
//--------------------------------------------------------------------
void CPuzzleFire::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{

}
//--------------------------------------------------------------------
bool CPuzzleFire::GetEntityPoolSignature( TSerialize signature )
{
	return true;
}
//--------------------------------------------------------------------
void CPuzzleFire::Reset()
{
	m_ScriptsProps.InitFromScript(*GetEntity());
}
//---------------------------------------------------------------------//
void CPuzzleFire::FullSerialize( TSerialize ser )
{
}
//---------------------------------------------------------------------//
void CPuzzleFire::PostSerialize()
{
}
//---------------------------------------------------------------------//

