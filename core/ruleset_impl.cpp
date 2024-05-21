#include "ruleset_impl.h"

#include <string.h>

#include "match_context.h"
namespace ultrascan {
RuleSet::~RuleSet() = default;
bool RuleSet::Create(FieldSet* fieldset, RuleSet** ptr, char** errptr) {
    *ptr = NULL;
    RuleSetImpl* pimpl = new RuleSetImpl();
    if (pimpl == NULL) {
        *errptr = strdup("alloc ruleset failed");
        return false;
    }
    pimpl->compile_context_ = new MatchContext(fieldset);
    if (pimpl->compile_context_ == NULL) {
        *errptr = strdup("alloc ruleset context failed");
        return false;
    }
    *ptr = pimpl;
    return true;
}

bool RuleSet::AddRule(const char* rule_name, const char* tag,
                      const char* const* field_names, const match_opt_t* opts,
                      const char* const* patterns, const unsigned int* ids,
                      unsigned int size, const char* logic, char** errptr) {
    return AddRule(rule_name, tag, field_names, opts, patterns, ids, size,
                   logic, errptr);
}

bool RuleSet::Compile(char** errptr) { return Compile(errptr); }

bool RuleSet::Match(const input_item_t* items, size_t size) {
    return Match(items, size);
}

void RuleSet::SetMatchCallback(rule_match_callback callback, void* user_data) {
    SetMatchCallback(callback, user_data);
}

// impl

RuleSetImpl::~RuleSetImpl() { delete compile_context_; }

bool RuleSetImpl::AddRule(const char* rule_name, const char* tag,
                          const char* const* field_names,
                          const match_opt_t* opts, const char* const* patterns,
                          const unsigned int* ids, unsigned int size,
                          const char* logic, char** errptr) {
    return compile_context_->AddRule(rule_name, tag, field_names, opts,
                                     patterns, ids, size, logic, errptr);
}
bool RuleSetImpl::Compile(char** errptr) {
    return compile_context_->Compile(errptr);
}

bool RuleSetImpl::Match(const input_item_t* items, size_t size) {
    return compile_context_->Match(items, size);
}

void RuleSetImpl::SetMatchCallback(rule_match_callback callback,
                                   void* user_data) {
    compile_context_->SetMatchCallback(callback, user_data);
}
}  // namespace ultrascan