#ifndef USCAN_CORE_CONDITION_IMPL_H_
#define USCAN_CORE_CONDITION_IMPL_H_
#include "condition.h"
#include "internal.h"

namespace ultrascan {
class FieldSet;
class ConditionImpl : public Condition {
   public:
    ConditionImpl() = default;
    ConditionImpl(const ConditionImpl&) = delete;

    ConditionImpl& operator=(const ConditionImpl&) = delete;

    ConditionImpl(const std::string& field_name, match_opt_t opt,
                  const std::string& pattern, FieldSet* fset);

    virtual ~ConditionImpl();

    void SetOrder(int order);

    int GetOrder();

    db_type_t GetDatabaseType() const;
    void SetDatabaseType(db_type_t db_type);

    int GetFieldIndex() const;
    void SetFieldIndex(int field_index);
    unsigned int GetId() const;
    void SetId(unsigned int id);

    size_t GetRootIndex() const;
    void SetRootIndex(size_t root_index);
    std::string GetPattern() const;
    void SetPattern(const std::string& pattern);

    void SetUsePCRE(bool use_pcre);
    bool GetUsePCRE();
    ConditionImpl* GetNext();
    void SetNext(ConditionImpl* next);

    void SetRuleName(const char* rule_name);
    const char* GetRuleName();

    void SetTag(const char* tag);
    const char* GetTag();

    void SetPayload(const unsigned char* payload, int payload_len);
    const unsigned char* GetPayload();
    int GetPayloadLength();

   private:
    std::string field_name_;
    match_opt_t opt_;

    std::string pattern_;
    unsigned int id_;
    int field_index_;

    size_t root_index_;
    int order_;

    bool use_pcre_;
    ConditionImpl* next_;

    db_type_t db_type_;
    FieldSet* fieldset_;
    const char* rule_name_;
    const char* tag_;

    const unsigned char* payload_;
    int payload_len_;
};

inline int CalcRootIndex(int field_index, db_type_t db_type) {
    return field_index * DB_TYPE_COUNT + db_type;
}
}  // namespace ultrascan

#endif /* USCAN_CORE_CONDITION_IMPL_H_ */
