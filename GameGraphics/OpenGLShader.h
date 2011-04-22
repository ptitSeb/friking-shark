#pragma once

enum EUniformType
{
	eUniformType_None,
	eUniformType_Int,
	eUniformType_Float,
	eUniformType_Vector2,
	eUniformType_Vector3,
	eUniformType_Color,
	eUniformType_Matrix
};

struct SUniformData
{
	int   nValue;
	float fValue;
	CVector vVector;
	CVector vColor;
	float fAlpha;
	float matrix[16];
	
	EUniformType eType;
	bool         bModified;
	int          nLocation;
	
	SUniformData():nValue(0),fValue(0),fAlpha(0),eType(eUniformType_None),bModified(false),nLocation(-1){memset(matrix,0,sizeof(matrix));}
};

class COpenGLShader: virtual public CSystemObjectBase, virtual public IGenericShader
{
  bool        m_bTriedToCompile;
  GLhandleARB m_hVertexShader;
  GLhandleARB m_hFragmentShader;
  GLhandleARB m_hShaderProgram;

  std::string m_sVertexShader;
  std::string m_sVertexShaderCode;
  std::string m_sFragmentShader;
  std::string m_sFragmentShaderCode;
  std::string m_sPreprocessorDefinitions;
  
  std::map<std::string,SUniformData> m_mUniforms;

  bool LoadCodeFile(std::string sSourceFile,std::string *psSourceCode);
  void FreeShader();

public:
    bool Unserialize(ISystemPersistencyNode *piNode);

    // IGenericShader

	void Load(std::string sVertexShaderFile,std::string sFragmentShaderFile,std::string sPreprocessorDefinitions);
	void Create(std::string sVertexShaderCode,std::string sFragmentShaderCode,std::string sPreprocessorDefinitions);
	
	bool Compile();
	
	void AddUniform( std::string sUniformName,int bValue );
	void AddUniform( std::string sUniformName,float fValue );
	void AddUniform( std::string sUniformName,float fValue1,float fValue2);
	void AddUniform( std::string sUniformName,const CVector &vVector );
	void AddUniform( std::string sUniformName,const CVector &vColor, float fAlpha );
	void AddUniform( std::string sUniformName,double *pMatrix);
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
