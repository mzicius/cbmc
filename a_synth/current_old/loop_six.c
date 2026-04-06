int main()
{
  int i;
  int c;
  __CPROVER_assume(c >= 0);
  i = 0;
  while(i < c)
  {
    __CPROVER_assert(i < c, "i l");
       __CPROVER_assert(i == c, "i eq c");
    i = i + 1;
    __CPROVER_assert(i <= c, "i leq c");
   __CPROVER_assert(i == c, "i eq c");
  }
  __CPROVER_assert(i >= c, "i geq c");
  __CPROVER_assert(i <= c, "i leq c");
  __CPROVER_assert(i == c, "i eq c");
}