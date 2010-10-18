// GUIManager.cpp: implementation of the CGUIManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameEngine.h"
#include "GUIManager.h"
#include "winver.h"

extern double g_dCurrentFps;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGUIManager::CGUIManager()
{
    
    m_sName="GUIManager";

    
    m_dwState=GUI_STATE_LOADING_SCENARIO;
	m_pLoadTexture=NULL;
}

CGUIManager::~CGUIManager()
{
}

void CGUIManager::Load(CGESFileType *pFile){}
void CGUIManager::Render()
{
	if(m_dwState==GUI_STATE_LOADING_SCENARIO)
	{
		VS_FIXEDFILEINFO *pFixedVersion=NULL;
		DWORD handle=NULL;
		char sVersion[MAX_PATH]={0};
		char sFile[MAX_PATH]={0};
		GetModuleFileName(NULL,sFile,MAX_PATH);
		DWORD dwSize=GetFileVersionInfoSize(sFile,&handle);
		if(dwSize)
		{
			UINT len=0;
			char *pVersionInfo=new char [dwSize];		
			GetFileVersionInfo(sFile,NULL,dwSize,pVersionInfo);
			VerQueryValue(pVersionInfo,"\\",(LPVOID*)&pFixedVersion,&len);
			if(pFixedVersion)
			{
				sprintf(sVersion,"GameEngine v%d.%d.%d.%d",HIWORD(pFixedVersion->dwFileVersionMS),LOWORD(pFixedVersion->dwFileVersionMS),HIWORD(pFixedVersion->dwFileVersionLS),LOWORD(pFixedVersion->dwFileVersionLS));
			}
			delete [] pVersionInfo;
		}
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-100,100,-100,100,-100,100);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glColor3d(1,1,1);
		if(m_pLoadTexture)
		{
			double dAspectRatio=CGlobals::g_pScreen->GetAspectRatio();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,m_pLoadTexture->m_nOpenGlIndex);
			CGlobals::g_pRender->RenderTextureRect(m_pLoadTexture,Origin,AxisPosX,AxisPosY,200*dAspectRatio,200);
		}
		glDisable(GL_TEXTURE_2D);
		COpenGLFonts::SetCurrentFont("Arial",30);
		COpenGLFonts::DrawText(-95,-80,m_sLoadString.c_str());
		COpenGLFonts::SetCurrentFont("Arial",14);
		COpenGLFonts::DrawText(70,90,sVersion);
		glPopAttrib();
	}
	else if(m_dwState==GUI_STATE_PLAYING)
	{
		CGlobals::g_pRender->Render();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-100,100,-100,100,-100,100);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
		glColor3d(1,1,1);
        DWORD dwMinutes=CGlobals::g_dwCurrentTime/(1000*60);
        DWORD dwSeconds=CGlobals::g_dwCurrentTime/1000-dwMinutes*60;
        DWORD dwMilliseconds=CGlobals::g_dwCurrentTime-(dwSeconds*1000+dwMinutes*60*1000);
		COpenGLFonts::DrawText(-95,95,"%f fps",g_dCurrentFps);
        COpenGLFonts::DrawText(-95,90,"%02d:%02d.%03d %s",dwMinutes,dwSeconds,dwMilliseconds,theApp.m_bPaused?"  <PAUSED>":"");
		glPopAttrib();
	}
	else if(m_dwState==GUI_STATE_SHOW_SHADOW_BUFFER)
	{
	/*	if(CGlobals::g_pWorld)
		{
			double dAspectRatio=CGlobals::g_pScreen->GetAspectRatio();

			CGlobals::g_pWorld->GenerateShadows();

			glEnable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-100,100,-100,100,-100,100);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glActiveTexture(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D,CGlobals::g_pWorld->m_nShadowTextureIndex);
			wglBindTexImageARB(CGlobals::g_pWorld->m_hShadowPBuffer,WGL_FRONT_LEFT_ARB);
			glActiveTexture(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D); // para desactivar la textura 1

			double s1=100,s2=100;
			if(SHADOW_BUFFER_SIZE_W>SHADOW_BUFFER_SIZE_H)
			{
				s2=s1*(SHADOW_BUFFER_SIZE_W/SHADOW_BUFFER_SIZE_H);
			}
			else
			{
				s1=s2*(SHADOW_BUFFER_SIZE_H/SHADOW_BUFFER_SIZE_W);
			}
			s2/=dAspectRatio;
			glBegin(GL_QUADS);
			glTexCoord2f(1,0);
			glVertex3dv((Origin+AxisPosX*(s1/2.0)+AxisPosY*(s2/2.0)).c);
			glTexCoord2f(1,1);
			glVertex3dv((Origin-AxisPosX*(s1/2.0)+AxisPosY*(s2/2.0)).c);
			glTexCoord2f(0,1);
			glVertex3dv((Origin-AxisPosX*(s1/2.0)-AxisPosY*(s2/2.0)).c);
			glTexCoord2f(0,0);
			glVertex3dv((Origin+AxisPosX*(s1/2.0)-AxisPosY*(s2/2.0)).c);
			glEnd();
			wglReleaseTexImageARB(CGlobals::g_pWorld->m_hShadowPBuffer,WGL_FRONT_LEFT_ARB);
		}*/
	}
}

void CGUIManager::SetState(DWORD dwState)
{
	m_dwState=dwState;
	if(m_dwState==GUI_STATE_LOADING_SCENARIO)
	{
		COpenGLFonts::SetCurrentFont("Arial",30);
		m_pLoadTexture=CGlobals::g_pResourceStore->LoadTexture("Resources\\GUI\\Loading.jpg");
		CGlobals::g_pScreen->Redraw();
		CGlobals::g_pScreen->FlushMessages();
	}
	else
	{
		COpenGLFonts::SetCurrentFont("Arial",14);
	}
}

void CGUIManager::SetLoadString(const char *format, ...)
{
	va_list vargs;
	va_start (vargs,format);
	char pTempBuffer[2400];
	int res=_vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);
	m_sLoadString=pTempBuffer;
	CGlobals::g_pScreen->Redraw();
	CGlobals::g_pScreen->FlushMessages();
    CGlobals::g_pScreen->ActivateRenderContext();
}
