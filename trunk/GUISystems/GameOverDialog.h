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

class CGameOverDialog: virtual public CGameDialogBase
{
	unsigned int m_nStartTime;
	bool m_bAlreadyFinished;
	
	CSoundTypeWrapper m_GameOverMusic;
	ISound *m_piGameOverMusicSound;
	
	void OnDraw(IGenericRender *piRender);
	
	int	Execute(IGameWindow *piParent);
	bool Unserialize(ISystemPersistencyNode *piNode);
	void Destroy();
	
	BEGIN_PROP_MAP(CGameOverDialog)
	PROP_CLASS_CHAIN(CGameDialogBase);
	PROP_FLAGS(m_GameOverMusic,"Music",MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()
	
public:

	CGameOverDialog(void);
	~CGameOverDialog(void);
};
