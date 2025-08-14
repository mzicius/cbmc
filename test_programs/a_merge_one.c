int main(void)
{
  int x = 4;
  assert(x>=4);
  assert(x<=4);

  while(x < 12)
  {
      assert(x>=4);
      assert(x<=11);
    x = x + 1;
      assert(x>=5);
      assert(x<=12);
  }
      //loosing lower bound is bad
      assert(x>=12);
      //from 4 to 12 forward edge 
     // 1d41≤12
      //The redundant inequality is: -1d41≤-12
//The redundant inequality is: -1d41≤-12
      assert(x<=12);

  return x;
}