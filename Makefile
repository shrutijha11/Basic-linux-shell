#makefile assignment 1:part 2

compiler=gcc
outF=a.out

compile:  
	$(compiler) myls.c -o myls
	$(compiler) mycat.c -o mycat
	$(compiler) mydate.c -o mydate
	$(compiler) mymkdir.c -o mymkdir
	$(compiler) myrm.c -o myrm
	$(compiler) myshell.c -lreadline
	
run:
	./$(outF)

clean:
	rm $(outF)