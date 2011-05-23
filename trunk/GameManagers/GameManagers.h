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


#ifndef __GAME_MANAGERS__
#define __GAME_MANAGERS__


#define PHYSIC_METER						1.0
#define PHYSIC_SECOND						1000.0

#define PHYSIC_FORCE_NORMAL					1 // es una fuerza normal, que se aplica y genera una aceleracion que depende de la masa del objeto
#define PHYSIC_FORCE_CONSTANT_ACCELERATION	2 // es un fuerza que es proporcional a la masa del objeto (la gravedad), genera una aceleracion constante.
#define PHYSIC_FORCE_CONSTANT_VELOCITY		3 // es velocidad constante aplicada a la velocidad final del objeto

#define PHYSIC_MOVE_TYPE_NONE				0 // Para entidades que no se mueven
#define PHYSIC_MOVE_TYPE_NORMAL				1
#define PHYSIC_MOVE_TYPE_FLY				2
#define PHYSIC_MOVE_TYPE_CUSTOM				3

#define PHYSIC_BOUNDS_TYPE_NONE				0
#define PHYSIC_BOUNDS_TYPE_BBOX				1
#define PHYSIC_BOUNDS_TYPE_BSP				2

#define PHYSIC_COLLISION_TYPE_NONE			0
#define PHYSIC_COLLISION_TYPE_STUCK			1
#define PHYSIC_COLLISION_TYPE_SLIDE			2
#define PHYSIC_COLLISION_TYPE_BOUNCE		3
#define PHYSIC_COLLISION_TYPE_THROUGH		4 // el objeto no modifica su posicion en la colision "pasa a traves"

#define ENTITY_ALIGNMENT_NEUTRAL		  0x0000
#define ENTITY_ALIGNMENT_PLAYER 		  0x0001
#define ENTITY_ALIGNMENT_ENEMIES      0x0002

#define DAMAGE_TYPE_NONE    0
#define DAMAGE_TYPE_NORMAL  1

class IGenericModel;
class IGenericCamera;
class IGenericTexture;
class IGenericRender;
class IGameGUIManager;

struct IEntity;
struct IWeapon;
struct IEntityType;
struct IFormationType;
struct IPlayAreaElement;
struct IAnimationType;

using namespace std;

struct SRoutePoint
{
public:
	
	bool         bAbsolutePoint; // si es falso son valores de 0 a 1 relativos al plano de juego aereo
	unsigned int nPause;
	CVector      vPosition;
	
	SRoutePoint(){nPause=0;bAbsolutePoint=true;}
	SRoutePoint(bool absolutePoint,CVector position,unsigned int pause=0){vPosition=position;bAbsolutePoint=absolutePoint;nPause=pause;}
};


struct SPhysicForce
{
    unsigned int   dwForceType;
    CVector vDir;
    double  dConstantAccel;
	double  dConstantVel;
	double  dForce;
    double	dMaxVelocity;

    SPhysicForce()
    {
        dwForceType=PHYSIC_FORCE_NORMAL;
        dConstantAccel=0.0;
		dConstantVel=0.0;
		dForce=0;
        dMaxVelocity=0;
    }
}; 

struct SPhysicInfo
{
    unsigned int			dwMoveType;
    unsigned int			dwBoundsType;
    unsigned int			dwCollisionType;
    double					dBounceFactor;
    double					dSlideFactor;

    CVector					vPosition;
    CVector					vVelocity;
    double					dMass;
    CVector					vAngles;
    CVector					vAngleVelocity;
	CVector					vForward;
    CVector					vMins;
    CVector					vMaxs;
    vector<SPhysicForce>	vForces;
    SPhysicForce			fOwnForce;
	double					dMaxVelocity;
	double					dMaxForce;
	bool					bOnSurface;
	CPlane					surfacePlane;
	
	CVector	 				vRefSysX;
	CVector	 				vRefSysY;
	CVector	 				vRefSysZ;
	CVector					vLocalAngles;
	CVector					vLocalPosition;
		
	SPhysicInfo():vRefSysX(AxisPosX),vRefSysY(AxisPosY),vRefSysZ(AxisPosZ)
    {
        dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
        dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
        dwCollisionType=PHYSIC_COLLISION_TYPE_STUCK;
        dMass=1.0;
        dBounceFactor=1.0;
        dSlideFactor=1.0;
		dMaxVelocity=0;
		dMaxForce=0;
		bOnSurface=false;
    }
};

