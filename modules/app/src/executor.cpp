//
// Created by Damian Netter on 20/06/2025.
//

#include "ZAKit/executor.hpp"

#include <ZNBKit/jni/signatures/method/void_method.hpp>

#include "ZAKit/bridge.hpp"

namespace za_kit
{
    const std::unordered_multimap<std::string, znb_kit::jni_bridge_reference> executor::methods = {
        {"ffi_zm_push_shader", znb_kit::jni_bridge_reference(&bridge::push, {"String, String"})}
    };

    void executor::run(JNIEnv* jni, znb_kit::jvmti_object& jvmti)
    {
        const znb_kit::klass_signature loader(jni, "org/dnttr/zephyr/management/Loader");
        znb_kit::void_method method(jni, loader, "load", "()V", std::nullopt, true);
    }
}
