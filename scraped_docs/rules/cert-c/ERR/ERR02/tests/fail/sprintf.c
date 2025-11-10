int i;
ssize_t count = 0;

for (i = 0; i < 9; ++i) {
  count += sprintf(
    buf + count, "%02x ", ((u8 *)&slreg_num)[i]
  );
}
count += sprintf(buf + count, "\n");