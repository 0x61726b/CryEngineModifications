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
#include "Game.h"
#include "Nodes/G2FlowBaseNode.h"
#include "Player.h"
#include "CraftSystem.h"
#include "PuzzleController.h"
//--------------------------------------------------------------------

class CFlowNode_CraftSystemInventory : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		eINP_Enable = 0,
	};

	enum EOutputPorts
	{
		eOUT_Bushes = 0,
		eOUT_Flintstones
	};

public:
	CFlowNode_CraftSystemInventory( SActivationInfo * pActInfo )
		: m_BushCount(0),
		m_FlintstoneCount(0)
	{

	}
	CFlowNode_CraftSystemInventory::~CFlowNode_CraftSystemInventory() {}

	void Serialize( SActivationInfo * pActInfo, TSerialize ser )
	{
		ser.Value("TotalBushes", m_BushCount);
		ser.Value("TotalFlintstones", m_FlintstoneCount);
	}

	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig_Void ("Enable", _HELP("Enables the craft system inventory")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig<int>("TotalBushes"),
			OutputPortConfig<int>("TotalFlintstones"),
			{0}
		};

		config.sDescription = _HELP( "Returns the inventory" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_CraftSystemInventory(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				m_BushCount = 0;
				m_FlintstoneCount = 0;
				break;
			}
		case eFE_Activate:
			{
				if (IsPortActive( pActInfo, eINP_Enable ))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

					if(!pPlayer)
						return;

					CraftSystem* crafting = pPlayer->GetCraftSystem();

					if(!crafting)
						return;

					m_BushCount = 0;
					m_FlintstoneCount = 0;

					ActivateOutput( &m_actInfo, eOUT_Bushes, m_BushCount );
					ActivateOutput( &m_actInfo, eOUT_Flintstones, m_FlintstoneCount );
				}

				break;
			}
		}
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}


	SActivationInfo m_actInfo;
	int m_BushCount;
	int m_FlintstoneCount;
};






class CFlowNode_CraftSystemPickup : public CFlowBaseNode<eNCT_Instanced>,public CraftSystemListener
{
	enum EInputPorts
	{
		eINP_Enable = 0,
		eINP_Disable
	};

	enum EOutputPorts
	{
		eOUT_onPickup = 0,
	};

public:
	CFlowNode_CraftSystemPickup( SActivationInfo * pActInfo )
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_CraftSystemPickup::~CFlowNode_CraftSystemPickup() 
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

		if(!pPlayer)
			return;

		CraftSystem* crafting = pPlayer->GetCraftSystem();

		if(!crafting)
			return;

		crafting->RemoveListener(this);
	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig_Void ("Enable", _HELP("Enables the craft system pickup event listener")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig_Void("OnPickup"),
			{0}
		};

		config.sDescription = _HELP( "Returns the inventory" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_CraftSystemPickup(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{
				if (IsPortActive( pActInfo, eINP_Enable ))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

					if(!pPlayer)
						return;

					CraftSystem* crafting = pPlayer->GetCraftSystem();

					if(!crafting)
						return;

					crafting->AddListener(this);
				}
				if (IsPortActive( pActInfo, eINP_Disable ))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

					if(!pPlayer)
						return;

					CraftSystem* crafting = pPlayer->GetCraftSystem();

					if(!crafting)
						return;

					crafting->RemoveListener(this);
				}
				break;
			}
		}
	}

	virtual void OnPickup(ICraftable* picked)
	{
		ActivateOutput(&m_actInfo, eOUT_onPickup, true );
	}

	SActivationInfo m_actInfo;
};

////////////////////////////////



class CFlowNode_HungerEvents : public CFlowBaseNode<eNCT_Instanced>,public IHungerSanityEventListener
{
	enum EInputPorts
	{
		eINP_Enable = 0,
		eINP_Disable
	};

