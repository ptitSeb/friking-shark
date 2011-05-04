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
#include "OpenGLGraphics.h"
#include "OpenGLShader.h"

COpenGLShader::COpenGLShader(void)
{
  m_hVertexShader=NULL;
  m_hFragmentShader=NULL;
  m_hShaderProgram=NULL;
  m_bTriedToCompile=false;
}

COpenGLShader::~COpenGLShader(void)
{
}

bool COpenGLShader::LoadCodeFile(std::string sSourceFile,std::string *psSourceCode)
{
	char *pSource=NULL;
	unsigned int dwSize=0;
	bool bOk=false;

	FILE *pFile=fopen(sSourceFile.c_str(),"rb");
	if(pFile!=NULL)
	{
		fseek(pFile,0,SEEK_END);
		dwSize=ftell(pFile);
		fseek(pFile,0,SEEK_SET);
		if(dwSize)
		{
			pSource=new char [dwSize+1];
			if(pSource)
			{
				if(fread(pSource,dwSize,1,pFile)==1)
				{
					pSource[dwSize]=0;
					bOk=true;
				}
			}
		}
	}
	if(pFile){fclose(pFile);pFile=NULL;}
	if(bOk)
	{
		*psSourceCode=pSource;
	}
	if(pSource){delete [] pSource;pSource=NULL;}
	m_bTriedToCompile=false;
	return bOk;
}

bool COpenGLShader::Compile()
{
  bool bOk=true;

  if(m_hShaderProgram==0 && m_bTriedToCompile){return false;}
  
  FreeShader();
  if(m_sFragmentShaderCode.length()==0 && m_sVertexShaderCode.length()==0){return false;}
  m_bTriedToCompile=true;
  
  m_hShaderProgram=glCreateProgramObjectARB();
  bOk=(m_hShaderProgram!=0);

  if(bOk && m_sVertexShaderCode.length())
  {
    m_hVertexShader=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    if(m_hVertexShader)
    {
      GLint glResult=0,iSize=m_sVertexShaderCode.length();
	  const char *psCode=m_sVertexShaderCode.c_str();
	  if(bOk){glShaderSourceARB(m_hVertexShader,1,const_cast<const GLcharARB**>(&psCode),&iSize);}
      if(bOk)
	  {
		  glCompileShaderARB(m_hVertexShader);
		  glGetObjectParameterivARB(m_hVertexShader,GL_OBJECT_COMPILE_STATUS_ARB,&glResult);
		  bOk=(glResult==1);

		  char sOuputBuffer[1024*16];
		  sOuputBuffer[0]=0;
		  int nLength=0;
		  glGetInfoLogARB(m_hVertexShader,sizeof(sOuputBuffer),&nLength,sOuputBuffer);
		  if(!bOk){RTTRACE("COpenGLShader::Compile -> Vertex Shader compilation Failed");}
		  if(sOuputBuffer[0]!=0){RTTRACE(sOuputBuffer);}
	  }
      if(bOk){glAttachObjectARB(m_hShaderProgram,m_hVertexShader);}
	}
    else
    {
      bOk=false;
    }
  }
  if(bOk && m_sFragmentShaderCode.length())
  {
    m_hFragmentShader=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    if(m_hFragmentShader)
    {
	  GLint glResult=0,iSize=m_sFragmentShaderCode.length();
	  const char *psCode=m_sFragmentShaderCode.c_str();
	  if(bOk){glShaderSourceARB(m_hFragmentShader,1,const_cast<const GLcharARB**>(&psCode),&iSize);}
      if(bOk)
	  {
		  glCompileShaderARB(m_hFragmentShader);
		  glGetObjectParameterivARB(m_hFragmentShader,GL_OBJECT_COMPILE_STATUS_ARB,&glResult);
		  bOk=(glResult==1);

		  char sOuputBuffer[1024*16];
		  sOuputBuffer[0]=0;
		  int nLength=0;
		  glGetInfoLogARB(m_hFragmentShader,sizeof(sOuputBuffer),&nLength,sOuputBuffer);
		  if(!bOk){RTTRACE("COpenGLShader::Compile -> Fragment Shader compilation Failed");}
		  if(sOuputBuffer[0]!=0){RTTRACE(sOuputBuffer);}
	  }
      if(bOk){glAttachObjectARB(m_hShaderProgram,m_hFragmentShader);}

    }
    else
    {
      bOk=false;
    }
  }
  if(bOk)
  {
    GLint glResult=0;
	glLinkProgramARB(m_hShaderProgram);
    glGetObjectParameterivARB(m_hShaderProgram,GL_OBJECT_LINK_STATUS_ARB,&glResult);
    bOk=(glResult==1);
	char sOuputBuffer[1024*16];
	sOuputBuffer[0]=0;
	int nLength=0;
	glGetInfoLogARB(m_hShaderProgram,sizeof(sOuputBuffer),&nLength,sOuputBuffer);
	if(sOuputBuffer[0]!=0){RTTRACE(sOuputBuffer);}
	RTTRACE(bOk?"COpenGLShader::Compile -> Shader link Succeeded":"COpenGLShader::Compile -> Shader link failed");
  }
  if(!bOk){FreeShader();}
  return bOk;
}

