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


#include "./stdafx.h"
#include <stack>
#include "./GameRunTimeLib.h"
#include "./ConfigFile.h"

#define NODE_PATH_MAX 512
#define CONFIG_FILE_DELIMITER " \t\r\n="

CConfigFileNode::CConfigFileNode(CConfigFileNode *pParent)
{
    m_pParent=pParent;
    m_bHasValue=false;
}

CConfigFileNode::CConfigFileNode()
{
    m_pParent=NULL;
    m_bHasValue=false;
}


CConfigFileNode::~CConfigFileNode()
{
    std::multimap<std::string,CConfigFileNode*>::iterator i;
    for (i=m_mNodes.begin();i!=m_mNodes.end();i++)
    {
        CConfigFileNode *pNode=i->second;
        delete pNode;
    }
    m_mNodes.clear();
    m_vNodes.clear();
}

CConfigFileNode *CConfigFileNode::GetAddNode_Internal(const char *pNodePath,bool bAdd)
{
    const char *pSubNodeEnd=strchr(pNodePath,'\\');
    if (pSubNodeEnd)
    {
        char sSubNodeName[NODE_PATH_MAX]={0};
        strncpy(sSubNodeName,pNodePath,(unsigned int)(pSubNodeEnd-pNodePath));
        std::multimap<std::string,CConfigFileNode*>::iterator i=m_mNodes.find((std::string)sSubNodeName);
        if (i!=m_mNodes.end())
        {
            CConfigFileNode *pNode=i->second;
            return pNode->GetAddNode_Internal(pSubNodeEnd+1,bAdd);
        }
        else
        {
            if (bAdd)
            {
                CConfigFileNode *pNode=new CConfigFileNode(this);
                pNode->m_sName=sSubNodeName;
                m_mNodes.insert(std::pair<std::string,CConfigFileNode*>(sSubNodeName,pNode));
                m_vNodes.push_back(pNode);
                return pNode->GetAddNode_Internal((pSubNodeEnd+1),bAdd);
            }
            else
            {
                return NULL;
            }
        }
    }
    else
    {
        std::multimap<std::string,CConfigFileNode*>::iterator i=m_mNodes.find((std::string)pNodePath);
        if (i!=m_mNodes.end())
        {
            CConfigFileNode *pNode=i->second;
            return pNode;
        }
        else
        {
            if (bAdd)
            {
                CConfigFileNode *pNode=new CConfigFileNode(this);
                pNode->m_sName=pNodePath;
                m_mNodes.insert(std::pair<std::string,CConfigFileNode*>(pNodePath,pNode));
                m_vNodes.push_back(pNode);
                return pNode;
            }
            else
            {
                return NULL;
            }
        }
    }
}

ISystemPersistencyNode *CConfigFileNode::AddNode(std::string &id) {
    return GetAddNode_Internal(id.c_str(),true);
}
ISystemPersistencyNode *CConfigFileNode::GetNode(std::string &id) {
    return GetAddNode_Internal(id.c_str(),false);
}
ISystemPersistencyNode *CConfigFileNode::AddNode(const char *id) {
    return GetAddNode_Internal(id,true);
}
ISystemPersistencyNode *CConfigFileNode::GetNode(const char *id) {
    return GetAddNode_Internal(id,false);
}
unsigned long			CConfigFileNode::GetNodeCount() {
    return (unsigned long)m_mNodes.size();
}

ISystemPersistencyNode *CConfigFileNode::GetNode(unsigned long nIndex)
{
    if (nIndex>=m_vNodes.size()) {
        return NULL;
    }
    return m_vNodes[nIndex];
}