	enum EOutputPorts
	{
		eOUT_OnHungerReachZero = 0,
		eOUT_OnSanityReachZero,
		eOUT_OnStartSleeping,
		eOUT_OnAwakeFromSleep
	};

public:
	CFlowNode_HungerEvents( SActivationInfo * pActInfo )
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_HungerEvents::~CFlowNode_HungerEvents() 
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

		if(!pPlayer)
			return;

		CHungerSanityController* hungerSystem = CHungerSanityController::Get();

		if(!hungerSystem)
			return;

		hungerSystem->RemoveListener(this);


	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig_Void ("Enable", _HELP("Enables the event listener")),
			InputPortConfig_Void ("Disable", _HELP("Disable the event listener")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig_Void("OnHungerReachZero"),
			OutputPortConfig_Void("OnSanityReachZero"),
			OutputPortConfig_Void("OnStartedSleeping"),
			OutputPortConfig_Void("OnAwakeFromSleep"),
			{0}
		};

		config.sDescription = _HELP( "" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_HungerEvents(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{
				if (IsPortActive( pActInfo, eINP_Enable ))
				{
					CHungerSanityController* hungerSystem = CHungerSanityController::Get();

					if(!hungerSystem)
						return;

					hungerSystem->AddListener(this);

				}
				if (IsPortActive( pActInfo, eINP_Disable ))
				{
					CHungerSanityController* hungerSystem = CHungerSanityController::Get();

					if(!hungerSystem)
						return;

					hungerSystem->RemoveListener(this);
				}
				break;
			}
		}
	}

	virtual void OnHungerChanged()
	{
		CHungerSanityController* hungerSystem = CHungerSanityController::Get();

		if(hungerSystem->GetHunger() <= 0)
		{
			ActivateOutput(&m_actInfo, eOUT_OnHungerReachZero, true );
		}
	}

	virtual void OnSanityChanged()
	{
		CHungerSanityController* hungerSystem = CHungerSanityController::Get();

		if(hungerSystem->GetSanity() <= 0)
		{
			ActivateOutput(&m_actInfo, eOUT_OnSanityReachZero, true );
		}
	}

	virtual void OnAwake()
	{
		CHungerSanityController* hungerSystem = CHungerSanityController::Get();


		ActivateOutput(&m_actInfo, eOUT_OnAwakeFromSleep, true );

	}

	virtual void OnStartSleeping()
	{
		CHungerSanityController* hungerSystem = CHungerSanityController::Get();


		ActivateOutput(&m_actInfo, eOUT_OnStartSleeping, true );
	}

	SActivationInfo m_actInfo;
};

class CFlowNode_SetThirdPerson : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		eINP_Set = 0,
	};

	enum EOutputPorts
	{

	};

public:
	CFlowNode_SetThirdPerson( SActivationInfo * pActInfo )
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_SetThirdPerson::~CFlowNode_SetThirdPerson() 
	{

	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig_Void ("Set", _HELP("Set player third person mode")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			{0}
		};

		config.sDescription = _HELP( "" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_SetThirdPerson(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{
				if (IsPortActive( pActInfo, eINP_Set ))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

					pPlayer->SetThirdPerson(true,true);
				}
				break;
			}
		}
	}


	SActivationInfo m_actInfo;
};


class CFlowNode_KillPlayer : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		eINP_Set = 0,
	};

	enum EOutputPorts
	{

	};

public:
	CFlowNode_KillPlayer( SActivationInfo * pActInfo )
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_KillPlayer::~CFlowNode_KillPlayer() 
	{

	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig_Void ("Set", _HELP("Set player third person mode")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			{0}
		};

		config.sDescription = _HELP( "" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_KillPlayer(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{
				if (IsPortActive( pActInfo, eINP_Set ))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

					pPlayer->SetThirdPerson(false,true);
					pPlayer->Kill();
				}
				break;
			}
		}
	}


	SActivationInfo m_actInfo;
};

