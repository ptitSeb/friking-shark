#pragma once

#include <GL/gl.h>

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
	virtual void SetCurrentRenderTarget(bool bSetAsCurrent)=0;
};

class IOpenGLRender: virtual public ISystemUnknown
{
public:

	virtual IOpenGLViewport *GetOpenGLViewPort()=0; // solo valido entre StartFrame y EndFrame.
};
