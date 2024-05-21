#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "c.h"
typedef struct {
    std::string name;
    std::string tag;
} rule_match_info_t;

typedef struct {
    const char* field_name;
    match_opt_t opt;
    const char* pattern;
} single_condition_data_t;

const std::vector<std::vector<int>> tests = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9}, {1, 2, 3, 4, 5, 7, 6, 8, 9},
    {1, 2, 3, 4, 5, 8, 6, 7, 9}, {1, 2, 3, 4, 5, 9, 6, 7, 8},
    {1, 2, 3, 4, 6, 7, 5, 8, 9}, {1, 2, 3, 4, 6, 8, 5, 7, 9},
    {1, 2, 3, 4, 6, 9, 5, 7, 8}, {1, 2, 3, 4, 7, 8, 5, 6, 9},
    {1, 2, 3, 4, 7, 9, 5, 6, 8}, {1, 2, 3, 4, 8, 9, 5, 6, 7},
    {1, 2, 4, 3, 5, 6, 7, 8, 9}, {1, 2, 4, 3, 5, 7, 6, 8, 9},
    {1, 2, 4, 3, 5, 8, 6, 7, 9}, {1, 2, 4, 3, 5, 9, 6, 7, 8},
    {1, 2, 4, 3, 6, 7, 5, 8, 9}, {1, 2, 4, 3, 6, 8, 5, 7, 9},
    {1, 2, 4, 3, 6, 9, 5, 7, 8}, {1, 2, 4, 3, 7, 8, 5, 6, 9},
    {1, 2, 4, 3, 7, 9, 5, 6, 8}, {1, 2, 4, 3, 8, 9, 5, 6, 7},
    {1, 2, 5, 3, 4, 6, 7, 8, 9}, {1, 2, 5, 3, 4, 7, 6, 8, 9},
    {1, 2, 5, 3, 4, 8, 6, 7, 9}, {1, 2, 5, 3, 4, 9, 6, 7, 8},
    {1, 2, 5, 3, 6, 7, 4, 8, 9}, {1, 2, 5, 3, 6, 8, 4, 7, 9},
    {1, 2, 5, 3, 6, 9, 4, 7, 8}, {1, 2, 5, 3, 7, 8, 4, 6, 9},
    {1, 2, 5, 3, 7, 9, 4, 6, 8}, {1, 2, 5, 3, 8, 9, 4, 6, 7},
    {1, 2, 6, 3, 4, 5, 7, 8, 9}, {1, 2, 6, 3, 4, 7, 5, 8, 9},
    {1, 2, 6, 3, 4, 8, 5, 7, 9}, {1, 2, 6, 3, 4, 9, 5, 7, 8},
    {1, 2, 6, 3, 5, 7, 4, 8, 9}, {1, 2, 6, 3, 5, 8, 4, 7, 9},
    {1, 2, 6, 3, 5, 9, 4, 7, 8}, {1, 2, 6, 3, 7, 8, 4, 5, 9},
    {1, 2, 6, 3, 7, 9, 4, 5, 8}, {1, 2, 6, 3, 8, 9, 4, 5, 7},
    {1, 2, 7, 3, 4, 5, 6, 8, 9}, {1, 2, 7, 3, 4, 6, 5, 8, 9},
    {1, 2, 7, 3, 4, 8, 5, 6, 9}, {1, 2, 7, 3, 4, 9, 5, 6, 8},
    {1, 2, 7, 3, 5, 6, 4, 8, 9}, {1, 2, 7, 3, 5, 8, 4, 6, 9},
    {1, 2, 7, 3, 5, 9, 4, 6, 8}, {1, 2, 7, 3, 6, 8, 4, 5, 9},
    {1, 2, 7, 3, 6, 9, 4, 5, 8}, {1, 2, 7, 3, 8, 9, 4, 5, 6},
    {1, 2, 8, 3, 4, 5, 6, 7, 9}, {1, 2, 8, 3, 4, 6, 5, 7, 9},
    {1, 2, 8, 3, 4, 7, 5, 6, 9}, {1, 2, 8, 3, 4, 9, 5, 6, 7},
    {1, 2, 8, 3, 5, 6, 4, 7, 9}, {1, 2, 8, 3, 5, 7, 4, 6, 9},
    {1, 2, 8, 3, 5, 9, 4, 6, 7}, {1, 2, 8, 3, 6, 7, 4, 5, 9},
    {1, 2, 8, 3, 6, 9, 4, 5, 7}, {1, 2, 8, 3, 7, 9, 4, 5, 6},
    {1, 2, 9, 3, 4, 5, 6, 7, 8}, {1, 2, 9, 3, 4, 6, 5, 7, 8},
    {1, 2, 9, 3, 4, 7, 5, 6, 8}, {1, 2, 9, 3, 4, 8, 5, 6, 7},
    {1, 2, 9, 3, 5, 6, 4, 7, 8}, {1, 2, 9, 3, 5, 7, 4, 6, 8},
    {1, 2, 9, 3, 5, 8, 4, 6, 7}, {1, 2, 9, 3, 6, 7, 4, 5, 8},
    {1, 2, 9, 3, 6, 8, 4, 5, 7}, {1, 2, 9, 3, 7, 8, 4, 5, 6},
    {1, 3, 4, 2, 5, 6, 7, 8, 9}, {1, 3, 4, 2, 5, 7, 6, 8, 9},
    {1, 3, 4, 2, 5, 8, 6, 7, 9}, {1, 3, 4, 2, 5, 9, 6, 7, 8},
    {1, 3, 4, 2, 6, 7, 5, 8, 9}, {1, 3, 4, 2, 6, 8, 5, 7, 9},
    {1, 3, 4, 2, 6, 9, 5, 7, 8}, {1, 3, 4, 2, 7, 8, 5, 6, 9},
    {1, 3, 4, 2, 7, 9, 5, 6, 8}, {1, 3, 4, 2, 8, 9, 5, 6, 7},
    {1, 3, 5, 2, 4, 6, 7, 8, 9}, {1, 3, 5, 2, 4, 7, 6, 8, 9},
    {1, 3, 5, 2, 4, 8, 6, 7, 9}, {1, 3, 5, 2, 4, 9, 6, 7, 8},
    {1, 3, 5, 2, 6, 7, 4, 8, 9}, {1, 3, 5, 2, 6, 8, 4, 7, 9},
    {1, 3, 5, 2, 6, 9, 4, 7, 8}, {1, 3, 5, 2, 7, 8, 4, 6, 9},
    {1, 3, 5, 2, 7, 9, 4, 6, 8}, {1, 3, 5, 2, 8, 9, 4, 6, 7},
    {1, 3, 6, 2, 4, 5, 7, 8, 9}, {1, 3, 6, 2, 4, 7, 5, 8, 9},
    {1, 3, 6, 2, 4, 8, 5, 7, 9}, {1, 3, 6, 2, 4, 9, 5, 7, 8},
    {1, 3, 6, 2, 5, 7, 4, 8, 9}, {1, 3, 6, 2, 5, 8, 4, 7, 9},
    {1, 3, 6, 2, 5, 9, 4, 7, 8}, {1, 3, 6, 2, 7, 8, 4, 5, 9},
    {1, 3, 6, 2, 7, 9, 4, 5, 8}, {1, 3, 6, 2, 8, 9, 4, 5, 7},
    {1, 3, 7, 2, 4, 5, 6, 8, 9}, {1, 3, 7, 2, 4, 6, 5, 8, 9},
    {1, 3, 7, 2, 4, 8, 5, 6, 9}, {1, 3, 7, 2, 4, 9, 5, 6, 8},
    {1, 3, 7, 2, 5, 6, 4, 8, 9}, {1, 3, 7, 2, 5, 8, 4, 6, 9},
    {1, 3, 7, 2, 5, 9, 4, 6, 8}, {1, 3, 7, 2, 6, 8, 4, 5, 9},
    {1, 3, 7, 2, 6, 9, 4, 5, 8}, {1, 3, 7, 2, 8, 9, 4, 5, 6},
    {1, 3, 8, 2, 4, 5, 6, 7, 9}, {1, 3, 8, 2, 4, 6, 5, 7, 9},
    {1, 3, 8, 2, 4, 7, 5, 6, 9}, {1, 3, 8, 2, 4, 9, 5, 6, 7},
    {1, 3, 8, 2, 5, 6, 4, 7, 9}, {1, 3, 8, 2, 5, 7, 4, 6, 9},
    {1, 3, 8, 2, 5, 9, 4, 6, 7}, {1, 3, 8, 2, 6, 7, 4, 5, 9},
    {1, 3, 8, 2, 6, 9, 4, 5, 7}, {1, 3, 8, 2, 7, 9, 4, 5, 6},
    {1, 3, 9, 2, 4, 5, 6, 7, 8}, {1, 3, 9, 2, 4, 6, 5, 7, 8},
    {1, 3, 9, 2, 4, 7, 5, 6, 8}, {1, 3, 9, 2, 4, 8, 5, 6, 7},
    {1, 3, 9, 2, 5, 6, 4, 7, 8}, {1, 3, 9, 2, 5, 7, 4, 6, 8},
    {1, 3, 9, 2, 5, 8, 4, 6, 7}, {1, 3, 9, 2, 6, 7, 4, 5, 8},
    {1, 3, 9, 2, 6, 8, 4, 5, 7}, {1, 3, 9, 2, 7, 8, 4, 5, 6},
    {1, 4, 5, 2, 3, 6, 7, 8, 9}, {1, 4, 5, 2, 3, 7, 6, 8, 9},
    {1, 4, 5, 2, 3, 8, 6, 7, 9}, {1, 4, 5, 2, 3, 9, 6, 7, 8},
    {1, 4, 5, 2, 6, 7, 3, 8, 9}, {1, 4, 5, 2, 6, 8, 3, 7, 9},
    {1, 4, 5, 2, 6, 9, 3, 7, 8}, {1, 4, 5, 2, 7, 8, 3, 6, 9},
    {1, 4, 5, 2, 7, 9, 3, 6, 8}, {1, 4, 5, 2, 8, 9, 3, 6, 7},
    {1, 4, 6, 2, 3, 5, 7, 8, 9}, {1, 4, 6, 2, 3, 7, 5, 8, 9},
    {1, 4, 6, 2, 3, 8, 5, 7, 9}, {1, 4, 6, 2, 3, 9, 5, 7, 8},
    {1, 4, 6, 2, 5, 7, 3, 8, 9}, {1, 4, 6, 2, 5, 8, 3, 7, 9},
    {1, 4, 6, 2, 5, 9, 3, 7, 8}, {1, 4, 6, 2, 7, 8, 3, 5, 9},
    {1, 4, 6, 2, 7, 9, 3, 5, 8}, {1, 4, 6, 2, 8, 9, 3, 5, 7},
    {1, 4, 7, 2, 3, 5, 6, 8, 9}, {1, 4, 7, 2, 3, 6, 5, 8, 9},
    {1, 4, 7, 2, 3, 8, 5, 6, 9}, {1, 4, 7, 2, 3, 9, 5, 6, 8},
    {1, 4, 7, 2, 5, 6, 3, 8, 9}, {1, 4, 7, 2, 5, 8, 3, 6, 9},
    {1, 4, 7, 2, 5, 9, 3, 6, 8}, {1, 4, 7, 2, 6, 8, 3, 5, 9},
    {1, 4, 7, 2, 6, 9, 3, 5, 8}, {1, 4, 7, 2, 8, 9, 3, 5, 6},
    {1, 4, 8, 2, 3, 5, 6, 7, 9}, {1, 4, 8, 2, 3, 6, 5, 7, 9},
    {1, 4, 8, 2, 3, 7, 5, 6, 9}, {1, 4, 8, 2, 3, 9, 5, 6, 7},
    {1, 4, 8, 2, 5, 6, 3, 7, 9}, {1, 4, 8, 2, 5, 7, 3, 6, 9},
    {1, 4, 8, 2, 5, 9, 3, 6, 7}, {1, 4, 8, 2, 6, 7, 3, 5, 9},
    {1, 4, 8, 2, 6, 9, 3, 5, 7}, {1, 4, 8, 2, 7, 9, 3, 5, 6},
    {1, 4, 9, 2, 3, 5, 6, 7, 8}, {1, 4, 9, 2, 3, 6, 5, 7, 8},
    {1, 4, 9, 2, 3, 7, 5, 6, 8}, {1, 4, 9, 2, 3, 8, 5, 6, 7},
    {1, 4, 9, 2, 5, 6, 3, 7, 8}, {1, 4, 9, 2, 5, 7, 3, 6, 8},
    {1, 4, 9, 2, 5, 8, 3, 6, 7}, {1, 4, 9, 2, 6, 7, 3, 5, 8},
    {1, 4, 9, 2, 6, 8, 3, 5, 7}, {1, 4, 9, 2, 7, 8, 3, 5, 6},
    {1, 5, 6, 2, 3, 4, 7, 8, 9}, {1, 5, 6, 2, 3, 7, 4, 8, 9},
    {1, 5, 6, 2, 3, 8, 4, 7, 9}, {1, 5, 6, 2, 3, 9, 4, 7, 8},
    {1, 5, 6, 2, 4, 7, 3, 8, 9}, {1, 5, 6, 2, 4, 8, 3, 7, 9},
    {1, 5, 6, 2, 4, 9, 3, 7, 8}, {1, 5, 6, 2, 7, 8, 3, 4, 9},
    {1, 5, 6, 2, 7, 9, 3, 4, 8}, {1, 5, 6, 2, 8, 9, 3, 4, 7},
    {1, 5, 7, 2, 3, 4, 6, 8, 9}, {1, 5, 7, 2, 3, 6, 4, 8, 9},
    {1, 5, 7, 2, 3, 8, 4, 6, 9}, {1, 5, 7, 2, 3, 9, 4, 6, 8},
    {1, 5, 7, 2, 4, 6, 3, 8, 9}, {1, 5, 7, 2, 4, 8, 3, 6, 9},
    {1, 5, 7, 2, 4, 9, 3, 6, 8}, {1, 5, 7, 2, 6, 8, 3, 4, 9},
    {1, 5, 7, 2, 6, 9, 3, 4, 8}, {1, 5, 7, 2, 8, 9, 3, 4, 6},
    {1, 5, 8, 2, 3, 4, 6, 7, 9}, {1, 5, 8, 2, 3, 6, 4, 7, 9},
    {1, 5, 8, 2, 3, 7, 4, 6, 9}, {1, 5, 8, 2, 3, 9, 4, 6, 7},
    {1, 5, 8, 2, 4, 6, 3, 7, 9}, {1, 5, 8, 2, 4, 7, 3, 6, 9},
    {1, 5, 8, 2, 4, 9, 3, 6, 7}, {1, 5, 8, 2, 6, 7, 3, 4, 9},
    {1, 5, 8, 2, 6, 9, 3, 4, 7}, {1, 5, 8, 2, 7, 9, 3, 4, 6},
    {1, 5, 9, 2, 3, 4, 6, 7, 8}, {1, 5, 9, 2, 3, 6, 4, 7, 8},
    {1, 5, 9, 2, 3, 7, 4, 6, 8}, {1, 5, 9, 2, 3, 8, 4, 6, 7},
    {1, 5, 9, 2, 4, 6, 3, 7, 8}, {1, 5, 9, 2, 4, 7, 3, 6, 8},
    {1, 5, 9, 2, 4, 8, 3, 6, 7}, {1, 5, 9, 2, 6, 7, 3, 4, 8},
    {1, 5, 9, 2, 6, 8, 3, 4, 7}, {1, 5, 9, 2, 7, 8, 3, 4, 6},
    {1, 6, 7, 2, 3, 4, 5, 8, 9}, {1, 6, 7, 2, 3, 5, 4, 8, 9},
    {1, 6, 7, 2, 3, 8, 4, 5, 9}, {1, 6, 7, 2, 3, 9, 4, 5, 8},
    {1, 6, 7, 2, 4, 5, 3, 8, 9}, {1, 6, 7, 2, 4, 8, 3, 5, 9},
    {1, 6, 7, 2, 4, 9, 3, 5, 8}, {1, 6, 7, 2, 5, 8, 3, 4, 9},
    {1, 6, 7, 2, 5, 9, 3, 4, 8}, {1, 6, 7, 2, 8, 9, 3, 4, 5},
    {1, 6, 8, 2, 3, 4, 5, 7, 9}, {1, 6, 8, 2, 3, 5, 4, 7, 9},
    {1, 6, 8, 2, 3, 7, 4, 5, 9}, {1, 6, 8, 2, 3, 9, 4, 5, 7},
    {1, 6, 8, 2, 4, 5, 3, 7, 9}, {1, 6, 8, 2, 4, 7, 3, 5, 9},
    {1, 6, 8, 2, 4, 9, 3, 5, 7}, {1, 6, 8, 2, 5, 7, 3, 4, 9},
    {1, 6, 8, 2, 5, 9, 3, 4, 7}, {1, 6, 8, 2, 7, 9, 3, 4, 5},
    {1, 6, 9, 2, 3, 4, 5, 7, 8}, {1, 6, 9, 2, 3, 5, 4, 7, 8},
    {1, 6, 9, 2, 3, 7, 4, 5, 8}, {1, 6, 9, 2, 3, 8, 4, 5, 7},
    {1, 6, 9, 2, 4, 5, 3, 7, 8}, {1, 6, 9, 2, 4, 7, 3, 5, 8},
    {1, 6, 9, 2, 4, 8, 3, 5, 7}, {1, 6, 9, 2, 5, 7, 3, 4, 8},
    {1, 6, 9, 2, 5, 8, 3, 4, 7}, {1, 6, 9, 2, 7, 8, 3, 4, 5},
    {1, 7, 8, 2, 3, 4, 5, 6, 9}, {1, 7, 8, 2, 3, 5, 4, 6, 9},
    {1, 7, 8, 2, 3, 6, 4, 5, 9}, {1, 7, 8, 2, 3, 9, 4, 5, 6},
    {1, 7, 8, 2, 4, 5, 3, 6, 9}, {1, 7, 8, 2, 4, 6, 3, 5, 9},
    {1, 7, 8, 2, 4, 9, 3, 5, 6}, {1, 7, 8, 2, 5, 6, 3, 4, 9},
    {1, 7, 8, 2, 5, 9, 3, 4, 6}, {1, 7, 8, 2, 6, 9, 3, 4, 5},
    {1, 7, 9, 2, 3, 4, 5, 6, 8}, {1, 7, 9, 2, 3, 5, 4, 6, 8},
    {1, 7, 9, 2, 3, 6, 4, 5, 8}, {1, 7, 9, 2, 3, 8, 4, 5, 6},
    {1, 7, 9, 2, 4, 5, 3, 6, 8}, {1, 7, 9, 2, 4, 6, 3, 5, 8},
    {1, 7, 9, 2, 4, 8, 3, 5, 6}, {1, 7, 9, 2, 5, 6, 3, 4, 8},
    {1, 7, 9, 2, 5, 8, 3, 4, 6}, {1, 7, 9, 2, 6, 8, 3, 4, 5},
    {1, 8, 9, 2, 3, 4, 5, 6, 7}, {1, 8, 9, 2, 3, 5, 4, 6, 7},
    {1, 8, 9, 2, 3, 6, 4, 5, 7}, {1, 8, 9, 2, 3, 7, 4, 5, 6},
    {1, 8, 9, 2, 4, 5, 3, 6, 7}, {1, 8, 9, 2, 4, 6, 3, 5, 7},
    {1, 8, 9, 2, 4, 7, 3, 5, 6}, {1, 8, 9, 2, 5, 6, 3, 4, 7},
    {1, 8, 9, 2, 5, 7, 3, 4, 6}, {1, 8, 9, 2, 6, 7, 3, 4, 5}};

