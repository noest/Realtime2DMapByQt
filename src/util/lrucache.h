#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <unordered_map>
#include <QDebug>
#include <QMutexLocker>

using namespace std;

template <typename K, typename V>
struct Node{
    K key;
    V value;
    Node *pre, *next;
};

template <typename K, typename V>
class LruCache
{
public:
    LruCache();
    ~LruCache();
    void set(K key, V value);
    V get(K key);
    void setCapacity(size_t size);
private:
    void attach(Node<K,V> *node);
    void detach(Node<K,V> *node);
    void remove(Node<K,V> *node);

    unordered_map<K, Node<K,V>*> hashmap;
    Node<K,V> *head, *tail;
    size_t capacity;
    QMutex lock;
};

template <typename K, typename V>
LruCache<K,V>::LruCache()
{
    capacity = 100;
    head = new Node<K,V>;
    tail = new Node<K,V>;
    head->pre = NULL;
    head->next = tail;
    tail->pre = head;
    tail->next = NULL;
}

template <typename K, typename V>
LruCache<K,V>::~LruCache()
{
    lock.lock();
    hashmap.clear();
    while(head->next != tail){
        remove(head->next);
    }
    delete head;
    delete tail;
    lock.unlock();
}

template <typename K, typename V>
void LruCache<K,V>::set(K key, V value)
{
    lock.lock();
    Node<K,V>* node = hashmap[key];
    if(node != NULL){
        node->value = value;
        detach(node);
        attach(node);
    }else{
        if(hashmap.size() > capacity){
            Node<K,V>* rnode = tail->pre;
            hashmap.erase(rnode->key);
            remove(rnode);
        }
        Node<K,V>* nnode = new Node<K,V>;
        nnode->key = key;
        nnode->value = value;
        hashmap[key] = nnode;
        attach(nnode);
    }
    lock.unlock();
}

template <typename K, typename V>
V LruCache<K,V>::get(K key)
{
    lock.lock();
    Node<K,V>* node = hashmap[key];
    if(node != NULL){
        detach(node);
        attach(node);
        lock.unlock();
        return node->value;
    }else{
        lock.unlock();
        return V();
    }
}

template <typename K, typename V>
void LruCache<K,V>::setCapacity(size_t size)
{
    this->capacity = size;
}


template <typename K, typename V>
void LruCache<K,V>::attach(Node<K, V> *node)
{
    node->next = head->next;
	node->pre = head;
    head->next = node;
    node->next->pre = node;
}


template <typename K, typename V>
void LruCache<K,V>::detach(Node<K, V> *node)
{
    node->pre->next = node->next;
    node->next->pre = node->pre;
}

template <typename K, typename V>
void LruCache<K,V>::remove(Node<K, V> *node)
{
    detach(node);
    delete node;
}

#endif // LRUCACHE_H
