#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	if(argc < 3){
		exit(1);
	}

	FILE *file;
	char check[255];

	file = fopen(argv[2], "r"); /* if file is empty, write 1 into it */
	fscanf(file, "%s", check);
	if(!atoi(check)){
		fclose(file);
		file = fopen(argv[2], "w");
		fprintf(file, "%d\n", 1);
		fclose(file);
	}

	int i = 0;
	int end = atoi(argv[1]);
	while(i < end){ /* loop and increment */
		char out[255];
		int status;

		/*critical section begins*/
		file = fopen(argv[2], "r");
		fseek(file, 0, SEEK_SET);
		while(!feof(file)){
			memset(out, 0x00, 255);
			fscanf(file, "%[^\n]\n", out);
		}
		fclose(file);

		file = fopen(argv[2], "a");
		fprintf(file, "%d\n", atoi(out)+1);
		fclose(file);
		/*critical section ends*/
		
		i++;
	}

	return 0;
}
