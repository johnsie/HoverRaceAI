/*
 * Windows Multimedia Audio Backend for HoverRace
 * Implements OpenAL API using waveOut* Windows Multimedia
 * This replaces the non-functional DirectSound stub
 */

#include "al.h"
#include "alc.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#pragma comment(lib, "winmm.lib")

/* Ensure waveOut functions are declared */
#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 1
#endif

/* Logging helper */
static void LogAudio(const char* fmt, ...) {
    FILE* f = fopen("C:\\temp\\audio_debug.log", "a");
    if (f) {
        va_list args;
        va_start(args, fmt);
        vfprintf(f, fmt, args);
        va_end(args);
        fprintf(f, "\n");
        fflush(f);
        fclose(f);
    }
}

/* Audio buffer management */
#define MAX_AUDIO_BUFFERS 256
#define MAX_WAVE_BUFFERS 32
#define BUFFER_POOL_SIZE (64 * 1024)  // 64KB pool per buffer

struct AudioBuffer {
    ALuint id;
    ALenum format;
    int frequency;
    LPSTR pAudioData;
    DWORD dwAudioDataSize;
    int refCount;
};

struct AudioSource {
    ALuint id;
    ALuint bufferId;
    float gain;
    float pitch;
    ALboolean looping;
    ALenum state;  // AL_INITIAL, AL_PLAYING, AL_PAUSED, AL_STOPPED
    DWORD playPosition;
    HWAVEOUT hWaveOut;
    WAVEHDR waveHeaders[MAX_WAVE_BUFFERS];
    int currentWaveBuffer;
};

/* Global state */
static AudioBuffer g_buffers[MAX_AUDIO_BUFFERS];
static int g_bufferCount = 0;
static AudioSource g_sources[128];
static int g_sourceCount = 0;
static CRITICAL_SECTION g_audioLock;
static HWAVEOUT g_hWaveOut = NULL;
static ALenum g_alError = AL_NO_ERROR;

/* Helper function: Convert dB to linear gain */
static float dbToGain(int db) {
    return powf(10.0f, (float)db / 20.0f);
}

