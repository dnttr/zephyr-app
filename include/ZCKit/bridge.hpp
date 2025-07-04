//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <jni.h>
#include <ZNBKit/internal/wrapper.hpp>
#include <ZNBKit/jni/signatures/klass_signature.hpp>
#include <ZNBKit/vm/vm_object.hpp>
#include <__thread/jthread.h>

#define DEFAULT JNIEnv *jni, jobject object

namespace zc_kit
{
    class bridge
    {
        static void invoke_connect();

        static void load_native(const znb_kit::klass_signature &signature, std::vector<std::string> libraries);

    public:
        static const std::string bridge_klass_name;

        static znb_kit::vm_object *vm_obj;

        static std::unique_ptr<znb_kit::klass_signature> bridge_signature;

        static const std::unordered_multimap<std::string, znb_kit::jni_bridge_reference> mapped_methods;

        static jint push_shader(DEFAULT, jstring name, jstring source);

        static jint finish_loading(DEFAULT);

        static jint push_texture(DEFAULT, jstring name, jobject direct_buffer, jint width, jint height);

        static jint push_font(DEFAULT, jstring name, jbyteArray bytes);

        static void initialize_bridge(znb_kit::vm_object *vm_obj);

        static void run(const znb_kit::klass_signature &signature,
                                                   std::vector<std::string> libraries_to_load);

        static void terminate();
    };
}
