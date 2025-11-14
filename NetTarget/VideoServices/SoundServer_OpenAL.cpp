// SoundServer_OpenAL.cpp
// OpenAL-based sound implementation for HoverRace
// Replaces legacy DirectSound with modern cross-platform audio
//
// Copyright (c) 1995-1998 - Richard Langlois and Grokksoft Inc.
// Modernized with OpenAL support (2025)

#include "stdafx.h"
#include "SoundServer.h"
#include "../Util/MR_Types.h"

// OpenAL headers
#include <AL/al.h>
#include <AL/alc.h>

#define MR_MAX_SOUND_COPY 6
#define MR_MAX_BUFFERS 256
#define MR_MAX_SOURCES 128

// Global OpenAL state
static ALCdevice* gOpenALDevice = NULL;
static ALCcontext* gOpenALContext = NULL;
static ALuint gOpenALSources[MR_MAX_SOURCES];
static int gOpenALSourceCount = 0;

// OpenAL Sound Buffer wrapper
class MR_SoundBuffer
{
   private:
      static MR_SoundBuffer* mList;
      MR_SoundBuffer* mNext;

   protected:
      int mNbCopy;
      ALuint mALBuffers[MR_MAX_SOUND_COPY];
      ALuint mALSources[MR_MAX_SOUND_COPY];
      int mNormalFreq;

   public:
      MR_SoundBuffer();
      virtual ~MR_SoundBuffer();

      BOOL Init(const char* pData, int pNbCopy);
      void SetParams(int pCopy, int pDB, double pSpeed, int pPan);
      int GetNbCopy() const;

      static void DeleteAll();
      virtual void ApplyCumCommand();
      static void ApplyCumCommandForAll();
};

class MR_ShortSound : public MR_SoundBuffer
{
   protected:
      int mCurrentCopy;

   public:
      MR_ShortSound();
      ~MR_ShortSound();
      void Play(int pDB, double pSpeed, int pPan);
};

class MR_ContinuousSound : public MR_SoundBuffer
{
   protected:
      BOOL mOn[MR_MAX_SOUND_COPY];
      int mMaxDB[MR_MAX_SOUND_COPY];
      double mMaxSpeed[MR_MAX_SOUND_COPY];

      void ResetCumStat();

   public:
      MR_ContinuousSound();
      ~MR_ContinuousSound();

      void Pause(int pCopy);
      void Restart(int pCopy);
      void ApplyCumCommand();
      void CumPlay(int pCopy, int pDB, double pSpeed);
};

// Implementation

MR_SoundBuffer* MR_SoundBuffer::mList = NULL;

MR_SoundBuffer::MR_SoundBuffer()
{
   mNbCopy = 0;
   mNormalFreq = 44100;
   mNext = mList;
   mList = this;

   for (int i = 0; i < MR_MAX_SOUND_COPY; i++)
   {
      mALBuffers[i] = 0;
      mALSources[i] = 0;
   }
}

MR_SoundBuffer::~MR_SoundBuffer()
{
   try
   {
      for (int i = 0; i < mNbCopy; i++)
      {
         if (mALSources[i] != 0)
         {
            alDeleteSources(1, &mALSources[i]);
         }
         if (mALBuffers[i] != 0)
         {
            alDeleteBuffers(1, &mALBuffers[i]);
         }
      }
   }
   catch (...)
   {
      // Ignore cleanup errors
   }
}

