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

// GCMFiles.h: Game Compiled Model file loader/saver.
// .gcm files are files designed to store models to be loaded fast.
//
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

#ifndef GCM_FILES
#define GCM_FILES

#define GCM_FILE_VERSION_V1		0x100 // 1.00
#define GCM_FILE_VERSION		GCM_FILE_VERSION_V1
#define GCM_FILE_MAGIC			"<GCM>"
#define GCM_FILE_MAGIC_LENGTH	5

#define GCM_BUFFER_FLAG_HAS_COLORS  	0x0001
#define GCM_BUFFER_FLAG_HAS_NORMALS 	0x0002
#define GCM_BUFFER_FLAG_HAS_NORMAL_MAP  0x0004

struct SGCMHeader
{
	char         sMagic[GCM_FILE_MAGIC_LENGTH];
	unsigned int nVersion;
	unsigned int nFrames;

	SGCMHeader();
};

struct SGCMTextureLevel
{
	std::string sTexture;
	float		*pTexVertexArray;

	SGCMTextureLevel();
	~SGCMTextureLevel();
};

struct SGCMBufferData
{
	double		vAmbientColor[3];
	double		vDiffuseColor[3];
	double		vSpecularColor[3];
	float		fShininess;
	float		fOpacity;
	
	unsigned int nVertexes;
	unsigned int nFaces;

	SGCMBufferData();
};

struct SGCMBuffer
{
	SGCMBufferData data;

	float			*pVertexArray;
	float			*pNormalArray;
	float			*pColorArray;
	float			*pNormalMapArray;
	unsigned int	*pFaceVertexIndexes;
	
	std::string 	 sNormalMap;
	std::vector<SGCMTextureLevel *> vTextureLevels;
	
	SGCMBuffer();
	~SGCMBuffer();
};

struct SGCMFrameData
{
	double vMaxs[3];
	double vMins[3];
	double vSize[3];
	double dRadius;

	SGCMFrameData();
};

struct SGCMFrame
{
	SGCMFrameData data;
	 
	std::vector<SGCMBuffer *> vBuffers;
	
	SGCMFrame();
	~SGCMFrame();
};


class CGCMFileType 
{
	std::vector<SGCMFrame *>  m_vFrames;
	
	void UpdateSizes();

	SGCMBuffer *GetBuffer( unsigned long nFrame, unsigned long nBuffer );

public:

	bool Open(const char *psFileName);
	bool Save(const char *psFileName);

	void Clear();
	
	unsigned long GetFrames();
	unsigned long GetFrameBuffers(unsigned long nFrame);
	CVector		  GetFrameSize(unsigned long nFrame);
	double		  GetFrameRadius(unsigned long nFrame);
	void		  GetFrameBBox(unsigned long nFrame,CVector *pvMins,CVector *pvMaxs);
	
	unsigned long AddFrame();
	unsigned long AddBuffer(unsigned long nFrame);
	
	void		  RemoveBuffers(unsigned long nFrame);
	void		  RemoveFrames();
	
	void		  SetBufferMaterial(unsigned long nFrame,unsigned long nBuffer,CVector vAmbientColor,CVector vDiffuseColor,CVector vSpecularColor, float fShininess, float fOpacity);
	void		  SetBufferTexture(unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,const std::string &sTextureFile);
	void		  SetBufferVertexes(unsigned long nFrame,unsigned long nBuffer,unsigned long nVertexes,float *pVertexes);
	void		  SetBufferFaces(unsigned long nFrame,unsigned long nBuffer,unsigned long nFaces,unsigned int *pFacesVertexes);
	void		  SetBufferNormals(unsigned long nFrame,unsigned long nBuffer,float *pNormals);
	void		  SetBufferColors(unsigned long nFrame,unsigned long nBuffer,float *pColors);
	void		  SetBufferTextureCoords(unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float *pTexVertexes);
	void		  SetBufferNormalMap(unsigned long nFrame,unsigned long nBuffer,const std::string &sNormalMapFile);
	void		  SetBufferNormalMapCoords(unsigned long nFrame,unsigned long nBuffer,float *pNormalMapVertexes);
	
	void		  GetBufferMaterial(unsigned long nFrame,unsigned long nBuffer,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor, float *pfShininess, float *pfOpacity);
	void		  GetBufferTexture(unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,std::string *psTextureFile);
	void		  GetBufferVertexes(unsigned long nFrame,unsigned long nBuffer,unsigned long *pVertexes,float **ppVertexes);
	void		  GetBufferFaces(unsigned long nFrame,unsigned long nBuffer,unsigned long *pFaces,unsigned int **ppFacesVertexes);
	void		  GetBufferNormals(unsigned long nFrame,unsigned long nBuffer,float **ppNormals);
	void		  GetBufferColors(unsigned long nFrame,unsigned long nBuffer,float **ppColors);
	void		  GetBufferTextureCoords(unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float **ppTexVertexes);
	void		  GetBufferTextureLevels(unsigned long nFrame,unsigned long nBuffer,unsigned long *pnTextureLevels);
	void		  GetBufferNormalMap(unsigned long nFrame,unsigned long nBuffer,std::string *psNormalMapFile);
	void		  GetBufferNormalMapCoords(unsigned long nFrame,unsigned long nBuffer,float **ppNormalMapVertexes);
	
	void		  GetTextures(std::set<std::string> *pvTextures);

	CGCMFileType();
	~CGCMFileType();
};

#endif 
