//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#pragma once

#include <SoundSystems.h>

DECLARE_CUSTOM_WRAPPER1(CGenericSoundTypeWrapper,ISoundType,m_piSoundType)

class CGameGUIButton: virtual public CGameGUILabel, virtual public IGameGUIButton
{
protected:
	bool m_bClickInProgress;
	bool m_bWantFocus;
	bool m_bFocusOnHover;
	bool m_bSoundsEnabled;

	CGenericTextureWrapper m_DeactivatedTexture;
	CGenericFontWrapper m_DeactivatedFont;
	SGameRect	m_rDeactivatedTextureCoords;
	CVector		m_vDeactivatedBackgroundColor;
	double		m_dDeactivatedBackgroundAlpha;
	CVector		m_vDeactivatedTextColor;
	double		m_dDeactivatedTextAlpha;
	
	SGameRect	m_rHoverTextureCoords;
	CGenericTextureWrapper m_HoverTexture;
	CGenericFontWrapper m_HoverFont;
	CVector		m_vHoverBackgroundColor;
	double		m_dHoverBackgroundAlpha;
	CVector		m_vHoverTextColor;
	double		m_dHoverTextAlpha;
	bool		m_bHoverEnabled;

	CGenericSoundTypeWrapper m_HoverSound;
	CGenericSoundTypeWrapper m_ClickSound;
	
	ISound *m_piHoverSound;
	ISound *m_piClickSound;
	
	BEGIN_PROP_MAP(CGameGUIButton)
		PROP_CLASS_CHAIN(CGameGUILabel)
		PROP_VALUE_FLAGS(m_bWantFocus,"WantFocus",false,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bFocusOnHover,"FocusOnHover",false,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_rDeactivatedTextureCoords,"DeactivatedTextureCoords",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_DeactivatedTexture,"DeactivatedTexture",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_DeactivatedFont,"DeactivatedFont",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dDeactivatedBackgroundAlpha,"DeactivatedBkAlpha",0.8,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vDeactivatedBackgroundColor,"DeactivatedBkColor",CVector(0.3,0.3,0.3),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dDeactivatedTextAlpha,"DeactivatedTextAlpha",0.8,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vDeactivatedTextColor,"DeactivatedTextColor",CVector(1,1,1),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_rHoverTextureCoords,"HoverTextureCoords",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_HoverTexture,"HoverTexture",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_HoverFont,"HoverFont",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bHoverEnabled,"HoverEnabled",true,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dHoverBackgroundAlpha,"HoverBkAlpha",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vHoverBackgroundColor,"HoverBkColor",CVector(0.5,0.5,1.0),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dHoverTextAlpha,"HoverTextAlpha",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vHoverTextColor,"HoverTextColor",CVector(1,1,1),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_HoverSound,"HoverSound",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_ClickSound,"ClickSound",MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	
	bool Unserialize(ISystemPersistencyNode *piNode);
	void Destroy();
	
	void OnMouseMove(double x,double y);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseUp(int nButton,double x,double y);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnWantFocus(bool *pbWant);
	
	void OnSetFocus();
	void OnKillFocus(IGameWindow *piFocusedWindow);
	
public:
	// IGameGUIButton
	
	void EnableSounds();
	void DisableSounds();
	bool AreSoundsEnabled();
	
	void PlayClickSound();
	void PlayHoverSound();
	
	void OnDrawBackground(IGenericRender *piRender);
	void OnDraw(IGenericRender *piRender);


	CGameGUIButton(void);
	~CGameGUIButton(void);
};
