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
Description: Controls Flash UI,calls ActionScript

-------------------------------------------------------------------------
History:
- 14.12.2015 - Created

*************************************************************************/

#include "StdAfx.h"
#include "ArkenUIController.h"
#include "CraftSystem.h"

ArkenUIController* gArkenUIInstance = NULL;

ArkenUIController::ArkenUIController()
	: m_pArkenUI(NULL),
	m_pSkillbar(NULL),
	obj2(true)
{
	gArkenUIInstance = this;

	if ( gEnv->pFlashUI )
	{
		m_pArkenUI = gEnv->pFlashUI->GetUIElement( "ArkenUI" );
		m_pSkillbar = gEnv->pFlashUI->GetUIElement( "ArkenSkillbar" );
		m_pCraftMenu = gEnv->pFlashUI->GetUIElement( "ArkenCraftMenu" );
		if ( m_pArkenUI )
		{
			m_pArkenUI->AddEventListener( this, "ArkenUI" );
			m_pSkillbar->AddEventListener( this, "ArkenSkillbar" );
			m_pCraftMenu->AddEventListener( this, "ArkenCraftMenu" );

		}
	}
}

ArkenUIController::~ArkenUIController()
{

	if(m_pArkenUI)m_pArkenUI->RemoveEventListener(this);
	if(m_pSkillbar)m_pSkillbar->RemoveEventListener(this);
	if(m_pCraftMenu)m_pCraftMenu->RemoveEventListener(this);
}

ArkenUIController* ArkenUIController::Get()
{
	return gArkenUIInstance;
}
void ArkenUIController::BerryText(int v)
{
	SUIArguments args;
	args.AddArgument( v );

	TUIData res;

	m_pArkenUI->CallFunction( "BerryText",args,&res );
}

//////////////

void ArkenUIController::BananaText(int v)
{
	SUIArguments args;
	args.AddArgument( v );

	TUIData res;

	m_pArkenUI->CallFunction( "BananaText",args,&res );
}

//////////////

void ArkenUIController::DurianText(int v)
{
	SUIArguments args;
	args.AddArgument( v );

	TUIData res;

	m_pArkenUI->CallFunction( "DurianText",args,&res );
}

//////////////

void ArkenUIController::WatermelonText(int v)
{
	SUIArguments args;
	args.AddArgument( v );

	TUIData res;

	m_pArkenUI->CallFunction( "WatermelonText",args,&res );
}

//////////////

void ArkenUIController::PomegranateText(int v)
{
	SUIArguments args;
	args.AddArgument( v );

	TUIData res;

	m_pArkenUI->CallFunction( "PomegranateText",args,&res );
}

//////////////

void ArkenUIController::DragonFruitText(int v)
{
	SUIArguments args;
	args.AddArgument( v );

	TUIData res;

	m_pArkenUI->CallFunction( "DragonFruitText",args,&res );
}

//////////////
void ArkenUIController::SetHealthOrb(int percent)
{
	SUIArguments args;
	args.AddArgument( percent );

	TUIData res;

	m_pSkillbar->CallFunction( "SetHunger",args,&res );
}

void ArkenUIController::SetManaOrb(int percent)
{
	SUIArguments args;
	args.AddArgument( percent );

	TUIData res;

	m_pSkillbar->CallFunction( "SetSanity",args,&res );
}

void ArkenUIController::EnableSkillOne(bool status)
{
	SUIArguments args;
	args.AddArgument( status );

	TUIData res;

	m_pSkillbar->CallFunction( "SetSkillOne",args,&res );
}

void ArkenUIController::EnableHungerWarning(bool status)
{
	SUIArguments args;
	args.AddArgument( status );

	TUIData res;

	m_pArkenUI->CallFunction( "EnableHungerWarning",args,&res );
}

void ArkenUIController::ShowNotEnough(bool status)
{
	SUIArguments args;
	args.AddArgument( status );

	TUIData res;

	m_pArkenUI->CallFunction( "ShowNotEnough",args,&res );
}

void ArkenUIController::SetObjectiveOne(int p11,int p21,bool p3,bool p4)
{

	SUIArguments args;
	args.AddArgument( p11 );
	args.AddArgument( 3 );
	args.AddArgument( p21 );
	args.AddArgument( 1 );
	args.AddArgument( p3 );
	args.AddArgument( p4 );


	TUIData res;

	m_pArkenUI->CallFunction( "SetObjectiveOne",args,&res );



}

void ArkenUIController::SetObjectiveTwo(bool p1,bool p4)
{

	SUIArguments args;
	args.AddArgument( p1 );
	args.AddArgument( p4 );



	TUIData res;

	m_pArkenUI->CallFunction( "SetObjectiveTwo",args,&res );

	SetObjectiveOne(0,0,false,false);


}
void ArkenUIController::OnUIEvent( IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args )
{
	if(strcmp(event.sName,"onButtonClicked") == 0)
	{
		/*CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
		pPlayer->GetPlayerInput()->SetIgnoreMouseInput(false);*/

		TUIData list = args.GetArg(0);
		string str;
		list.GetValueWithConversion<string>(str);

		if(strcmp(str,"First") == 0)
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

<<<<<<< HEAD
			pPlayer->GetCraftSystem()->Craft("Pie");
		}
		int x = 0;
	}
	if(event.sDisplayName== "OnButtonRollOver")
	{
		//CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
		//pPlayer->GetPlayerInput()->SetIgnoreMouseInput(true);
	}
}
=======
}
>>>>>>> 0df81d51f9a3aad1a29c2f3f5f31608a01916d3c
