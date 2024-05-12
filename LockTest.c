// #include "lock.h"
// #include <stdio.h>
// #include <stdlib.h>

// void amountInit(lock_t* Account) {
//   if (Account == NULL)
//     exit(1);
//   Account->amount = 0;
//   pthread_mutex_init(&(Account->mutex), NULL);
// }

// void Income(lock_t* Account, int amount) {
//   pthread_mutex_lock(&(Account->mutex));
//   Account->amount += amount;
//   pthread_mutex_unlock(&(Account->mutex));
// }

// void Expend(lock_t* Account, int amount) {
//   pthread_mutex_lock(&(Account->mutex));
//   Account->amount -= amount;
//   pthread_mutex_unlock(&(Account->mutex));
// }



// #include "list_lock.h"
// #include <stdio.h>
// #include <stdlib.h>

// void listInit(list_lock_t* list) {
//   if (list == NULL)
//     exit(1);
//   list->head = NULL;
//   pthread_mutex_init(&list->mutex, NULL);
//   pthread_cond_init(&list->cond, NULL);
// }
// //将生成的数据 value 放入 list 链表中，你可能需要为此分配一个堆区资源
// void producer(list_lock_t* list, DataType value) {
//   LNode* new = (LNode*)malloc(sizeof(LNode));
//   new->value = value;
//   new->next = NULL;
//   pthread_mutex_lock(&list->mutex);
//   if (list->head != NULL)
//     new->next = list->head;
//   list->head = new;
//   pthread_cond_signal(&list->cond);
//   pthread_mutex_unlock(&list->mutex);
// }
// //从 list 链表中消耗一个数据，并释放其占有的资源
// void consumer(list_lock_t* list) {
//   pthread_mutex_lock(&list->mutex);
//   //等待生产者产生数据
//   while (list->head == NULL)
//     pthread_cond_wait(&list->cond, &list->mutex);

//   LNode* pp = list->head;
//   list->head = list->head->next;
//   pthread_mutex_unlock(&list->mutex);
//   free(pp);
// }
// //获取当前 list 中的资源个数
// int getListSize(list_lock_t* list) {
//   int i = 0;
//   pthread_mutex_lock(&list->mutex);
//   LNode* current = list->head;
//   while (current != NULL) {
//     i++;
//     current = current->next;
//   }
//   pthread_mutex_unlock(&list->mutex);
//   return i;
// }





// #include "hash_lock.h"
// #include <stdio.h>
// #include <stdlib.h>

// void hashInit(hash_lock_t* bucket) {
//   if (bucket == NULL)
//     exit(1);
//   for (int i = 0; i < HASHNUM; ++i) {
//     bucket->table[i].head = NULL;
//     pthread_mutex_init(&bucket->table[i].mutex, NULL);
//   }
// }
// //通过 key 值来获取对应的 value
// int getValue(hash_lock_t* bucket, int key) {
//   if (bucket == NULL)
//     exit(1);
//   int i = HASH(key);
//   pthread_mutex_lock(&bucket->table[i].mutex);
//   Hlist current = bucket->table[i].head;  // 获取链表头指针
//   while (current != NULL) {
//     if (current->key == key) {
//       pthread_mutex_unlock(&bucket->table[i].mutex);
//       return current->value;
//     }
//     current = current->next;
//   }
//   pthread_mutex_unlock(&bucket->table[i].mutex);
//   return -1;
// }
// //向哈希桶中添加一个节点，通过 key 来索引对应的哈希序列，如果键已经存在，则覆盖现有值
// void insert(hash_lock_t* bucket, int key, int value) {
//   if (bucket == NULL)
//     exit(1);
//   int i = HASH(key);
//   pthread_mutex_lock(&bucket->table[i].mutex);
//   Hlist current = bucket->table[i].head;
//   while (current != NULL) {
//     if (current->key == key) {
//       current->value = value;
//       pthread_mutex_unlock(&bucket->table[i].mutex);
//       return;  //键已存在，更新后返回
//     }
//     current = current->next;
//   }
//   // 如果链表为空或未找到相同的键，则创建新的节点并插入到链表头部
//   Hlist newNode = (Hlist)malloc(sizeof(Hnode));
//   pthread_mutex_unlock(&bucket->table[i].mutex);
//   newNode->key = key;
//   newNode->value = value;
//   newNode->next = bucket->table[i].head;
//   bucket->table[i].head = newNode;
//   pthread_mutex_unlock(&bucket->table[i].mutex);
// }
// //重新设置一个节点的key,并将其移动到对应的序列，如果键不存在，则返回-1,成功返回0
// int setKey(hash_lock_t* bucket, int key, int new_key) {
//   if (bucket == NULL)
//     exit(1);
//   int old = HASH(key);      
//   int new = HASH(new_key);  
//   pthread_mutex_lock(&bucket->table[old].mutex);
//   Hlist current = bucket->table[old].head;
//   Hlist prev = NULL;  // 保存前一个节点指针
//   while (current != NULL) {
//     if (current->key == key) {
//       // 如果当前节点是链表头节点
//       if (prev == NULL)  
//         bucket->table[old].head = current->next;
//       else
//         prev->next = current->next;

//       pthread_mutex_unlock(&bucket->table[old].mutex);
//       insert(bucket, new_key, current->value);
//       free(current);
//       return 0;
//     }
//     prev = current;
//     current = current->next;
//   }
//   pthread_mutex_unlock(&bucket->table[old].mutex);
//   return -1;
// }