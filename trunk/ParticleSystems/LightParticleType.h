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

class CLightParticleType: virtual public CSystemObjectBase,virtual public IParticleType
{
public:
	CGenericRenderWrapper m_Render;


    IParticle *CreateInstance(IParticleEmitter *piEmiter,unsigned int dwCurrentTime);

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

    CLightParticleType();
    ~CLightParticleType();
};

class CLightParticle: public IParticle
{
    CLightParticleType  *m_pType;
    CGenericLightWrapper m_Light;

public:

    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CLightParticle(CLightParticleType *pType,IParticleEmitter *pEmiter);
    ~CLightParticle();
};
