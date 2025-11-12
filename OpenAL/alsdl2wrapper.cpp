/* 
 * OpenAL wrapper using SDL2 audio for HoverRace
 * This bridges OpenAL calls to SDL2's audio system
 */

#include "al.h"
#include "alc.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <map>
#include <vector>

/* Global state */
ALCdevice* g_alcDevice = NULL;
ALCcontext* g_alcContext = NULL;
ALenum g_alError = AL_NO_ERROR;

/* SDL2 audio state */
SDL_AudioDeviceID g_audioDevice = 0;
SDL_AudioSpec g_desiredSpec, g_obtainedSpec;

/* Track created buffers and sources */
struct ALBuffer {
   ALuint id;
   std::vector<ALshort> samples;
   ALenum format;
   ALsizei frequency;
};

struct ALSource {
   ALuint id;
   ALuint buffer;
   ALfloat gain;
   ALfloat pitch;
   ALboolean looping;
   ALboolean playing;
};

static std::map<ALuint, ALBuffer> g_buffers;
static std::map<ALuint, ALSource> g_sources;
static ALuint g_nextBufferId = 1;
static ALuint g_nextSourceId = 1;

/* AL Error management */
ALenum alGetError(void)
{
   ALenum err = g_alError;
   g_alError = AL_NO_ERROR;
   return err;
}

/* Listener functions - no-op for now */
ALvoid alListener3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
}

ALvoid alListenerfv(ALenum param, const ALfloat *values)
{
}

ALvoid alListenerf(ALenum param, ALfloat value)
{
}

ALvoid alListeneri(ALenum param, ALint value)
{
}

ALvoid alListener3i(ALenum param, ALint value1, ALint value2, ALint value3)
{
}

/* Source functions */
ALvoid alGenSources(ALsizei n, ALuint *sources)
{
   for(ALsizei i = 0; i < n; i++) {
      sources[i] = g_nextSourceId;
      ALSource src;
      src.id = g_nextSourceId;
      src.buffer = 0;
      src.gain = 1.0f;
      src.pitch = 1.0f;
      src.looping = AL_FALSE;
      src.playing = AL_FALSE;
      g_sources[g_nextSourceId] = src;
      g_nextSourceId++;
   }
}

ALvoid alDeleteSources(ALsizei n, const ALuint *sources)
{
   for(ALsizei i = 0; i < n; i++) {
      g_sources.erase(sources[i]);
   }
}

ALvoid alSourcef(ALuint source, ALenum param, ALfloat value)
{
   if(g_sources.find(source) != g_sources.end()) {
      switch(param) {
         case AL_GAIN:
            g_sources[source].gain = value;
            break;
         case AL_PITCH:
            g_sources[source].pitch = value;
            break;
      }
   }
}

ALvoid alSourcei(ALuint source, ALenum param, ALint value)
{
   if(g_sources.find(source) != g_sources.end()) {
      switch(param) {
         case AL_BUFFER:
            g_sources[source].buffer = value;
            break;
         case AL_LOOPING:
            g_sources[source].looping = value;
            break;
      }
   }
}

ALvoid alSource3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
   // Handle position, velocity, direction parameters
   // For now, just a stub
}

ALvoid alGetSourcei(ALuint source, ALenum param, ALint *value)
{
   if(value) {
      if(g_sources.find(source) != g_sources.end()) {
         switch(param) {
            case AL_SOURCE_STATE:
               *value = g_sources[source].playing ? AL_PLAYING : AL_STOPPED;
               break;
            default:
               *value = 0;
               break;
         }
      } else {
         *value = 0;
      }
   }
}

ALvoid alSourceRewind(ALuint source)
{
   // Rewind the source to the beginning
   if(g_sources.find(source) != g_sources.end()) {
      g_sources[source].playing = AL_FALSE;
   }
}

ALvoid alSourcePlay(ALuint source)
{
   if(g_sources.find(source) != g_sources.end()) {
      g_sources[source].playing = AL_TRUE;
   }
}

