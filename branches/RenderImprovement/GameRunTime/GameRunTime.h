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

#ifdef GAMERUNTIME_EXPORTS
	#ifdef WIN32 
		#define GAMERUNTIME_API __declspec(dllexport)
	#else
		#define GAMERUNTIME_API 
	#endif
	
#else
	#ifdef WIN32 
		#define GAMERUNTIME_API __declspec(dllimport)
	#else
		#define GAMERUNTIME_API 
	#endif
#ifndef GAMERUNTIMELIB_COMPILATION
//#pragma comment (lib,"GameRunTime.lib")
#endif
#endif

#include <string>
#include <set>
#include <map>
#include <vector>


class ISystemModule;
class ISystemClass;
class ISystemObject;
class ISystem;
class ISubscriber;
class IPublisher;

struct SSystemProperty
{
    std::string name;
    std::string value;
};

class ISystemUnknown
{
public:
    virtual void AddReference()=0;
    virtual void ReleaseReference()=0;
};

class ISystemModule:virtual public ISystemUnknown
{
public:

    virtual bool Init(std::string sPath,ISystem *piSystem)=0;
    virtual void Destroy()=0;

    virtual std::string GetName()=0;
};

class ISystemClass:virtual public ISystemUnknown
{
public:
    virtual bool CreateInstance(ISystemObject **piObject)=0;
    virtual std::string GetName()=0;
};

class ISystemObject:virtual public ISystemUnknown
{
public:
    virtual bool Init(std::string sClass,std::string sName,ISystem *piSystem)=0;
    virtual void Destroy()=0;

    virtual std::string  GetName()=0;
    virtual std::string  GetClass()=0;
    virtual std::string  GetSystemName()=0;
    virtual ISystem     *GetSystem()=0;
};

class ISystemPersistencyNode
{
public:
    virtual void Clear()=0;
	virtual	const char			   *GetName()=0;
	virtual ISystemPersistencyNode *AddNode(std::string &id)=0;
	virtual ISystemPersistencyNode *AddNode(const char *id)=0;
	virtual ISystemPersistencyNode *GetNode(std::string &id)=0;
	virtual ISystemPersistencyNode *GetNode(const char *id)=0;
	virtual ISystemPersistencyNode *GetNode(unsigned long nIndex)=0;
	virtual unsigned long			GetNodeCount()=0;
	virtual void					SetValue(const char *sValue)=0;
	virtual void					SetValue(std::string &sValue)=0;
	virtual const char			   *GetValue()=0;
	virtual void					RemoveValue()=0;
	virtual void DeleteNode(std::string &id)=0;
	virtual void DeleteNode(const char *id)=0;

	virtual std::string GetDebugInfoPath()=0;
};

class ISystemSerializable:virtual public ISystemUnknown
{
public:
    virtual bool Serialize(ISystemPersistencyNode *piNode)=0;
    virtual bool Unserialize(ISystemPersistencyNode *piNode)=0;
};

class ISystem:virtual public ISystemObject
{
public:

    // class factories

    virtual bool RegisterClassFactory(ISystemClass *piClass)=0;
    virtual void UnregisterClassFactory(ISystemClass *piClass)=0;

    // modules

    virtual bool LoadModule(std::string sPath,ISystemModule **ppiModule)=0;
    virtual bool GetModule(std::string sPath,ISystemModule **ppiModule)=0;
	virtual void GetModules(std::vector<ISystemModule *> *pvModules)=0;
	virtual bool RegisterModule(ISystemModule *piModule)=0;
    virtual void UnregisterModule(ISystemModule *piModule)=0;

    // objects (controllers)

    virtual bool RegisterObject(ISystemObject *piObject)=0;
    virtual void UnregisterObject(ISystemObject *piObject)=0;

    virtual bool GetObject(std::string sName,ISystemObject **piObject)=0;
	virtual void GetObjects(std::vector<ISystemObject *> *pvObjects)=0;
    virtual bool CreateObject(std::string sClass,ISystemObject **piObject)=0;
    virtual bool CreateObject(std::string sClass,std::string sName,ISystemObject **piObject)=0;
	
