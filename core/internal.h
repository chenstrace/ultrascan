#ifndef USCAN_CORE_INTERNAL_H_
#define USCAN_CORE_INTERNAL_H_
#include <string>
namespace ultrascan {

enum db_type_t {
    DB_TYPE_BEGIN = 0,
    DB_TYPE_LEN_EQUAL = 0,
    DB_TYPE_LEN_NOT_EQUAL,
    DB_TYPE_LEN_LESS_THAN,
    DB_TYPE_LEN_GREAT_THAN,
    DB_TYPE_IP_IN_LIST,
    DB_TYPE_IP_BELONG,
    DB_TYPE_NOT_INCLUDE,
    DB_TYPE_REGEX_MATCH,
    DB_TYPE_REGEX_NOT_MATCH,

    DB_TYPE_NUM_EQUAL,
    DB_TYPE_NUM_NOT_EQUAL,
    DB_TYPE_NUM_LESS_THAN,
    DB_TYPE_NUM_GREAT_THAN,

    DB_TYPE_IP_NOT_IN_LIST,
    DB_TYPE_IP_NOT_BELONG,
    DB_TYPE_END,
    DB_TYPE_COUNT = DB_TYPE_END,
};

typedef struct pattern_info_t {
    int id;
    std::string pattern;
    bool use_pcre;
} pattern_info_t;
}  // namespace ultrascan

#endif /* USCAN_CORE_INTERNAL_H_ */
