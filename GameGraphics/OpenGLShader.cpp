#include "./stdafx.h"
#include "OpenGLGraphics.h"
#include "OpenGLShader.h"

COpenGLShader::COpenGLShader(void)
{
  m_hVertexShader=NULL;
  m_hFragmentShader=NULL;
  m_hShaderProgram=NULL;
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
	return bOk;
}

bool COpenGLShader::CreateShaderInstance()
{
  bool bOk=true;

  FreeShader();

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
		  RTTRACE("Compiling Vertex shader...");

		  glCompileShaderARB(m_hVertexShader);
		  glGetObjectParameterivARB(m_hVertexShader,GL_OBJECT_COMPILE_STATUS_ARB,&glResult);
		  bOk=(glResult==1);

		  char sOuputBuffer[1024*16];
		  sOuputBuffer[0]=0;
		  int nLength=0;
		  glGetInfoLogARB(m_hVertexShader,sizeof(sOuputBuffer),&nLength,sOuputBuffer);
		  if(sOuputBuffer[0]!=0){RTTRACE(sOuputBuffer);}
		  RTTRACE(bOk?"Vertex Shader compilation Succeded":"Vertex Shader compilation Failed");
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
		  RTTRACE("Compiling Fragment shader...");

		  glCompileShaderARB(m_hFragmentShader);
		  glGetObjectParameterivARB(m_hFragmentShader,GL_OBJECT_COMPILE_STATUS_ARB,&glResult);
		  bOk=(glResult==1);

		  char sOuputBuffer[1024*16];
		  sOuputBuffer[0]=0;
		  int nLength=0;
		  glGetInfoLogARB(m_hFragmentShader,sizeof(sOuputBuffer),&nLength,sOuputBuffer);
		  if(sOuputBuffer[0]!=0){RTTRACE(sOuputBuffer);}
		  RTTRACE(bOk?"Fragment Shader compilation Succeded":"Fragment Shader compilation Failed");
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
	RTTRACE("Linking program");
    glLinkProgramARB(m_hShaderProgram);
    glGetObjectParameterivARB(m_hShaderProgram,GL_OBJECT_LINK_STATUS_ARB,&glResult);
    bOk=(glResult==1);
	char sOuputBuffer[1024*16];
	sOuputBuffer[0]=0;
	int nLength=0;
	glGetInfoLogARB(m_hShaderProgram,sizeof(sOuputBuffer),&nLength,sOuputBuffer);
	if(sOuputBuffer[0]!=0){RTTRACE(sOuputBuffer);}
	RTTRACE(bOk?"Shader link Succeeded":"Shader link failed");
  }
  if(!bOk){FreeShader();}
  return bOk;
}

void COpenGLShader::FreeShader()
{
  if(m_hVertexShader){glDetachObjectARB(m_hShaderProgram,m_hVertexShader);glDeleteObjectARB(m_hVertexShader);m_hVertexShader=NULL;}
  if(m_hFragmentShader){glDetachObjectARB(m_hShaderProgram,m_hFragmentShader);glDeleteObjectARB(m_hFragmentShader);m_hFragmentShader=NULL;}
  if(m_hShaderProgram){glDeleteObjectARB(m_hShaderProgram);m_hShaderProgram=NULL;}
}

bool COpenGLShader::Activate()
{  
  if(m_hShaderProgram==0){return false;}
  glUseProgramObjectARB(m_hShaderProgram);

  return true;
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
  CreateShaderInstance();
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
	CreateShaderInstance();
}

void COpenGLShader::Create( std::string sVertexShaderCode,std::string sFragmentShaderCode,std::string sPreprocessorDefinitions)
{
	m_sFragmentShader="";
	m_sFragmentShaderCode=sPreprocessorDefinitions+"\n"+sFragmentShaderCode;
	m_sVertexShader="";
	m_sVertexShaderCode=sPreprocessorDefinitions+"\n"+sVertexShaderCode;
	m_sPreprocessorDefinitions=sPreprocessorDefinitions;
	CreateShaderInstance();
}

void COpenGLShader::AddUniform( std::string sUniformName,int bValue )
{
	int location=glGetUniformLocationARB(m_hShaderProgram, sUniformName.c_str()); 
	glUniform1iARB(location, bValue);

}

void COpenGLShader::AddUniform( std::string sUniformName,float fValue )
{
	int location=glGetUniformLocationARB(m_hShaderProgram, sUniformName.c_str()); 
	glUniform1fARB(location, (float)fValue);

}

void COpenGLShader::AddUniform( std::string sUniformName,const CVector &vVector )
{
	int location=glGetUniformLocationARB(m_hShaderProgram, sUniformName.c_str()); 
	glUniform3fARB(location, (float)vVector.c[0],(float)vVector.c[1],(float)vVector.c[2]);
}

void COpenGLShader::AddUniform( std::string sUniformName,const CVector &vColor, float fAlpha )
{
	int location=glGetUniformLocationARB(m_hShaderProgram, sUniformName.c_str()); 
	glUniform4fARB(location, (float)vColor.c[0],(float)vColor.c[1],(float)vColor.c[2],(float)fAlpha);
}

void COpenGLShader::AddUniform( std::string sUniformName,double *pMatrix)
{
	int location=glGetUniformLocationARB(m_hShaderProgram, sUniformName.c_str()); 
	float fTemp[16];
	for(int x=0;x<16;x++)
	{
		fTemp[x]=(float)pMatrix[x];
	}
	glUniformMatrix4fvARB(location, 1,true,fTemp);
}