struct IPhysicManager:virtual public ISystemUnknown
{
public:

    virtual CVector ProcessPhysicInfo(SPhysicInfo *pInfo,double dInterval)=0;
};

struct IRoute
{
    virtual unsigned    GetPointCount()=0;
    virtual CVector     GetAbsolutePoint(unsigned nIndex)=0;
	virtual	CVector     GetEstimatedAbsolutePoint(unsigned int nIndex,double dTime)=0;
	virtual CVector     GetDirection(unsigned nSection)=0;
    virtual unsigned    GetNextPointIndex(unsigned nIndex)=0;
    virtual void        ProcessPoint(IEntity *piEntity,unsigned int dwCurrentFrame,double dTimeFraction)=0;
	
	virtual bool 		 GetPoint(unsigned int nIndex,SRoutePoint *psPoint)=0;
	virtual bool 		 AddPoint(unsigned int nIndex,const SRoutePoint &sPoint)=0;
	virtual bool 		 SetPoint(unsigned int nIndex,const SRoutePoint &sPoint)=0;
	virtual void		 RemovePoint(unsigned int nIndex)=0;
	virtual void 		 Clear()=0;	
};

struct IEntity:virtual public IPublisher
{
public:
    virtual string      *GetEntityClass()=0;
    virtual string      *GetEntityName()=0;
    virtual SPhysicInfo *GetPhysicInfo()=0;

	virtual unsigned int GetDamageType()=0;
	virtual unsigned int GetAlignment()=0;
    virtual void         SetAlignment(unsigned int dwAlignment)=0;
	
	virtual void         Kill()=0;
	
	virtual IEntity 	*GetTarget()=0;
	virtual void	 	  SetTarget(IEntity *piTarget)=0;	

	virtual IEntity      *GetParent()=0;
	virtual void          SetParent(IEntity *pEntity)=0;
	virtual void          AddChild(IEntity *pEntity,CVector vPos,CVector vAngles)=0;
	virtual void          RemoveChild(IEntity *pEntity)=0;
	virtual void          SetChildLocation(IEntity *piEntity,CVector vPosition,CVector vAngles)=0;
	virtual void          GetChildLocation(IEntity *piEntity,CVector &vPosition,CVector &vAngles)=0;
	virtual unsigned int  GetChildren()=0;
	virtual IEntity      *GetChild(unsigned int nIndex)=0;

	virtual unsigned int  AddWeapon(IWeapon *piWeapon)=0;
	virtual unsigned int  GetWeapons()=0;
	virtual IWeapon		 *GetWeapon(unsigned int nIndex)=0;
	
	virtual double GetHealth()=0;
    virtual double GetMaxHealth()=0;

    virtual bool  IsRemoved()=0;
    virtual void  Remove()=0;

	virtual void Render(IGenericRender *piRender,IGenericCamera *piCamera)=0;
	
	virtual void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)=0;
    virtual void ProcessAnimations(unsigned int dwCurrentTime,double dTimeFraction,bool *pbAnimationsFinished)=0;
    virtual unsigned int GetNextProcessFrame()=0;

    virtual bool OnCollision(IEntity *piOther,CVector &vCollisionPos)=0;
    virtual void OnDamage(double dDamage,IEntity *pAggresor)=0;
    virtual void OnAnimationEvent(string sEvent,string sParams)=0;


	virtual CTraceInfo GetTrace(const CVector &p1,const CVector &p2)=0;

	virtual void 	SetRouteDelay(unsigned int nDelay)=0;
	virtual void    SetRoute(IRoute *piRoute)=0;
	virtual bool    HasFinishedRoute()=0;
	virtual IRoute *GetRoute()=0;
	
    virtual ~IEntity(){}
};

struct IEntityEvents
{
public:
    virtual void OnRemoved(IEntity *piEntity){};
    virtual void OnKilled(IEntity *piEntity){};
};

typedef void (*EntityUnaryOperation)(IEntity *piEntity,void *pParam1,void *pParam2);
typedef void (*EntityBinaryOperation)(IEntity *piEntity1,IEntity *piEntity2,void *pParam1,void *pParam2);

struct IEntityManager:virtual public ISystemUnknown
{
public:
	

	virtual void AddEntity(IEntity *pEntity)=0; 
	virtual void RemoveAllEntities()=0;
	virtual unsigned long GetEntityCount()=0;

