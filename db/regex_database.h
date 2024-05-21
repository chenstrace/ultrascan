#ifndef USCAN_DB_REGEX_DATABASE_H_
#define USCAN_DB_REGEX_DATABASE_H_
#include <assert.h>
#include <hs.h>
#include <pcre.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
namespace ultrascan {

static bool is_hyperscan_supported(const char* pattern);
class RegexDatabase : public Database {
   public:
    RegexDatabase() : database_(NULL), scratch_(NULL), max_captures_(0) {}

    RegexDatabase(const RegexDatabase&) = delete;

    RegexDatabase& operator=(const RegexDatabase&) = delete;

    virtual ~RegexDatabase() { Destroy(); }
    virtual bool Init(const std::vector<pattern_info_t>& info, char** errptr) {
        std::vector<const char*> patterns;
        std::vector<unsigned int> ids;
        std::vector<unsigned int> flags;
        hs_error_t err;
        *errptr = NULL;
        hs_database_t* database = NULL;
        hs_scratch_t* scratch = NULL;
        hs_compile_error_t* compile_err = NULL;

        if (info.size() == 0) {
            *errptr = strdup("empty pattern info");
            goto failed;
        }

        for (size_t i = 0; i < info.size(); i++) {
            int id = info[i].id;
            std::string pattern = info[i].pattern;
            bool use_pcre = info[i].use_pcre;

            if (is_hyperscan_supported(pattern.c_str()) && !use_pcre) {
                flags.push_back(HS_FLAG_SINGLEMATCH);
                patterns.push_back(pattern.c_str());
                ids.push_back(id);
                hs_id_.push_back(id);
            } else {
                int options = 0;
                const char* pcre_errptr = NULL;
                int erroffset = -1;

                void* (*old_malloc_func)(size_t) = pcre_malloc;
                void (*old_free_func)(void*) = pcre_free;

                pcre_malloc = malloc;
                pcre_free = free;
                pcre* re = pcre_compile(pattern.c_str(), options, &pcre_errptr,
                                        &erroffset, NULL);
                pcre_malloc = old_malloc_func;
                pcre_free = old_free_func;

                if (re == NULL) {
                    std::string msg = "regex pattern " + pattern +
                                      " compile failed, " + pcre_errptr;
                    *errptr = strdup(msg.c_str());
                    goto failed;
                }
                int captures = 0;
                int rc =
                    pcre_fullinfo(re, NULL, PCRE_INFO_CAPTURECOUNT, &captures);
                if (rc < 0) {
                    std::string msg =
                        "regex pattern " + pattern + " get capture info failed";
                    *errptr = strdup(msg.c_str());
                    goto failed;
                }
                if (captures > 0) {
                    max_captures_ = std::max(max_captures_, captures);
                }
                pcre_malloc = malloc;
                pcre_free = free;
                pcre_extra* extra =
                    pcre_study(re, PCRE_STUDY_JIT_COMPILE, &pcre_errptr);
                pcre_malloc = old_malloc_func;
                pcre_free = old_free_func;
                if (pcre_errptr != NULL) {
                    std::string msg = "regex pattern " + pattern +
                                      " JIT compile failed, " + pcre_errptr;
                    *errptr = strdup(msg.c_str());
                    goto failed;
                }

                pcre_re_.push_back(re);
                pcre_extra_.push_back(extra);
                pcre_id_.push_back(id);
            }
        }
        pcre_ovector_.assign(3 * (max_captures_ + 1), 0);

        if (patterns.size() == 0) {
            return true;
        }

        err = hs_compile_multi(patterns.data(), flags.data(), ids.data(),
                               patterns.size(), HS_MODE_BLOCK, NULL, &database,
                               &compile_err);

        if (err != HS_SUCCESS) {
            if (compile_err) {
                std::string msg = std::string("regex pattern compile failed, ");
                msg += std::string("error pattern id:") +
                       std::to_string(compile_err->expression) + ", ";
                msg += std::string("error msg: ") + compile_err->message;
                *errptr = strdup(msg.c_str());
                hs_free_compile_error(compile_err);
            } else {
                *errptr = strdup("hs_compile_multi failed");
            }
            goto failed;
        }
        database_ = database;

        err = hs_alloc_scratch(database, &scratch);
        if (err != HS_SUCCESS) {
            *errptr = strdup("hs_alloc_scratch failed");
            goto failed;
        }

        scratch_ = scratch;
        return true;
    failed:
        Destroy();
        return false;
    }

   private:
    void Destroy() {
        if (scratch_) {
            hs_free_scratch(scratch_);
            scratch_ = NULL;
        }
        if (database_) {
            hs_free_database(database_);
            database_ = NULL;
        }

        for (size_t i = 0; i < pcre_extra_.size(); i++) {
            if (pcre_extra_[i]) {
                pcre_free_study(pcre_extra_[i]);
            }
        }

        for (size_t i = 0; i < pcre_re_.size(); i++) {
            if (pcre_re_[i]) {
                pcre_free(pcre_re_[i]);
            }
        }
    }

   protected:
    hs_database_t* database_;
    hs_scratch_t* scratch_;

    std::vector<int> hs_id_;
    int max_captures_;
    std::vector<pcre*> pcre_re_;
    std::vector<pcre_extra*> pcre_extra_;
    std::vector<int> pcre_id_;
    std::vector<int> pcre_ovector_;
};

static bool is_hyperscan_supported(const char* pattern) {
    hs_database_t* db = NULL;
    hs_compile_error_t* error = NULL;
    if (pattern == NULL) {
        return false;
    }
    hs_error_t err = hs_compile(pattern, HS_FLAG_SINGLEMATCH, HS_MODE_BLOCK,
                                NULL, &db, &error);
    if (err != HS_SUCCESS) {
        hs_free_compile_error(error);
        return false;
    }
    hs_free_database(db);
    return true;
}

}  // namespace ultrascan

#endif /* USCAN_DB_REGEX_DATABASE_H_ */
