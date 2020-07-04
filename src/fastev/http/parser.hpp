#ifndef FASTEV_HTTP_PARSER
#define FASTEV_HTTP_PARSER

#include <unordered_map>
#include "../core/byte_buffer.hpp"

using namespace std;

namespace fastev
{
    enum ParserState
    {
        method,
        uri,
        proto,
        header_name,
        header_value,
        host,
        content_type,
        content_len,
        agent,
        body,
        eof
    };

    class HTTPParser
    {
    private:
    public:
        static void parse(char *method, char *uri, char *http_ver, unordered_map<string, string> &headers, ByteBuffer<1024> *buf, size_t delimiter_pos);
    };

    void HTTPParser::parse(char *method, char *uri, char *http_ver, unordered_map<string, string> &headers, ByteBuffer<1024> *buf, size_t delimiter_pos)
    {
        auto src = buf->c_str();
        auto state = ParserState::method;
        size_t start_pos = 0;
        char header_name_buff[64];
        char header_value_buff[256];
        for (size_t i = 0; i < delimiter_pos; i++)
        {
            char byte = src[i];
            switch (state)
            {
            // HTTP METHOD
            case ParserState::method:
                if (byte == ' ')
                {
                    memcpy(method, src, i);
                    method[i] = '\0';
                    state = ParserState::uri;
                    start_pos = i + 1;
                    continue;
                }
                break;

            // URI
            case ParserState::uri:
                if (byte == ' ')
                {
                    memcpy(uri, src + start_pos, i - start_pos);
                    uri[i - start_pos] = '\0';
                    start_pos = start_pos + i - start_pos + 1;
                    state = ParserState::proto;
                    continue;
                }
                break;

            // PROTO
            case ParserState::proto:
                if (byte == '\r')
                {
                    memcpy(http_ver, src + start_pos, i - start_pos);
                    http_ver[i - start_pos] = '\0';
                    start_pos = start_pos + i - start_pos + 1;
                    state = ParserState::eof;
                    continue;
                }
                break;

            // EOF
            case ParserState::eof:
                if (byte != '\r' && byte != '\n')
                {
                    state = ParserState::header_name;
                    start_pos = start_pos + i - start_pos;
                    continue;
                }
                if (src[i - 3] == '\r' && src[i - 2] == '\n' && src[i - 1] == '\r' && byte == '\n')
                {
                    return;
                }
                break;

            // HEADER NAME
            case ParserState::header_name:
                if (byte == ':')
                {
                    memcpy(header_name_buff, src + start_pos, i - start_pos);
                    header_name_buff[i - start_pos] = '\0';
                    start_pos = start_pos + i - start_pos + 2;
                    state = ParserState::header_value;
                    continue;
                }
                break;

            // HEADER VALUE
            case ParserState::header_value:
                if (byte == ' ')
                {
                    continue;
                }
                if (byte == '\r')
                {
                    memcpy(header_value_buff, src + start_pos, i - start_pos);
                    header_value_buff[i - start_pos] = '\0';
                    start_pos = start_pos + i - start_pos;
                    headers[header_name_buff] = header_value_buff;
                    state = ParserState::eof;
                    continue;
                }
                break;

            default:
                throw KernelException("Unknown state: %d", state);
            }
        }
    }
} // namespace fastev

#endif