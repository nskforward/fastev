#ifndef FASTEV_HTML_PAGE
#define FASTEV_HTML_PAGE

#include <string>
#include <sstream>
#include "template.hpp"
#include "../http/output_buffer.hpp"

namespace fastev
{
    class HTMLPage : Template
    {
    private:
        std::string _title;
        std::stringstream _body;

    public:
        HTMLPage(std::string filename);
        std::stringstream &body();
        void title(std::string title);
        std::string str();

        friend OutputBuffer &operator<<(OutputBuffer &resp, HTMLPage &page)
        {
            //resp.body() << page.str();
            return resp;
        }
    };

} // namespace fastev

#endif