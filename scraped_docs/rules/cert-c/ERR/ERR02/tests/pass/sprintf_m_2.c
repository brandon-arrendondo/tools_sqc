int i;
rsize_t count = 0;
errno_t err;

for (i = 0; i < 9; ++i) {
  err = sprintf_m(
    buf + count, "%02x ", &count, ((u8 *)&slreg_num)[i]
  );
  if (err != 0) {
    /* Handle print error */
  }
}
err = sprintf_m(
  buf + count, "%02x ", &count, ((u8 *)&slreg_num)[i]
);
if (err != 0) {
  /* Handle print error */
}