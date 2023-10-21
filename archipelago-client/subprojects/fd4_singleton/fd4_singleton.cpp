// By tremwil, extracted from TGA's CheatEngine table.
// See https://gist.github.com/tremwil/0eef8f13337f32a41c7e546d4deff93c
// Originally https://github.com/The-Grand-Archives/Dark-Souls-III-CT-TGA/blob/main/resources/include/tga/fd4_singleton.h

#include "fd4_singleton.h"
#include "mem/pattern.h"
#include "mem/module.h"

#include <algorithm>
#include <optional>

// Get a particular code section of the main module of the process
const mem::region main_module_section(const char* name)
{
    auto main_module = mem::module::main();
    for (const auto& section : main_module.section_headers()) {
        if (strncmp((char*)section.Name, name, sizeof(section.Name))) continue;
        return mem::region(main_module.start.add(section.VirtualAddress), section.Misc.VirtualSize);
    }
    throw std::runtime_error(std::format("{} section does not exist in main executable module", name));
}

std::optional<std::string_view> get_printable_cstr_at(void* addr, size_t min_len, size_t max_len)
{
    // Check if string is null terminated
    auto len = strnlen((char*)addr, max_len);
    if (len < min_len || len == max_len) return std::nullopt;

    std::string_view str((char*)addr, len);

    // Check if string is printable
    if (std::any_of(str.begin(), str.end(), [](char c) { return c < '!' || c > 'z'; }))
        return std::nullopt;

    return str;
}

void** FD4SingletonFinder::address_of(const std::string_view class_name) const
{
    auto i = singleton_addresses.find(class_name);
    return (i == singleton_addresses.end()) ? nullptr : i->second;
}

void* FD4SingletonFinder::instance_of(const std::string_view class_name) const
{
    auto i = singleton_addresses.find(class_name);
    return (i == singleton_addresses.end()) ? nullptr : *i->second;
}

FD4SingletonFinder::FD4SingletonFinder()
{
    mem::pattern pat(
        "48 8b ? ? ? ? ? "    // MOV REG, [MEM]
        "48 85 ? "          // TEST REG, REG
        "75 26 "            // JNZ +26
        "4c 8d 0d ? ? ? ? " // LEA R9, [singleton_name]
        "4c 8d 05 ? ? ? ? " // LEA R8, [% s:未初期化のシングルトンにアクセスしました]
        "ba b1 00 00 00 "   // MOV EDX, 0xb1
        "48 8d 0d ? ? ? ? " // LEA RCX, [file_path]
        "e8 ? ? ? ?"        // CALL log_thunk
    );
    mem::default_scanner scanner(pat);

    auto text_section = main_module_section(".text");
    auto data_section = main_module_section(".data");
    auto rdata_section = main_module_section(".rdata");

    for (auto candidate : scanner.scan_all(text_section)) {
        // Check static address in module
        auto static_addr = candidate.add(7 + *candidate.add(3).as<int32_t*>());
        if (!data_section.contains(static_addr)) continue;

        // Check if FD4Singleton header path is there
        auto filepath_ptr = candidate.add(38 + *candidate.add(34).as<int32_t*>());
        if (!rdata_section.contains(filepath_ptr)) continue;

        // Check if FD4Singleton path string is valid
        auto mabye_path = get_printable_cstr_at(filepath_ptr.as<void*>(), 10, 256);
        if (!mabye_path.has_value() || !mabye_path.value().ends_with("FD4Singleton.h"))
            continue;

        // Check if the singleton name string is in the module
        auto name_ptr = candidate.add(19 + *candidate.add(15).as<int32_t*>());
        if (!rdata_section.contains(name_ptr)) continue;

        // Check if it's a valid string
        if (auto name = get_printable_cstr_at(name_ptr.as<void*>(), 2, 128)) {
            auto i = name.value().rfind("::");
            auto no_namespace_name = name.value().substr(i == -1 ? 0 : i + 2);

            auto prv_entry = singleton_addresses.find(no_namespace_name);
            if (prv_entry == singleton_addresses.end()) {
                //LogTrace("{} -> {:x}", no_namespace_name, static_addr.as<uintptr_t>());
                singleton_addresses[no_namespace_name] = static_addr.as<void**>();
            }
            else if (prv_entry->second != static_addr) { // FD4DebugMenuManager has 2 addresses for some reason
                //LogWarn("Address mismatch for singleton {} : {:p} vs {:p}",
                //    no_namespace_name, (void*)prv_entry->second, static_addr.as<void*>());
            }
        }
    }
}
