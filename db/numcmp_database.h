#ifndef DB_NUMCMP_DATABASE_H
#define DB_NUMCMP_DATABASE_H

#include <algorithm>
#include <vector>

#include "database.h"
#include "internal.h"
#include "utils.h"
namespace ultrascan {

struct num_cmp_info_t {
   public:
    int id;
    long num;
    bool operator<(const num_cmp_info_t& right) const {
        return num < right.num;
    }
};
class NumCmpDatabase : public Database {
   public:
    NumCmpDatabase() = default;
    NumCmpDatabase(const NumCmpDatabase&) = delete;

    NumCmpDatabase& operator=(const NumCmpDatabase&) = delete;

    virtual ~NumCmpDatabase() = default;
    virtual bool Init(const std::vector<pattern_info_t>& info, char** errptr) {
        *errptr = NULL;
        for (size_t i = 0; i < info.size(); i++) {
            num_cmp_info_t t;
            t.id = info[i].id;
            t.num = safe_atol((unsigned char*)info[i].pattern.c_str(),
                              info[i].pattern.size(), errptr);
            if (*errptr) {
                return false;
            }
            nums_info.push_back(t);
        }
        std::sort(nums_info.begin(), nums_info.end());
        return true;
    }

   protected:
    std::vector<num_cmp_info_t> nums_info;
};
}  // namespace ultrascan
#endif
