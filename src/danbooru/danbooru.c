#include "danbooru.h"

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

static pcre2_code * g_whitespace_pattern;

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
    info->src = json_object_get_string(json_object_object_get(jdata, "source"));

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

static void
danbooru_load_taglist_to_array(json_object * out, const char * out_key,
    json_object * post_info, const char * taglist_key)
{
    json_object * arr     = json_object_new_array();
    char *        taglist = (char *)json_object_get_string(
        json_object_object_get(post_info, taglist_key));
    const char * ptr;
    while ((ptr = strsep(&taglist, " ")))
        json_object_array_add(arr, json_object_new_string(ptr));
    json_object_object_add(out, out_key, arr);
}

static void
danbooru_save_metadata(const char * metadata_file, danbooru_info info,
    json_object * post_info, json_object * commentary)
{
    json_object * obj = json_object_new_object();
    assert(obj);

    json_object_object_add(obj, "name", json_object_new_string(info.name));
    json_object_object_add(obj, "src", json_object_new_string(info.src));

    /* artist */
    danbooru_load_taglist_to_array(
        obj, "artist", post_info, "tag_string_artist");

    /* copyright */
    danbooru_load_taglist_to_array(
        obj, "copyright", post_info, "tag_string_copyright");

    /* character */
    danbooru_load_taglist_to_array(
        obj, "character", post_info, "tag_string_character");

    /* general */
    danbooru_load_taglist_to_array(
        obj, "general", post_info, "tag_string_general");

    /* meta */
    danbooru_load_taglist_to_array(obj, "meta", post_info, "tag_string_meta");

    /* information */
    json_object * information = json_object_new_object();

    json_object_object_add(information, "id",
        json_object_new_string(info.taglists.information[0]));
    json_object_object_add(information, "date",
        json_object_new_string(info.taglists.information[1]));
    json_object_object_add(information, "source",
        json_object_new_string(info.taglists.information[2]));
    json_object_object_add(information, "rating",
        json_object_new_string(info.taglists.information[3]));

    json_object_object_add(obj, "information", information);

    /* artist commentary */
    json_object * artist_commentary = lambda({
        json_object * r = json_object_new_object();

        json_object * title_obj
            = json_object_object_get(commentary, "original_title");
        const char * title = json_object_get_string(title_obj);

        json_object * desc_obj
            = json_object_object_get(commentary, "original_description");
        const char * old_desc = json_object_get_string(desc_obj);

        if (!title_obj)
            title_obj = json_object_new_string("");
        if (!desc_obj) {
            desc_obj = json_object_new_string("");
            goto fill_commentary;
        }

        /* replace '\r\n' with '\n' */
        PCRE2_SIZE desc_size = strlen(old_desc) + 1;
        char *     desc      = malloc(desc_size);
        assert(desc);
        assert(
            pcre2_substitute(g_whitespace_pattern, (PCRE2_SPTR8)old_desc,
                PCRE2_ZERO_TERMINATED, 0,
                PCRE2_SUBSTITUTE_GLOBAL | PCRE2_SUBSTITUTE_EXTENDED, NULL, NULL,
                (PCRE2_SPTR8) "\n", PCRE2_ZERO_TERMINATED, desc, &desc_size)
            >= 0); /* NOTE: no idea what `PCRE2_SUBSTITUTE_EXTENDED` actually
                      does */
        desc_obj = json_object_new_string(desc);

    fill_commentary:
        json_object_object_add(r, "original_title", title_obj);
        json_object_object_add(r, "original_description", desc_obj);

        r;
    });

    json_object_object_add(obj, "artist_commentary", artist_commentary);

    /* extra shit */
    json_object * extra = lambda({
        json_object * r = json_object_new_object();
        if (info.parent_id)
            json_object_object_add(
                r, "parent_id", json_object_new_string(info.parent_id));
        else
            json_object_object_add(r, "parent_id", json_object_new_null());

        if (!info.taglists.children)
            json_object_object_add(r, "children", json_object_new_null());
        else {
            json_object * arr
                = json_object_new_array_ext(info.taglists.nchildren);
            for (int i = 0; i < info.taglists.nchildren; ++i)
                json_object_array_add(
                    arr, json_object_new_string(info.taglists.children[i]));
            json_object_object_add(r, "children", arr);
        }

        if (!info.taglists.siblings)
            json_object_object_add(r, "siblings", json_object_new_null());
        else {
            json_object * arr
                = json_object_new_array_ext(info.taglists.nsiblings);
            for (int i = 0; i < info.taglists.nsiblings; ++i)
                json_object_array_add(
                    arr, json_object_new_string(info.taglists.siblings[i]));
            json_object_object_add(r, "siblings", arr);
        }
        r;
    });

    json_object_object_add(obj, "_", extra);

    print_info("saving metadata to: '%s'\n", metadata_file);

    write_buffer_to_file(metadata_file, 0,
        json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PRETTY));

    json_object_put(obj);
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

    g_whitespace_pattern = pcre2_compile((PCRE2_SPTR8) "\r\n",
        PCRE2_ZERO_TERMINATED, 0, &(int) { 0 }, &(PCRE2_SIZE) { 0 }, NULL);
    assert(g_whitespace_pattern);

    return &g_libdanbooru;
}

void
danbooru_deinit()
{
    print_debug_warn("cleaning up danbooru...\n");
    free(g_whitespace_pattern);
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

    chlsdl_defer char * metadata_file
        = svconcat("%s/%s.json", module_downloads_dir, info.name);

    if (file_exists(metadata_file)) {
        print_warn("'%s' has already been downloaded\n", info.name);
        return;
    }

    /* download artist commentary json */
    chlsdl_defer char * commentary_url
        = svconcat("https://danbooru.donmai.us/posts/%s/artist_commentary.json",
            info.taglists.information[0]);
    assert(commentary_url);

    __chlsdl_defer(__curl_buffer_dealloc) struct curl_buffer * buf
        = curl_buffer_alloc(1024);
    if (curl_request_get(commentary_url, buf) != CURLE_OK) {
        print_error(
            "failed to download artist commentary: '%s'\n", commentary_url);
        return;
    }

    /* parse it */
    json_object * commentary = json_tokener_parse(buf->data);
    if (!commentary) {
        print_error("failed to parse artist commentary\n");
        return;
    }

    /* download post info json */
    chlsdl_defer char * post_info_url
        = svconcat("https://danbooru.donmai.us/posts/%s.json",
            info.taglists.information[0]);
    assert(post_info_url);

    print_info("downloading post info: '%s'\n", post_info_url);

    buf->at = 0; /* re-use buffer */
    if (curl_request_get(post_info_url, buf) != CURLE_OK) {
        print_error("failed to download post info: '%s'\n", post_info_url);
        return;
    }

    /* parse it */
    json_object * post_info = json_tokener_parse(buf->data);
    if (!post_info) {
        print_error("failed to parse post info\n");
        return;
    }

    print_info("downloading: '%s'\n", info.url);

    /* download post media */
    buf->at = 0; /* re-use buffer */
    if (curl_request_get(info.url, buf) != CURLE_OK) {
        print_error("failed to download: '%s'\n", info.url);
        return;
    }

    char * out = svconcat("%s/%s", module_downloads_dir, info.name);
    assert(out);
    /* TODO: check md5sum */
    print_info("saving to: '%s'\n", out);
    write_buffer_to_file(out, buf->at, buf->data);

    danbooru_save_metadata(metadata_file, info, post_info, commentary);

    free(out);
}
