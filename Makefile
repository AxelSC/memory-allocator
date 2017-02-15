build: allocator
	
allocator: tema2.c
	gcc -o allocator tema2.c
clean:
	rm allocator