<Programs with no conditional statements>
./goto-analyzer ../../minimal/no_conds_1.c  --show-goto-functions   --tvpi --verbosity 10 --recursive-interprocedural
./goto-analyzer ../../minimal/no_conds_1.c --show --tvpi --verbosity 10 --recursive-interprocedural

./goto-analyzer ../../minimal/one_var.c  --show --tvpi --verbosity 10 --recursive-interprocedural


./goto-analyzer ../../minimal/article_one.c  --show --tvpi --verbosity 10 --recursive-interprocedural
./goto-analyzer ../../minimal/one_var.c  --show --tvpi --verbosity 10 --recursive-interprocedural

SUBSTITUTING VARS 
Dimension count is only dimension, not variable
you increment when binding starts using it
decrement when biding stops using it 
assign decrements what it used to be
then increment to what you are assigned in too 
invariant
in ref count approach
vars in maths world are not mutable but in program world they are

Cleanup should be done at the end of transform
appears multiple times design: ref count of how many times each dimension used 
increment and increment only in assign
sum<+ a + b
b<= u
project b out
sum <= a + u
look at the items so one of thigs will be the found 
math + smt 
closure is giving a normal form 


Fix dev tools
sudo rm -rf /Library/Developer/CommandLineTools/
xcode-select --install

./goto-analyzer ../../minimal/mult.c  --show --tvpi --verbosity 10 --recursive-interprocedural
./goto-analyzer ../../minimal/equal.c  --show --tvpi --verbosity 10 --recursive-interprocedural

./goto-analyzer ../../minimal/sub.c  --show --tvpi --verbosity 10 --recursive-interprocedural



new format
./goto-analyzer ../../example_programs/equal.c --show --tvpi --verbosity 10 --recursive-interprocedural


min
unsat core
minimal unsat core