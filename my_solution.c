#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){
	if(argc < 4){
		exit(1);
	}

	FILE *file; /* increment file */

	file = fopen(argv[2], "w"); /* clear file to be written into */
	fclose(file);

	int myPid = getpid();
	file = fopen(argv[3], "a"); /* add PID to PID file */
	fprintf(file, "%d\n", myPid);
	fclose(file);

	int contains = 0; /* loop until both PIDs are present */
	while(contains < 2){
		sleep(1);
		char check[255];
		file = fopen(argv[3], "r");
		contains = 0;
		fseek(file, 0, SEEK_SET);
		while(!feof(file)){
			fscanf(file, "%[^\n]\n", check);
			contains++;
		}
		fclose(file);
	}

	int otherPid; /* find other process' PID */
	int rank = 0; /* the first process will have rank 0, the other 1 */
	char check[255];
	file = fopen(argv[3], "r");
	fseek(file, 0, SEEK_SET);
	while(!feof(file)){
		memset(check, 0x00, 255);
		fscanf(file, "%[^\n]\n", check);
		if(atoi(check) != myPid){
			otherPid = atoi(check);
			rank = 1 - rank;
			break;
		}
		rank++;
	}
	fclose(file);

	memset(check, 0x00, 255);
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

		set_sv(1, &status); /* interested */


		while(1){
			/* if other process isn't interested */
			if((get_sv(otherPid, &status) & 1) == 0){
				set_sv(3, &status);
				break;
			}
			/* if both processes are interested but haven't entered the critical section yet */
			if((get_sv(myPid, &status) == 1) && (get_sv(otherPid, &status) == 1)){
				if(rank == 0){
					set_sv(3, &status);
					break;
				}
			}
		}

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

		set_sv(0, &status); /* not interested anymore */
		
		
		i++;
	}

	return 0;
}
