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
#include "BoulderRotater.h"
#include "Game.h"
#include "Actor.h"
//---------------------------------------------------------------------
void CBoulderRotater::SProperties::InitFromScript(const IEntity& entity)
{
	IScriptTable* pScriptTable = entity.GetScriptTable();

	if(pScriptTable != NULL)
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			propertiesTable->GetValue("bAutoTurn",m_AutoTurn);
			propertiesTable->GetValue("fRotateFor",m_fRotateFor);
			propertiesTable->GetValue("fRotateInterval",m_fRotateInterval);
		}
	}

}
//---------------------------------------------------------------------
CBoulderRotater::CBoulderRotater()
	: m_pRotater( NULL ),
	m_bTriggered(false),
	m_bAutoTurn(false),
	m_fTimer(0.0f),
	m_fTurnInterval(4.0f),
	m_fRotatedAngle(0.0f)
{
}
//---------------------------------------------------------------------
CBoulderRotater::~CBoulderRotater()
{
}
//---------------------------------------------------------------------
bool CBoulderRotater::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);
	return true;
}
//---------------------------------------------------------------------
void CBoulderRotater::PostInit(IGameObject *pGameObject)
{
	Reset();
}
//--------------------------------------------------------------------
void CBoulderRotater::ProcessEvent( SEntityEvent &event)
{
	if (event.event == ENTITY_EVENT_XFORM)
	{

	}
	if(event.event == ENTITY_EVENT_START_GAME)
	{
		IEntityLink* link = GetEntity()->GetEntityLinks();

		if(link != NULL)
		{
			while(link != NULL)
			{
				m_LinkMap.insert( std::make_pair( link->name,gEnv->pEntitySystem->GetEntity(link->entityId)) );
				link = link->next;
			}
			m_pRotater = m_LinkMap.find("Rotater")->second;
			m_pTrigger = m_LinkMap.find("Trigger")->second;

			if(m_pRotater)
			{
				GetEntity()->Activate( true );

				GetGameObject()->EnableUpdateSlot(this,0);
			}
			if(m_pTrigger)
			{
				IEntityTriggerProxy* proxy = (IEntityTriggerProxy*)m_pTrigger->GetProxy(ENTITY_PROXY_TRIGGER);

				proxy->ForwardEventsTo( this->GetEntityId() );
			}
		}
	}
	if(event.event == ENTITY_EVENT_ENTERAREA)
	{
		IEntity * pEntity = gEnv->pEntitySystem->GetEntity((EntityId)event.nParam[0]);
		IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor((EntityId)event.nParam[0]);
		EntityId PlayerId =  gEnv->pGame->GetIGameFramework()->GetClientActorId();
		IActor* pPlayerAc = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(PlayerId);

		if(pActor == pPlayerAc)
		{
			m_bTriggered = true;
		}
	}

	else if (gEnv->IsEditor() && (event.event == ENTITY_EVENT_RESET))
	{
		const bool leavingGameMode = (event.nParam[0] == 0);
		if (leavingGameMode)
		{
			Reset();

			GetEntity()->Activate( false );

			GetGameObject()->DisableUpdateSlot(this,0);
		}
	}
}
//--------------------------------------------------------------------
void CBoulderRotater::AutoRotate()
{
	float dt = gEnv->pTimer->GetFrameTime();

	if(m_fTimer > m_fTurnInterval)
	{
		Ang3 currRot = m_pRotater->GetWorldAngles();
		currRot.z += DEG2RAD( 0.3f );

		m_pRotater->SetRotation( Quat( currRot ) );
		m_fRotatedAngle += 0.3f;

		if(m_fRotatedAngle >= script.m_fRotateFor)
		{
			m_fTimer= 0.0f;
			m_fRotatedAngle = 0.0f;
		}

	}
	else
	{
		m_fTimer += dt;
	}

}
//--------------------------------------------------------------------
void CBoulderRotater::Update( SEntityUpdateContext& ctx, int updateSlot )
{
	if(script.m_AutoTurn)
	{
		AutoRotate();
	}
	else
	{
		if(m_bTriggered)
		{

			Ang3 currRot = m_pRotater->GetWorldAngles();
			currRot.z += DEG2RAD( 0.3f );

			m_pRotater->SetRotation( Quat( currRot ) );
			m_fRotatedAngle += 0.3f;

			if(m_fRotatedAngle >= script.m_fRotateFor)
			{
				m_fTimer= 0.0f;
				m_fRotatedAngle = 0.0f;
				m_bTriggered = false;
			}



		}
	}

}
//--------------------------------------------------------------------
bool CBoulderRotater::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();

	return true;
}
//--------------------------------------------------------------------
void CBoulderRotater::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{

}
//--------------------------------------------------------------------
bool CBoulderRotater::GetEntityPoolSignature( TSerialize signature )
{
	return true;
}
//--------------------------------------------------------------------
void CBoulderRotater::Reset()
{

	script.InitFromScript(*GetEntity());

	m_fTimer = 0.0f;

	m_fRotatedAngle = 0.0f;
}
//---------------------------------------------------------------------//
void CBoulderRotater::FullSerialize( TSerialize ser )
{
}
//---------------------------------------------------------------------//
void CBoulderRotater::PostSerialize()
{
}
//---------------------------------------------------------------------//

