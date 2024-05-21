#include "condition_impl.h"

#include <string.h>

#include <iomanip>
#include <sstream>

#include "fieldset_impl.h"
#include "internal.h"
namespace ultrascan {
static std::string ConvertEqualToRegex(const std::string& s);
static std::string ConvertIncludeToRegex(const std::string& s);

Condition::~Condition() = default;

ConditionImpl::ConditionImpl(const std::string& field_name, match_opt_t opt,
                             const std::string& pattern, FieldSet* fieldset)
    : field_name_(field_name),
      opt_(opt),
      pattern_(pattern),
      id_(0),
      field_index_(-1),
      root_index_(-1),
      order_(-1),
      use_pcre_(false),
      next_(NULL),
      db_type_(DB_TYPE_COUNT),
      fieldset_(fieldset),
      rule_name_(NULL),
      tag_(NULL),
      payload_(NULL),
      payload_len_(0) {}

ConditionImpl::~ConditionImpl() {}

bool Condition::Create(const std::string& field_name, match_opt_t opt,
                       const std::string& pattern, FieldSet* fieldset,
                       Condition** condition_ptr, char** errptr) {
    db_type_t db_type;

    *condition_ptr = NULL;
    *errptr = NULL;
    switch (opt) {
        case OPT_LEN_EQ:
            db_type = DB_TYPE_LEN_EQUAL;
            break;
        case OPT_LEN_NOT_EQ:
            db_type = DB_TYPE_LEN_NOT_EQUAL;
            break;
        case OPT_LEN_LT:
            db_type = DB_TYPE_LEN_LESS_THAN;
            break;
        case OPT_LEN_GT:
            db_type = DB_TYPE_LEN_GREAT_THAN;
            break;
        case OPT_IP_IN_LIST:
            db_type = DB_TYPE_IP_IN_LIST;
            break;
        case OPT_IP_BELONG:
            db_type = DB_TYPE_IP_BELONG;
            break;
        case OPT_NOT_INCLUDE:
            db_type = DB_TYPE_NOT_INCLUDE;
            break;
        case OPT_EQ:
        case OPT_MATCH:
        case OPT_INCLUDE:
            db_type = DB_TYPE_REGEX_MATCH;
            break;
        case OPT_NOT_MATCH:
            db_type = DB_TYPE_REGEX_NOT_MATCH;
            break;
        case OPT_NUM_EQ:
            db_type = DB_TYPE_NUM_EQUAL;
            break;
        case OPT_NUM_NOT_EQ:
            db_type = DB_TYPE_NUM_NOT_EQUAL;
            break;
        case OPT_NUM_LT:
            db_type = DB_TYPE_NUM_LESS_THAN;
            break;
        case OPT_NUM_GT:
            db_type = DB_TYPE_NUM_GREAT_THAN;
            break;
        case OPT_IP_NOT_IN_LIST:
            db_type = DB_TYPE_IP_NOT_IN_LIST;
            break;
        case OPT_IP_NOT_BELONG:
            db_type = DB_TYPE_IP_NOT_BELONG;
            break;
        default:
            *errptr = strdup("unkown condition opt");
            return false;
    }

    ConditionImpl* impl = new ConditionImpl(field_name, opt, pattern, fieldset);

    impl->SetDatabaseType(db_type);

    FieldSetImpl* fieldset_impl = reinterpret_cast<FieldSetImpl*>(fieldset);

    int field_index = fieldset_impl->GetFieldIndex(field_name);
    if (field_index == -1) {
        *errptr = strdup("field_index error");
        delete impl;
        return false;
    }
    bool is_valid_opt = fieldset_impl->IsValidOpt(field_index, opt);
    if (!is_valid_opt) {
        *errptr = strdup("is_valid_opt error");
        delete impl;
        return false;
    }
    impl->SetFieldIndex(field_index);

    int root_index = CalcRootIndex(field_index, db_type);
    impl->SetRootIndex(root_index);

    if (opt == OPT_EQ) {
        impl->SetPattern(ConvertEqualToRegex(pattern));
    } else if (opt == OPT_INCLUDE) {
        impl->SetPattern(ConvertIncludeToRegex(pattern));
    }

    *condition_ptr = impl;
    return true;
}

int ConditionImpl::GetOrder() { return order_; }

void ConditionImpl::SetOrder(int order) { order_ = order; }

void ConditionImpl::SetRootIndex(size_t root_index) {
    root_index_ = root_index;
}

db_type_t ConditionImpl::GetDatabaseType() const { return db_type_; }

void ConditionImpl::SetDatabaseType(db_type_t db_type) { db_type_ = db_type; }

int ConditionImpl::GetFieldIndex() const { return field_index_; }

void ConditionImpl::SetFieldIndex(int field_index) {
    field_index_ = field_index;
}

void ConditionImpl::SetId(unsigned int id) { id_ = id; }

unsigned int ConditionImpl::GetId() const { return id_; }

size_t ConditionImpl::GetRootIndex() const { return root_index_; }

std::string ConditionImpl::GetPattern() const { return pattern_; }

void ConditionImpl::SetPattern(const std::string& pattern) {
    pattern_ = pattern;
}

ConditionImpl* ConditionImpl::GetNext() { return next_; }

void ConditionImpl::SetUsePCRE(bool use_pcre) { use_pcre_ = use_pcre; }
bool ConditionImpl::GetUsePCRE() { return use_pcre_; }

void ConditionImpl::SetNext(ConditionImpl* next) { next_ = next; }

void ConditionImpl::SetRuleName(const char* rule_name) {
    rule_name_ = rule_name;
}

void ConditionImpl::SetTag(const char* tag) { tag_ = tag; }

const char* ConditionImpl::GetRuleName() { return rule_name_; }

const char* ConditionImpl::GetTag() { return tag_; }

void ConditionImpl::SetPayload(const unsigned char* payload, int payload_len) {
    payload_ = payload;
    payload_len_ = payload_len;
}
const unsigned char* ConditionImpl::GetPayload() { return payload_; }
int ConditionImpl::GetPayloadLength() { return payload_len_; }
static bool IsMetaChar(char c) {
    switch (c) {
        case '\\':
        case '|':
        case '(':
        case ')':
        case '*':
        case '+':
        case '.':
        case '/':
        case '#':
        case '$':
        case '?':
        case '[':
        case ']':
        case '^':
        case '{':
        case '}':
            return true;
        default:
            break;
    }
    return false;
}

static std::string ToRegex(const std::string& s) {
    std::ostringstream os;
    for (char c : s) {
        if (0x20 <= c && c <= 0x7e) {
            if (IsMetaChar(c)) {
                os << "\\" << c;
            } else {
                os << c;
            }
        } else if (c == '\n') {
            os << "\\n";
        } else if (c == '\r') {
            os << "\\r";
        } else if (c == '\t') {
            os << "\\t";
        } else {
            os << "\\x" << std::hex << std::setw(2) << std::setfill('0')
               << (unsigned)(c & 0xff) << std::dec;
        }
    }
    return os.str();
}

static std::string ConvertEqualToRegex(const std::string& s) {
    return std::string("^") + ToRegex(s) + std::string("$");
}

static std::string ConvertIncludeToRegex(const std::string& s) {
    return ToRegex(s);
}
}  // namespace ultrascan