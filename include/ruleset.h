#ifndef USCAN_INCLUDE_RULESET_H_
#define USCAN_INCLUDE_RULESET_H_

#include "export.h"
#include "types.h"
namespace ultrascan {
class FieldSet;

class ME_EXPORT RuleSet {
   public:
    RuleSet() = default;
    RuleSet(const RuleSet&) = delete;

    RuleSet& operator=(const RuleSet&) = delete;

    virtual ~RuleSet();

    static bool Create(FieldSet* fieldset, RuleSet** ptr, char** errptr);

    virtual bool AddRule(const char* rule_name, const char* tag,
                         const char* const* field_names,
                         const match_opt_t* opts, const char* const* patterns,
                         const unsigned int* ids, unsigned int size,
                         const char* logic, char** errptr) = 0;
    virtual bool Compile(char** errptr) = 0;

    virtual void SetMatchCallback(rule_match_callback callback,
                                  void* user_data) = 0;

    virtual bool Match(const input_item_t* items, size_t size) = 0;
};
}  // namespace ultrascan

#endif /* USCAN_INCLUDE_RULESET_H_ */