/* Initialize audio system */
static BOOL InitAudioSystem() {
    if (g_hWaveOut != NULL) {
        LogAudio("InitAudioSystem: Already initialized");
        return TRUE;
    }

    LogAudio("InitAudioSystem: Initializing waveOut device...");

    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;  // Mono - most game sounds are mono
    wfx.nSamplesPerSec = 11025;  // Standard game audio rate
    wfx.wBitsPerSample = 8;  // Most legacy game audio is 8-bit
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    MMRESULT result = waveOutOpen(&g_hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    LogAudio("InitAudioSystem: waveOutOpen result=%d (handle=%p)", result, g_hWaveOut);
    
    if (result != MMSYSERR_NOERROR) {
        // Fallback to 16-bit 22050Hz
        LogAudio("InitAudioSystem: Trying fallback format (16-bit 22050Hz mono)");
        wfx.wBitsPerSample = 16;
        wfx.nSamplesPerSec = 22050;
        wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
        result = waveOutOpen(&g_hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
        LogAudio("InitAudioSystem: Fallback result=%d (handle=%p)", result, g_hWaveOut);
    }
    
    if (result == MMSYSERR_NOERROR) {
        LogAudio("InitAudioSystem: Success!");
        return TRUE;
    }
    
    LogAudio("InitAudioSystem: FAILED!");
    return FALSE;
}

/* Re-initialize audio system for a specific format (if needed) */
static BOOL ReinitAudioSystem(int channels, int samplesPerSec, int bitsPerSample) {
    // Close existing device if needed
    if (g_hWaveOut != NULL) {
        // Check if format matches - if so, no need to reinit
        if (channels == 1 && samplesPerSec == 11025 && bitsPerSample == 8) {
            return TRUE;
        }
        if (channels == 1 && samplesPerSec == 22050 && bitsPerSample == 16) {
            return TRUE;
        }
        // Format mismatch - close and reopen with new format
        waveOutClose(g_hWaveOut);
        g_hWaveOut = NULL;
    }

    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = channels;
    wfx.nSamplesPerSec = samplesPerSec;
    wfx.wBitsPerSample = bitsPerSample;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    MMRESULT result = waveOutOpen(&g_hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    if (result == MMSYSERR_NOERROR) {
        return TRUE;
    }

    // Try common fallback formats
    if (result != MMSYSERR_NOERROR) {
        wfx.nChannels = 1;
        wfx.nSamplesPerSec = 22050;
        wfx.wBitsPerSample = 16;
        wfx.nBlockAlign = 2;
        wfx.nAvgBytesPerSec = 44100;
        result = waveOutOpen(&g_hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    }

    return (result == MMSYSERR_NOERROR);
}

/* Buffer Functions */
ALvoid alGenBuffers(ALsizei n, ALuint *buffers) {
    EnterCriticalSection(&g_audioLock);
    
    for (int i = 0; i < n; i++) {
        if (g_bufferCount < MAX_AUDIO_BUFFERS) {
            int idx = g_bufferCount++;
            g_buffers[idx].id = idx + 1;
            g_buffers[idx].format = 0;
            g_buffers[idx].frequency = 0;
            g_buffers[idx].pAudioData = NULL;
            g_buffers[idx].dwAudioDataSize = 0;
            g_buffers[idx].refCount = 0;
            buffers[i] = g_buffers[idx].id;
        } else {
            buffers[i] = 0;
        }
    }
    
    LeaveCriticalSection(&g_audioLock);
}

ALvoid alDeleteBuffers(ALsizei n, const ALuint *buffers) {
    EnterCriticalSection(&g_audioLock);
    
    for (int i = 0; i < n; i++) {
        if (buffers[i] > 0 && buffers[i] <= MAX_AUDIO_BUFFERS) {
            int idx = buffers[i] - 1;
            if (g_buffers[idx].pAudioData != NULL) {
                free(g_buffers[idx].pAudioData);
                g_buffers[idx].pAudioData = NULL;
            }
            g_buffers[idx].id = 0;
        }
    }
    
    LeaveCriticalSection(&g_audioLock);
}

ALboolean alIsBuffer(ALuint buffer) {
    return (buffer > 0 && buffer <= MAX_AUDIO_BUFFERS && g_buffers[buffer - 1].id != 0);
}

ALvoid alBufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei frequency) {
    EnterCriticalSection(&g_audioLock);
    
    LogAudio("alBufferData: buffer=%d, format=%d, size=%d, freq=%d, data=%p", buffer, format, size, frequency, data);
    
    if (buffer > 0 && buffer <= MAX_AUDIO_BUFFERS) {
        int idx = buffer - 1;
        
        // Ensure audio system initialized for format negotiation
        if (g_hWaveOut == NULL) {
            LogAudio("  Initializing audio system...");
            InitAudioSystem();
        }
        
        // Free old data
        if (g_buffers[idx].pAudioData != NULL) {
            free(g_buffers[idx].pAudioData);
        }
        
        // Allocate and copy new data
        g_buffers[idx].pAudioData = (LPSTR)malloc(size);
        if (g_buffers[idx].pAudioData != NULL) {
            memcpy(g_buffers[idx].pAudioData, data, size);
            g_buffers[idx].dwAudioDataSize = size;
            g_buffers[idx].format = format;
            g_buffers[idx].frequency = frequency;
            LogAudio("  Buffer loaded successfully!");
        } else {
            g_alError = AL_OUT_OF_MEMORY;
            LogAudio("  ERROR: malloc failed!");
        }
    } else {
        LogAudio("  ERROR: Invalid buffer ID!");
    }
    
    LeaveCriticalSection(&g_audioLock);
}

/* Source Functions */
ALvoid alGenSources(ALsizei n, ALuint *sources) {
    EnterCriticalSection(&g_audioLock);
    
    for (int i = 0; i < n; i++) {
        if (g_sourceCount < 128) {
            int idx = g_sourceCount++;
            g_sources[idx].id = idx + 1;
            g_sources[idx].bufferId = 0;
            g_sources[idx].gain = 1.0f;
            g_sources[idx].pitch = 1.0f;
            g_sources[idx].looping = AL_FALSE;
            g_sources[idx].state = AL_INITIAL;
            g_sources[idx].playPosition = 0;
            g_sources[idx].hWaveOut = NULL;
            g_sources[idx].currentWaveBuffer = 0;
            memset(g_sources[idx].waveHeaders, 0, sizeof(g_sources[idx].waveHeaders));
            sources[i] = g_sources[idx].id;
        } else {
            sources[i] = 0;
        }
    }
    
    LeaveCriticalSection(&g_audioLock);
}

ALvoid alDeleteSources(ALsizei n, const ALuint *sources) {
    EnterCriticalSection(&g_audioLock);
    
    for (int i = 0; i < n; i++) {
        if (sources[i] > 0 && sources[i] <= 128) {
            int idx = sources[i] - 1;
            
            // Stop playing if active
            if (g_sources[idx].hWaveOut != NULL) {
                waveOutReset(g_sources[idx].hWaveOut);
            }
            
            g_sources[idx].id = 0;
            g_sources[idx].state = AL_STOPPED;
        }
    }
    
    LeaveCriticalSection(&g_audioLock);
}

ALboolean alIsSource(ALuint source) {
    return (source > 0 && source <= 128 && g_sources[source - 1].id != 0);
}

ALvoid alSourcei(ALuint source, ALenum param, ALint value) {
    EnterCriticalSection(&g_audioLock);
    
    if (source > 0 && source <= 128) {
        int idx = source - 1;
        switch (param) {
            case AL_BUFFER:
                g_sources[idx].bufferId = value;
                break;
            case AL_LOOPING:
                g_sources[idx].looping = (value ? AL_TRUE : AL_FALSE);
                break;
        }
    }
    
    LeaveCriticalSection(&g_audioLock);
}

ALvoid alSourcef(ALuint source, ALenum param, ALfloat value) {
    EnterCriticalSection(&g_audioLock);
    
    if (source > 0 && source <= 128) {
        int idx = source - 1;
        switch (param) {
            case AL_GAIN:
                g_sources[idx].gain = (value < 0.0f) ? 0.0f : (value > 1.0f ? 1.0f : value);
                break;
            case AL_PITCH:
                g_sources[idx].pitch = (value < 0.5f) ? 0.5f : (value > 2.0f ? 2.0f : value);
                break;
        }
    }
    
    LeaveCriticalSection(&g_audioLock);
}

ALvoid alSource3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3) {
    // Position/velocity - not fully implemented but accepted
}

ALvoid alSourcePlay(ALuint source) {
    EnterCriticalSection(&g_audioLock);
    
    LogAudio("alSourcePlay: source=%d", source);
    
    if (source > 0 && source <= 128) {
        int idx = source - 1;
        if (g_sources[idx].state != AL_PLAYING) {
            g_sources[idx].state = AL_PLAYING;
            g_sources[idx].playPosition = 0;
            
            // Get buffer data
            if (g_sources[idx].bufferId > 0 && g_sources[idx].bufferId <= MAX_AUDIO_BUFFERS) {
                int bufIdx = g_sources[idx].bufferId - 1;
                AudioBuffer* buf = &g_buffers[bufIdx];
                
                LogAudio("  bufferId=%d, pAudioData=%p, size=%d, format=%d, freq=%d",
                         g_sources[idx].bufferId, buf->pAudioData, buf->dwAudioDataSize, buf->format, buf->frequency);
                
                if (buf->pAudioData != NULL) {
                    // Determine format parameters from AL format
                    int channels = 1, bitsPerSample = 8, samplesPerSec = 11025;
                    
                    if (buf->format == AL_FORMAT_MONO8) {
                        channels = 1; bitsPerSample = 8;
                    } else if (buf->format == AL_FORMAT_MONO16) {
                        channels = 1; bitsPerSample = 16;
                    } else if (buf->format == AL_FORMAT_STEREO8) {
                        channels = 2; bitsPerSample = 8;
                    } else if (buf->format == AL_FORMAT_STEREO16) {
                        channels = 2; bitsPerSample = 16;
                    }
                    
                    if (buf->frequency > 0) {
                        samplesPerSec = buf->frequency;
                    }
                    
                    LogAudio("  Format: %dch, %dbit, %dHz", channels, bitsPerSample, samplesPerSec);
                    
                    // Ensure waveOut is initialized for this format
                    if (!ReinitAudioSystem(channels, samplesPerSec, bitsPerSample)) {
                        LogAudio("  ERROR: ReinitAudioSystem failed!");
                        g_sources[idx].state = AL_STOPPED;
                        LeaveCriticalSection(&g_audioLock);
                        return;
                    }
                    
                    // Allocate persistent header for this playback
                    WAVEHDR* hdr = (WAVEHDR*)malloc(sizeof(WAVEHDR));
                    if (hdr) {
                        memset(hdr, 0, sizeof(WAVEHDR));
                        
                        // Set up the buffer data
                        hdr->lpData = buf->pAudioData;
                        hdr->dwBufferLength = buf->dwAudioDataSize;
                        hdr->dwLoops = g_sources[idx].looping ? 1 : 0;
                        hdr->dwFlags = 0;
                        
                        LogAudio("  Preparing header (size=%d, looping=%d)", hdr->dwBufferLength, hdr->dwLoops);
                        
                        // Prepare the header for waveOut
                        MMRESULT res = waveOutPrepareHeader(g_hWaveOut, hdr, sizeof(WAVEHDR));
                        LogAudio("  waveOutPrepareHeader result=%d", res);
                        
                        if (res == MMSYSERR_NOERROR) {
                            // Queue the buffer for playback
                            res = waveOutWrite(g_hWaveOut, hdr, sizeof(WAVEHDR));
                            LogAudio("  waveOutWrite result=%d", res);
                            
                            if (res == MMSYSERR_NOERROR) {
                                // Success - waveOut now owns the header
                                LogAudio("  Playback queued successfully!");
                            } else {
                                // Failed to write - clean up
                                LogAudio("  ERROR: waveOutWrite failed!");
                                g_sources[idx].state = AL_STOPPED;
                                waveOutUnprepareHeader(g_hWaveOut, hdr, sizeof(WAVEHDR));
                                free(hdr);
                            }
                        } else {
                            // Failed to prepare - clean up
                            LogAudio("  ERROR: waveOutPrepareHeader failed!");
                            g_sources[idx].state = AL_STOPPED;
                            free(hdr);
                        }
                    } else {
                        LogAudio("  ERROR: malloc failed!");
                        g_sources[idx].state = AL_STOPPED;
                    }
                } else {
                    LogAudio("  ERROR: No audio data!");
                }
            } else {
                LogAudio("  ERROR: Invalid buffer ID!");
            }
        } else {
            LogAudio("  Already playing");
        }
    } else {
        LogAudio("  ERROR: Invalid source ID!");
    }
    
    LeaveCriticalSection(&g_audioLock);
}

ALvoid alSourceStop(ALuint source) {
    EnterCriticalSection(&g_audioLock);
    
    if (source > 0 && source <= 128) {
        int idx = source - 1;
        g_sources[idx].state = AL_STOPPED;
        g_sources[idx].playPosition = 0;
        
        if (g_sources[idx].hWaveOut != NULL) {
            waveOutReset(g_sources[idx].hWaveOut);
        }
    }
    
    LeaveCriticalSection(&g_audioLock);
}

ALvoid alSourceRewind(ALuint source) {
    EnterCriticalSection(&g_audioLock);
    
    if (source > 0 && source <= 128) {
        int idx = source - 1;
        g_sources[idx].playPosition = 0;
        g_sources[idx].state = AL_INITIAL;
    }
    
    LeaveCriticalSection(&g_audioLock);
}

ALvoid alSourcePause(ALuint source) {
    EnterCriticalSection(&g_audioLock);
    
    if (source > 0 && source <= 128) {
        int idx = source - 1;
        if (g_sources[idx].state == AL_PLAYING) {
            g_sources[idx].state = AL_PAUSED;
            if (g_sources[idx].hWaveOut != NULL) {
                waveOutPause(g_sources[idx].hWaveOut);
            }
        }
    }
    
    LeaveCriticalSection(&g_audioLock);
}

/* Error handling */
ALenum alGetError(void) {
    ALenum err = g_alError;
    g_alError = AL_NO_ERROR;
    return err;
}

/* Context Functions */
ALCdevice *alcOpenDevice(const ALCchar *devicename) {
    LogAudio("alcOpenDevice: Initializing...");
    InitAudioSystem();
    if (g_hWaveOut != NULL) {
        LogAudio("  Success - device=%p", (ALCdevice*)0x1);
        return (ALCdevice*)0x1;
    }
    LogAudio("  FAILED!");
    return NULL;
}

ALCboolean alcCloseDevice(ALCdevice *device) {
    LogAudio("alcCloseDevice");
    if (g_hWaveOut != NULL) {
        waveOutClose(g_hWaveOut);
        g_hWaveOut = NULL;
    }
    return ALC_TRUE;
}

ALCcontext *alcCreateContext(ALCdevice *device, const ALCint *attrlist) {
    LogAudio("alcCreateContext: device=%p", device);
    return (ALCcontext*)0x1;
}

ALCboolean alcMakeContextCurrent(ALCcontext *context) {
    LogAudio("alcMakeContextCurrent: context=%p", context);
    return ALC_TRUE;
}

ALCboolean alcDestroyContext(ALCcontext *context) {
    LogAudio("alcDestroyContext");
    return ALC_TRUE;
}

/* Initialization - called once */
static void InitOnce() {
    static BOOL initialized = FALSE;
    if (!initialized) {
        InitializeCriticalSection(&g_audioLock);
        initialized = TRUE;
    }
}

/* Listener functions (no-op for basic audio) */
ALvoid alListener3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3) { }
ALvoid alListenerfv(ALenum param, const ALfloat *values) { }
ALvoid alListenerf(ALenum param, ALfloat value) { }
ALvoid alListeneri(ALenum param, ALint value) { }
ALvoid alListener3i(ALenum param, ALint value1, ALint value2, ALint value3) { }
ALvoid alListeneriv(ALenum param, const ALint *values) { }

ALvoid alGetListenerf(ALenum param, ALfloat *value) {
    if (value) *value = 0.0f;
}

ALvoid alGetListener3f(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3) {
    if (value1) *value1 = 0.0f;
    if (value2) *value2 = 0.0f;
    if (value3) *value3 = 0.0f;
}

ALvoid alGetListenerfv(ALenum param, ALfloat *values) {
    if (values) memset(values, 0, sizeof(ALfloat) * 4);
}

ALvoid alGetListeneri(ALenum param, ALint *value) {
    if (value) *value = 0;
}

ALvoid alGetListener3i(ALenum param, ALint *value1, ALint *value2, ALint *value3) {
    if (value1) *value1 = 0;
    if (value2) *value2 = 0;
    if (value3) *value3 = 0;
}

ALvoid alGetListeneriv(ALenum param, ALint *values) {
    if (values) memset(values, 0, sizeof(ALint) * 4);
}

/* Additional required stubs */
ALvoid alSourcePlayv(ALsizei n, const ALuint *sources) { }
ALvoid alSourceStopv(ALsizei n, const ALuint *sources) { }
ALvoid alSourceRewindv(ALsizei n, const ALuint *sources) { }
ALvoid alSourcePausev(ALsizei n, const ALuint *sources) { }
ALvoid alSourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers) { }
ALvoid alSourceUnqueueBuffers(ALuint source, ALsizei nb, ALuint *buffers) { }

