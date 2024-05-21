#ifndef USCAN_CORE_FIELDSET_IMPL_H_
#define USCAN_CORE_FIELDSET_IMPL_H_
#include <string>
#include <unordered_map>

#include "fieldset.h"
namespace ultrascan {
class FieldSetImpl : public FieldSet {
   public:
    FieldSetImpl() = default;
    FieldSetImpl(const FieldSetImpl&) = delete;

    FieldSetImpl& operator=(const FieldSetImpl&) = delete;

    bool Init(const field_item_t* field_list, size_t size, char** errptr);

    int GetFieldIndex(const std::string& name);

    bool IsValidOpt(int field_index, match_opt_t opt);
    virtual size_t GetFieldCount() const override;

   private:
    std::unordered_map<std::string, int> name2index_;
    std::unordered_map<int, field_item_t> index2field_;
};
}  // namespace ultrascan
#endif /* USCAN_CORE_FIELDSET_IMPL_H_ */
