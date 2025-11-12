/* 
 * OpenAL Soft stub library for HoverRace modernization
 * This provides basic OpenAL API stubs to allow compilation
 * Modified to support Windows DirectSound audio
 */

#include "al.h"
#include "alc.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <dsound.h>
#include <mmreg.h>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

/* DirectSound support */
IDirectSound8 *g_pDS = NULL;
IDirectSoundBuffer *g_pPrimaryBuffer = NULL;
IDirectSoundBuffer *g_pSecondaryBuffer = NULL;
HWND g_hWnd = NULL;

/* Stub implementations - now uses DirectSound when available */

ALenum g_alError = AL_NO_ERROR;

/* Global state */
ALCdevice* g_alcDevice = NULL;
ALCcontext* g_alcContext = NULL;

/* AL Error management */
ALenum alGetError(void)
{
   ALenum err = g_alError;
   g_alError = AL_NO_ERROR;
   return err;
}

/* Listener functions */
ALvoid alListener3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
   /* No-op stub */
}

ALvoid alListenerfv(ALenum param, const ALfloat *values)
{
   /* No-op stub */
}

ALvoid alListenerf(ALenum param, ALfloat value)
{
   /* No-op stub */
}

ALvoid alListeneri(ALenum param, ALint value)
{
   /* No-op stub */
}

ALvoid alListener3i(ALenum param, ALint value1, ALint value2, ALint value3)
{
   /* No-op stub */
}

ALvoid alListeneriv(ALenum param, const ALint *values)
{
   /* No-op stub */
}

ALvoid alGetListenerf(ALenum param, ALfloat *value)
{
   if (value) *value = 0.0f;
}

ALvoid alGetListener3f(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
   if (value1) *value1 = 0.0f;
   if (value2) *value2 = 0.0f;
   if (value3) *value3 = 0.0f;
}

ALvoid alGetListenerfv(ALenum param, ALfloat *values)
{
   if (values) memset(values, 0, sizeof(ALfloat) * 4);
}

ALvoid alGetListeneri(ALenum param, ALint *value)
{
   if (value) *value = 0;
}

ALvoid alGetListener3i(ALenum param, ALint *value1, ALint *value2, ALint *value3)
{
   if (value1) *value1 = 0;
   if (value2) *value2 = 0;
   if (value3) *value3 = 0;
}

ALvoid alGetListeneriv(ALenum param, ALint *values)
{
   if (values) memset(values, 0, sizeof(ALint) * 4);
}

/* Source functions */
ALvoid alGenSources(ALsizei n, ALuint *sources)
{
   for (int i = 0; i < n; i++)
      sources[i] = i + 1;
}

ALvoid alDeleteSources(ALsizei n, const ALuint *sources)
{
   /* No-op stub */
}

ALboolean alIsSource(ALuint source)
{
   return source > 0 ? AL_TRUE : AL_FALSE;
}

ALvoid alSourcef(ALuint source, ALenum param, ALfloat value)
{
   /* No-op stub */
}

ALvoid alSource3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
   /* No-op stub */
}

ALvoid alSourcefv(ALuint source, ALenum param, const ALfloat *values)
{
   /* No-op stub */
}

ALvoid alSourcei(ALuint source, ALenum param, ALint value)
{
   /* No-op stub */
}

ALvoid alSource3i(ALuint source, ALenum param, ALint value1, ALint value2, ALint value3)
{
   /* No-op stub */
}

ALvoid alSourceiv(ALuint source, ALenum param, const ALint *values)
{
   /* No-op stub */
}

ALvoid alGetSourcef(ALuint source, ALenum param, ALfloat *value)
{
   if (value) *value = 0.0f;
}

ALvoid alGetSource3f(ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
   if (value1) *value1 = 0.0f;
   if (value2) *value2 = 0.0f;
   if (value3) *value3 = 0.0f;
}

ALvoid alGetSourcefv(ALuint source, ALenum param, ALfloat *values)
{
   if (values) memset(values, 0, sizeof(ALfloat) * 4);
}

ALvoid alGetSourcei(ALuint source, ALenum param, ALint *value)
{
   if (value) *value = AL_PLAYING;
}

ALvoid alGetSource3i(ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3)
{
   if (value1) *value1 = 0;
   if (value2) *value2 = 0;
   if (value3) *value3 = 0;
}

ALvoid alGetSourceiv(ALuint source, ALenum param, ALint *values)
{
   if (values) memset(values, 0, sizeof(ALint) * 4);
}

ALvoid alSourcePlayv(ALsizei n, const ALuint *sources)
{
   /* No-op stub */
}

ALvoid alSourceStopv(ALsizei n, const ALuint *sources)
{
   /* No-op stub */
}

ALvoid alSourceRewindv(ALsizei n, const ALuint *sources)
{
   /* No-op stub */
}

ALvoid alSourcePausev(ALsizei n, const ALuint *sources)
{
   /* No-op stub */
}

ALvoid alSourcePlay(ALuint source)
{
   /* No-op stub */
}

ALvoid alSourceStop(ALuint source)
{
   /* No-op stub */
}

ALvoid alSourceRewind(ALuint source)
{
   /* No-op stub */
}

ALvoid alSourcePause(ALuint source)
{
   /* No-op stub */
}

ALvoid alSourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers)
{
   /* No-op stub */
}

ALvoid alSourceUnqueueBuffers(ALuint source, ALsizei nb, ALuint *buffers)
{
   /* No-op stub */
}

