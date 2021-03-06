# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH_COPY:= $(call my-dir)
include $(CLEAR_VARS)

include $(LOCAL_PATH_COPY)/../code/thirdParty/libusb/android/libusb.mk

LOCAL_PATH := $(LOCAL_PATH_COPY)
include $(CLEAR_VARS)

RENDER_TO_FRONTBUFFER := 0

ifeq ($(RENDER_TO_FRONTBUFFER), 1)
	LOCAL_MODULE := simple-prebuilt
	LOCAL_SRC_FILES := libsimplejni.so
	include $(PREBUILT_SHARED_LIBRARY)
	include $(CLEAR_VARS)
endif

LOCAL_C_INCLUDES += \
$(LOCAL_PATH)/../code \
$(LOCAL_PATH)/../code/thirdParty \
$(LOCAL_PATH)/../code/thirdParty/libusb/src

#macro for minzip
LOCAL_CFLAGS := -DUSE_FILE32API -DRAPIDJSON_HAS_CXX11_RVALUE_REFS=0 

LOCAL_CPPFLAGS := -std=c++11


LOCAL_MODULE    := aurora-native
#VisualGDBAndroid: AutoUpdateSourcesInNextLine
LOCAL_SRC_FILES := ../code/android/ClientAndroid.cpp ../code/android/CompressedTexture.cpp ../code/android/device.cpp ../code/android/InputAndroid.cpp ../code/android/jniMain.cpp ../code/android/MessageQueue.cpp ../code/android/Sensor.cpp ../code/android/TextureETC.cpp ../code/android/Thread.cpp ../code/android/TrackDropping.cpp ../code/android/MyLog.cpp ../code/android/PlatfromAndroid.cpp ../code/android/UsbDevice.cpp ../code/audio/audio.cpp ../code/audio/WavFile.cpp ../code/AurMath.cpp ../code/BitmapFont.cpp ../code/Calibration.cpp ../code/CommandSystem.cpp ../code/FileSystem.cpp ../code/GlobalVar.cpp ../code/glUtil.cpp ../code/Input.cpp ../code/Model.cpp ../code/Rendering.cpp ../code/ShaderManager.cpp ../code/SpriteBatch.cpp ../code/Texture.cpp ../code/thirdParty/etc1/etc1.cpp ../code/thirdParty/stb/stb_image.c ../code/thirdParty/zip/ioapi.c ../code/thirdParty/zip/unzip.c ../code/Timer.cpp

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -lz -lOpenSLES

ifeq ($(RENDER_TO_FRONTBUFFER), 1)
	LOCAL_SHARED_LIBRARIES := simple-prebuilt
	LOCAL_CFLAGS += -DUSE_FRONT_BUFFER
endif


LOCAL_STATIC_LIBRARIES := libusb1.0

include $(BUILD_SHARED_LIBRARY)
