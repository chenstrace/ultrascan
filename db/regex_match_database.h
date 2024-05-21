#ifndef DB_REGEX_MATCH_DATABASE_H_
#define DB_REGEX_MATCH_DATABASE_H_
#include <assert.h>
#include <hs.h>
#include <pcre.h>
#include <stdlib.h>
#include <string.h>

#include "regex_database.h"
namespace ultrascan {

static int internal_callback(unsigned int id, unsigned long long from,
                             unsigned long long to, unsigned int flags,
                             void* context);
class RegexMatchDatabase : public RegexDatabase {
   public:
    RegexMatchDatabase() : user_callback_(NULL), user_data_(NULL) {}

    RegexMatchDatabase(const RegexMatchDatabase&) = delete;

    RegexMatchDatabase& operator=(const RegexMatchDatabase&) = delete;


    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) {
        if (str == NULL || str->data == NULL || str->len == 0) {
            return;
        }

        int options = 0;
        int rc;
        for (size_t i = 0; i < pcre_id_.size(); i++) {
            rc = pcre_exec(pcre_re_[i], pcre_extra_[i], (const char*)str->data,
                           str->len, 0, options, pcre_ovector_.data(),
                           pcre_ovector_.size());

            if (rc < 0) {
                continue;
            }
            /*
            The value returned by pcre_exec() is one more than the highest
            numbered pair that has been set. For example, if two substrings have
            been captured, the returned value is 3. If there are no capturing
            subpatterns, the return value from a successful match is 1,
            indicating that just the first pair of offsets has been set.
            */

            /*
            If  the vector is too small to hold
            all the captured substring offsets, it is used as far as possible
            (up to two-thirds of its length), and the function returns a value
            of zero*/

            if (callback) {
                int ret = callback(pcre_id_[i], user_data,
                                   str->data + pcre_ovector_[0],
                                   pcre_ovector_[1] - pcre_ovector_[0]);
                if (ret != 0) {
                    return;
                }
            }
        }

        user_callback_ = callback;
        user_data_ = user_data;

        hs_scan(database_, (const char*)str->data, str->len, 0, scratch_,
                internal_callback, this);
    }

    scan_db_callback GetCallback() { return user_callback_; }

    void* GetUserData() { return user_data_; }

   private:
    scan_db_callback user_callback_;
    void* user_data_;
};

static int internal_callback(unsigned int id, unsigned long long from,
                             unsigned long long to, unsigned int flags,
                             void* context) {
    RegexMatchDatabase* database = (RegexMatchDatabase*)context;

    scan_db_callback callback = database->GetCallback();
    void* user_data = database->GetUserData();
    if (callback) {
        return callback(id, user_data, NULL, 0);
    }
    return 0;
}
}  // namespace ultrascan

#endif /* DB_REGEX_MATCH_DATABASE_H_ */
