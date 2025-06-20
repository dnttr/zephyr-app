//
// Created by Damian Netter on 20/06/2025.
//

#include "ZAKit/executor.hpp"

#include <ZNBKit/jni/signatures/method/void_method.hpp>

#include "ZAKit/bridge.hpp"

namespace za_kit
{
    const std::string executor::executor_class_name = "org/dnttr/zephyr/management/Loader";
    const std::string executor::executor_method_name = "load";
    const std::string executor::executor_method_signature = "()V";

    const std::string executor::bridge_class_name = "org/dnttr/zephyr/bridge/internal/ZAKit";

    const std::unordered_multimap<std::string, znb_kit::jni_bridge_reference> executor::methods = {
        {"ffi_zm_push_shader", znb_kit::jni_bridge_reference(&bridge::push, { znb_kit::STRING, znb_kit::STRING })}
    };

    void executor::run(JNIEnv* jni, znb_kit::jvmti_object& jvmti)
    {
        const znb_kit::klass_signature loader_signature(jni, executor_class_name);
        const znb_kit::klass_signature bridge_signature(jni, bridge_class_name);

        znb_kit::void_method method(jni, loader_signature, executor_method_name, executor_method_signature, std::nullopt, true);

        auto [native_methods, size] = jvmti.try_mapping_methods<void>(bridge_signature, methods);

        znb_kit::wrapper::register_natives(jni, bridge_class_name,bridge_signature.get_owner(), native_methods);

        std::vector<jvalue> parameters;
        method.invoke(nullptr, parameters);
    }
}