void COpenGLShader::FreeShader()
{
  if(m_hVertexShader){glDetachObjectARB(m_hShaderProgram,m_hVertexShader);glDeleteObjectARB(m_hVertexShader);m_hVertexShader=NULL;}
  if(m_hFragmentShader){glDetachObjectARB(m_hShaderProgram,m_hFragmentShader);glDeleteObjectARB(m_hFragmentShader);m_hFragmentShader=NULL;}
  if(m_hShaderProgram){glDeleteObjectARB(m_hShaderProgram);m_hShaderProgram=NULL;}
  std::map<std::string,SUniformData>::iterator i;
  for(i=m_mUniforms.begin();i!=m_mUniforms.end();i++)
  {
	  i->second.bModified=true;
	  i->second.nLocation=-1;
  }
}

bool COpenGLShader::Activate()
{  
	if(m_hShaderProgram==0)
	{
	  if(!Compile()){return false;}
	}
  
	glUseProgramObjectARB(m_hShaderProgram);
  
	std::map<std::string,SUniformData>::iterator i;
	for(i=m_mUniforms.begin();i!=m_mUniforms.end();i++)
	{
		SUniformData *pData=&i->second;
		if(pData->nLocation==-1){pData->nLocation=glGetUniformLocationARB(m_hShaderProgram, i->first.c_str());}
		if(!pData->bModified){continue;}
		pData->bModified=false;
		
		switch(i->second.eType)
		{
			case eUniformType_Int:
				glUniform1iARB(pData->nLocation, pData->nValue);
			break;
			case eUniformType_Float:
				glUniform1fARB(pData->nLocation, pData->fValue);
			break;
			case eUniformType_Vector2:
				glUniform2fARB(pData->nLocation, (float)pData->vVector.c[0],(float)pData->vVector.c[1]);
			break;
			case eUniformType_Vector3:
				glUniform3fARB(pData->nLocation, (float)pData->vVector.c[0],(float)pData->vVector.c[1],(float)pData->vVector.c[2]);
				break;
			case eUniformType_Color:
				glUniform4fARB(pData->nLocation, (float)pData->vColor.c[0],(float)pData->vColor.c[1],(float)pData->vColor.c[2],(float)pData->fAlpha);
			break;
			case eUniformType_Matrix:
				glUniformMatrix4fvARB(pData->nLocation,  1,true,pData->matrix);
			break;
			default:
				break;
		}
	}
	
	
	return true;
}

void COpenGLShader::AddUniform( std::string sUniformName,int nValue )
{
	std::map<std::string,SUniformData>::iterator i;
	i=m_mUniforms.find(sUniformName);
	if(i==m_mUniforms.end())
	{
		SUniformData data;
		i=m_mUniforms.insert(std::pair<std::string,SUniformData>(sUniformName,data)).first;
	}
	
	SUniformData *pData=&i->second;
	pData->eType=eUniformType_Int;
	pData->nValue=nValue;
	pData->bModified=true;
}

  void COpenGLShader::AddUniform( std::string sUniformName,float fValue )
  {
	  std::map<std::string,SUniformData>::iterator i;
	  i=m_mUniforms.find(sUniformName);
	  if(i==m_mUniforms.end())
	  {
		  SUniformData data;
		  i=m_mUniforms.insert(std::pair<std::string,SUniformData>(sUniformName,data)).first;
	  }
	  
	  SUniformData *pData=&i->second;
	  pData->eType=eUniformType_Float;
	  pData->fValue=fValue;
	  pData->bModified=true;
  }
  
  void COpenGLShader::AddUniform( std::string sUniformName,float fValue1,float fValue2)
  {
	  std::map<std::string,SUniformData>::iterator i;
	  i=m_mUniforms.find(sUniformName);
	  if(i==m_mUniforms.end())
	  {
		  SUniformData data;
		  i=m_mUniforms.insert(std::pair<std::string,SUniformData>(sUniformName,data)).first;
	  }
	  
	  SUniformData *pData=&i->second;
	  pData->eType=eUniformType_Vector2;
	  pData->vVector.c[0]=fValue1;
	  pData->vVector.c[1]=fValue2;
	  pData->bModified=true;
  }
  
  void COpenGLShader::AddUniform( std::string sUniformName,const CVector &vVector )
  {
	  std::map<std::string,SUniformData>::iterator i;
	  i=m_mUniforms.find(sUniformName);
	  if(i==m_mUniforms.end())
	  {
		  SUniformData data;
		  i=m_mUniforms.insert(std::pair<std::string,SUniformData>(sUniformName,data)).first;
	  }
	  
	  SUniformData *pData=&i->second;
	  pData->eType=eUniformType_Vector3;
	  pData->vVector=vVector;
	  pData->bModified=true;
	  
}
  
