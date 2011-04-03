#pragma once

DECLARE_CUSTOM_WRAPPER1(CPlayAreaElementWrapper,IPlayAreaElement,m_piElement)
DECLARE_CUSTOM_WRAPPER1(CPlayAreaEntityWrapper,IPlayAreaEntity,m_piElement)

struct SEntityLayerData
{
	CEntityTypeWrapper	m_EntityType;
	SEntityLayer		m_LayerConfig;
};

BEGIN_STRUCT_PROPS(SEntityLayer)
	PROP(dSeparation,"Separation")
	PROP(dPositionJitter,"PositionJitter")
	PROP(dYawJitter,"YawJitter")
	PROP(vColor,"Color")
	PROP(dColorSaturationMargin,"ColorSaturationMargin")
	PROP(dColorValueMargin,"ColorValueMargin")
	PROP(sEntityType,"EntityType")
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SEntityLayerData)
	PROP(m_EntityType,"EntityType")
	PROP(m_LayerConfig,"Config")
END_STRUCT_PROPS()

class CPlayAreaManager: virtual public CSystemObjectBase,
                        virtual public CSystemSerializableBase,
                        virtual public IPlayAreaManager,
                        virtual public IGameManager,
						virtual public IPlayAreaDesign,
						virtual public IEntityEvents
{
	bool m_bStarted;

    vector<CPlayAreaElementWrapper>	m_vElements;
	vector<CPlayAreaEntityWrapper>	m_vEntityLayerElements;

    double m_dCameraDistanceFromPlayer;
    double m_dCameraPitch;

	double m_dPlayAreaHeight;
    C3DSVector m_vPlayMovementPos;
    CVector m_vPlayMovementRight;
    CVector m_vPlayMovementUp;
    CVector m_vPlayMovementForward;
    double  m_dPlayMovementSpeed;
	double  m_dPlayMovementMaxHorzScroll;

    C3DSVector m_vPlayerRouteStart;
    C3DSVector m_vPlayerRouteEnd;

    SPlayAreaInfo   m_PlayArea;
    IEntity         *m_piPlayerEntity;
	IPlayer         *m_piPlayer;

    CGameControllerWrapper  m_GameControllerWrapper;
	CGenericCameraWrapper	m_CameraWrapper;
	CFrameManagerWrapper	m_FrameManagerWrapper;

    CVector m_vAirPlayAreaMins;
    CVector m_vAirPlayAreaMaxs;
    CVector m_vVisibleAirPlayAreaMins;
    CVector m_vVisibleAirPlayAreaMaxs;

	bool m_bScenarioCompleted;

	double  m_dCameraFollowFactor;
	double	m_dPlayMovementMaxRoll;
	double  m_dPlayMovementMaxForward;
	double  m_dPlayMovementMaxRight;
	double  m_dPlayMovementMinForward;
	double  m_dPlayMovementMinRight;
	double  m_dPlayMovementCurrentForward;
	double  m_dPlayMovementCurrentRight;
	double	m_dPlayMovementCurrentRoll;
	double	m_dPlayMovementRollVelocity;
	unsigned int	m_dwPlayMovementLastRollTime;

	CVector m_PlayerKilledVelocity;
	bool m_bProcessingPlayerOutroPhase1;
	bool m_bProcessingPlayerOutroPhase2;
	bool m_bProcessingPlayerIntroPhase1;
	
	double m_dNormalPlayerSpeed;


	std::vector<SEntityLayerData>  m_vEntityLayers;

    void CalculateAirPlayArea();

public:

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

	// Element management

	unsigned long	AddElement(std::string sType);
	void			RemoveElement(unsigned int nIndex);
	void			GetElement(unsigned int nIndex,IPlayAreaElement **ppiElement);
	unsigned int	GetElements();

	// IPlayAreaDesign Color based Entity layers

	unsigned long	AddEntityLayer(SEntityLayer *pLayer);
	void			UpdateEntityLayer(unsigned int nIndex,SEntityLayer *pLayer);
	void			RemoveEntityLayer(unsigned int nIndex);
	void			GetEntityLayer(unsigned int nIndex,SEntityLayer *pLayer,IEntityType **ppiEntityType);
	unsigned int	GetEntityLayers();

	bool			UpdateEntityLayers();

	// IDesign

	void DesignRender(IGenericRender *piRender);

    // IPlayAreaManager

	IGenericCamera *GetCamera();
	double          GetCameraSpeed();
	
	void	GetPlayAreaPlaneAt(CVector vPos,CVector *pPlayAreaMins,CVector *pPlayAreaMaxs);
    void    GetAirPlayPlane(CVector *pPlayAreaMins,CVector *pPlayAreaMaxs);
    void    GetVisibleAirPlayPlane(CVector *pVisiblePlayAreaMins,CVector *pVisiblePlayAreaMaxs);

    CVector GetPlayMovementPosition();
    CVector GetPlayMovementForward();
    CVector GetPlayMovementRight();
    CVector GetPlayMovementUp();

	void	GetPlayerRoute(CVector *pvStart,CVector *pvEnd);

	void	EnumeratePlayAreaElements(IPlayAreaElementEnumerationCallback *piCallback);
	void	SetPlayMovementPosition(CVector vPosition);
	
	bool	IsScenarioCompleted();

    // IGameManager

	void CreateScenario();
	void LoadScenario(ISystemPersistencyNode *piNode);
	void SaveScenario(ISystemPersistencyNode *piNode);
	void CloseScenario();

    void Start();
    void Stop();
    void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

	void UpdatePlayCameraPosition();
	void MovePlayer(unsigned long nKey);
	void ProcessInput(IGameGUIManager *piManager);

	
	// IEntityEvents
	
	void OnRemoved(IEntity *piEntity);
	void OnKilled(IEntity *piEntity);
	
	// Edition method

	void	GetPlayAreaConfig(SPlayAreaConfig *pConfig);
	void	SetPlayAreaConfig(SPlayAreaConfig *pConfig);
	void	UpdatePlayArea();

	BEGIN_PROP_MAP(CPlayAreaManager)
        BEGIN_PROP_SUBMAP("ScenarioProps")
		  PROP(m_CameraWrapper,			"Camara");
          PROP(m_vElements,             "Elementos");
          PROP(m_vPlayerRouteStart,     "Origen");
          PROP(m_vPlayerRouteEnd,       "Destino");
          PROP_VALUE(m_dCameraDistanceFromPlayer,"DistanciaCamara",115);
          PROP_VALUE(m_dPlayMovementSpeed,"VelocidadCamara",5);
		  PROP_VALUE(m_dCameraPitch,      "InclinacionVista",0);
		  PROP_VALUE(m_dPlayMovementMaxHorzScroll,    "MaximoScrollHorz",0);
		  PROP_VALUE_FLAGS(m_dPlayAreaHeight,    "AlturaJuego",20,MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_FLAGS(m_vEntityLayers,"EntityLayers",MRPF_NORMAL|MRPF_OPTIONAL)
        END_PROP_SUBMAP("ScenarioProps")
   END_PROP_MAP();

    CPlayAreaManager(void);
    ~CPlayAreaManager(void);
};