void CConfigFileNode::DeleteNode(std::string &sNodePath) {
    DeleteNode(sNodePath.c_str());
}
void CConfigFileNode::DeleteNode(const char *pNodePath)
{
    const char *pSubNodeEnd=strchr(pNodePath,'\\');
    if (pSubNodeEnd)
    {
        char sSubNodeName[NODE_PATH_MAX]={0};
        strncpy(sSubNodeName,pNodePath,(unsigned int)(pSubNodeEnd-pNodePath));
        std::multimap<std::string,CConfigFileNode*>::iterator i=m_mNodes.find((std::string)sSubNodeName);
        if (i!=m_mNodes.end())
        {
            CConfigFileNode *pNode=i->second;
            pNode->DeleteNode(pSubNodeEnd+1);
        }
    }
    else
    {
        std::multimap<std::string,CConfigFileNode*>::iterator i=m_mNodes.find(pNodePath);
        if (i!=m_mNodes.end())
        {
            CConfigFileNode *pNode=i->second;
            m_mNodes.erase(i);

            std::vector<CConfigFileNode*>::iterator iVector;
            for (iVector=m_vNodes.begin();iVector!=m_vNodes.end();iVector++)
            {
                if (*iVector==pNode) {
                    m_vNodes.erase(iVector);
                    break;
                }
            }
            delete pNode;
        }
    }
}

// ISystemPersistencyNode
void CConfigFileNode::Clear()
{
    m_bHasValue=false;
    m_sValue="";

    std::multimap<std::string,CConfigFileNode*>::iterator i;
    for (i=m_mNodes.begin();i!=m_mNodes.end();i++)
    {
        CConfigFileNode *pNode=i->second;
        delete pNode;
    }
    m_mNodes.clear();
    m_vNodes.clear();
}

const char *CConfigFileNode::GetName() {
    return m_sName.c_str();
}
const char *CConfigFileNode::GetValue() {
    return m_bHasValue?m_sValue.c_str():NULL;
}
void CConfigFileNode::SetValue(const char *psValue) {
    m_bHasValue=true;
    m_sValue=psValue;
}
void CConfigFileNode::SetValue(std::string &sValue) {
    m_bHasValue=true;
    m_sValue=sValue;
}
void CConfigFileNode::RemoveValue() {
    m_bHasValue=false;
    m_sValue="";
}

std::string CConfigFileNode::GetDebugInfoPath()
{
    if (m_pParent)
    {
        return m_pParent->GetDebugInfoPath()+"\\"+m_sName;
    }
    else
    {
        return m_sParentFileName;
    }
}

void CConfigFileNode::SetFileName( std::string &sFileName )
{
    m_sParentFileName=sFileName;
}

CConfigFile::CConfigFile(void)
{
    m_dwSaveTabCount=0;
    m_RootNode.m_sName="*ROOT*";
    m_pFile=NULL;
    m_pBuffer=NULL;
}

CConfigFile::~CConfigFile(void)
{
}

char *CConfigFile::RemoveSpacesAndTabs(char *pString)
{
    int nLength=strlen(pString);
    while (nLength && (pString[nLength-1]==' ' || pString[nLength-1]=='\t')) {
        nLength--;
    }
    pString[nLength]=0;
    while (*pString==' ' || *pString=='\t') {
        pString++;
    }
    return pString;
}

void CConfigFile::UnescapeString(char *pString)
{
    const char *pReadCursor=pString;
    char *pWriteCursor=pString;
    while (*pReadCursor)
    {
        if (pReadCursor[0]=='\\' && pReadCursor[1]=='\\')
        {
            *pWriteCursor++='\\';
            pReadCursor+=2;
        }
        else if (pReadCursor[0]=='\\' && pReadCursor[1]=='n')
        {
            *pWriteCursor++='\n';
            pReadCursor+=2;
        }
        else if (pReadCursor[0]=='\\' && pReadCursor[1]=='r')
        {
            *pWriteCursor++='\r';
            pReadCursor+=2;
        }
        else if (pReadCursor[0]=='\\' && pReadCursor[1]=='"')
        {
            *pWriteCursor++='"';
            pReadCursor+=2;
        }
        else
        {
            *pWriteCursor++=*pReadCursor++;
        }
    }
    *pWriteCursor=0;
}