	virtual void RenderEntities(IGenericRender *piRender,IGenericCamera *piCamera)=0;

    virtual void PerformUnaryOperation(EntityUnaryOperation pFunc,void *pParam1,void *pParam2)=0;
    virtual void PerformBinaryOperation(EntityBinaryOperation pFunc,void *pParam1,void *pParam2)=0;

    virtual IEntity *FindEntity(string sName)=0;
};

struct IGameManager:virtual public ISystemUnknown
{
public:

	virtual void CreateScenario()=0;
    virtual void LoadScenario(ISystemPersistencyNode *piNode)=0;
	virtual void SaveScenario(ISystemPersistencyNode *piNode)=0;
    virtual void CloseScenario()=0;

    virtual void Start()=0;
    virtual void Stop()=0;
    virtual void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)=0;
};

struct IDesign:virtual public ISystemUnknown
{
public:
	// IDesign General settings

	virtual void DesignRender(IGenericRender *piRender)=0;
};

struct IDesignObject:virtual public ISystemUnknown
{
public:
	// IDesign General settings

	virtual void DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected)=0;
	virtual void DesignGetBBox(CVector *pvMins,CVector *pvMaxs)=0;
	virtual CTraceInfo DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )=0;
	virtual double DesignGetRadius()=0;
};

struct IEntityType:virtual public ISystemUnknown,virtual public IDesignObject
{
	virtual IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)=0;

	virtual void	GetBBox(CVector *pvMins,CVector *pvMaxs)=0;
	virtual CVector	GetSize()=0;

};

struct IFormationEvents:virtual public ISystemUnknown
{
	virtual void OnFormationRemoved(ISystemObject *piFormation)=0;
	virtual void OnFormationKilled(ISystemObject *piFormation,IEntity *piLastEntity)=0;
};

struct IFormation:virtual public ISystemUnknown
{
	virtual bool ProcessFrame(unsigned int dwCurrentTime,double dInterval)=0;
};

struct IFormationType:virtual public ISystemUnknown,virtual public IDesignObject
{
	virtual IFormation *CreateInstance(CVector vPosition,unsigned int dwCurrentTime)=0;
};

struct IFormationTypeDesign:virtual public IDesignObject
{
public:
	virtual unsigned long	AddElement()=0;
	virtual void			RemoveElement(unsigned int nIndex)=0;
	virtual unsigned int	GetElements()=0;
	
	virtual void 		SetElementEntityType(unsigned int nElement,IEntityType *piEntityType)=0;
	virtual void		GetElementEntityType(unsigned int nElement,IEntityType **piEntityType)=0;
	
	virtual void 		 SetElementEntityCount(unsigned int nElement,unsigned int nCount)=0;
	virtual unsigned int GetElementEntityCount(unsigned int nElement)=0;
	
	virtual void 		 SetElementEntityDelay(unsigned int nElement,unsigned int nMilliseconds)=0;
	virtual unsigned int GetElementEntityDelay(unsigned int nElement)=0;

	virtual void 		 SetElementEntityInterval(unsigned int nElement,unsigned int nMilliseconds)=0;
	virtual unsigned int GetElementEntityInterval(unsigned int nElement)=0;
	
	virtual unsigned int GetElementRoutePoints(unsigned int nElement)=0;
	virtual bool 		 GetElementRoutePoint(unsigned int nElement,unsigned int nIndex,SRoutePoint *psPoint)=0;
	virtual bool 		 AddElementRoutePoint(unsigned int nElement,unsigned int nIndex,const SRoutePoint &sPoint)=0;
	virtual bool 		 SetElementRoutePoint(unsigned int nElement,unsigned int nIndex,const SRoutePoint &sPoint)=0;
	virtual void		 RemoveElementRoutePoint(unsigned int nElement,unsigned int nIndex)=0;
	virtual void 		 ClearElementRoute(unsigned int nElement)=0;	
};

struct IPlayAreaCheckPoint:virtual public ISystemUnknown
{
	virtual CVector GetCheckPointPosition()=0;
};

struct SPlayAreaConfig
{
	double dCameraScroll;
	double dCameraDistance;
	double dCameraAspectRatio;
	double dCameraSpeed;
	double dCameraViewAngle;
	double dAirPlaneHeight;
	bool   bPlayerLandingEnabled;
	bool   bPlayerTakeOffEnabled;
	CVector pvPlayerTakeOffPoints[4];
	CVector pvPlayerLandingPoints[4];
	
