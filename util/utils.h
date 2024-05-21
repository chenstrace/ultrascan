#ifndef USCAN_UTIL_UTILS_H_
#define USCAN_UTIL_UTILS_H_
#include <arpa/inet.h>

#include "types.h"
#include <string>
#include <vector>

namespace ultrascan {
typedef intptr_t ngx_int_t;
typedef uintptr_t ngx_uint_t;

typedef struct {
    in_addr_t addr;
    in_addr_t mask;
} ngx_in_cidr_t;

typedef struct {
    struct in6_addr addr;
    struct in6_addr mask;
} ngx_in6_cidr_t;

typedef struct {
    ngx_uint_t family;
    union {
        ngx_in_cidr_t in;
        ngx_in6_cidr_t in6;
    } u;
} ngx_cidr_t;

#define NGX_OK 0
#define NGX_ERROR -1
#define NGX_DONE -4

ngx_int_t ngx_ptocidr(me_str_t* text, ngx_cidr_t* cidr);
u_char* strnstr(u_char* s1, size_t len1, const char* s2, size_t len2);

long safe_atol(const unsigned char* s, size_t n, char** errptr);
std::vector<std::string> split(const std::string& s, char delimiter);
}  // namespace ultrascan
#endif /* USCAN_UTIL_UTILS_H_ */
