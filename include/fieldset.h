#ifndef USCAN_INCLUDE_FIELDSET_H_
#define USCAN_INCLUDE_FIELDSET_H_
#include <stddef.h>

#include "export.h"
#include "types.h"
namespace ultrascan {

class ME_EXPORT FieldSet {
   public:
    FieldSet() = default;
    FieldSet(const FieldSet&) = delete;

    FieldSet& operator=(const FieldSet&) = delete;

    virtual ~FieldSet();

    static bool Create(const field_item_t* field_list, size_t size,
                       FieldSet** fieldset, char** errptr);

    virtual size_t GetFieldCount() const = 0;
};
}  // namespace ultrascan

#endif /* USCAN_INCLUDE_FIELDSET_H_ */
