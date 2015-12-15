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


	void SetBushText(const string&);
	void SetFlintText(const string&);

	void SetHealthOrb(int percent);
	void SetManaOrb(int percent);

	void EnableSkillOne(bool);

	void EnableHungerWarning(bool);
	
	//IUIElementEventListener
	void OnUIEvent( IUIElement* pSender, const SUIEventDesc& event, const SUIArguments& args );
	//~

	IUIElement* m_pArkenUI;
	IUIElement* m_pSkillbar;

};
//-------------------------------------------------------------------------
#endif