	SPlayAreaConfig(){bPlayerLandingEnabled=false;bPlayerTakeOffEnabled=false;dCameraScroll=0;dCameraDistance=0;dCameraAspectRatio=0;dCameraSpeed=0;dCameraViewAngle=0;dAirPlaneHeight=0;}
};

struct SEntityLayer
{
	double dSeparation;
	double dPositionJitter;
	double dYawJitter;

	CVector vColor;
	double dColorSaturationMargin;
	double dColorValueMargin;

	std::string	sEntityType;

	SEntityLayer()
	{
		dSeparation=1;
		dPositionJitter=0;
		dYawJitter=0;
		dColorSaturationMargin=1;
		dColorValueMargin=0.01;
	}
};

struct IPlayAreaDesign:virtual public IDesign
{
public:

	virtual void	GetPlayAreaConfig(SPlayAreaConfig *pConfig)=0;
	virtual void	SetPlayAreaConfig(SPlayAreaConfig *pConfig)=0;
	virtual void	UpdatePlayArea()=0;

	// Element management

	virtual unsigned long	AddElement(std::string sType)=0;
	virtual void			RemoveElement(unsigned int nIndex)=0;
	virtual void			GetElement(unsigned int nIndex,IPlayAreaElement **ppiElement)=0;
	virtual unsigned int	GetElements()=0;

	// IPlayAreaDesign Color based Entity layers

	virtual unsigned long	AddEntityLayer(SEntityLayer *pLayer)=0;
	virtual void			UpdateEntityLayer(unsigned int nIndex,SEntityLayer *pLayer)=0;
	virtual void			RemoveEntityLayer(unsigned int nIndex)=0;
	virtual void			GetEntityLayer(unsigned int nIndex,SEntityLayer *pLayer,IEntityType **ppiEntityType)=0;
	virtual unsigned int	GetEntityLayers()=0;

	virtual bool			UpdateEntityLayers()=0;
};

struct IGameController:virtual public ISystemUnknown
{
	virtual bool SetupGame()=0;
	virtual void EndGame()=0;

	virtual void CreateScenario()=0;
	virtual bool LoadScenario(std::string sScenario)=0;
	virtual bool SaveScenario(std::string sScenario)=0;
	virtual void CloseScenario()=0;

    virtual void Start()=0;
    virtual void Stop()=0;
    virtual void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)=0;

    virtual bool RegisterManager(unsigned int dwHeight,IGameManager *piManager)=0;
    virtual void UnregisterManager(IGameManager *piManager)=0;
};

enum GameActivationType
{
    AT_None,
    AT_EnterInPlayArea,
    AT_PlayerPosition
};

enum GameDeactivationType
{
    DT_None,
    DT_ExitFromPlayArea,
    DT_PlayerPosition,
    DT_TimeInterval
};

struct SPlayAreaInfo
{
    enum EPlayAreaPlane {Far,Near,Left,Right,Top,Bottom,PlaneCount};

    CPlane  planes[6];
    int     nPlaneCount;
    CVector vMins;
    CVector vMaxs;

    SPlayAreaInfo(){nPlaneCount=0;}
};


#define KEY_FORWARD		1
#define KEY_BACK		2
#define KEY_RIGHT		3
#define KEY_LEFT		4
#define KEY_UP			5
#define KEY_DOWN		6
#define KEY_PAUSE		7
#define KEY_PROCESS_ONE_FRAME		8

struct IPlayAreaElement:virtual public ISystemUnknown
{
    virtual void Activate(unsigned int dwCurrentTime)=0;
    virtual void Deactivate()=0;
    virtual bool IsActive()=0;
	virtual void Reset()=0;

    virtual bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pPlayAreaInfo,unsigned int dwCurrentTime,double dInterval)=0;

	virtual void DesignRender(IGenericRender *piRender,bool bSelected)=0;
	virtual CTraceInfo DesignGetTrace(const CVector &p1,const CVector &p2)=0;
};

struct IPlayAreaEntity:virtual public IPlayAreaElement
{
	virtual void SetCount(unsigned int nCount)=0;
	virtual void SetDelay(unsigned int nDelay)=0;
	virtual void SetRouteDelay(unsigned int nDelay)=0;
	virtual void SetInterval(unsigned int nInterval)=0;
	virtual void SetDynamic(bool bDynamic)=0;
	