char *CConfigFile::EscapeString(const char *pString)
{
    char *pNewString=new char [strlen(pString)*2+1];
    const char *pReadCursor=pString;
    char *pWriteCursor=pNewString;
    while (*pReadCursor)
    {
        if (pReadCursor[0]=='\\')
        {
            *pWriteCursor++='\\';
            *pWriteCursor++='\\';
            pReadCursor++;
        }
        else if (pReadCursor[0]=='\n')
        {
            *pWriteCursor++='\\';
            *pWriteCursor++='n';
            pReadCursor++;
        }
        else if (pReadCursor[0]=='\r')
        {
            *pWriteCursor++='\\';
            *pWriteCursor++='\r';
            pReadCursor++;
        }
        else if (pReadCursor[0]=='"')
        {
            *pWriteCursor++='\\';
            *pWriteCursor++='\"';
            pReadCursor++;
        }
        else
        {
            *pWriteCursor++=*pReadCursor++;
        }
    }
    *pWriteCursor=0;
    return pNewString;
}

bool CConfigFile::Open(std::string &sFileName) {
    return Open(sFileName.c_str());
}
bool CConfigFile::Open(const char *pFileName)
{
    unsigned int dwFileLength=0;

    m_RootNode.Clear();

    m_pFile=fopen(pFileName,"rb");
    if (m_pFile==NULL) {return false;}

	fseek(m_pFile,0,SEEK_END);
    dwFileLength=ftell(m_pFile);
	fseek(m_pFile,0,SEEK_SET);
    if (!dwFileLength) {return false;}
    
    std::string sFileName=pFileName;
    m_RootNode.SetFileName(sFileName);
    m_pBuffer=new char [dwFileLength+1];
    // se reserva el triple porque la normalizacion puede meter 2 espacio por cada llave encontrada
    char *pNormalizedBuffer=new char [dwFileLength*3+1];
    dwFileLength=fread(m_pBuffer,1,dwFileLength,m_pFile);
	m_pBuffer[dwFileLength]=0;
    strcpy(pNormalizedBuffer,m_pBuffer);

    // End of Line normalization CRLF/CR/LF -> LF (formato linux)
    char *pEOLNormalizationReadCursor=m_pBuffer;
    char *pEOLNormalizationWriteCursor=m_pBuffer;
    while (*pEOLNormalizationReadCursor)
    {
        if (pEOLNormalizationReadCursor[0]=='\r' && pEOLNormalizationReadCursor[1]=='\n')
        {
            *pEOLNormalizationWriteCursor++='\n';
            pEOLNormalizationReadCursor+=2;
        }
        else if (pEOLNormalizationReadCursor[0]=='\r')
        {
            *pEOLNormalizationWriteCursor++='\n';
            pEOLNormalizationReadCursor++;
        }
        else
        {
            *pEOLNormalizationWriteCursor++=*pEOLNormalizationReadCursor++;
        }
    }
    *pEOLNormalizationWriteCursor=0;
    // Eliminacion de comentarios.

    ENormalizationState eState=eNormalizationState_RegularCharacter;
    ENormalizationState ePreviousState=eNormalizationState_RegularCharacter;

    // usando pSearchBuffer podemos poner pToken a cero para seguir la busqueda en otro punto
    char *pNormalizationReadCursor=m_pBuffer;
    char *pNormalizationResult=pNormalizedBuffer;
    while (*pNormalizationReadCursor)
    {
        ePreviousState=eState;

        // aperturas

        if (eState==eNormalizationState_RegularCharacter && pNormalizationReadCursor[0]=='"')
        {
            eState=eNormalizationState_InString;
        }
        if (eState==eNormalizationState_RegularCharacter && pNormalizationReadCursor[0]=='/' && pNormalizationReadCursor[1]=='/')
        {
            eState=eNormalizationState_InLineComment;
        }
        if (eState==eNormalizationState_RegularCharacter && pNormalizationReadCursor[0]=='/' && pNormalizationReadCursor[1]=='*')
        {
            eState=eNormalizationState_InBlockComment;
        }

        // caracters normales y cierres
        if (eState==eNormalizationState_RegularCharacter)
        {
            if (*pNormalizationReadCursor=='{' || *pNormalizationReadCursor=='}')
            {
                *pNormalizationResult++='\n';
                *pNormalizationResult++=*pNormalizationReadCursor;
                *pNormalizationResult++='\n';
                pNormalizationReadCursor++;
            }
            else if (*pNormalizationReadCursor=='\r')
            {
                pNormalizationReadCursor++;
            }
            else
            {
                *pNormalizationResult++=*pNormalizationReadCursor;
                pNormalizationReadCursor++;
            }
        }
        else if (eState==eNormalizationState_InString)
        {

            if (pNormalizationReadCursor[0]=='\n')
            {
                // escapamos el \n para soportar textos multilinea
                *pNormalizationResult++='\\';
                *pNormalizationResult++='n';
                pNormalizationReadCursor++;
            }
            else
            {
                if (ePreviousState==eNormalizationState_InString && pNormalizationReadCursor[0]=='"')
                {
                    eState=eNormalizationState_RegularCharacter;
                }
                *pNormalizationResult++=*pNormalizationReadCursor;
                pNormalizationReadCursor++;
            }
        }
        else if (eState==eNormalizationState_InLineComment)
        {
            if (pNormalizationReadCursor[0]=='\n')
            {
                eState=eNormalizationState_RegularCharacter;
            }
            pNormalizationReadCursor++;
        }
        else if (eState==eNormalizationState_InBlockComment)
        {
            if (pNormalizationReadCursor[0]=='*' && pNormalizationReadCursor[1]=='/')
            {
                eState=eNormalizationState_RegularCharacter;
                pNormalizationReadCursor+=2;
            }
            else
            {
                pNormalizationReadCursor++;
            }
        }
    }
    *pNormalizationResult=0;

    // Se leen los nodos del archivo
    CConfigFileNode *pLastNode=&m_RootNode;
    CConfigFileNode *pCurrentNode=&m_RootNode;
    std::stack<CConfigFileNode*> sNodeStack;
    sNodeStack.push(pCurrentNode);

    // usando pSearchBuffer podemos poner pToken a cero para seguir la busqueda en otro punto
    char *pToken=NULL;
    while ((pToken=strtok(pToken?NULL:pNormalizedBuffer,"\n")))
    {
        char *pTokenNoSpaces=RemoveSpacesAndTabs(pToken);

        if (strcmp(pTokenNoSpaces,"")==0)
        {
            continue;
        }
        if (strcmp(pTokenNoSpaces,"{")==0)
        {
            pCurrentNode=pLastNode;
            sNodeStack.push(pLastNode);
        }
        else if (strcmp(pTokenNoSpaces,"}")==0)
        {
            sNodeStack.pop();
            pCurrentNode=sNodeStack.top();
        }
        else // Node or value
        {
            char *pName=pTokenNoSpaces;
            char *pValue=NULL;
            char *pEqual=strchr(pTokenNoSpaces,'=');
            if (pEqual) {
                *pEqual=0;
                pValue=pEqual+1;
            }
            if (pName) {
                pName=RemoveSpacesAndTabs(pName);
            }

            if (pValue)
            {
                pValue=RemoveSpacesAndTabs(pValue);

                // eliminar " y desescapar strings
                if (pValue && pValue[0]=='"')
                {
                    // eliminar inicial y final
                    pValue++;
                    int nValueLen=strlen(pValue);
                    if (nValueLen && pValue[nValueLen-1]=='"') {
                        pValue[nValueLen-1]=0;
                    }

                    // desescapar string (desescapamos: \ \r \n y \"
                    UnescapeString(pValue);
                }
            }


            pLastNode=new CConfigFileNode(pCurrentNode);
            pLastNode->m_sName=pName;
            pCurrentNode->m_mNodes.insert(std::pair<std::string,CConfigFileNode*>(pName,pLastNode));
            pCurrentNode->m_vNodes.push_back(pLastNode);
            pLastNode=pLastNode;

            if (pValue)
            {
                pLastNode->SetValue(pValue);
            }
        }
    }
    if (m_pBuffer) {
        delete [] m_pBuffer;
        m_pBuffer=NULL;
    }
    if (pNormalizedBuffer) {
        delete [] pNormalizedBuffer;
        pNormalizedBuffer=NULL;
    }
    fclose(m_pFile);
    m_pFile=NULL;
    m_pBuffer=NULL;
    return true;
}

