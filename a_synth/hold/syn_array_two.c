//array_doub_access_init_const.c
//unreach_call: true 
//scenario: iterate over large loop bound
//
int main()
{
  int i;
  int N=5;
  int a[102];

  for(i=0;i<=N;i++) {
    a[2*i]=0;
    a[2*i+1]=0;
  }

  //for(i=0;i<=2*N;i++)
  //  __VERIFIER_assert(a[i]>=0);
  for(i=0;i<=2*N;i++)
    __CPROVER_assert(a[i]>=0,"array element is non-negative");
  
  return 0;
}
