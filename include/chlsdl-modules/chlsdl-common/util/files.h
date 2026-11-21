#ifndef FILES_H_
#define FILES_H_

#include <chlsdl-modules/chlsdl-common/macros.h>
#include <string.h>
#include <unistd.h>

#define MD5_SUM_MAX (16)

#define mymkdir(path, ...) mymkdir_args(path, 0 __VA_OPT__(+__VA_ARGS__))

extern void
write_buffer_to_file(const char * f, size_t sz, const void * data);

extern struct curl_buffer *
read_file_to_buffer(const char * f);

CHLSDL_ALWAYS_INLINE inline bool
file_exists(const char * path)
{
    return access(path, F_OK) == 0;
}

extern bool
md5sum_verify(size_t n, const void * data, const char * md5);

/*
 * `mkdir -p`
 */
extern bool
mymkdir_args(const char * path, int mode);

#endif // FILES_H_
