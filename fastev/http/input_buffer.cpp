#include "input_buffer.hpp"

namespace fastev
{
    void InputBuffer::append(char *chunk, size_t size)
    {
        for (size_t i = 0; i < size; i++)
        {
            char byte = chunk[i];
            switch (state)
            {
            // HTTP METHOD
            case InputBufferState::method:
                if (byte == '\r' || byte == '\n')
                {
                    break;
                }
                if (byte == ' ')
                {
                    data[pos] = '\0';
                    if (!onMethod(data))
                    {
                        //cout << chunk << endl;
                        throw HTTPParserException("unknown http method: %s", method);
                    }
                    strcpy(method, data);
                    pos = 0;
                    state = InputBufferState::uri;
                    break;
                }
                data[pos++] = byte;
                if (pos == 7)
                {
                    throw HTTPParserException("http method is too long");
                }
                break;

            // URI
            case InputBufferState::uri:
                if (byte == ' ')
                {
                    data[pos] = '\0';
                    strcpy(uri, data);
                    pos = 0;
                    state = InputBufferState::proto;
                    break;
                }
                data[pos++] = byte;
                if (pos > FASTEV_HTTP_MAX_URI_SIZE)
                {
                    throw HTTPParserException("uri is too long");
                }
                break;

            // PROTO
            case InputBufferState::proto:
                if (byte == '\r')
                {
                    data[pos] = '\0';
                    onProto(data);
                    strcpy(http_version, data);
                    pos = 0;
                    data[pos++] = byte;
                    state = InputBufferState::eof;
                    break;
                }
                data[pos++] = byte;
                if (pos > 8)
                {
                    throw HTTPParserException("protocol version is too long");
                }
                break;

            // EOF
            case InputBufferState::eof:
                if (byte != '\r' && byte != '\n')
                {
                    pos = 0;
                    data[pos++] = byte;
                    state = InputBufferState::header_name;
                    break;
                }
                if (pos == 3 && data[0] == '\r' && data[1] == '\n' && data[2] == '\r' && byte == '\n')
                {
                    is_full = true;
                    return;
                }
                if (pos > 3)
                {
                    throw HTTPParserException("cannot find delimiter");
                }
                data[pos++] = byte;
                break;

            // HEADER NAME
            case InputBufferState::header_name:
                if (byte == ':')
                {
                    data[pos] = '\0';
                    onHeader(data); // chnages state
                    pos = 0;
                    break;
                }
                data[pos++] = byte;
                if (pos > 128)
                {
                    throw HTTPParserException("header name is too long");
                }
                break;

            // HOST HEADER NAME
            case InputBufferState::host:
                if (pos == 0 && byte == ' ')
                {
                    continue;
                }
                if (byte == '\r')
                {
                    data[pos] = '\0';
                    strcpy(host, data);
                    pos = 0;
                    data[pos++] = byte;
                    state = InputBufferState::eof;
                    break;
                }
                data[pos++] = byte;
                if (pos > FASTEV_HTTP_MAX_HEADER_VALUE_SIZE)
                {
                    throw HTTPParserException("header host is too long");
                }
                break;

            // CONTENT TYPE HEADER
            case InputBufferState::content_type:
                if (pos == 0 && byte == ' ')
                {
                    continue;
                }
                if (byte == '\r')
                {
                    data[pos] = '\0';
                    strcpy(content_type, data);
                    pos = 0;
                    data[pos++] = byte;
                    state = InputBufferState::eof;
                    break;
                }
                data[pos++] = byte;
                if (pos > FASTEV_HTTP_MAX_HEADER_VALUE_SIZE)
                {
                    throw HTTPParserException("header content type is too long");
                }
                break;

            // CONTENT LENGTH HEADER
            case InputBufferState::content_len:
                if (pos == 0 && byte == ' ')
                {
                    continue;
                }
                if (byte == '\r')
                {
                    data[pos] = '\0';
                    strcpy(content_length, data);
                    pos = 0;
                    data[pos++] = byte;
                    state = InputBufferState::eof;
                    break;
                }
                data[pos++] = byte;
                if (pos > 5)
                {
                    throw HTTPParserException("header content length is too long");
                }
                break;

            // AGENT HEADER
            case InputBufferState::agent:
                if (pos == 0 && byte == ' ')
                {
                    continue;
                }
                if (byte == '\r')
                {
                    data[pos] = '\0';
                    strcpy(agent, data);
                    pos = 0;
                    data[pos++] = byte;
                    state = InputBufferState::eof;
                    break;
                }
                data[pos++] = byte;
                if (pos > FASTEV_HTTP_MAX_HEADER_VALUE_SIZE)
                {
                    throw HTTPParserException("header user-agent is too long");
                }
                break;

            // HEADER VALUE
            case InputBufferState::header_value:
                if (pos == 0 && byte == ' ')
                {
                    continue;
                }
                if (byte == '\r')
                {
                    data[pos] = '\0';
                    pos = 0;
                    data[pos++] = byte;
                    state = InputBufferState::eof;
                    break;
                }
                data[pos++] = byte;
                if (pos > FASTEV_HTTP_MAX_HEADER_VALUE_SIZE)
                {
                    throw HTTPParserException("header is too long");
                }
                break;

            default:
                throw HTTPParserException("Unknown state: %d", state);
            }
        }
    } // namespace fastev

    void InputBuffer::reset()
    {
        pos = 0;
        state = InputBufferState::method;
        method[0] = '\0';
        uri[0] = '\0';
        http_version[0] = '\0';
        host[0] = '\0';
        agent[0] = '\0';
        content_type[0] = '\0';
        content_length[0] = 0;
        is_full = false;
    }

    bool InputBuffer::isFull()
    {
        return is_full;
    }

    char *InputBuffer::getURI()
    {
        return uri;
    }

    char *InputBuffer::getMethod()
    {
        return method;
    }

    char *InputBuffer::getHost()
    {
        return host;
    }

    // PRIVATE METHODS
    bool InputBuffer::onMethod(char *method)
    {
        if (strcmp(method, "GET") != 0 &&
            strcmp(method, "POST") != 0 &&
            strcmp(method, "PUT") != 0 &&
            strcmp(method, "DELETE") != 0)
        {
            return false;
        }
        return true;
    }

    void InputBuffer::onProto(char *proto)
    {
        if (strcmp(method, "HTTP/1.0") == 0 &&
            strcmp(method, "HTTP/1.1") != 0 &&
            strcmp(method, "HTTP/2.0") != 0 &&
            strcmp(method, "DELETE") != 0)
        {
            throw HTTPParserException("unknown http version: %s", proto);
        }
    }

    void InputBuffer::onHeader(char *header)
    {
        if (strcmp(header, "Host") == 0)
        {
            state = InputBufferState::host;
            return;
        }
        if (strcmp(header, "Content-Type") == 0)
        {
            state = InputBufferState::content_type;
            return;
        }
        if (strcmp(header, "User-Agent") == 0)
        {
            state = InputBufferState::agent;
            return;
        }
        state = InputBufferState::header_value;
    }

    OutputBuffer *InputBuffer::getOutputBuffer()
    {
        return &output_buffer;
    }
} // namespace fastev