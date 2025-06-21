//
// Created by Damian Netter on 20/06/2025.
//

#include "ZAKit/bridge.hpp"

#include <ZNBKit/internal/util.hpp>
#include <ZNBKit/jni/signatures/method_signature.hpp>

#include "ZNBKit/debug.hpp"

jint za_kit::bridge::push_shader(JNIEnv* jni, [[maybe_unused]] jobject, const jstring name, const jstring source)
{
    const auto name_str = znb_kit::get_string(jni, name, true);
    const auto source_str = znb_kit::get_string(jni, source, true);

    debug_print(name_str);
    debug_print(source_str);

    return 0;
}
