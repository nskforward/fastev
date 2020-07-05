#ifndef FASTEV_CORE_ISTRING
#define FASTEV_CORE_ISTRING

#include <string>

namespace fastev
{
    class IString
    {
    public:
        virtual std::string str() = 0;
    };
} // namespace fastev

#endif