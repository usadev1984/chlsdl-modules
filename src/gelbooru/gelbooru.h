#ifndef GELBOORU_H_
#define GELBOORU_H_

#include <chlsdl/module.h>

extern struct module g_libgelbooru;

extern const struct module *
gelbooru_init(const struct chlsdl_data * cdata);
extern void
gelbooru_deinit();
extern void
gelbooru_func(void * vargp);

#endif // GELBOORU_H_
