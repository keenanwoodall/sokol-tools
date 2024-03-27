/*
    Utility functions shared by output generators
 */
#include "util.h"
#include "fmt/format.h"
#include "pystring.h"

namespace shdc::gen::util {

using namespace refl;

ErrMsg check_errors(const Input& inp,
                      const Spirvcross& spirvcross,
                      Slang::Enum slang)
{
    for (const auto& item: inp.programs) {
        const Program& prog = item.second;
        int vs_snippet_index = inp.snippet_map.at(prog.vs_name);
        int fs_snippet_index = inp.snippet_map.at(prog.fs_name);
        const SpirvcrossSource* vs_src = spirvcross.find_source_by_snippet_index(vs_snippet_index);
        const SpirvcrossSource* fs_src = spirvcross.find_source_by_snippet_index(fs_snippet_index);
        if (vs_src == nullptr) {
            return inp.error(inp.snippets[vs_snippet_index].lines[0],
                fmt::format("no generated '{}' source for vertex shader '{}' in program '{}'",
                    Slang::to_str(slang), prog.vs_name, prog.name));
        }
        if (fs_src == nullptr) {
            return inp.error(inp.snippets[vs_snippet_index].lines[0],
                fmt::format("no generated '{}' source for fragment shader '{}' in program '{}'",
                    Slang::to_str(slang), prog.fs_name, prog.name));
        }
    }
    // all ok
    return ErrMsg();
}

int roundup(int val, int round_to) {
    return (val + (round_to - 1)) & ~(round_to - 1);
}

std::string mod_prefix(const Input& inp) {
    if (inp.module.empty()) {
        return "";
    } else {
        return fmt::format("{}_", inp.module);
    }
}

const SpirvcrossSource* find_spirvcross_source_by_shader_name(const std::string& shader_name, const Input& inp, const Spirvcross& spirvcross) {
    assert(!shader_name.empty());
    int snippet_index = inp.snippet_map.at(shader_name);
    return spirvcross.find_source_by_snippet_index(snippet_index);
}

const BytecodeBlob* find_bytecode_blob_by_shader_name(const std::string& shader_name, const Input& inp, const Bytecode& bytecode) {
    assert(!shader_name.empty());
    int snippet_index = inp.snippet_map.at(shader_name);
    return bytecode.find_blob_by_snippet_index(snippet_index);
}

std::string to_pascal_case(const std::string& str) {
    std::vector<std::string> splits;
    pystring::split(str, splits, "_");
    std::vector<std::string> parts;
    for (const auto& part: splits) {
        parts.push_back(pystring::capitalize(part));
    }
    return pystring::join("", parts);
}

std::string to_ada_case(const std::string& str) {
    std::vector<std::string> splits;
    pystring::split(str, splits, "_");
    std::vector<std::string> parts;
    for (const auto& part: splits) {
        parts.push_back(pystring::capitalize(part));
    }
    return pystring::join("_", parts);
}

std::string to_camel_case(const std::string& str) {
    std::string res = to_pascal_case(str);
    res[0] = tolower(res[0]);
    return res;
}

std::string replace_C_comment_tokens(const std::string& str) {
    static const std::string comment_start_old = "/*";
    static const std::string comment_start_new = "/_";
    static const std::string comment_end_old = "*/";
    static const std::string comment_end_new = "_/";
    std::string s = pystring::replace(str, comment_start_old, comment_start_new);
    s = pystring::replace(s, comment_end_old, comment_end_new);
    return s;
}

const char* slang_file_extension(Slang::Enum c, bool binary) {
    switch (c) {
        case Slang::GLSL410:
        case Slang::GLSL430:
        case Slang::GLSL300ES:
            return ".glsl";
        case Slang::HLSL4:
        case Slang::HLSL5:
            return binary ? ".fxc" : ".hlsl";
        case Slang::METAL_MACOS:
        case Slang::METAL_IOS:
        case Slang::METAL_SIM:
            return binary ? ".metallib" : ".metal";
        case Slang::WGSL:
            return ".wgsl";
        default:
            return "";
    }
}

} // namespace
