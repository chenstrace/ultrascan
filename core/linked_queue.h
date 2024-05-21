#ifndef USCAN_CORE_LINKED_QUEUE_H_
#define USCAN_CORE_LINKED_QUEUE_H_
#include <stddef.h>

namespace ultrascan {
template <class T>
struct QueueNode {
    T data;
    struct QueueNode<T>* next;
};

template <class T>
struct LinkedQueue {
    struct QueueNode<T>* head;
    struct QueueNode<T>* tail;
};

template <class T>
static inline void InitQueue(LinkedQueue<T>* queue) {
    queue->head = NULL;
    queue->tail = NULL;
}

template <class T>
static inline void PushQueue(LinkedQueue<T>* queue, QueueNode<T>* node) {
    if (queue == NULL) return;

    if (queue->head == NULL) {
        queue->head = node;
        queue->tail = node;
        queue->tail->next = NULL;
    } else {
        queue->tail->next = node;
        queue->tail = node;
        queue->tail->next = NULL;
    }
}
template <class T>
static inline QueueNode<T>* Front(LinkedQueue<T>* queue) {
    if (queue == NULL) return NULL;
    return queue->head;
}
template <class T>
static inline void PopQueue(LinkedQueue<T>* queue) {
    if (queue == NULL) return;
    queue->head = queue->head->next;
}
template <class T>
static inline bool IsQueueEmpty(LinkedQueue<T>* queue) {
    if (queue == NULL) return true;
    if (queue->head == NULL) return true;
    return false;
}
template <class T>
static inline size_t QueueSize(LinkedQueue<T>* queue) {
    if (!queue || !queue->head) return 0;

    size_t size = 0;
    QueueNode<T>* head = queue->head;
    while (head) {
        ++size;
        head = head->next;
    }
    return size;
}
}  // namespace ultrascan
#endif /* USCAN_CORE_LINKED_QUEUE_H_ */
