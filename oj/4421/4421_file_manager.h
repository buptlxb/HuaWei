#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_

typedef struct node_t {
    struct node_t *child; // first child;
    struct node_t *parent; // parent directory;
    struct node_t *next; // next sibling;
    char *name;
    int is_dir;
} node_t;

extern node_t root;

int CreateDir(const char * ParentDirName, const char * DirName);

void DeleteDir(const char * DirName);

int MoveDir(const char * SrcDirName, const char * DestDirName);

int CreateFile(const char * DirName, const char * FileName);

void DeleteFile(const char * FileName);

unsigned int GetFileNum(const char * DirName);

void Clear(void);

#endif
