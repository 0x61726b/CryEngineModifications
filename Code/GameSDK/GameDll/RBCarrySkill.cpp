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
- 3.1.2016 - Created

*************************************************************************/

#include "StdAfx.h"
#include "RBCarrySkill.h"

RBCarry::RBCarry()
{
	m_bPressing = false;
	m_bCanMove = false;
	m_vTarget = Vec3(0,0,0);
	m_vSpeed = Vec3(0,0,0);
}
RBCarry::~RBCarry()
{

}

void RBCarry::OnPress()
{
	m_bPressing = true;
}

void RBCarry::OnRelease()
{
	m_bPressing = false;
}

void RBCarry::OnSpellRelease()
{

}

bool RBCarry::OnSpellActivate()
{
	m_bCanMove = false;
	CryLog("Disabled");
	return true;
}

void RBCarry::Reset()
{
	m_bCanMove = false;
	m_bPressing = false;
	m_vSpeed = Vec3(0,0,0);
}
void RBCarry::PostUpdate(float dt)
{
	if(m_bPressing)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());

		Vec3 origin = pPlayer->GetEntity()->GetPos();
		Vec3 fwd = pPlayer->GetEntity()->GetForwardDir();

		IRenderAuxGeom* pRenderAux = gEnv->pRenderer->GetIRenderAuxGeom();



		float rad = 3;
		for( int i=0; i < 360; ++i )
		{
			/*float t = 2 * g_PI * i / 360.0f;*/
			float x = origin.x + rad*cosf(i);
			float y = origin.y + rad*sinf(i);

			Vec3 dir = Vec3(x,y,origin.z + 0.1f);


			pRenderAux->DrawPoint(Vec3(x,y,origin.z + 0.1f),ColorB(255,255,255),10.0f);

		}
		AABB aabb(origin,rad);
		pRenderAux->DrawAABB(aabb,pPlayer->GetEntity()->GetWorldTM(),false,ColorB(0,0,0),EBoundingBoxDrawStyle::eBBD_Faceted);

		bool ok = false;

		IPhysicalEntity** vEntities;
		if(gEnv->pPhysicalWorld->GetEntitiesInBox(aabb.min,aabb.max,vEntities,ent_rigid | ent_sleeping_rigid) > 0)
		{
			if(IEntity* pEntity = gEnv->pEntitySystem->GetEntityFromPhysics(vEntities[0]))
			{
				m_pPhysEntity = vEntities[0];
				CryLog( pEntity->GetName() );
				ok = true;
			}
		}

		if(ok)
		{
			Vec3 mouse = pPlayer->GetSpellSystem()->GetMouseCoordinates();

			int invMouseY = gEnv->pRenderer->GetHeight() - mouse.y;

			Vec3 vPos0(0,0,0);
			gEnv->pRenderer->UnProjectFromScreen((float)mouse.x, (float)invMouseY, 0, &vPos0.x, &vPos0.y, &vPos0.z);

			Vec3 vPos1(0,0,0);
			gEnv->pRenderer->UnProjectFromScreen((float)mouse.x, (float)invMouseY, 1, &vPos1.x, &vPos1.y, &vPos1.z);

			Vec3 vDir = vPos1 - vPos0;
			vDir.Normalize();



			ray_hit hit;
			const unsigned int flags = rwi_stop_at_pierceable|rwi_colltype_any;

			if (gEnv->pPhysicalWorld && gEnv->pPhysicalWorld->RayWorldIntersection(vPos0, vDir *  gEnv->p3DEngine->GetMaxViewDistance(),  ent_rigid | ent_sleeping_rigid, flags, &hit, 1))
			{
				if(IEntity* ent = gEnv->pEntitySystem->GetEntityFromPhysics(hit.pCollider))
				{
					m_bCanMove = true;
					m_bPressing = false;
					CryLog("Starting");

					pe_action_awake awake;
					awake.bAwake = true;
					awake.minAwakeTime = 2.0f;
					m_pPhysEntity->Action(&awake,1);

					pe_action_set_velocity move;
					move.v = Vec3(0,0,2);
					move.w = Vec3(0,0,0.3f);

					m_pPhysEntity->Action(&move,1);

					m_vSpeed.z = 2;
				}
			}
		}
	}
	else
	{
		if(m_bCanMove)
		{
			if(m_pPhysEntity)
			{
				IEntity* pEntity = gEnv->pEntitySystem->GetEntityFromPhysics(m_pPhysEntity);

				if(pEntity)
				{
					IEntityPhysicalProxy* pProxy = (IEntityPhysicalProxy*)pEntity->GetProxy(ENTITY_PROXY_PHYSICS);

					if(pProxy)
					{
					
						float currentZvel = m_vSpeed.z;

						Interpolate(currentZvel,0,1.0f,dt);

						m_vSpeed.z = currentZvel;

						pe_action_set_velocity move;
						move.v = Vec3(0,0,currentZvel);

						m_pPhysEntity->Action(&move,1);


					}

				}


			}
		}
	}
}