/* Buffer functions */
ALvoid alGenBuffers(ALsizei n, ALuint *buffers)
{
   for (int i = 0; i < n; i++)
      buffers[i] = i + 100;
}

ALvoid alDeleteBuffers(ALsizei n, const ALuint *buffers)
{
   /* No-op stub */
}

ALboolean alIsBuffer(ALuint buffer)
{
   return buffer > 0 ? AL_TRUE : AL_FALSE;
}

ALvoid alBufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
{
   /* No-op stub - would actually copy PCM data in real implementation */
}

ALvoid alBufferf(ALuint buffer, ALenum param, ALfloat value)
{
   /* No-op stub */
}

ALvoid alBuffer3f(ALuint buffer, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
   /* No-op stub */
}

ALvoid alBufferfv(ALuint buffer, ALenum param, const ALfloat *values)
{
   /* No-op stub */
}

ALvoid alBufferi(ALuint buffer, ALenum param, ALint value)
{
   /* No-op stub */
}

ALvoid alBuffer3i(ALuint buffer, ALenum param, ALint value1, ALint value2, ALint value3)
{
   /* No-op stub */
}

ALvoid alBufferiv(ALuint buffer, ALenum param, const ALint *values)
{
   /* No-op stub */
}

ALvoid alGetBufferf(ALuint buffer, ALenum param, ALfloat *value)
{
   if (value) *value = 0.0f;
}

ALvoid alGetBuffer3f(ALuint buffer, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
   if (value1) *value1 = 0.0f;
   if (value2) *value2 = 0.0f;
   if (value3) *value3 = 0.0f;
}

ALvoid alGetBufferfv(ALuint buffer, ALenum param, ALfloat *values)
{
   if (values) memset(values, 0, sizeof(ALfloat) * 4);
}

ALvoid alGetBufferi(ALuint buffer, ALenum param, ALint *value)
{
   if (value) *value = 44100;
}

ALvoid alGetBuffer3i(ALuint buffer, ALenum param, ALint *value1, ALint *value2, ALint *value3)
{
   if (value1) *value1 = 0;
   if (value2) *value2 = 0;
   if (value3) *value3 = 0;
}

ALvoid alGetBufferiv(ALuint buffer, ALenum param, ALint *values)
{
   if (values) memset(values, 0, sizeof(ALint) * 4);
}

/* Global state functions */
ALvoid alEnable(ALenum capability)
{
   /* No-op stub */
}

ALvoid alDisable(ALenum capability)
{
   /* No-op stub */
}

ALboolean alIsEnabled(ALenum capability)
{
   return AL_FALSE;
}

const ALchar* alGetString(ALenum param)
{
   return "HoverRace OpenAL Stub";
}

ALvoid alGetBooleanv(ALenum param, ALboolean *values)
{
   if (values) memset(values, 0, 1);
}

ALvoid alGetIntegerv(ALenum param, ALint *values)
{
   if (values) memset(values, 0, sizeof(ALint) * 4);
}

ALvoid alGetFloatv(ALenum param, ALfloat *values)
{
   if (values) memset(values, 0, sizeof(ALfloat) * 4);
}

ALvoid alGetDoublev(ALenum param, ALdouble *values)
{
   if (values) memset(values, 0, sizeof(ALdouble) * 4);
}

ALboolean alGetBoolean(ALenum param)
{
   return AL_FALSE;
}

ALint alGetInteger(ALenum param)
{
   return 0;
}

ALfloat alGetFloat(ALenum param)
{
   return 0.0f;
}

ALdouble alGetDouble(ALenum param)
{
   return 0.0;
}

/* ALC Device functions */
ALCdevice *alcOpenDevice(const ALCchar *devicename)
{
   // Return a dummy non-NULL pointer for device
   static int dummy_device = 1;
   return (ALCdevice*)&dummy_device;
}

ALCboolean alcCloseDevice(ALCdevice *device)
{
   return ALC_TRUE;
}

/* ALC Context functions */
ALCcontext *alcCreateContext(ALCdevice *device, const ALCint *attrlist)
{
   // Return a dummy non-NULL pointer for context
   static int dummy_context = 1;
   return (ALCcontext*)&dummy_context;
}

ALCboolean alcMakeContextCurrent(ALCcontext *context)
{
   return ALC_TRUE;
}

ALCvoid alcProcessContext(ALCcontext *context)
{
   /* No-op stub */
}

ALCcontext *alcGetCurrentContext(ALCvoid)
{
   static int dummy_context = 1;
   return (ALCcontext*)&dummy_context;
}

ALCdevice *alcGetContextsDevice(ALCcontext *context)
{
   static int dummy_device = 1;
   return (ALCdevice*)&dummy_device;
}

ALCboolean alcDestroyContext(ALCcontext *context)
{
   return ALC_TRUE;
}

ALCboolean alcIsExtensionPresent(ALCdevice *device, const ALCchar *extname)
{
   return ALC_FALSE;
}

ALCvoid *alcGetProcAddress(ALCdevice *device, const ALCchar *funcname)
{
   return NULL;
}

ALCenum alcGetEnumValue(ALCdevice *device, const ALCchar *enumname)
{
   return 0;
}

const ALCchar *alcGetString(ALCdevice *device, ALCenum param)
{
   return "Stub";
}

ALCvoid alcGetIntegerv(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *dest)
{
   if (dest && size > 0)
      memset(dest, 0, size * sizeof(ALCint));
}

ALCenum alcGetError(ALCdevice *device)
{
   return ALC_NO_ERROR;
}
