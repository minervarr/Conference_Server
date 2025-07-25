// src/web/embedded_resources.h
#pragma once
#include <string>

namespace utec {

    class EmbeddedResources {
    public:
        static std::string getIndexHtml();
        static std::string getMainCss();
        static std::string getMainJs();
        static std::string getFavicon();

        // Template variables replacement
        static std::string processTemplate(const std::string& html,
                                         const std::string& server_url);

    private:
        static const std::string INDEX_HTML;
        static const std::string MAIN_CSS;
        static const std::string MAIN_JS;
    };

} // namespace utec

// ================================================================