//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCKit/bridge.hpp"

#include <ZNBKit/internal/util.hpp>

#include "ZCApp/app/app_runner.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/textures/texture_loader.hpp"
#include <ZNBKit/jni/buffer.hpp>
#include <ZNBKit/jni/signatures/method/void_method.hpp>
#include <ZNBKit/vm/vm_management.hpp>
#include <jni.h>
#include <thread>
#include <ZNBKit/vm/vm_object.hpp>

#include "ZCApp/graphics/fonts/font_loader.hpp"

namespace zc_kit
{
    const std::string bridge::bridge_klass_name = "org/dnttr/zephyr/bridge/internal/ZAKit";
    std::unique_ptr<znb_kit::klass_signature> bridge::bridge_signature;
    znb_kit::vm_object *bridge::vm_obj = nullptr;

    const std::unordered_multimap<std::string, znb_kit::jni_bridge_reference> bridge::mapped_methods = {
        {"ffi_zm_push_shader", znb_kit::jni_bridge_reference(&bridge::push_shader, {znb_kit::STRING, znb_kit::STRING})},
        {"ffi_zm_finish_loading", znb_kit::jni_bridge_reference(&bridge::finish_loading)},
        {
            "ffi_zm_push_texture",
            znb_kit::jni_bridge_reference(&bridge::push_texture, {
                                              "java.lang.String", "java.nio.ByteBuffer", "int", "int"
                                          })
        },
        //its deprecated however for now has to do, since i would have to refactor ZNB, which would be a waste of time for now
        {"ffi_zm_push_font", znb_kit::jni_bridge_reference(&bridge::push_font, {znb_kit::STRING, znb_kit::BYTE_ARRAY})}
    };

    void bridge::initialize_bridge(znb_kit::vm_object *_vm_obj)
    {
        if (_vm_obj)
        {
            vm_obj = _vm_obj;
        }

        znb_kit::klass_signature bridge_klass = {vm_obj->get_env(), bridge_klass_name};
        bridge_signature = std::make_unique<znb_kit::klass_signature>(std::move(bridge_klass));

    }

    jint bridge::push_shader(JNIEnv *jni, [[maybe_unused]] jobject, const jstring name, const jstring source)
    {
        const auto name_str = znb_kit::get_string(jni, name, true);
        const auto source_str = znb_kit::get_string(jni, source, true);

        zc_app::shaders::manager::add_shader(name_str, source_str);

        return 0;
    }

    jint bridge::finish_loading([[maybe_unused]] JNIEnv *, [[maybe_unused]] jobject)
    {
        std::lock_guard lock(app_runner::mtx);
        app_runner::ready = true;
        app_runner::cv.notify_all();

        return 0;
    }

    jint bridge::push_texture(JNIEnv *jni, [[maybe_unused]] jobject, const jstring name, const jobject direct_buffer,
                              const jint width, const jint height)
    {
        const auto buffer = znb_kit::wrapper::get_direct_buffer(jni, direct_buffer);
        const zc_app::texture_loader::texture_info info{buffer, width, height};

        zc_app::texture_loader::push(znb_kit::get_string(jni, name, true), info);

        return 0;
    }

    jint bridge::push_font(JNIEnv *jni, [[maybe_unused]] jobject, const jstring name, const jbyteArray bytes)
    {
        const auto name_str = znb_kit::get_string(jni, name, true);
        const int size = jni->GetArrayLength(bytes);

        std::vector<int8_t> buffer(size);
        znb_kit::buffer::get_ptr_byte(jni, bytes, buffer.data(), size, 0);

        zc_app::font_loader::push_font(name_str, buffer);

        return 0;
    }

    void bridge::invoke_connect()
    {
        znb_kit::void_method connect_method(vm_obj->get_env(), *bridge_signature, "connect", "()V", std::nullopt, true);
        std::vector<jvalue> parameters;

        connect_method.invoke(nullptr, parameters);
    }

    void bridge::run(const znb_kit::klass_signature &signature,
                                                std::vector<std::string> libraries_to_load) {
        try {
            load_native(signature, libraries_to_load);
            invoke_connect();
        } catch (const std::exception& e)
        {
            std::cerr << "Exception in async bridge operations: " << e.what() << std::endl;
        }
    }

    void bridge::terminate()
    {
        fflush(stdout);
        exit(0);
    }

    void bridge::load_native(const znb_kit::klass_signature &signature, std::vector<std::string> libraries)
    {
        for (const auto &library : libraries)
        {
            std::vector<std::string> signature_parameters;
            signature_parameters.emplace_back("java.lang.String");

            std::cout << "Loading native library: " << library << std::endl;
            auto str = znb_kit::wrapper::get_string(vm_obj->get_env(), library);

            znb_kit::void_method load_method(vm_obj->get_env(), signature, "loadNative", "(Ljava/lang/String;)V", { signature_parameters }, true);
            std::vector<jvalue> parameters;
            jvalue val;
            val.l = str;
            parameters.emplace_back(val);
            load_method.invoke(nullptr, parameters);
            znb_kit::wrapper::remove_string(vm_obj->get_env(), str);
        }
    }
}
