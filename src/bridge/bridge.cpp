//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCKit/bridge.hpp"

#include <ZNBKit/internal/util.hpp>

#include "ZCApp/app/app_runner.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"

jint zc_kit::bridge::push_shader(JNIEnv *jni, [[maybe_unused]] jobject, const jstring name, const jstring source)
{
    const auto name_str = znb_kit::get_string(jni, name, true);
    const auto source_str = znb_kit::get_string(jni, source, true);

    zc_app::shaders::manager::add_shader(name_str, source_str);

    return 0;
}

jint zc_kit::bridge::finish_loading([[maybe_unused]] JNIEnv *, [[maybe_unused]] jobject)
{
    std::cout << "Finished loading" << std::endl;
    std::lock_guard lock(app_runner::mtx);
    app_runner::ready = true;
    app_runner::cv.notify_all();
}
