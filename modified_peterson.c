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
	file = fopen(argv[3], "a");
	fprintf(file, "%d\n", myPid);
	fclose(file);

	int otherPid; /* find other process' PID */
	int rank = 0; /* the first process will have rank 0, the other 1 */
	char check[255];
	while(1){
		file = fopen(argv[3], "r");
		int x, y;
		sleep(1); /* keep from overloading */
		fscanf(file, "%d %d", &x, &y);
	
		if((x != 0) && (y != 0)){ /* check for both PIDs */
			if(x == myPid){
				otherPid = y;
			}
			else{
				otherPid = x;
			}
			break;
		}
		fclose(file);
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

	int turn = 0;
	int i = 0;
	int end = atoi(argv[1]);
	while(i < end){ /* loop and increment */
		char out[255];
		int status;

		int mySV = (get_sv(myPid, &status) | 1);
		set_sv(mySV, &status); /* interested */

		turn = (((mySV ^ get_sv(otherPid, &status)) & 2) >> 1);
		if(turn != rank){
			mySV = mySV ^ 2;
			set_sv(mySV, &status); /* my turn */
			/* & 2 is added for redundancy */
			turn = (((mySV ^ get_sv(otherPid, &status)) & 2) >> 1);
		}
		
		/* while other process is interested and turn = me */
		while((((get_sv(otherPid, &status)) & 1) == 1) && turn == rank){
			/* & 2 is added for redundancy */
			turn = (((mySV ^ get_sv(otherPid, &status)) & 2) >> 1);
			/* spin */
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
		
		mySV = (mySV & 2);
		set_sv(mySV, &status); /* not interested */
		
		i++;
	}

	return 0;
}
