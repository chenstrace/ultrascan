#ifndef USCAN_DB_IPCMP_DATABASE_H_
#define USCAN_DB_IPCMP_DATABASE_H_
#include "database.h"
#include "utils.h"
namespace ultrascan {

typedef struct {
    int id;
    ngx_cidr_t cidr;
    bool mask;
} ip_cmp_info_t;

class IPCmpDatabase : public Database {
   public:
    IPCmpDatabase() = default;
    IPCmpDatabase(const IPCmpDatabase&) = delete;

    IPCmpDatabase& operator=(const IPCmpDatabase&) = delete;

    virtual ~IPCmpDatabase() = default;
};
}  // namespace ultrascan
#endif /* USCAN_DB_IPCMP_DATABASE_H_ */
