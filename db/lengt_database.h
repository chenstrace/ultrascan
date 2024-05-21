#ifndef DB_LENGT_DATABASE_H
#define DB_LENGT_DATABASE_H
#include "lencmp_database.h"
namespace ultrascan {

class LenGreateThanDatabase : public LenCmpDatabase {
   public:
    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) {
        len_cmp_info_t cmp;
        cmp.num = str->len;
        auto p = std::lower_bound(lens_info.begin(), lens_info.end(), cmp);

        for (auto i = lens_info.begin(); i != p; ++i) {
            int ret = callback(i->id, user_data, NULL, 0);
            if (ret != 0) {
                break;
            }
        }
    }
};
}  // namespace ultrascan
#endif
