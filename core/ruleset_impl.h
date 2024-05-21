#ifndef USCAN_CORE_RULESET_IMPL_H_
#define USCAN_CORE_RULESET_IMPL_H_
#include <vector>

#include "ruleset.h"
namespace ultrascan {
class MatchContext;

class RuleSetImpl : public RuleSet {
   public:
    RuleSetImpl() = default;
    RuleSetImpl(const RuleSet&) = delete;
    RuleSetImpl& operator=(const RuleSetImpl&) = delete;

    virtual ~RuleSetImpl();

    virtual bool AddRule(const char* rule_name, const char* tag,
                         const char* const* field_names,
                         const match_opt_t* opts, const char* const* patterns,
                         const unsigned int* ids, unsigned int size,
                         const char* logic, char** errptr) override;
    virtual bool Compile(char** errptr) override;

    virtual bool Match(const input_item_t* items, size_t size) override;

    void SetMatchCallback(rule_match_callback callback,
                          void* user_data) override;

   private:
    friend class RuleSet;

    MatchContext* compile_context_;
};
}  // namespace ultrascan

#endif /* USCAN_CORE_RULESET_IMPL_H_ */
