#ifndef USCAN_DB_DATABASE_FACTORY_H_
#define USCAN_DB_DATABASE_FACTORY_H_
#include "database.h"
#include "internal.h"
#include "ip_belong_database.h"
#include "ip_inlist_database.h"
#include "leneq_database.h"
#include "lengt_database.h"
#include "lenlt_database.h"
#include "lenneq_database.h"
#include "notinclude_database.h"
#include "numeq_database.h"
#include "numgt_database.h"
#include "numlt_database.h"
#include "numneq_database.h"
#include "regex_match_database.h"
#include "regex_not_match_database.h"
#include "ip_notbelong_database.h"
#include "ip_notinlist_database.h"
namespace ultrascan {
class Factory {
   public:
    static Database* Create(int type) {
        switch (type) {
            case DB_TYPE_LEN_EQUAL:
                return new LenEqualDatabase();
            case DB_TYPE_LEN_NOT_EQUAL:
                return new LenNotEqualDatabase();
            case DB_TYPE_LEN_LESS_THAN:
                return new LenLessThanDatabase();
            case DB_TYPE_LEN_GREAT_THAN:
                return new LenGreateThanDatabase();
            case DB_TYPE_IP_IN_LIST:
                return new IPInListDatabase();
            case DB_TYPE_IP_BELONG:
                return new IPBelongDatabase();
            case DB_TYPE_NOT_INCLUDE:
                return new NotIncludeDatabase();
            case DB_TYPE_REGEX_MATCH:
                return new RegexMatchDatabase();
            case DB_TYPE_REGEX_NOT_MATCH:
                return new RegexNotMatchDatabase();
            case DB_TYPE_NUM_EQUAL:
                return new NumEqualDatabase();
            case DB_TYPE_NUM_GREAT_THAN:
                return new NumGreateThanDatabase();
            case DB_TYPE_NUM_LESS_THAN:
                return new NumLessThanDatabase();
            case DB_TYPE_NUM_NOT_EQUAL:
                return new NumNotEqualDatabase();
            case DB_TYPE_IP_NOT_IN_LIST:
                return new IPNotInListDatabase();
            case DB_TYPE_IP_NOT_BELONG:
                return new IPNotBelongDatabase();
            default:
                break;
        }
        return NULL;
    }
};
}  // namespace ultrascan

#endif /* USCAN_DB_DATABASE_FACTORY_H_ */
