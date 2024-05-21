#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "c.h"
#include "test_data.h"

#define MAX_NAME_SIZE 256

static int match_callback(const char* name, const char* tag,
                          const unsigned char* payload, int payload_len,
                          void* user_data) {
    printf("matched, rule name is %s, tag is %s", name, tag);
    if (payload && payload_len > 0) {
        printf(", payload is %.*s\n", payload_len, payload);
    } else {
        printf("\n");
    }
    // return 0 means continue matching, other means stop matching
    return 0;
}
int main() {
    int i;
    int j;
    us_fieldset_t* fieldset = NULL;
    us_ruleset_t* ruleset = NULL;

    char* errptr = NULL;
    bool success = false;
    const int MAX_ONE_RULE_CONDITION_COUNT = 100;
    const char* field_names[MAX_ONE_RULE_CONDITION_COUNT];
    match_opt_t opts[MAX_ONE_RULE_CONDITION_COUNT];
    const char* patterns[MAX_ONE_RULE_CONDITION_COUNT];

    unsigned int condition_count;
    printf("%s\n", us_library_build_info());

    fieldset = us_fieldset_create(field_list, field_list_size, &errptr);
    assert(fieldset);

    ruleset = us_ruleset_create(fieldset, &errptr);
    assert(ruleset);

    for (i = 0; i < g_rules_count; i++) {
        char rule_name[MAX_NAME_SIZE] = {0};
        snprintf(rule_name, MAX_NAME_SIZE, "rule%d", i + 1);
        condition_count = 0;
        condition_info_t* conditions = g_rules_info[i];

        for (j = 0; conditions[j].field_name != NULL; j++) {
            assert(condition_count < MAX_ONE_RULE_CONDITION_COUNT);
            const char* field_name = conditions[j].field_name;
            match_opt_t opt = conditions[j].opt;
            const char* pattern = conditions[j].pattern;
            field_names[condition_count] = field_name;
            opts[condition_count] = opt;
            patterns[condition_count] = pattern;
            condition_count++;
        }
        us_ruleset_add_rule(ruleset, rule_name, "deny", field_names, opts,
                            patterns, NULL, condition_count, NULL, &errptr);
    }

    us_ruleset_set_match_callback(ruleset, match_callback, NULL);
    success = us_ruleset_compile(ruleset, &errptr);
    assert(success);
    if (!success) {
        printf("%s\n", errptr);
        return EXIT_FAILURE;
    }
    us_ruleset_match(ruleset, g_input, g_input_count);
    us_ruleset_destroy(ruleset);
    us_fieldset_destroy(fieldset);
    return 0;
}