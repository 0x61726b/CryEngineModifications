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
		eOUT_OnSanityReachZero
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
REGISTER_FLOW_NODE( "Crafting:Inventory", CFlowNode_CraftSystemInventory );
REGISTER_FLOW_NODE( "Crafting:Pickup", CFlowNode_CraftSystemPickup );
REGISTER_FLOW_NODE( "HungerSystem:HungerEvents", CFlowNode_HungerEvents );
REGISTER_FLOW_NODE( "Arken:SetThirdPerson", CFlowNode_SetThirdPerson );
REGISTER_FLOW_NODE( "Arken:KillPlayer", CFlowNode_KillPlayer );
//--------------------------------------------------------------------