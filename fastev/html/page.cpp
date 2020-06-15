#include "page.hpp"

namespace fastev
{
    HTMLPage::HTMLPage(std::string filename) : Template(filename) {}

    void HTMLPage::title(std::string title)
    {
        _title = title;
    }

    std::stringstream &HTMLPage::body()
    {
        return _body;
    }

    std::string HTMLPage::str()
    {
        replace("%title%", _title);
        replace("%body%", _body.str());
        return Template::str();
    }

} // namespace fastev