#pragma once

class CConfigFile;
class CConfigFileNode;

class CConfigFileNode: public ISystemPersistencyNode
{
    friend CConfigFile;

	std::string					                m_sName;
	std::string					                m_sValue;
	bool										m_bHasValue;
    std::multimap<std::string,CConfigFileNode*>	m_mNodes;
	std::vector<CConfigFileNode *>				m_vNodes; // para mantener el orden de carga

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
	virtual void DeleteNode(std::string &id);
	virtual void DeleteNode(const char *id);

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
		eNormalizationState_InBlockComment,
	};

    DWORD			m_dwSaveTabCount;
    HANDLE  	    m_hFile;
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
        if(piNode){return SUCCEEDED(PersistencySave(piNode));}else{return false;}\
    }\
    bool LoadFrom(CConfigFile *pFile,std::string sNode)\
    {\
        ISystemPersistencyNode *piNode=pFile->GetNode(sNode);\
        if(piNode){return SUCCEEDED(PersistencyLoad(piNode));}else{return false;}\
    }

