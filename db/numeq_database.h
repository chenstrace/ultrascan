#ifndef DB_NUMEQ_DATABASE_H
#define DB_NUMEQ_DATABASE_H

#include <vector>

#include "numcmp_database.h"
namespace ultrascan {

class NumEqualDatabase : public NumCmpDatabase {
   public:
    NumEqualDatabase() = default;
    NumEqualDatabase(const NumEqualDatabase&) = delete;

    NumEqualDatabase& operator=(const NumEqualDatabase&) = delete;

    virtual ~NumEqualDatabase() = default;
    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) {
        num_cmp_info_t cmp;

        char* errptr = NULL;
        cmp.num = safe_atol(str->data, str->len, &errptr);

        if (errptr) {
            return;
        }

        auto p = std::equal_range(nums_info.begin(), nums_info.end(), cmp);

        for (auto i = p.first; i != p.second; ++i) {
            int ret = callback(i->id, user_data, NULL, 0);
            if (ret != 0) {
                break;
            }
        }
    }
};
}  // namespace ultrascan

#endif
