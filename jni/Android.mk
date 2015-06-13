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
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../code

LOCAL_MODULE    := native-activity
#VisualGDBAndroid: AutoUpdateSourcesInNextLine
LOCAL_SRC_FILES := ../code/android/Sensor.cpp ../code/android/Thread.cpp ../code/android/TrackDropping.cpp ../code/android/draw.cpp ../code/android/main.cpp ../code/android/MyLog.cpp ../code/android/PlatfromAndroid.cpp ../code/AurMath.cpp ../code/CommandSystem.cpp ../code/FileSystem.cpp ../code/GlobalVar.cpp ../code/glUtil.cpp ../code/Model.cpp ../code/ShaderManager.cpp ../code/Texture.cpp ../code/thirdParty/stb/stb_image.c ../code/Timer.cpp
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2
LOCAL_STATIC_LIBRARIES := android_native_app_glue


include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
