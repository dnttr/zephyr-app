//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCKit/bridge.hpp"

#include <ZNBKit/internal/util.hpp>

#include "ZCApp/managers/shaders_manager.hpp"

jint zc_kit::bridge::push_shader(JNIEnv* jni, [[maybe_unused]] jobject, const jstring name, const jstring source)
{
    const auto name_str = znb_kit::get_string(jni, name, true);
    const auto source_str = znb_kit::get_string(jni, source, true);

    zc_app::shaders_manager::add_shader(name_str, source_str);

    return 0;
}