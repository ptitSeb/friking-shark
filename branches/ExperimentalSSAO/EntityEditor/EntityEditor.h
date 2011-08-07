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

class IEntityEditorPropertyPanel:virtual public IGameWindow
{
public:
	virtual bool SetObject(ISystemObject *piObject)=0;
};

class IEntityEditorPropertyPanelEvents
{
public:
	virtual void OnObjectChanged(IEntityEditorPropertyPanel *piPanel,ISystemObject *piObject)=0;
	virtual void OnObjectRemoved(IEntityEditorPropertyPanel *piPanel,ISystemObject *piObject)=0;
};

class IEntityEditorNamedObjectSelector:virtual public IGameWindow
{
public:
	virtual bool SelectObject(IGameWindow *piParent,std::string sTitle,std::string sSystem,std::string *psSelectedObject)=0;
};

class IEntityEditorClassSelector:virtual public IGameWindow
{
public:
	virtual bool SelectClass(IGameWindow *piParent,std::string sTitle,std::string sSystem,std::string *psClass)=0;
};


class IEntityEditorObjectListSelector: virtual public ISystemUnknown
{
public:
	
	virtual bool SelectObject(std::string sTitle,IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject)=0;
	
};