bool CConfigFile::Save(std::string &sFileName) {
    return Save(sFileName.c_str());
}
bool CConfigFile::Save(const char *pFileName)
{
    m_pFile=fopen(pFileName,"wb");
    if (m_pFile)
    {
        m_dwSaveTabCount=0;
        std::string sFileName=pFileName;
        m_RootNode.SetFileName(sFileName);
        SaveNode(&m_RootNode);
        fclose(m_pFile);
        m_pFile=NULL;
        return true;
    }
    return false;
}

void CConfigFile::SaveNode(CConfigFileNode *pNode)
{
    if (pNode!=&m_RootNode)
    {
        if (pNode->m_bHasValue)
        {
            SaveValue(pNode->m_sName.c_str(),pNode->m_sValue.c_str());
        }
        else
        {
            SaveLine(pNode->m_sName.c_str());
        }

        if (pNode->GetNodeCount())
        {
            SaveBeginSection();
        }
    }
    std::multimap<std::string,CConfigFileNode*>::iterator iChild;
    for (iChild=pNode->m_mNodes.begin();iChild!=pNode->m_mNodes.end();iChild++)
    {
        CConfigFileNode *pChild=iChild->second;
        SaveNode(pChild);
    }
    if (pNode!=&m_RootNode && pNode->GetNodeCount())
    {
        SaveEndSection();
    }
}