void COpenGLShader::AddUniform( std::string sUniformName,const CVector &vColor, float fAlpha )
{
	  std::map<std::string,SUniformData>::iterator i;
	  i=m_mUniforms.find(sUniformName);
	  if(i==m_mUniforms.end())
	  {
		  SUniformData data;
		  i=m_mUniforms.insert(std::pair<std::string,SUniformData>(sUniformName,data)).first;
	  }
	  
	  SUniformData *pData=&i->second;
	  pData->eType=eUniformType_Color;
	  pData->vColor=vColor;
	  pData->fAlpha=fAlpha;
	  pData->bModified=true;
}
  
  void COpenGLShader::AddUniform( std::string sUniformName,double *pMatrix)
  {
	  std::map<std::string,SUniformData>::iterator i;
	  i=m_mUniforms.find(sUniformName);
	  if(i==m_mUniforms.end())
	  {
		  SUniformData data;
		  i=m_mUniforms.insert(std::pair<std::string,SUniformData>(sUniformName,data)).first;
	  }
	  
	  SUniformData *pData=&i->second;
	  pData->eType=eUniformType_Matrix;
	  pData->bModified=true;
	  for(int x=0;x<16;x++)
	  {
		  pData->matrix[x]=(float)pMatrix[x];
	  }
  }  

void COpenGLShader::Deactivate()
{
  if(m_hShaderProgram){glUseProgramObjectARB(NULL);}
}

bool COpenGLShader::Unserialize(ISystemPersistencyNode *piNode)
{
  bool bOk=CSystemObjectBase::Unserialize(piNode);
  std::string sVertexShaderCode;
  std::string sFragmentShaderCode;
  if(m_sVertexShader.c_str()){LoadCodeFile(m_sVertexShader,&sVertexShaderCode);}
  if(m_sFragmentShader.c_str()){LoadCodeFile(m_sFragmentShader,&sFragmentShaderCode);}
  m_sVertexShaderCode=m_sPreprocessorDefinitions+"\n"+sVertexShaderCode;
  m_sFragmentShaderCode=m_sPreprocessorDefinitions+"\n"+sFragmentShaderCode;
  m_bTriedToCompile=false;
  return bOk;
}

void COpenGLShader::Load( std::string sVertexShaderFile,std::string sFragmentShaderFile,std::string sPreprocessorDefinitions)
{
	m_sFragmentShader=sFragmentShaderFile;
	m_sFragmentShaderCode="";
	m_sVertexShader=sVertexShaderFile;
	m_sVertexShaderCode="";
	m_sPreprocessorDefinitions=sPreprocessorDefinitions;
	std::string sVertexShaderCode;
	std::string sFragmentShaderCode;
	if(m_sVertexShader.length()){LoadCodeFile(m_sVertexShader,&sVertexShaderCode);}
	if(m_sFragmentShader.length()){LoadCodeFile(m_sFragmentShader,&sFragmentShaderCode);}
	m_sVertexShaderCode=sPreprocessorDefinitions+"\n"+sVertexShaderCode;
	m_sFragmentShaderCode=sPreprocessorDefinitions+"\n"+sFragmentShaderCode;
	m_bTriedToCompile=false;
}

void COpenGLShader::Create( std::string sVertexShaderCode,std::string sFragmentShaderCode,std::string sPreprocessorDefinitions)
{
	m_sFragmentShader="";
	m_sFragmentShaderCode=sPreprocessorDefinitions+"\n"+sFragmentShaderCode;
	m_sVertexShader="";
	m_sVertexShaderCode=sPreprocessorDefinitions+"\n"+sVertexShaderCode;
	m_sPreprocessorDefinitions=sPreprocessorDefinitions;
	m_bTriedToCompile=false;
}
