#include <lzo/lzoconf.h>
#include <lzo/lzo1x.h>
#include <string.h>
#include <stdlib.h>

int __lzo_init_v2(unsigned int v, int s1, int s2, int s3, int s4, int s5, int s6, int s7, int s8, int s9)
{
    (void)v; (void)s1; (void)s2; (void)s3; (void)s4; (void)s5; (void)s6; (void)s7; (void)s8; (void)s9;
    return LZO_E_OK;
}

int lzo1x_1_compress(const lzo_bytep in, lzo_uint in_len,
                     lzo_bytep out, lzo_uintp out_len,
                     lzo_voidp wrkmem)
{
    (void)wrkmem;
    if (in && out && out_len) {
        memcpy(out, in, in_len);
        *out_len = in_len;
    }
    return LZO_E_OK;
}

int lzo1x_decompress_safe(const lzo_bytep in, lzo_uint in_len,
                          lzo_bytep out, lzo_uintp out_len,
                          lzo_voidp wrkmem)
{
    (void)wrkmem;
    if (in && out && out_len) {
        memcpy(out, in, in_len);
        *out_len = in_len;
    }
    return LZO_E_OK;
}

/* OpenSSL 3.0 -> 1.1.1 compatibility wrapper */
void* SSL_get1_peer_certificate(void* ssl) {
    extern void* SSL_get_peer_certificate(void*);
    return SSL_get_peer_certificate(ssl);
}

/* GNU libstdc++ __cxx11 ABI compatibility stubs for DevIL (libIL.a) */
void _ZSt20__throw_length_errorPKc(const char* msg) { (void)msg; }
void _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj(void* self, void* capacity, unsigned int old_cap) { (void)self; (void)capacity; (void)old_cap; }
int _ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7compareEPKc(void* self, const char* s) { (void)self; (void)s; return 0; }
void* _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_assignERKS4_(void* self, const void* other) { (void)self; (void)other; return self; }