class CFlowNode_SetHungerSanity : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		eINP_Set,
		eINP_Hunger,
		eINP_Sanity
	};

	enum EOutputPorts
	{
		eOUT_OnHungerChanged,
		eOUT_OnSanityChanged
	};

public:
	CFlowNode_SetHungerSanity( SActivationInfo * pActInfo )
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_SetHungerSanity::~CFlowNode_SetHungerSanity() 
	{

	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig_Void("Set", _HELP("Set hunger")),
			InputPortConfig<int>("Hunger", _HELP("Set hunger")),
			InputPortConfig<int>("Sanity", _HELP("Set sanity")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig_Void("OnHungerChanged"),
			OutputPortConfig_Void("OnSanityChanged"),
			{0}
		};

		config.sDescription = _HELP( "" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_SetHungerSanity(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{
				if (IsPortActive( pActInfo, eINP_Set ))
				{

					CHungerSanityController::Get()->SetHunger(GetPortInt( pActInfo,eINP_Hunger ));

					ActivateOutput( pActInfo,eOUT_OnHungerChanged,true);

					CHungerSanityController::Get()->SetSanity(GetPortInt( pActInfo,eINP_Sanity ));

					ActivateOutput( pActInfo,eOUT_OnSanityChanged,true);
				}
				break;
			}
		}
	}


	SActivationInfo m_actInfo;
};


class CFlowNode_ArkenUIHelper : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		eINP_HideAll = 0,
		eINP_ShowAll,
		eINP_OnlyOverlay
	};

	enum EOutputPorts
	{

	};

public:
	CFlowNode_ArkenUIHelper( SActivationInfo * pActInfo )
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_ArkenUIHelper::~CFlowNode_ArkenUIHelper() 
	{

	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig_Void ("HideAll", _HELP("")),
			InputPortConfig_Void ("ShowAll", _HELP("")),
			InputPortConfig_Void ("Show only overlay", _HELP("")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			{0}
		};

		config.sDescription = _HELP( "" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_ArkenUIHelper(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
				if (IsPortActive( pActInfo, eINP_ShowAll ))
				{
					g_pGame->GetUI()->GetArkenUI()->ShowAll();
				}
				if (IsPortActive( pActInfo, eINP_HideAll ))
				{
					g_pGame->GetUI()->GetArkenUI()->HideAll();
				}
				if (IsPortActive( pActInfo, eINP_OnlyOverlay ))
				{
					g_pGame->GetUI()->GetArkenUI()->ShowOnlyOverlay();
				}
				break;
			}
		}
	}


	SActivationInfo m_actInfo;
};

class CFlowNode_PuzzleListener : public CFlowBaseNode<eNCT_Instanced>,public CPuzzleControllerEventListener
{
	enum EInputPorts
	{
		eINP_PuzzleController
	};

	enum EOutputPorts
	{
		eOUT_On,
		eOUT_Off
	};

public:
	CFlowNode_PuzzleListener( SActivationInfo * pActInfo )
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_PuzzleListener::~CFlowNode_PuzzleListener() 
	{


	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig<EntityId>("PuzzleController", _HELP("")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig_Void("On",_HELP("")),
			OutputPortConfig_Void("Off",_HELP("")),
			{0}
		};

		config.sDescription = _HELP( "" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_PuzzleListener(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{
				IEntity* puzzleCtrl = gEnv->pEntitySystem->GetEntity( GetPortEntityId(pActInfo,eINP_PuzzleController) );
				m_Puzzle = GetPortEntityId(pActInfo,eINP_PuzzleController);
				if(puzzleCtrl)
				{
					SEntityEvent evt;
					evt.event = ENTITY_EVENT_ADD_PZL_LISTENER;
					evt.nParam[0] = (INT_PTR)(CPuzzleControllerEventListener*)this; //Wow....
					puzzleCtrl->SendEvent(evt);
				}
			}
		}
	}

	virtual void TriggerOn()
	{
		ActivateOutput( &m_actInfo, eOUT_On, true );
	}
	virtual void TriggerOff()
	{
		ActivateOutput( &m_actInfo, eOUT_Off, true );
	}


	SActivationInfo m_actInfo;
	EntityId m_Puzzle;
};


class CFlowNode_RopeBoulderCollision : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{

