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

	virtual void BuildEntity() = 0;
	virtual void Update() = 0;
};

class BuildFireCamp : public ISpell,public IHardwareMouseEventListener
{
public:
	BuildFireCamp();
	~BuildFireCamp();

	void OnPress();
	void OnRelease();

	void Update();
	void BuildEntity();

	void OnHardwareMouseEvent(int iX,int iY,EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta = 0);


private:
	int m_mouseX;
	int m_mouseY;

	Vec3 mouseWorldPos;
	void CalculateMouseWorldPos();
	IEntity* localEntity;
};
class SpellSystem
{
public:
	SpellSystem();
	~SpellSystem();

	void Reset();

	void OnSpellPressed(int SpellId);
	void OnSpellReleased(int SpellId);

	ISpell* lastSpell;
};

#endif