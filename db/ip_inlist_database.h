#ifndef USCAN_DB_IP_INLIST_DATABASE_H_
#define USCAN_DB_IP_INLIST_DATABASE_H_
#include <assert.h>
#include <string.h>

#include <sstream>
#include <string>
#include <vector>

#include "ipcmp_database.h"
namespace ultrascan {
class IPInListDatabase : public IPCmpDatabase {
   public:
    IPInListDatabase() = default;
    IPInListDatabase(const IPInListDatabase&) = delete;

    IPInListDatabase& operator=(const IPInListDatabase&) = delete;

    virtual ~IPInListDatabase() = default;
    virtual bool Init(const std::vector<pattern_info_t>& info, char** errptr) {
        *errptr = NULL;
        unsigned char ff[16];

        memset(ff, 0xff, 16);

        for (size_t i = 0; i < info.size(); i++) {
            //处理每一个条件
            int id = info[i].id;
            //一个条件中的多个IP以逗号分隔
            std::vector<std::string> iplist = split(info[i].pattern, ',');

            if (iplist.empty()) {
                std::string msg =
                    "ip inlist: split " + info[i].pattern + " to array failed";
                *errptr = strdup(msg.c_str());
                return false;
            }

            std::vector<ip_cmp_info_t> cmp_info_v4;
            std::vector<ip_cmp_info_t> cmp_info_v6;
            //一个条件中的一个或多个IP
            for (size_t j = 0; j < iplist.size(); j++) {
                ip_cmp_info_t temp;
                temp.id = id;
                me_str_t text;
                text.data = (u_char*)iplist[j].c_str();
                text.len = iplist[j].length();

                if (ngx_ptocidr(&text, &temp.cidr) == NGX_ERROR) {
                    std::string msg =
                        "ip inlist: convert " + iplist[j] + " to cidr failed";
                    *errptr = strdup(msg.c_str());
                    return false;
                }

                if (temp.cidr.family == AF_INET) {
                    if (temp.cidr.u.in.mask == 0xffffffff) {
                        // 普通IP, 例如192.168.1.5
                        temp.mask = false;
                    } else {
                        // 网段, 例如192.168.1.0/24
                        temp.mask = true;
                    }
                    cmp_info_v4.push_back(temp);
                } else {
                    if (memcmp(temp.cidr.u.in6.mask.s6_addr, ff, 16) == 0) {
                        temp.mask = false;
                    } else {
                        temp.mask = true;
                    }

                    cmp_info_v6.push_back(temp);
                }
            }

            if (!cmp_info_v4.empty()) {
                ipv4_list_.push_back(cmp_info_v4);
            }

            if (!cmp_info_v6.empty()) {
                ipv6_list_.push_back(cmp_info_v6);
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
        if (!callback) {
            return;
        }
        if (cidr.family == AF_INET) {
            for (size_t i = 0; i < ipv4_list_.size(); i++) {
                std::vector<ip_cmp_info_t>& items = ipv4_list_[i];
                // items存放的是一个条件中的多个inlist IP，所以这些IP的id相同
                for (size_t j = 0; j < items.size(); j++) {
                    if (!items[j].mask) {
                        if (cidr.u.in.addr == items[j].cidr.u.in.addr) {
                            if (callback(items[j].id, user_data, NULL, 0) !=
                                0) {
                                return;
                            }
                            /*
                            这里break是避免命中一个条件时重复回调。
                            比如1.1.1.1 inlist "1.1.1.1,2.2.2.2,1.1.1.1"，
                            如果不break，会回调两次。
                            */
                            break;
                        }
                    } else {
                        // 192.168.1.5 inlist 192.168.1.0/24
                        //此时相当于belong操作

                        if ((cidr.u.in.addr & items[j].cidr.u.in.mask) ==
                            items[j].cidr.u.in.addr) {
                            if (callback(items[j].id, user_data, NULL, 0) !=
                                0) {
                                return;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (cidr.family == AF_INET6) {
            for (size_t i = 0; i < ipv6_list_.size(); i++) {
                std::vector<ip_cmp_info_t>& items = ipv6_list_[i];
                for (size_t j = 0; j < items.size(); j++) {
                    if (!items[j].mask) {
                        uint8_t* a = cidr.u.in6.addr.s6_addr;  // 16 bytes
                        uint8_t* b =
                            items[j].cidr.u.in6.addr.s6_addr;  // 16 bytes
                        bool match = true;
                        for (int k = 0; k < 16; k++) {
                            if (a[k] != b[k]) {
                                match = false;
                                break;
                            }
                        }

                        if (match) {
                            if (callback(items[j].id, user_data, NULL, 0) !=
                                0) {
                                return;
                            }
                            break;  //跳到下一组
                        }
                    } else {
                        //同ipv4，这里相当于belong操作

                        bool match = true;
                        for (int n = 0; n < 16; n++) {
                            if ((cidr.u.in6.addr.s6_addr[n] &
                                 items[j].cidr.u.in6.mask.s6_addr[n]) !=
                                items[j].cidr.u.in6.addr.s6_addr[n]) {
                                match = false;
                                break;
                            }
                        }

                        if (match) {
                            if (callback(items[j].id, user_data, NULL, 0) !=
                                0) {
                                return;
                            }
                            break;  //跳到下一组
                        }
                    }
                }
            }

        } else {
            assert(0);
        }
    }

   private:
    std::vector<std::vector<ip_cmp_info_t>> ipv4_list_;
    std::vector<std::vector<ip_cmp_info_t>> ipv6_list_;
};
}  // namespace ultrascan

#endif /* USCAN_DB_IP_INLIST_DATABASE_H_ */