		eINP_Enable = 0,
		eINP_Disable,
		eINP_Boulder ,

	};

	enum EOutputPorts
	{

	};

public:
	CFlowNode_RopeBoulderCollision( SActivationInfo * pActInfo )
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_RopeBoulderCollision::~CFlowNode_RopeBoulderCollision() 
	{
		for(auto iter=g_listeners.begin(); iter!=g_listeners.end(); )
			if (iter->second->pNode==this)
			{
				delete iter->second;
				g_listeners.erase(iter++);
			}
			else iter++;
	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig_Void ("Enable", _HELP("")),
			InputPortConfig_Void ("Disable", _HELP("")),
			InputPortConfig<EntityId> ("Boulder", _HELP("")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			{0}
		};

		config.sDescription = _HELP( "" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_RopeBoulderCollision(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{
				IEntity* pent;
				if (IsPortActive( pActInfo, eINP_Enable ))
				{
					SCollListener cl;
					cl.id = GetPortEntityId(pActInfo, eINP_Boulder);
					cl.pNode = this;
					if (g_listeners.find(cl.id)==g_listeners.end() && (pent=gEnv->pEntitySystem->GetEntity(cl.id)) && pent->GetPhysics())
					{
						if (g_listeners.empty())
							gEnv->pPhysicalWorld->AddEventClient(EventPhysCollision::id, (int(*)(const EventPhys*))OnCollision, 1);

						g_listeners.insert(std::pair<EntityId,SCollListener*>(cl.id,new SCollListener(cl)));
					}
				}
				if (IsPortActive( pActInfo, eINP_Disable ))
				{
					std::map<EntityId,SCollListener*>::iterator iter;

					if((iter=g_listeners.find(GetPortEntityId(pActInfo,eINP_Boulder)))!=g_listeners.end())
					{
						delete iter->second;
						g_listeners.erase(iter);
						if (g_listeners.empty())
							gEnv->pPhysicalWorld->RemoveEventClient(EventPhysCollision::id, (int(*)(const EventPhys*))OnCollision, 1);
					}

				}
				break;
			}
		}
	}
	static int OnCollision(const EventPhysCollision *pColl)
	{
		IEntity *pTarget0 = pColl->iForeignData[0]==PHYS_FOREIGN_ID_ENTITY ? (IEntity*)pColl->pForeignData[0]:nullptr;
		IEntity *pTarget1 = pColl->iForeignData[1]==PHYS_FOREIGN_ID_ENTITY ? (IEntity*)pColl->pForeignData[1]:nullptr;

		if(pTarget0 && pTarget1)
		{
			if(pTarget0->GetId() == g_pGame->GetIGameFramework()->GetClientActorId())
			{
				std::map<EntityId,SCollListener*>::iterator iter = g_listeners.find( pTarget1->GetId() );

				if(iter != g_listeners.end())
				{
					CryLog("Entity0: %i, Entity1: %i", pTarget0->GetId(),pTarget1->GetId());
					CHungerSanityController::Get()->SetHunger(-10);

					CActor* pLocalActor = static_cast<CActor*>(g_pGame->GetIGameFramework()->GetClientActor());
					if (pLocalActor)
					{
						const float backwardsImpulse = 1;
						pLocalActor->KnockDown(backwardsImpulse);


					}

					delete iter->second;
					g_listeners.erase(iter);
					if (g_listeners.empty())
						gEnv->pPhysicalWorld->RemoveEventClient(EventPhysCollision::id, (int(*)(const EventPhys*))OnCollision, 1);


				}
			}

		}

		return 1;
	}


	struct SCollListener {
		EntityId id;
		CFlowNode_RopeBoulderCollision *pNode;
	};
	static std::map<EntityId,SCollListener*> g_listeners;
	SActivationInfo m_actInfo;
};

std::map<EntityId,CFlowNode_RopeBoulderCollision::SCollListener*> CFlowNode_RopeBoulderCollision::g_listeners;


class CFlowNode_SetupPlayerListener : public CFlowBaseNode<eNCT_Instanced>,public IEntityEventListener
{
	enum EInputPorts
	{
		eINP_Set
	};

