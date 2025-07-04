//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <ZNBKit/jvmti/jvmti_object.hpp>

namespace zc_kit
{
    class app_runner
    {
        static const std::string executor_klass_name;
        static const std::string executor_method_name;
        static const std::string executor_method_signature;

        static void submit(JNIEnv *jni, znb_kit::jvmti_object jvmti);
        static void invoke(JNIEnv *jni, std::vector<std::string> &native_libraries);

    public:
        static std::mutex mtx;
        static std::condition_variable cv;

        static bool ready;

        static void run(const std::vector<std::string> &paths, std::vector<std::string> &native_libraries);
    };
}
