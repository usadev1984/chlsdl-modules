#include "bunkr.h"

#include <assert.h>
#include <chlsdl-modules/chlsdl-common/print.h>
#include <chlsdl-modules/chlsdl-common/util/notify.h>
#include <chlsdl-modules/chlsdl-common/util/util.h>
#include <chlsdl/macros.h>
#include <chlsdl/module.h>
#include <errno.h>
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#define __MOD_NOTIFY_MOD_NAME "bunkr"
#include "../chlsdl-common/shared.h"

typedef struct {
    const char * url;
    const char * name;
    const char * src;

    json_object * post_info;
} bunkr_info;

struct module g_libbunkr = {
    bunkr_deinit,
    bunkr_func,
};

static const char * module_downloads_dir;

static int default_notification_timeout;

static void
to_bunkr_info(bunkr_info * info, char * data)
{
    json_object * jdata = json_tokener_parse(data);
    assert(jdata);
    info->url  = json_object_get_string(json_object_object_get(jdata, "url"));
    info->name = json_object_get_string(json_object_object_get(jdata, "name"));
    info->src = json_object_get_string(json_object_object_get(jdata, "source"));

    print_debug_warn("info->url: '%s'\n", info->url);
    print_debug_warn("info->name: '%s'\n", info->name);
    print_debug_warn("info->src: '%s'\n", info->src);

    info->post_info = json_object_object_get(jdata, "post_info");
    assert(info->post_info);
}

const struct module *
bunkr_init(const struct chlsdl_data * cdata)
{
    print_debug_warn("initalizing bunkr...\n");

    g_libbunkr.regex.pattern
        = pcre2_compile((PCRE2_SPTR8) "https://dl\\.bunkr\\.cr/file/\\d+",
            PCRE2_ZERO_TERMINATED, 0, &(int) { 0 }, &(PCRE2_SIZE) { 0 }, NULL);
    assert(g_libbunkr.regex.pattern);
    g_libbunkr.regex.md
        = pcre2_match_data_create_from_pattern(g_libbunkr.regex.pattern, NULL);
    assert(g_libbunkr.regex.md);

    module_downloads_dir = svconcat("%s/bunkr", cdata->downloads_dir);
    if (mkdir(module_downloads_dir, S_IRWXU | S_IRGRP) == -1 && errno != EEXIST)
        assert(0);

    default_notification_timeout = cdata->default_notification_timeout;

    return &g_libbunkr;
}

void
bunkr_deinit()
{
    print_debug_warn("cleaning up bunkr...\n");
    free((char *)module_downloads_dir);
    free(g_libbunkr.regex.md);
    free(g_libbunkr.regex.pattern);
}

static char *
get_line_from_string(const char * s)
{
    const char * nl = strchr(s, '\n');
    if (!nl)
        return NULL;

    char * r = strndup(s, nl - s);
    assert(r);
    return r;
}

void
bunkr_func(void * vargp)
{
    chlsdl_defer char * orig_data = strdup(vargp);
    assert(orig_data);

    char * data = orig_data + strlen(get_line_from_string(orig_data));

    bunkr_info info;
    to_bunkr_info(&info, data);

    chlsdl_defer char * metadata_file
        = svconcat("%s/%s.json", module_downloads_dir, info.name);

    if (file_exists(metadata_file)) {
        MOD_PRINT_AND_NOTIFY(
            print_warn, "'%s' has already been downloaded", info.name);
        return;
    }

    __chlsdl_defer(__curl_buffer_dealloc) struct curl_buffer * buf
        = curl_buffer_alloc(MEGABYTES_TO_BYTES(2));
    assert(buf);

    print_info("downloading: '%s'\n", info.url);

    /* download media */
    if (curl_request_get(info.url, .writer = { buf }) != CURLE_OK) {
        MOD_ERROR_AND_NOTIFY(print_error, "failed to download: '%s'", info.url);
        return;
    }

    char * out = svconcat("%s/%s", module_downloads_dir, info.name);
    assert(out);
    print_info("saving to: '%s'\n", out);
    write_buffer_to_file(out, buf->at, buf->data);

    free(out);
}