BOOL MR_SoundBuffer::Init(const char* pData, int pNbCopy)
{
   try
   {
      FILE* debugLog = fopen("C:\\originalhr\\HoverRace\\Release\\Game2_SoundInit_Debug.log", "a");
      if(debugLog) fprintf(debugLog, "MR_SoundBuffer::Init called - pData=%p, pNbCopy=%d\n", pData, pNbCopy), fflush(debugLog);
      
      if (gOpenALContext == NULL) {
         if(debugLog) fprintf(debugLog, "ERROR: gOpenALContext is NULL\n"), fflush(debugLog), fclose(debugLog);
         return FALSE;
      }

      if (pNbCopy > MR_MAX_SOUND_COPY)
         pNbCopy = MR_MAX_SOUND_COPY;

      mNbCopy = pNbCopy;

      // Parse WAV data
      MR_UInt32 lBufferLen = *(MR_UInt32*)pData;
      WAVEFORMATEX* lWaveFormat = (WAVEFORMATEX*)(pData + sizeof(MR_UInt32));
      const char* lSoundData = pData + sizeof(MR_UInt32) + sizeof(WAVEFORMATEX);

      if(debugLog) fprintf(debugLog, "  BufferLen=%d, Channels=%d, BitsPerSample=%d, SamplesPerSec=%d\n", 
                           lBufferLen, lWaveFormat->nChannels, lWaveFormat->wBitsPerSample, lWaveFormat->nSamplesPerSec), fflush(debugLog);

      mNormalFreq = lWaveFormat->nSamplesPerSec;

      ALenum format = AL_NONE;
      if (lWaveFormat->nChannels == 1 && lWaveFormat->wBitsPerSample == 16)
         format = AL_FORMAT_MONO16;
      else if (lWaveFormat->nChannels == 2 && lWaveFormat->wBitsPerSample == 16)
         format = AL_FORMAT_STEREO16;
      else if (lWaveFormat->nChannels == 1 && lWaveFormat->wBitsPerSample == 8)
         format = AL_FORMAT_MONO8;
      else if (lWaveFormat->nChannels == 2 && lWaveFormat->wBitsPerSample == 8)
         format = AL_FORMAT_STEREO8;
      else {
         if(debugLog) fprintf(debugLog, "ERROR: Unsupported format\n"), fflush(debugLog), fclose(debugLog);
         return FALSE;
      }

      // Create first buffer
      alGenBuffers(1, &mALBuffers[0]);
      if (alGetError() != AL_NO_ERROR) {
         if(debugLog) fprintf(debugLog, "ERROR: alGenBuffers failed\n"), fflush(debugLog), fclose(debugLog);
         return FALSE;
      }

      if(debugLog) fprintf(debugLog, "  Created AL buffer: %u\n", mALBuffers[0]), fflush(debugLog);

      alBufferData(mALBuffers[0], format, (void*)lSoundData, lBufferLen, mNormalFreq);
      if (alGetError() != AL_NO_ERROR) {
         if(debugLog) fprintf(debugLog, "ERROR: alBufferData failed\n"), fflush(debugLog), fclose(debugLog);
         return FALSE;
      }

      if(debugLog) fprintf(debugLog, "  Loaded %d bytes into buffer\n", lBufferLen), fflush(debugLog);

      // Create sources for each copy
      for (int i = 0; i < mNbCopy; i++)
      {
         alGenSources(1, &mALSources[i]);
         if (alGetError() != AL_NO_ERROR) {
            if(debugLog) fprintf(debugLog, "ERROR: alGenSources failed for copy %d\n", i), fflush(debugLog), fclose(debugLog);
            return FALSE;
         }

         alSourcei(mALSources[i], AL_BUFFER, mALBuffers[0]);
         alSourcei(mALSources[i], AL_LOOPING, AL_TRUE);  // Enable looping for continuous sounds
         alSourcef(mALSources[i], AL_PITCH, 1.0f);
         alSourcef(mALSources[i], AL_GAIN, 1.0f);
         alSource3f(mALSources[i], AL_POSITION, 0, 0, 0);
         
         if(debugLog) fprintf(debugLog, "  Created AL source %d: %u\n", i, mALSources[i]), fflush(debugLog);
      }

      if(debugLog) fprintf(debugLog, "MR_SoundBuffer::Init SUCCESS - mNbCopy=%d\n", mNbCopy), fflush(debugLog), fclose(debugLog);
      return TRUE;
   }
   catch (...)
   {
      return FALSE;
   }
}

void MR_SoundBuffer::SetParams(int pCopy, int pDB, double pSpeed, int pPan)
{
   try
   {
      if (pCopy >= mNbCopy)
         pCopy = mNbCopy - 1;

      if (pCopy < 0 || pCopy >= mNbCopy || mALSources[pCopy] == 0)
         return;

      // Convert dB to linear gain (0 dB = 1.0)
      float lGain = powf(10.0f, pDB / 20.0f);
      if (lGain < 0.0f) lGain = 0.0f;
      if (lGain > 1.0f) lGain = 1.0f;

      alSourcef(mALSources[pCopy], AL_GAIN, lGain);

      // Set pitch (speed)
      float lPitch = (float)pSpeed;
      if (lPitch < 0.5f) lPitch = 0.5f;
      if (lPitch > 2.0f) lPitch = 2.0f;
      alSourcef(mALSources[pCopy], AL_PITCH, lPitch);

      // Set pan (position on X axis for mono effect)
      float lPanf = pPan / 100.0f;
      if (lPanf < -1.0f) lPanf = -1.0f;
      if (lPanf > 1.0f) lPanf = 1.0f;
      alSource3f(mALSources[pCopy], AL_POSITION, lPanf, 0, 0);
   }
   catch (...)
   {
      // Ignore parameter errors
   }
}

