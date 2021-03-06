/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* This is a JNI example where we use native methods to play sounds
 * using OpenSL ES. See the corresponding Java source file located at:
 *
 *   src/com/example/nativeaudio/NativeAudio/NativeAudio.java
 */

#include <assert.h>
#include <jni.h>
#include <string.h>

// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
// #include <android/log.h>

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "MyLog.h"
#include "audio/audio.h"

#include "WavFile.h"
// pre-recorded sound clips, both are 8 kHz mono 16-bit signed little endian



// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static SLEffectSendItf bqPlayerEffectSend;
static SLMuteSoloItf bqPlayerMuteSolo;
static SLVolumeItf bqPlayerVolume;

// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
    SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;


// file descriptor player interfaces
static SLObjectItf fdPlayerObject = NULL;
static SLPlayItf fdPlayerPlay;
static SLSeekItf fdPlayerSeek;
static SLMuteSoloItf fdPlayerMuteSolo;
static SLVolumeItf fdPlayerVolume;

// recorder interfaces
static SLObjectItf recorderObject = NULL;
static SLRecordItf recorderRecord;
static SLAndroidSimpleBufferQueueItf recorderBufferQueue;

// synthesized sawtooth clip
#define SAWTOOTH_FRAMES 8000
static short sawtoothBuffer[SAWTOOTH_FRAMES];

// 5 seconds of recorded audio at 16 kHz mono, 16-bit signed little endian
#define RECORDER_FRAMES (16000 * 5)
static short recorderBuffer[RECORDER_FRAMES];
static unsigned recorderSize = 0;
static SLmilliHertz recorderSR;

// pointer and size of the next player buffer to enqueue, and number of remaining buffers
static short *nextBuffer;
static unsigned nextSize;
static int nextCount;

using namespace FancyTech;

// synthesize a mono sawtooth wave and place it into a buffer (called automatically on load)
__attribute__((constructor)) static void onDlOpen(void)
{
    unsigned i;
    for (i = 0; i < SAWTOOTH_FRAMES; ++i) {
        sawtoothBuffer[i] = 32768 - ((i % 100) * 660);
    }
}


// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);


// this callback handler is called every time a buffer finishes recording
void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(bq == recorderBufferQueue);
    assert(NULL == context);
    // for streaming recording, here we would call Enqueue to give recorder the next buffer to fill
    // but instead, this is a one-time buffer so we stop recording
    SLresult result;
    result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_STOPPED);
    if (SL_RESULT_SUCCESS == result) {
        recorderSize = RECORDER_FRAMES * sizeof(short);
        recorderSR = SL_SAMPLINGRATE_16;
    }
}


// create the engine and output mix objects
void Java_com_example_nativeaudio_NativeAudio_createEngine(/*JNIEnv* env, jclass clazz*/)
{
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
            &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void)result;
    }
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example

}


// create buffer queue audio player
void Java_com_example_nativeaudio_NativeAudio_createBufferQueueAudioPlayer(const PCMInfo& info)
{
    SLresult result;

    // configure audio source
	SLuint32 channelMask = (info.channels == 2) ?
		(SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT) : SL_SPEAKER_FRONT_CENTER;

    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 
		info.channels, info.samplesPerSec * 1000,
        info.bitsPerSample, info.containerSize,
		channelMask, SL_BYTEORDER_LITTLEENDIAN };
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
            3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
            &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the effect send interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
            &bqPlayerEffectSend);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

#if 0   // mute/solo is not supported for sources that are known to be mono, as this is
    // get the mute/solo interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_MUTESOLO, &bqPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
#endif

    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}

// expose the mute/solo APIs to Java for one of the 3 players

static SLMuteSoloItf getMuteSolo()
{
    if (fdPlayerMuteSolo != NULL)
        return fdPlayerMuteSolo;
    else
        return bqPlayerMuteSolo;
}



void Java_com_example_nativeaudio_NativeAudio_setChannelSoloUriAudioPlayer(JNIEnv* env,
        jclass clazz, jint chan, jboolean solo)
{
    SLresult result;
    SLMuteSoloItf muteSoloItf = getMuteSolo();
    if (NULL != muteSoloItf) {
        result = (*muteSoloItf)->SetChannelSolo(muteSoloItf, chan, solo);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    }
}

