//
// Created by Damian Netter on 20/06/2025.
//

#include "ZAKit/bridge.hpp"

#include "ZNBKit/debug.hpp"

jint za_kit::bridge::push_shader(JNIEnv* jni, jobject object, jstring name, jstring source)
{
    debug_print("Bridge push called");
    return 0; // Placeholder for actual implementation
}