static int match_callback(const char* name, const char* tag,
                          const unsigned char* payload, int payload_len,
                          void* user_data) {
    rule_match_info_t* info = (rule_match_info_t*)user_data;
    info->name = name;
    info->tag = tag;
    return 0;
}

static int match_combine_callback(const char* name, const char* tag,
                                  const unsigned char* payload, int payload_len,
                                  void* user_data) {
    int* match_times = (int*)user_data;

    *match_times = *match_times + 1;

    // printf("match rulename is %s\n", name);
    return 0;
}
static void TestSingleCondition(const std::string& input, match_opt_t opt,
                                const std::string& pattern) {
    field_item_t field_list[] = {{0, "NAME0", opt}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    char* errptr = NULL;
    us_fieldset_t* fieldset = NULL;
    fieldset = us_fieldset_create(field_list, size, &errptr);
    ASSERT_TRUE(fieldset != NULL);
    ASSERT_TRUE(errptr == NULL);

    us_ruleset_t* ruleset = us_ruleset_create(fieldset, &errptr);
    ASSERT_TRUE(ruleset != NULL);
    ASSERT_TRUE(errptr == NULL);

    std::vector<const char*> field_names;
    std::vector<const char*> patterns;
    std::vector<match_opt_t> opts;
    const char* rule_name = "rule1";
    const char* tag = "tag1";
    field_names.push_back(field_list[0].name);
    patterns.push_back(pattern.c_str());
    opts.push_back(opt);
    us_ruleset_add_rule(ruleset, rule_name, tag, field_names.data(),
                        opts.data(), patterns.data(), NULL, 1, NULL, &errptr);
    ASSERT_TRUE(errptr == NULL);

    us_ruleset_compile(ruleset, &errptr);
    ASSERT_TRUE(errptr == NULL);
    input_item_t in;
    in.index = field_list[0].index;
    in.name = field_list[0].name;
    me_str_t value = {input.length(), (unsigned char*)input.c_str()};
    in.value = value;

    rule_match_info_t match_info;
    us_ruleset_set_match_callback(ruleset, match_callback, &match_info);
    us_ruleset_match(ruleset, &in, 1);

    ASSERT_STREQ(match_info.name.c_str(), rule_name);
    ASSERT_STREQ(match_info.tag.c_str(), tag);

    us_ruleset_destroy(ruleset);
    us_fieldset_destroy(fieldset);
}


static void TestSingleCondition2(const std::string& input, match_opt_t opt,
                                const std::string& pattern) {
    field_item_t field_list[] = {{0, "NAME0", opt}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    char* errptr = NULL;
    us_fieldset_t* fieldset = NULL;
    fieldset = us_fieldset_create(field_list, size, &errptr);
    ASSERT_TRUE(fieldset != NULL);
    ASSERT_TRUE(errptr == NULL);

    us_ruleset_t* ruleset = us_ruleset_create(fieldset, &errptr);
    ASSERT_TRUE(ruleset != NULL);
    ASSERT_TRUE(errptr == NULL);

    std::vector<const char*> field_names;
    std::vector<const char*> patterns;
    std::vector<match_opt_t> opts;
    const char* rule_name = "rule1";
    const char* tag = "tag1";
    field_names.push_back(field_list[0].name);
    patterns.push_back(pattern.c_str());
    opts.push_back(opt);
    us_ruleset_add_rule(ruleset, rule_name, tag, field_names.data(),
                        opts.data(), patterns.data(), NULL, 1, NULL, &errptr);
    ASSERT_TRUE(errptr == NULL);

    us_ruleset_compile(ruleset, &errptr);
    ASSERT_TRUE(errptr == NULL);
    input_item_t in;
    in.index = field_list[0].index;
    in.name = field_list[0].name;
    me_str_t value = {input.length(), (unsigned char*)input.c_str()};
    in.value = value;

    rule_match_info_t match_info;
    us_ruleset_set_match_callback(ruleset, match_callback, &match_info);
    us_ruleset_match(ruleset, &in, 1);

    ASSERT_TRUE(match_info.name == "");
    ASSERT_TRUE(match_info.tag == "");

    us_ruleset_destroy(ruleset);
    us_fieldset_destroy(fieldset);
}

TEST(RuleSetTest, Create) {
    field_item_t field_list[] = {{0, "NAME0", OPT_MATCH}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    char* errptr = NULL;
    us_fieldset_t* fieldset = NULL;
    fieldset = us_fieldset_create(field_list, size, &errptr);
    ASSERT_TRUE(fieldset != NULL);
    ASSERT_TRUE(errptr == NULL);

    us_ruleset_t* ruleset = us_ruleset_create(fieldset, &errptr);
    ASSERT_TRUE(ruleset != NULL);
    ASSERT_TRUE(errptr == NULL);

    us_ruleset_destroy(ruleset);
    us_fieldset_destroy(fieldset);
}

TEST(RuleSetTest, SimpleMatch) {
    typedef struct {
        std::string input;
        match_opt_t opt;
        std::string pattern;
    } single_match_data_t;

    std::vector<single_match_data_t> v = {
        {"abc", OPT_MATCH, "^abc$"},
        {"abcdef", OPT_MATCH, "bcd"},
        {"aabc", OPT_NOT_MATCH, "^abc$"},
        {"testabc", OPT_INCLUDE, "abc"},
        {"testabc", OPT_EQ, "testabc"},
        {"abc", OPT_LEN_EQ, "3"},
        {"abc", OPT_LEN_NOT_EQ, "4"},
        {"abc", OPT_LEN_GT, "2"},
        {"abc", OPT_LEN_LT, "4"},
        {"abc", OPT_NOT_INCLUDE, "123"},

        {"23", OPT_NUM_EQ, "23"},
        {"23", OPT_NUM_NOT_EQ, "25"},
        {"23", OPT_NUM_GT, "21"},
        {"23", OPT_NUM_LT, "25"},

        // ip inlist v4
        {"192.168.2.2", OPT_IP_IN_LIST, "192.168.2.2"},
        {"192.168.2.2", OPT_IP_IN_LIST, "192.168.2.2,192.168.2.3"},
        {"192.168.2.2", OPT_IP_IN_LIST, "192.168.2.3,192.168.2.2"},
        {"192.168.2.2", OPT_IP_IN_LIST, "192.168.2.3,192.168.2.2,1.1.1.1"},

        // ip inlist v6
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_IN_LIST,
         "1050:0:0:0:5:600:300c:326b"},
        {"1050:0:0:0:5:600:300c:326b", OPT_IP_IN_LIST,
         "1050:0000:0000:0000:0005:0600:300c:326b"},
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_IN_LIST,
         "1050:0000:0000:0000:0005:0600:300c:326b"},
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_IN_LIST,
         "1050:0000:0000:0000:0005:0600:300c:0000/112"},
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_IN_LIST,
         "192.168.1.2,1050:0000:0000:0000:0005:0600:300c:326b,192.168.1.1,192."
         "168.55.3"},
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_IN_LIST,
         "192.168.1.2,1050:0:0:0:5:600:300c:326b,192.168.1.1,192.168.55.3"},

        // ip inlist mixed
        {"192.168.2.2", OPT_IP_IN_LIST, "192.168.2.3,192.168.2.0/24"},
        {"192.168.1.1", OPT_IP_IN_LIST,
         "1050:0000:0000:0000:0005:0600:300c:326b,192.168.1.2,192.168.1.1,192."
         "168.55.3"},
        {"192.168.1.1", OPT_IP_IN_LIST,
         "192.168.1.2,192.168.5.0/"
         "24,1050:0000:0000:0000:0005:0600:300c:326b,192.168.1.1,192.168.55.3"},
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_IN_LIST,
         "192.168.1.0/"
         "24,192.168.2.3,1050:0000:0000:0000:0005:0600:300c:abcd,1050:0000:"
         "0000:0000:0005:0600:300c:0000/112"},

        // belong
        {"192.168.55.3", OPT_IP_BELONG, "192.168.55.0/24"},
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_BELONG,
         "1050:0000:0000:0000:0005:0600:300c:0000/112"},

        // not in list
        // only v4
        {"192.168.55.3", OPT_IP_NOT_IN_LIST,
         "192.168.55.4,192.168.55.5,192.168.55.6"},

        // only v4, with mask
        {"192.168.55.3", OPT_IP_NOT_IN_LIST,
         "192.168.56.0/24,192.168.55.5,192.168.55.6"},

        // only v6
        {"1050:0000:0000:0000:0005:0600:300c:326a", OPT_IP_NOT_IN_LIST,
         "1050:0000:0000:0000:0005:0600:300c:326b,1050:0000:0000:0000:0005:"
         "0600:300c:326c"},

        // only v6 with mask
        {"1050:0000:0000:0000:0005:0600:300c:326a", OPT_IP_NOT_IN_LIST,
         "1050:0000:0000:0000:0005:0600:300d:0000/"
         "112,1050:0000:0000:0000:0005:0600:300c:326b"},

        // input v4, but only v6 rule
        {"123.207.208.128", OPT_IP_NOT_IN_LIST,
         "1050:0000:0000:0000:0005:0600:300c:326b,1050:0000:0000:0000:0005:"
         "0600:300c:326a"},

        // input v6, but only v4 rule
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_NOT_IN_LIST,
         "123.207.208.128"},

        // input v4, but only v6 rule, with mask
        {"123.207.208.128", OPT_IP_NOT_IN_LIST,
         "1050:0000:0000:0000:0005:0600:300c:326b,1050:0000:0000:0000:0005:"
         "0600:300c:0000/112"},

        // input v6, but only v4 rule,with mask
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_NOT_IN_LIST,
         "123.207.208.128,1.1.1.0/24"},

        // mixed, input ipv4
        {"192.168.55.3", OPT_IP_NOT_IN_LIST,
         "192.168.56.0/24,192.168.55.5,1050:0000:0000:0000:0005:0600:300d:0000/"
         "112,1050:0000:0000:0000:0005:0600:300c:326b"},
        
        // mixed, input ipv6
        {"1050:0000:0000:0000:0005:0600:300c:326a", OPT_IP_NOT_IN_LIST,
         "192.168.56.0/24,192.168.55.5,1050:0000:0000:0000:0005:0600:300d:0000/112,1050:0000:0000:0000:0005:0600:300c:326b"},

        // not belong
        {"192.168.55.3", OPT_IP_NOT_BELONG, "192.168.56.0/24"},
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_NOT_BELONG,
         "1050:0000:0000:0000:0005:0600:300d:0000/112"},

    };
    for (size_t i = 0; i < v.size(); i++) {
        TestSingleCondition(v[i].input, v[i].opt, v[i].pattern);
    }
}

