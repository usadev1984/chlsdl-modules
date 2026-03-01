#include "danbooru.h"
#include "print.h"

#include <chlsdl/common/common.h>
#include <chlsdl/module.h>

struct module g_libdanbooru = {
    danbooru_deinit,
};

const struct module *
danbooru_init(const struct chlsdl_data * cdata)
{
    print_debug_warn("initalizing danbooru...\n");
    return &g_libdanbooru;
}

void
danbooru_deinit()
{
    print_debug_warn("cleaning up danbooru...\n");
}
