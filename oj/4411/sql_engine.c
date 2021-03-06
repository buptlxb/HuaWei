#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "sql_engine.h"
#include <stdbool.h>
#include <assert.h>


// static int type = 0;
// static int repeat = 0;
// static int repeat_del = 0;
// static int row = 0;

table_t head = {{{0, INVALID_TYPE}, }, 0, 0, 0, NULL, NULL, ""};

sql_execute_func_t sql_execute_funcs[] = {
    sql_execute_create,
    sql_execute_drop,
    sql_execute_delete,
    sql_execute_insert,
    sql_execute_select
};

/***********************************设计规则 ***********************************
1、支持的数据类型：
    integer : 整型，有符号 4 字节整数；
    text : 变长字符串；（实现的时候，内部可以直接用char*）
2、支持语法： 
    SQL关键字(包括数据类型)不区分大小写，表名/列名/列值 区分大小写；
    SQL关键字包括：
        CREATE TABLE/INTEGER/TEXT/DROP TABLE/INSERT INTO/VALUES/ORDER BY/
        SELECT FROM WHERE/ORDER BY/DELETE FROM WHERE
        SQL语句中的特殊字符包括：( ) ' ; = 
3、语法细节：
    关于语句返回的错误信息，请参考.h文件定义
    创建表：CREATE TABLE table_name (col1_name   integer,col2_name   text);最大列数为5,列号从1开始
    删除表：DROP TABLE table_name;
    插入记录：INSERT INTO table_name VALUES (100, 'Bananas');允许有重复
    查询记录：SELECT * FROM table_name WHERE col1_name = 100;
              SELECT * FROM table_name WHERE col2_name = 'Bananas' ORDER BY col1_name;
                1、ORDER BY 以升序排列
                2、当前SELECT关键字后，仅支持*（即返回所有列，列顺序按照创建表时候的列顺序），不需要考虑返回局部列的情况
*********************************************************************************/




//////////////////////////////// 记录集接口定义 ///////////////////////////////

/*****************************************
*   功能:  SQL语句执行接口
*    
*   输入:   const char *sql ：  SQL语句
*    
*   输出:   无输出
*     
*   返回:  SQL语句执行后的结果集，只有SELECT语句返回的结果集可能存在多于1条的情况
*           CREATE TABLE/DROP TABLE/INSERT INTO 语句返回的结果集只有一条记录，
*           反映出当前语句的执行结果 。
*           如CREATE TABLE执行成功后返回CREATE TABLE SUCCESS，失败则返回CREATE TABLE FAILED
*****************************************/

bool __expect(const char **sql, char c)
{
    const char *p = *sql;
    while (*p && isspace(*p))
        ++p;
    if (*p == c) {
        *sql = p+1;
        return true;
    }
    return false;
}

bool __sql_id_parse(const char **sql, char **id)
{
    const char *p = *sql;
    *id = NULL;
    while (*p && isspace(*p))
        ++p;
    int i = 0;
    while (*p && (isalnum(*p) || *p == '_')) {
        ++p;
        ++i;
    }
    if (i) {
        *id = strndup(p-i, i);
        *sql = p;
        return true;
    }
    return false;
}

bool __sql_integer_parse(const char **sql, int *integer)
{
    char *p = NULL;
    *integer = strtol(*sql, &p, 10);
    if (*sql == p)
        return false;
    *sql = p;
    return true;
}

bool __sql_text_parse(const char **sql, char **text)
{
    if (!__expect(sql, '\''))
        return false;
    const char *p = *sql;
    while (*p && *p != '\'')
        ++p;
    if (!*p)
        return false;
    *text = strndup(*sql, p-*sql);
    *sql = p+1;
    return true;
}

bool __sql_type_parse(const char **sql, enum type_t *type)
{
    const char *p = *sql;
    while (*p && isspace(*p))
        ++p;
    char *type_name;
    if (!__sql_id_parse(&p, &type_name))
        return false;
    if (CASE_EQUAL_TO(type_name, TYPE_INTEGER))
        *type = INTEGER;
    else if (CASE_EQUAL_TO(type_name, TYPE_TEXT))
        *type = TEXT;
    else
        return false;
    *sql = p;
    return true;
}

