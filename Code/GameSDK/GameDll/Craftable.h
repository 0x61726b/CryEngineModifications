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
#ifndef __Craftable_h__
#define __Craftable_h__
//------------------------------------------------------------------------
#include <IGameObject.h>
#include "CraftSystem.h"
//------------------------------------------------------------------------

class CCraftable : public CGameObjectExtensionHelper<CCraftable, IGameObjectExtension>,public ICraftable
{
	struct SProperties
	{
		SProperties()
			: m_Model(""),
			m_ItemCollectSoundTriggerID(""),
			m_Scale(1),
			m_Drop(-1),
			m_Type(-1),
			m_HealthBonus(0)
		{

		}
		void InitFromScript( const IEntity& entity );

		const char* m_Model;
		const char* m_ItemCollectSoundTriggerID;
		int			m_Drop;
		int			m_Type;
		int			m_HealthBonus;
		float m_Scale;
	};
	enum ECraftableItemSounds
	{
		eSID_Collect,
		eSID_Max
	};
public:
	CCraftable();
	virtual ~CCraftable();

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
	virtual void HandleEvent( const SGameObjectEvent& );
	virtual void ProcessEvent( SEntityEvent& );
	virtual void SetChannelId(uint16 id) {};
	virtual void SetAuthority( bool auth ) {};

	virtual void GetMemoryUsage(ICrySizer * s) const	
	{
		s->AddObject(this, sizeof(*this));
	}

protected:
	SProperties m_ScriptsProps;

private:
	TAudioControlID m_audioControlIDs[eSID_Max];

	IEntityAudioProxy* m_pEntityAudioProxy;
	TAudioProxyID	   m_pAudioProxyId;

	void Reset();

	void Spawn();

	
};



#endif

