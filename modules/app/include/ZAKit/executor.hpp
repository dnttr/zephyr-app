//
// Created by Damian Netter on 20/06/2025.
//

#pragma once
#include <jni.h>
#include <ZNBKit/jvmti/jvmti_object.hpp>

namespace za_kit
{
    class executor
    {
    public:
        static void run(JNIEnv *jni, znb_kit::jvmti_object &jvmti);
    };
}