TEST(RuleSetTest, SimpleMatch2) {
    typedef struct {
        std::string input;
        match_opt_t opt;
        std::string pattern;
    } single_match_data_t;

    std::vector<single_match_data_t> v = {
        {"1050:0000:0000:0000:0005:0600:300c:326b", OPT_IP_NOT_IN_LIST,
         "192.168.56.0/24,192.168.55.5,1050:0000:0000:0000:0005:0600:300c:326b"},
    };
    for (size_t i = 0; i < v.size(); i++) {
        TestSingleCondition2(v[i].input, v[i].opt, v[i].pattern);
    }
}

TEST(RuleSetTest, CombineMatch1) {
    single_condition_data_t conditions[] = {
        {"ARGS", OPT_MATCH, "abc"},   {"ARGS", OPT_LEN_EQ, "4"},
        {"ARGS", OPT_EQ, "abcd"},

        {"HEADER", OPT_MATCH, "xyz"}, {"HEADER", OPT_NOT_INCLUDE, "ab"},
        {"HEADER", OPT_EQ, "wxyz"},

        {"BODY", OPT_MATCH, "123"},   {"BODY", OPT_INCLUDE, "23"},
        {"BODY", OPT_EQ, "1234"},

    };

    field_item_t field_list[] = {
        {0, "ARGS",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {1, "HEADER",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {2, "BODY",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
    };
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    for (size_t i = 0; i < tests.size(); i++) {
        char* errptr = NULL;
        us_fieldset_t* fieldset = NULL;
        fieldset = us_fieldset_create(field_list, size, &errptr);
        ASSERT_TRUE(fieldset != NULL);
        ASSERT_TRUE(errptr == NULL);

        us_ruleset_t* ruleset = us_ruleset_create(fieldset, &errptr);
        ASSERT_TRUE(ruleset != NULL);
        ASSERT_TRUE(errptr == NULL);

        for (size_t j = 0; j < tests[i].size(); j += 3) {
            std::vector<const char*> field_names;
            std::vector<const char*> patterns;
            std::vector<match_opt_t> opts;

            int index = tests[i][j] - 1;

            field_names.push_back(conditions[index].field_name);
            field_names.push_back(conditions[index + 1].field_name);
            field_names.push_back(conditions[index + 2].field_name);

            opts.push_back(conditions[index].opt);
            opts.push_back(conditions[index + 1].opt);
            opts.push_back(conditions[index + 2].opt);

            patterns.push_back(conditions[index].pattern);
            patterns.push_back(conditions[index + 1].pattern);
            patterns.push_back(conditions[index + 2].pattern);

            std::string rule_name =
                "rule" + std::to_string(i) + "_" + std::to_string(j);
            const char* tag = "tag1";
            us_ruleset_add_rule(ruleset, rule_name.c_str(), tag,
                                field_names.data(), opts.data(),
                                patterns.data(), NULL, 3, NULL, &errptr);
            ASSERT_TRUE(errptr == NULL);
        }

        us_ruleset_compile(ruleset, &errptr);
        ASSERT_TRUE(errptr == NULL);
        input_item_t input[] = {{0, "ARGS", {4, (u_char*)"abcd"}},
                                {1, "HEADER", {4, (u_char*)"wxyz"}},
                                {2, "BODY", {4, (u_char*)"1234"}}};

        int match_times = 0;
        us_ruleset_set_match_callback(ruleset, match_combine_callback,
                                      &match_times);
        us_ruleset_match(ruleset, input, 3);

        ASSERT_EQ(match_times, 3);

        us_ruleset_destroy(ruleset);
        us_fieldset_destroy(fieldset);
    }
}

TEST(RuleSetTest, CombineMatch2) {
    single_condition_data_t conditions[] = {
        {"ARGS", OPT_MATCH, "abc"},        {"ARGS", OPT_LEN_EQ, "4"},
        {"ARGS", OPT_EQ, "abcd"},

        {"HEADER", OPT_MATCH, "xyz"},      {"HEADER", OPT_NOT_INCLUDE, "ab"},
        {"HEADER", OPT_EQ, "wxyz"},

        {"SDKVERSION", OPT_NUM_EQ, "123"}, {"SDKVERSION", OPT_NUM_GT, "120"},
        {"SDKVERSION", OPT_NUM_LT, "125"},

    };

    field_item_t field_list[] = {
        {0, "ARGS",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {1, "HEADER",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {2, "SDKVERSION",
         OPT_NUM_EQ | OPT_NUM_NOT_EQ | OPT_NUM_GT | OPT_NUM_LT},
    };
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    for (size_t i = 0; i < tests.size(); i++) {
        char* errptr = NULL;
        us_fieldset_t* fieldset = NULL;
        fieldset = us_fieldset_create(field_list, size, &errptr);
        ASSERT_TRUE(fieldset != NULL);
        ASSERT_TRUE(errptr == NULL);

        us_ruleset_t* ruleset = us_ruleset_create(fieldset, &errptr);
        ASSERT_TRUE(ruleset != NULL);
        ASSERT_TRUE(errptr == NULL);

        for (size_t j = 0; j < tests[i].size(); j += 3) {
            std::vector<const char*> field_names;
            std::vector<const char*> patterns;
            std::vector<match_opt_t> opts;

            int index = tests[i][j] - 1;

            field_names.push_back(conditions[index].field_name);
            field_names.push_back(conditions[index + 1].field_name);
            field_names.push_back(conditions[index + 2].field_name);

            opts.push_back(conditions[index].opt);
            opts.push_back(conditions[index + 1].opt);
            opts.push_back(conditions[index + 2].opt);

            patterns.push_back(conditions[index].pattern);
            patterns.push_back(conditions[index + 1].pattern);
            patterns.push_back(conditions[index + 2].pattern);

            std::string rule_name =
                "rule" + std::to_string(i) + "_" + std::to_string(j);
            const char* tag = "tag1";
            us_ruleset_add_rule(ruleset, rule_name.c_str(), tag,
                                field_names.data(), opts.data(),
                                patterns.data(), NULL, 3, NULL, &errptr);
            ASSERT_TRUE(errptr == NULL);
        }

        us_ruleset_compile(ruleset, &errptr);
        ASSERT_TRUE(errptr == NULL);
        input_item_t input[] = {{0, "ARGS", {4, (u_char*)"abcd"}},
                                {1, "HEADER", {4, (u_char*)"wxyz"}},
                                {2, "SDKVERSION", {3, (u_char*)"123"}}};

        int match_times = 0;
        us_ruleset_set_match_callback(ruleset, match_combine_callback,
                                      &match_times);
        us_ruleset_match(ruleset, input, 3);

        ASSERT_EQ(match_times, 3);

        us_ruleset_destroy(ruleset);
        us_fieldset_destroy(fieldset);
    }
}

TEST(RuleSetTest, CombineMatch3) {
    single_condition_data_t conditions[] = {
        {"ARGS", OPT_MATCH, "abc"},
        {"ARGS", OPT_LEN_EQ, "4"},
        {"ARGS", OPT_EQ, "abcd"},

        {"IP", OPT_IP_IN_LIST, "1.1.1.1,2.2.2.2,3.3.3.3"},
        {"IP", OPT_IP_NOT_IN_LIST, "5.5.5.5,6.6.6.6,7.7.7.7"},
        {"IP", OPT_IP_NOT_BELONG, "192.168.55.0/24"},

        {"SDKVERSION", OPT_NUM_EQ, "123"},
        {"SDKVERSION", OPT_NUM_GT, "120"},
        {"SDKVERSION", OPT_NUM_LT, "125"},

    };

    field_item_t field_list[] = {
        {0, "ARGS",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {1, "IP",
         OPT_IP_IN_LIST | OPT_IP_BELONG | OPT_IP_NOT_IN_LIST |
             OPT_IP_NOT_BELONG},
        {2, "SDKVERSION",
         OPT_NUM_EQ | OPT_NUM_NOT_EQ | OPT_NUM_GT | OPT_NUM_LT},
    };
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    for (size_t i = 0; i < tests.size(); i++) {
        char* errptr = NULL;
        us_fieldset_t* fieldset = NULL;
        fieldset = us_fieldset_create(field_list, size, &errptr);
        ASSERT_TRUE(fieldset != NULL);
        ASSERT_TRUE(errptr == NULL);

        us_ruleset_t* ruleset = us_ruleset_create(fieldset, &errptr);
        ASSERT_TRUE(ruleset != NULL);
        ASSERT_TRUE(errptr == NULL);

        for (size_t j = 0; j < tests[i].size(); j += 3) {
            std::vector<const char*> field_names;
            std::vector<const char*> patterns;
            std::vector<match_opt_t> opts;

            int index = tests[i][j] - 1;

            field_names.push_back(conditions[index].field_name);
            field_names.push_back(conditions[index + 1].field_name);
            field_names.push_back(conditions[index + 2].field_name);

            opts.push_back(conditions[index].opt);
            opts.push_back(conditions[index + 1].opt);
            opts.push_back(conditions[index + 2].opt);

            patterns.push_back(conditions[index].pattern);
            patterns.push_back(conditions[index + 1].pattern);
            patterns.push_back(conditions[index + 2].pattern);

            std::string rule_name =
                "rule" + std::to_string(i) + "_" + std::to_string(j);
            const char* tag = "tag1";
            us_ruleset_add_rule(ruleset, rule_name.c_str(), tag,
                                field_names.data(), opts.data(),
                                patterns.data(), NULL, 3, NULL, &errptr);
            ASSERT_TRUE(errptr == NULL);
        }

        us_ruleset_compile(ruleset, &errptr);
        ASSERT_TRUE(errptr == NULL);
        input_item_t input[] = {{0, "ARGS", {4, (u_char*)"abcd"}},
                                {1, "IP", {7, (u_char*)"3.3.3.3"}},
                                {2, "SDKVERSION", {3, (u_char*)"123"}}};

        int match_times = 0;
        us_ruleset_set_match_callback(ruleset, match_combine_callback,
                                      &match_times);
        us_ruleset_match(ruleset, input, 3);

        ASSERT_EQ(match_times, 3);

        us_ruleset_destroy(ruleset);
        us_fieldset_destroy(fieldset);
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
