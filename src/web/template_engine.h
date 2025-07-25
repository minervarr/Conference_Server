// src/web/template_engine.h
#pragma once
#include <string>
#include <map>

namespace utec {

    class TemplateEngine {
    public:
        static std::string render(const std::string& template_str,
                                 const std::map<std::string, std::string>& variables);

    private:
        static void replaceVariable(std::string& content,
                                   const std::string& var_name,
                                   const std::string& value);
    };

} // namespace utec

// ================================================================