	enum EOutputPorts
	{

	};

public:
	CFlowNode_SetupPlayerListener( SActivationInfo * pActInfo )
		: m_pAudioListener(NULL)
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_SetupPlayerListener::~CFlowNode_SetupPlayerListener() 
	{

	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig_Void("Set", _HELP("Set")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			{0}
		};

		config.sDescription = _HELP( "" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_SetupPlayerListener(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;

				pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, true);
				break;
			}
		case eFE_Activate:
			{
				if (IsPortActive( pActInfo, eINP_Set ))
				{
					CreateAudioListener();
				}
				break;
			}
		case eFE_Update:
			{
				CActor* pLocalActor = static_cast<CActor*>(g_pGame->GetIGameFramework()->GetClientActor());

				if(pLocalActor)
				{
					UpdateAudioListener(pLocalActor->GetEntity()->GetWorldTM());
				}
				break;
			}
		}
	}

	void CreateAudioListener()
	{
		if (m_pAudioListener == nullptr)
		{
			SEntitySpawnParams oEntitySpawnParams;
			oEntitySpawnParams.sName  = "AudioListener";
			oEntitySpawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("AudioListener");
			m_pAudioListener = gEnv->pEntitySystem->SpawnEntity(oEntitySpawnParams, true);

			if (m_pAudioListener != nullptr)
			{
				// We don't want the audio listener to serialize as the entity gets completely removed and recreated during save/load!
				m_pAudioListener->SetFlags(m_pAudioListener->GetFlags() | (ENTITY_FLAG_TRIGGER_AREAS | ENTITY_FLAG_NO_SAVE));
				m_pAudioListener->SetFlagsExtended(m_pAudioListener->GetFlagsExtended() | ENTITY_FLAG_EXTENDED_AUDIO_LISTENER);
				gEnv->pEntitySystem->AddEntityEventListener(m_pAudioListener->GetId(), ENTITY_EVENT_DONE, this);
				CryFixedStringT<64> sTemp;
				sTemp.Format("AudioListener(%d)", static_cast<int>(m_pAudioListener->GetId()));
				m_pAudioListener->SetName(sTemp.c_str());

				IEntityAudioProxyPtr pIEntityAudioProxy = crycomponent_cast<IEntityAudioProxyPtr>(m_pAudioListener->CreateProxy(ENTITY_PROXY_AUDIO));
				CRY_ASSERT(pIEntityAudioProxy.get());
			}
			else
			{
				CryFatalError("<Audio>: Audio listener creation failed in CView::CreateAudioListener!");
			}
		}
		else
		{
			m_pAudioListener->SetFlagsExtended(m_pAudioListener->GetFlagsExtended() | ENTITY_FLAG_EXTENDED_AUDIO_LISTENER);
			m_pAudioListener->InvalidateTM(ENTITY_XFORM_POS);
		}
	}

	void UpdateAudioListener(Matrix34 const& matrix)
	{
		if (m_pAudioListener != nullptr)
		{
			m_pAudioListener->SetWorldTM(matrix);
		}
	}

