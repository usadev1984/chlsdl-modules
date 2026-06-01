#include "r34.h"

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
            chlsdl_notify_notification_show_new(                               \
                "chlsdl-r34", body, .timeout = default_notification_timeout);  \
            free(body);                                                        \
        })

#    define MOD_ERROR_AND_NOTIFY(print, fmt, ...)                              \
        ({                                                                     \
            char * body = svconcat(fmt __VA_OPT__(, ) __VA_ARGS__);            \
            assert(body);                                                      \
            print("%s\n", body);                                               \
            chlsdl_notify_notification_show_new("chlsdl-r34", body,            \
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
} r34_info;

struct module g_libr34 = {
    r34_deinit,
    r34_func,
};

static const char * module_downloads_dir;

static int default_notification_timeout;

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

    default_notification_timeout = cdata->default_notification_timeout;

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

static void
r34_save_metadata(
    const char * metadata_file, r34_info info, json_object * post_info)
{
    json_object * obj = json_object_new_object();
    assert(obj);

    json_object_object_add(obj, "name", json_object_new_string(info.name));
    json_object_object_add(obj, "src", json_object_new_string(info.src));

    /* artist */
    json_object_object_add(
        obj, "artist", json_object_object_get(post_info, "artist"));

    /* copyright */
    json_object_object_add(
        obj, "copyright", json_object_object_get(post_info, "copyright"));

    /* character */
    json_object_object_add(
        obj, "character", json_object_object_get(post_info, "character"));

    /* general */
    json_object_object_add(
        obj, "general", json_object_object_get(post_info, "general"));

    /* meta */
    json_object_object_add(
        obj, "meta", json_object_object_get(post_info, "meta"));

    /* statistics */
    json_object * statistics = lambda({
        json_object * r     = json_object_new_object();
        json_object * jinfo = json_object_object_get(post_info, "information");
        assert(jinfo);

        json_object_object_add(r, "id", json_object_object_get(jinfo, "id"));
        json_object_object_add(
            r, "date", json_object_object_get(jinfo, "date"));

        json_object * source = json_object_object_get(jinfo, "source");
        json_object_object_add(
            r, "source", source ? source : json_object_new_array());

        json_object_object_add(
            r, "rating", json_object_object_get(jinfo, "rating"));
        r;
    });

    json_object_object_add(obj, "statistics", statistics);

    print_info("saving metadata to: '%s'\n", metadata_file);

    write_buffer_to_file(metadata_file, 0,
        json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PRETTY));

    json_object_put(obj);
}

static bool
verify_media(const struct curl_buffer * media, const char * file_name)
{
    char * md5 = strdup(file_name);
    assert(md5);
    {
        char * c = strrchr(md5, '.');
        assert(c);
        *c = '\0';
    }

    bool r = md5sum_verify(media->at, media->data, md5);
    free(md5);
    return r;
}

void
r34_func(void * vargp)
{
    chlsdl_defer char * orig_data = strdup(vargp);
    assert(orig_data);

    char * data = orig_data + strlen(get_line_from_string(orig_data));

    r34_info info;
    to_r34_info(&info, data);

    chlsdl_defer char * metadata_file
        = svconcat("%s/%s.json", module_downloads_dir, info.name);

    if (file_exists(metadata_file))
        return (void)MOD_PRINT_AND_NOTIFY(
            print_warn, "'%s' has already been downloaded", info.name);

    __chlsdl_defer(__curl_buffer_dealloc) struct curl_buffer * buf
        = curl_buffer_alloc(1024);

    MOD_PRINT_AND_NOTIFY(print_info, "downloading: '%s'", info.url);

    /* download post media */
    if (curl_request_get(info.url, buf) != CURLE_OK)
        return (void)MOD_ERROR_AND_NOTIFY(
            print_error, "failed to download: '%s'", info.url);

    /*
     * FIXME: video hashes for whatever reason don't always match the file's
     * name. no other choice but to skip checking their hashes entirely for now™
     */
    if (strcmp(strrchr(info.name, '.') + 1, "mp4") == 0)
        print_warn("FIXME: skipping hash verification for mp4 file.\n");
    else if (!verify_media(buf, info.name)) {
        MOD_ERROR_AND_NOTIFY(
            print_error, "md5 hash mismatch. not saving: '%s'", info.name);
        return;
    }

    char * out = svconcat("%s/%s", module_downloads_dir, info.name);
    assert(out);

    MOD_PRINT_AND_NOTIFY(print_info, "saving to: '%s'", out);
    write_buffer_to_file(out, buf->at, buf->data);

    json_object * post_info = json_tokener_parse(data);
    assert(post_info);
    r34_save_metadata(metadata_file, info, post_info);

    free(out);
}
