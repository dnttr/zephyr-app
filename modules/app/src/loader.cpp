//
// Created by Damian Netter on 20/06/2025.
//

#include <iostream>

#include "ZNBKit/vm_management.hpp"
#include "ZNBKit/jni/signatures/method/void_method.hpp"

int main(const int argc, char* argv[])
{
    std::vector<std::string> args[argc - 1];
    for (int i = 1; i < argc; ++i)
    {
        args->push_back(std::string(argv[i]));
    }

    if (args->size() != 1)
    {
        std::cerr << "Invalid arguments." << std::endl;
        return 1;
    }

    const auto argument = args->at(0);
    const size_t position = argument.find_last_of('=');

    if (position == std::string::npos)
    {
        std::cerr << "Invalid VM path format. Expected format: vm_path=<vm_classpath>" << std::endl;
        return 1;
    }

    const auto vm_path = argument.substr(position + 1, argument.length());

    if (!std::filesystem::exists(vm_path))
    {
        std::cerr << "VM path does not exist: " << vm_path << std::endl;
        return 1;
    }

    const auto vm_object = znb_kit::vm_management::create_and_wrap_vm(vm_path);
    const auto vm = vm_object.get();

    if (!vm)
    {
        std::cerr << "Failed to create VM object." << std::endl;
        return 1;
    }

    const auto jni_env = vm->get_env();
    const auto jvmti_env = vm->get_jvmti()->get().get_owner();

    znb_kit::jvmti_object jvmti(jni_env, jvmti_env);

    const znb_kit::klass_signature loader(
        jni_env, "org/dnttr/zephyr/management/Loader");

    znb_kit::void_method method(jni_env, loader, "load", "()V", std::nullopt, true);

    return 0;
}
