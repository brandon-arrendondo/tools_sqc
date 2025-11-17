/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: database_schema.c
 *
 * This case demonstrates violations where database schema constants
 * and query templates are not const-qualified.
 */

#include <stdio.h>
#include <string.h>

void table_definitions(void) {
    /* NON-COMPLIANT: Table names should be const */
    char table_users[] = "users";
    char table_products[] = "products";
    char table_orders[] = "orders";
    char table_order_items[] = "order_items";
    char table_categories[] = "categories";

    /* NON-COMPLIANT: Column names should be const */
    char col_id[] = "id";
    char col_name[] = "name";
    char col_email[] = "email";
    char col_password[] = "password";
    char col_created_at[] = "created_at";
    char col_updated_at[] = "updated_at";

    printf("Database Table Definitions:\\n");
    printf("  Tables: %s, %s, %s, %s, %s\\n",
           table_users, table_products, table_orders, table_order_items, table_categories);
    printf("  Common columns: %s, %s, %s, %s\\n",
           col_id, col_name, col_created_at, col_updated_at);

    /* Table names used for query construction but never modified */
    char query[256];
    sprintf(query, "SELECT * FROM %s WHERE %s = ?", table_users, col_email);
    printf("  Sample query: %s\\n", query);
}

void sql_templates(void) {
    /* NON-COMPLIANT: SQL query templates should be const */
    char select_template[] = "SELECT %s FROM %s WHERE %s = ?";
    char insert_template[] = "INSERT INTO %s (%s) VALUES (%s)";
    char update_template[] = "UPDATE %s SET %s = ? WHERE %s = ?";
    char delete_template[] = "DELETE FROM %s WHERE %s = ?";

    /* NON-COMPLIANT: JOIN templates should be const */
    char inner_join[] = "SELECT * FROM %s INNER JOIN %s ON %s.%s = %s.%s";
    char left_join[] = "SELECT * FROM %s LEFT JOIN %s ON %s.%s = %s.%s";
    char right_join[] = "SELECT * FROM %s RIGHT JOIN %s ON %s.%s = %s.%s";

    printf("\\nSQL Query Templates:\\n");
    printf("  SELECT: %s\\n", select_template);
    printf("  INSERT: %s\\n", insert_template);
    printf("  UPDATE: %s\\n", update_template);
    printf("  DELETE: %s\\n", delete_template);

    printf("  INNER JOIN: %s\\n", inner_join);
    printf("  LEFT JOIN: %s\\n", left_join);

    /* Templates used for query generation but never modified */
    char built_query[512];
    sprintf(built_query, select_template, "*", "users", "id");
    printf("  Built query: %s\\n", built_query);
}

void data_types(void) {
    /* NON-COMPLIANT: SQL data type names should be const */
    char type_varchar[] = "VARCHAR";
    char type_int[] = "INT";
    char type_bigint[] = "BIGINT";
    char type_decimal[] = "DECIMAL";
    char type_datetime[] = "DATETIME";
    char type_boolean[] = "BOOLEAN";
    char type_text[] = "TEXT";
    char type_blob[] = "BLOB";

    /* NON-COMPLIANT: Data type constraints should be const */
    int max_varchar_length = 255;
    int max_text_length = 65535;
    int decimal_precision = 10;
    int decimal_scale = 2;

    printf("\\nSQL Data Types:\\n");
    printf("  String types: %s(%d), %s(%d)\\n",
           type_varchar, max_varchar_length, type_text, max_text_length);
    printf("  Numeric types: %s, %s, %s(%d,%d)\\n",
           type_int, type_bigint, type_decimal, decimal_precision, decimal_scale);
    printf("  Other types: %s, %s, %s, %s\\n",
           type_datetime, type_boolean, type_text, type_blob);

    /* Data types used for schema creation but never modified */
    char column_def[128];
    sprintf(column_def, "%s %s(%d) NOT NULL", "name", type_varchar, max_varchar_length);
    printf("  Column definition: %s\\n", column_def);
}

void index_definitions(void) {
    /* NON-COMPLIANT: Index types should be const */
    char index_btree[] = "BTREE";
    char index_hash[] = "HASH";
    char index_fulltext[] = "FULLTEXT";
    char index_spatial[] = "SPATIAL";

    /* NON-COMPLIANT: Index creation templates should be const */
    char create_index[] = "CREATE INDEX %s ON %s (%s)";
    char create_unique_index[] = "CREATE UNIQUE INDEX %s ON %s (%s)";
    char drop_index[] = "DROP INDEX %s";

    /* NON-COMPLIANT: Index naming patterns should be const */
    char idx_prefix[] = "idx_";
    char uk_prefix[] = "uk_";
    char fk_prefix[] = "fk_";

    printf("\\nDatabase Index Configuration:\\n");
    printf("  Index types: %s, %s, %s, %s\\n",
           index_btree, index_hash, index_fulltext, index_spatial);
    printf("  Index prefixes: %s, %s, %s\\n", idx_prefix, uk_prefix, fk_prefix);

    printf("  CREATE INDEX: %s\\n", create_index);
    printf("  CREATE UNIQUE: %s\\n", create_unique_index);
    printf("  DROP INDEX: %s\\n", drop_index);

    /* Index templates used for DDL generation but never modified */
    char index_sql[256];
    sprintf(index_sql, create_index, "idx_users_email", "users", "email");
    printf("  Generated DDL: %s\\n", index_sql);
}

void connection_parameters(void) {
    /* NON-COMPLIANT: Connection string components should be const */
    char param_host[] = "host";
    char param_port[] = "port";
    char param_database[] = "database";
    char param_username[] = "username";
    char param_password[] = "password";
    char param_charset[] = "charset";
    char param_timeout[] = "timeout";

    /* NON-COMPLIANT: Default values should be const */
    char default_host[] = "localhost";
    int default_port = 3306;
    char default_charset[] = "utf8mb4";
    int default_timeout = 30;

    printf("\\nDatabase Connection Parameters:\\n");
    printf("  Parameters: %s, %s, %s, %s\\n",
           param_host, param_port, param_database, param_username);
    printf("  Defaults: %s=%s, %s=%d, %s=%s, %s=%d\\n",
           param_host, default_host, param_port, default_port,
           param_charset, default_charset, param_timeout, default_timeout);

    /* Parameters used for connection building but never modified */
    char connection_string[256];
    sprintf(connection_string, "%s=%s;%s=%d;%s=%s",
           param_host, default_host, param_port, default_port, param_charset, default_charset);
    printf("  Connection string: %s\\n", connection_string);
}

int main(void) {
    /* NON-COMPLIANT: Database configuration should be const */
    char db_engine[] = "InnoDB";
    char db_charset[] = "utf8mb4";
    char db_collation[] = "utf8mb4_unicode_ci";
    int max_connections = 100;
    int query_cache_size = 16777216;

    printf("Database Configuration:\\n");
    printf("  Engine: %s\\n", db_engine);
    printf("  Charset: %s\\n", db_charset);
    printf("  Collation: %s\\n", db_collation);
    printf("  Max connections: %d\\n", max_connections);
    printf("  Query cache: %d bytes\\n", query_cache_size);

    table_definitions();
    sql_templates();
    data_types();
    index_definitions();
    connection_parameters();

    return 0;
}