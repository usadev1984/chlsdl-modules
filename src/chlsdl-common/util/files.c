#include <chlsdl-modules/chlsdl-common/print.h>
#include <chlsdl-modules/chlsdl-common/util/curl_request.h>
#include <chlsdl-modules/chlsdl-common/util/files.h>

#include <assert.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

EVP_MD_CTX *   g_md_ctx;
const EVP_MD * g_md5_type;

void
write_buffer_to_file(const char * f, size_t sz, const void * data)
{
    print_debug_warn("writing to: '%s'\n", f);
    FILE * fp = fopen(f, "w");
    assert(fp);

    fwrite(data, sizeof(char), sz > 0 ? sz : strlen(data), fp);

    fclose(fp);
}

struct curl_buffer *
read_file_to_buffer(const char * f)
{
    FILE * fp = fopen(f, "r");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    struct curl_buffer * buf = curl_buffer_alloc(len);
    assert(buf);
    buf->at = buf->size = len;

    fread(buf->data, sizeof(*buf->data), len, fp);

    fclose(fp);

    return buf;
}

bool
md5sum_verify(size_t n, const void * data, const char * md5)
{
    unsigned char sum[MD5_SUM_MAX];
    assert(EVP_Digest(data, n, sum, NULL, g_md5_type, NULL) == 1);

#ifdef DEBUG
    print_debug_warn("calculated md5sum (%d):\n", MD5_SUM_MAX);
    for (int i = 0; i < sizeof(sum); ++i)
        printf("%02x", sum[i]);
    print_debug_warn("\n");
#endif

    unsigned char hex[MD5_SUM_MAX];
    assert(OPENSSL_hexstr2buf_ex(hex, sizeof(hex), NULL, md5, '\0') == 1);

#ifdef DEBUG
    printf("buf2hexstr:\n");
    for (int i = 0; i < sizeof(hex); ++i)
        printf("%02x ", hex[i]);
    printf("\n");
#endif

    return memcmp(sum, hex, sizeof(sum)) == 0;
}
