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
Description: 

-------------------------------------------------------------------------
History:
- 18.12.2015 - Created

*************************************************************************/
#ifndef __PuzzleFire_h__
#define __PuzzleFire_h__
//------------------------------------------------------------------------
#include <IGameObject.h>
#include "Actor.h"
#include "ItemResourceCache.h"
#include "Game.h"
#include "GameParameters.h"
//------------------------------------------------------------------------

class CPuzzleFire : public CGameObjectExtensionHelper<CPuzzleFire, IGameObjectExtension>,public IInputEventListener
{
	struct SProperties
	{
		SProperties():
			m_Color(Vec3(0,0,0)),
			m_Model(""),
			m_Mat(""),
			m_Tex(""),
			m_zrot(0.0f),
			m_xrot(0.0f),
			m_yrot(0.0f),
			m_fradius(0.0f),
			m_Offset(Vec3(0,0,0)),
			m_fDiffuseMul(0.0f),
			m_fPointLightRad(0.0f),
			m_fProjFov(0.0f),
			m_fProjNear(0.0f),
			m_fTriggerRad(0.0f),
			m_AudioTrigger(""),
			m_bActive(true)
		{

		}
		void InitFromScript( const IEntity& entity );

		Vec3 m_Color;
		char* m_Model;
		char* m_Mat;
		char* m_Tex;
		float m_zrot;
		float m_xrot;
		float m_yrot;
		float m_fradius;
		Vec3 m_Offset;
		float m_fDiffuseMul;
		float m_fPointLightRad;
		float m_fProjFov;
		float m_fProjNear;
		float m_fTriggerRad;
		char* m_AudioTrigger;
		bool m_bActive;
	};

	enum ECraftableItemSounds
	{
		eSID_SwitchOn,
		eSID_SwitchOff,
		eSID_Max
	};
public:
	CPuzzleFire();
	virtual ~CPuzzleFire();

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


	bool OnInputEvent( const SInputEvent &event ) ;

	virtual void GetMemoryUsage(ICrySizer * s) const	
	{
		s->AddObject(this, sizeof(*this));
	}
private:

	void Reset();
	void Spawn();

	void SwitchLights(bool b);
	bool m_bCanInteract;
	ILightSource*	m_pLight;
	ILightSource*	m_pPointLight;
	IEntityAudioProxy* m_pEntityAudioProxy;
	SProperties m_ScriptsProps;

	TAudioControlID m_audioControlIDs[eSID_Max];

	TAudioProxyID m_pAudioProxyId;

	IEntity* m_pPuzzleController;
};

#endif