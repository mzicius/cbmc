int main(void)
{
  int x = 5;
  int y = 7;

  while(x + y < 15)
  {
    x = x + 1;
  }
  assert(x+y<=15);
  assert(x+y>=15);

  return x;
}