all:
	gcc main.c common.c image_loader.c image_processing.c -lpthread -lm -o bin -Wall

san:
	gcc main.c common.c image_loader.c image_processing.c -lpthread -lm -o bin -Wall -fsanitize=address

checkstyle:
	perl /home/max/DEV/linux-4.14.22/scripts/checkpatch.pl -f *.h *.c