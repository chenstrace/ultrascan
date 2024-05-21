#ifndef USCAN_CORE_MATCH_CONTEXT_H_
#define USCAN_CORE_MATCH_CONTEXT_H_

#include <vector>

#include "database.h"
#include "internal.h"
#include "linked_queue.h"
#include "types.h"
namespace ultrascan {
class FieldSet;
class ConditionImpl;

class MatchContext {
   public:
    explicit MatchContext(FieldSet* fieldset);
    ~MatchContext();

    MatchContext() = delete;
    MatchContext(const MatchContext&) = delete;
    MatchContext& operator=(const MatchContext&) = delete;

    void SetMatchCallback(rule_match_callback callback, void* user_data);

    bool AddRule(const char* rule_name, const char* tag,
                 const char* const* field_names, const match_opt_t* opts,
                 const char* const* patterns, const unsigned int* ids,
                 unsigned int size, const char* logic, char** errptr);

    bool Compile(char** errptr);

    bool Match(const input_item_t* items, size_t size);
    ConditionImpl* FindConditionById(unsigned int id);
    void PushMatchedQueue(unsigned int id);
    void PushMatchedCache(unsigned int id);

   private:
    void InitMatchData();
    bool MatchHelper(int root_field_index, db_type_t db_type);

    std::vector<std::string> rule_names_;
    std::vector<std::string> tags_;
    //下标从1开始，0的位置不用，下标为condition的id
    std::vector<ConditionImpl*> conditions_;
    FieldSet* fieldset_;
    std::vector<Database*> databases_;
    rule_match_callback callback_;
    void* user_data_;
    std::vector<me_str_t> input_;
    std::vector<char> is_finished_root_;
    size_t field_count_;
    size_t root_count_;
    LinkedQueue<ConditionImpl*> matched_condition_queue_;
    
    //matched_condition_queue_nodes_的下标是condition的ID，查找前，预分配数组的空间，数组的长度为conditions的个数。
    //与matched_condition_queue_配合使用
    std::vector<QueueNode<ConditionImpl*>> matched_condition_queue_nodes_;


    //matched_condition_cache_为命中条件的缓存。下标为condition的id，查找前，预分配数组的空间，数组的长度为conditions的个数。
    //调用Match前，所有元素置为NULL。如果元素不为空，表示之前的查找过程中命中了。

    //举例：
    //系统中总共有2条规则：
    //rule1 : header match "abc" AND body eq "xyz"
    //rule2 : header match "abcd" AND header match "456"
    //假设输入的header是abcdefg456，先命中rule1的第1个条件header match "abc"，再命中rule2的第2个条件header match "456"，此时会把rule2的第2个条件缓存起来（数组元素赋值）
    //后续命中header match "abcd"，寻找下一个条件(即header match "456")时，可以直接查看
    std::vector<ConditionImpl*> matched_condition_cache_;
};
}  // namespace ultrascan

#endif /* USCAN_CORE_MATCH_CONTEXT_H_ */
