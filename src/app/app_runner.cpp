//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCApp/app/app_runner.hpp"

#include <ZNBKit/jni/signatures/method/void_method.hpp>
#include <ZNBKit/vm/vm_management.hpp>

#include "ZCApp/graphics/window.hpp"
#include "ZCKit/bridge.hpp"

namespace zc_kit
{
    bool app_runner::ready = false;

    std::mutex app_runner::mtx;
    std::condition_variable app_runner::cv;

    const std::string app_runner::bridge_klass_name = "org/dnttr/zephyr/bridge/internal/ZAKit";
    const std::string app_runner::executor_klass_name = "org/dnttr/zephyr/management/Loader";

    const std::string app_runner::executor_method_name = "load";
    const std::string app_runner::executor_method_signature = "()V";

    const std::unordered_multimap<std::string, znb_kit::jni_bridge_reference> app_runner::mapped_methods = {
        {"ffi_zm_push_shader", znb_kit::jni_bridge_reference(&bridge::push_shader, {znb_kit::STRING, znb_kit::STRING})},
        {"ffi_zm_finish_loading", znb_kit::jni_bridge_reference(&bridge::finish_loading)}
    };

    void app_runner::run(const std::string &vm_path)
    {
        const auto vm_object = znb_kit::vm_management::create_and_wrap_vm(vm_path);
        const auto vm = vm_object.get();

        VAR_CHECK(vm);

        const auto jni = vm->get_env();
        znb_kit::jvmti_object jvmti(jni, vm->get_jvmti()->get().get_owner());

        submit(jni, std::move(jvmti));
        invoke(jni);

        display::config display_cfg;

        display_cfg.window_width = 1920;
        display_cfg.window_height = 1080;
        display_cfg.virtual_width = 1920.0F;
        display_cfg.virtual_height = 1080.0F;

        zc_app::window window;

        window.allocate("Core Graphics", 0, 0, display_cfg);

        std::unique_lock lock(mtx);

        cv.wait(lock, [] { return ready; });

        window.run();
    }

    void app_runner::submit(JNIEnv *jni, znb_kit::jvmti_object jvmti)
    {
        const znb_kit::klass_signature bridge_signature(jni, bridge_klass_name);
        const auto [methods, size] = jvmti.try_mapping_methods<void>(bridge_signature, mapped_methods);

        znb_kit::wrapper::register_natives(jni, bridge_klass_name, bridge_signature.get_owner(), methods);
    }

    void app_runner::invoke(JNIEnv *jni)
    {
        const znb_kit::klass_signature loader_signature(jni, executor_klass_name);

        znb_kit::void_method loadMethod(jni, loader_signature, executor_method_name, executor_method_signature,
                                        std::nullopt, true);

        std::vector<jvalue> parameters;
        loadMethod.invoke(nullptr, parameters);
    }
}
