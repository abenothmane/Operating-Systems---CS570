
#include "p2.h"
#include "getword.h"
#include "CHK.h"


void pipeLine();
char argv[MAXSTORAGE];
char *newargv[MAXITEM];
char *fin , *fout;		 // file-in and file-out pointers in argv  
int eofTracker;			//detect if there is a EOF character
int bigger = 0,inf = 0,outf = 0 , pipef = 0;    //trackers for  <, >, |, &
extern int tracker;     // $ tracker
int pipe = 0;           // refers to the second newargv in in pipe.
int o_file, i_file;	    // helps with redirecting in-file and out-file
int main(){
	signal(SIGTERM,myhandler);
	pid_t pid, child_pid;
	DIR *dirp;
	struct stat sb;
	struct dirent *dp;
	int argc;
	int o_file, i_file;	//this is used for redirection
	for(;;){
		printf("p2: ");
		argc = getArgument();
		if(eofTracker == -2){
			fprintf(stderr,"Unmatched '.\n");
			continue;
		}
		if(eofTracker == -1)  //This will terminate the program when getword sees EOF
			break;
		if(argc == 0)		//Will re-issue prompt if no words entered
			continue;
		if(newargv[0] == NULL){		//This will be used if the <,> are flagged and no arg
			fprintf(stderr,"Invalid null command.\n");
			continue;
		}
	
		if(pipef != 0){												/*if pipeline is encountered*/
			if(pipef > 1){
				fprintf(stderr,"Syntax Error: double '|'\n");
				continue;
			}
			else{
				 pipeLine();										/*if encountered only once, call pipeLine()*/
				
			}
		}
/////////////////printenv code//////////////////////////////
		if(strcmp(newargv[0],"printenv") == 0){
			if(argc <= 1 || argc > 2)	//Too many arguments to cd
				fprintf(stderr,"printenv: Must contain only ONE argument.\n");
			else{
				printf("printenv seen\n");
			}
			continue;
		}		
/////////////////setenv code//////////////////////////////
		if(strcmp(newargv[0],"setenv") == 0){
			if(argc <= 2 || argc > 3)  //Only one argument allowed
				fprintf(stderr,"setenv: Must contain only TWO argument.\n");
			else{
				printf("setenv seen\n");
			}
			continue;
		}				
/
		if(strcmp(newargv[0],"cd") == 0){                                         /*if cd is encountered*/
			if(argc > 2)	//Too many arguments to cd
				fprintf(stderr,"cd: Too many arguments.\n");
			else if(argc == 1){	//cd was entered with no arguments 
				if(chdir(getenv("HOME")) != 0)
					fprintf(stderr,"cd: could not find home directory.\n");
			}
			else{	//argc is two so you want to cd to the argument
				if(chdir(newargv[1]) != 0)
					fprintf(stderr,"%s: No such file or directory.\n",newargv[1]);
			}
			continue;
		}

		if(strcmp(newargv[0],"ls-F") == 0){
			if(argc > 2){	//Too many arguments to ls-F
				fprintf(stderr,"ls-F: Too many arguments.\n");
			}
			if(argc == 1){	//ls-F was entered with no arguments
				dirp = opendir(".");
				if(stat(".",&sb) == -1){
					fprintf(stderr,"stat ls-F failed.\n");
					exit(EXIT_FAILURE);
				}
				while(dirp){ //This will read through the file
					if((dp = readdir(dirp)) != NULL){
						if(stat(dp->d_name, &sb) == -1)
							fprintf(stderr,"stat ls-F failed.\n");
						if((sb.st_mode & S_IFMT) == S_IFDIR) //This states its a directory
							printf("%s\n",dp->d_name);
						else
							printf("%s\n",dp->d_name);
					}
					else{
						closedir(dirp);
						break;
					}
				}
			}	
			else{	//This executes when ls-F has an argument with it
				if(stat(newargv[1], &sb) == -1){
					fprintf(stderr,"%s: No such file or directory.\n",newargv[1]);
					continue;
				}
				if((sb.st_mode & S_IFMT) != S_IFDIR){ //IF NOT DIRECTORY
					printf("%s\n",newargv[1]);
					continue;
				}
				if(chdir(newargv[1]) != 0){ //If a directory but unreadable
					fprintf(stderr,"%s is unreadable\n",newargv[1]);
					continue;
				}
				if((dirp = opendir(newargv[1])) == NULL){
					printf("%s: failed to open directory\n",newargv[1]);
					continue;
				}	
				while(dirp){
					if((dp = readdir(dirp)) != NULL){
						if(stat(dp->d_name, &sb) == -1)
							fprintf(stderr,"stat ls-F failed.\n");
						if((sb.st_mode & S_IFMT) == S_IFDIR)
							printf("%s\n",dp->d_name);
						else
							printf("%s\n",dp->d_name);
					}
					else{
						closedir(dirp);
						break;
					}
				}
			}
			continue;
		}
																/*redirects when '<' is encountered*/               
		if(outf != 0){
			int flags = O_CREAT | O_EXCL | O_RDWR ;				/*Enforces argument number (1), name input*/
			if(outf > 2){										/*and existence of the file*/
				fprintf(stderr,"Only one argument is needed.\n");
				continue;
			}
			if(fout == NULL){
				fprintf(stderr,"Input Name required.\n");
				continue;
			}
			if((o_file = open(fout,flags,S_IRWXU)) < 0){
				fprintf(stderr,"%s: Already Exists.\n",fout);
				outf = 0;
				continue;
			}
		}
																/*redirects when '<' is encountered*/
		if(inf != 0){											/*Enforces argument number (1), name input*/
			int flags = O_RDONLY;								/*and existence of the file*/
			if(inf > 2){
				fprintf(stderr,"Ambiguous input redirect.\n");
				continue;
			}
			if(fin == NULL){
				fprintf(stderr,"Missing name for redirect.\n");
				continue;
			}
			if((i_file = open(fin,flags)) < 0){
				fprintf(stderr,"Failed to open: %s\n",fin);
				continue;
			}
		}
																	/*fork code*/
		fflush(stdout);
		fflush(stderr);
		child_pid = fork();
		if(child_pid < 0){
			printf("Terminating! Cant Fork!");
			exit(1);
		}
		else if(child_pid == 0){
			if(bigger != 0 && inf == 0){		                       /*redirecting stdin to /dev/null*/
				int devnull;
				int flags = O_RDONLY;
				if((devnull = open("/dev/null",flags)) < 0){
					fprintf(stderr,"Failed to open /dev/null.\n");
					exit(9);
				}
				dup2(devnull,STDIN_FILENO);
				close(devnull);
			}
			if(outf != 0){                                        /*redirecting standard output to the output file*/
				dup2(o_file,STDOUT_FILENO);
				close(o_file);
			}
			if(inf != 0){
				dup2(i_file,STDIN_FILENO);                       /*redirecting standard output to the input file*/
				close(i_file);
			}
			if((execvp(*newargv, newargv)) < 0){	                       /*execute the command and check if it exists*/
				fprintf(stderr,"%s: Command not found.\n",newargv[0]);
				exit(9);
			}
				
		}

		if(bigger !=0){													/*Check if we can place in the backgroung*/
			printf("%s [%d]\n", *newargv , child_pid);					/*then set stdin to /dev/null*/
			bigger = 0;
			continue;
		}
		else{	
			for(;;){
				pid = wait(NULL);
				if(pid == child_pid)
					break;
			}
		}
	}
	killpg(getpid(),SIGTERM);
	printf("p2 terminated.\n");
	exit(0);
}


