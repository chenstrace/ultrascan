#ifndef DB_IP_NOTBELONG_DATABASE_H_
#define DB_IP_NOTBELONG_DATABASE_H_

#include <assert.h>
#include <string.h>

#include "ipcmp_database.h"
namespace ultrascan {

class IPNotBelongDatabase : public IPCmpDatabase {
   public:
    IPNotBelongDatabase() = default;
    IPNotBelongDatabase(const IPNotBelongDatabase&) = delete;

    IPNotBelongDatabase& operator=(const IPNotBelongDatabase&) = delete;

    virtual ~IPNotBelongDatabase() = default;
    virtual bool Init(const std::vector<pattern_info_t>& info, char** errptr) {
        *errptr = NULL;
        for (size_t i = 0; i < info.size(); i++) {
            int id = info[i].id;
            const std::string& pattern = info[i].pattern;

            ip_cmp_info_t temp;
            temp.id = id;
            me_str_t text;
            text.data = (u_char*)pattern.c_str();
            text.len = pattern.length();

            if (ngx_ptocidr(&text, &temp.cidr) == NGX_ERROR) {
                std::string msg =
                    "ip notbelong: convert " + pattern + " to cidr failed";
                *errptr = strdup(msg.c_str());
                return false;
            }

            if (temp.cidr.family == AF_INET) {
                ipv4_list_.push_back(temp);
            } else if (temp.cidr.family == AF_INET6) {
                ipv6_list_.push_back(temp);
            } else {
                *errptr = strdup("ip notbelong: unknown address family");
                return false;
            }
        }
        return true;
    }

    virtual void Scan(me_str_t* ip, scan_db_callback callback,
                      void* user_data) {
        ngx_cidr_t cidr;
        if (ngx_ptocidr(ip, &cidr) == NGX_ERROR) {
            return;
        }
        // 1.1.1.5 notbelong  1.1.1.0/24
        if (cidr.family == AF_INET) {
            for (size_t i = 0; i < ipv4_list_.size(); i++) {
                if ((cidr.u.in.addr & ipv4_list_[i].cidr.u.in.mask) ==
                    ipv4_list_[i].cidr.u.in.addr) {
                    //到这里，说明是belong，而不是notbelong
                    continue;
                } else {
                    if (callback(ipv4_list_[i].id, user_data, NULL, 0) != 0) {
                        return;
                    }
                }
            }

        } else if (cidr.family == AF_INET6) {
            for (size_t i = 0; i < ipv6_list_.size(); i++) {
                bool match = true;
                for (int n = 0; n < 16; n++) {
                    if ((cidr.u.in6.addr.s6_addr[n] &
                         ipv6_list_[i].cidr.u.in6.mask.s6_addr[n]) !=
                        ipv6_list_[i].cidr.u.in6.addr.s6_addr[n]) {
                        match = false;
                        break;
                    }
                }

                if (match) {
                    continue;
                } else {
                    if (callback(ipv6_list_[i].id, user_data, NULL, 0) != 0) {
                        return;
                    }
                }
            }

        } else {
            assert(0);
        }
    }

   private:
    std::vector<ip_cmp_info_t> ipv4_list_;
    std::vector<ip_cmp_info_t> ipv6_list_;
};
}  // namespace ultrascan

#endif
