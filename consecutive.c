#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	if(argc < 2){
		exit(1);
	}

	FILE *file;
	file = fopen(argv[1], "r");
	int previous = 0;
	int line = 0;
	char out[255];
	int first = 1;

	while(!feof(file)){
		memset(out, 0x00, 255);
		fscanf(file, "%s", out);
		if(atoi(out)){
			if(first == 1){
				first = 0;
			}
			else{
				if(atoi(out) != (previous + 1)){
					printf("Lines %d and %d: %d, %s\n", line-1, line, previous, out);
				}
			}
		}

		previous = atoi(out);
		line++;
	}
	fclose(file);

	return 0;
}