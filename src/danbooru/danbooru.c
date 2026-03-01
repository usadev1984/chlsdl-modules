#include "danbooru.h"
#include "print.h"

#include <assert.h>
#include <chlsdl/common/common.h>
#include <chlsdl/common/util/util.h>
#include <chlsdl/module.h>
#include <stdlib.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>


struct module g_libdanbooru = {
    danbooru_deinit,
};

static const char * module_downloads_dir;

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