inline void __sql_free(void **ptr)
{
    if (*ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

void sql_handle_release(sql_handle_t *sh)
{
    if (!sh)
        return;
    __sql_free((void **)&sh->table_name);
    switch(sh->operation)
    {
        case CREATE:
            while (sh->col_nums) {
                __sql_free((void **)&sh->columns[--(sh->col_nums)].name);
            }
            break;
        case DROP:
            break;
        case SELECT:
            while (sh->cond_nums) {
                if (sh->conds[--(sh->cond_nums)].type == TEXT)
                    __sql_free((void **)&sh->conds[sh->cond_nums].name);
            }
            while (sh->order_nums) {
                __sql_free((void **)&sh->orders[--(sh->order_nums)]);
            }
            break;
        case DELETE:
            break;
        case INSERT:
            while (sh->value_nums) {
                if (sh->types[--(sh->col_nums)] == TEXT)
                    __sql_free((void **)&sh->values[sh->col_nums].text);
            }
            break;
        default:
            printf("Unknown sql operation type\n");
            exit(-1);
    }
}

bool sql_parse(const char *sql, sql_handle_t *sh)
{
    if (CASE_STARTS_WITH(sql, CREATE_TABLE_CLAUSE)) {
        sh->operation = CREATE;
        sql += strlen(CREATE_TABLE_CLAUSE);
        if (!__sql_id_parse(&sql, &sh->table_name))
            return false;
        if (!__expect(&sql, '('))
            return false;
        sh->col_nums = 0;
        while (*sql && (__expect(&sql, ',') || !__expect(&sql, ')'))) {
            if (!__sql_id_parse(&sql, &sh->columns[sh->col_nums].name))
                return false;
            if (!__sql_type_parse(&sql, &sh->columns[sh->col_nums].type))
                return false;
            sh->col_nums += 1;
        }
        if (!*sql)
            return false;
        if (!__expect(&sql, ';'))
            return false;
    } else if (CASE_STARTS_WITH(sql, DROP_TABLE_CLAUSE)) {
        sh->operation = DROP;
        sql += strlen(DROP_TABLE_CLAUSE);
        if (!__sql_id_parse(&sql, &sh->table_name))
            return false;
        if (!__expect(&sql, ';'))
            return false;
    } else if (CASE_STARTS_WITH(sql, INSERT_INTO_CLAUSE)) {
        sh->operation = INSERT;
        sql += strlen(INSERT_INTO_CLAUSE);
        if (!__sql_id_parse(&sql, &sh->table_name))
            return false;
        while (*sql && isspace(*sql))
            ++sql;
        if (!*sql)
            return false;
        if (!CASE_STARTS_WITH(sql, VALUE_CLAUSE))
            return false;
        sql += strlen(VALUE_CLAUSE);
        if (!__expect(&sql, '('))
            return false;
        sh->value_nums = 0;
        // table_t *pt = get_table_by_name(sh->table_name);
        while (*sql && (__expect(&sql, ',') || !__expect(&sql, ')'))/* && sh->value_nums < pt->col_nums*/) {
            if (__sql_integer_parse(&sql, &sh->values[sh->value_nums].integer))
                sh->types[sh->value_nums] = INTEGER;
            else if (__sql_text_parse(&sql, &sh->values[sh->value_nums].text))
                sh->types[sh->value_nums] = TEXT;
            else
                return false;
            sh->value_nums += 1;
        }
        if (!*sql)
            return false;
        if (!__expect(&sql, ';'))
            return false;
    } else if (CASE_STARTS_WITH(sql, DELETE_FROM_CLAUSE)) {

    } else if (CASE_STARTS_WITH(sql, SELECT_STAR_FROM_CLAUSE)) {
        sh->operation = SELECT;
        sql += strlen(SELECT_STAR_FROM_CLAUSE);
        if (!__sql_id_parse(&sql, &sh->table_name))
            return false;
        while (*sql && isspace(*sql))
            ++sql;
        if (!*sql)
            return false;
        sh->cond_nums = 0;
        sh->order_nums = 0;
        if (!CASE_STARTS_WITH(sql, WHERE_CLAUSE)) {
            if (!__expect(&sql, ';'))
                return false;
            return true;
        }
        sql += strlen(WHERE_CLAUSE);
        if (!__sql_id_parse(&sql, &sh->conds[sh->cond_nums].name))
            return false;
        if (!__expect(&sql, '='))
            return false;
        if (__sql_integer_parse(&sql, &sh->conds[sh->cond_nums].val.integer))
            sh->conds[sh->cond_nums].type = INTEGER;
        else if (__sql_text_parse(&sql, &sh->conds[sh->cond_nums].val.text))
            sh->conds[sh->cond_nums].type = TEXT;
        else
            return false;
        sh->cond_nums += 1;
        while (*sql && isspace(*sql))
            ++sql;
        if (!*sql)
            return false;
        if (!CASE_STARTS_WITH(sql, ORDER_BY_CLAUSE)) {
            if (!__expect(&sql, ';'))
                return false;
            return true;
        }
        sql += strlen(ORDER_BY_CLAUSE);
        if (!__sql_id_parse(&sql, &sh->orders[sh->order_nums]))
            return false;
        sh->order_nums = 1;
        if (!__expect(&sql, ';'))
            return false;
    }
    else
        return false;
    return true;
}

table_t *find_table(const char *name)
{
    assert(strlen(name) < TABLE_NAME_LEN);
    table_t *p = &head;
    while (p->next) {
        int x  = strncmp(p->next->name, name, TABLE_NAME_LEN);
        if (x == 0)
            return p->next;
        else if (x > 0)
            return NULL;
    }
    return NULL;
}

table_t *lower_bound(const char *name)
{
    assert(strlen(name) < TABLE_NAME_LEN);
    table_t *p = &head;
    while (p->next)
        if (0 <= strncmp(p->next->name, name, TABLE_NAME_LEN))
            break;
    return p;
}

inline void insert(table_t *prev, table_t *n)
{
    n->next = prev->next;
    prev->next = n;
}

void table_release(table_t **t)
{
    table_t *p = *t;
    int i, j;
    for (i = 0; i < p->row_nums; ++i) {
        for (j = 0; j < p->col_nums; ++j) {
            if (p->columns[i].type == TEXT)
                __sql_free((void **)&p->records[i*p->col_nums + j].text);
        }
    }
    for (i = 0; i < p->col_nums; ++i) {
        __sql_free((void **)&p->columns[i].name);
    }
    __sql_free((void **)&p->records);
    __sql_free((void **)t);
}

RecordSet sql_execute_create(const sql_handle_t *sh)
{
    assert(sh->operation == CREATE);
    table_t *p = find_table(sh->table_name);
    if (p || sh->col_nums > TABLE_MAX_COL) {
        RecordSet fmsg = (RecordSet)malloc(sizeof(struct db_record_set));
        strcpy(fmsg->result, CREATE_TABLE_CLAUSE " " FMSG);
        fmsg->next = NULL;
        return fmsg;
    }
    table_t *prev = lower_bound(sh->table_name);
    table_t *n = calloc(1, sizeof(table_t));
    int i;
    for (i = 0; i < sh->col_nums; ++i) {
        n->columns[i].type = sh->columns[i].type;
        n->columns[i].name = strdup(sh->columns[i].name);
    }
    n->col_nums = sh->col_nums;
    strncpy(n->name, sh->table_name, TABLE_NAME_LEN);
    insert(prev, n);

    RecordSet smsg = (RecordSet)malloc(sizeof(struct db_record_set));
    strcpy(smsg->result, CREATE_TABLE_CLAUSE " " SMSG);
    smsg->next = NULL;
    return smsg;
}

RecordSet sql_execute_drop(const sql_handle_t *sh)
{
    assert(sh->operation == DROP);
    table_t *p = find_table(sh->table_name);
    if (!p) {
        RecordSet fmsg = (RecordSet)malloc(sizeof(struct db_record_set));
        strcpy(fmsg->result, DROP_TABLE_CLAUSE " " FMSG);
        fmsg->next = NULL;
        return fmsg;
    }
    table_t *prev = lower_bound(sh->table_name);
    table_t *d = prev->next;
    prev->next = d->next;
    table_release(&d);

    RecordSet smsg = (RecordSet)malloc(sizeof(struct db_record_set));
    strcpy(smsg->result, DROP_TABLE_CLAUSE " " SMSG);
    smsg->next = NULL;
    return smsg;
}

RecordSet sql_execute_delete(const sql_handle_t *sh)
{
    assert(sh->operation == DELETE);
    return NULL;
}
RecordSet sql_execute_insert(const sql_handle_t *sh)
{
    assert(sh->operation == INSERT);
    table_t *p = find_table(sh->table_name);
    RecordSet fmsg = (RecordSet)malloc(sizeof(struct db_record_set));
    strcpy(fmsg->result, INSERT_INTO_CLAUSE " " FMSG);
    fmsg->next = NULL;
    if (!p || p->col_nums != sh->col_nums)
        return fmsg;
    int i;
    for (i = 0; i < p->col_nums; ++i)
        if (p->columns[i].type != sh->types[i])
            return fmsg;
    if (p->row_nums == p->row_cap) {
        p->row_cap += p->row_cap / 2 + 1;
        p->records = (value_t *)realloc(p->records, sizeof(value_t)*p->row_cap*p->col_nums);
    }
    if (!p->records)
        return fmsg;
    for (i = 0; i < p->col_nums; ++i) {
        if (p->columns[i].type == TEXT)
            p->records[p->row_nums*p->col_nums + i].text = strdup(sh->values[i].text);
        else
            p->records[p->row_nums*p->col_nums + i].integer = sh->values[i].integer;
    }
    p->row_nums += 1;
    free(fmsg);

    RecordSet smsg = (RecordSet)malloc(sizeof(struct db_record_set));
    strcpy(smsg->result, INSERT_INTO_CLAUSE " " SMSG);
    smsg->next = NULL;
    return smsg;
}

void record_to_nstring(char *buf, unsigned int size, const table_t *p, unsigned int row)
{
    unsigned int cur = 0, i;
    value_t *pv = p->records + row * p->col_nums;

    for (i = 0; i < p->col_nums; ++i) {
        if (i != 0)
            cur += snprintf(buf+cur, size-cur, ", ");
        switch (p->columns[i].type)
        {
            case INTEGER:
                cur += snprintf(buf+cur, size-cur, "%d", (pv+i)->integer);
                break;
            case TEXT:
                cur += snprintf(buf+cur, size-cur, "'%s'", (pv+i)->text);
                break;
            default:
                assert(0 && "Unknown column type");
        }
        assert(cur < size);
    }
}

bool filter(const sql_handle_t *sh, const table_t *p, unsigned int row, unsigned int *indices)
{
    unsigned int i;
    value_t *pv = p->records + row * p->col_nums;
    for (i = 0; i < sh->cond_nums; ++i) {
        switch (sh->conds[i].type)
        {
            case INTEGER:
                if (sh->conds[i].val.integer != pv[indices[i]].integer)
                    return false;
                break;
            case TEXT:
                if (strcmp(sh->conds[i].val.text, pv[indices[i]].text))
                    return false;
                break;
            default:
                assert(0 && "Unknown condition type");
        }
    }
    return true;
}

RecordSet sql_execute_select(const sql_handle_t *sh)
{
    assert(sh->operation == SELECT);

    RecordSet fmsg = (RecordSet)malloc(sizeof(struct db_record_set));
    strcpy(fmsg->result, SELECT_STAR_FROM_CLAUSE " " FMSG);
    fmsg->next = NULL;

    table_t *p = find_table(sh->table_name);
    if (!p)
        return fmsg;
    unsigned int i, cond_indices[WHERE_MAX_COND];
    for (i = 0; i < sh->cond_nums; ++i) {
        if (!sh->conds[i].name)
            return fmsg;
        unsigned int j;
        for (j = 0; j < p->col_nums; ++j) {
            if (strcmp(p->columns[j].name, sh->conds[i].name) == 0)
                break;
        }
        if (j == p->col_nums || p->columns[j].type != sh->conds[i].type)
            return fmsg;
        cond_indices[i] = j;
    }
    struct db_record_set dummy = {"", NULL}, *pos = &dummy;
    for (i = 0; i < p->row_nums; ++i) {
        if (sh->cond_nums && !filter(sh, p, i, cond_indices))
            continue;
        RecordSet n = (RecordSet)malloc(sizeof(struct db_record_set));
        record_to_nstring(n->result, sizeof(n->result), p, i);
        n->next = NULL;
        pos->next = n;
        pos = pos->next;
    }
    return dummy.next;
}

RecordSet sql_execute(const char *sql)
{
    sql_handle_t sh;
    memset(&sh, 0, sizeof(sh));
    if (!sql_parse(sql, &sh)) {
        RecordSet emsg = (RecordSet)malloc(sizeof(struct db_record_set));
        strcpy(emsg->result, "Syntax error!");
        emsg->next = NULL;
        return emsg;
    }
    RecordSet res = sql_execute_funcs[sh.operation](&sh);
    sql_handle_release(&sh);
    return res;
}



/*****************************************
*   功能:  SQL语句执行接口
            从上面 sql_execute 接口已经说明，结果集只有两种类情况：
*           (1) CREATE TABLE/DROP TABLE/INSERT INTO 语句返回的结果集只有一条记录，
*           使用方式如下：
*           RecordSet rs = 
*           sql_execute("CREATE TABLE table_select_multi ( id integer, name text );");
*           Record rcd = get_record(rs, 0);
*           char * rcd_s = record_to_string(rcd);
*           int ret = strcmp(rcd_s, "CREATE TABLE SUCCESS");
*
*           (2) SELECT语句返回的结果集可能存在多于1条的情况，使用方式：
*           RecordSet rs = sql_execute("SELECT * FROM table_select_multi ORDER BY id;");
*           // 取第一行
*           Record rcd = get_record(rs, 0);
*           char * rcd_s = record_to_string(rcd);
*           ret = strcmp(rcd_s, "100, 'Bananas'");
*           CPPUNIT_ASSERT(ret == 0);
*           // 取第二行
*           rcd = get_record(rs, 1);
*           rcd_s = record_to_string(rcd);
*           ret = strcmp(rcd_s, "200, 'Apple'");
*           CPPUNIT_ASSERT(ret == 0);
*    
*   输入:   const RecordSet rs ：  结果集
*           int index ：           行号（行号从0开始）
*    
*   输出:   无输出
*     
*   返回:  结果集rs中的第index条记录
*****************************************/

Record get_record(const RecordSet rs, int index)
{
    if (index < 0)
        return NULL;
    RecordSet p = rs;
    for (; index && p; --index)
        p = p->next;
    Record r = NULL;
    if (p) {
        r = (Record)malloc(sizeof(struct db_record));
        strncpy(r->result, p->result, sizeof(r->result));
        sprintf(p->result, MAGIC_STR "%016p#", r);
    }
    return r;
}

/******************************************************************************
 * 功能描述 :    释放记录集中的所有资源（动态分配的内存）
 *                特别说明，记录集中的内存可能是在下面几种情况下分配的：
 *                1、记录集中的内存主体是在 sql_execute 函数中分配的；
 *                2、在调用 get_record 的时候有可能会分配内存（跟实现方式有关）
 *                3、在调用 record_to_string 的时候会分配内存，也要管理起来
 *
 * 输入:          [IN] 记录集
 *
 * 输出:          无输出
 *
 * 返回:         调用此接口后，rs 中的内存会被释放完毕，最后rs的值为NULL
******************************************************************************/

void rm_recordset(RecordSet* rs)
{
    RecordSet p = *rs;
    while (p) {
        RecordSet d = p;
        p = p->next;
        if (!strncmp(MAGIC_STR, d->result, strlen(MAGIC_STR))) {
            void *q;
            sscanf(d->result+strlen(MAGIC_STR), "%p", &q);
            __sql_free(&q);
        }
        __sql_free((void **)&d);
    }
    *rs = NULL;
}
/******************************************************************************
 * 功能描述 :    将记录转换成字符串，这样就可以便于校验结果
 *                结果记录有两种情况：
 *                (1) CREATE TABLE/DROP TABLE/INSERT INTO 语句返回的结果
 *                    都是提示信息，相当于结果只有一列，是text类型字符串
 *                    例如：CREATE TABLE SUCCESS
 *                (2) SELECT语句返回的结果
 *                    多个值之间用英文逗号','分隔，且逗号后留有一个空格，举例：
 *                    100, 'Bananas'
 *                    200, 'Apple'
 *                    特别注意：text类型的值，需要加英文单引号括起来，如上
 *
 * 输入  :        [IN] 记录
 *
 * 输出  :        无输出。
 *
 * 返回  :       记录转成的字符串
 *                特别说明：返回的字符串的内存仍然由SQL引擎管理，在函数rm_recordset
 *                中一同释放
******************************************************************************/
char* record_to_string(const Record rcd)
{
    return rcd->result;
}

char *sql_handle_to_string(const sql_handle_t *sh)
{
    if (!sh)
        return NULL;
    int size = 1024, cur = 0, i;
    char *buf = (char *)malloc(size*sizeof(char));
    cur += snprintf(buf+cur, size-cur, "operation: %d\n", sh->operation);
    cur += snprintf(buf+cur, size-cur, "table name: %s\n", sh->table_name);
    switch(sh->operation)
    {
        case CREATE:
            cur += snprintf(buf+cur, size-cur, "columns: %d\n", sh->col_nums);
            for (i = 0; i < sh->col_nums; ++i) {
                cur += snprintf(buf+cur, size-cur, "\t%s %d\n", sh->columns[i].name, sh->columns[i].type);
                assert(cur < size);
            }
            break;
        case DROP:
            break;
        case SELECT:
            cur += snprintf(buf+cur, size-cur, "conditions: %d\n", sh->cond_nums);
            for (i = 0; i < sh->cond_nums; ++i) {
                switch (sh->conds[i].type) {
                    case INTEGER:
                        cur += snprintf(buf+cur, size-cur, "\t%s %d\n", sh->conds[i].name, sh->conds[i].val.integer);
                        break;
                    case TEXT:
                        cur += snprintf(buf+cur, size-cur, "\t%s %s\n", sh->conds[i].name, sh->conds[i].val.text);
                        break;
                    default:
                        printf("Unknown condition value type\n");
                        exit(-1);
                }
                assert(cur < size);
            }
            cur += snprintf(buf+cur, size-cur, "orders: %d\n", sh->order_nums);
            for (i = 0; i < sh->order_nums; ++i) {
                cur += snprintf(buf+cur, size-cur, "\t%s\n", sh->orders[i]);
                assert(cur < size);
            }
            break;
        case DELETE:
            break;
        case INSERT:
            cur += snprintf(buf+cur, size-cur, "values: %d\n", sh->value_nums);
            for (i = 0; i < sh->value_nums; ++i) {
                switch (sh->types[i]) {
                    case INTEGER:
                        cur += snprintf(buf+cur, size-cur, "\t%d\n", sh->values[i].integer);
                        break;
                    case TEXT:
                        cur += snprintf(buf+cur, size-cur, "\t%s\n", sh->values[i].text);
                        break;
                    default:
                        printf("Unknown value type\n");
                        exit(-1);
                }
                assert(cur < size);
            }
            break;
        default:
            return NULL;
    }
    buf = (char *)realloc(buf, cur);
    return buf;
}

int main(void)
{
    sql_handle_t sh;
    memset(&sh, 0, sizeof(sh));
    int i;
    char *sqls[] = {
        "CREATE TABLE table_name ( id1 integer, id2 integer, id3 integer, id4 integer, id5 integer, name text );",
        "CREATE TABLE table_name ( id1 integer, id2 integer, id3 integer, name text );",
        "INSERT INTO table_name VALUES (100, 'Bananas');",
        "INSERT INTO table_name VALUES (100, 20, 3, 'Bananas');",
        "INSERT INTO table_name VALUES (101, 22, 3, 'Bananas');",
        "INSERT INTO table_name VALUES (102, 20, 3, 'ananas');",
        "SELECT * FROM table_name;", 
        "SELECT * FROM table_name WHERE col2_name = 'Bananas' ORDER BY col1_name;",
        "SELECT * FROM table_name WHERE id2 = 20 ORDER BY id1;",
        "SELECT * FROM table_name WHERE name = 'Bananas' ORDER BY id1;",
        "DROP TABLE table_name;"
    };
    for (i = 0; i < sizeof(sqls)/sizeof(sqls[0]); ++i) {
        if (sql_parse(sqls[i], &sh)) {
            char *buf = sql_handle_to_string(&sh);
            puts(buf);
            free(buf);
        } else
            printf("\033[31mParing [%s] failed\033[m\n", sqls[i]);
    }

    for (i = 0; i < sizeof(sqls)/sizeof(sqls[0]); ++i) {
        printf("Executing \033[32m[%s]\033[m :\n", sqls[i]);
        RecordSet res = sql_execute(sqls[i]);
        unsigned int j = 0;
        while (res) {
            Record r = get_record(res, j++);
            if (!r)
                break;
            puts(record_to_string(r));
        }
        if (!j)
            printf("\033[31mExecuting [%s] failed\033[m\n", sqls[i]);
        rm_recordset(&res);
    }
    return 0;
}
