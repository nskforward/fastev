#ifndef FASTEV_HTML_PAGE_POOL
#define FASTEV_HTML_PAGE_POOL

#include <map>
#include "page.hpp"

namespace fastev
{
    class PagePool
    {
    private:
        std::map<std::string, HTMLPage> pool;

    public:
        HTMLPage &get(std::string filename)
        {
            if (pool.count(filename) == 0)
            {
                pool[filename] = HTMLPage(filename);
            }
            return pool[filename];
        }
    };

} // namespace fastev

#endif