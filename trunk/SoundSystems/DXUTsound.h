//-----------------------------------------------------------------------------
// File: DXUTsound.h
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef DXUTSOUND_H
#define DXUTSOUND_H

#include <dsound.h>
//-----------------------------------------------------------------------------
// Classes used by this header
//-----------------------------------------------------------------------------
class CSoundManager;
class CSound;
class CStreamingSound;
class CWaveFile;


//-----------------------------------------------------------------------------
// Typing macros 
//-----------------------------------------------------------------------------
#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

#define DXUT_StopSound(s)         { if(s) s->Stop(); }
#define DXUT_PlaySound(s)         { if(s) s->Play( 0, 0 ); }
#define DXUT_PlaySoundLooping(s)  { if(s) s->Play( 0, DSBPLAY_LOOPING ); }


//-----------------------------------------------------------------------------
// Name: class CSoundManager
// Desc: 
//-----------------------------------------------------------------------------
class CSoundManager
{
protected:
    IDirectSound8* m_pDS;

public:
    CSoundManager();
    ~CSoundManager();

    HRESULT Initialize( HWND hWnd, unsigned int dwCoopLevel );
    inline  LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
    HRESULT SetPrimaryBufferFormat( unsigned int dwPrimaryChannels, unsigned int dwPrimaryFreq, unsigned int dwPrimaryBitRate );
    HRESULT Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );

    HRESULT Create( CSound** ppSound, LPSTR strWaveFileName, unsigned int dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, unsigned int dwNumBuffers = 1 );
    HRESULT CreateFromMemory( CSound** ppSound, unsigned char* pbData, ULONG ulDataSize, LPWAVEFORMATEX pwfx, unsigned int dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, unsigned int dwNumBuffers = 1 );
    HRESULT CreateStreaming( CStreamingSound** ppStreamingSound, LPSTR strWaveFileName, unsigned int dwCreationFlags, GUID guid3DAlgorithm, unsigned int dwNotifyCount, unsigned int dwNotifySize, HANDLE hNotifyEvent );
};


//-----------------------------------------------------------------------------
// Name: class CSound
// Desc: Encapsulates functionality of a DirectSound buffer.
//-----------------------------------------------------------------------------
class CSound
{
protected:
    LPDIRECTSOUNDBUFFER* m_apDSBuffer;
    unsigned int                m_dwDSBufferSize;
    CWaveFile*           m_pWaveFile;
    unsigned int                m_dwNumBuffers;
    unsigned int                m_dwCreationFlags;

    HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

public:
    CSound( LPDIRECTSOUNDBUFFER* apDSBuffer, unsigned int dwDSBufferSize, unsigned int dwNumBuffers, CWaveFile* pWaveFile, unsigned int dwCreationFlags );
    virtual ~CSound();

    HRESULT Get3DBufferInterface( unsigned int dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );
    HRESULT FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger );
    LPDIRECTSOUNDBUFFER GetFreeBuffer();
    LPDIRECTSOUNDBUFFER GetBuffer( unsigned int dwIndex );

    HRESULT Play( unsigned int dwPriority = 0, unsigned int dwFlags = 0, LONG lVolume = 0, LONG lFrequency = -1, LONG lPan = 0 );
    HRESULT Play3D( LPDS3DBUFFER p3DBuffer, unsigned int dwPriority = 0, unsigned int dwFlags = 0, LONG lFrequency = 0 );
    HRESULT Stop();
    HRESULT Reset();
    BOOL    IsSoundPlaying();
};


//-----------------------------------------------------------------------------
// Name: class CStreamingSound
// Desc: Encapsulates functionality to play a wave file with DirectSound.  
//       The Create() method loads a chunk of wave file into the buffer, 
//       and as sound plays more is written to the buffer by calling 
//       HandleWaveStreamNotification() whenever hNotifyEvent is signaled.
//-----------------------------------------------------------------------------
class CStreamingSound : public CSound
{
protected:
    unsigned int m_dwLastPlayPos;
    unsigned int m_dwPlayProgress;
    unsigned int m_dwNotifySize;
    unsigned int m_dwNextWriteOffset;
    BOOL  m_bFillNextNotificationWithSilence;

public:
    CStreamingSound( LPDIRECTSOUNDBUFFER pDSBuffer, unsigned int dwDSBufferSize, CWaveFile* pWaveFile, unsigned int dwNotifySize );
    ~CStreamingSound();

    HRESULT HandleWaveStreamNotification( BOOL bLoopedPlay );
    HRESULT Reset();
};


//-----------------------------------------------------------------------------
// Name: class CWaveFile
// Desc: Encapsulates reading or writing sound data to or from a wave file
//-----------------------------------------------------------------------------
class CWaveFile
{
public:
    WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    HMMIO         m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO      m_ck;          // Multimedia RIFF chunk
    MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
    unsigned int         m_dwSize;      // The size of the wave file
    MMIOINFO      m_mmioinfoOut;
    unsigned int         m_dwFlags;
    BOOL          m_bIsReadingFromMemory;
    unsigned char*         m_pbData;
    unsigned char*         m_pbDataCur;
    ULONG         m_ulDataSize;
    CHAR*         m_pResourceBuffer;

protected:
    HRESULT ReadMMIO();
    HRESULT WriteMMIO( WAVEFORMATEX *pwfxDest );

public:
    CWaveFile();
    ~CWaveFile();

    HRESULT Open( LPSTR strFileName, WAVEFORMATEX* pwfx, unsigned int dwFlags );
    HRESULT OpenFromMemory( unsigned char* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, unsigned int dwFlags );
    HRESULT Close();

    HRESULT Read( unsigned char* pBuffer, unsigned int dwSizeToRead, unsigned int* pdwSizeRead );
    HRESULT Write( UINT nSizeToWrite, unsigned char* pbData, UINT* pnSizeWrote );

    unsigned int   GetSize();
    HRESULT ResetFile();
    WAVEFORMATEX* GetFormat() { return m_pwfx; };
};


#endif // DXUTSOUND_H
