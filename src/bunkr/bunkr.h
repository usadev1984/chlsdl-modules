#ifndef BUNKR_H_
#define BUNKR_H_

#include <chlsdl/module.h>

extern struct module g_libbunkr;

extern const struct module *
bunkr_init(const struct chlsdl_data * cdata);
extern void
bunkr_deinit();
extern void
bunkr_func(void * vargp);

#endif // BUNKR_H_