	void OnEntityEvent( IEntity *pEntity,SEntityEvent &event )
	{
		switch (event.event)
		{
		case ENTITY_EVENT_DONE:
			{
				// In case something destroys our listener entity before we had the chance to remove it.
				if ((m_pAudioListener != nullptr) && (pEntity->GetId() == m_pAudioListener->GetId()))
				{
					gEnv->pEntitySystem->RemoveEntityEventListener(m_pAudioListener->GetId(), ENTITY_EVENT_DONE, this);
					m_pAudioListener = nullptr;
				}

				break;
			}
		default:
			{
				break;
			}
		}
	}
	IEntity* m_pAudioListener;
	SActivationInfo m_actInfo;
};


class CFlowNode_RotaterActivator : public CFlowBaseNode<eNCT_Instanced>
{
	enum EInputPorts
	{
		eINP_Rotator,
		eINP_Activate,
		eINP_Disable
	};

	enum EOutputPorts
	{

	};

public:
	CFlowNode_RotaterActivator( SActivationInfo * pActInfo )
	{

	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	CFlowNode_RotaterActivator::~CFlowNode_RotaterActivator() 
	{

	}


	virtual void GetConfiguration( SFlowNodeConfig &config )
	{
		static const SInputPortConfig inp_config[] = {
			InputPortConfig<EntityId> ("Rotator", _HELP("")),
			InputPortConfig_Void ("Activate", _HELP("")),
			InputPortConfig_Void ("Disable", _HELP("")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			{0}
		};

		config.sDescription = _HELP( "" );
		config.pInputPorts = inp_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}


	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) { return new CFlowNode_ArkenUIHelper(pActInfo); }


	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				m_actInfo = *pActInfo;
				break;
			}
		case eFE_Activate:
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
				if (IsPortActive( pActInfo, eINP_Rotator ))
				{
					m_Rotator = GetPortEntityId(pActInfo,eINP_Rotator);
				}
				if (IsPortActive( pActInfo, eINP_Activate ))
				{
					if(m_Rotator)
					{
						IEntity* pRotator = gEnv->pEntitySystem->GetEntity( m_Rotator );

						if(pRotator)
						{
							SEntityEvent evt1;
							evt1.event = ENTITY_EVENT_ACTIVATE_WHEN_NEAR;
							evt1.nParam[0] = 1;
							pRotator->SendEvent(evt1);
						}
					}
				}
				if (IsPortActive( pActInfo, eINP_Disable))
				{
					if(m_Rotator)
					{
						IEntity* pRotator = gEnv->pEntitySystem->GetEntity( m_Rotator );

						if(pRotator)
						{
							SEntityEvent evt1;
							evt1.event = ENTITY_EVENT_ACTIVATE_WHEN_NEAR;
							evt1.nParam[0] = 0;
							pRotator->SendEvent(evt1);
						}
					}
				}
				break;
			}
		}
	}


	SActivationInfo m_actInfo;
	EntityId m_Rotator;
};


REGISTER_FLOW_NODE( "Crafting:Inventory", CFlowNode_CraftSystemInventory );
REGISTER_FLOW_NODE( "Crafting:Pickup", CFlowNode_CraftSystemPickup );
REGISTER_FLOW_NODE( "HungerSystem:HungerEvents", CFlowNode_HungerEvents );
REGISTER_FLOW_NODE( "Arken:SetThirdPerson", CFlowNode_SetThirdPerson );
REGISTER_FLOW_NODE( "Arken:KillPlayer", CFlowNode_KillPlayer );
REGISTER_FLOW_NODE( "Arken:SetHungerSanity", CFlowNode_SetHungerSanity);
REGISTER_FLOW_NODE( "Arken:ArkenUIHelper", CFlowNode_ArkenUIHelper);
REGISTER_FLOW_NODE( "Arken:RopeBoulderCollision", CFlowNode_RopeBoulderCollision);
REGISTER_FLOW_NODE( "Arken:SetupPlayerAudio", CFlowNode_SetupPlayerListener);
REGISTER_FLOW_NODE( "Arken:PuzzleListener", CFlowNode_PuzzleListener);
REGISTER_FLOW_NODE( "Arken:RotatorActivator", CFlowNode_RotaterActivator);
//--------------------------------------------------------------------