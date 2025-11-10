volatile bool flag = false;

void test() {
  while (!flag){
    sleep(1000);
  }
}

void wakeup(){
  flag = true;
}

void debit(unsigned int amount) {
  test();
  account_balance -= amount;
}