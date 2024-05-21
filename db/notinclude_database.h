#ifndef USCAN_DB_NOTINCLUDE_DATABASE_H_
#define USCAN_DB_NOTINCLUDE_DATABASE_H_
#include <string>
#include <vector>

#include "database.h"
#include "types.h"
#include "utils.h"
namespace ultrascan {

typedef struct {
    int id;
    std::string str;
} not_include_info;

class NotIncludeDatabase : public Database {
   public:
    NotIncludeDatabase() = default;
    NotIncludeDatabase(const NotIncludeDatabase&) = delete;

    NotIncludeDatabase& operator=(const NotIncludeDatabase&) = delete;

    virtual ~NotIncludeDatabase() = default;
    virtual bool Init(const std::vector<pattern_info_t>& info, char** errptr) {
        *errptr = NULL;
        for (size_t i = 0; i < info.size(); i++) {
            not_include_info nii;
            nii.id = info[i].id;
            nii.str = info[i].pattern;
            list_.push_back(nii);
        }
        return true;
    }

    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) {
        if (str == NULL || str->data == NULL || str->len == 0) {
            for (size_t i = 0; i < list_.size(); i++) {
                int ret = callback(list_[i].id, user_data,NULL,0);
                if (ret != 0) {
                    break;
                }
            }
            return;
        }
        for (size_t i = 0; i < list_.size(); i++) {
            u_char* pos = strnstr(str->data, str->len, list_[i].str.c_str(),
                                  list_[i].str.length());
            if (!pos) {
                int ret = callback(list_[i].id, user_data,NULL,0);
                if (ret != 0) {
                    break;
                }
            } else {
                // include the string, do nothing here
            }
        }
    }

   protected:
    std::vector<not_include_info> list_;
};
}  // namespace ultrascan

#endif /* USCAN_DB_NOTINCLUDE_DATABASE_H_ */