// expose the volume APIs to Java for one of the 3 players

static SLVolumeItf getVolume()
{
    if (fdPlayerVolume != NULL)
        return fdPlayerVolume;
    else
        return bqPlayerVolume;
}

// enable reverb on the buffer queue player
jboolean Java_com_example_nativeaudio_NativeAudio_enableReverb(JNIEnv* env, jclass clazz,
        jboolean enabled)
{
    SLresult result;

    // we might not have been able to add environmental reverb to the output mix
    if (NULL == outputMixEnvironmentalReverb) {
        return JNI_FALSE;
    }

    result = (*bqPlayerEffectSend)->EnableEffectSend(bqPlayerEffectSend,
            outputMixEnvironmentalReverb, (SLboolean) enabled, (SLmillibel) 0);
    // and even if environmental reverb was present, it might no longer be available
    if (SL_RESULT_SUCCESS != result) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

//
//// select the desired clip and play count, and enqueue the first buffer if idle
//jboolean Java_com_example_nativeaudio_NativeAudio_selectClip(JNIEnv* env, jclass clazz, jint which,
//        jint count)
//{
//    switch (which) {
//    case 0:     // CLIP_NONE
//        nextBuffer = (short *) NULL;
//        nextSize = 0;
//        break;
//    case 1:     // CLIP_HELLO
//        nextBuffer = (short *) hello;
//        nextSize = sizeof(hello);
//        break;
//    case 2:     // CLIP_ANDROID
//        nextBuffer = (short *) android;
//        nextSize = sizeof(android);
//        break;
//    case 3:     // CLIP_SAWTOOTH
//        nextBuffer = sawtoothBuffer;
//        nextSize = sizeof(sawtoothBuffer);
//        break;
//    case 4:     // CLIP_PLAYBACK
//        // we recorded at 16 kHz, but are playing buffers at 8 Khz, so do a primitive down-sample
//        if (recorderSR == SL_SAMPLINGRATE_16) {
//            unsigned i;
//            for (i = 0; i < recorderSize; i += 2 * sizeof(short)) {
//                recorderBuffer[i >> 2] = recorderBuffer[i >> 1];
//            }
//            recorderSR = SL_SAMPLINGRATE_8;
//            recorderSize >>= 1;
//        }
//        nextBuffer = recorderBuffer;
//        nextSize = recorderSize;
//        break;
//    default:
//        nextBuffer = NULL;
//        nextSize = 0;
//        break;
//    }
//    nextCount = count;
//    if (nextSize > 0) {
//        // here we only enqueue one buffer because it is a long clip,
//        // but for streaming playback we would typically enqueue at least 2 buffers to start
//        SLresult result;
//        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
//        if (SL_RESULT_SUCCESS != result) {
//            return JNI_FALSE;
//        }
//    }
//
//    return JNI_TRUE;
//}


// create asset audio player
jboolean Java_com_example_nativeaudio_NativeAudio_createAssetAudioPlayer(JNIEnv* env, jclass clazz,
        jobject assetManager, jstring filename)
{
    SLresult result;

    // convert Java string to UTF-8
    const char *utf8 = env->GetStringUTFChars(filename, NULL);
    assert(NULL != utf8);

    // use asset manager to open asset by filename
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    assert(NULL != mgr);
    AAsset* asset = AAssetManager_open(mgr, utf8, AASSET_MODE_UNKNOWN);

    // release the Java string and UTF-8
    env->ReleaseStringUTFChars(filename, utf8);

    // the asset might not be found
    if (NULL == asset) {
        return JNI_FALSE;
    }

    // open asset as file descriptor
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    assert(0 <= fd);
    AAsset_close(asset);

    // configure audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &fdPlayerObject, &audioSrc, &audioSnk,
            3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the player
    result = (*fdPlayerObject)->Realize(fdPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the play interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the seek interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_SEEK, &fdPlayerSeek);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the mute/solo interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_MUTESOLO, &fdPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the volume interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_VOLUME, &fdPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // enable whole file looping
    result = (*fdPlayerSeek)->SetLoop(fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    return JNI_TRUE;
}


// set the playing state for the asset audio player
void Java_com_example_nativeaudio_NativeAudio_setPlayingAssetAudioPlayer(JNIEnv* env,
        jclass clazz, jboolean isPlaying)
{
    SLresult result;

    // make sure the asset audio player was created
    if (NULL != fdPlayerPlay) {

        // set the player's state
        result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, isPlaying ?
            SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    }

}




// shut down the native audio system
void Java_com_example_nativeaudio_NativeAudio_shutdown(JNIEnv* env, jclass clazz)
{

    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
        bqPlayerMuteSolo = NULL;
        bqPlayerVolume = NULL;
    }

    // destroy file descriptor audio player object, and invalidate all associated interfaces
    if (fdPlayerObject != NULL) {
        (*fdPlayerObject)->Destroy(fdPlayerObject);
        fdPlayerObject = NULL;
        fdPlayerPlay = NULL;
        fdPlayerSeek = NULL;
        fdPlayerMuteSolo = NULL;
        fdPlayerVolume = NULL;
    }


    // destroy audio recorder object, and invalidate all associated interfaces
    if (recorderObject != NULL) {
        (*recorderObject)->Destroy(recorderObject);
        recorderObject = NULL;
        recorderRecord = NULL;
        recorderBufferQueue = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

}



WavFile wavFile;
const char*	playBufferPos = NULL;
const char* playBufferEnd = NULL;
int bufferSizeInByte_ = 0;

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	assert(bq == bqPlayerBufferQueue);
	assert(NULL == context);

	if (nextSize != 0)	{
		SLresult result;
		// enqueue another buffer
		result = (*bq)->Enqueue(bq, playBufferPos, nextSize);
		assert(SL_RESULT_SUCCESS == result);

		playBufferPos += nextSize;
		if ((playBufferEnd - playBufferPos) < nextSize) {
			nextSize = playBufferEnd - playBufferPos;
		}
	}

	//// for streaming playback, replace this test by logic to find and fill the next buffer
	//if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {
	//    SLresult result;
	//    // enqueue another buffer
	//    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
	//    // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
	//    // which for this code example would indicate a programming error
	//    assert(SL_RESULT_SUCCESS == result);
	//    (void)result;
	//}
}



bool AudioSystem::Init(const char* pathName/*, int sampleRate, int framesPerBuffer*/)
{
	if (!wavFile.Load(pathName)) {
		GLog.LogError("load wave file failed! %s", pathName);
		return false;
	}

	PCMInfo info;
	wavFile.GetPCMInfo(info);

	// framesPerBuffer * channels * bytePerChannelPerSample
	bufferSizeInByte_ = 200 * info.channels * (info.bitsPerSample / 8);

	Java_com_example_nativeaudio_NativeAudio_createEngine();
	Java_com_example_nativeaudio_NativeAudio_createBufferQueueAudioPlayer(info);

	return true;
}


void AudioSystem::Play()
{
	//Java_com_example_nativeaudio_NativeAudio_selectClip(NULL, NULL, 1, 1);

	if (wavFile.GetPCMData() != NULL && bqPlayerPlay != NULL) {
		SLresult result;
		// set the player's state to playing
		playBufferPos = (const char*)wavFile.GetPCMData();
		playBufferEnd = playBufferPos + wavFile.GetPCMDataLength();
		nextSize = bufferSizeInByte_;

		result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);

		bqPlayerCallback(bqPlayerBufferQueue, NULL);
	}
}

void AudioSystem::Pause()
{
	if (bqPlayerPlay != NULL) {
		SLresult result;
		result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);

	}
}


void AudioSystem::Stop()
{
	if (bqPlayerPlay != NULL) {
		SLresult result;
		result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
		assert(SL_RESULT_SUCCESS == result);
	}
}

uint32_t AudioSystem::GetPosition()
{
	if (bqPlayerPlay == NULL) {
		return 0;
	}
	SLmillisecond t;
	(*bqPlayerPlay)->GetPosition(bqPlayerPlay, &t);

	return t;
}

uint32_t AudioSystem::GetDuration()
{
	if (bqPlayerPlay == NULL) {
		return 0;
	}

	SLmillisecond t;
	(*bqPlayerPlay)->GetDuration(bqPlayerPlay, &t);

	return t;
}

AudioSystem		GAudioSystem;