void CConfigFile::SaveLine(const char *psValue)
{
    char sTemp[512]={0};
    memset(sTemp,'\t',m_dwSaveTabCount);
    std::string sTempLine=sTemp;
    sTempLine+=psValue;
    sTempLine+="\r\n";
    fwrite(sTempLine.c_str(),1,sTempLine.length(),m_pFile);
}

void CConfigFile::SaveValue(const char *sName,const char *sValue)
{
    std::string sLine=sName;
    sLine+="=";
    // Se determina si hay que entrecomillar.
    // se entrecomilla si hay un espacio o \r \n \" o "\"
    if (sValue[0]==0 || strchr(sValue,'\r') || strchr(sValue,'\n') || strchr(sValue,' ') || strchr(sValue,'"')|| strchr(sValue,'\\'))
    {
        char *pEscapedValue=EscapeString(sValue);
        sLine+="\"";
        sLine+=pEscapedValue;
        sLine+="\"";
        delete [] pEscapedValue;
    }
    else
    {
        sLine+=sValue;
    }
    SaveLine(sLine.c_str());
}

void CConfigFile::SaveBeginSection()
{
    SaveLine("{");
    m_dwSaveTabCount++;
}

void CConfigFile::SaveEndSection()
{
    m_dwSaveTabCount--;
    SaveLine("}");
}

ISystemPersistencyNode *CConfigFile::GetRoot() {
    return &m_RootNode;
}
ISystemPersistencyNode *CConfigFile::GetNode(std::string &id) {
    return m_RootNode.GetNode(id);
}
ISystemPersistencyNode *CConfigFile::AddNode(std::string &id) {
    return m_RootNode.AddNode(id);
}
ISystemPersistencyNode *CConfigFile::GetNode(const char *id) {
    return m_RootNode.GetNode(id);
}
ISystemPersistencyNode *CConfigFile::AddNode(const char *id) {
    return m_RootNode.AddNode(id);
}

