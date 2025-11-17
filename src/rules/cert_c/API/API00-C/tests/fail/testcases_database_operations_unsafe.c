/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: database_operations_unsafe.c
 *
 * This case demonstrates violations where database operation functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Mock database structures for demonstration */
typedef struct {
    void *handle;
    char *connection_string;
    int is_connected;
} Database;

typedef struct {
    void *stmt_handle;
    char *query;
    int is_prepared;
} Statement;

/* NON-COMPLIANT: No validation of connection parameters */
Database *connect_database(const char *host, const char *username, const char *password, const char *database_name) {
    Database *db = malloc(sizeof(Database));
    /* No validation of any parameters */
    size_t conn_str_len = strlen(host) + strlen(username) + strlen(database_name) + 100;  /* Could dereference NULL */
    db->connection_string = malloc(conn_str_len);
    sprintf(db->connection_string, "host=%s user=%s dbname=%s", host, username, database_name);
    db->is_connected = 1;
    return db;
}

/* NON-COMPLIANT: No validation of SQL query */
Statement *prepare_statement(Database *db, const char *sql_query) {
    /* No validation of db or sql_query */
    Statement *stmt = malloc(sizeof(Statement));
    stmt->query = malloc(strlen(sql_query) + 1);  /* sql_query could be NULL */
    strcpy(stmt->query, sql_query);
    stmt->is_prepared = 1;
    return stmt;
}

/* NON-COMPLIANT: No validation of parameter binding */
void bind_parameter(Statement *stmt, int parameter_index, const char *value) {
    /* No validation of stmt, parameter_index, or value */
    printf("Binding parameter %d with value: %s\n", parameter_index, value);  /* value could be NULL */
}

/* NON-COMPLIANT: No validation of statement execution */
int execute_statement(Statement *stmt) {
    /* No validation of stmt */
    printf("Executing query: %s\n", stmt->query);  /* stmt could be NULL */
    return 1;  /* Mock success */
}

/* NON-COMPLIANT: No validation of result fetching */
char *fetch_result(Statement *stmt, int column_index) {
    /* No validation of stmt or column_index */
    static char mock_result[256];
    sprintf(mock_result, "Result from column %d", column_index);  /* stmt could be NULL */
    return mock_result;
}

/* NON-COMPLIANT: No validation of transaction operations */
void begin_transaction(Database *db) {
    /* No validation of db */
    printf("Beginning transaction on database: %s\n", db->connection_string);  /* db could be NULL */
}

/* NON-COMPLIANT: No validation of bulk insert parameters */
void bulk_insert(Database *db, const char *table_name, char **column_names,
                char ***data_rows, int row_count, int column_count) {
    /* No validation of any parameters */
    printf("Bulk inserting %d rows into table: %s\n", row_count, table_name);  /* table_name could be NULL */

    for (int i = 0; i < row_count; i++) {
        printf("Row %d: ", i);
        for (int j = 0; j < column_count; j++) {
            printf("%s=%s ", column_names[j], data_rows[i][j]);  /* Could dereference NULL arrays */
        }
        printf("\n");
    }
}

/* NON-COMPLIANT: No validation of backup parameters */
void backup_database(Database *db, const char *backup_path, int compression_level) {
    /* No validation of db or backup_path */
    printf("Backing up database to: %s with compression level %d\n",
           backup_path, compression_level);  /* backup_path could be NULL */
}

/* NON-COMPLIANT: No validation of index creation */
void create_index(Database *db, const char *table_name, const char *column_name, const char *index_name) {
    /* No validation of any parameters */
    char create_sql[512];
    sprintf(create_sql, "CREATE INDEX %s ON %s (%s)",
            index_name, table_name, column_name);  /* All could be NULL */
    printf("Creating index: %s\n", create_sql);
}

int main(void) {
    Database *null_db = NULL;
    Statement *null_stmt = NULL;
    char *null_string = NULL;

    /* Examples of dangerous database operations */
    // connect_database(null_string, null_string, null_string, null_string);  /* NULL parameters */
    // prepare_statement(null_db, null_string);  /* NULL database and query */
    // bind_parameter(null_stmt, -1, null_string);  /* NULL statement and value */
    // execute_statement(null_stmt);  /* NULL statement */
    // fetch_result(null_stmt, -1);  /* NULL statement and invalid column */
    // begin_transaction(null_db);  /* NULL database */
    // bulk_insert(null_db, null_string, NULL, NULL, -1, -1);  /* All NULL/invalid */
    // backup_database(null_db, null_string, -5);  /* NULL parameters */
    // create_index(null_db, null_string, null_string, null_string);  /* All NULL */

    printf("Database functions compiled but lack parameter validation\n");
    return 0;
}