/*
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2007 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1335 USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#ifndef AL_H
#define AL_H

#ifdef __cplusplus
extern "C" {
#endif

#define AL_VERSION_1_0
#define AL_VERSION_1_1

#include <stddef.h>

/* Enumerant values begin at column 50. No tabs. */

/* bad value */
#define AL_INVALID                               (-1)
#define AL_NONE                                  0

/* Boolean values */
#define AL_FALSE                                 0
#define AL_TRUE                                  1

/* Errors */
#define AL_NO_ERROR                              AL_FALSE
#define AL_INVALID_NAME                          0xA001
#define AL_INVALID_ENUM                          0xA002
#define AL_INVALID_VALUE                         0xA003
#define AL_INVALID_OPERATION                     0xA004
#define AL_OUT_OF_MEMORY                         0xA005

/* Numeric types */
typedef char                ALchar;
typedef short               ALshort;
typedef int                 ALint;
typedef unsigned int        ALuint;
typedef int                 ALsizei;
typedef float               ALfloat;
typedef double              ALdouble;
typedef unsigned int        ALenum;
typedef int                 ALboolean;
typedef void                ALvoid;

/* Listener parameters */
#define AL_GAIN                                  0x100000
#define AL_POSITION                              0x1004
#define AL_VELOCITY                              0x1006
#define AL_ORIENTATION                           0x100F

/* Source attributes */
#define AL_PITCH                                 0x1003
#define AL_BUFFER                                0x1009
#define AL_SOURCE_STATE                          0x1010
#define AL_LOOPING                               0x1007

/* Source state values */
#define AL_INITIAL                               0
#define AL_PLAYING                               0x1014
#define AL_PAUSED                                0x1015
#define AL_STOPPED                               0x1016

/* Buffer attributes */
#define AL_FREQUENCY                             0x2001
#define AL_BITS                                  0x2002
#define AL_CHANNELS                              0x2003
#define AL_SIZE                                  0x2004

/* Audio format encoding */
#define AL_FORMAT_MONO8                          0x1100
#define AL_FORMAT_MONO16                         0x1101
#define AL_FORMAT_STEREO8                        0x1102
#define AL_FORMAT_STEREO16                       0x1103

/* Global state */
extern ALvoid alEnable(ALenum capability);
extern ALvoid alDisable(ALenum capability);
extern ALboolean alIsEnabled(ALenum capability);
extern const ALchar* alGetString(ALenum param);
extern ALvoid alGetBooleanv(ALenum param, ALboolean *values);
extern ALvoid alGetIntegerv(ALenum param, ALint *values);
extern ALvoid alGetFloatv(ALenum param, ALfloat *values);
extern ALvoid alGetDoublev(ALenum param, ALdouble *values);
extern ALboolean alGetBoolean(ALenum param);
extern ALint alGetInteger(ALenum param);
extern ALfloat alGetFloat(ALenum param);
extern ALdouble alGetDouble(ALenum param);
extern ALenum alGetError(void);

/* Listener functions */
extern ALvoid alListenerf(ALenum param, ALfloat value);
extern ALvoid alListener3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
extern ALvoid alListenerfv(ALenum param, const ALfloat *values);
extern ALvoid alListeneri(ALenum param, ALint value);
extern ALvoid alListener3i(ALenum param, ALint value1, ALint value2, ALint value3);
extern ALvoid alListeneriv(ALenum param, const ALint *values);
extern ALvoid alGetListenerf(ALenum param, ALfloat *value);
extern ALvoid alGetListener3f(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
extern ALvoid alGetListenerfv(ALenum param, ALfloat *values);
extern ALvoid alGetListeneri(ALenum param, ALint *value);
extern ALvoid alGetListener3i(ALenum param, ALint *value1, ALint *value2, ALint *value3);
extern ALvoid alGetListeneriv(ALenum param, ALint *values);

/* Source functions */
extern ALvoid alGenSources(ALsizei n, ALuint *sources);
extern ALvoid alDeleteSources(ALsizei n, const ALuint *sources);
extern ALboolean alIsSource(ALuint source);
extern ALvoid alSourcef(ALuint source, ALenum param, ALfloat value);
extern ALvoid alSource3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
extern ALvoid alSourcefv(ALuint source, ALenum param, const ALfloat *values);
extern ALvoid alSourcei(ALuint source, ALenum param, ALint value);
extern ALvoid alSource3i(ALuint source, ALenum param, ALint value1, ALint value2, ALint value3);
extern ALvoid alSourceiv(ALuint source, ALenum param, const ALint *values);
extern ALvoid alGetSourcef(ALuint source, ALenum param, ALfloat *value);
extern ALvoid alGetSource3f(ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
extern ALvoid alGetSourcefv(ALuint source, ALenum param, ALfloat *values);
extern ALvoid alGetSourcei(ALuint source, ALenum param, ALint *value);
extern ALvoid alGetSource3i(ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3);
extern ALvoid alGetSourceiv(ALuint source, ALenum param, ALint *values);
extern ALvoid alSourcePlayv(ALsizei n, const ALuint *sources);
extern ALvoid alSourceStopv(ALsizei n, const ALuint *sources);
extern ALvoid alSourceRewindv(ALsizei n, const ALuint *sources);
extern ALvoid alSourcePausev(ALsizei n, const ALuint *sources);
extern ALvoid alSourcePlay(ALuint source);
extern ALvoid alSourceStop(ALuint source);
extern ALvoid alSourceRewind(ALuint source);
extern ALvoid alSourcePause(ALuint source);
extern ALvoid alSourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers);
extern ALvoid alSourceUnqueueBuffers(ALuint source, ALsizei nb, ALuint *buffers);

/* Buffer functions */
extern ALvoid alGenBuffers(ALsizei n, ALuint *buffers);
extern ALvoid alDeleteBuffers(ALsizei n, const ALuint *buffers);
extern ALboolean alIsBuffer(ALuint buffer);
extern ALvoid alBufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);
extern ALvoid alBufferf(ALuint buffer, ALenum param, ALfloat value);
extern ALvoid alBuffer3f(ALuint buffer, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
extern ALvoid alBufferfv(ALuint buffer, ALenum param, const ALfloat *values);
extern ALvoid alBufferi(ALuint buffer, ALenum param, ALint value);
extern ALvoid alBuffer3i(ALuint buffer, ALenum param, ALint value1, ALint value2, ALint value3);
extern ALvoid alBufferiv(ALuint buffer, ALenum param, const ALint *values);
extern ALvoid alGetBufferf(ALuint buffer, ALenum param, ALfloat *value);
extern ALvoid alGetBuffer3f(ALuint buffer, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
extern ALvoid alGetBufferfv(ALuint buffer, ALenum param, ALfloat *values);
extern ALvoid alGetBufferi(ALuint buffer, ALenum param, ALint *value);
extern ALvoid alGetBuffer3i(ALuint buffer, ALenum param, ALint *value1, ALint *value2, ALint *value3);
extern ALvoid alGetBufferiv(ALuint buffer, ALenum param, ALint *values);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif
