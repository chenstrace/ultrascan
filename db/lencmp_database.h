#ifndef DB_LENCMP_DATABASE_H
#define DB_LENCMP_DATABASE_H
#include <algorithm>
#include <vector>

#include "database.h"
#include "internal.h"
#include "utils.h"
namespace ultrascan {

struct len_cmp_info_t {
   public:
    int id;
    long num;
    bool operator<(const len_cmp_info_t& right) const {
        return num < right.num;
    }
};
class LenCmpDatabase : public Database {
   public:
    LenCmpDatabase() = default;
    LenCmpDatabase(const LenCmpDatabase&) = delete;

    LenCmpDatabase& operator=(const LenCmpDatabase&) = delete;

    virtual ~LenCmpDatabase() = default;
    virtual bool Init(const std::vector<pattern_info_t>& info, char** errptr) {
        *errptr = NULL;
        for (size_t i = 0; i < info.size(); i++) {
            len_cmp_info_t t;
            t.id = info[i].id;
            t.num = safe_atol((unsigned char*)info[i].pattern.c_str(),
                              info[i].pattern.size(), errptr);
            if (*errptr) {
                return false;
            }
            lens_info.push_back(t);
        }
        std::sort(lens_info.begin(), lens_info.end());
        return true;
    }

   protected:
    std::vector<len_cmp_info_t> lens_info;
};
}  // namespace ultrascan
#endif
