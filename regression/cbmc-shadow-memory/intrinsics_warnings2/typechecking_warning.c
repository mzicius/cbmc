//
// Created by Fotis Koutoulakis on 19/09/2023.
//
int main()
{
  __CPROVER_field_decl_local("uninitialized", 10ul);

  return 0;
}
