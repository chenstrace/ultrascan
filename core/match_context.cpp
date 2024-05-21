#include "match_context.h"

#include "condition_impl.h"
#include "database_factory.h"
#include "fieldset_impl.h"
namespace ultrascan {
static int RootCallback(unsigned int id, void* user_data,
                        unsigned char* payload, int payload_len);
static int NextCallback(unsigned int id, void* user_data,
                        unsigned char* payload, int payload_len);
static bool CompareCondition(const ConditionImpl* a, const ConditionImpl* b);

typedef struct {
    ConditionImpl* next_condition;
    MatchContext* match_context;
} next_callback_data_t;

MatchContext::MatchContext(FieldSet* fieldset)
    : fieldset_(fieldset), callback_(NULL), user_data_(NULL) {
    field_count_ = fieldset_->GetFieldCount();

    //一共有field_count_个字段，那么根的数量最多为field_count_ * DB_TYPE_COUNT
    root_count_ = field_count_ * DB_TYPE_COUNT;

    databases_.assign(root_count_, NULL);
    input_.assign(field_count_, {0, NULL});
    is_finished_root_.assign(root_count_, 0);

    conditions_.assign(1, NULL); /* id从1开始，0的位置不用 */
    InitQueue(&matched_condition_queue_);
}
MatchContext::~MatchContext() {
    for (auto database : databases_) {
        if (database) {
            delete database;
        }
    }

    for (auto condition : conditions_) {
        if (condition) {
            delete condition;
        }
    }
}

void MatchContext::SetMatchCallback(rule_match_callback callback,
                                    void* user_data) {
    callback_ = callback;
    user_data_ = user_data;
}

bool MatchContext::AddRule(const char* rule_name, const char* tag,
                           const char* const* field_names,
                           const match_opt_t* opts, const char* const* patterns,
                           const unsigned int* ids, unsigned int size,
                           const char* logic, char** errptr) {
    assert(rule_name);
    assert(field_names);
    assert(opts);
    assert(patterns);
    assert(size > 0);

    *errptr = NULL;

    if (rule_name == NULL) {
        *errptr = strdup("rule name empty");
        return false;
    }

    if (field_names == NULL) {
        *errptr = strdup("field_names empty");
        return false;
    }
    if (opts == NULL) {
        *errptr = strdup("opts empty");
        return false;
    }

    if (patterns == NULL) {
        *errptr = strdup("patterns empty");
        return false;
    }

    if (size == 0) {
        *errptr = strdup("size must be great than 0");
        return false;
    }

    // TODO: 根据ids和logic拆分出多个rule
    /*传进来的ids只用于拆分出rule, 真正的condition
     * id由id2condition_的size计算得出*/
    std::vector<ConditionImpl*> conditions;
    //目前只处理一个rule
    for (unsigned int i = 0; i < size; i++) {
        Condition* c = NULL;
        Condition::Create(field_names[i], opts[i], patterns[i], fieldset_, &c,
                          errptr);
        if (c == NULL) {
            return false;
        }
        ConditionImpl* pimpl = reinterpret_cast<ConditionImpl*>(c);
        conditions.push_back(pimpl);
    }
    std::sort(conditions.begin(), conditions.end(), CompareCondition);
    ConditionImpl* current = NULL;

    rule_names_.push_back(rule_name);
    tags_.push_back(tag);

    const char* rule_name_ptr = rule_names_[rule_names_.size() - 1].c_str();
    const char* tag_ptr = tags_[tags_.size() - 1].c_str();
    // conditions_的长度初始化为1，所以这里id从1开始
    size_t id = conditions_.size();

    if (size == 1 && opts[0] == OPT_MATCH) {
        ConditionImpl* c = conditions[0];
        c->SetUsePCRE(true);
    }

    for (size_t i = 0; i < conditions.size(); i++) {
        ConditionImpl* c = conditions[i];
        c->SetId(id);
        c->SetOrder(i + 1);
        c->SetRuleName(rule_name_ptr);
        c->SetTag(tag_ptr);
        if (i == 0) {
            current = c;
            c->SetNext(NULL);
        } else {
            current->SetNext(c);
            c->SetNext(NULL);
            current = c;
        }
        conditions_.push_back(c);
        ++id;
    }
    return true;
}

bool MatchContext::Compile(char** errptr) {
    *errptr = NULL;
    // 因为初始化成1，所以这里判断的条件是conditions_.size() <= 1
    if (conditions_.size() <= 1) {
        return true;
    }
    matched_condition_cache_.assign(conditions_.size(), NULL);

    for (ConditionImpl* c : conditions_) {
        QueueNode<ConditionImpl*> node = {.data = c, .next = NULL};
        matched_condition_queue_nodes_.push_back(node);
    }

    for (size_t root_index = 0; root_index < root_count_; root_index++) {
        std::vector<pattern_info_t> vpi;
        db_type_t db_type = DB_TYPE_COUNT;

        for (size_t i = 1; i < conditions_.size(); ++i) {
            ConditionImpl* condition = conditions_[i];

            if (root_index == condition->GetRootIndex()) {
                pattern_info_t pi;
                pi.id = condition->GetId();
                pi.pattern = condition->GetPattern();
                pi.use_pcre = condition->GetUsePCRE();
                vpi.push_back(pi);

                db_type = condition->GetDatabaseType();
            }
        }
        if (vpi.size() > 0) {
            Database* database = Factory::Create(db_type);
            if (database) {
                bool success = database->Init(vpi, errptr);
                if (!success) {
                    return false;
                }
                databases_[root_index] = database;
            } else {
                *errptr = strdup("create database failed");
                return false;
            }
        }
    }

    return true;
}

void MatchContext::InitMatchData() {
    is_finished_root_.assign(root_count_, 0);
    matched_condition_cache_.assign(matched_condition_cache_.size(), NULL);
    InitQueue(&matched_condition_queue_);
}
bool MatchContext::Match(const input_item_t* items, size_t size) {
    if (items == NULL || size == 0) {
        return false;
    }

    if (conditions_.size() <= 1) {
        return false;
    }

    InitMatchData();

    input_.assign(field_count_, {0, NULL});

    for (size_t i = 0; i < size; i++) {
        int index = items[i].index;
        input_[index].data = items[i].value.data;
        input_[index].len = items[i].value.len;
    }

    for (size_t i = 0; i < field_count_; i++) {
        for (int j = DB_TYPE_BEGIN; j < DB_TYPE_END; j++) {
            bool b = MatchHelper(i, (db_type_t)j);
            if (b) {
                return true;
            }
        }
    }
    return false;
}

ConditionImpl* MatchContext::FindConditionById(unsigned int id) {
    if (id >= conditions_.size()) {
        return NULL;
    }
    return conditions_[id];
}

bool MatchContext::MatchHelper(int root_field_index, db_type_t db_type) {
    int root_index = CalcRootIndex(root_field_index, db_type);

    Database* db = databases_[root_index];
    if (db == NULL) {
        is_finished_root_[root_index] = 1;
        return false;
    }

    if (!is_finished_root_[root_index]) {
        // Scan的目的是找到匹配的condition
        me_str_t input = {input_[root_field_index].len,
                          input_[root_field_index].data};
        db->Scan(&input, RootCallback, this);
        is_finished_root_[root_index] = 1;
    }

    while (!IsQueueEmpty(&matched_condition_queue_)) {
        ConditionImpl* condition = Front(&matched_condition_queue_)->data;
        PopQueue(&matched_condition_queue_);

        //到达了最后一个condition的处理开始
        ConditionImpl* next_condition = condition->GetNext();

        if (next_condition == NULL) {
            if (!callback_) {
                continue;
            }
            const char* rule_name = condition->GetRuleName();
            const char* tag = condition->GetTag();
            const unsigned char* payload = condition->GetPayload();
            int payload_len = condition->GetPayloadLength();
            if (callback_(rule_name, tag, payload, payload_len, user_data_) !=
                0) {
                return true;
            } else {
                continue;
            }
        }
        //到达了最后一个condition的处理结束

        //以下处理下一个condition
        int next_root_index = next_condition->GetRootIndex();

        if (!is_finished_root_[next_root_index]) {
            //之前没有查找过这个DB
            Database* next_db = databases_[next_root_index];
            if (next_db == NULL) {
                is_finished_root_[next_root_index] = 1;
                return false;
            }
            int next_field_index = next_condition->GetFieldIndex();

            me_str_t input = {input_[next_field_index].len,
                              input_[next_field_index].data};

            next_callback_data_t next_callback_data;
            next_callback_data.next_condition = next_condition;
            next_callback_data.match_context = this;
            next_db->Scan(&input, NextCallback, &next_callback_data);

            is_finished_root_[next_root_index] = 1;
        } else {
            // 之前查找过这个db，直接在命中的表中查找
            int next_condition_id = next_condition->GetId();
            ConditionImpl* cache = matched_condition_cache_[next_condition_id];
            if (cache) {
                PushMatchedQueue(cache->GetId());
            }
        }
    }

    return false;
}

void MatchContext::PushMatchedQueue(unsigned int id) {
    PushQueue(&matched_condition_queue_, &matched_condition_queue_nodes_[id]);
}

void MatchContext::PushMatchedCache(unsigned int id) {
    matched_condition_cache_[id] = conditions_[id];
}

static int RootCallback(unsigned int id, void* user_data,
                        unsigned char* payload, int payload_len) {
    MatchContext* context = (MatchContext*)user_data;
    ConditionImpl* condition = context->FindConditionById(id);

    bool use_pcre = condition->GetUsePCRE();
    if (use_pcre) {
        condition->SetPayload(payload, payload_len);
    }

    if (condition->GetOrder() == 1) {
        context->PushMatchedQueue(id);
    } else {
        context->PushMatchedCache(id);
    }
    return 0;  //返回0代表继续匹配
}

static int NextCallback(unsigned int id, void* user_data,
                        unsigned char* payload, int payload_len) {
    next_callback_data_t* next_callback_data = (next_callback_data_t*)user_data;
    ConditionImpl* next_condition = next_callback_data->next_condition;

    MatchContext* match_context = next_callback_data->match_context;
    ConditionImpl* condition = match_context->FindConditionById(id);
    bool use_pcre = condition->GetUsePCRE();
    if (use_pcre) {
        condition->SetPayload(payload, payload_len);
    }
    unsigned int next_id = next_condition->GetId();

    if (id == next_id) {
        match_context->PushMatchedQueue(next_id);
    } else {
        //两种情况: 1.根    2.非根
        if (condition->GetOrder() == 1) {
            match_context->PushMatchedQueue(condition->GetId());
        } else {
            match_context->PushMatchedCache(condition->GetId());
        }
    }

    return 0;  //返回0代表继续匹配
}

static bool CompareCondition(const ConditionImpl* a, const ConditionImpl* b) {
    if (a->GetFieldIndex() < b->GetFieldIndex())
        return true;
    else if (a->GetFieldIndex() > b->GetFieldIndex())
        return false;
    else {
        if (a->GetDatabaseType() < b->GetDatabaseType()) {
            return true;
        } else {
            return false;
        }
    }
    return true;
}
}  // namespace ultrascan