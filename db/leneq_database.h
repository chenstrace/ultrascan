#ifndef DB_LENEQ_DATABASE_H
#define DB_LENEQ_DATABASE_H
#include <vector>

#include "lencmp_database.h"
namespace ultrascan {

class LenEqualDatabase : public LenCmpDatabase {
   public:
    LenEqualDatabase() = default;
    LenEqualDatabase(const LenEqualDatabase&) = delete;

    LenEqualDatabase& operator=(const LenEqualDatabase&) = delete;

    virtual ~LenEqualDatabase() = default;
    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) {
        len_cmp_info_t cmp;
        cmp.num = str->len;
        auto p = std::equal_range(lens_info.begin(), lens_info.end(), cmp);

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
