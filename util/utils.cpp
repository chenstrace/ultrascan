#include "utils.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>


#include <sstream>


namespace ultrascan {
#define NGX_MAX_INT_T_VALUE 9223372036854775807

#define ngx_memset(buf, c, n) (void)memset(buf, c, n)
#define ngx_memzero(buf, n) (void)memset(buf, 0, n)

static inline u_char* ngx_strlchr(u_char* p, u_char* last, u_char c) {
    while (p < last) {
        if (*p == c) {
            return p;
        }

        p++;
    }

    return NULL;
}

in_addr_t ngx_inet_addr(u_char* text, size_t len) {
    u_char *p, c;
    in_addr_t addr;
    ngx_uint_t octet, n;

    addr = 0;
    octet = 0;
    n = 0;

    for (p = text; p < text + len; p++) {
        c = *p;

        if (c >= '0' && c <= '9') {
            octet = octet * 10 + (c - '0');

            if (octet > 255) {
                return INADDR_NONE;
            }

            continue;
        }

        if (c == '.') {
            addr = (addr << 8) + octet;
            octet = 0;
            n++;
            continue;
        }

        return INADDR_NONE;
    }

    if (n == 3) {
        addr = (addr << 8) + octet;
        return htonl(addr);
    }

    return INADDR_NONE;
}

ngx_int_t ngx_inet6_addr(u_char* p, size_t len, u_char* addr) {
    u_char c, *zero, *digit, *s, *d;
    size_t len4;
    ngx_uint_t n, nibbles, word;

    if (len == 0) {
        return NGX_ERROR;
    }

    zero = NULL;
    digit = NULL;
    len4 = 0;
    nibbles = 0;
    word = 0;
    n = 8;

    if (p[0] == ':') {
        p++;
        len--;
    }

    for (/* void */; len; len--) {
        c = *p++;

        if (c == ':') {
            if (nibbles) {
                digit = p;
                len4 = len;
                *addr++ = (u_char)(word >> 8);
                *addr++ = (u_char)(word & 0xff);

                if (--n) {
                    nibbles = 0;
                    word = 0;
                    continue;
                }

            } else {
                if (zero == NULL) {
                    digit = p;
                    len4 = len;
                    zero = addr;
                    continue;
                }
            }

            return NGX_ERROR;
        }

        if (c == '.' && nibbles) {
            if (n < 2 || digit == NULL) {
                return NGX_ERROR;
            }

            word = ngx_inet_addr(digit, len4 - 1);
            if (word == INADDR_NONE) {
                return NGX_ERROR;
            }

            word = ntohl(word);
            *addr++ = (u_char)((word >> 24) & 0xff);
            *addr++ = (u_char)((word >> 16) & 0xff);
            n--;
            break;
        }

        if (++nibbles > 4) {
            return NGX_ERROR;
        }

        if (c >= '0' && c <= '9') {
            word = word * 16 + (c - '0');
            continue;
        }

        c |= 0x20;

        if (c >= 'a' && c <= 'f') {
            word = word * 16 + (c - 'a') + 10;
            continue;
        }

        return NGX_ERROR;
    }

    if (nibbles == 0 && zero == NULL) {
        return NGX_ERROR;
    }

    *addr++ = (u_char)(word >> 8);
    *addr++ = (u_char)(word & 0xff);

    if (--n) {
        if (zero) {
            n *= 2;
            s = addr - 1;
            d = s + n;
            while (s >= zero) {
                *d-- = *s--;
            }
            ngx_memzero(zero, n);
            return NGX_OK;
        }

    } else {
        if (zero == NULL) {
            return NGX_OK;
        }
    }

    return NGX_ERROR;
}

ngx_int_t ngx_atoi(u_char* line, size_t n) {
    ngx_int_t value, cutoff, cutlim;

    if (n == 0) {
        return NGX_ERROR;
    }

    cutoff = NGX_MAX_INT_T_VALUE / 10;
    cutlim = NGX_MAX_INT_T_VALUE % 10;

    for (value = 0; n--; line++) {
        if (*line < '0' || *line > '9') {
            return NGX_ERROR;
        }

        if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
            return NGX_ERROR;
        }

        value = value * 10 + (*line - '0');
    }

    return value;
}

