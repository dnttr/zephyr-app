//
// Created by Damian Netter on 20/06/2025.
//

#include "ZAKit/executor.hpp"

#include <ZNBKit/jni/signatures/method/void_method.hpp>

namespace za_kit
{
    void executor::run(JNIEnv* jni, znb_kit::jvmti_object& jvmti)
    {
        const znb_kit::klass_signature loader(jni, "org/dnttr/zephyr/management/Loader");
        znb_kit::void_method method(jni, loader, "load", "()V", std::nullopt, true);
    }
}
