/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: api_endpoints.c
 *
 * This case demonstrates violations where API endpoint paths
 * and HTTP constants are not const-qualified.
 */

#include <stdio.h>

void rest_endpoints(void) {
    /* NON-COMPLIANT: API endpoint paths should be const */
    char api_base[] = "/api/v1";
    char users_endpoint[] = "/api/v1/users";
    char products_endpoint[] = "/api/v1/products";
    char orders_endpoint[] = "/api/v1/orders";
    char auth_endpoint[] = "/api/v1/auth";

    /* NON-COMPLIANT: Endpoint patterns should be const */
    char user_by_id[] = "/api/v1/users/%d";
    char product_by_id[] = "/api/v1/products/%d";
    char user_orders[] = "/api/v1/users/%d/orders";
    char order_items[] = "/api/v1/orders/%d/items";

    printf("REST API Endpoints:\\n");
    printf("  Base: %s\\n", api_base);
    printf("  Users: %s\\n", users_endpoint);
    printf("  Products: %s\\n", products_endpoint);
    printf("  Orders: %s\\n", orders_endpoint);
    printf("  Auth: %s\\n", auth_endpoint);

    printf("\\nParameterized Endpoints:\\n");
    printf("  User by ID: %s\\n", user_by_id);
    printf("  Product by ID: %s\\n", product_by_id);
    printf("  User orders: %s\\n", user_orders);

    /* Endpoints used for routing but never modified */
    char full_url[256];
    sprintf(full_url, user_by_id, 123);
    printf("  Sample URL: %s\\n", full_url);
}

void http_headers(void) {
    /* NON-COMPLIANT: HTTP header names should be const */
    char header_accept[] = "Accept";
    char header_content_type[] = "Content-Type";
    char header_authorization[] = "Authorization";
    char header_user_agent[] = "User-Agent";
    char header_cache_control[] = "Cache-Control";

    /* NON-COMPLIANT: Content type values should be const */
    char content_json[] = "application/json";
    char content_xml[] = "application/xml";
    char content_form[] = "application/x-www-form-urlencoded";
    char content_multipart[] = "multipart/form-data";
    char content_text[] = "text/plain";

    printf("\\nHTTP Headers:\\n");
    printf("  Headers: %s, %s, %s, %s\\n",
           header_accept, header_content_type, header_authorization, header_user_agent);

    printf("  Content Types: %s, %s, %s\\n",
           content_json, content_xml, content_form);

    /* Headers used for request processing but never modified */
    printf("  Default Accept: %s\\n", content_json);
    printf("  Default Content-Type: %s\\n", content_json);
}

void response_codes(void) {
    /* NON-COMPLIANT: HTTP status codes should be const */
    int status_continue = 100;
    int status_ok = 200;
    int status_created = 201;
    int status_accepted = 202;
    int status_no_content = 204;
    int status_moved_permanently = 301;
    int status_found = 302;
    int status_not_modified = 304;
    int status_bad_request = 400;
    int status_unauthorized = 401;
    int status_forbidden = 403;
    int status_not_found = 404;
    int status_method_not_allowed = 405;
    int status_conflict = 409;
    int status_internal_error = 500;
    int status_not_implemented = 501;
    int status_bad_gateway = 502;
    int status_service_unavailable = 503;

    printf("\\nHTTP Status Codes:\\n");
    printf("  Success: %d, %d, %d, %d\\n",
           status_ok, status_created, status_accepted, status_no_content);
    printf("  Redirect: %d, %d, %d\\n",
           status_moved_permanently, status_found, status_not_modified);
    printf("  Client Error: %d, %d, %d, %d\\n",
           status_bad_request, status_unauthorized, status_forbidden, status_not_found);
    printf("  Server Error: %d, %d, %d\\n",
           status_internal_error, status_bad_gateway, status_service_unavailable);

    /* Status codes used for response handling but never modified */
    int response_status = status_ok;
    if (response_status == status_ok) {
        printf("  Request successful\\n");
    }
}

int main(void) {
    /* NON-COMPLIANT: API configuration should be const */
    char api_version[] = "v1";
    char api_host[] = "api.example.com";
    int api_port = 443;
    char api_protocol[] = "https";

    printf("API Configuration:\\n");
    printf("  Version: %s\\n", api_version);
    printf("  Host: %s\\n", api_host);
    printf("  Port: %d\\n", api_port);
    printf("  Protocol: %s\\n", api_protocol);

    rest_endpoints();
    http_headers();
    response_codes();

    return 0;
}