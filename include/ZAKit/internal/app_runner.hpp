//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <ZNBKit/jvmti/jvmti_object.hpp>

namespace za_kit
{
    class app_runner
    {
        static const std::unordered_multimap<std::string, znb_kit::jni_bridge_reference> methods;

        static const std::string executor_klass_name;
        static const std::string executor_method_name;
        static const std::string executor_method_signature;

        static const std::string bridge_klass_name;
    public:
        static void run(const std::string& vm_path);
    };
}