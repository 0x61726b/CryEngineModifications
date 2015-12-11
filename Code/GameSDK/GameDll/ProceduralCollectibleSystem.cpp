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
	SEntitySpawnParams ESP;
	ESP.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("GeomEntity");
	if(!ESP.pClass)
		return;

	Vec3 entityPos = GetEntity()->GetPos();

	ESP.nFlags = ENTITY_FLAG_MODIFIED_BY_PHYSICS | ENTITY_FLAG_SPAWNED | ENTITY_FLAG_CALC_PHYSICS | ENTITY_FLAG_CLIENT_ONLY; 
	ESP.vScale = Vec3(1,1,1);
	ESP.vPosition = entityPos;
	IEntity* pEnt = gEnv->pEntitySystem->SpawnEntity(ESP, false);
	pEnt->SetUpdatePolicy(ENTITY_UPDATE_ALWAYS);
	pEnt->LoadGeometry(0, m_ScriptsProps.m_model);

	IEntityPhysicalProxy* pPhysicalProxy = (IEntityPhysicalProxy *)pEnt->GetProxy(ENTITY_PROXY_PHYSICS);
	if (!pPhysicalProxy)
		pPhysicalProxy = (IEntityPhysicalProxy*)pEnt->CreateProxy(ENTITY_PROXY_PHYSICS).get();

	SEntityPhysicalizeParams pparams;
	pparams.type = PE_PARTICLE;//PE_RIGID;//PE_STATIC;
	pparams.nSlot = -1;
	pparams.mass = 5.f;
	pparams.density = 1.0f;
	pPhysicalProxy->Physicalize(pparams);

	pPhysicalProxy->EnablePhysics(true);

	IPhysicalEntity* pPE = pEnt->GetPhysics();
	if(pPE)
	{
		pEnt->UpdateSlotPhysics(-1);
	}

	gEnv->pEntitySystem->InitEntity(pEnt, ESP);
	m_vEntities.push_back(pEnt);
}
//---------------------------------------------------------------------
void CProceduralCollectibleSystem::SpawnEntities()
{
	int objCount = 10;


}
//---------------------------------------------------------------------
void CProceduralCollectibleSystem::RemoveEntites()
{
	EntityVector::iterator It = m_vEntities.begin();

	for(It;It != m_vEntities.end(); ++It)
	{
		gEnv->pEntitySystem->RemoveEntity((*It)->GetId());
	}
	m_vEntities.clear();
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
		RemoveEntites();
		/*SpawnSingleEntity();*/
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
	const bool debugDraw = m_ScriptsProps.m_enabled && m_ScriptsProps.m_visualize && gEnv->IsEditing() && g_pGame->DisplayEditorHelpersEnabled();
	if (debugDraw)
	{
		IRenderAuxGeom* pRenderAux = gEnv->pRenderer->GetIRenderAuxGeom();

		const ColorF colorGreen(0.0f, 1.0f, 0.0f, 0.0f);

		const Vec3 center(GetEntity()->GetWorldPos());

		float radius = m_ScriptsProps.m_radius;
		int pointCount = 360;
		float Pi = 3.14f;
		float slice = 2.0f * (Pi / pointCount);

		for(int i=0; i < pointCount;++i)
		{
			float angle = slice * i;
			float x = center.x + radius*cosf(angle);
			float y = center.y + radius*sinf(angle);

			pRenderAux->DrawPoint(Vec3(x,y,center.z),colorGreen);
		}
	}
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
	RemoveEntites();

	m_ScriptsProps.InitFromScript(*GetEntity());


	ActivateGeneration(m_ScriptsProps.m_visualize);
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

