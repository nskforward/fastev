#ifndef FASTEV_HTTP_BYTE_BUFFER
#define FASTEV_HTTP_BYTE_BUFFER

#ifndef FASTEV_HTTP_MAX_HEADERS_SIZE
#define FASTEV_HTTP_MAX_HEADERS_SIZE 1024
#endif

#ifndef FASTEV_HTTP_MAX_BODY_SIZE
#define FASTEV_HTTP_MAX_BODY_SIZE 4096
#endif

#ifndef FASTEV_HTTP_MAX_URI_SIZE
#define FASTEV_HTTP_MAX_URI_SIZE 512
#endif

#ifndef FASTEV_HTTP_MAX_HEADER_VALUE_SIZE
#define FASTEV_HTTP_MAX_HEADER_VALUE_SIZE 512
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "../core/logger.hpp"
#include "exception.hpp"
#include "output_buffer.hpp"

using namespace std;

namespace fastev
{
    enum InputBufferState
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

    class InputBuffer
    {
    private:
        char data[FASTEV_HTTP_MAX_BODY_SIZE];
        size_t pos;
        int state;
        char content_length[6];
        char method[8];
        char http_version[9];
        char uri[FASTEV_HTTP_MAX_URI_SIZE + 1];
        char host[FASTEV_HTTP_MAX_HEADER_VALUE_SIZE + 1];
        char agent[FASTEV_HTTP_MAX_HEADER_VALUE_SIZE + 1];
        char content_type[FASTEV_HTTP_MAX_HEADER_VALUE_SIZE + 1];
        bool is_full;
        OutputBuffer output_buffer;

        bool onMethod(char *method);
        void onProto(char *proto);
        void onHeader(char *header);

    public:
        void reset();
        void append(char *chunk, size_t size);
        bool isFull();
        char *getURI();
        char *getMethod();
        char *getHost();
        OutputBuffer *getOutputBuffer();
    };

} // namespace fastev

#endif