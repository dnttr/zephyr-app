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
        static const std::unordered_multimap<std::string, znb_kit::jni_bridge_reference> methods;

        static const std::string executor_class_name;
        static const std::string executor_method_name;
        static const std::string executor_method_signature;

        static const std::string bridge_class_name;
    public:
        static void run(JNIEnv *jni, znb_kit::jvmti_object &jvmti);
    };
}
