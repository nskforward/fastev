#include "tag.hpp"

namespace fastev
{
    HTMLTag::HTMLTag(std::string name)
    {
        _name = name;
    }

    HTMLTag::HTMLTag(std::string name, bool closed) : HTMLTag::HTMLTag(name)
    {
        _closed = closed;
    }

    HTMLTag &HTMLTag::attr(std::string name, std::string value)
    {
        _attr[name] = value;
        return *this;
    }

    HTMLTag &HTMLTag::body(std::string body)
    {
        _body = body;
        if (!_closed)
        {
            _closed = true;
        }
        return *this;
    }

    std::string HTMLTag::str()
    {
        std::stringstream ss;
        ss << "<" << _name;
        std::map<std::string, std::string>::iterator it = _attr.begin();
        while (it != _attr.end())
        {
            ss << " " << it->first << "=\"" << it->second << "\"";
            it++;
        }
        ss << ">";
        if (_closed)
        {
            ss << _body << "</" << _name << ">";
        }
        return ss.str();
    }
} // namespace fastev