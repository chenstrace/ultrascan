#ifndef USCAN_DEMO_TEST_DATA_H_
#define USCAN_DEMO_TEST_DATA_H_
#include <stddef.h>

#include "types.h"
typedef struct {
    const char* field_name;
    match_opt_t opt;
    const char* pattern;
} condition_info_t;

typedef condition_info_t* rule_info_t;
#define EMPTY_CONDITION \
    { NULL, OPT_NONE, NULL }

field_item_t field_list[] = {
    {4, "BODY",
     OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ | OPT_LEN_GT |
         OPT_LEN_LT | OPT_NOT_INCLUDE | OPT_NOT_MATCH},
    {0, "IP", OPT_IP_IN_LIST | OPT_IP_BELONG},
    {1, "URL",
     OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ | OPT_LEN_GT |
         OPT_LEN_LT | OPT_NOT_INCLUDE | OPT_NOT_MATCH},
    {2, "ARGS",
     OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ | OPT_LEN_GT |
         OPT_LEN_LT | OPT_NOT_INCLUDE | OPT_NOT_MATCH},
    {3, "HEADER",
     OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
         OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE | OPT_NOT_MATCH},

};

condition_info_t g_r1[] = {{"HEADER", OPT_MATCH, "android111"},
                           {"URL", OPT_EQ, "/test"},
                           {"URL", OPT_LEN_EQ, "5"},
                           {"BODY", OPT_INCLUDE, "def"},
                           EMPTY_CONDITION};
condition_info_t g_r2[] = {{"HEADER", OPT_MATCH, "android111"},
                           {"URL", OPT_EQ, "/test"},
                           {"URL", OPT_LEN_EQ, "5"},
                           {"URL", OPT_LEN_GT, "3"},
                           {"BODY", OPT_INCLUDE, "abc"},
                           EMPTY_CONDITION};
condition_info_t g_r3[] = {{"HEADER", OPT_MATCH, "android1"}, EMPTY_CONDITION};
condition_info_t g_r4[] = {{"HEADER", OPT_MATCH, "^((?!badword).)*$"},
                           EMPTY_CONDITION};

condition_info_t g_r5[] = {{"HEADER", OPT_MATCH, "android111"},
                           EMPTY_CONDITION};

condition_info_t g_r6[] = {
    {"ARGS", OPT_NOT_MATCH, "^foo"}, {"URL", OPT_EQ, "/test"}, EMPTY_CONDITION};

rule_info_t g_rules_info[] = {g_r1, g_r2, g_r3, g_r4, g_r5, g_r6};

#define me_string(str) \
    { sizeof(str) - 1, (unsigned char*)str }
input_item_t g_input[] = {
    {4, "BODY", me_string("abcdef")},
    {1, "URL", me_string("/test")},
    {2, "ARGS", me_string("ffoo")},
    {3, "HEADER", me_string("prefix-android1111")},
    {0, "IP", me_string("192.168.55.3")},
};

const int g_rules_count = sizeof(g_rules_info) / sizeof(g_rules_info[0]);
const int g_input_count = sizeof(g_input) / sizeof(g_input[0]);

const int field_list_size = sizeof(field_list) / sizeof(field_list[0]);

#endif /* USCAN_DEMO_TEST_DATA_H_ */
