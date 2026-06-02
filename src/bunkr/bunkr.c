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

struct module g_libbunkr = {
    bunkr_deinit,
    bunkr_func,
};

static const char * module_downloads_dir;

static int default_notification_timeout;

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

void
bunkr_func(void * vargp)
{
    chlsdl_defer char * orig_data = strdup(vargp);
    assert(orig_data);
}
