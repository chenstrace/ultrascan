#ifndef USCAN_DB_DATABASE_H_
#define USCAN_DB_DATABASE_H_
#include <vector>

#include "internal.h"
#include "types.h"

namespace ultrascan {

typedef int (*scan_db_callback)(unsigned int id, void* context, unsigned char* payload, int payload_len);

class Database {
   public:
    Database() = default;
    Database(const Database&) = delete;

    Database& operator=(const Database&) = delete;

    virtual ~Database() = default;
    virtual bool Init(const std::vector<pattern_info_t>& info,
                      char** errptr) = 0;
    virtual void Scan(me_str_t* str, scan_db_callback callback,
                      void* user_data) = 0;
};
}  // namespace ultrascan

#endif /* USCAN_DB_DATABASE_H_ */
