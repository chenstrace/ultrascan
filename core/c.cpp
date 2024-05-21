#include "c.h"

#include <string.h>

#include "condition.h"
#include "fieldset.h"
#include "ruleset.h"

using ultrascan::FieldSet;
using ultrascan::RuleSet;

extern "C" {

struct us_fieldset_t {
    FieldSet* rep;
};

struct us_ruleset_t {
    RuleSet* rep;
};

// fieldset
us_fieldset_t* us_fieldset_create(const field_item_t* field_list, size_t size,
                            char** errptr) {
    *errptr = NULL;
    FieldSet* ptr = NULL;
    FieldSet::Create(field_list, size, &ptr, errptr);

    if (ptr == NULL) {
        return NULL;
    }

    us_fieldset_t* result = new us_fieldset_t;
    if (result == NULL) {
        *errptr = strdup("alloc fieldset_t failed");
        return NULL;
    }
    result->rep = ptr;

    return result;
}

size_t us_fieldset_get_field_count(us_fieldset_t* fieldset) {
    return fieldset->rep->GetFieldCount();
}

void us_fieldset_destroy(us_fieldset_t* fieldset) {
    delete fieldset->rep;
    delete fieldset;
}

// ruleset

us_ruleset_t* us_ruleset_create(us_fieldset_t* fieldset, char** errptr) {
    *errptr = NULL;

    if (fieldset == NULL) {
        *errptr = strdup("invalid fieldset");
        return NULL;
    }
    us_ruleset_t* ruleset = new us_ruleset_t;
    if (ruleset == NULL) {
        *errptr = strdup("alloc ruleset failed");
        return NULL;
    }

    RuleSet* ptr = NULL;
    RuleSet::Create(fieldset->rep, &ptr, errptr);
    if (ptr == NULL) {
        return NULL;
    }
    ruleset->rep = ptr;

    return ruleset;
}

void us_ruleset_destroy(us_ruleset_t* ruleset) {
    delete ruleset->rep;
    delete ruleset;
}

bool us_ruleset_add_rule(us_ruleset_t* ruleset, const char* rule_name,
                      const char* tag, const char* const* field_names,
                      const match_opt_t* opts, const char* const* patterns,
                      const unsigned int* ids, unsigned int size,
                      const char* logic, char** errptr) {
    return ruleset->rep->AddRule(rule_name, tag, field_names, opts, patterns,
                                 ids, size, logic, errptr);
}

void us_ruleset_set_match_callback(us_ruleset_t* ruleset,
                                rule_match_callback callback, void* user_data) {
    ruleset->rep->SetMatchCallback(callback, user_data);
}

bool us_ruleset_compile(us_ruleset_t* ruleset, char** errptr) {
    return ruleset->rep->Compile(errptr);
}

bool us_ruleset_match(us_ruleset_t* ruleset, const input_item_t* input, size_t size) {
    return ruleset->rep->Match(input, size);
}

}  // end extern "C"
