#pragma once

#include <gl\gl.h>
#include <gl\glu.h>

class CPreprocessedRenderBuffer;

class IOpenGLTexture: virtual public ISystemUnknown
{
public:

	virtual unsigned GetOpenGLIndex()=0;
};
class IOpenGLModel:virtual public ISystemUnknown
{
public:
	virtual void PrepareRenderBuffer(IGenericRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow)=0;
	virtual void CallRenderBuffer(IGenericRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer)=0;
	virtual void UnPrepareRenderBuffer(IGenericRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow)=0;
};
class IOpenGLViewport: virtual public ISystemUnknown
{
public:

	virtual HGLRC GetRenderContext()=0;
	virtual HDC	 GetDeviceContext()=0;

	virtual void SetCurrentRenderTarget(bool bSetAsCurrent)=0;
};

class IOpenGLRender: virtual public ISystemUnknown
{
public:

	virtual IOpenGLViewport *GetOpenGLViewPort()=0; // solo valido entre StartFrame y EndFrame.
};
