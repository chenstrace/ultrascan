#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "c.h"
#include "cJSON/cJSON.h"

#define me_string(str) \
    { sizeof(str) - 1, (unsigned char*)str }

static off_t filesize(const char* filename) {
    struct stat buf;

    if (!filename) {
        return 0;
    }

    return stat(filename, &buf) == 0 ? buf.st_size : 0;
}

static char* readfile(const char* filename, off_t* size) {
    char* buf = NULL;
    FILE* fp = NULL;

    if (!filename) {
        return NULL;
    }

    if (!size) {
        return NULL;
    }
    fp = fopen(filename, "rb");
    if (!fp) {
        goto failed;
    }
    *size = filesize(filename);
    if (*size == 0) {
        goto failed;
    }

    buf = (char*)malloc(*size);
    if (!buf) {
        goto failed;
    }

    if (fread(buf, *size, 1, fp) != 1) {
        goto failed;
    }
    fclose(fp);
    return buf;

failed:
    if (fp) {
        fclose(fp);
    }
    if (buf) {
        free(buf);
    }
    return NULL;
}

match_opt_t convert_opt(const char* opt) {
    match_opt_t ret = OPT_NONE;

    if (strncasecmp(opt, "match", 5) == 0) {
        ret = OPT_MATCH;
    } else if (strncasecmp(opt, "include", 7) == 0) {
        ret = OPT_INCLUDE;
    } else if (strncasecmp(opt, "eq", 2) == 0) {
        ret = OPT_EQ;
    } else if (strncasecmp(opt, "leneq", 5) == 0) {
        ret = OPT_LEN_EQ;
    } else if (strncasecmp(opt, "lennoteq", 8) == 0) {
        ret = OPT_LEN_NOT_EQ;
    } else if (strncasecmp(opt, "lengt", 5) == 0) {
        ret = OPT_LEN_GT;
    } else if (strncasecmp(opt, "lenlt", 5) == 0) {
        ret = OPT_LEN_LT;
    } else if (strncasecmp(opt, "notinclude", 10) == 0) {
        ret = OPT_NOT_INCLUDE;
    } else if (strncasecmp(opt, "belong", 6) == 0) {
        ret = OPT_IP_BELONG;
    } else if (strncasecmp(opt, "inlist", 6) == 0) {
        ret = OPT_IP_IN_LIST;
    }

    return ret;
}

static bool handle_helper(const char* host, cJSON* json, us_fieldset_t* fset,
                          us_ruleset_t* ruleset) {
    int i;
    const int MAX_ONE_RULE_CONDITION_COUNT = 100;
    const char* field_names[MAX_ONE_RULE_CONDITION_COUNT];
    match_opt_t opts[MAX_ONE_RULE_CONDITION_COUNT];
    const char* patterns[MAX_ONE_RULE_CONDITION_COUNT];

    if (json == NULL) {
        return true;
    }
    cJSON* ptr_exact_access = cJSON_GetObjectItem(json, "exact_access");
    if (ptr_exact_access == NULL) {
        return true;
    }

    cJSON* ptr_access_rule =
        cJSON_GetObjectItem(ptr_exact_access, "access_rule");
    if (ptr_access_rule == NULL) {
        return true;
    }

    cJSON* ptr_rule = ptr_access_rule->child;

    while (ptr_rule) {
        if (ptr_rule->type != cJSON_Array) {
            return false;
        }
        int condition_count = cJSON_GetArraySize(ptr_rule);
        if (condition_count == 0) {
            return false;
        }
        const char* rule_name = ptr_rule->string;
        // fprintf(stderr, "rule name %s begin\n", rule_name);
        const char* tag = NULL;

        for (i = 0; i < condition_count && i < MAX_ONE_RULE_CONDITION_COUNT;
             i++) {
            cJSON* condition = cJSON_GetArrayItem(ptr_rule, i);

            cJSON* ptr_type = cJSON_GetObjectItem(condition, "Type");
            cJSON* ptr_opt = cJSON_GetObjectItem(condition, "Opt");
            cJSON* ptr_rule_content =
                cJSON_GetObjectItem(condition, "Rule_content");
            cJSON* ptr_action = cJSON_GetObjectItem(condition, "Action");

            if (ptr_type == NULL) {
                return false;
            }
            if (ptr_opt == NULL) {
                return false;
            }
            if (ptr_rule_content == NULL) {
                return false;
            }
            if (ptr_action == NULL) {
                return false;
            }

            char* type = ptr_type->valuestring;
            char* opt = ptr_opt->valuestring;
            char* rule_content = ptr_rule_content->valuestring;
            char* action = ptr_action->valuestring;

            if (type == NULL) {
                return false;
            }
            if (opt == NULL) {
                return false;
            }
            if (rule_content == NULL) {
                return false;
            }
            if (action == NULL) {
                return false;
            }
            match_opt_t match_opt = convert_opt(opt);
            if (match_opt == OPT_NONE) {
                return false;
            }

            field_names[i] = type;
            opts[i] = match_opt;
            patterns[i] = rule_content;

            if (i == condition_count - 1) {
                tag = action;
            }
        }

        if (host != NULL) {
            field_names[condition_count] = "HOST";
            opts[condition_count] = OPT_EQ, patterns[condition_count] = host;
            condition_count++;
        }
        char* errptr = NULL;
        us_ruleset_add_rule(ruleset, rule_name, tag, field_names, opts, patterns,
                         NULL, condition_count, NULL, &errptr);

        // fprintf(stderr, "rule name %s end\n", rule_name);
        // move to next rule
        ptr_rule = ptr_rule->next;
    }
    return true;
}
static bool handle_default(cJSON* json, us_fieldset_t* fset, us_ruleset_t* ruleset) {
    if (!json) {
        return true;
    }

    return handle_helper(NULL, json, fset, ruleset);
}

