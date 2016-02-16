#ifdef SOUND_OPENSL_ES_ANDROID

#include "TheEngine/resource/sound.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "TheEngine/logger.h"

using namespace the;

#define DATA (static_cast<data * >(this->_audio_data))

void the::sound::init()
{
    logger::debug("[Sound  ] init OpenSLES sound subsystem");
}

void the::sound::close()
{
    logger::debug("[Sound  ] close OpenSLES sound subsystem");


}
struct WAVHeader
{
    char                RIFF[4];        
    unsigned long       ChunkSize;      
    char                WAVE[4];        
    char                fmt[4];         
    unsigned long       Subchunk1Size;
    unsigned short      AudioFormat;    
    unsigned short      NumOfChan;      
    unsigned long       SamplesPerSec;  
    unsigned long       bytesPerSec;  
    unsigned short      blockAlign;     
    unsigned short      bitsPerSample;  
    char                Subchunk2ID[4]; 
    unsigned long       Subchunk2Size;  
};

struct data
{
    SLObjectItf outputMixObj;
    SLObjectItf playerObj;

    SLPlayItf playerInterface;
    SLVolumeItf volumeInterface;
    SLBufferQueueItf bufferQueue;

    SLmillibel maxVolumeLevel = 0;
    SLmillibel minVolumeLevel = -9600;

    WAVHeader header;

    char *data;
    bool loopPlayed=  false;

    static void callback(SLBufferQueueItf caller, void *pContext)
    {
        auto *s = (the::sound*)pContext;
        s->stop();
        if(s->getLoop())
            s->play(true);
    }
};



class AndroidSLESManager
{
private:
    SLObjectItf engineObj;
    SLEngineItf engine;

    static AndroidSLESManager *instance;

    AndroidSLESManager();
public:

    static void init();
    static void finalize();
    static AndroidSLESManager* getInstance();

    SLEngineItf& getEngine() {return engine;}
    SLObjectItf& getEngineObject() {return engineObj;}
    virtual ~AndroidSLESManager(){}
};

AndroidSLESManager* AndroidSLESManager::instance;

AndroidSLESManager::AndroidSLESManager()
{
    const SLInterfaceID pIDs[1] = {SL_IID_ENGINE};
    const SLboolean pIDsRequired[1]  = {SL_BOOLEAN_TRUE};

    SLresult result = slCreateEngine(&engineObj,0,0,1,pIDs,pIDsRequired);
    if(result != SL_RESULT_SUCCESS){
        FATAL_ERROR("[Sound  ] SLES initialization failed");
    }

    result = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        FATAL_ERROR("[Sound  ] Error after Realize engineObj");
    }

    result = (*engineObj)->GetInterface(engineObj,SL_IID_ENGINE,&engine);
}

void AndroidSLESManager::init()
{
    if(instance)
        return;

    instance = new AndroidSLESManager();
}

void AndroidSLESManager::finalize()
{
    if(!instance)
        return;

    SLObjectItf engineObj = instance->getEngineObject();
    (*engineObj)->Destroy(engineObj);

    delete instance;
    instance = nullptr;
}

AndroidSLESManager* AndroidSLESManager::getInstance()
{
    AndroidSLESManager::init();
    return instance;
}

