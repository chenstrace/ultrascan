#ifndef DB_REGEX_NOT_MATCH_DATABASE_H_
#define DB_REGEX_NOT_MATCH_DATABASE_H_
#include <assert.h>
#include <hs.h>
#include <pcre.h>
#include <stdlib.h>
#include <string.h>

#include <unordered_map>
#include <unordered_set>

#include "regex_database.h"
namespace ultrascan {
class RegexNotMatchDatabase : public RegexDatabase {
   public:
    RegexNotMatchDatabase() = default;
    RegexNotMatchDatabase(const RegexNotMatchDatabase&) = delete;
    RegexNotMatchDatabase& operator=(const RegexNotMatchDatabase&) = delete;

   public:
    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) {
        if (str == NULL || str->data == NULL || str->len == 0) {
            for (int id : pcre_id_) {
                if (callback) {
                    int ret = callback(id, user_data, NULL, 0);
                    if (ret != 0) {
                        return;
                    }
                }
            }

            for (int id : hs_id_) {
                if (callback) {
                    int ret = callback(id, user_data, NULL, 0);
                    if (ret != 0) {
                        return;
                    }
                }
            }
            return;
        }

        for (size_t i = 0; i < pcre_id_.size(); i++) {
            int rc = pcre_exec(pcre_re_[i], pcre_extra_[i],
                               (const char*)str->data, str->len, 0, 0,
                               pcre_ovector_.data(), pcre_ovector_.size());

            if (rc == PCRE_ERROR_NOMATCH) {
                if (callback) {
                    int ret = callback(pcre_id_[i], user_data, str->data,
                                       str->len <= 32 ? str->len : 32);
                    if (ret != 0) {
                        return;
                    }
                }
            }
        }

        if (hs_id_.size() == 0) {
            return;
        }

        std::unordered_set<int> matched;

        hs_scan(database_, (const char*)str->data, str->len, 0, scratch_,
                internal_callback, &matched);

        for (int id : hs_id_) {
            auto it = matched.find(id);
            if (it == matched.end()) {
                //没命中， 执行用户callback
                if (callback) {
                    int ret = callback(id, user_data, str->data,
                                       str->len <= 32 ? str->len : 32);
                    if (ret != 0) {
                        return;
                    }
                }
            }
        }
    }

    static int internal_callback(unsigned int id, unsigned long long from,
                                 unsigned long long to, unsigned int flags,
                                 void* context) {
        std::unordered_set<int>* matched = (std::unordered_set<int>*)context;

        matched->insert(id);
        return 0;
    }
};

}  // namespace ultrascan

#endif /* DB_REGEX_NOT_MATCH_DATABASE_H_ */
