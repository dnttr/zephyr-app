//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <jni.h>

#define DEFAULT JNIEnv *jni, jobject object

namespace zc_kit
{
    class bridge
    {
    public:
        static jint push_shader(DEFAULT, jstring name, jstring source);

        static jint finish_loading(DEFAULT); //btw, DEFAULT isn't needed at this point; after all, no JNI parameters are needed, therefore, it's not a requirement, however, for now I'd like to keep it for consistency and future-proofing

        static jint push_texture(DEFAULT, jstring name, jobject direct_buffer, jint width, jint height);
    };
}
