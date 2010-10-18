#ifndef _VECTOR_LIB_UTILITIES_
#define _VECTOR_LIB_UTILITIES_

#include <vector>

template<typename T>
T* BufferFromVector(std::vector< T > *pContainer)
{
	T* pBuffer=new T[pContainer->size()];
	T* pBufferCursor=pBuffer;
	vector<T>::iterator i;
	for(i=pContainer->begin();i!=pContainer->end();i++,pBufferCursor++)
	{
		*pBufferCursor=*i;
	}
	return pBuffer;
}

void _MRT(const char *format, ...);
void SkipCommentsStringsAndSpaces(char *pBuffer,DWORD *pOffset,DWORD bufLen);
bool GetFileModificationTime(char *pfile,FILETIME *pModificationTime);

#endif