	virtual void SetPosition(const CVector &vPosition)=0;
	virtual void SetAngles(const CVector &vAngles)=0;
	virtual void SetEntityType(IEntityType *piEntityType)=0;
	virtual void SetBonusType(IEntityType *piEntityType)=0;
	
	virtual unsigned int GetCount()=0;
	virtual unsigned int GetDelay()=0;
	virtual unsigned int GetRouteDelay()=0;
	virtual unsigned int GetInterval()=0;
	
	virtual CVector GetPosition()=0;
	virtual CVector GetAngles()=0;
	virtual void	GetEntityType(IEntityType **piEntityType)=0;
	virtual void 	GetBonusType(IEntityType **piEntityType)=0;
	
	virtual unsigned int GetRoutePoints()=0;
	virtual bool 		 GetRoutePoint(unsigned int nIndex,SRoutePoint *psPoint)=0;
	virtual bool 		 AddRoutePoint(unsigned int nIndex,const SRoutePoint &sPoint)=0;
	virtual bool 		 SetRoutePoint(unsigned int nIndex,const SRoutePoint &sPoint)=0;
	virtual void		 RemoveRoutePoint(unsigned int nIndex)=0;
	virtual void 		 ClearRoute()=0;
};
	

enum EPlayerStateCondition
{
	ePlayerStateCondition_None,
	ePlayerStateCondition_MaxPrimaryWeaponLevel,
	ePlayerStateCondition_Count
};

DECLARE_SERIALIZABLE_ENUMERATION(EPlayerStateCondition)

struct IPlayAreaFormation:virtual public IPlayAreaElement
{
	virtual void SetPosition(CVector &vPosition)=0;
	virtual void SetFormationType(IFormationType *piFormationType)=0;
	virtual void SetBonusType(IEntityType *piEntityType)=0;
	virtual void SetAlternativeFormationType(IFormationType *piFormationType)=0;
	virtual void SetAlternativeBonusType(IEntityType *piEntityType)=0;
	virtual void SetCondition(EPlayerStateCondition eConditionType,unsigned int nValue)=0;

	virtual CVector GetPosition()=0;
	virtual void	GetFormationType(IFormationType **piFormationType)=0;
	virtual void	GetBonusType(IEntityType **ppiEntityType)=0;
	virtual void	GetAlternativeFormationType(IFormationType **piFormationType)=0;
	virtual void	GetAlternativeBonusType(IEntityType **ppiEntityType)=0;
	virtual void    GetCondition(EPlayerStateCondition *peConditionType,unsigned int *pnValue)=0;
};


class IPlayAreaElementEnumerationCallback
{
public:
	virtual void ProcessEnumeratedPlayAreaElement(IPlayAreaElement *piElement,bool *pbStopEnumerating)=0;

	virtual ~IPlayAreaElementEnumerationCallback(){}
};

struct IPlayAreaManager:virtual public ISystemUnknown
{
    virtual void GetAirPlayPlane(CVector *pPlayAreaMins,CVector *pPlayAreaMaxs)=0;
    virtual void GetVisibleAirPlayPlane(CVector *pVisiblePlayAreaMins,CVector *pVisiblePlayAreaMaxs)=0;
	virtual void GetPlayAreaPlaneAt(CVector vPos,CVector *pPlayAreaMins,CVector *pPlayAreaMaxs)=0;
	virtual void GetCurrentVisibleArea(CVector *pVisiblePlayAreaMins,CVector *pVisiblePlayAreaMaxs)=0;
	
	virtual void	SetPlayMovementPosition(CVector vPosition)=0;
    virtual CVector GetPlayMovementPosition()=0;
    virtual CVector GetPlayMovementForward()=0;
    virtual CVector GetPlayMovementRight()=0;
    virtual CVector GetPlayMovementUp()=0;
	virtual void	GetPlayerRoute(CVector *pvStart,CVector *pvEnd)=0;

	virtual IGenericCamera *GetCamera()=0;
	virtual double          GetCameraSpeed()=0;
	virtual void EnumeratePlayAreaElements(IPlayAreaElementEnumerationCallback *piCallback)=0;

	virtual void ProcessInput(IGameGUIManager *piManager)=0;
	virtual bool IsScenarioCompleted()=0;

	virtual bool IsVisible(CVector vPos,double dRadius,bool bWithScroll)=0;