	// Class enumeration
	virtual bool GetClass(std::string sNameClass,ISystemClass **piClass)=0;
	virtual void GetClasses(std::vector<ISystemClass *> *pvClasses)=0;

	
	virtual void DestroyAllObjects()=0;
};

class ISystemManager: virtual public ISystemUnknown
{
public:

    virtual ISystem *CreateSystem(std::string sName)=0;
    virtual ISystem *GetSystem(std::string sName)=0;

    virtual bool    RegisterSystem(ISystem *piSystem)=0;
    virtual void    UnregisterSystem(ISystem *piSystem)=0;


	virtual void			DebugRegisterObject(ISystemObject *piObject)=0;
	virtual void			DebugUnregisterObject(ISystemObject *piObject)=0;
	virtual unsigned long	DebugGetRegisteredObjectCount()=0;
	virtual void			DebugGetRegisteredObjects(std::map<ISystemObject *,std::string> *pObjects)=0;

};

class ISystemManagerPathControl: virtual public ISystemUnknown
{
public:

    virtual void AddLibraryPath(std::string sPath)=0;
    virtual void RemoveLibraryPath(std::string sPath)=0;
    virtual void ClearLibraryPaths()=0;
    virtual void GetLibraryPaths(std::set<std::string> *pvPaths)=0;
};

ISystemManager GAMERUNTIME_API *GetSystemManager();

class ISystemEvents:virtual public ISystemUnknown
{
public:

    // class factories

    virtual void OnSystemClassFactoryRegistered(ISystemClass *piClass){};
    virtual void OnSystemClassFactoryUnregistered(std::string sName){};

    // modules

    virtual void OnSystemModuleLoaded(ISystemModule *piModule){};
    virtual void OnSystemModuleRegistered(ISystemModule *piModule){};
    virtual void OnSystemModuleUnregistered(std::string sPath){};

    // objects (controllers)

    virtual void OnSystemObjectRegistered(ISystemObject *piObject){};
    virtual void OnSystemObjectUnregistered(ISystemObject *piObject){};
};

class ISystemManagerEvents:virtual public ISystemUnknown
{
public:

    virtual bool    OnSystemUnregistered(ISystem *piSystem)=0;
    virtual void    OnSystemRegistered(ISystem *piSystem)=0;
};

class ISubscriber
{
public:

    virtual bool SubscribeTo(std::string sEventInterface,IPublisher *pPublisher)=0;
    virtual void UnsubscribeFrom(std::string sEventInterface,IPublisher *pPublisher,bool bPublisherCommand)=0;
    virtual void UnsubscribeFrom(IPublisher *pPublisher)=0;
    virtual void UnsubscribeInterface(std::string sEventInterface)=0;
    virtual void UnsubscribeAll()=0;
};

class IPublisher
{
public:

    virtual bool Subscribe(std::string sEventInterface,ISubscriber *pSubscriber)=0;
    virtual void Unsubscribe(std::string sEventInterface,ISubscriber *pSubscriber)=0;
};

    template<class _destination,class _origin>
    _destination *__make_qi(_origin *pOrg,_destination *pDestFake,const char *pfile,int nLine)
    {
        _destination *temp=dynamic_cast<_destination*>(pOrg);
        if(temp)
		{
			temp->AddReference();
		}
        return temp;
    }
    template<class INTERFACE>
    inline INTERFACE *__make_addref(INTERFACE *piUnk,const char *pfile,int nLine)
    {
        if(piUnk!=NULL)
		{
			piUnk->AddReference();
		}

        return piUnk;
    }
	template<class INTERFACE>
	inline INTERFACE *__make_release(INTERFACE *piUnk,const char *pfile,int nLine)
	{
		if(piUnk!=NULL)
		{
			piUnk->ReleaseReference();
		}
		return piUnk;
	}

#define ADD(x) __make_addref(x,__FILE__,__LINE__)
#define REL(x) {if((x)){__make_release(x,__FILE__,__LINE__);(x)=NULL;}}
#define QI(i,x) __make_qi<i>((x),(i*)NULL,__FILE__,__LINE__)
