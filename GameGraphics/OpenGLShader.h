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

enum EUniformType
{
	eUniformType_None,
	eUniformType_Int,
	eUniformType_Float,
	eUniformType_Vector2,
	eUniformType_Vector3,
	eUniformType_Color,
	eUniformType_Matrix,
	eUniformType_IntArray,
	eUniformType_FloatArray,
	eUniformType_Float2Array,
	eUniformType_Vector3Array,
	eUniformType_ColorArray,
	eUniformType_MatrixArray,
};

union SUniformUnion
{
	int   nInteger;
	float fFloats[16];
};

struct SUniformData
{
	SUniformUnion data;
	
	EUniformType eType;
	bool         bModified;
	bool         bTemporal;
	int          nLocation;
	bool         bTriedToGetLocation;
	unsigned int nElements;
	char        *pBuffer;
	
	SUniformData():eType(eUniformType_None),bModified(false),bTemporal(false),nLocation(-1),nElements(0),pBuffer(NULL),bTriedToGetLocation(false)
	{
		memset(&data,0,sizeof(data));
	}
	~SUniformData()
	{
		delete [] pBuffer;
		pBuffer=NULL;
	}
};

struct SAttributeData
{
	int   nIndex;
	bool  bModified;
	
	SAttributeData():nIndex(-1),bModified(false){}
};

class COpenGLShader: virtual public CSystemObjectBase, virtual public IGenericShader
{
  bool        m_bTriedToCompile;
  bool        m_bActive;
  GLhandleARB m_hVertexShader;
  GLhandleARB m_hFragmentShader;
  GLhandleARB m_hShaderProgram;
  
  std::string m_sVertexShader;
  std::string m_sVertexShaderCode;
  std::string m_sFragmentShader;
  std::string m_sFragmentShaderCode;
  std::string m_sPreprocessorDefinitions;
  
  std::map<std::string,SUniformData*>  m_mUniforms;
  std::map<std::string,SAttributeData> m_mAttributes;
  
  bool LoadCodeFile(std::string sSourceFile,std::string *psSourceCode);
  void FreeShader();

public:
    bool Unserialize(ISystemPersistencyNode *piNode);

    // IGenericShader

	void Load(std::string sVertexShaderFile,std::string sFragmentShaderFile,std::string sPreprocessorDefinitions);
	void Create(std::string sVertexShaderCode,std::string sFragmentShaderCode,std::string sPreprocessorDefinitions);
	
	bool Compile();
	
	void AddUniform( std::string sUniformName,int nValue,bool temporal);
	void AddUniform( std::string sUniformName,float fValue,bool temporal );
	void AddUniform( std::string sUniformName,float fValue1,float fValue2,bool temporal );
	void AddUniform( std::string sUniformName,const CVector &vVector,bool temporal );
	void AddUniform( std::string sUniformName,const CVector &vColor, float fAlpha,bool temporal );
	void AddUniform( std::string sUniformName,CMatrix &matrix,bool temporal);
	
	void AddUniformIntegers( std::string sUniformName,unsigned int nValues,int *pValues,bool temporal);
	void AddUniformFloats( std::string sUniformName,unsigned int nValues,float *pValues,bool temporal);
	void AddUniformVectors( std::string sUniformName,unsigned int nValues,const CVector *pvVectors,bool temporal);
	void AddUniformColors( std::string sUniformName,unsigned int nValues,const CVector *pvColor, float *pvfAlphas,bool temporal);
	void AddUniformMatrixes( std::string sUniformName,unsigned int nValues,double *pMatrixes,bool temporal);
	void AddUniformMatrixes( std::string sUniformName,unsigned int nValues,float *pMatrixes,bool temporal);
	
	void AddAttribute( std::string sAttributeName,int nIndex);
	
	bool Activate();
    void Deactivate();
	
	// Propiedades Persistentes

    BEGIN_PROP_MAP(COpenGLShader);
      PROP_FLAGS(m_sVertexShader,"VertexShaderFile",MRPF_NORMAL|MRPF_OPTIONAL);
      PROP_FLAGS(m_sFragmentShader,"FragmentShaderFile",MRPF_NORMAL|MRPF_OPTIONAL);
	  PROP_FLAGS(m_sPreprocessorDefinitions,"PreprocessorDefinitions",MRPF_NORMAL|MRPF_OPTIONAL);
    END_PROP_MAP();

    COpenGLShader();
    ~COpenGLShader();
};