int getArgument(){                                                   /*Gets the argument from stdin*/
	
	int wcounter=0;    
	fin = '\0';
	fout = '\0';
	inf = 0;
	outf = 0;
	bigger = 0;
	pipef = 0;
	int p = 0;
	int ptr = 0;
	
	while((eofTracker = getword(argv + ptr)) > 0 ){
		if(eofTracker == -2){										/*Checks for second quote*/
			break;
		}
		if(*(argv+ptr) == '$' && tracker == 0){						/*if $ is encountered for the first time exit program*/
			break;
		}
		if(*(argv+ptr) == '|' && pipef == 0){						/*if | is encountered for the first time call pipeline*/
			pipeLine();
			
		}		
		if(*(argv+ptr) == '&'){	
			 if((fin == NULL && fout == NULL) && (inf == 1 || outf == 1)){ /*checks for & */
				break;
			}
			bigger++;
			break;
		}
		else if((*(argv+ptr) == '<' && eofTracker == 1) || inf == 1){	/* tracks < */
			inf++;
			if(inf == 2)
				fin = argv +ptr;
		}
		else if((*(argv+ptr) == '>' && eofTracker == 1) || outf == 1){ /* tracks > */
			outf++;
			if(outf == 2)
				fout = argv+ptr;
		}
		else{
			newargv[p++] =  argv + ptr;
		}
	
		argv[ptr+ eofTracker] = '\0';
		ptr += eofTracker+1;
		wcounter++;

	}
	newargv[p] = NULL;
	return wcounter;
	
}

void pipeLine()
{
	int fildes[2];
   	pid_t first, second;

   	CHK(pipe(fildes));

   	CHK(first = fork());                                                              /*First Fork*/
   	if (0 == first) 
	{
      		
      		CHK(dup2(fildes[1],STDOUT_FILENO));
      		CHK(close(fildes[0]));
      		CHK(close(fildes[1]));
		if(inf != 0){
			CHK(dup2(i_file,STDIN_FILENO));
			CHK(close(i_file));
		}
      		if((execvp(*newargv, newargv)) < 0){	//execute the command
			fprintf(stderr,"%s: Command not found.\n",newargv[0]);
			exit(9);
		}
   	}  
																					/*Second Fork*/
   	CHK(second = fork());
   	if (second == 0) 
	{
      		CHK(dup2(fildes[0],STDIN_FILENO));
      		CHK(close(fildes[0]));
      		CHK(close(fildes[1]));
		if(outf != 0){
			CHK(dup2(o_file,STDOUT_FILENO));
			CHK(close(o_file));
		}
      		if((execvp(newargv[pipe], newargv+pipe)) < 0){	
			fprintf(stderr,"%s: Command not found.\n",newargv[0]);
			exit(9);
		}
   	}
																					/* only the parent reaches this point in the code. */

   	CHK(close(fildes[0]));
   	CHK(close(fildes[1]));

																					/* wait() might find the first child; continue reaping children until
																					the second child is found */

	
	if(bigger != 0 && inf == 0){													/*redirecting stdin to /dev/null */
		int devnull;
		int flags = O_RDONLY;
		if((devnull = open("/dev/null",flags)) < 0){
			fprintf(stderr,"Failed to open /dev/null.\n");
			exit(9);
		}
		CHK(dup2(devnull,STDIN_FILENO));
		CHK(close(devnull));
	}
	if(bigger !=0){	 																/*Check if we can place in the backgroung*/
		printf("%s [%d]\n", *newargv , first);										 /*then set stdin to /dev/null*/
		bigger = 0;
	}
	else{	
		for(;;){
			pid_t pid;
			CHK(pid = wait(NULL));
			if(pid == second)
				break;
		}
	}
}


void myhandler(){};                    												/*Sigterm*/