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


#include "./stdafx.h"
#include "ParticleSystems.h"
#include "ParticleModifierType.h"
#include "ParticleColorModifierType.h"


CParticleColorModifierType::CParticleColorModifierType(void)
{
    m_vStartColor=CRGBColor(1,1,1);
    m_vEndColor=CRGBColor(1,1,1);
    m_dStartAlpha=1;
    m_dEndAlpha=1;
    m_dAlphaTransitionStart=0;
    m_dAlphaTransitionEnd=1;
    m_dColorTransitionStart=0;
    m_dColorTransitionEnd=1;
}

CParticleColorModifierType::~CParticleColorModifierType(void){}
IParticleModifier *CParticleColorModifierType::CreateInstance(unsigned int dwCurrentTime){return new CParticleColorModifier(this);}

CParticleColorModifier::CParticleColorModifier(CParticleColorModifierType *pType)
{
    m_pType=pType;
}

void CParticleColorModifier::ProcessParticle(IParticle *pParticle,IParticleSystem *pSystem,unsigned int dwCurrentTime,double dInterval)
{
    if(m_sEmitters.size()!=0 && m_sEmitters.find(pParticle->m_piEmiter)==m_sEmitters.end()){return;}
    pParticle->m_vColor=GetTransitionValue(m_pType->m_dColorTransitionStart,m_pType->m_vStartColor,m_pType->m_dColorTransitionEnd,m_pType->m_vEndColor,&m_pType->m_dIntermediateColorTransitions,pParticle->m_dLifeSpent);
    pParticle->m_dAlpha=GetTransitionValue(m_pType->m_dAlphaTransitionStart,m_pType->m_dStartAlpha,m_pType->m_dAlphaTransitionEnd,m_pType->m_dEndAlpha,&m_pType->m_dIntermediateAlphaTransitions,pParticle->m_dLifeSpent);
}

        /*double dTransitionStart=m_pType->m_dColorTransitionStart;
        CVector vStartColor=m_pType->m_vStartColor;
        double dTransitionEnd=m_pType->m_dColorTransitionEnd;
        CVector vEndColor=m_pType->m_vEndColor;
        if(m_pType->m_dIntermediateColorTransitions.size())
        {
            dTransitionEnd=m_pType->m_dIntermediateColorTransitions[0].dTime;
            vEndColor=m_pType->m_dIntermediateColorTransitions[0].vColor;

            for(unsigned x=0;x<m_pType->m_dIntermediateColorTransitions.size();x++)
            {
                if(pParticle->m_dLifeSpent>=m_pType->m_dIntermediateColorTransitions[x].dTime)
                {
                    dTransitionStart=m_pType->m_dIntermediateColorTransitions[x].dTime;
                    vStartColor=m_pType->m_dIntermediateColorTransitions[x].vColor;

                    if(x<(m_pType->m_dIntermediateColorTransitions.size()-1))
                    {
                        dTransitionEnd=m_pType->m_dIntermediateColorTransitions[x+1].dTime;
                        vEndColor=m_pType->m_dIntermediateColorTransitions[x+1].vColor;
                    }
                    else
                    {
                        dTransitionEnd=m_pType->m_dColorTransitionEnd;
                        vEndColor=m_pType->m_vEndColor;
                    }
                }
            }
        }
        double dTransitionSize=dTransitionEnd-dTransitionStart;
        double dFactor=(pParticle->m_dLifeSpent-dTransitionStart)/dTransitionSize;
        pParticle->m_vColor=vStartColor+(vEndColor-vStartColor)*dFactor;

        if(pParticle->m_dLifeSpent<=m_pType->m_dAlphaTransitionStart)
        {
        pParticle->m_dAlpha=m_pType->m_dStartAlpha;
        }
        else if(pParticle->m_dLifeSpent>=m_pType->m_dAlphaTransitionEnd)
        {
        pParticle->m_dAlpha=m_pType->m_dEndAlpha;
        }
        else
        {
        double dTransitionSize=m_pType->m_dAlphaTransitionEnd-m_pType->m_dAlphaTransitionStart;
        double dFactor=(pParticle->m_dLifeSpent-m_pType->m_dAlphaTransitionStart)/dTransitionSize;
        pParticle->m_dAlpha=m_pType->m_dStartAlpha+(m_pType->m_dEndAlpha-m_pType->m_dStartAlpha)*dFactor;
        }        */
