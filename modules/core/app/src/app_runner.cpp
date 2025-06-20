//
// Created by Damian Netter on 20/06/2025.
//

#include "ZAKit/app_runner.hpp"

#include <ZNBKit/vm_management.hpp>
#include <ZNBKit/jni/signatures/method/void_method.hpp>

#include "ZAKit/bridge.hpp"

namespace za_kit
{
    const std::string app_runner::bridge_klass_name = "org/dnttr/zephyr/bridge/internal/ZAKit";
    const std::string app_runner::executor_klass_name = "org/dnttr/zephyr/management/Loader";

    const std::string app_runner::executor_method_name = "load";
    const std::string app_runner::executor_method_signature = "()V";

    const std::unordered_multimap<std::string, znb_kit::jni_bridge_reference> app_runner::methods = {
        {"ffi_zm_push_shader", znb_kit::jni_bridge_reference(&bridge::push_shader, { znb_kit::STRING, znb_kit::STRING })}
    };

    int app_runner::run(const std::string& vm_path)
    {
        const auto vm_object = znb_kit::vm_management::create_and_wrap_vm(vm_path);
        const auto vm = vm_object.get();

        if (!vm)
        {
            std::cerr << "Failed to create VM object." << std::endl;
            return 1;
        }

        const auto jni = vm->get_env();

        znb_kit::jvmti_object jvmti(jni, vm->get_jvmti()->get().get_owner());

        const znb_kit::klass_signature bridge_signature(jni, bridge_klass_name);
        const znb_kit::klass_signature loader_signature(jni, executor_klass_name);

        znb_kit::void_method method(jni, loader_signature, executor_method_name, executor_method_signature, std::nullopt, true);

        auto [native_methods, size] = jvmti.try_mapping_methods<void>(bridge_signature, methods);

        znb_kit::wrapper::register_natives(jni, bridge_klass_name,bridge_signature.get_owner(), native_methods);

        std::vector<jvalue> parameters;
        method.invoke(nullptr, parameters);

        return 0;
    }
}
