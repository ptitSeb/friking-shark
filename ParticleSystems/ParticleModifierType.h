#pragma once

class CParticleModifierType: virtual public CSystemObjectBase,virtual public IParticleModifierType
{
public:
    // Propiedades Persistentes

    string          m_sName;
    deque<string>   m_dEmitterNames;

    BEGIN_PROP_MAP(CParticleModifierType);
        PROP_FLAGS(m_sName,"Nombre",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_FLAGS(m_dEmitterNames,"Emisores",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    virtual unsigned GetEmitterNameCount();
    virtual string   GetEmitterName(unsigned index);

    CParticleModifierType();
    ~CParticleModifierType();
};

class CParticleModifier:public IParticleModifier
{
protected:
    set<IParticleEmitter *> m_sEmitters;
public:

    void AddEmitter(IParticleEmitter *pEmitter);
    void ProcessParticle(IParticle *pParticle,IParticleSystem *pSystem,DWORD dwCurrentTime,double dInterval)=0;

    CParticleModifier();
    virtual ~CParticleModifier(){}
};


template<typename VALUE_TYPE,typename INTERMEDITATE_DEQUE_VALUE_TYPE> VALUE_TYPE GetTransitionValue(double dTransitionStart,VALUE_TYPE startValue,double dTransitionEnd,VALUE_TYPE endValue,deque<INTERMEDITATE_DEQUE_VALUE_TYPE> *pIntermediateValues,double dCurrentTime)
{
    double      dAbsTransitionStart=dTransitionStart;
    double      dAbsTransitionEnd=dTransitionEnd;
    VALUE_TYPE  absStartValue=startValue;
    VALUE_TYPE  absEndValue=endValue;

    if(dCurrentTime<=dTransitionStart)
    {
        return startValue;
    }
    else if(dCurrentTime>=dTransitionEnd)
    {
        return endValue;
    }
    else
    {
        if(pIntermediateValues->size())
        {
            dAbsTransitionEnd=(*pIntermediateValues)[0].dTime;
            absEndValue=(*pIntermediateValues)[0].value;

            for(unsigned x=0;x<pIntermediateValues->size();x++)
            {
                if(dCurrentTime>=(*pIntermediateValues)[x].dTime)
                {
                    dAbsTransitionStart=(*pIntermediateValues)[x].dTime;
                    absStartValue=(*pIntermediateValues)[x].value;

                    if(x<(pIntermediateValues->size()-1))
                    {
                        dAbsTransitionEnd=(*pIntermediateValues)[x+1].dTime;
                        absEndValue=(*pIntermediateValues)[x+1].value;
                    }
                    else
                    {
                        dAbsTransitionEnd=dTransitionEnd;
                        absEndValue=endValue;
                    }
                }
            }
        }
        double dTransitionSize=dAbsTransitionEnd-dAbsTransitionStart;
        double dFactor=(dCurrentTime-dAbsTransitionStart)/dTransitionSize;
        return absStartValue+(absEndValue-absStartValue)*dFactor;
    }
}

