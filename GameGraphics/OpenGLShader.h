#pragma once

class COpenGLShader: virtual public CSystemObjectBase, virtual public IGenericShader
{
  GLhandleARB m_hVertexShader;
  GLhandleARB m_hFragmentShader;
  GLhandleARB m_hShaderProgram;

  string m_sVertexShader;
  string m_sVertexShaderCode;
  string m_sFragmentShader;
  string m_sFragmentShaderCode;
  string m_sPreprocessorDefinitions;

  bool LoadCodeFile(string sSourceFile,string *psSourceCode);
  void FreeShader();
  bool CreateShaderInstance();

public:
    bool Unserialize(ISystemPersistencyNode *piNode);

    // IGenericShader

	void Load(string sVertexShaderFile,string sFragmentShaderFile,string sPreprocessorDefinitions);
	void Create(string sVertexShaderCode,string sFragmentShaderCode,string sPreprocessorDefinitions);

	void AddUniform( string sUniformName,int bValue );
	void AddUniform( string sUniformName,float fValue );
	void AddUniform( string sUniformName,const CVector &vVector );
	void AddUniform( string sUniformName,const CVector &vColor, float fAlpha );
	void AddUniform( string sUniformName,double *pMatrix);
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
