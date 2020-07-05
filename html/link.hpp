#ifndef FASTEV_HTML_LINK
#define FASTEV_HTML_LINK

#include "tag.hpp"

namespace fastev
{
    class HTMLLink : public HTMLTag
    {
    public:
        HTMLLink(std::string href, std::string text) : HTMLTag("a")
        {
            attr("href", href).body(text);
        }

        HTMLLink(std::string href, std::string text, bool new_tab) : HTMLLink(href, text)
        {
            if (new_tab)
            {
                this->attr("target", "_blank");
            }
        }
    };
} // namespace fastev

#endif