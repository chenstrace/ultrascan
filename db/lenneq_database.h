#ifndef DB_LENNEQ_DATABASE_H
#define DB_LENNEQ_DATABASE_H
#include <vector>

#include "lencmp_database.h"
namespace ultrascan {

class LenNotEqualDatabase : public LenCmpDatabase {
   public:
    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) {
        for (auto i = lens_info.begin(); i != lens_info.end(); ++i) {
            if (i->num != (long)str->len) {
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