	virtual void CreateDynamicEntityElement(IEntityType *piEntityType,CVector vPosition,CVector vAngles,IPlayAreaEntity **ppiElement)=0;
	
};

struct IWeapon
{
  virtual unsigned int GetSlot()=0;

  virtual void  Fire(unsigned int dwCurrentTime)=0;
  virtual bool  IsReady(unsigned int dwCurrentTime)=0;

  virtual unsigned int GetCurrentLevel()=0;
  virtual void  SetCurrentLevel(unsigned int dwLevel)=0;
  
  virtual CVector  GetIdealHeadingToTarget(CVector vTargetPosition,CVector vTargetVelocity)=0;
  
  virtual bool         UsesAmmo()=0;
  virtual void         SetAmmo(unsigned int nAmmo)=0;
  virtual void         AddAmmo(unsigned int nAmmo)=0;
  virtual unsigned int GetAmmo()=0;
  
  virtual ~IWeapon(){}
};

struct IWeaponType:virtual public ISystemUnknown
{
  virtual IWeapon *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime)=0;
};


struct IPlayer
{
  virtual unsigned int GetPoints()=0;
  virtual void  SetPoints(unsigned int dwPoints)=0;
  virtual void  AddPoints(unsigned int dwPoints)=0;

  virtual unsigned int GetLivesLeft()=0;
  virtual void  SetLivesLeft(unsigned int dwLivesLeft)=0;
  virtual void  AddLivesLeft(unsigned int dwLivesLeft)=0;

  virtual double GetSpeed()=0;
  virtual void   SetSpeed(double dSpeed)=0;

  virtual void  GetWeapons(vector<IWeapon*> *pWeapons)=0;
  virtual void  GetWeaponsOnSlot(unsigned int dwWeaponSlot,vector<IWeapon*> *pWeapons)=0;
  virtual void  FireWeaponsOnSlot(unsigned int dwWeaponSlot,unsigned int dwCurrentTime)=0;

  virtual void         SetDifficultyLevel(unsigned int nLevel)=0;
  virtual unsigned int GetDifficultyLevel()=0;
  
  virtual void         SetGodMode(bool bGod)=0;
  
  virtual ~IPlayer(){}
};
struct STerrainWater
{
	bool		 bEnabled;
	double		 dSpeed;
	double		 dOpacity;
	double		 dHorizontalResolution;
	double		 dVerticalResolution;
	std::string  sTextureFile1;
	std::string  sTextureFile2;
	double		 dHeight;

	STerrainWater()
	{
		dOpacity=1;
		dSpeed=0;
		bEnabled=false;
		dHorizontalResolution=1;
		dVerticalResolution=1;
		dHeight=0;
	}
};

struct STerrainSky
{
	bool		 bEnabled;
	double		 dSpeed;
	double		 dOpacity;
	double		 dHorizontalResolution;
	double		 dVerticalResolution;
	std::string  sTextureFile;

	STerrainSky()
	{
		dOpacity=1;
		dSpeed=0;
		bEnabled=false;
		dHorizontalResolution=1;
		dVerticalResolution=1;
	}
};

struct STerrainSun
{
	double		 dDistance;
	double		 dElevation;
	double		 dAzimuth;
	CVector		 vColor;

	STerrainSun()
	{
		dDistance=1;
		dElevation=90;
		dAzimuth=0;
		vColor=CVector(0.2,0.2,0.2);
	}
};

struct STerrainFog
{
	bool		 bEnabled;
	double		 dStart;
	double		 dEnd;
	CVector		 vColor;

	STerrainFog()
	{
		bEnabled=false;
		dStart=100;
		dEnd=1000;
		vColor=CVector(1,1,1);
	}
};

struct STerrainHeightLayer
{
	double dMinHeight;
	double dMaxHeight;
	double dHorizontalResolution;
	double dVerticalResolution;
	double dDecayMargin;

	std::string sTextureFile;

	STerrainHeightLayer()
	{
		dMinHeight=0;
		dMaxHeight=0;
		dHorizontalResolution=1;
		dVerticalResolution=1;
		dDecayMargin=0;
	}
};

struct STerrainColorLayer
{
	CVector vColor;
	double dHorizontalResolution;
	double dVerticalResolution;

	std::string sTextureFile;

	// Componentes Saturacion y Valor en HSV