int MR_SoundBuffer::GetNbCopy() const
{
   return mNbCopy;
}

void MR_SoundBuffer::DeleteAll()
{
   while (mList != NULL)
   {
      delete mList;
   }
}

void MR_SoundBuffer::ApplyCumCommand()
{
   // No-op for base class
}

void MR_SoundBuffer::ApplyCumCommandForAll()
{
   MR_SoundBuffer* lBuffer = mList;
   while (lBuffer != NULL)
   {
      lBuffer->ApplyCumCommand();
      lBuffer = lBuffer->mNext;
   }
}

// MR_ShortSound implementation
MR_ShortSound::MR_ShortSound()
{
   mCurrentCopy = 0;
}

MR_ShortSound::~MR_ShortSound()
{
}

void MR_ShortSound::Play(int pDB, double pSpeed, int pPan)
{
   try
   {
      if (mCurrentCopy < 0 || mCurrentCopy >= mNbCopy || mALSources[mCurrentCopy] == 0)
         return;

      // Stop any currently playing sound
      alSourceStop(mALSources[mCurrentCopy]);

      // Rewind to beginning
      alSourceRewind(mALSources[mCurrentCopy]);

      // Set parameters
      SetParams(mCurrentCopy, pDB, pSpeed, pPan);

      // Play the sound
      alSourcePlay(mALSources[mCurrentCopy]);

      // Move to next copy for next play call
      mCurrentCopy++;
      if (mCurrentCopy >= mNbCopy)
         mCurrentCopy = 0;
   }
   catch (...)
   {
      // Silently ignore play errors
   }
}

// MR_ContinuousSound implementation
MR_ContinuousSound::MR_ContinuousSound()
{
   ResetCumStat();
}

MR_ContinuousSound::~MR_ContinuousSound()
{
}

void MR_ContinuousSound::ResetCumStat()
{
   for (int i = 0; i < mNbCopy; i++)
   {
      mOn[i] = FALSE;
      mMaxSpeed[i] = 0;
      mMaxDB[i] = -10000;
   }
}

void MR_ContinuousSound::Pause(int pCopy)
{
   try
   {
      if (pCopy >= mNbCopy)
         pCopy = mNbCopy - 1;

      if (pCopy >= 0 && pCopy < mNbCopy && mALSources[pCopy] != 0)
      {
         alSourcePause(mALSources[pCopy]);
      }
   }
   catch (...)
   {
      // Ignore pause errors
   }
}

void MR_ContinuousSound::Restart(int pCopy)
{
   try
   {
      if (pCopy >= mNbCopy)
         pCopy = mNbCopy - 1;

      if (pCopy >= 0 && pCopy < mNbCopy && mALSources[pCopy] != 0)
      {
         // Check if already playing
         ALint state;
         alGetSourcei(mALSources[pCopy], AL_SOURCE_STATE, &state);

         if (state != AL_PLAYING)
         {
            alSourcePlay(mALSources[pCopy]);
         }
      }
   }
   catch (...)
   {
      // Ignore restart errors
   }
}

void MR_ContinuousSound::ApplyCumCommand()
{
   try
   {
      for (int i = 0; i < mNbCopy; i++)
      {
         if (mOn[i])
         {
            SetParams(i, mMaxDB[i], mMaxSpeed[i], 0);
            Restart(i);
         }
         else
         {
            Pause(i);
         }
      }
   }
   catch (...)
   {
      // Ignore cumulative command errors
   }
   ResetCumStat();
}

void MR_ContinuousSound::CumPlay(int pCopy, int pDB, double pSpeed)
{
   if (pCopy >= mNbCopy)
      pCopy = mNbCopy - 1;

   mOn[pCopy] = TRUE;
   mMaxDB[pCopy] = max(mMaxDB[pCopy], pDB);
   mMaxSpeed[pCopy] = max(mMaxSpeed[pCopy], pSpeed);
}

