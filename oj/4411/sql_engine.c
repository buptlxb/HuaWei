#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "sql_engine.h"
#include <stdbool.h>
#include <assert.h>


static int type = 0;
static int repeat = 0;
static int repeat_del = 0;
static int row = 0;

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

bool __except(const char **sql, char c)
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
    if (!__except(sql, '\''))
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
    __sql_free((void **)&sh->table_name);
    if (sh->operation == CREATE) {
        while (sh->col_nums) {
            __sql_free((void **)&sh->columns[--(sh->col_nums)].name);
        }
    } else if (sh->operation == INSERT) {
        // while (sh->value_nums) {
        //     __sql__free(&sh->values[--(sh->col_nums)].name);
        // }
    } else {
        while (sh->cond_nums) {
            __sql_free((void **)&sh->conds[--(sh->cond_nums)].name);
            //value_t need free
        }
    }
    while (sh->order_nums) {
        __sql_free((void **)&sh->orders[--(sh->order_nums)]);
    }
}

bool sql_parse(const char *sql, sql_handle_t *sh)
{
    if (CASE_STARTS_WITH(sql, CREATE_TABLE_CLAUSE)) {
        sh->operation = CREATE;
        sql += strlen(CREATE_TABLE_CLAUSE);
        if (!__sql_id_parse(&sql, &sh->table_name))
            return false;
        if (!__except(&sql, '('))
            return false;
        sh->col_nums = 0;
        while (*sql && (__except(&sql, ',') || !__except(&sql, ')'))) {
            if (!__sql_id_parse(&sql, &sh->columns[sh->col_nums].name))
                return false;
            if (!__sql_type_parse(&sql, &sh->columns[sh->col_nums].type))
                return false;
            sh->col_nums += 1;
        }
        if (!*sql)
            return false;
        if (!__except(&sql, ';'))
            return false;
    } else if (CASE_STARTS_WITH(sql, DROP_TABLE_CLAUSE)) {
        sh->operation = DROP;
        sql += strlen(DROP_TABLE_CLAUSE);
        if (!__sql_id_parse(&sql, &sh->table_name))
            return false;
        if (!__except(&sql, ';'))
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
        if (!__except(&sql, '('))
            return false;
        sh->value_nums = 0;
        // table_t *pt = get_table_by_name(sh->table_name);
        while (*sql && (__except(&sql, ',') || !__except(&sql, ')'))/* && sh->value_nums < pt->col_nums*/) {
            if (__sql_integer_parse(&sql, &sh->values[sh->value_nums].integer))
                ;
            else if (!__sql_text_parse(&sql, &sh->values[sh->value_nums].text))
                return false;
            sh->value_nums += 1;
        }
        if (!*sql)
            return false;
        if (!__except(&sql, ';'))
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
        if (!CASE_STARTS_WITH(sql, WHERE_CLAUSE)) {
            if (!__except(&sql, ';'))
                return false;
            return true;
        }
        sql += strlen(WHERE_CLAUSE);
        sh->cond_nums = 0;
        if (!__sql_id_parse(&sql, &sh->conds[sh->cond_nums].name))
            return false;
        //table_t *pt = get_table_by_name(sh->table_name);
        if (!__except(&sql, '='))
            return false;
        if (__sql_integer_parse(&sql, &sh->conds[sh->cond_nums].val.integer))
            ;
        else if (!__sql_text_parse(&sql, &sh->conds[sh->cond_nums].val.text))
            return false;
        sh->cond_nums += 1;
        while (*sql && isspace(*sql))
            ++sql;
        if (!*sql)
            return false;
        if (!CASE_STARTS_WITH(sql, ORDER_BY_CLAUSE)) {
            if (!__except(&sql, ';'))
                return false;
            return true;
        }
        sql += strlen(ORDER_BY_CLAUSE);
        sh->order_nums = 0;
        if (!__sql_id_parse(&sql, &sh->orders[sh->order_nums]))
            return false;
        sh->order_nums = 1;
        if (!__except(&sql, ';'))
            return false;
    }
    else
        return false;
    return true;
}

RecordSet sql_execute(const char *sql)
{
    sql_handle_t sh;
    if (!sql_parse(sql, &sh))
        return NULL;
    return NULL;
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
    return NULL;
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
    return;
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
    return NULL;
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
                cur += snprintf(buf+cur, size-cur, "\t%s %d\n", sh->conds[i].name, sh->conds[i].val.integer);
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
                cur += snprintf(buf+cur, size-cur, "\t%d\n", sh->values[i].integer);
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

    if (sql_parse("CREATE TABLE table_name ( id1 integer, id2 integer, id3 integer, id4 integer, id5 integer, name text );", &sh)) {
        char *buf = sql_handle_to_string(&sh);
        puts(buf);
        free(buf);
    } else
        puts("failed");

    if (sql_parse("DROP TABLE table_name;", &sh)) {
        char *buf = sql_handle_to_string(&sh);
        puts(buf);
        free(buf);
    } else
        puts("failed");

    if (sql_parse("INSERT INTO table_name VALUES (100, 'Bananas');", &sh)) {
        char *buf = sql_handle_to_string(&sh);
        puts(buf);
        free(buf);
    } else
        puts("failed");

    if (sql_parse("SELECT * FROM table_name WHERE col2_name = 'Bananas' ORDER BY col1_name;", &sh)) {
        char *buf = sql_handle_to_string(&sh);
        puts(buf);
        free(buf);
    } else
        puts("failed");
    return 0;
}
