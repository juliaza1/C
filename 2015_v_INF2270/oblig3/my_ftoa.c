void add_int (char **sp, int v)
{
  if (v >= 10) {
    add_int(sp, v/10);
  }
  **sp = v%10 + '0';  (*sp)++;
}

void my_ftoa (char *s, double f)
{
  int n, i;

  if (f < 0.0) {
    *s = '-';  s++;  f = -f;
  }
  f += 0.0000005;

  n = (int)f;  f -= n;  add_int(&s, n);
  *s = '.';  s++;
  for (i = 1;  i <= 6;  i++) {
    f *= 10.0;  n = (int)f;  f -= n;
    *s = n + '0';  s++;
  }

  *s = 0;
}
