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

class CParticleEmitterModifierType: virtual public CSystemObjectBase,virtual public IParticleEmitterModifierType
{
public:
    // Propiedades Persistentes

    string          m_sName;
    deque<string>   m_dEmitterNames;

    BEGIN_PROP_MAP(CParticleEmitterModifierType);
        PROP_FLAGS(m_sName,"Nombre",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_FLAGS(m_dEmitterNames,"Emisores",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    virtual unsigned GetEmitterNameCount();
    virtual string   GetEmitterName(unsigned index);

    CParticleEmitterModifierType();
    ~CParticleEmitterModifierType();
};

