#ifndef FASTEV_HTML_TEMPLATE
#define FASTEV_HTML_TEMPLATE

#include <string>
#include <fstream>
#include <sstream>
#include "../core/exception.hpp"

namespace fastev
{
    class Template
    {
    private:
        std::string _data;

    public:
        Template(std::string filename)
        {
            std::ifstream f(filename);
            if (!f)
            {
                throw KernelException("cannot open file: %s", filename.c_str());
            }
            std::ostringstream ss;
            ss << f.rdbuf();
            _data = ss.str();
            f.close();
        }

        void replace(std::string placeholder, std::string value)
        {
            int pos = _data.find(placeholder);
            if (pos < 0)
            {
                throw KernelException("template does not contain placeholder: %s", placeholder.c_str());
            }
            while (pos > -1)
            {
                _data.replace(pos, placeholder.size(), value);
                pos = _data.find(placeholder, pos + value.size());
            }
        }

        std::string str()
        {
            return _data;
        }
    };
} // namespace fastev

#endif