	double dColorSaturationMargin;
	double dColorValueMargin;
	bool   bAbruptEnd;

	STerrainColorLayer()
	{
		dColorSaturationMargin=1.0;
		dColorValueMargin=0.01;
		dHorizontalResolution=1;
		dVerticalResolution=1;
		bAbruptEnd=false;
	}
};

struct ITerrainDesign:virtual public IDesign
{
public:
	// ITerrainDesign General settings

	virtual bool SetTerrainBaseModel(std::string sModel)=0;
	virtual bool SetTerrainColorMap(std::string sColorMap)=0;

	virtual void GetTerrainBaseModel(std::string *psModel,IGenericModel **ppiModel)=0;
	virtual void GetTerrainColorMap(std::string *psColorMap,IGenericTexture **ppiTexture)=0;

	virtual void GetTerrainModel(IGenericModel **ppiModel)=0;

	// Water

	virtual bool SetTerrainWater(STerrainWater *pWater)=0;
	virtual void GetTerrainWater(STerrainWater *pWater,IGenericTexture **ppiTexture1,IGenericTexture **ppiTexture2)=0;

	// Sky

	virtual bool SetTerrainSky(STerrainSky *pSky)=0;
	virtual void GetTerrainSky(STerrainSky *pSky,IGenericTexture **ppiTexture)=0;

	// Sun

	virtual bool SetTerrainSun(STerrainSun *pSun)=0;
	virtual void GetTerrainSun(STerrainSun *pSun)=0;

	// Ambient Color

	virtual void	SetTerrainAmbientColor(const CVector &vAmbientColor)=0;
	virtual CVector GetTerrainAmbientColor()=0;

	// Fog

	virtual bool SetTerrainFog(STerrainFog *pFog)=0;
	virtual void GetTerrainFog(STerrainFog *pFog)=0;

	// ITerrainDesign Color layers

	virtual unsigned long	AddTerrainColorLayer(STerrainColorLayer *pLayer)=0;
	virtual void			UpdateTerrainColorLayer(unsigned int nIndex,STerrainColorLayer *pLayer)=0;
	virtual unsigned int	MoveTerrainColorLayer(unsigned int nIndex,bool bUp)=0;
	virtual void			RemoveTerrainColorLayer(unsigned int nIndex)=0;
	virtual void			GetTerrainColorLayer(unsigned int nIndex,STerrainColorLayer *pLayer,IGenericTexture **ppiTexture)=0;
	virtual unsigned int	GetTerrainColorLayers()=0;

	// ITerrainDesign Height layers

	virtual unsigned long	AddTerrainHeightLayer(STerrainHeightLayer *pLayer)=0;
	virtual void			UpdateTerrainHeightLayer(unsigned int nIndex,STerrainHeightLayer *pLayer)=0;
	virtual unsigned int	MoveTerrainHeightLayer(unsigned int nIndex,bool bUp)=0;
	virtual void			RemoveTerrainHeightLayer(unsigned int nIndex)=0;
	virtual void			GetTerrainHeightLayer(unsigned int nIndex,STerrainHeightLayer *pLayer,IGenericTexture **ppiTexture)=0;
	virtual unsigned int	GetTerrainHeightLayers()=0;

	virtual bool			UpdateTerrain()=0;

	// Information methods
	virtual bool			GetTerrainStats( double *pdVertexOverhead,double *pdFaceOverhead )=0;
	virtual bool			GetTerrainHeightAt( CVector vPoint ,double *pdHeight)=0;
	virtual bool			GetTerrainTrace( CVector vPoint1 ,CVector vPoint2 ,CVector *pHitPos)=0;
};

struct IWorldManager:virtual public ISystemUnknown
{
public:
	virtual void SetupRenderingEnvironment(IGenericRender *piRender)=0;
};

struct IFrameManager:virtual public ISystemUnknown
{
public:
		virtual void Reset()=0;
		virtual bool IsPaused()=0;
		virtual void TogglePauseOnNextFrame()=0;
		virtual void SetPauseOnNextFrame(bool bPause)=0;

		virtual void ProcessFrame()=0;

		virtual unsigned int  GetCurrentRealTime()=0;
		virtual unsigned int  GetCurrentTime()=0;
		virtual double GetTimeFraction()=0;
		virtual double GetRealTimeFraction()=0;
		virtual double GetCurrentFps()=0;
};


