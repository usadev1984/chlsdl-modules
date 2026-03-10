#include "r34.h"

#include <assert.h>
#include <chlsdl/common/common.h>
#include <chlsdl/common/print.h>
#include <chlsdl/common/util/util.h>
#include <chlsdl/macros.h>
#include <chlsdl/module.h>
#include <errno.h>
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

typedef struct {
    const char * url;
    const char * name;
    const char * src;
} r34_info;

struct module g_libr34 = {
    r34_deinit,
    r34_func,
};

static const char * module_downloads_dir;

const struct module *
r34_init(const struct chlsdl_data * cdata)
{
    print_debug_warn("initalizing r34...\n");

    g_libr34.regex.pattern = pcre2_compile(
        (PCRE2_SPTR8) "https://(?:wimg|.+)\\.rule34\\.xxx//images/"
                      ".*/.*\\.(?:png|jpg|jpeg|gif|mp4)",
        PCRE2_ZERO_TERMINATED, 0, &(int) { 0 }, &(PCRE2_SIZE) { 0 }, NULL);
    assert(g_libr34.regex.pattern);
    g_libr34.regex.md
        = pcre2_match_data_create_from_pattern(g_libr34.regex.pattern, NULL);
    assert(g_libr34.regex.md);

    module_downloads_dir = svconcat("%s/r34", cdata->downloads_dir);

    if (mkdir(module_downloads_dir, S_IRWXU | S_IRGRP) == -1 && errno != EEXIST)
        assert(0);

    return &g_libr34;
}

void
r34_deinit()
{
    print_debug_warn("cleaning up r34...\n");
    free((char *)module_downloads_dir);
    free(g_libr34.regex.md);
    free(g_libr34.regex.pattern);
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

static void
to_r34_info(r34_info * info, const char * data)
{
    json_object * jdata = json_tokener_parse(data);
    assert(jdata);
    info->url  = json_object_get_string(json_object_object_get(jdata, "url"));
    info->name = json_object_get_string(json_object_object_get(jdata, "name"));
    info->src = json_object_get_string(json_object_object_get(jdata, "source"));

    print_debug_warn("info->url: '%s'\n", info->url);
    print_debug_warn("info->name: '%s'\n", info->name);
    print_debug_warn("info->src: '%s'\n", info->src);
}

void
r34_func(void * vargp)
{
    chlsdl_defer char * orig_data = strdup(vargp);
    assert(orig_data);

    char * data = orig_data + strlen(get_line_from_string(orig_data));

    r34_info info;
    to_r34_info(&info, data);

}
