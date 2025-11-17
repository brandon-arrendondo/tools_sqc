/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: xml_parser_unsafe.c
 *
 * This case demonstrates violations where XML parsing functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simple XML node structure */
typedef struct XmlNode {
    char *tag_name;
    char *text_content;
    struct XmlNode **children;
    size_t child_count;
    struct XmlNode *parent;
} XmlNode;

/* Simple XML attribute structure */
typedef struct {
    char *name;
    char *value;
} XmlAttribute;

/* NON-COMPLIANT: No validation of XML string */
XmlNode *parse_xml(const char *xml_string) {
    /* No validation of xml_string */
    XmlNode *root = malloc(sizeof(XmlNode));

    /* Mock parsing without validation */
    const char *tag_start = strchr(xml_string, '<');  /* xml_string could be NULL */
    const char *tag_end = strchr(tag_start + 1, '>');

    size_t tag_length = tag_end - tag_start - 1;
    root->tag_name = malloc(tag_length + 1);
    strncpy(root->tag_name, tag_start + 1, tag_length);
    root->tag_name[tag_length] = '\0';

    root->children = NULL;
    root->child_count = 0;
    root->parent = NULL;

    return root;
}

/* NON-COMPLIANT: No validation of node or tag name */
XmlNode *find_child_by_tag(XmlNode *parent, const char *tag_name) {
    /* No validation of parent or tag_name */
    for (size_t i = 0; i < parent->child_count; i++) {  /* parent could be NULL */
        if (strcmp(parent->children[i]->tag_name, tag_name) == 0) {  /* tag_name could be NULL */
            return parent->children[i];
        }
    }
    return NULL;
}

/* NON-COMPLIANT: No validation of attribute parameters */
char *get_attribute_value(XmlNode *node, const char *attr_name) {
    /* No validation of node or attr_name */
    printf("Getting attribute '%s' from node '%s'\n", attr_name, node->tag_name);  /* Both could be NULL */

    /* Mock attribute retrieval */
    static char mock_value[256];
    sprintf(mock_value, "value_of_%s", attr_name);  /* attr_name could be NULL */
    return mock_value;
}

/* NON-COMPLIANT: No validation of text content setting */
void set_text_content(XmlNode *node, const char *text) {
    /* No validation of node or text */
    if (node->text_content) {  /* node could be NULL */
        free(node->text_content);
    }

    node->text_content = malloc(strlen(text) + 1);  /* text could be NULL */
    strcpy(node->text_content, text);
}

/* NON-COMPLIANT: No validation of child addition */
void add_child_node(XmlNode *parent, XmlNode *child) {
    /* No validation of parent or child */
    parent->child_count++;  /* parent could be NULL */
    parent->children = realloc(parent->children, parent->child_count * sizeof(XmlNode *));
    parent->children[parent->child_count - 1] = child;
    child->parent = parent;  /* child could be NULL */
}

/* NON-COMPLIANT: No validation of node creation parameters */
XmlNode *create_xml_node(const char *tag_name, const char *text_content) {
    XmlNode *node = malloc(sizeof(XmlNode));

    /* No validation of tag_name */
    node->tag_name = malloc(strlen(tag_name) + 1);  /* tag_name could be NULL */
    strcpy(node->tag_name, tag_name);

    if (text_content) {  /* Partial validation, but not complete */
        node->text_content = malloc(strlen(text_content) + 1);
        strcpy(node->text_content, text_content);
    } else {
        node->text_content = NULL;
    }

    node->children = NULL;
    node->child_count = 0;
    node->parent = NULL;

    return node;
}

/* NON-COMPLIANT: No validation of serialization parameters */
char *serialize_xml(XmlNode *node, int indent_level) {
    /* No validation of node */
    char *result = malloc(4096);  /* Fixed size without checking content */

    /* No validation of indent_level */
    for (int i = 0; i < indent_level; i++) {
        strcat(result, "  ");
    }

    sprintf(result + strlen(result), "<%s>", node->tag_name);  /* node could be NULL */

    if (node->text_content) {
        strcat(result, node->text_content);
    }

    sprintf(result + strlen(result), "</%s>\n", node->tag_name);

    return result;
}

/* NON-COMPLIANT: No validation of XPath-like query */
XmlNode *query_xml(XmlNode *root, const char *xpath) {
    /* No validation of root or xpath */
    printf("Querying XML with XPath: %s\n", xpath);  /* xpath could be NULL */

    /* Mock XPath processing */
    char *path_copy = malloc(strlen(xpath) + 1);  /* xpath could be NULL */
    strcpy(path_copy, xpath);

    XmlNode *current = root;
    char *token = strtok(path_copy, "/");

    while (token && current) {  /* current could become NULL */
        current = find_child_by_tag(current, token);
        token = strtok(NULL, "/");
    }

    free(path_copy);
    return current;
}

/* NON-COMPLIANT: No validation of namespace parameters */
void set_namespace(XmlNode *node, const char *namespace_uri, const char *prefix) {
    /* No validation of any parameters */
    printf("Setting namespace %s:%s on node %s\n",
           prefix, namespace_uri, node->tag_name);  /* All could be NULL */
}

int main(void) {
    char *null_xml = NULL;
    XmlNode *null_node = NULL;
    char *null_string = NULL;

    /* Examples of dangerous XML operations */
    // parse_xml(null_xml);  /* NULL XML string */
    // find_child_by_tag(null_node, null_string);  /* NULL parameters */
    // get_attribute_value(null_node, null_string);  /* NULL parameters */
    // set_text_content(null_node, null_string);  /* NULL parameters */
    // add_child_node(null_node, null_node);  /* NULL parameters */
    // create_xml_node(null_string, "text");  /* NULL tag name */
    // serialize_xml(null_node, -5);  /* NULL node and negative indent */
    // query_xml(null_node, null_string);  /* NULL parameters */
    // set_namespace(null_node, null_string, null_string);  /* NULL parameters */

    printf("XML functions compiled but lack parameter validation\n");
    return 0;
}