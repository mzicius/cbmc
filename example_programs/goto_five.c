int main(void)
{
  int y = 7;
  int x;
  int c;
  __CPROVER_assume(x>2);

  if(x > 3)
  {
    c = 9;
  }
  else
  {
    c = 4;
  }

  return c;
}