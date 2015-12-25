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
#include "PuzzleController.h"
#include "Game.h"
#include "Actor.h"
#include <ISystem.h>
#include <ICryAnimation.h>
#include <IViewSystem.h>
#include "UI/ArkenUIController.h"
void CPuzzleController::SProperties::InitFromScript(const IEntity& entity)
{
	IScriptTable* pScriptTable = entity.GetScriptTable();

	if(pScriptTable != NULL)
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			propertiesTable->GetValue("CorrectOrder",m_CorrectOrder);
		}
	}

}
CPuzzleController::CPuzzleController()
	: m_C1(false),
	m_C2(false),
	m_C3(false)
{
	
}
//---------------------------------------------------------------------
CPuzzleController::~CPuzzleController()
{
	
}
//---------------------------------------------------------------------
bool CPuzzleController::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);
	return true;
}
//---------------------------------------------------------------------
void CPuzzleController::PostInit(IGameObject *pGameObject)
{
	Reset();
}
//--------------------------------------------------------------------
void CPuzzleController::HandleEvent( const SGameObjectEvent& e)
{

}
//--------------------------------------------------------------------
void CPuzzleController::ProcessEvent( SEntityEvent &event)
{
	if(event.event == ENTITY_EVENT_LINK)
	{

		IEntityLink* link = (IEntityLink*)event.nParam[0];

		if(link != NULL)
		{
			m_vLinks.insert( std::make_pair( link->name,gEnv->pEntitySystem->GetEntity(link->entityId)) );

		}

		if(m_vLinks.size() == 4)
		{
			IEntity* trigger = NULL;
			for(std::map<string,IEntity*>::iterator It = m_vLinks.begin(); It != m_vLinks.end(); ++It)
			{
				SEntityEvent evt1;
				evt1.event = ENTITY_EVENT_CUSTOM_SETOWNER;
				evt1.nParam[0] = (INT_PTR)this->GetEntity();

				IEntity* pEntity = It->second;
				if(pEntity != NULL && It->first != "Trigger")
					pEntity->SendEvent(evt1);

				if(It->first == "Trigger")
					trigger = It->second;
			}


			//Get trigger
			if(trigger)
			{
				IEntityTriggerProxy* proxy = (IEntityTriggerProxy*)trigger->GetProxy(ENTITY_PROXY_TRIGGER);

				proxy->ForwardEventsTo( this->GetEntityId() );
			}
			else
			{
				IEntity* t = gEnv->pEntitySystem->FindEntityByName("ProximityTrigger1");

				if(t)
				{
					IEntityTriggerProxy* proxy = (IEntityTriggerProxy*)t->GetProxy(ENTITY_PROXY_TRIGGER);		

					if(proxy) proxy->ForwardEventsTo( this->GetEntityId() );
				}
			}
		}

	}
	if(event.event == ENTITY_EVENT_CUSTOM_NOTIFYOWNER)
	{
		EntityId sender = (EntityId)event.nParam[0];
		INT_PTR bStatus = event.nParam[1];

		int index = 0;
		for(std::map<string,IEntity*>::iterator It = m_vLinks.begin(); It != m_vLinks.end(); ++It)
		{
			if(It->second == gEnv->pEntitySystem->GetEntity(sender))
			{
				StateMap::iterator it = m_States.find(It->second->GetId());
				if(it == m_States.end())
				{
					m_States.insert( StateMap::value_type(It->second->GetId(),bStatus == 0 ? false : true));
					m_Orders.push_back( It->second->GetId() );
				}
				else
				{
					if(bStatus == 1)
					{
						int iAt = -1;
						for( int i=0; i < m_Orders.size(); ++i )
						{
							if(m_Orders[i] == It->second->GetId())
								iAt = i;
						}

						if(iAt == 0)
						{
							m_C1 = true;
						}
						if(iAt == 1)
						{
							if(m_C1)
							{
								m_C2 = true;
							}
							else
							{
								m_C3 = false;
							}
						}
						if(iAt == 2)
						{
							if(m_C1 && m_C2)
							{
								m_C3 = true;
							}
							else
							{
								m_C1 = false;
								m_C2 = false;
							}
						}
					}

					int firstDigit = int(m_ScriptsProps.m_CorrectOrder[0]);
					int secondDigit = int(m_ScriptsProps.m_CorrectOrder[1]);
					int thirdDigit = int(m_ScriptsProps.m_CorrectOrder[2]);



				}
				CryLog("%s controlling %s:%s",GetEntity()->GetName(),gEnv->pEntitySystem->GetEntity(sender)->GetName(),ToString((int)bStatus));
			}

		}
	}
	if(event.event == ENTITY_EVENT_RESET)
	{
		Reset();
	}

	if(event.event == ENTITY_EVENT_START_GAME)
	{
		ResetOrder();

		IEntity* t = gEnv->pEntitySystem->FindEntityByName("ProximityTrigger1");

		if(t)
		{
			IEntityTriggerProxy* proxy = (IEntityTriggerProxy*)t->GetProxy(ENTITY_PROXY_TRIGGER);		

			if(proxy) proxy->ForwardEventsTo( this->GetEntityId() );
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
			if(m_C1 && m_C2 && m_C3)
			{
				IAnimSequence *pSequence = gEnv->pMovieSystem->FindSequence("Puzzle_Solved_1");

				gEnv->pMovieSystem->AddMovieListener(pSequence,this);
				if (pSequence == NULL)
				{
					return;
				}

				IViewSystem* pViewSystem = g_pGame->GetIGameFramework()->GetIViewSystem();

				if (pViewSystem && pSequence->GetActiveDirector())
				{
					IAnimNode *pDirectorNode = pSequence->GetActiveDirector();

					float blendPosSpeed = 0;
					float blendRotSpeed = 0;
					bool performBlendOut = true;
					pViewSystem->SetBlendParams(blendPosSpeed, blendRotSpeed, performBlendOut);
				}
				gEnv->pMovieSystem->PlaySequence(pSequence,NULL,true,false);
				m_C1 = false;
				m_C2 = false;
				m_C3 = false;
			}
			else
			{
				LinkEntityMap::iterator It = m_vLinks.begin();

				SEntityEvent se;
				se.event = ENTITY_EVENT_CUSTOM_TURNOFFLIGHTS;

				It->second->SendEvent( se );
				((++It)->second )->SendEvent( se );
				( (++It)->second )->SendEvent( se );
			}
		}
	}
}
//--------------------------------------------------------------------
void CPuzzleController::OnMovieEvent(IMovieListener::EMovieEvent event, IAnimSequence* pSequence)
{
	if (event == IMovieListener::eMovieEvent_Started)
	{
		g_pGame->GetUI()->GetArkenUI()->HideAll();
	}
	if(event == IMovieListener::eMovieEvent_Stopped)
	{
		g_pGame->GetUI()->GetArkenUI()->ShowAll();

		gEnv->pMovieSystem->RemoveMovieListener(pSequence,this);
	}
}
//--------------------------------------------------------------------
void CPuzzleController::Update( SEntityUpdateContext& ctx, int updateSlot )
{

}
//--------------------------------------------------------------------
bool CPuzzleController::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();

	return true;
}
//--------------------------------------------------------------------
void CPuzzleController::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{

}
//--------------------------------------------------------------------
bool CPuzzleController::GetEntityPoolSignature( TSerialize signature )
{
	return true;
}
//--------------------------------------------------------------------
void CPuzzleController::ResetOrder()
{
	StateMap::iterator It = m_States.begin();
	m_Orders[0] = It->first;
	m_Orders[1] = (++It)->first;
	m_Orders[2] = (++It)->first;
}
void CPuzzleController::Reset()
{
	m_ScriptsProps.InitFromScript(*GetEntity());
}
//---------------------------------------------------------------------//
void CPuzzleController::FullSerialize( TSerialize ser )
{

}
//---------------------------------------------------------------------//
void CPuzzleController::PostSerialize()
{
}
//---------------------------------------------------------------------//