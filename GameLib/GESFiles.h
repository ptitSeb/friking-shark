// GESFiles.h: interface for the GESFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GESFILES_H__F0BB4668_70BB_4E10_B453_25549A5BF17D__INCLUDED_)
#define AFX_GESFILES_H__F0BB4668_70BB_4E10_B453_25549A5BF17D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;

struct SGESDetailLevel
{
	double dMinimunDistance;
	string sFilePath;

	SGESDetailLevel():dMinimunDistance(0){}
};

struct SGESModel
{
	string sName;
	string sFilePath;

	vector<SGESDetailLevel *> vDetailLevels;

	~SGESModel()
	{
		for(unsigned x=0;x<vDetailLevels.size();x++)
		{
			SGESDetailLevel *pDetailLevel=vDetailLevels[x];
			delete pDetailLevel;
		}
	}
};

struct SGESTexture
{
	string		sName;
	string		sFilePath;
	string		sAlphaPath;
	double		bAlphaFile;
	double		bColorKey;
	COLORREF	cColorKey;

	SGESTexture():bAlphaFile(0),bColorKey(0),cColorKey(RGB(0,0,0)){};
};

struct SGESGUI
{
	string sCabinModel;
	string sCrosshairModel;
};

struct SGESFog
{
	double		dStart;
	double		dEnd;
	COLORREF	cColor;

	SGESFog():dStart(0),dEnd(0),cColor(RGB(0,0,0)){};
};

struct SGESCloudsShadow
{
	string		sTexture;
	CVector		vVelocity;

	SGESCloudsShadow(){};
};

struct SGESPlayerRoute
{
	CVector		vStart;
	CVector		vEnd;

	double		dCameraMovementSpeed;
	double		dPlayerMovementSpeed;

	double		dMovementMinRight;
	double		dMovementMaxRight;

	double		dActivationMinRight;
	double		dActivationMinUp;
	double		dActivationMinForward;
	double		dActivationMaxRight;
	double		dActivationMaxUp;
	double		dActivationMaxForward;

	SGESPlayerRoute():	dCameraMovementSpeed(5),dPlayerMovementSpeed(80),
						dMovementMinRight(0),dMovementMaxRight(0),
						dActivationMinRight(0),dActivationMinUp(0),dActivationMinForward(0),
						dActivationMaxRight(0),dActivationMaxUp(0),dActivationMaxForward(0){}
};

struct SGESDebris
{
	string	sModel;	
};

struct SGESUnitType
{
	string	sName;
	string	sModel;
	string  sDestroyedModel;
	string	sClass;
	double  dHealth;
	CVector	vMinDebrisSpeed;
	CVector	vMaxDebrisSpeed;
	double	dMinDebrisAngleSpeed;
	double	dMaxDebrisAngleSpeed;

	vector<SGESDebris *> vDebris;

	SGESUnitType():dHealth(0),dMinDebrisAngleSpeed(0),dMaxDebrisAngleSpeed(0){}
	~SGESUnitType()
	{
		for(unsigned x=0;x<vDebris.size();x++)
		{
			SGESDebris *pDebris=vDebris[x];
			delete pDebris;
		}
	}
};

struct SGESUnit
{
	string	sName;
	CVector vPosition;
	CVector vAngles;
	string  sType;
	bool	bHealthOverride;
	double  dHealth;

	SGESUnit():bHealthOverride(false),dHealth(0){}
};

struct SGESFormationUnitRoutePoint
{
	CVector vPosition;
};

struct SGESFormationUnit
{
	enum	{SGESFormationUnit_Type=1,SGESFormationUnit_StartAngles=2};

	DWORD	dwValidFields;
	CVector vStartAngles;
	string  sType;

	map<DWORD,SGESFormationUnitRoutePoint> routePoints;

	SGESFormationUnit():dwValidFields(0){}
};

struct SGESFormationTemplate
{
	string	sName;
	map<DWORD,SGESFormationUnit *> mUnits;

	~SGESFormationTemplate()
	{
		map<DWORD,SGESFormationUnit *>::iterator i;
		for(i=mUnits.begin();i!=mUnits.end();i++)
		{
			SGESFormationUnit *pUnit=i->second;
			delete pUnit;
		}
	}
};

struct SGESFormation
{
	string	sName;
	CVector vPosition;
	string  sTemplate;
	string  sActivationType;

	map<DWORD,SGESFormationUnit *> mOverloadedUnits;

