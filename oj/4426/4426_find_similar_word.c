#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DICT_SIZE 503

typedef struct list_node_t {
    union {
        char *key;
        int count;
    };
    struct list_node_t *next;
} list_node_t;

typedef struct hash_node_t {
    char *key;
    struct hash_node_t *next;
    struct list_node_t head;
} hash_node_t;

hash_node_t dict[DICT_SIZE];

unsigned int hash(const char *str)
{
    unsigned int ret = 0;
    while (*str)
        ret = ret * 131 + *str++;
    return ret;
}


int char_cmp(const void *lhs, const void *rhs)
{
    return *(const char *)lhs - *(const char *)rhs;
}

hash_node_t *get_head(const char *word)
{
    char *key = strdup(word);
    qsort(key, strlen(key), sizeof(char), char_cmp);
    hash_node_t *p = dict + hash(key) % DICT_SIZE;
    while (p->next && strcmp(p->next->key, key))
        p = p->next;
    free(key);
    return p->next;
}


int AddOneWord (char* Word)
{
    char *key = strdup(Word);
    qsort(key, strlen(key), sizeof(char), char_cmp);
    hash_node_t *bucket = dict + hash(key) % DICT_SIZE;
    hash_node_t *p = bucket;
    while (p->next && strcmp(p->next->key, key))
        p = p->next;
    if (!p->next) {
        p = calloc(1, sizeof(hash_node_t));
        p->next = bucket->next;
        p->key = key;
        bucket->next = p;
    } else {
        p = p->next;
        free(key);
    }
    list_node_t *h = &p->head;
    while (h->next) {
        int c = strcmp(h->next->key, Word);
        if (c == 0)
            return -1;
        else if (c > 0)
            break;
        h = h->next;
    }
    list_node_t *n = (list_node_t *)malloc(sizeof(list_node_t));
    n->key = strdup(Word);
    n->next = h->next;
    h->next = n;
    ++(p->head.count);
    return 0;
}

int FindSimilarWordNum (char* Word)
{
    hash_node_t *head = get_head(Word);
    if (!head)
        return 0;
    list_node_t *p = &head->head;
    while (p->next) {
        int c = strcmp(p->next->key, Word);
        if (c == 0)
            return head->head.count-1;
        else if (c > 0)
            break;
        p = p->next;
    }
    return head->head.count;
}

int FindOneSimilarWord (char* Word, int Seq, char* SimilarWord)
{
    hash_node_t *head = get_head(Word);
    *SimilarWord = '\0';
    if (!head || Seq > head->head.count)
        return -1;
    list_node_t *p = &head->head;
    list_node_t *saved = NULL;
    while (p->next) {
        int c = strcmp(p->next->key, Word);
        if (c == 0) {
            saved = p->next;
            p->next = p->next->next;
            if (Seq == head->head.count) {
                p->next = saved;
                return -1;
            }
            break;
        } else if (c > 0)
            break;
        p = p->next;
    }
    list_node_t *res = &head->head;
    while (Seq) {
        res = res->next;
        --Seq;
    }
    if (saved)
        p->next = saved;
    strcpy(SimilarWord, res->key);
    return 0;
}

void ClearAllWords(void)
{
    int i;
    for (i = 0; i < DICT_SIZE; ++i) {
        while(dict[i].next) {
            hash_node_t *cur = dict[i].next;
            dict[i].next = cur->next;
            list_node_t *p = &cur->head;
            while (p->next) {
                list_node_t *d = p->next;
                p->next = d->next;
                free(d);
            }
            free(cur);
        }
    }
}


int main(void)
{
    const char* words[] = {
        "mock",
        "aabc",
        "abc",
        "ckom",
        "bcaa",
        "abca"
    };
    int i;
    for (i = 0; i < 6; ++i)
        printf("add %s -> %d\n", words[i], AddOneWord(words[i]));
    int ret = FindSimilarWordNum("aabc");
    printf("%d\n", ret);
    char sw[51];
    int r = FindOneSimilarWord("aabc", 2, sw);
    printf("%d:%s\n", r, sw);
    ClearAllWords();
    ret = FindSimilarWordNum("aabc");
    printf("%d\n", ret);
    for (i = 0; i < 6; ++i)
        printf("add %s -> %d\n", words[i], AddOneWord(words[i]));
    ret = FindSimilarWordNum("aabc");
    printf("%d\n", ret);
    r = FindOneSimilarWord("aabc", 2, sw);
    printf("%d:%s\n", r, sw);
    return 0;
}
