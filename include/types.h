#ifndef USCAN_INCLUDE_TYPES_H_
#define USCAN_INCLUDE_TYPES_H_

#include <stddef.h>
#include <stdint.h>

enum match_opt_t {
    OPT_NONE = 0,
    OPT_MATCH = 1 << 0,
    OPT_INCLUDE = 1 << 1,
    OPT_EQ = 1 << 2,
    OPT_LEN_EQ = 1 << 3,
    OPT_LEN_NOT_EQ = 1 << 4,
    OPT_LEN_GT = 1 << 5,
    OPT_LEN_LT = 1 << 6,
    OPT_NOT_INCLUDE = 1 << 7,
    OPT_IP_IN_LIST = 1 << 8,
    OPT_IP_BELONG = 1 << 9,

    OPT_NOT_MATCH = 1 << 10,

    OPT_NUM_EQ = 1 << 11,
    OPT_NUM_NOT_EQ = 1 << 12,
    OPT_NUM_GT = 1 << 13,
    OPT_NUM_LT = 1 << 14,
    OPT_IP_NOT_IN_LIST = 1 << 15,
    OPT_IP_NOT_BELONG = 1 << 16,
};
typedef enum match_opt_t match_opt_t;

typedef struct {
    size_t index;  // 0,1,2, ...
    const char* name;
    uint32_t opt_mask;
} field_item_t;

typedef struct {
    size_t len;
    unsigned char* data;
} me_str_t;

typedef struct {
    size_t index;  // 0,1,2, ...
    const char* name;
    me_str_t value;
} input_item_t;

typedef int (*rule_match_callback)(const char* name, const char* tag,
                                   const unsigned char* payload,
                                   int payload_len, void* user_data);

#endif /* USCAN_INCLUDE_TYPES_H_ */
