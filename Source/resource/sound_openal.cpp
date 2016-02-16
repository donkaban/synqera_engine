#ifdef SOUND_OPENAL

#include "TheEngine/resource/sound.h"
#include "TheEngine/logger.h"
#include "TheEngine/filesystem/stream.h"


#include <iostream>
#include <fstream>
#include <cstring>

#include <AL/al.h>
#include <AL/alc.h>

#include <memory>
#include <string>

using namespace the;

struct WAVHEADER
{  
    char chunkId[4];                //  RIFF
    unsigned long chunkSize;        //  
    char format[4];                 //  WAVE
    char subchunk1[4];              // "fmt "
    unsigned long subchunk1Size;    //  16 for PCM
    unsigned short audioFormat;     //  PCM = 1
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char subchunk2Id[4];            //"data"
    unsigned long subchunk2Size;
};

static ALCdevice *device;
static ALCcontext *context;

void the::sound::init()
{
    logger::debug("[Sound  ] init OpenAL sound subsystem");
    device = alcOpenDevice(NULL);
    if(!device)
        logger::error("[Sound  ] Can't open sound card");
    context = alcCreateContext(device,NULL);
    alcMakeContextCurrent(context);
}

void the::sound::close()
{
    logger::debug("[Sound  ] destroy OpenAL sound subsystem...");
    alcDestroyContext(context);
    alcCloseDevice(device);
    logger::debug("[Sound  ] OpenAL sound subsystem destroyed");
}

struct data
{
    WAVHEADER   header;
    unsigned int bufferid;
    unsigned int format;
    unsigned int sourceid;
    char    *data;
    bool    isLooped=false;
    bool    isBigEndian();
    int     convertToInt(char *buffer, int len);
    bool    loadWAV(const std::string &file);
};
#define DATA (static_cast<data * >(this->_audio_data))

the::sound::sound(strref file)
{
    _audio_data = static_cast<void *> (new data());
    std::memset(DATA,0,sizeof(data));
    if(!device || !context)
        logger::error("[Sound  ] can't find OpenAL context");
    else
	   load(file);	
}

void the::sound::play(bool loop)
{
    if (DATA->sourceid)
    {
	   alSourcePlay(DATA->sourceid);		
	   if (loop)
	    	alSourcei(DATA->sourceid,AL_LOOPING,AL_TRUE);
	   else    	
	    	alSourcei(DATA->sourceid,AL_LOOPING,AL_FALSE);
        DATA->isLooped = loop;
    }	
}

bool the::sound::getLoop()
{
    return DATA->isLooped;
}

void the::sound::playLoop()
{
    play(true);
}

void the::sound::playUnLoop()
{
    play(false);
}

void the::sound::stop()
{
    if (DATA->sourceid)
    {
	   alSourceStop(DATA->sourceid);	
    }
}

void the::sound::pause()
{	
    if (DATA->sourceid)
    {
    	alSourcePause(DATA->sourceid);
    }
}

void the::sound::load(strref file)
{
	DATA->loadWAV(file);
	alGenBuffers(1,&DATA->bufferid);
    if(DATA->header.numChannels == 1)
    {
        if(DATA->header.bitsPerSample == 8)
            DATA->format=AL_FORMAT_MONO8;        
        else
            DATA->format=AL_FORMAT_MONO16;   
    }
    else
    {
        if(DATA->header.bitsPerSample == 8)        
            DATA->format=AL_FORMAT_STEREO8;
        else      
            DATA->format=AL_FORMAT_STEREO16;                         
    }
    alBufferData(DATA->bufferid, DATA->format,DATA->data,DATA->header.subchunk2Size, DATA->header.sampleRate);
    alGenSources(1,&DATA->sourceid);

    alSourcei(DATA->sourceid,AL_BUFFER,DATA->bufferid);
}

bool the::sound::isPlaying()  
{
    if (DATA->sourceid)
    {
	   ALint state;
	   alGetSourcei(DATA->sourceid, AL_SOURCE_STATE, &state);
	   if (state == AL_PLAYING)
	   {
		   return true;
       }
    }
    return false;
}

bool the::sound::isPaused()  
{
    if (DATA->sourceid)
    {
	   ALint state;
	   alGetSourcei(DATA->sourceid, AL_SOURCE_STATE, &state);
	   if (state == AL_PAUSED)	
		   return true;
    }
    return false;
}

bool the::sound::isStopped() 
{
    if (DATA->sourceid)
    {
	   ALint state;
	   alGetSourcei(DATA->sourceid, AL_SOURCE_STATE, &state);
	   if (state == AL_STOPPED)	
		   return true;	
    }
	return false;
}

the::sound::~sound()
{
    if (DATA->sourceid)
    {
	    alSourceStop(DATA->sourceid);
    	alDeleteSources(1,&DATA->sourceid);
        alDeleteBuffers(1,&DATA->bufferid);
    }
    if (DATA->data != 0)
        delete[] DATA->data;
}

bool data::isBigEndian()
{
	int a = 1;
    return !((char*)&a)[0];
}

int data::convertToInt(char* buffer, int len)
{
	int a=0;
    if(!isBigEndian())
        for(int i=0;i<len;i++)
                    ((char*)&a)[i]=buffer[i];
    else
        for(int i=0;i<len;i++)
            ((char*)&a)[3-i]=buffer[i];    
    return a;
}

bool data::loadWAV(strref filename)
{   
    the::filesystem::stream stream(filename);
    if(!the::filesystem::is_file(filename))
    {
        logger::error("[Sound  ] wav file '%s' not exist", filename.c_str());
        return false;
    }    
    stream.read(reinterpret_cast<char*>(&header), sizeof(header));

    if( (strncmp(header.chunkId, "RIFF", 4) != 0)   ||
        (strncmp(header.format,"WAVE",4) !=0)       ||
        (strncmp(header.subchunk1,"fmt ",4)!=0)     ||
        (header.subchunk1Size != 16)                ||
        (header.audioFormat != 1)                   ||
        (strncmp(header.subchunk2Id, "data", 4) != 0))
    {
       logger::error("[Sound  ] invalid wav file '%s' format ", filename.c_str());
       return false;           
    }
    data = new char[header.subchunk2Size];
    stream.read(data,header.subchunk2Size);
    return true;
}

void the::sound::setGain(float v)
{
    if (v > 1.0f)
        v = 1.0f;
    else if (v < 0.0f)
        v = 0.0f;    
    alSourcef(DATA->sourceid, AL_GAIN, v);    
}

float the::sound::getGain()
{
    float tmp;
    alGetSourcef(DATA->sourceid, AL_GAIN, &tmp);
    return tmp;
}

#undef DATA

#endif