the::sound::sound(strref filename)
{
    if(!the::filesystem::is_file(filename))
        FATAL_ERROR("[Sound  ] cannot open wav %s",filename.c_str());
    _audio_data = static_cast<void *> (new data());

    SLEngineItf engine = AndroidSLESManager::getInstance()->getEngine();

    const SLInterfaceID pOutputMixIDs[] = {};
    const SLboolean pOutputMixRequired[] = {};

    SLresult result = (*engine)->CreateOutputMix(engine, &DATA->outputMixObj, 0, pOutputMixIDs, pOutputMixRequired);
    result = (*DATA->outputMixObj)->Realize(DATA->outputMixObj, SL_BOOLEAN_FALSE);

    SLDataLocator_AndroidSimpleBufferQueue locatorBufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1};

    the::filesystem::stream stream(filename);
    stream.read(reinterpret_cast<char*>(&DATA->header), sizeof(DATA->header));

    DATA->data = new char[DATA->header.Subchunk2Size];
    stream.read(DATA->data,DATA->header.Subchunk2Size);

    SLuint32 chanelMask = DATA->header.NumOfChan==1 ? SL_SPEAKER_FRONT_CENTER : SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;

    SLDataFormat_PCM formatPCM = 
    {
        SL_DATAFORMAT_PCM,  
        DATA->header.NumOfChan, 
        static_cast<SLuint32>(DATA->header.SamplesPerSec * 1000),
        DATA->header.bitsPerSample, 
        DATA->header.bitsPerSample,
        chanelMask, 
        SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource audioSrc = {&locatorBufferQueue, &formatPCM};
    SLDataLocator_OutputMix locatorOutMix = {SL_DATALOCATOR_OUTPUTMIX, DATA->outputMixObj};
    SLDataSink audioSnk = {&locatorOutMix, NULL};
    const SLInterfaceID pIDs[2] = {SL_IID_BUFFERQUEUE,SL_IID_VOLUME};
    const SLboolean pIDsRequired[2] = {SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE};
    result = (*engine)->CreateAudioPlayer(engine, &DATA->playerObj, &audioSrc, &audioSnk, 2, pIDs, pIDsRequired);
    if(result != SL_RESULT_SUCCESS)
    {
        logger::error("[Sound  ] can't CreateAudioPlayer with error %d",result);
    }
    result = (*DATA->playerObj)->Realize(DATA->playerObj, SL_BOOLEAN_FALSE);

    result = (*DATA->playerObj)->GetInterface(DATA->playerObj, SL_IID_PLAY, &DATA->playerInterface);
    result = (*DATA->playerObj)->GetInterface(DATA->playerObj, SL_IID_VOLUME, &DATA->volumeInterface);
    result = (*DATA->playerObj)->GetInterface(DATA->playerObj, SL_IID_BUFFERQUEUE, &DATA->bufferQueue);

    (*DATA->volumeInterface)->GetMaxVolumeLevel(DATA->volumeInterface,&DATA->maxVolumeLevel);
    result = (*DATA->bufferQueue)->RegisterCallback(DATA->bufferQueue, DATA->callback, this);
    if(result != SL_RESULT_SUCCESS)
    {
        logger::error("[Sound  ] can't set playpack finished callback with error %d",result);
    }
}

//void the::sound::callback(SLBufferQueueItf caller, void *pContext)


void the::sound::play(bool loop)
{
    DATA->loopPlayed = loop;
    SLuint32 state;
    (*DATA->playerInterface)->GetPlayState(DATA->playerInterface, &state);
    if(state==SL_PLAYSTATE_STOPPED)
    {
        (*DATA->playerInterface)->SetPlayState(DATA->playerInterface, SL_PLAYSTATE_PLAYING);
        (*DATA->bufferQueue)->Clear(DATA->bufferQueue);
        (*DATA->bufferQueue)->Enqueue(DATA->bufferQueue, DATA->data, DATA->header.Subchunk2Size);
    }
    if(state==SL_PLAYSTATE_PAUSED)
    {
        (*DATA->playerInterface)->SetPlayState(DATA->playerInterface, SL_PLAYSTATE_PLAYING);
    }
}

void the::sound::setGain(float v)
{
    SLmillibel volumeLevel = DATA->minVolumeLevel + v*(DATA->maxVolumeLevel - DATA->minVolumeLevel);
    SLresult result = (*DATA->volumeInterface)->SetVolumeLevel(DATA->volumeInterface,volumeLevel);
    if(result != SL_RESULT_SUCCESS)
    {
        logger::error("[Sound  ] Cannot set volume with value: %d",volumeLevel);
    }
}

void the::sound::stop()
{
    (*DATA->playerInterface)->SetPlayState(DATA->playerInterface, SL_PLAYSTATE_STOPPED);
    (*DATA->bufferQueue)->Clear(DATA->bufferQueue);
}

void the::sound::pause()
{
    (*DATA->playerInterface)->SetPlayState(DATA->playerInterface, SL_PLAYSTATE_PAUSED); 
}

float the::sound::getGain()
{
    SLmillibel vol;
    (*DATA->volumeInterface)->GetVolumeLevel(DATA->volumeInterface,&vol);
    return vol/(DATA->maxVolumeLevel-DATA->minVolumeLevel);
}

bool the::sound::isPlaying()
{
    SLuint32 state;
    (*DATA->playerInterface)->GetPlayState(DATA->playerInterface, &state);
    return state==SL_PLAYSTATE_PLAYING;
}

bool the::sound::isPaused()
{
    SLuint32 state;
    (*DATA->playerInterface)->GetPlayState(DATA->playerInterface, &state);
    return state==SL_PLAYSTATE_PAUSED;
}

bool the::sound::isStopped()
{
    SLuint32 state;
    (*DATA->playerInterface)->GetPlayState(DATA->playerInterface, &state);
    return state==SL_PLAYSTATE_STOPPED;
}

the::sound::~sound()
{
    (*DATA->playerObj)->Destroy(DATA->playerObj);
    (*DATA->outputMixObj)->Destroy(DATA->outputMixObj);
    delete[] DATA->data;
    delete DATA;
}

bool the::sound::getLoop()
{
    return DATA->loopPlayed;
}

void the::sound::playUnLoop()
{
    play(true);
}

void the::sound::playLoop()
{
    play(false);
}


#undef DATA

#endif