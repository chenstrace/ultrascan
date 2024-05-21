#ifndef USCAN_INCLUDE_C_H_
#define USCAN_INCLUDE_C_H_

#include <stdbool.h>

#include "export.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct us_fieldset_t us_fieldset_t;
typedef struct us_ruleset_t us_ruleset_t;

// fieldset

ME_EXPORT const char* us_library_build_info();

ME_EXPORT us_fieldset_t* us_fieldset_create(const field_item_t* field_list,
                                      size_t size, char** errptr);
ME_EXPORT size_t us_fieldset_get_field_count(us_fieldset_t* fieldset);
ME_EXPORT void us_fieldset_destroy(us_fieldset_t* fieldset);

// ruleset
ME_EXPORT us_ruleset_t* us_ruleset_create(us_fieldset_t* fieldset, char** errptr);
ME_EXPORT bool us_ruleset_add_rule(us_ruleset_t* ruleset, const char* rule_name,
                                const char* tag, const char* const* field_names,
                                const match_opt_t* opts,
                                const char* const* patterns,
                                const unsigned int* ids, unsigned int size,
                                const char* logic, char** errptr);
ME_EXPORT void us_ruleset_set_match_callback(us_ruleset_t* ruleset,
                                          rule_match_callback callback,
                                          void* user_data);
ME_EXPORT bool us_ruleset_compile(us_ruleset_t* ruleset, char** errptr);
ME_EXPORT bool us_ruleset_match(us_ruleset_t* ruleset, const input_item_t* input,
                             size_t size);
ME_EXPORT void us_ruleset_destroy(us_ruleset_t* ruleset);

#ifdef __cplusplus
}
#endif

#endif /* USCAN_INCLUDE_C_H_ */
