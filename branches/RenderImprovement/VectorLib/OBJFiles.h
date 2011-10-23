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
//////////////////////////////////////////////////////////////////////

#ifndef OBJ_FILES
#define OBJ_FILES

class CGCMFileType;
struct SOBJMaterial;

struct SOBJFace
{
	int pVertexes[3];
	int pTexCoords[3];
	int pNormals[3];
	bool bSmooth;
	SOBJMaterial *pMaterial;
	
	SOBJFace()
	{
		pVertexes[0]=pVertexes[1]=pVertexes[2]=-1;
		pTexCoords[0]=pTexCoords[1]=pTexCoords[2]=-1;
		pNormals[0]=pNormals[1]=pNormals[2]=-1;
		pMaterial=NULL;
		bSmooth=false;
	}
};

struct SOBJMaterial
{
	CVector cAmbient;
	CVector cDiffuse;
	CVector cSpecular;
	float   fOpacity;
	float   fShininess;

	std::string sTexture;
	std::string sNormalFile;
	
	std::vector<SOBJFace>  vFaces;

	SOBJMaterial():cAmbient(1,1,1),cDiffuse(1,1,1),cSpecular(1,1,1){fShininess=1;fOpacity=1;}
};


class COBJFileType 
{
	std::vector<double>    m_vVertexes;
	std::vector<double>    m_vTexCoords;
	std::vector<double>    m_vNormals;
	std::map<std::string,SOBJMaterial*> m_mMaterials;
	
	SOBJMaterial 		  *m_pCurrentMaterial;
	
	
	void Clear();
	
	bool ReadMTL(const char *psObjFile, const char *psMTLFile);
	
public:

	bool Open(const char *psFileName);
	
	void ToGCM(CGCMFileType *pFile);

	COBJFileType();
	~COBJFileType();
};

#endif 
