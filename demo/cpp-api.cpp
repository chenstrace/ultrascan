#include <bits/stdc++.h>

#include "fieldset.h"
#include "ruleset.h"
#include "test_data.h"
#include "types.h"

using ultrascan::FieldSet;
using ultrascan::RuleSet;

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
int main(int argc, char** argv) {
    FieldSet* fieldset = NULL;
    RuleSet* ruleset = NULL;
    char* errptr = NULL;
    bool success;

    success = FieldSet::Create(field_list, field_list_size, &fieldset, &errptr);
    if (!success) {
        printf("%s\n", errptr);
        return EXIT_FAILURE;
    }
    success = RuleSet::Create(fieldset, &ruleset, &errptr);
    if (!success) {
        printf("%s\n", errptr);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < g_rules_count; i++) {
        const std::string rule_name = "rule" + std::to_string(i + 1);

        std::vector<const char*> field_names;
        std::vector<match_opt_t> opts;
        std::vector<const char*> patterns;
        std::vector<unsigned int> ids;
        const char* logic = NULL;

        condition_info_t* conditions = g_rules_info[i];
        for (int j = 0; conditions[j].field_name != NULL; j++) {
            const char* field_name = conditions[j].field_name;
            match_opt_t opt = conditions[j].opt;
            const char* pattern = conditions[j].pattern;

            field_names.push_back(field_name);
            opts.push_back(opt);
            patterns.push_back(pattern);
        }

        ruleset->AddRule(rule_name.c_str(), "deny", field_names.data(),
                         opts.data(), patterns.data(), ids.data(),
                         patterns.size(), logic, &errptr);
    }

    success = ruleset->Compile(&errptr);
    if (!success) {
        printf("%s\n", errptr);
        return EXIT_FAILURE;
    }
    ruleset->SetMatchCallback(match_callback, NULL);
    ruleset->Match(g_input, g_input_count);

    delete fieldset;
    delete ruleset;

    return 0;
}
