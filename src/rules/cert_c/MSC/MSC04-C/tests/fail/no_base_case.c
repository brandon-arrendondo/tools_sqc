// MSC04-C fail: recursion with parameter but no conditional return
void process_all(int *data) {  // expected-warning {{MSC04-C}}
    *data += 1;
    process_all(data + 1);
}
