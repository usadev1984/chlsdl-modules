#include "danbooru.h"
#include "print.h"

#include <assert.h>
#include <chlsdl/common/common.h>
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

    const char * parent_id;

    struct {
        int           ninformation;
        const char ** information;
        int           nchildren;
        const char ** children;
        int           nsiblings;
        const char ** siblings;
    } taglists;
} danbooru_info;

struct module g_libdanbooru = {
    danbooru_deinit,
    danbooru_func,
};

static const char * module_downloads_dir;

static void
danbooru_load_to_array(json_object * jdata, const char * jdata_key,
    int * array_len, const char *** array)
{
    json_object * tmp = json_object_object_get(jdata, jdata_key);
    if (!tmp) {
        *array_len = 0;
        *array     = NULL;
        return;
    }
    int ntmp = json_object_array_length(tmp);

    const char ** r = malloc(sizeof(*r) * ntmp);
    assert(r);
    for (int i = 0; i < ntmp; ++i) {
        r[i]
            = strdup(json_object_get_string(json_object_array_get_idx(tmp, i)));
        assert(r[i]);
    }

    *array_len = ntmp;
    *array     = r;
}

static void
to_danbooru_info(danbooru_info * info, char * data)
{
    json_object * jdata = json_tokener_parse(data);
    assert(jdata);
    info->url  = json_object_get_string(json_object_object_get(jdata, "url"));
    info->name = json_object_get_string(json_object_object_get(jdata, "name"));
    info->src = json_object_get_string(json_object_object_get(jdata, "module"));

    print_debug_warn("info->url: '%s'\n", info->url);
    print_debug_warn("info->name: '%s'\n", info->name);
    print_debug_warn("info->src: '%s'\n", info->src);

    info->parent_id
        = json_object_get_string(json_object_object_get(jdata, "parent_id"));
    print_debug_warn("info->parent_id: '%s'\n", info->parent_id);

    danbooru_load_to_array(jdata, "information", &info->taglists.ninformation,
        &info->taglists.information);
    for (int i = 0; i < info->taglists.ninformation; ++i)
        print_debug_warn("info->taglists.information: '%s'\n",
            info->taglists.information[i]);

    danbooru_load_to_array(
        jdata, "children", &info->taglists.nchildren, &info->taglists.children);
    for (int i = 0; i < info->taglists.nchildren; ++i)
        print_debug_warn(
            "info->taglists.children: '%s'\n", info->taglists.children[i]);

    danbooru_load_to_array(
        jdata, "siblings", &info->taglists.nsiblings, &info->taglists.siblings);
    for (int i = 0; i < info->taglists.nsiblings; ++i)
        print_debug_warn(
            "info->taglists.siblings: '%s'\n", info->taglists.siblings[i]);
}

const struct module *
danbooru_init(const struct chlsdl_data * cdata)
{
    print_debug_warn("initalizing danbooru...\n");

    g_libdanbooru.regex.pattern
        = pcre2_compile((PCRE2_SPTR8) "https://cdn\\.donmai\\.us/original/"
                                      ".*\\.(?:png|jpg|jpeg)",
            PCRE2_ZERO_TERMINATED, 0, &(int) { 0 }, &(PCRE2_SIZE) { 0 }, NULL);
    assert(g_libdanbooru.regex.pattern);
    g_libdanbooru.regex.md = pcre2_match_data_create_from_pattern(
        g_libdanbooru.regex.pattern, NULL);
    assert(g_libdanbooru.regex.md);

    module_downloads_dir = svconcat("%s/danbooru", cdata->downloads_dir);

    if (mkdir(module_downloads_dir, S_IRWXU | S_IRGRP) == -1 && errno != EEXIST)
        assert(0);

    return &g_libdanbooru;
}

void
danbooru_deinit()
{
    print_debug_warn("cleaning up danbooru...\n");
    free((char *)module_downloads_dir);
    free(g_libdanbooru.regex.md);
    free(g_libdanbooru.regex.pattern);
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
danbooru_func(void * vargp)
{
    chlsdl_defer char * orig_data = strdup(vargp);
    assert(orig_data);

    char * data = orig_data + strlen(get_line_from_string(orig_data));

    danbooru_info info;
    to_danbooru_info(&info, data);
}
