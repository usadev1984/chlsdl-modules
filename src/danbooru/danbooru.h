#ifndef DANBOORU_H_
#define DANBOORU_H_

#include <chlsdl/module.h>

extern struct module g_libdanbooru;

extern const struct module *
danbooru_init(const struct chlsdl_data * cdata);
extern void
danbooru_deinit();

#endif // DANBOORU_H_
