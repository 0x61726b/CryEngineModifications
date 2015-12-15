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
#ifndef __Campfire_h__
#define __Campfire_h__
//------------------------------------------------------------------------
#include <IGameObject.h>
#include "Actor.h"
#include "ItemResourceCache.h"
#include "Game.h"
#include "GameParameters.h"
//------------------------------------------------------------------------

class CCampfire : public CActor
{
	struct SProperties
	{
		SProperties():
			m_Scale(1),
			particleEffect(""),
			triggerRadius(1)
		{

		}
		void InitFromScript( const IEntity& entity );

		float m_Scale;
		float triggerRadius;
		char* particleEffect;
	};
public:
	CCampfire();
	virtual ~CCampfire();

	//IGameObjectExtension
	virtual bool Init( IGameObject * pGameObject );
	virtual void InitClient(int channelId) {};
	virtual void PostInit( IGameObject * pGameObject );
	virtual void PostInitClient(int channelId) {};
	virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params );
	virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params );
	virtual bool GetEntityPoolSignature( TSerialize signature );
	virtual void Release() { delete this; };
	virtual void FullSerialize( TSerialize ser );
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags ) { return true; }
	virtual void PostSerialize();
	virtual void SerializeSpawnInfo( TSerialize ser ) {}
	virtual ISerializableInfoPtr GetSpawnInfo() {return 0;}
	virtual void Update( SEntityUpdateContext& ctx, int );
	virtual void PostUpdate(float frameTime );
	virtual void PostRemoteSpawn() {};
	virtual void HandleEvent( const SGameObjectEvent& );
	virtual void ProcessEvent( SEntityEvent& );
	virtual void SetChannelId(uint16 id) {};
	virtual void SetAuthority( bool auth ) {};


	virtual void GetMemoryUsage(ICrySizer * s) const	
	{
		s->AddObject(this, sizeof(*this));
	}

	void Reset(bool) {}
	void	SerializeXML( XmlNodeRef& node, bool bLoading ) {}
	IActorMovementController * CreateMovementController() { return NULL;}
private:

	void Reset();
	void Spawn();

	bool m_bGenerating;
	float m_fTime;

	IParticleEmitter* m_pParticleEffect;
	SProperties m_ScriptsProps;
};

#endif