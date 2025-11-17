/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: protocol_constants.c
 *
 * This case demonstrates violations where protocol constants
 * and communication parameters are not const-qualified.
 */

#include <stdio.h>

void http_protocol(void) {
    /* NON-COMPLIANT: HTTP method strings should be const */
    char method_get[] = "GET";
    char method_post[] = "POST";
    char method_put[] = "PUT";
    char method_delete[] = "DELETE";
    char method_patch[] = "PATCH";

    /* NON-COMPLIANT: HTTP status codes should be const */
    int status_ok = 200;
    int status_created = 201;
    int status_bad_request = 400;
    int status_unauthorized = 401;
    int status_not_found = 404;
    int status_server_error = 500;

    /* NON-COMPLIANT: HTTP headers should be const */
    char header_content_type[] = "Content-Type";
    char header_authorization[] = "Authorization";
    char header_user_agent[] = "User-Agent";
    char header_accept[] = "Accept";

    printf("HTTP Protocol Constants:\n");
    printf("  Methods: %s, %s, %s, %s, %s\n",
           method_get, method_post, method_put, method_delete, method_patch);
    printf("  Status codes: %d, %d, %d, %d, %d, %d\n",
           status_ok, status_created, status_bad_request,
           status_unauthorized, status_not_found, status_server_error);
    printf("  Headers: %s, %s, %s, %s\n",
           header_content_type, header_authorization, header_user_agent, header_accept);

    /* Constants used for protocol handling but never modified */
    char current_method[] = "GET";
    if (strcmp(current_method, method_get) == 0) {
        printf("  Processing GET request\n");
    }
}

void tcp_ip_protocol(void) {
    /* NON-COMPLIANT: TCP/IP constants should be const */
    int tcp_protocol = 6;
    int udp_protocol = 17;
    int icmp_protocol = 1;

    /* NON-COMPLIANT: Port numbers should be const */
    int http_port = 80;
    int https_port = 443;
    int ftp_port = 21;
    int ssh_port = 22;
    int smtp_port = 25;
    int dns_port = 53;

    /* NON-COMPLIANT: TCP flags should be const */
    unsigned char tcp_fin = 0x01;
    unsigned char tcp_syn = 0x02;
    unsigned char tcp_rst = 0x04;
    unsigned char tcp_psh = 0x08;
    unsigned char tcp_ack = 0x10;
    unsigned char tcp_urg = 0x20;

    printf("\nTCP/IP Protocol Constants:\n");
    printf("  Protocol numbers: TCP=%d, UDP=%d, ICMP=%d\n",
           tcp_protocol, udp_protocol, icmp_protocol);
    printf("  Well-known ports: HTTP=%d, HTTPS=%d, FTP=%d, SSH=%d\n",
           http_port, https_port, ftp_port, ssh_port);
    printf("  TCP flags: FIN=0x%02X, SYN=0x%02X, RST=0x%02X, ACK=0x%02X\n",
           tcp_fin, tcp_syn, tcp_rst, tcp_ack);

    /* Values used for packet analysis but never modified */
    int service_port = 80;
    if (service_port == http_port) {
        printf("  HTTP service detected\n");
    } else if (service_port == https_port) {
        printf("  HTTPS service detected\n");
    }
}

void ethernet_protocol(void) {
    /* NON-COMPLIANT: Ethernet constants should be const */
    int min_frame_size = 64;
    int max_frame_size = 1518;
    int header_size = 14;
    int fcs_size = 4;
    int vlan_tag_size = 4;

    /* NON-COMPLIANT: EtherType values should be const */
    unsigned short ethertype_ipv4 = 0x0800;
    unsigned short ethertype_ipv6 = 0x86DD;
    unsigned short ethertype_arp = 0x0806;
    unsigned short ethertype_vlan = 0x8100;

    printf("\nEthernet Protocol Constants:\n");
    printf("  Frame sizes: min=%d, max=%d bytes\n", min_frame_size, max_frame_size);
    printf("  Header size: %d bytes\n", header_size);
    printf("  FCS size: %d bytes\n", fcs_size);
    printf("  VLAN tag: %d bytes\n", vlan_tag_size);

    printf("  EtherTypes: IPv4=0x%04X, IPv6=0x%04X, ARP=0x%04X, VLAN=0x%04X\n",
           ethertype_ipv4, ethertype_ipv6, ethertype_arp, ethertype_vlan);

    /* Constants used for frame parsing but never modified */
    int payload_size = max_frame_size - header_size - fcs_size;
    printf("  Max payload: %d bytes\n", payload_size);
}

void serial_protocol(void) {
    /* NON-COMPLIANT: Serial communication parameters should be const */
    int baud_rate_9600 = 9600;
    int baud_rate_19200 = 19200;
    int baud_rate_38400 = 38400;
    int baud_rate_57600 = 57600;
    int baud_rate_115200 = 115200;

    /* NON-COMPLIANT: Data format constants should be const */
    int data_bits_7 = 7;
    int data_bits_8 = 8;
    int stop_bits_1 = 1;
    int stop_bits_2 = 2;

    /* NON-COMPLIANT: Parity options should be const */
    char parity_none[] = "NONE";
    char parity_even[] = "EVEN";
    char parity_odd[] = "ODD";
    char parity_mark[] = "MARK";
    char parity_space[] = "SPACE";

    printf("\nSerial Protocol Constants:\n");
    printf("  Baud rates: %d, %d, %d, %d, %d\n",
           baud_rate_9600, baud_rate_19200, baud_rate_38400,
           baud_rate_57600, baud_rate_115200);
    printf("  Data bits: %d, %d\n", data_bits_7, data_bits_8);
    printf("  Stop bits: %d, %d\n", stop_bits_1, stop_bits_2);
    printf("  Parity: %s, %s, %s, %s, %s\n",
           parity_none, parity_even, parity_odd, parity_mark, parity_space);

    /* Parameters used for configuration but never modified */
    int current_baud = baud_rate_115200;
    printf("  Current configuration: %d baud, %d data bits, %d stop bit, %s parity\n",
           current_baud, data_bits_8, stop_bits_1, parity_none);
}

int main(void) {
    /* NON-COMPLIANT: Protocol version numbers should be const */
    int http_version_1_0 = 10;
    int http_version_1_1 = 11;
    int http_version_2_0 = 20;
    int tls_version_1_2 = 0x0303;
    int tls_version_1_3 = 0x0304;

    printf("Protocol Versions:\n");
    printf("  HTTP: 1.0=%d, 1.1=%d, 2.0=%d\n",
           http_version_1_0, http_version_1_1, http_version_2_0);
    printf("  TLS: 1.2=0x%04X, 1.3=0x%04X\n", tls_version_1_2, tls_version_1_3);

    http_protocol();
    tcp_ip_protocol();
    ethernet_protocol();
    serial_protocol();

    return 0;
}