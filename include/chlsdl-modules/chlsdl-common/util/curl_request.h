#ifndef CURL_REQUEST_H_
#define CURL_REQUEST_H_

#include <chlsdl/macros.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#define curl_request_get(url, buf, ...)                                        \
    curl_request_get_args(                                                     \
        url, buf, &(const struct curl_request_get_args) { __VA_ARGS__ })

struct curl_buffer {
    size_t size;
    size_t at;
    char * data;
};

#define __DECLARE_CURL_REQUEST_COMMON                                          \
    struct {                                                                   \
        const char *  referer;                                                 \
        const char ** custom_headers; /* NULL-terminated array */              \
    }

struct curl_request_get_args {
    __DECLARE_CURL_REQUEST_COMMON;
};

struct curl_request_post_args {
    __DECLARE_CURL_REQUEST_COMMON;
    curl_off_t postfieldsize_large;
};

#ifndef __KEEP_DECLARE_CURL_REQUEST_COMMON
#    undef __DECLARE_CURL_REQUEST_COMMON
#endif

extern struct curl_buffer *
curl_buffer_alloc(size_t n);

CHLSDL_ALWAYS_INLINE inline void
curl_buffer_dealloc(struct curl_buffer * buf)
{
    if (buf) {
        free(buf->data);
        buf->data = NULL;
    }

    free(buf);
    buf = NULL;
}

/**
 * for use with __chlsdl_defer() macro
 */
CHLSDL_ALWAYS_INLINE inline void
__curl_buffer_dealloc(struct curl_buffer ** pbuf)
{
    curl_buffer_dealloc(*pbuf);
}

extern void
set_curl_logfile_path(const char * path);
extern void
unset_curl_logfile_path();

extern void
unset_curl_user_agent();
extern void
set_curl_user_agent(const char * new_user_agent);

extern int
curl_request_get_args(const char * url, struct curl_buffer * buf,
    const struct curl_request_get_args * args);
extern int
curl_request_post(const char * url, struct curl_buffer * buf,
    const char * postfields, curl_off_t postfieldsize_large);

#endif // CURL_REQUEST_H_
