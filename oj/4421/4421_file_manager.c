#include <stdio.h>
#include "4421_file_manager.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

node_t root = {NULL, NULL, NULL, "root", 1};

node_t *__find_node(const char *name, int is_dir, node_t *p)
{
    assert(p && "p must not be NULL");
    node_t *res;
    if (is_dir == p->is_dir && !strcmp(name, p->name))
        return p;
    if (p->child && (res = __find_node(name, is_dir, p->child)))
        return res;
    return p->next ? __find_node(name, is_dir, p->next) : NULL;
}

node_t *find_node(const char *name, int is_dir)
{
    return name ? __find_node(name, is_dir, &root) : NULL;
}

int CreateDir(const char * ParentDirName, const char * DirName)
{
    if (!ParentDirName || !DirName)
        return -1;
    node_t *p = find_node(ParentDirName, 1), *c = find_node(DirName, 1);
    if (!p || !p->is_dir || c)
        return -1;
    node_t *n = (node_t *)malloc(sizeof(node_t));
    n->parent = p;
    n->name = strdup(DirName);
    n->is_dir = 1;
    n->next = p->child;
    n->child = NULL;
    p->child = n;
    return 0;
}

void __DeleteDir(node_t *p)
{
    assert(p && p->name && "p and p->name must not be NULL");
    if (p->child)
        __DeleteDir(p->child);
    if (p->next)
        __DeleteDir(p->next);
    free(p->name);
    free(p);
}

void DeleteDir(const char * DirName)
{
    if (!DirName)
        return;
    node_t *p = find_node(DirName, 1);
    if (!p)
        return;
    if (!p->parent) {
        Clear();
    } else {
        if (p->parent->child == p) {
            p->parent->child = p->next;
        } else {
            node_t *prev = p->parent->child;
            while (prev->next != p) {
                prev = prev->next;
            }
            prev->next = p->next;
        }
        p->next = NULL;
        __DeleteDir(p);
    }
}


int MoveDir(const char * SrcDirName, const char * DestDirName)
{
    if (!SrcDirName || !DestDirName || !strcmp(SrcDirName, DestDirName))
        return -1;
    node_t *psd = find_node(SrcDirName, 1), *pdd = find_node(DestDirName, 1);
    if (!psd || !pdd || psd->parent == pdd)
        return -1;
    if (psd->child && __find_node(DestDirName, 1, psd->child))
        return -1;
    if (psd->parent->child == psd)
        psd->parent->child = psd->next;
    else {
        node_t *prev = psd->parent->child;
        while (prev->next != psd) {
            prev = prev->next;
        }
        prev->next = psd->next;
    }
    psd->next = pdd->child;
    pdd->child = psd;
    psd->parent = pdd;
    return 0;
}

int CreateFile(const char * DirName, const char * FileName)
{
    if (!FileName || !DirName)
        return -1;
    node_t *p = find_node(DirName, 1), *c = find_node(FileName, 0);
    if (!p || c)
        return -1;
    node_t *n = (node_t *)malloc(sizeof(node_t));
    n->parent = p;
    n->name = strdup(FileName);
    n->is_dir = 0;
    n->next = p->child;
    n->child = NULL;
    p->child = n;
    return 0;
}

void DeleteFile(const char * FileName)
{
    if (!FileName)
        return;
    node_t *p = find_node(FileName, 0);
    if (!p)
        return;
    if (p->parent->child == p)
        p->parent->child = p->next;
    else {
        node_t *prev = p->parent->child;
        while (prev->next != p) {
            prev = prev->next;
        }
        prev->next = p->next;
    }
    free(p->name);
    free(p);
    return;
}

unsigned int __GetFileNum(const node_t *p)
{
    assert(p && "p must not be NULL");
    int cnt = 0;
    if (p->child)
        cnt += __GetFileNum(p->child);
    if (p->next)
        cnt += __GetFileNum(p->next);
    return cnt + !p->is_dir;
}

unsigned int GetFileNum(const char * DirName)
{
    node_t *p;
    if (!DirName || !(p = find_node(DirName, 1)) || !p->child)
        return 0;
    return __GetFileNum(p->child);
}

void Clear(void)
{
    if (root.child)
        __DeleteDir(root.child);
    root.child = NULL;
}

int main(void)
{
    CreateDir("root", "DirA");
    CreateDir("root", "DirB");
    CreateDir("root", "DirC");
    CreateDir("root", "DirD");
    CreateDir("root", "DirE");
    CreateFile("root", "root");
    CreateFile("DirA", "File1");
    CreateFile("DirA", "File2");
    CreateFile("DirA", "File3");
    CreateFile("DirB", "File4");
    CreateFile("DirC", "File5");
    CreateFile("DirC", "DirC");
    assert(-1 == MoveDir("DirA", "root"));
    assert(-1 == MoveDir("DirB", "root"));
    assert(3 == GetFileNum("DirA"));
    assert(7 == GetFileNum("root"));
    assert(0 == MoveDir("DirB", "DirA"));
    assert(0 == MoveDir("DirB", "root"));
    assert(0 == MoveDir("DirB", "DirA"));
    assert(4 == GetFileNum("DirA"));
    assert(-1 == MoveDir("root", "DirA"));
    assert(0 == MoveDir("DirC", "DirB"));
    assert(6 == GetFileNum("DirA"));
    DeleteDir("DirC");
    assert(4 == GetFileNum("DirA"));
    DeleteFile("File2");
    assert(3 == GetFileNum("DirA"));
    DeleteFile("File0");
    DeleteFile("root");
    assert(3 == GetFileNum("DirA"));
    DeleteDir("DirA");
    assert(0 == GetFileNum("root"));
    Clear();
    assert(0 == GetFileNum("DirA"));
    Clear();
    return 0;
}
