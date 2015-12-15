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
#include "ProceduralCollectibleSystem.h"
#include "Game.h"
#include "Actor.h"
//---------------------------------------------------------------------
#define PROCEDURAL_COLLECTIBLE_SYS_UPDATE_SLOT 0
//---------------------------------------------------------------------
void CProceduralCollectibleSystem::SProperties::InitFromScript(const IEntity& entity)
{
	IScriptTable* pScriptTable = entity.GetScriptTable();

	if(pScriptTable != NULL)
	{
		SmartScriptTable propertiesTable;
		if (pScriptTable->GetValue("Properties", propertiesTable))
		{
			propertiesTable->GetValue("bEnabled",m_enabled);
			propertiesTable->GetValue("bVisualize",m_visualize);

			SmartScriptTable spawnTable;
			if(propertiesTable->GetValue("Spawn",spawnTable))
			{
				spawnTable->GetValue("fInterval",m_interval);
				spawnTable->GetValue("fRadius",m_radius);
				spawnTable->GetValue("object_Model",m_model);
			}
		}
	}

}
//---------------------------------------------------------------------
CProceduralCollectibleSystem::CProceduralCollectibleSystem()
{
}
//---------------------------------------------------------------------
CProceduralCollectibleSystem::~CProceduralCollectibleSystem()
{
}
//---------------------------------------------------------------------
void CProceduralCollectibleSystem::SpawnSingleEntity()
{

}
//---------------------------------------------------------------------
void CProceduralCollectibleSystem::SpawnEntities()
{


}
//---------------------------------------------------------------------
void CProceduralCollectibleSystem::RemoveEntites()
{

}
//---------------------------------------------------------------------
bool CProceduralCollectibleSystem::Init( IGameObject * pGameObject )
{
	SetGameObject(pGameObject);
	return true;
}
//---------------------------------------------------------------------
void CProceduralCollectibleSystem::PostInit(IGameObject *pGameObject)
{
	Reset();

	/*SpawnSingleEntity();*/
}
//--------------------------------------------------------------------
void CProceduralCollectibleSystem::ProcessEvent( SEntityEvent &event)
{
	if (event.event == ENTITY_EVENT_XFORM)
	{

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
void CProceduralCollectibleSystem::Update( SEntityUpdateContext& ctx, int updateSlot )
{


}
//--------------------------------------------------------------------
bool CProceduralCollectibleSystem::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	ResetGameObject();

	return true;
}
//--------------------------------------------------------------------
void CProceduralCollectibleSystem::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{

}
//--------------------------------------------------------------------
bool CProceduralCollectibleSystem::GetEntityPoolSignature( TSerialize signature )
{
	return true;
}
//--------------------------------------------------------------------
void CProceduralCollectibleSystem::ActivateGeneration(bool activate)
{
	GetEntity()->Activate( activate );

	if (activate && (gEnv->IsEditor()))
	{
		if (GetGameObject()->GetUpdateSlotEnables( this, PROCEDURAL_COLLECTIBLE_SYS_UPDATE_SLOT) == 0)
		{
			GetGameObject()->EnableUpdateSlot( this, PROCEDURAL_COLLECTIBLE_SYS_UPDATE_SLOT ) ;
		}
	}
	else
	{
		GetGameObject()->DisableUpdateSlot( this, PROCEDURAL_COLLECTIBLE_SYS_UPDATE_SLOT );
	}
}
//--------------------------------------------------------------------
void CProceduralCollectibleSystem::Reset()
{

	m_ScriptsProps.InitFromScript(*GetEntity());

}
//---------------------------------------------------------------------//
void CProceduralCollectibleSystem::FullSerialize( TSerialize ser )
{
}
//---------------------------------------------------------------------//
void CProceduralCollectibleSystem::PostSerialize()
{
}
//---------------------------------------------------------------------//