enum EEntityAttributeType
{
	eEntityAttributeType_None=0,
	eEntityAttributeType_Bool,
	eEntityAttributeType_Int,
	eEntityAttributeType_Float,
	eEntityAttributeType_Vector,
	eEntityAttributeType_String
};

struct SEntityTypeConfig
{
	CVector 		vBBoxMins;
	CVector 		vBBoxMaxs;
	unsigned int	nMovementType;
	unsigned int 	nCollisionType;
	unsigned int 	nDamageType;
	unsigned int 	nBoundsType;
	unsigned int 	nAlignment;
	double			dMaxHealth;
	double			dMaxVelocity;
	unsigned int    nPoints;

	SEntityTypeConfig(){nPoints=0;dMaxHealth=1;dMaxVelocity=0;nDamageType=DAMAGE_TYPE_NONE;nMovementType=PHYSIC_MOVE_TYPE_NONE;nCollisionType=PHYSIC_COLLISION_TYPE_STUCK;nBoundsType=PHYSIC_BOUNDS_TYPE_NONE;nAlignment=ENTITY_ALIGNMENT_NEUTRAL;}
};

struct IAttributeManagement:virtual public ISystemUnknown
{
	// Attribute Management.
	
	virtual unsigned int GetAttributeCount()=0;
	virtual bool		 GetAttributeType(unsigned int nIndex,EEntityAttributeType *peType)=0;
	virtual bool		 GetAttributeName(unsigned int nIndex,std::string *psName)=0;
	
	virtual bool		 SetAttributeValue(std::string sName,bool bValue)=0;
	virtual bool		 SetAttributeValue(std::string sName,unsigned int nValue)=0;
	virtual bool		 SetAttributeValue(std::string sName,double dValue)=0;
	virtual bool		 SetAttributeValue(std::string sName,CVector vValue)=0;
	virtual bool		 SetAttributeValue(std::string sName,string sValue)=0;
	
	virtual bool		 GetAttributeValue(std::string sName,bool *pbValue)=0;
	virtual bool		 GetAttributeValue(std::string sName,unsigned int *pnValue)=0;
	virtual bool		 GetAttributeValue(std::string sName,double *pdValue)=0;
	virtual bool		 GetAttributeValue(std::string sName,CVector *pvValue)=0;
	virtual bool		 GetAttributeValue(std::string sName,string *psValue)=0;
};

#define ENTITY_STATE_BASE 	 0
#define ENTITY_STATE_INVALID (unsigned int)(-1)
#define ANIMATION_RANDOM     (unsigned int)(-2)
#define ANIMATION_INVALID	 (unsigned int)(-1)
#define WEAPON_INVALID	 	 (unsigned int)(-1)

struct IEntityTypeDesign:virtual public ISystemUnknown
{
	virtual void		 GetEntityTypeConfig(SEntityTypeConfig *pConfig)=0;
	virtual void		 SetEntityTypeConfig(SEntityTypeConfig *pConfig)=0;

	// State Management

	virtual unsigned int GetStateCount()=0;
	virtual bool		 GetStateName(unsigned int nIndex,std::string *psName)=0;

	// Animation management

	virtual unsigned int	AddStateAnimation(unsigned int nState)=0;
	virtual bool			RemoveStateAnimation(unsigned int nState,unsigned int nAnimation)=0;
	virtual bool			GetStateAnimation(unsigned int nState,unsigned int nAnimation,IAnimationType **ppiAnimation)=0;
	virtual unsigned int	GetStateAnimationCount(unsigned int nState)=0;

	// Weapon management

	virtual unsigned int	AddWeapon(std::string sWeaponType)=0;
	virtual bool			RemoveWeapon(unsigned int nWeapon)=0;
	virtual bool			GetWeapon(unsigned int nWeapon,IWeaponType **ppiWeapon)=0;
	virtual unsigned int	GetWeaponCount(unsigned int nState)=0;
	
	// Children management
	
	virtual unsigned int	AddChild(std::string sEntity)=0;
	virtual bool			RemoveChild(unsigned int nChild)=0;
	virtual bool			GetChild(unsigned int nChild,IEntityType **ppiEntityType)=0;
	virtual unsigned int	GetChildren()=0;
	virtual void			SetChildLocation(unsigned int nChild,CVector vPosition,CVector vAngles)=0;
	virtual void			GetChildLocation(unsigned int nChild,CVector &vPosition,CVector &vAngles)=0;
};

#endif