ngx_int_t ngx_ptocidr(me_str_t* text, ngx_cidr_t* cidr) {
    u_char *addr, *mask, *last;
    size_t len;
    ngx_int_t shift;
    ngx_int_t rc;
    ngx_uint_t s, i;

    addr = text->data;
    last = addr + text->len;

    mask = ngx_strlchr(addr, last, '/');
    len = (mask ? mask : last) - addr;

    cidr->u.in.addr = ngx_inet_addr(addr, len);

    if (cidr->u.in.addr != INADDR_NONE) {
        cidr->family = AF_INET;

        if (mask == NULL) {
            cidr->u.in.mask = 0xffffffff;
            return NGX_OK;
        }

    } else if (ngx_inet6_addr(addr, len, cidr->u.in6.addr.s6_addr) == NGX_OK) {
        cidr->family = AF_INET6;

        if (mask == NULL) {
            ngx_memset(cidr->u.in6.mask.s6_addr, 0xff, 16);
            return NGX_OK;
        }

    } else {
        return NGX_ERROR;
    }

    mask++;

    shift = ngx_atoi(mask, last - mask);
    if (shift == NGX_ERROR) {
        return NGX_ERROR;
    }

    switch (cidr->family) {
        case AF_INET6:
            if (shift > 128) {
                return NGX_ERROR;
            }

            addr = cidr->u.in6.addr.s6_addr;
            mask = cidr->u.in6.mask.s6_addr;
            rc = NGX_OK;

            for (i = 0; i < 16; i++) {
                s = (shift > 8) ? 8 : shift;
                shift -= s;

                mask[i] = (u_char)(0xffu << (8 - s));

                if (addr[i] != (addr[i] & mask[i])) {
                    rc = NGX_DONE;
                    addr[i] &= mask[i];
                }
            }

            return rc;

        default: /* AF_INET */
            if (shift > 32) {
                return NGX_ERROR;
            }

            if (shift) {
                cidr->u.in.mask =
                    htonl((uint32_t)(0xffffffffu << (32 - shift)));

            } else {
                /* x86 compilers use a shl instruction that shifts by modulo 32
                 */
                cidr->u.in.mask = 0;
            }

            if (cidr->u.in.addr == (cidr->u.in.addr & cidr->u.in.mask)) {
                return NGX_OK;
            }

            cidr->u.in.addr &= cidr->u.in.mask;

            return NGX_DONE;
    }
}

u_char* strnstr(u_char* s1, size_t len1, const char* s2, size_t len2) {
    u_char c1, c2;
    size_t n;

    c2 = *(u_char*)s2++;

    n = len2 - 1;

    do {
        do {
            if (len1-- == 0) {
                return NULL;
            }

            c1 = *s1++;

            if (c1 == 0) {
                return NULL;
            }

        } while (c1 != c2);

        if (n > len1) {
            return NULL;
        }

    } while (strncmp((const char*)s1, s2, n) != 0);

    return --s1;
}

long safe_atol(const unsigned char* s, size_t n, char** errptr) {
    const long max_cut_off = LONG_MAX / 10;
    const long max_cut_limit = LONG_MAX % 10;
    const long min_cut_off = LONG_MIN / 10;
    const long min_cut_limit = LONG_MIN % 10;
    *errptr = NULL;
    size_t i = 0;
    long res = 0;

    while (i < n) {
        if (isspace(s[i])) {
            i++;
        } else {
            break;
        }
    }
    if (i == n) {
        *errptr = strdup("No digits were found");
        return 0;
    }
    int sign = 1;

    if (s[i] == '-') {
        sign = -1;
        i++;
    } else if (s[i] == '+') {
        sign = 1;
        i++;
    }

    while (i < n) {
        if (isdigit(s[i])) {
            int cur = s[i] - '0';
            if (sign == 1) {
                if ((res > max_cut_off) ||
                    (res == max_cut_off && cur > max_cut_limit)) {
                    *errptr = strdup("erange max");
                    return LONG_MAX;
                }

            } else {
                if ((res < min_cut_off) ||
                    (res == min_cut_off && (-cur) < min_cut_limit)) {
                    *errptr = strdup("erange min");
                    return LONG_MIN;
                }
            }
            res = res * 10 + sign * cur;
        } else {
            *errptr = strdup("not digit");
            return 0;
        }
        i++;
    }

    return res;
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
}  // namespace ultrascan