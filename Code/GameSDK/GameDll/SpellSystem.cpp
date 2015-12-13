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
#include "SpellSystem.h"
#include "ItemResourceCache.h"
#include "Game.h"
#include "GameParameters.h"

SpellSystem::SpellSystem()
{

}

SpellSystem::~SpellSystem()
{

}

void SpellSystem::Reset()
{

}

void SpellSystem::OnSpellPressed(int SpellId)
{
	if(SpellId == Spells::ESpells::BuildFireCamp)
	{
		BuildFireCamp* fireCamp = new BuildFireCamp;
		lastSpell = fireCamp;

		fireCamp->OnPress();
	}
}

void SpellSystem::OnSpellReleased(int SpellId)
{
	if(lastSpell)
		lastSpell->OnRelease();
}
BuildFireCamp::BuildFireCamp()
	: m_mouseX(0),
	m_mouseY(0),
	mouseWorldPos(Vec3(0,0,0)),
	localEntity(NULL)
{
	//tHERE IS NO TIME TO GET AN EVENT BEFORE THE UPDATE TICK. FIX PLZ
	gEnv->pHardwareMouse->AddListener(this);
}

BuildFireCamp::~BuildFireCamp()
{
	gEnv->pHardwareMouse->RemoveListener(this);
}
void BuildFireCamp::OnPress()
{
	BuildEntity();
}

void BuildFireCamp::OnRelease()
{

}

void BuildFireCamp::Update()
{

}

void BuildFireCamp::BuildEntity()
{
	//SEntitySpawnParams ESP;
	//ESP.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("BasicEntity");
	//if(!ESP.pClass)
	//	return;

	CalculateMouseWorldPos();
	Vec3 entityPos = mouseWorldPos;

	//ESP.nFlags = ENTITY_FLAG_MODIFIED_BY_PHYSICS | ENTITY_FLAG_SPAWNED | ENTITY_FLAG_CALC_PHYSICS | ENTITY_FLAG_CLIENT_ONLY; 
	//ESP.vScale = Vec3(1,1,1);
	//ESP.vPosition = entityPos;

	string effect = "smoke_and_fire.fire_small.fire1";

	//First, cache the effect - you would normally do this elsewhere in your code
	CItemParticleEffectCache& particleCache = g_pGame->GetGameSharedParametersStorage()->GetItemResourceCache().GetParticleEffectCache();
	particleCache.CacheParticle(effect);


	//Get the effect from the cache
	IParticleEffect* pParticleEffect = particleCache.GetCachedParticle(effect);

	if (pParticleEffect)
	{
		Matrix34 loc;
		loc.SetIdentity();
		loc.SetTranslation(entityPos);
		//spawn the effect
		IParticleEmitter* pEffect = pParticleEffect->Spawn(false, loc);
	}
	//JB: end spawn a particle effect
}


void BuildFireCamp::CalculateMouseWorldPos()
{
	//Calculate destination
	int invMouseY = gEnv->pRenderer->GetHeight() - m_mouseY;

	Vec3 vPos0(0,0,0);
	gEnv->pRenderer->UnProjectFromScreen((float)m_mouseX, (float)invMouseY, 0, &vPos0.x, &vPos0.y, &vPos0.z);

	Vec3 vPos1(0,0,0);
	gEnv->pRenderer->UnProjectFromScreen((float)m_mouseX, (float)invMouseY, 1, &vPos1.x, &vPos1.y, &vPos1.z);

	Vec3 vDir = vPos1 - vPos0;
	vDir.Normalize();


	ray_hit hit;
	const unsigned int flags = rwi_stop_at_pierceable|rwi_colltype_any;

	if (gEnv->pPhysicalWorld && gEnv->pPhysicalWorld->RayWorldIntersection(vPos0, vDir *  gEnv->p3DEngine->GetMaxViewDistance(), ent_terrain, flags, &hit, 1))
	{
		mouseWorldPos = hit.pt;
		if(hit.pCollider)
		{
			if(IEntity *pEntity = gEnv->pEntitySystem->GetEntityFromPhysics(hit.pCollider))
			{
				/*IGeomCacheRenderNode* node = pEntity->GetGeomCacheRenderNode(0);*/



			}

		}
	}
}
void BuildFireCamp::OnHardwareMouseEvent(int iX,int iY,EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta )
{
	m_mouseX = iX;
	m_mouseY = iY;
}