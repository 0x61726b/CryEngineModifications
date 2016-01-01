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
			propertiesTable->GetValue("AnimSeq",m_AnimSeq);
			propertiesTable->GetValue("Trigger",m_TriggerName);
			propertiesTable->GetValue("Preset",Preset);
		}
	}

}
CPuzzleController::CPuzzleController()
	: m_C1(false),
	m_C2(false),
	m_C3(false),
	m_bPlayedCutscene(false)
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

		//IEntityLink* link = (IEntityLink*)event.nParam[0];

		//if(link != NULL)
		//{
		//	m_vLinks.insert( std::make_pair( link->name,gEnv->pEntitySystem->GetEntity(link->entityId)) );

		//}

		//if(m_vLinks.size() == 4)
		//{
		//	IEntity* trigger = NULL;
		//	for(std::map<string,IEntity*>::iterator It = m_vLinks.begin(); It != m_vLinks.end(); ++It)
		//	{
		//		SEntityEvent evt1;
		//		evt1.event = ENTITY_EVENT_CUSTOM_SETOWNER;
		//		evt1.nParam[0] = (INT_PTR)this->GetEntity();

		//		IEntity* pEntity = It->second;
		//		if(pEntity != NULL && It->first != "Trigger")
		//			pEntity->SendEvent(evt1);

		//		if(It->first == "Trigger")
		//			trigger = It->second;
		//	}


		//	//Get trigger
		//	if(trigger)
		//	{
		//		IEntityTriggerProxy* proxy = (IEntityTriggerProxy*)trigger->GetProxy(ENTITY_PROXY_TRIGGER);

		//		proxy->ForwardEventsTo( this->GetEntityId() );
		//	}
		//	else
		//	{
		//		IEntity* t = gEnv->pEntitySystem->FindEntityByName(m_ScriptsProps.m_TriggerName);

		//		if(t)
		//		{
		//			IEntityTriggerProxy* proxy = (IEntityTriggerProxy*)t->GetProxy(ENTITY_PROXY_TRIGGER);		

		//			if(proxy) proxy->ForwardEventsTo( this->GetEntityId() );
		//		}
		//	}
		//}

	}
	if(event.event == ENTITY_EVENT_ACTIVATED)
	{
		m_bPlayedCutscene = false;
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

					it->second = bStatus == 0 ? false : true;
					if(m_ScriptsProps.Preset == 0)
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

					}
					if(m_ScriptsProps.Preset == 1)
					{

						bool all_open = true;
						StateMap::iterator states_it = m_States.begin();

						for(states_it;states_it != m_States.end(); ++states_it)
						{
							if(states_it->second == false)
							{
								all_open = false;
							}
						}

						if(all_open)
						{
							CryLog("Yeah");
							m_C1 = true;
							m_C2 = true;
							m_C3 = true;
						}
					}

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

		IEntity* t = gEnv->pEntitySystem->FindEntityByName(m_ScriptsProps.m_TriggerName);

		if(t)
		{
			IEntityTriggerProxy* proxy = (IEntityTriggerProxy*)t->GetProxy(ENTITY_PROXY_TRIGGER);		

			if(proxy) proxy->ForwardEventsTo( this->GetEntityId() );
		}

		//links
		IEntityLink* link = GetEntity()->GetEntityLinks();

		if(link != NULL)
		{
			m_vLinks.clear();
			IEntityLink* nextLink = link;
			while(nextLink != NULL)
			{
				m_vLinks.insert( std::make_pair( nextLink->name,gEnv->pEntitySystem->GetEntity(nextLink->entityId)) );
				nextLink = nextLink->next;
			}

			if(nextLink != NULL)
			{
				m_vLinks.insert( std::make_pair( nextLink->name,gEnv->pEntitySystem->GetEntity(nextLink->entityId)) );
			}
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
				IEntity* t = gEnv->pEntitySystem->FindEntityByName(m_ScriptsProps.m_TriggerName);

				if(t)
				{
					IEntityTriggerProxy* proxy = (IEntityTriggerProxy*)t->GetProxy(ENTITY_PROXY_TRIGGER);		

					if(proxy) proxy->ForwardEventsTo( this->GetEntityId() );
				}
			}
		}
		m_C1 = false;
		m_C2 = false;
		m_C3 = false;
	}
	if(event.event == ENTITY_EVENT_ADD_PZL_LISTENER)
	{
		AddListener( (CPuzzleControllerEventListener*)event.nParam[0] );
	}
	if(event.event == ENTITY_EVENT_REMOVE_PZL_LISTENER)
	{
		RemoveListener( (CPuzzleControllerEventListener*)event.nParam[0] );
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
				IAnimSequence *pSequence = gEnv->pMovieSystem->FindSequence(m_ScriptsProps.m_AnimSeq);

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
				if(!m_bPlayedCutscene)
					gEnv->pMovieSystem->PlaySequence(pSequence,NULL,true,false);
				m_C1 = false;
				m_C2 = false;
				m_C3 = false;


				m_bPlayedCutscene = true;
			}
			else
			{
				string str1 = "Let there be light.";
				string str2 = "We will flame the light.";
				string str3 = "You are not the chosen one.";
				string str4 = "You are the chosen one.";

				int cryRandom = cry_random<int>(0,4);

				if(cryRandom == 0)
				{
					g_pGame->GetUI()->GetArkenUI()->ShowText(str1);
				}
				if(cryRandom == 1)
				{
					g_pGame->GetUI()->GetArkenUI()->ShowText(str2);
				}
				if(cryRandom == 2)
				{
					g_pGame->GetUI()->GetArkenUI()->ShowText(str3);
				}
				if(cryRandom == 3)
				{
					g_pGame->GetUI()->GetArkenUI()->ShowText(str4);
				}
				//wHY THE HELL DID I DO THIS LOL
				//LinkEntityMap::iterator It = m_vLinks.begin();

				//SEntityEvent se;
				//se.event = ENTITY_EVENT_CUSTOM_TURNOFFLIGHTS;

				//It->second->SendEvent( se );
				//((++It)->second )->SendEvent( se );
				//( (++It)->second )->SendEvent( se );
			}
			for(int i=0; i < m_vListeners.size(); ++i )
			{
				bool all_closed = true;
				StateMap::iterator states_it = m_States.begin();

				for(states_it;states_it != m_States.end(); ++states_it)
				{
					if(states_it->second == true)
					{
						all_closed = false;
					}
				}
				if(all_closed)
				{
					m_vListeners[i]->TriggerOff();
				}
				else
				{
					m_vListeners[i]->TriggerOn();
				}
			}
		}
	}
}
//--------------------------------------------------------------------
void CPuzzleController::AddListener(CPuzzleControllerEventListener* listener)
{
	std::vector<CPuzzleControllerEventListener*>::iterator It = std::find(m_vListeners.begin(),m_vListeners.end(),listener);

	if(It == m_vListeners.end())
		m_vListeners.push_back(listener);
}
//--------------------------------------------------------------------
void CPuzzleController::RemoveListener(CPuzzleControllerEventListener* listener)
{
	std::vector<CPuzzleControllerEventListener*>::iterator It = std::find(m_vListeners.begin(),m_vListeners.end(),listener);

	if(It != m_vListeners.end())
	{
		m_vListeners.erase(It);
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

	if(m_States.size() > 0)
	{
		m_Orders[0] = It->first;
		m_Orders[1] = (++It)->first;
		m_Orders[2] = (++It)->first;
	}

}
void CPuzzleController::Reset()
{
	m_ScriptsProps.InitFromScript(*GetEntity());

	m_bPlayedCutscene = false;


	if(m_ScriptsProps.Preset == 1)
	{
		for(std::map<string,IEntity*>::iterator It = m_vLinks.begin(); It != m_vLinks.end(); ++It)
		{
			SEntityEvent evt1;
			evt1.event = ENTITY_EVENT_CUSTOM_TURNOFFLIGHTS;
			evt1.nParam[0] = (INT_PTR)this->GetEntity();

			IEntity* pEntity = It->second;
			if(pEntity != NULL && It->first != "Trigger")
				pEntity->SendEvent(evt1);
		}
	}

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