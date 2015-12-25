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
#ifndef __SpellSystem_h__
#define __SpellSystem_h__

#include <IGameObject.h>
#include <IHardwareMouse.h>
#include "HungerSanityController.h"

#define FIRECAMP_REQUIRED_SANITY 40

class SpellSystem;

namespace Spells
{
	enum ESpells
	{
		BuildFireCamp
	};

}

class ISpell
{
public:
	virtual void OnPress() = 0;
	virtual void OnRelease() = 0;

	virtual void OnSpellRelease() = 0; //Means that right click is pressed.Handle this event accordingly.
	virtual bool OnSpellActivate() = 0; //Means that left click is pressed 2nd time.

	virtual void BuildEntity() = 0;
	virtual void PostUpdate(float dt) = 0;
};

class BuildFireCamp : public ISpell
{
public:
	BuildFireCamp(SpellSystem*);
	~BuildFireCamp();

	void OnPress();
	void OnRelease();
	void OnSpellRelease();
	bool OnSpellActivate();

	void PostUpdate(float dt);
	void BuildEntity();

	CPlayer* m_pPlayer;
	bool m_bPlacing;
	Vec3 mouseWorldPos;
	IEntity* localEntity;
	SpellSystem* m_SpellSystem;
};
class SpellSystem : public IHardwareMouseEventListener,public IHungerSanityEventListener
{
public:
	SpellSystem();
	~SpellSystem();

	void Reset();

	void OnSpellPressed(int SpellId);
	void OnSpellReleased(int SpellId);
	void OnRightClick();
	bool OnLeftClick();

	void PostUpdate(float dt);

	void OnSanityChanged();
	void OnHungerChanged();
	void OnAwake() {}
	void OnStartSleeping() {}

	void OnHardwareMouseEvent(int iX,int iY,EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta = 0);

	ISpell* lastSpell;

	Vec3 GetMouseCoordinates();

	std::vector<ISpell*> m_vActiveSpells;
private:
	int m_mouseX;
	int m_mouseY;


};

#endif