backup: backup.c
	gcc -Wall -Wextra -o backup backup.c

run: backup
	./backup part_3_test dest_dir

clean:
	rm -f backup
	rm -rf dest_dir

