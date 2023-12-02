export ANDROID_HOME=/opt/android-sdk/
export PATH=$PATH:${ANDROID_HOME}/tools
export PATH=$PATH:${ANDROID_HOME}/platform-tools
export PATH=$PATH:${ANDROID_HOME}/ndk/21.1.6352462
patrace/project/android/gradlew  --build-file patrace/project/android/eglretrace/build.gradle assembleRelease --stacktrace