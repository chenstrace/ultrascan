#ifndef DB_IP_NOTINLIST_DATABASE_H_
#define DB_IP_NOTINLIST_DATABASE_H_

#include <assert.h>
#include <string.h>

#include "ipcmp_database.h"
namespace ultrascan {
class IPNotInListDatabase : public IPCmpDatabase {
   public:
    IPNotInListDatabase() = default;
    IPNotInListDatabase(const IPNotInListDatabase&) = delete;

    IPNotInListDatabase& operator=(const IPNotInListDatabase&) = delete;

    virtual ~IPNotInListDatabase() = default;
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
                std::string msg = "ip notinlist: split " + info[i].pattern +
                                  " to array failed";
                *errptr = strdup(msg.c_str());
                return false;
            }

            std::vector<ip_cmp_info_t> cmp_info;
            //一个条件中的一个或多个IP
            for (size_t j = 0; j < iplist.size(); j++) {
                ip_cmp_info_t temp;
                temp.id = id;
                me_str_t text;
                text.data = (u_char*)iplist[j].c_str();
                text.len = iplist[j].length();

                if (ngx_ptocidr(&text, &temp.cidr) == NGX_ERROR) {
                    std::string msg = "ip notinlist: convert " + iplist[j] +
                                      " to cidr failed";
                    *errptr = strdup(msg.c_str());
                    return false;
                }

                if (temp.cidr.family != AF_INET &&
                    temp.cidr.family != AF_INET6) {
                    std::string msg =
                        "ip notinlist: " + iplist[j] + " error family";
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
                } else {
                    if (memcmp(temp.cidr.u.in6.mask.s6_addr, ff, 16) == 0) {
                        temp.mask = false;
                    } else {
                        temp.mask = true;
                    }
                }
                cmp_info.push_back(temp);
            }

            ip_list_.push_back(cmp_info);
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
        if (ip_list_.size() == 0) {
            return;
        }

        ngx_uint_t input_family = cidr.family;

        for (size_t i = 0; i < ip_list_.size(); i++) {
            //每一个条件
            std::vector<ip_cmp_info_t>& items = ip_list_[i];
            // items存放的是一个条件中的多个notinlist IP，所以这些IP的id相同
            // 1.1.1.1
            // notinlist 2.2.2.2,5.5.5.5,1050:0000:0000:0000:0005:0600:300c:326b
            //当list中所有IP都不等于1.1.1.1时，notinlist条件成立
            bool ok = true;
            for (size_t j = 0; j < items.size(); j++) {
                ip_cmp_info_t& cmp_ip = items[j];
                if (input_family != cmp_ip.cidr.family) {
                    // v4 notinlist v6 或者 v6 notinlist v4，必然成立
                    continue;
                }
                //单个条件中的每一个IP
                if (input_family == AF_INET && cmp_ip.cidr.family == AF_INET) {
                    // IPV4的情况
                    if (!cmp_ip.mask) {
                        //不带掩码的情况
                        if (cidr.u.in.addr == cmp_ip.cidr.u.in.addr) {
                            //相等，说明notinlist不成立
                            ok = false;
                            break;
                        }
                    } else {
                        // 192.168.1.5 notinlist 192.168.1.0/24
                        //此时相当于belong操作
                        if ((cidr.u.in.addr & cmp_ip.cidr.u.in.mask) ==
                            cmp_ip.cidr.u.in.addr) {
                            ok = false;
                            break;
                        }
                    }
                } else if (input_family == AF_INET6 &&
                           cmp_ip.cidr.family == AF_INET6) {
                    if (!cmp_ip.mask) {
                        uint8_t* a = cidr.u.in6.addr.s6_addr;  // 16 bytes
                        uint8_t* b =
                            cmp_ip.cidr.u.in6.addr.s6_addr;  // 16 bytes
                        bool match = true;
                        for (int k = 0; k < 16; k++) {
                            if (a[k] != b[k]) {
                                match = false;
                                break;
                            }
                        }

                        if (match) {
                            //如果match，说明是inlist，而不是notinlist，所以条件不满足
                            ok = false;
                            break;
                        }
                    } else {
                        //同ipv4，这里相当于belong操作

                        bool match = true;
                        for (int n = 0; n < 16; n++) {
                            if ((cidr.u.in6.addr.s6_addr[n] &
                                 cmp_ip.cidr.u.in6.mask.s6_addr[n]) !=
                                cmp_ip.cidr.u.in6.addr.s6_addr[n]) {
                                match = false;
                                break;
                            }
                        }

                        if (match) {
                            ok = false;
                            break;
                        }
                    }
                } else {
                    //理论上不可能到这里
                    assert(0);
                    ok = false;
                    break;
                }
            }

            if (ok) {
                if (callback(items[0].id, user_data, NULL, 0) != 0) {
                    return;
                }
            }
        }
    }

   private:
    std::vector<std::vector<ip_cmp_info_t>> ip_list_;
};
}  // namespace ultrascan

#endif
