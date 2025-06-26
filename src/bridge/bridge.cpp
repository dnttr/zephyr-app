//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCKit/bridge.hpp"

#include <ZNBKit/internal/util.hpp>

#include "ZCApp/app/app_runner.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/textures/texture_loader.hpp"

namespace zc_kit
{
    const std::unordered_multimap<std::string, znb_kit::jni_bridge_reference> bridge::mapped_methods = {
        {"ffi_zm_push_shader", znb_kit::jni_bridge_reference(&bridge::push_shader, {znb_kit::STRING, znb_kit::STRING})},
        {"ffi_zm_finish_loading", znb_kit::jni_bridge_reference(&bridge::finish_loading)},
        {"ffi_zm_push_texture", znb_kit::jni_bridge_reference(&bridge::push_texture, {"java.lang.String", "java.nio.ByteBuffer", "int", "int"})}, //its deprecated however for now has to do, since i would have to refactor ZNB, which would be a waste of time for now
        {"ffi_zm_push_font", znb_kit::jni_bridge_reference(&bridge::push_font, {znb_kit::STRING, znb_kit::BYTE_ARRAY})}
    };

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

    jint bridge::push_texture(JNIEnv *jni, [[maybe_unused]] jobject, const jstring name, const jobject direct_buffer, const jint width, const jint height)
    {
        const auto buffer = znb_kit::wrapper::get_direct_buffer(jni, direct_buffer);
        const zc_app::texture_loader::texture_info info{buffer, width, height};

        zc_app::texture_loader::push(znb_kit::get_string(jni, name, true), info);

        return 0;
    }

    jint bridge::push_font(JNIEnv* jni, jobject object, jstring name, jbyteArray bytes)
    {

    }
}
