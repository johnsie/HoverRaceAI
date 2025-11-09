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

#ifndef ALC_H
#define ALC_H

#include "al.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALC_VERSION_0_1

/* Forward declarations */
typedef void ALCdevice;
typedef void ALCcontext;
typedef char ALCchar;
typedef int ALCboolean;
typedef int ALCint;
typedef int ALCenum;
typedef int ALCsizei;
typedef void ALCvoid;
typedef void (*PFNALCGETINTEGERV)(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *dest);

/* Enumerant values begin at column 50. No tabs. */

/* bad value */
#define ALC_INVALID                              0

/* Boolean values */
#define ALC_FALSE                                0
#define ALC_TRUE                                 1

/* Errors */
#define ALC_NO_ERROR                             ALC_FALSE
#define ALC_INVALID_DEVICE                       0xA001
#define ALC_INVALID_CONTEXT                      0xA002
#define ALC_INVALID_ENUM                         0xA003
#define ALC_INVALID_VALUE                        0xA004
#define ALC_OUT_OF_MEMORY                        0xA005

/* Runtime attributes with get/set */
#define ALC_FREQUENCY                            0x1007
#define ALC_REFRESH                              0x1008
#define ALC_SYNC                                 0x1009

/* The Specifier string for default device */
#define ALC_DEFAULT_DEVICE_SPECIFIER             0x1004
#define ALC_DEVICE_SPECIFIER                     0x1005
#define ALC_EXTENSIONS                           0x1006

/* Parameters */
#define ALC_MAJOR_VERSION                        0x1000
#define ALC_MINOR_VERSION                        0x1001

#define ALC_ATTRIBUTES_SIZE                      0x1002
#define ALC_ALL_ATTRIBUTES                       0x1003

/* Functions */
extern ALCdevice *alcOpenDevice(const ALCchar *devicename);
extern ALCboolean alcCloseDevice(ALCdevice *device);
extern ALCcontext *alcCreateContext(ALCdevice *device, const ALCint *attrlist);
extern ALCboolean alcMakeContextCurrent(ALCcontext *context);
extern ALCvoid alcProcessContext(ALCcontext *context);
extern ALCcontext *alcGetCurrentContext(ALCvoid);
extern ALCdevice *alcGetContextsDevice(ALCcontext *context);
extern ALCboolean alcDestroyContext(ALCcontext *context);
extern ALCboolean alcIsExtensionPresent(ALCdevice *device, const ALCchar *extname);
extern ALCvoid *alcGetProcAddress(ALCdevice *device, const ALCchar *funcname);
extern ALCenum alcGetEnumValue(ALCdevice *device, const ALCchar *enumname);
extern const ALCchar *alcGetString(ALCdevice *device, ALCenum param);
extern ALCvoid alcGetIntegerv(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *dest);
extern ALCenum alcGetError(ALCdevice *device);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif
