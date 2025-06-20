//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <jni.h>

#define DEFAULT JNIEnv *jni, jobject object

namespace zc_kit
{
    class bridge {
    public:

        class outbound
        {
            //TODO: look into obtaining config (properties) via ZNBKit, so I can use it in the future, without making a mess here
            //probably some kind of factory needed to reduce calls amount.
            static jint get_config(DEFAULT);
        };

        static jint push_shader(DEFAULT, jstring name, jstring source);
    };
}