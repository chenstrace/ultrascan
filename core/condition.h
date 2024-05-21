#ifndef USCAN_CORE_CONDITION_H_
#define USCAN_CORE_CONDITION_H_
#include <string>

#include "types.h"

namespace ultrascan {
class FieldSet;

class Condition {
   public:
    static bool Create(const std::string& field_name, match_opt_t opt,
                       const std::string& pattern, FieldSet* fieldset,
                       Condition** condition_ptr,char** errptr);
    Condition() = default;
    Condition(const Condition&) = delete;

    Condition& operator=(const Condition&) = delete;

    virtual ~Condition();
};
}  // namespace ultrascan

#endif /* USCAN_CORE_CONDITION_H_ */
