#include "fieldset_impl.h"

#include <assert.h>
#include <string.h>

#include <algorithm>
#include <set>
#include <vector>
namespace ultrascan {
static bool CompareFieldItem(const field_item_t& a, const field_item_t& b);
FieldSet::~FieldSet() = default;
bool FieldSet::Create(const field_item_t* field_list, size_t size,
                      FieldSet** fieldset, char** errptr) {
    *fieldset = NULL;
    *errptr = NULL;

    if (field_list == NULL) {
        *errptr = strdup("invalid field list");
        return false;
    }
    if (size == 0) {
        *errptr = strdup("invalid field list size");
        return false;
    }

    FieldSetImpl* impl = new FieldSetImpl;
    if (impl == NULL) {
        *errptr = strdup("alloc field set failed");
        return false;
    }

    bool success = impl->Init(field_list, size, errptr);
    if (!success) {
        delete impl;
        return false;
    }

    *fieldset = impl;
    return true;
}

bool FieldSetImpl::Init(const field_item_t* field_list, size_t size,
                        char** errptr) {
    *errptr = NULL;

    std::vector<field_item_t> v;
    std::set<std::string> name_set;
    for (size_t i = 0; i < size; i++) {
        if (field_list[i].opt_mask == 0) {
            *errptr = strdup("opt_mask's value is 0, not allowed");
            return false;
        }

        field_item_t t = {field_list[i].index, field_list[i].name,
                          field_list[i].opt_mask};
        v.push_back(t);
        auto it = name_set.insert(field_list[i].name);
        if (!it.second) {
            *errptr = strdup("field with same name is not allowed");
            return false;
        }
    }

    std::sort(v.begin(), v.end(), CompareFieldItem);
    for (size_t i = 0; i < size; i++) {
        if (v[i].index != i) {
            std::string msg = "field index must be unique and from 0 to " +
                              std::to_string(size - 1);
            *errptr = strdup(msg.c_str());
            return false;
        }
    }

    for (size_t i = 0; i < size; i++) {
        std::string name = v[i].name;
        name2index_.emplace(name, v[i].index);

        field_item_t field;
        field.index = v[i].index;
        field.opt_mask = v[i].opt_mask;
        auto it = name2index_.find(name);
        field.name = it->first.c_str();
        index2field_.emplace(v[i].index, field);
    }
    return true;
}

int FieldSetImpl::GetFieldIndex(const std::string& name) {
    auto it = name2index_.find(name);
    if (it != name2index_.end()) {
        return it->second;
    } else {
        return -1;
    }
}

size_t FieldSetImpl::GetFieldCount() const { return index2field_.size(); }

bool FieldSetImpl::IsValidOpt(int field_index, match_opt_t opt) {
    auto it = index2field_.find(field_index);
    if (it == index2field_.end()) {
        return false;
    }
    if (it->second.opt_mask & opt) {
        return true;
    }
    return false;
}

static bool CompareFieldItem(const field_item_t& a, const field_item_t& b) {
    if (a.index < b.index) return true;
    return false;
}
}  // namespace ultrascan