ALvoid alGetSourcef(ALuint source, ALenum param, ALfloat *value) { if(value) *value = 0.0f; }
ALvoid alGetSource3f(ALuint source, ALenum param, ALfloat *v1, ALfloat *v2, ALfloat *v3) { }
ALvoid alGetSourcefv(ALuint source, ALenum param, ALfloat *values) { }
ALvoid alGetSourcei(ALuint source, ALenum param, ALint *value) { if(value) *value = 0; }
ALvoid alGetSource3i(ALuint source, ALenum param, ALint *v1, ALint *v2, ALint *v3) { }
ALvoid alGetSourceiv(ALuint source, ALenum param, ALint *values) { }

ALvoid alBufferf(ALuint buffer, ALenum param, ALfloat value) { }
ALvoid alBuffer3f(ALuint buffer, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3) { }
ALvoid alBufferfv(ALuint buffer, ALenum param, const ALfloat *values) { }
ALvoid alBufferi(ALuint buffer, ALenum param, ALint value) { }
ALvoid alBuffer3i(ALuint buffer, ALenum param, ALint value1, ALint value2, ALint value3) { }
ALvoid alBufferiv(ALuint buffer, ALenum param, const ALint *values) { }

ALvoid alGetBufferf(ALuint buffer, ALenum param, ALfloat *value) { }
ALvoid alGetBuffer3f(ALuint buffer, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3) { }
ALvoid alGetBufferfv(ALuint buffer, ALenum param, ALfloat *values) { }
ALvoid alGetBufferi(ALuint buffer, ALenum param, ALint *value) { }
ALvoid alGetBuffer3i(ALuint buffer, ALenum param, ALint *value1, ALint *value2, ALint *value3) { }
ALvoid alGetBufferiv(ALuint buffer, ALenum param, ALint *values) { }

/* ALC functions */
ALCboolean alcIsExtensionPresent(ALCdevice *device, const ALCchar *extname) { return ALC_FALSE; }
ALCvoid *alcGetProcAddress(ALCdevice *device, const ALCchar *funcname) { return NULL; }
ALCenum alcGetEnumValue(ALCdevice *device, const ALCchar *enumname) { return 0; }
const ALCchar *alcGetString(ALCdevice *device, ALCenum param) { return ""; }
ALCvoid alcGetIntegerv(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *dest) { }
ALCenum alcGetError(ALCdevice *device) { return ALC_NO_ERROR; }
ALCvoid alcProcessContext(ALCcontext *context) { }
ALCcontext *alcGetCurrentContext(ALCvoid) { return (ALCcontext*)0x1; }
ALCdevice *alcGetContextsDevice(ALCcontext *context) { return (ALCdevice*)0x1; }

// Ensure initialization
namespace {
    struct Init {
        Init() { InitOnce(); }
    } init;
}
