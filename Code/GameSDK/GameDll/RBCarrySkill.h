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

#ifndef __RBCarrySkill_h__
#define __RBCarrySkill_h__

#include "ISkill.h"


class RBCarry : public ISpell
{
public:
	RBCarry();
	~RBCarry();

	void OnPress();
	void OnRelease();

	void OnSpellRelease(); //Means that right click is pressed.Handle this event accordingly.
	bool OnSpellActivate(); //Means that left click is pressed 2nd time.

	void Reset();

	void PostUpdate(float dt);

	IPhysicalEntity* m_pPhysEntity;

	bool m_bPressing;

	bool m_bCanMove;

	Vec3 m_vTarget;
	Vec3 m_vSpeed;
};

#endif