	~SGESFormation()
	{
		map<DWORD,SGESFormationUnit *>::iterator i;
		for(i=mOverloadedUnits.begin();i!=mOverloadedUnits.end();i++)
		{
			SGESFormationUnit *pUnit=i->second;
			delete pUnit;
		}
	}
};

struct SGESController
{
	string  sName;
	string  sClass;
	CVector vPosition;
	string  sActivationType;
	string  sDeactivationType;
};

struct SGESSmokeConfig
{
	string sTexture;

	SGESSmokeConfig(){}
};

struct SGESExplosionConfig
{
	string sModel;

	SGESExplosionConfig(){}
};

struct SGESMachineGunConfig
{
	double dInterval;
	string sProjectileModel;
	double dProjectileCount;

	SGESMachineGunConfig():dInterval(0),dProjectileCount(0){}
};

struct SGESMissilesConfig
{
	double dInterval;
	string sProjectileModel;

	SGESMissilesConfig():dInterval(0){}
};

struct SGESScenarioSector
{
	string sModel;
	CVector vAbsMins;
	CVector vAbsMaxs;
	CVector vOffset;
	bool	bSelectedForEditing;

	SGESScenarioSector():bSelectedForEditing(true){}
};

struct SGESScenario
{
	string sModel;
	string sPlayerModel;
	string sSkyModel;
	double dFarPlane;

	double		dCameraDistance;
	double		dCameraPitch;
	double		dCameraViewAngle;

	double		dInspectionSpeed;
	CVector		vInspectionPosition;
	CVector		vInspectionAngles;

	SGESFog						*pFog;
	SGESCloudsShadow			*pCloudsShadow;
	vector<SGESScenarioSector *> vSectors;

	SGESScenario():pFog(NULL),pCloudsShadow(NULL),dFarPlane(0),dCameraDistance(160),dCameraPitch(0),dCameraViewAngle(60),dInspectionSpeed(300){}
	~SGESScenario()
	{
		if(pFog){delete pFog;}
		if(pCloudsShadow){delete pCloudsShadow;}
		for(unsigned x=0;x<vSectors.size();x++){delete vSectors[x];}
	}
};


struct SGESNode
{
	~SGESNode();

	string						sName;
	map<string,string>			mData;
	multimap<string,SGESNode*>	mNodes;

	WORD			GetWord(char *pName);
	DWORD			GetDWord(char *pName);
	float			GetFloat(char *pName);
	unsigned char	GetByte(char *pName);
	string			GetString(char *pName);
	bool			HasString(char *pName);
	CVector			GetVector(char *pName);
	COLORREF		GetColor(char *pName);

	SGESNode		*GetNode(char *pNodePath);
};

class CGESFileType
{
	DWORD			m_dwSaveTabCount;
	FILE			*m_pFile;
	char			*m_pBuffer;

	void			ReadNodeRecursive(SGESNode *pNode,double dVersion);
	void			SaveValueByte(string sName,unsigned char cValue);
	void			SaveValueFloat(string sName,double fValue);
	void			SaveValueDWord(string sName,DWORD dwValue);
	void			SaveValueWord(string sName,WORD wValue);
	void			SaveValueVector(string sName,CVector vValue);
	void			SaveValueColor(string sName,COLORREF cValue);
	void			SaveValueString(string sName,string sValue);
	void			SaveLine(string sValue);

	void			SaveBeginSection(string sName);
	void			SaveEndSection();

	map<string,DWORD> m_mKeyNames;

public:

	SGESNode							m_RootNode;
										
	map<string,SGESModel *>				m_mModels;
	map<string,SGESTexture *>			m_mTextures;
	map<string,SGESUnitType *>			m_mUnitTypes;
	map<string,SGESFormationTemplate *>	m_mFormationTemplates;

	map<string,SGESUnit *>			m_mUnits;
	map<string,SGESFormation *>		m_mFormations;
	map<string,SGESController *>	m_mControllers;

	SGESGUI					m_GUI;
	SGESScenario			m_Scenario;
	SGESPlayerRoute			m_PlayerRoute;

	SGESSmokeConfig			m_SmokeConfig;
	SGESExplosionConfig		m_ExplosionConfig;
	SGESMachineGunConfig	m_MachineGunConfig;
	SGESMissilesConfig		m_MissilesConfig;

	bool Open(const char *pFile);
	bool Save(const char *pFile);

	CGESFileType();
	virtual ~CGESFileType();

};

#endif // !defined(AFX_GESFILES_H__F0BB4668_70BB_4E10_B453_25549A5BF17D__INCLUDED_)
