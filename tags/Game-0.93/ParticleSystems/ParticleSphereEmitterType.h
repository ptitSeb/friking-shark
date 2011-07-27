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

class CParticleSphereEmitterType: virtual public CSystemObjectBase,virtual public IParticleEmitterType
{
public:

    // Propiedades Persistentes.

	bool		m_bSpheric;
    string      m_sName;
    unsigned int       m_dwEmitStartTime;  // si es cero se empieza inmediatamente a emitir, tiempo relativo
    unsigned int       m_dwEmitEndTime;    // si es cero, emite siempre si no es el momento en el que deja de emitir, tiempo relativo
    double      m_dStartRate;            // numero de particulas por segundo en el inicio
    double      m_dEndRate;            // numero de particulas por segundo en el final

    CVector     m_vPosition;
	
    CVector     m_vMinPosition;
    CVector     m_vMaxPosition;

    CVector     m_vMinDirection;
    CVector     m_vMaxDirection;

    double      m_dMinVelocity;
    double      m_dMaxVelocity;

    double      m_dMinAngularVelocity;
    double      m_dMaxAngularVelocity;
    double      m_dMinAngle;
    double      m_dMaxAngle;

    unsigned int       m_dwParticleDuration;    // cuanto duran las particulas emitidas
    unsigned int       m_dwParticleCount;      // numero de particulas (si m_dwEmitEndTime es cero o igual a m_dwEmitStartTime se emiten instantaneamente, en caso contrario se distribuyen en el tiempo)
    unsigned int       m_dwMovementType;
    bool        m_bFixedPositionOnParent; // Mantiene la posicion inicial en el sistema de referencia del padre, pensado para fuentes de luz y efectos estaticos respecto a la entidad

    CParticleTypeWrapper m_ParticleType;
	
	double m_dSphereMinPitch;
	double m_dSphereMaxPitch;
	double m_dSphereMinRadius;
	double m_dSphereMaxRadius;
	double m_dSphereDensity;
	double m_dSphereMinExitVelocity;
	double m_dSphereMaxExitVelocity;
	
    // Propiedades RunTime

    IParticleEmitter *CreateInstance(unsigned int dwCurrentTime);

    BEGIN_PROP_MAP(CParticleSphereEmitterType);
		PROP_VALUE_FLAGS(m_dSphereMinPitch,"SphereMinPitch",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dSphereMaxPitch,"SphereMaxPitch",90,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dSphereMinRadius,"SphereMinRadius",20,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dSphereMaxRadius,"SphereMaxRadius",20,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dSphereDensity,"SphereDensity",20,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dSphereMinExitVelocity,"SphereMinExitVelocity",10,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dSphereMaxExitVelocity,"SphereMaxExitVelocity",10,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_ParticleType,"TipoParticulas",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_sName,"Nombre","",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwEmitStartTime,"TiempoInicio",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwEmitEndTime,"TiempoFin",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dStartRate,"RitmoInicial",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dEndRate,"RitmoFinal",m_dStartRate,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwParticleCount,"NumeroParticulas",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_vPosition,"Position",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_vMinPosition,"PositionInicialMinima",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_vMaxPosition,"PositionInicialMaxima",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_vMinDirection,"DireccionMinima",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_vMaxDirection,"DireccionMaxima",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dMinVelocity,"VelocidadMinima",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dMaxVelocity,"VelocidadMaxima",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dMinAngularVelocity,"VelocidadAngularMinima",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dMaxAngularVelocity,"VelocidadAngularMaxima",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dMinAngle,"AnguloInicialMinimo",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dMaxAngle,"AnguloInicialMaximo",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwParticleDuration,"DuracionParticulas",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwMovementType,"TipoMovimiento",PHYSIC_MOVE_TYPE_NORMAL,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_bFixedPositionOnParent,"PosicionFijaEnPadre",false,MRPF_NORMAL|MRPF_OPTIONAL) 
    END_PROP_MAP();
    CParticleSphereEmitterType();
    ~CParticleSphereEmitterType();
};


class CParticleSphereEmitter:public IParticleEmitter
{
    bool        m_bActive;  // a 1 hasta que llega EmitEndTime
    unsigned int       m_dwLastEmitTime;
    unsigned int       m_dwEmitStartTime;  // igual que el del tipo pero con tiempo absoluto en lugar de relativo
    unsigned int       m_dwEmitEndTime;    // igual que el del tipo pero con tiempo absoluto en lugar de relativo
    CParticleSphereEmitterType *m_pType;
	unsigned int       m_dwEmittedCycles;
	CVector            m_vPosition;
	CVector            m_vLastWorldPosition;
	
public:
	CVector GetPosition();
	void    SetPosition(CVector vPosition);
	
    string GetName();

    bool IsActive();
    void Deactivate();

    void ProcessFrame(IParticleSystem *pSystem,unsigned int dwCurrentTime,double dInterval);

    CParticleSphereEmitter(CParticleSphereEmitterType *pType,unsigned int dwCurrentTime);
};
