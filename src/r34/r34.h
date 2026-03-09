#ifndef R34_H_
#define R34_H_

#include <chlsdl/module.h>

extern struct module g_libr34;

extern const struct module *
r34_init(const struct chlsdl_data * cdata);
extern void
r34_deinit();
extern void
r34_func(void * vargp);

#endif // R34_H_