static bool handle_domain(cJSON* json, us_fieldset_t* fset, us_ruleset_t* ruleset) {
    if (!json) {
        return true;
    }
    bool b;
    cJSON* ptr_domain = json->child;
    while (ptr_domain) {
        b = handle_helper(ptr_domain->string, ptr_domain, fset, ruleset);
        if (!b) {
            return false;
        }
        ptr_domain = ptr_domain->next;
    }
    return true;
}

static int match_callback(const char* name, const char* tag,const unsigned char* payload, int payload_len, void* user_data) {
    printf("matched, rule name is %s, tag is %s\n", name, tag);
    return 0;
}
int main(int argc, char** argv) {
    bool success;
    us_fieldset_t* fieldset = NULL;
    us_ruleset_t* ruleset = NULL;
    char* errptr = NULL;

    field_item_t field_list[] = {
        {0, "HOST",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {1, "SCENE",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {2, "METHOD",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {3, "TICKET",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {4, "TOKEN",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {5, "COOKIE",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {6, "IP", OPT_IP_IN_LIST | OPT_IP_BELONG},
        {7, "URL",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {8, "REF",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {9, "UA",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {10, "ARGS",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {11, "HEADER",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
        {12, "BODY",
         OPT_MATCH | OPT_INCLUDE | OPT_EQ | OPT_LEN_EQ | OPT_LEN_NOT_EQ |
             OPT_LEN_GT | OPT_LEN_LT | OPT_NOT_INCLUDE},
    };

    const int field_list_size = sizeof(field_list) / sizeof(field_list[0]);
    off_t size = 0;
    char* buf = readfile("anticc.json", &size);
    assert(buf);
    cJSON* json = cJSON_Parse(buf);
    assert(json);

    fieldset = us_fieldset_create(field_list, field_list_size, &errptr);
    ruleset = us_ruleset_create(fieldset, &errptr);

    assert(fieldset);
    assert(ruleset);

    cJSON* ptr_default = cJSON_GetObjectItem(json, "default");
    handle_default(ptr_default, fieldset, ruleset);

    cJSON* ptr_host = cJSON_GetObjectItem(json, "host");
    handle_domain(ptr_host, fieldset, ruleset);

    cJSON_Delete(json);
    free(buf);

    input_item_t g_input[] = {
        {7, "URL", me_string("/server/msgmonitor/update")},
        {9, "UA",
         me_string(
             "Android/11 didihttp OneNet/2.1.1.11 com.sdu.didi.psnger/6.2.4")},
        {0, "HOST", me_string("msggate.xiaojukeji.com")},
        {11, "HEADER",
         me_string(
             "Accept:*/*\r\nUser-Agent:Android/11 didihttp OneNet/2.1.1.11 "
             "com.sdu.didi.psnger/"
             "6.2.4\r\nHost:msggate.xiaojukeji.com\r\n\r\n")},
        {4, "TOKEN",
         me_string("RPqEOv6imxAtxQkRiIqWm7DblxRsvuYnbOisqC5GxGUkzDtuAm"
                   "EMAOG7TG2tbP-vtdv0uUMem0DzI4GoVtwdLdQz-"
                   "namkpRFF0WYRpownTRVVWEW0kYLbx5FvceRK2m9dK-lahNmI_"
                   "n4RPgiQfgmfbUYpj1qFK8h_JKtChu5c7vcrz8bqQ_"
                   "h77B8rX28rH8SW8O7jlA3hNPbPB_7MwAA__8=")},
        {6, "IP", me_string("127.0.0.1")},
        {2, "METHOD", me_string("GET")},
        {10, "ARGS",
         me_string("app_type=1&state=3&p_id=5364848241437336943&token="
                   "RPqEOv6imxAtxQkRiIqWm7DblxRsvuYnbOisqC5GxGUkzDtuAmE"
                   "MAOG7TG2tbP-vtdv0uUMem0DzI4GoVtwdLdQz-"
                   "namkpRFF0WYRpownTRVVWEW0kYLbx5FvceRK2m9dK-lahNmI_"
                   "n4RPgiQfgmfbUYpj1qFK8h_JKtChu5c7vcrz8bqQ_"
                   "h77B8rX28rH8SW8O7jlA3hNPbPB_7MwAA__8=")},
    };

    const int g_input_count = sizeof(g_input) / sizeof(g_input[0]);

    us_ruleset_set_match_callback(ruleset, match_callback, NULL);
    struct timeval tv1, tv2, tv_res;
    int elapsed_in_us = 0;
    success = us_ruleset_compile(ruleset, &errptr);
    if (!success) {
        printf("%s\n", errptr);
        return EXIT_FAILURE;
    }
    assert(success);
    int i;
    gettimeofday(&tv1, NULL);
    const int N = 1000;
    for (i = 0; i < N; i++) {
        us_ruleset_match(ruleset, g_input, g_input_count);
    }
    gettimeofday(&tv2, NULL);
    timersub(&tv2, &tv1, &tv_res);
    elapsed_in_us = tv_res.tv_sec * 1000000 + tv_res.tv_usec;
    fprintf(stderr, "elapsed %d us\n", elapsed_in_us / N);

    us_ruleset_destroy(ruleset);
    us_fieldset_destroy(fieldset);

    return 0;
}
