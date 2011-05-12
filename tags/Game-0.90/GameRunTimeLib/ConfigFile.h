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

class CConfigFile;
class CConfigFileNode;

class CConfigFileNode: public ISystemPersistencyNode
{
    friend class CConfigFile;

    std::string					                m_sName;
    std::string					                m_sValue;
    bool										m_bHasValue;
    std::multimap<std::string,CConfigFileNode*>	m_mNodes;
    std::vector<CConfigFileNode *>				m_vNodes; // para mantener el orden de carga
    CConfigFileNode								*m_pParent;
    std::string 								m_sParentFileName; // File Name for debugging pourposes (only for root nodes)

    CConfigFileNode *GetAddNode_Internal(const char *pNodePath,bool bAdd);

public:

    // ISystemPersistencyNode
    virtual void Clear();

    virtual const char *GetName();
    virtual const char *GetValue();
    virtual void SetValue(const char *psValue);
    virtual void SetValue(std::string &sValue);
    virtual void RemoveValue();

    virtual ISystemPersistencyNode *AddNode(std::string &id);
    virtual ISystemPersistencyNode *AddNode(const char *id);
    virtual ISystemPersistencyNode *GetNode(std::string &id);
    virtual ISystemPersistencyNode *GetNode(const char *id);
    virtual ISystemPersistencyNode *GetNode(unsigned long nIndex);
    virtual unsigned long			GetNodeCount();

    virtual void SetFileName(std::string &sFileName);
    std::string  GetDebugInfoPath();

    virtual void DeleteNode(std::string &id);
    virtual void DeleteNode(const char *id);

    CConfigFileNode(CConfigFileNode *pParent);
    CConfigFileNode();
    ~CConfigFileNode();
};

class CConfigFile
{
    enum ENormalizationState
    {
        eNormalizationState_RegularCharacter,
        eNormalizationState_InString,
        eNormalizationState_InLineComment,
        eNormalizationState_InBlockComment
    };

    unsigned int			m_dwSaveTabCount;
    FILE  	    	*m_pFile;
    char			*m_pBuffer;

    CConfigFileNode m_RootNode;

    void SaveLine(const char *sValue);
    void SaveValue(const char *sName,const char *sValue);
    void SaveBeginSection();
    void SaveEndSection();
    void SaveNode(CConfigFileNode *pNode);

    void UnescapeString(char *pString);
    char *RemoveSpacesAndTabs(char *pString);
    char *EscapeString(const char *pString);

public:

    bool Open(std::string &sFileName);
    bool Open(const char *pFileName);
    bool Save(std::string &sFileName);
    bool Save(const char *psFileName);

    ISystemPersistencyNode	*AddNode(std::string  &sNodePath);
    ISystemPersistencyNode	*AddNode(const char *pNodePath);
    ISystemPersistencyNode	*GetNode(std::string  &sNodePath);
    ISystemPersistencyNode	*GetNode(const char *pNodePath);
    ISystemPersistencyNode	*GetRoot();

    CConfigFile(void);
    ~CConfigFile(void);
};

#define DECLARE_CONFIG_FILE_MEDIA()\
    bool SaveTo(CConfigFile *pFile,std::string sNode)\
    {\
        ISystemPersistencyNode *piNode=pFile->AddNode(sNode);\
        if(piNode){return PersistencySave(piNode);}else{return false;}\
    }\
    bool LoadFrom(CConfigFile *pFile,std::string sNode)\
    {\
        ISystemPersistencyNode *piNode=pFile->GetNode(sNode);\
        if(piNode){return PersistencyLoad(piNode);}else{return false;}\
    }

