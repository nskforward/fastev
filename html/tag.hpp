#ifndef FASTEV_HTMLTAG_HPP
#define FASTEV_HTMLTAG_HPP

#include <map>
#include <string>
#include <sstream>
#include <iterator>

namespace fastev
{
    class HTMLTag
    {
    private:
        bool _closed = false;
        std::map<std::string, std::string> _attr;
        std::string _name;
        std::string _body;

    public:
        HTMLTag(std::string name, bool closed);
        HTMLTag(std::string name);
        std::string str();
        HTMLTag &body(std::string body);
        HTMLTag &attr(std::string name, std::string value);

        friend HTMLTag &operator<<(HTMLTag &dest, HTMLTag &src)
        {
            dest._body = dest._body + " " + src.str();
            return dest;
        }

        friend std::ostream &operator<<(std::ostream &out, HTMLTag &src)
        {
            out << src.str();
            return out;
        }
    };
} // namespace fastev

#endif