#ifndef DB_NUMNEQ_DATABASE_H
#define DB_NUMNEQ_DATABASE_H

#include <vector>

#include "numcmp_database.h"
namespace ultrascan {

class NumNotEqualDatabase : public NumCmpDatabase {
   public:
    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) {
        char* errptr = NULL;
        long num = safe_atol(str->data, str->len, &errptr);

        if (errptr) {
            return;
        }
        for (auto i = nums_info.begin(); i != nums_info.end(); ++i) {
            if (i->num != num) {
                int ret = callback(i->id, user_data, NULL, 0);
                if (ret != 0) {
                    break;
                }
            }
        }
    }
};
}  // namespace ultrascan

#endif
