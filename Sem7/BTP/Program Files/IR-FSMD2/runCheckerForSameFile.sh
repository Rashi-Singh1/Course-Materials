#make -f Makefile2
#for f in *.ll; do
#  ./a.out $f
#  echo $f > out.txt
#  cat FSMD.txt > out.txt
#done
./bin/FsmdaEqvChecker FSMD.txt FSMD.txt variables.txt variables.txt