ALvoid alSourceStop(ALuint source)
{
   if(g_sources.find(source) != g_sources.end()) {
      g_sources[source].playing = AL_FALSE;
   }
}

ALvoid alSourcePause(ALuint source)
{
   if(g_sources.find(source) != g_sources.end()) {
      g_sources[source].playing = AL_FALSE;
   }
}

/* Buffer functions */
ALvoid alGenBuffers(ALsizei n, ALuint *buffers)
{
   for(ALsizei i = 0; i < n; i++) {
      buffers[i] = g_nextBufferId;
      ALBuffer buf;
      buf.id = g_nextBufferId;
      buf.format = AL_FORMAT_MONO16;
      buf.frequency = 22050;
      g_buffers[g_nextBufferId] = buf;
      g_nextBufferId++;
   }
}

ALvoid alDeleteBuffers(ALsizei n, const ALuint *buffers)
{
   for(ALsizei i = 0; i < n; i++) {
      g_buffers.erase(buffers[i]);
   }
}

ALvoid alBufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
{
   if(g_buffers.find(buffer) != g_buffers.end()) {
      g_buffers[buffer].format = format;
      g_buffers[buffer].frequency = freq;
      
      // Copy sample data
      ALsizei samples = size / sizeof(ALshort);
      g_buffers[buffer].samples.clear();
      const ALshort *srcData = (const ALshort *)data;
      for(ALsizei i = 0; i < samples; i++) {
         g_buffers[buffer].samples.push_back(srcData[i]);
      }
   }
}

/* ALC Device functions */
ALCdevice* alcOpenDevice(const ALCchar *devicename)
{
   if(g_alcDevice != NULL)
      return g_alcDevice;
   
   // Initialize SDL audio if not already done
   if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
      return NULL;
   }
   
   // Try to open a default audio device
   SDL_zero(g_desiredSpec);
   g_desiredSpec.freq = 44100;
   g_desiredSpec.format = AUDIO_S16;
   g_desiredSpec.channels = 2;
   g_desiredSpec.samples = 4096;
   g_desiredSpec.callback = NULL;  // Use polling mode
   
   g_audioDevice = SDL_OpenAudioDevice(NULL, 0, &g_desiredSpec, &g_obtainedSpec, 0);
   if(g_audioDevice == 0) {
      return NULL;
   }
   
   // Create a fake device pointer
   g_alcDevice = (ALCdevice *)0x12345678;
   
   SDL_PauseAudioDevice(g_audioDevice, 0);
   
   return g_alcDevice;
}

ALCboolean alcCloseDevice(ALCdevice *device)
{
   if(g_audioDevice != 0) {
      SDL_CloseAudioDevice(g_audioDevice);
      g_audioDevice = 0;
   }
   g_alcDevice = NULL;
   return ALC_TRUE;
}

ALCcontext* alcCreateContext(ALCdevice *device, const ALCint *attrlist)
{
   if(device == NULL)
      return NULL;
   
   g_alcContext = (ALCcontext *)0x87654321;
   return g_alcContext;
}

ALCboolean alcMakeContextCurrent(ALCcontext *context)
{
   if(context == g_alcContext || context == NULL)
      return ALC_TRUE;
   return ALC_FALSE;
}

ALCboolean alcDestroyContext(ALCcontext *context)
{
   if(context == g_alcContext) {
      g_alcContext = NULL;
   }
   return ALC_TRUE;
}

const ALCchar* alcGetString(ALCdevice *device, ALCenum param)
{
   switch(param) {
      case ALC_DEVICE_SPECIFIER:
         if(device == g_alcDevice) {
            return "SDL2 Audio Device";
         }
         return "SDL2 Audio Devices";
      case ALC_EXTENSIONS:
         return "";
      default:
         return "";
   }
}

ALCenum alcGetError(ALCdevice *device)
{
   return ALC_NO_ERROR;
}

