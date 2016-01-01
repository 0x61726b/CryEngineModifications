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

#ifndef __BoulderRotater_h__
#define __BoulderRotater_h__
#pragma once
//----------------------------------------------------------------------------
#include <IGameObject.h>
//----------------------------------------------------------------------------


class CBoulderRotater : public CGameObjectExtensionHelper<CBoulderRotater, IGameObjectExtension>
{

	struct SProperties
	{
		SProperties() :
			m_AutoTurn(false),
			m_fRotateInterval(0.0f),
			m_fRotateFor(0.0f)
		{

		}
		void InitFromScript( const IEntity& entity );

		bool m_AutoTurn;
		float m_fRotateInterval;
		float m_fRotateFor;	
	};
public:
	CBoulderRotater();
	virtual ~CBoulderRotater();

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
	virtual void PostUpdate(float frameTime ) {};
	virtual void PostRemoteSpawn() {};
	virtual void HandleEvent( const SGameObjectEvent& ) {};
	virtual void ProcessEvent( SEntityEvent& );
	virtual void SetChannelId(uint16 id) {};
	virtual void SetAuthority( bool auth ) {};

	virtual void GetMemoryUsage(ICrySizer * s) const	
	{
		s->AddObject(this, sizeof(*this));
	}

private:

	void Reset();

	SProperties script;

	IEntity*	m_pRotater;
	IEntity*	m_pTrigger;

	void AutoRotate();

	float m_fTurnInterval;
	float m_fTimer;
	float m_fRotatedAngle;
	typedef std::map<string,IEntity*> LinkMap;
	LinkMap m_LinkMap;

	bool m_bTriggered;

	bool m_bAutoTurn;
};



//----------------------------------------------------------------------------
#endif //~__ProceduralCollectibleSystem_h__