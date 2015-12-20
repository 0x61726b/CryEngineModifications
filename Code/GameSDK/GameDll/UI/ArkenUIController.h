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
- 14.12.2015 - Created

*************************************************************************/
#ifndef __ArkenUIController_h__
#define __ArkenUIController_h__
//-------------------------------------------------------------------------
#include <IFlashUI.h>


class ArkenUIController : public IUIElementEventListener
{
public:
	ArkenUIController();
	~ArkenUIController();

	//Get singleton instance
	static ArkenUIController* Get();


	void BerryText(int);
	void BananaText(int);
	void DurianText(int);
	void WatermelonText(int);
	void DragonFruitText(int);
	void PomegranateText(int);


	void SetHealthOrb(int percent);
	void SetManaOrb(int percent);

	void EnableSkillOne(bool);

	void EnableHungerWarning(bool);

	void ShowNotEnough(bool);

	void SetObjectiveOne(int p11,int p21,bool p3,bool p4);
	void SetObjectiveTwo(bool p1,bool p4);

	void HideAll();
	void ShowAll();
	
	
	
	//IUIElementEventListener
	void OnUIEvent( IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args );
	//~

	IUIElement* m_pArkenUI;
	IUIElement* m_pSkillbar;
	IUIElement* m_pCraftMenu;
	bool obj2;

};
//-------------------------------------------------------------------------
#endif


