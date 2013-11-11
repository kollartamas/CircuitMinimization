all:
	cd alanmi_abc && make libabc.a
	gcc -c ./Espresso/*.c -iquote./Espresso
	g++-4.8 ./*.cpp ./Quine_McClusky/*.cpp *.o -iquote./Espresso -iquote./Quine_McClusky -iquote./Abc -std=c++0x ./alanmi_abc/libabc.a -D ABC_NAMESPACE=abc -DLIN -D LOG_ALL -w -fpermissive -lrt -ldl




