#include <chlsdl-modules/chlsdl-common/common.h>
#include <openssl/evp.h>

extern EVP_MD_CTX *   g_md_ctx; /* util/files.c */
extern const EVP_MD * g_md5_type; /* util/files.c */

const struct version *
get_libchlsdl_common_version()
{
    static const struct version version
        = { CHLSDL_VERSION, CHLSDL_MAJOR, CHLSDL_MINOR, CHLSDL_PATCH };
    return &version;
}

bool
chlsdl_common_init()
{
    g_md_ctx = EVP_MD_CTX_new();
    if (!g_md_ctx)
        return false;

    OpenSSL_add_all_digests();
    g_md5_type = EVP_get_digestbyname("md5");
    return true;
}

void
chlsdl_common_deinit()
{
    EVP_cleanup(); /* for `OpenSSL_add_all_digests()` */
    EVP_MD_CTX_free(g_md_ctx);
}
