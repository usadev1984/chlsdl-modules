#ifndef COMMON_H_
#define COMMON_H_

struct version {
    const char * version;
    int          major;
    int          minor;
    int          patch;
};

extern const struct version *
get_libchlsdl_common_version();

extern bool
chlsdl_common_init();

extern void
chlsdl_common_deinit();

#endif // COMMON_H_
