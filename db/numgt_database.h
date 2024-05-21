#ifndef DB_NUMGT_DATABASE_H
#define DB_NUMGT_DATABASE_H

#include "numcmp_database.h"
namespace ultrascan {

class NumGreateThanDatabase : public NumCmpDatabase {
   public:
    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) {
        num_cmp_info_t cmp;
        char* errptr = NULL;
        cmp.num = safe_atol(str->data, str->len, &errptr);

        if (errptr) {
            return;
        }
        auto p = std::lower_bound(nums_info.begin(), nums_info.end(), cmp);

        for (auto i = nums_info.begin(); i != p; ++i) {
            int ret = callback(i->id, user_data, NULL, 0);
            if (ret != 0) {
                break;
            }
        }
    }
};
}  // namespace ultrascan
#endif
