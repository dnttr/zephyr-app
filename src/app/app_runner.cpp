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

    const std::string app_runner::executor_klass_name = "org/dnttr/zephyr/management/Loader";

    const std::string app_runner::executor_method_name = "load";
    const std::string app_runner::executor_method_signature = "()V";

    void app_runner::run(const std::vector<std::string> &paths)
    {
        const auto vm_object = znb_kit::vm_management::create_and_wrap_vm(paths);

        bridge::initialize_bridge(vm_object.get());
        const auto jni = bridge::vm_obj->get_env();

        znb_kit::jvmti_object jvmti(jni, bridge::vm_obj->get_jvmti()->get().get_owner());

        submit(jni, std::move(jvmti));
        invoke(jni);

        zcg_kit::display::config display_cfg;

        display_cfg.window_width = 1920;
        display_cfg.window_height = 1080;
        display_cfg.virtual_width = 1920.0F;
        display_cfg.virtual_height = 1080.0F;

        zc_app::window window;

        window.allocate("Core Graphics", 0, 0, display_cfg);

        std::unique_lock lock(mtx);
        cv.wait(lock, [] { return ready; });

        bridge::invoke_connect();
        window.run();

        znb_kit::vm_management::cleanup_vm(bridge::vm_obj->get_owner());
    }

    void app_runner::submit(JNIEnv *jni, znb_kit::jvmti_object jvmti)
    {
        const auto [methods, size] = jvmti.try_mapping_methods<void>(*bridge::bridge_signature, bridge::mapped_methods);

        if (size == 0)
        {
            throw std::runtime_error("No methods found for the bridge class: " + bridge::bridge_klass_name);
        }

        znb_kit::wrapper::register_natives(jni, bridge::bridge_klass_name, (*bridge::bridge_signature).get_owner(), methods);
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
