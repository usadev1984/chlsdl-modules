#include "gelbooru.h"

#include <assert.h>
#include <chlsdl-modules/chlsdl-common/common.h>
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

#ifdef USE_LIBNOTIFY
#    define MOD_PRINT_AND_NOTIFY(print, fmt, ...)                              \
        ({                                                                     \
            char * body = svconcat(fmt __VA_OPT__(, ) __VA_ARGS__);            \
            assert(body);                                                      \
            print("%s\n", body);                                               \
            chlsdl_notify_notification_show_new("chlsdl-gelbooru", body,       \
                .timeout = default_notification_timeout);                      \
            free(body);                                                        \
        })

#    define MOD_ERROR_AND_NOTIFY(print, fmt, ...)                              \
        ({                                                                     \
            char * body = svconcat(fmt __VA_OPT__(, ) __VA_ARGS__);            \
            assert(body);                                                      \
            print("%s\n", body);                                               \
            chlsdl_notify_notification_show_new("chlsdl-gelbooru", body,       \
                .timeout = default_notification_timeout,                       \
                .urgency = chlsdl_notify_urgency_critical);                    \
            free(body);                                                        \
        })
#else
#    define MOD_PRINT_AND_NOTIFY(print, fmt, ...)                              \
        print(fmt "\n" __VA_OPT__(, ) __VA_ARGS__)

#    define MOD_ERROR_AND_NOTIFY(print, fmt, ...)                              \
        print(fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#endif

typedef struct {
    const char * url;
    const char * name;
    const char * src;
} gelbooru_info;

struct module g_libgelbooru = {
    gelbooru_deinit,
    gelbooru_func,
};

static const char * module_downloads_dir;

static int default_notification_timeout;

const struct module *
gelbooru_init(const struct chlsdl_data * cdata)
{
    print_debug_warn("initalizing gelbooru...\n");

    g_libgelbooru.regex.pattern = pcre2_compile(
        (PCRE2_SPTR8) "https://(?:img2|.+)\\.gelbooru\\.com//?images/"
                      ".*/.*\\.(?:png|jpg|jpeg|gif|mp4)",
        PCRE2_ZERO_TERMINATED, 0, &(int) { 0 }, &(PCRE2_SIZE) { 0 }, NULL);
    assert(g_libgelbooru.regex.pattern);
    g_libgelbooru.regex.md = pcre2_match_data_create_from_pattern(
        g_libgelbooru.regex.pattern, NULL);
    assert(g_libgelbooru.regex.md);

    module_downloads_dir = svconcat("%s/gelbooru", cdata->downloads_dir);

    if (mkdir(module_downloads_dir, S_IRWXU | S_IRGRP) == -1 && errno != EEXIST)
        assert(0);

    default_notification_timeout = cdata->default_notification_timeout;

    return &g_libgelbooru;
}

void
gelbooru_deinit()
{
    print_debug_warn("cleaning up gelbooru...\n");
    free((char *)module_downloads_dir);
    free(g_libgelbooru.regex.md);
    free(g_libgelbooru.regex.pattern);
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
to_gelbooru_info(gelbooru_info * info, const char * data)
{
    json_object * jdata = json_tokener_parse(data);
    assert(jdata);
    info->url  = json_object_get_string(json_object_object_get(jdata, "url"));
    info->name = json_object_get_string(json_object_object_get(jdata, "name"));
    info->src  = json_object_get_string(json_object_object_get(jdata, "src"));

    print_debug_warn("info->url: '%s'\n", info->url);
    print_debug_warn("info->name: '%s'\n", info->name);
    print_debug_warn("info->src: '%s'\n", info->src);
}

void
gelbooru_func(void * vargp)
{
    chlsdl_defer char * orig_data = strdup(vargp);
    assert(orig_data);

    char * data = orig_data + strlen(get_line_from_string(orig_data));

    gelbooru_info info;
    to_gelbooru_info(&info, data);

    chlsdl_defer char * metadata_file
        = svconcat("%s/%s.json", module_downloads_dir, info.name);

    if (file_exists(metadata_file))
        return (void)MOD_PRINT_AND_NOTIFY(
            print_warn, "'%s' has already been downloaded", info.name);
}