// MR_SoundServer implementation
namespace MR_SoundServer
{
   BOOL Init(HWND pWindow)
   {
      try
      {
         FILE* f = fopen("C:\\temp\\audio_debug.log", "a");
         if(f) fprintf(f, "MR_SoundServer::Init called\n"), fflush(f), fclose(f);
         
         // Get default audio device
         gOpenALDevice = alcOpenDevice(NULL);
         
         f = fopen("C:\\temp\\audio_debug.log", "a");
         if(f) fprintf(f, "alcOpenDevice returned: %p\n", gOpenALDevice), fflush(f);
         
         if (gOpenALDevice == NULL) {
            if(f) fprintf(f, "ERROR: alcOpenDevice returned NULL\n"), fflush(f), fclose(f);
            return FALSE;
         }

         // Create audio context
         gOpenALContext = alcCreateContext(gOpenALDevice, NULL);
         if(f) fprintf(f, "alcCreateContext returned: %p\n", gOpenALContext), fflush(f);
         
         if (gOpenALContext == NULL)
         {
            if(f) fprintf(f, "ERROR: alcCreateContext returned NULL\n"), fflush(f);
            alcCloseDevice(gOpenALDevice);
            gOpenALDevice = NULL;
            if(f) fclose(f);
            return FALSE;
         }

         // Make context current
         if (!alcMakeContextCurrent(gOpenALContext))
         {
            if(f) fprintf(f, "ERROR: alcMakeContextCurrent failed\n"), fflush(f);
            alcDestroyContext(gOpenALContext);
            alcCloseDevice(gOpenALDevice);
            gOpenALContext = NULL;
            gOpenALDevice = NULL;
            if(f) fclose(f);
            return FALSE;
         }

         // Set listener parameters
         alListener3f(AL_POSITION, 0, 0, 0);
         alListener3f(AL_VELOCITY, 0, 0, 0);
         ALfloat lOrient[] = {0, 0, -1, 0, 1, 0};
         alListenerfv(AL_ORIENTATION, lOrient);

         if(f) fprintf(f, "MR_SoundServer::Init SUCCESS\n"), fflush(f), fclose(f);
         return TRUE;
      }
      catch (...)
      {
         FILE* f = fopen("C:\\temp\\audio_debug.log", "a");
         if(f) fprintf(f, "ERROR: Exception in Init\n"), fflush(f), fclose(f);
         return FALSE;
      }
   }

   void Close()
   {
      try
      {
         MR_SoundBuffer::DeleteAll();

         if (gOpenALContext != NULL)
         {
            alcMakeContextCurrent(NULL);
            alcDestroyContext(gOpenALContext);
            gOpenALContext = NULL;
         }

         if (gOpenALDevice != NULL)
         {
            alcCloseDevice(gOpenALDevice);
            gOpenALDevice = NULL;
         }
      }
      catch (...)
      {
         // Ignore cleanup errors
      }
   }

   MR_ShortSound* CreateShortSound(const char* pData, int pNbCopy)
   {
      try
      {
         if (gOpenALContext == NULL)
            return NULL;

         MR_ShortSound* lSound = new MR_ShortSound;
         if (!lSound->Init(pData, pNbCopy))
         {
            delete lSound;
            return NULL;
         }
         return lSound;
      }
      catch (...)
      {
         return NULL;
      }
   }

   void DeleteShortSound(MR_ShortSound* pSound)
   {
      delete pSound;
   }

   void Play(MR_ShortSound* pSound, int pDB, double pSpeed, int pPan)
   {
      if (pSound != NULL)
      {
         try
         {
            pSound->Play(pDB, pSpeed, pPan);
         }
         catch (...)
         {
         }
      }
   }

   int GetNbCopy(MR_ShortSound* pSound)
   {
      if (pSound != NULL)
         return pSound->GetNbCopy();
      return 1;
   }

   MR_ContinuousSound* CreateContinuousSound(const char* pData, int pNbCopy)
   {
      try
      {
         if (gOpenALContext == NULL)
            return NULL;

         MR_ContinuousSound* lSound = new MR_ContinuousSound;
         if (!lSound->Init(pData, pNbCopy))
         {
            delete lSound;
            return NULL;
         }
         return lSound;
      }
      catch (...)
      {
         return NULL;
      }
   }

   void DeleteContinuousSound(MR_ContinuousSound* pSound)
   {
      delete pSound;
   }

   void Play(MR_ContinuousSound* pSound, int pCopy, int pDB, double pSpeed, int pPan)
   {
      if (pSound != NULL)
      {
         try
         {
            pSound->CumPlay(pCopy, pDB, pSpeed);
         }
         catch (...)
         {
         }
      }
   }

   int GetNbCopy(MR_ContinuousSound* pSound)
   {
      if (pSound != NULL)
         return pSound->GetNbCopy();
      return 1;
   }

   void ApplyContinuousPlay()
   {
      try
      {
         if (gOpenALContext != NULL)
         {
            MR_SoundBuffer::ApplyCumCommandForAll();
         }
      }
      catch (...)
      {
      }
   }
}
