//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <jni.h>

#define DEFAULT JNIEnv *jni, jobject object

namespace za_kit
{
    class bridge {
    public:

        static jint push_shader(DEFAULT, jstring name, jstring source);
    };
}