#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 

#define MAXLEN 1000 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported 
// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 

char ls_path[1024];
char cat_path[1024];
char date_path[1024];
char rm_path[1024];
char mkdir_path[1024];
char hist_file_path[1024];

// introduction to shell on running the shell 
void intro_shell() 
{ 
	clear(); 

	printf("\n\n\n\t*****WELCOME TO SHRUTI SHELL*****\n\n\n"); 
	char* username = getenv("USER"); 
	printf("\n\n\tUSER is: @%s", username); 
	printf("\n"); 
	sleep(1); 
	clear();

} 

// Function to take input 
int takeInput(char* str) 
{ 	
	//using_history();
	char* buf; 

	buf = readline(">>> "); 

	FILE* fptr= fopen(hist_file_path,"a");
	
	if (strlen(buf) != 0) 
	{ 
		add_history(buf); 
		strcpy(str, buf); 
		if(fptr!=NULL)
		{
			fputs(buf,fptr);
			fputc('\n',fptr);
			fclose(fptr);
		}

		return 0; //if input is non-empty and non-null
	} 
	else 
	{ 
		return 1; //if no input
	} 


} 

void internalcd(char ** parsed)
{
	if(parsed[1]==NULL || strcmp(parsed[1],"~")==0)
	{
		int ch=chdir("/home"); 
    
    	if(ch<0)
    		printf("cd: %s: No such file or directory.\n","/home");

	}

	else if(strcmp(parsed[1],"--help")==0)
	{
		printf("\t%s\n\t%s\n", "cd: cd [-L/--help] [dir]" ,"change the Shruti Shell working directory to 'dir'.");
		printf("\t%s\n", "The default directory is the 'home' directory.");
		printf("\t%s\n", "If 'dir' is NULL or '~', directory is changed to 'home'");
		printf("\t%s\n", "Options:");
		printf("\t%s\n", "-L:   force symbolic links to be followed: resolve symbolic links in 'dir' after processing instances of `..'");
		printf("\t%s\n", "--help:   print details of internal command 'cd' on STDOUT.");
		printf("\t%s\n", "The default is to follow symbolic links, as if `-L' were specified.");
		
	}

	else if(strcmp(parsed[1],"-L")==0 && parsed[2]==NULL)
	{
		int ch=chdir("/home"); 
    
    	if(ch<0)
    		printf("cd: %s: No such file or directory.\n","/home");
	}

	else if(strcmp(parsed[1],"-L")==0 && parsed[3]==NULL)
	{
		int ch=chdir(parsed[2]); 
    
    	if(ch<0)
    		printf("cd: %s: No such file or directory.\n",parsed[2]);

	}

	else if(strcmp(parsed[1],"-L")==0)
	{
	
    	printf("cd: too many arguments with -L option\n");

	}
	
	else if(parsed[2]==NULL)
	{
    	int ch=chdir(parsed[1]); 
    
    	if(ch<0)
    		printf("cd: %s: No such file or directory.\n",parsed[1]);
    	
    }

    else
    {
    	printf("cd: too many arguments.\n");
    }

    return ;

}



// Function to print Current Directory. 
void internalpwd(char** parsed) 
{ 
	if(parsed[1]==NULL)
	{
		char cwd[1024]; 

    	if (getcwd(cwd, sizeof(cwd)) == NULL)
      		perror("getcwd");
    	else
      		printf("Current Working Directory: %s\n", cwd);

   	}

   	else if(strcmp(parsed[1],"--help")==0)
   	{
   		printf("\t%s\n\t%s\n", "pwd: cd [-P/--help]" ,"print the name of the current working directory in Shruti Shell.");
		printf("\t%s\n", "Options:");
		printf("\t%s\n", "-P:   print the physical directory, without any symbolic links.");
		printf("\t%s\n", "--help:   print details of internal command 'pwd' on STDOUT.");
		printf("\t%s\n", "By default, 'pwd' behaves as if '-P' were specified.");
	}

	else if(strcmp(parsed[1],"-P")==0 && parsed[2]==NULL)
	{
		char cwd[1024]; 

    	if (getcwd(cwd, sizeof(cwd)) == NULL)
      		perror("getcwd");
    	else
      		printf("Current Working Directory: %s\n", cwd);
	}

	else
	{
		printf("%s\n","pwd: invalid option(s)");
	}
		
   return ; 
	
}

void internalHistory(char** parsed)
{
	
	if(parsed[1]==NULL)
	{

		int line_no=1;
		FILE* fptr= fopen(hist_file_path,"r");

		if(fptr==NULL)
			printf("%s\n", "history: cannot open history file");

		else
		{	
			char line[1024];
			while(fgets(line,sizeof(line),fptr))
				{
					printf("%d\t%s", line_no,line);
					line_no++;
				}
		}

		fclose(fptr);

	}

	else if(strcmp(parsed[1],"--help")==0)
	{
		printf("\t%s\n\t%s\n", "history: history [-c/--help] [n]" ,"Display or manipulate the history list in Shruti Shell.");
		printf("\t%s\n\t%s\n", "Display the history list with line numbers.","An argument of 'n' lists only the last n entries.");
		printf("\t%s\n", "Options:");
		printf("\t%s\n", "-c:   clear the entire history of the shell.");
		printf("\t%s\n", "--help:   print details of internal command 'history' on STDOUT.");
		printf("\t%s\n", "By default, 'history' prints the entire history list.");

	}

	else if(strcmp(parsed[1],"-c")==0 && parsed[2]==NULL)
	{
		clear_history();
		FILE* fptr= fopen(hist_file_path,"w");

		if(fptr!=NULL)
		{
			printf("All history cleared.\n");
		}

		fclose(fptr);
	}

	else if(atoi(parsed[1])!=0 && parsed[2]==NULL)
	{
		int max=atoi(parsed[1]);

 		FILE *fline= fopen(hist_file_path, "r"); 
    	int count = 0;  
    	char c;  
 
    	if (fline == NULL) 
    	{ 
        	printf("history: could not open history file\n");
        	return; 
    	} 
  
    	else
    	{
    		for (c = getc(fline); c != EOF; c = getc(fline)) 
        	if (c == '\n') 
            	count = count + 1; 
  		}
    	fclose(fline);

		FILE* fptr= fopen(hist_file_path,"r");

    	if(max>=count)
    	{
    		char line[1024];
    		int line_no=1;
			while(fgets(line,sizeof(line),fptr))
				{
					printf("%d\t%s", line_no,line);
					line_no++;
				}
    	}

    	else
    	{
    		char line[1024];
    		for(int i=0; i<count-max; i++)
    		{
    			fgets(line,sizeof(line),fptr);
    		}

    		int line_no=count-max+1;
    		while(fgets(line,sizeof(line),fptr))
				{
					printf("%d\t%s", line_no,line);
					line_no++;
				}

    	}
    	fclose(fptr);
 	   	
	}


	else
	{
		printf("history: invalid option(s)\n");
	}


    return ;

}

void internalEcho(char** parsed)
{
	char* start=parsed[1];

	if(start==NULL)
	{
		printf("\n");
	}

	else if(strcmp(start,"-n")==0)
	{
		for(int i=2;i<MAXLIST;i++)
		{
			char* word= parsed[i];
			if(word==NULL)
				break;
			else
				{
					if(word[0]!='"' && word[strlen(word)-1]!='"')
					{
						for(int j=0; j<strlen(word);j++)
						{
							if(word[j]!='\\')
							{
								putchar(word[j]);
							}
						}
						
					}
					else if(word[0]=='"' && word[strlen(word)-1]=='"')
					{
						for(int j=1; j<strlen(word)-1; j++)
						{
							putchar(word[j]);
						}
					}

					else if(word[0]=='"')
					{
						for(int j=1; j<strlen(word); j++)
						{
							putchar(word[j]);
						}

					}

					else if(word[strlen(word)-1]=='"')
					{
						for(int j=0; j<strlen(word)-1; j++)
						{
							putchar(word[j]);
						}
					}

					printf(" ");
					

				}

		}

		
	}


	else if(strcmp(start,"-E")==0)
	{
		for(int i=2;i<MAXLIST;i++)
		{
			char* word= parsed[i];
			if(word==NULL)
				break;
			else
				{
					if(word[0]!='"' && word[strlen(word)-1]!='"')
					{
						for(int j=0; j<strlen(word);j++)
						{
							if(word[j]!='\\')
							{
								putchar(word[j]);
							}
						}
						
					}
					else if(word[0]=='"' && word[strlen(word)-1]=='"')
					{
						for(int j=1; j<strlen(word)-1; j++)
						{
							putchar(word[j]);
						}
					}

					else if(word[0]=='"')
					{
						for(int j=1; j<strlen(word); j++)
						{
							putchar(word[j]);
						}

					}

					else if(word[strlen(word)-1]=='"')
					{
						for(int j=0; j<strlen(word)-1; j++)
						{
							putchar(word[j]);
						}
					}

					printf(" ");
					

				}

		}
		printf("\n");


	}

	else 
	{

		for(int i=1;i<MAXLIST;i++)
		{
			char* word= parsed[i];
			if(word==NULL)
				break;
			else
				{
					if(word[0]!='"' && word[strlen(word)-1]!='"')
					{
						for(int j=0; j<strlen(word);j++)
						{
							if(word[j]!='\\')
							{
								putchar(word[j]);
							}
						}
						
					}
					else if(word[0]=='"' && word[strlen(word)-1]=='"')
					{
						for(int j=1; j<strlen(word)-1; j++)
						{
							putchar(word[j]);
						}
					}

					else if(word[0]=='"')
					{
						for(int j=1; j<strlen(word); j++)
						{
							putchar(word[j]);
						}

					}

					else if(word[strlen(word)-1]=='"')
					{
						for(int j=0; j<strlen(word)-1; j++)
						{
							putchar(word[j]);
						}
					}

					printf(" ");
					

				}

		}
		printf("\n");

	}
	return;


}

void internalexit(char** parsed)
{
	if(parsed[1]==NULL)
		exit(0);


	else if(strcmp(parsed[1],"--help")==0)
	{
		printf("\t%s\n\t%s\n", "exit: exit [n]" ,"exit the Shruti Shell.");
		printf("\t%s\n", "Exit the shell with a status of n. If n is omitted, exit is made with status 0.");
		
	}

	else if(atoi(parsed[1])==0)
		exit(0);

	else
		exit(atoi(parsed[1]));

}

void externalLs(char** parsed)
{
	pid_t pid = fork(); //child forked

	if (pid == -1) { 
		printf("\nFailed forking child.."); //if error in forking
		return; 
	} 

	else if (pid == 0) { 
		if (execv(ls_path, parsed) < 0) {  //load the myls binary in the memory of child process

			printf("\nCould not execute command 'ls'..\n"); 
		} 
		exit(0); 
	} 
	else { 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 
}

void externalCat(char** parsed)
{
	pid_t pid = fork(); 

	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} 

	else if (pid == 0) { 
		if (execv(cat_path, parsed) < 0) { 
			printf("\nCould not execute command 'cat'..\n"); 
		} 
		exit(0); 
	} 
	else { 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 

}

void externalDate(char** parsed)
{

	pid_t pid = fork(); 

	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} 

	else if (pid == 0) { 
		if (execv(date_path, parsed) < 0) { 
			printf("\nCould not execute command 'date'..\n"); 
		} 
		exit(0); 
	} 
	else { 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 
}

void externalRm(char** parsed)
{

pid_t pid = fork(); 

	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} 

	else if (pid == 0) { 
		if (execv(rm_path, parsed) < 0) { 
			printf("\nCould not execute command 'rm'..\n"); 
		} 
		exit(0); 
	} 
	else { 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 
}

void externalMkdir(char** parsed)
{

	pid_t pid = fork(); 

	if (pid == -1) { 
		printf("\nFailed forking child.."); 
		return; 
	} 

	else if (pid == 0) { 
		if (execv(mkdir_path, parsed) < 0) { 
			printf("\nCould not execute command 'mkdir'..\n"); 
		} 
		exit(0); 
	} 
	else { 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 

}
// Function where the external command is executed 
int execArgs(char** parsed) 
{ 
	int NoOfExtCmds = 5;
	int selectedCmd = 0; 
	char* ListOfExtCmds[NoOfExtCmds]; 

	ListOfExtCmds[0] = "ls"; 
	ListOfExtCmds[1] = "cat"; 
	ListOfExtCmds[2] = "date"; 
	ListOfExtCmds[3] = "rm";
	ListOfExtCmds[4] = "mkdir"; 


	for (int i = 0; i < NoOfExtCmds; i++) { 
		if (strcmp(parsed[0], ListOfExtCmds[i]) == 0) { 
			selectedCmd = i + 1; 
			break; 
		} 
	} 

	switch (selectedCmd) { 
	case 1: 
			externalLs(parsed);
			return 1; 
		 
	case 2: 
			externalCat(parsed);
			return 1;
			 
		 
	case 3: 
			externalDate(parsed);
			return 1; 
	case 4: 
			externalRm(parsed);
			return 1; 

	case 5:
			externalMkdir(parsed); 
			return 1;

	default: 
			break; 
	} 

	return 0;
	
} 

// Function to execute internal commands 
int ownCmdHandler(char** parsed) 
{ 
	int NoOfOwnCmds = 5;
	int selectedCmd = 0; 
	char* ListOfOwnCmds[NoOfOwnCmds]; 

	ListOfOwnCmds[0] = "cd"; 
	ListOfOwnCmds[1] = "echo"; 
	ListOfOwnCmds[2] = "history"; 
	ListOfOwnCmds[3] = "pwd";
	ListOfOwnCmds[4] = "exit"; 


	for (int i = 0; i < NoOfOwnCmds; i++) { 
		if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) { 
			selectedCmd = i + 1; 
			break; 
		} 
	} 

	switch (selectedCmd) { 
	case 1: 
			internalcd(parsed);
			return 1; 
		 
	case 2: 
			internalEcho(parsed);
			return 1;
			 
		 
	case 3: 
			internalHistory(parsed); 
			return 1; 
	case 4: 
			internalpwd(parsed); 
			return 1; 

	case 5:
			printf("\nExiting Shruti Shell.\n"); 
			internalexit(parsed);
			return 1;

	default: 
			break; 
	} 

	return 0; 
} 



// function for parsing command words 
void parseSpace(char* str, char** parsed) 
{ 
	int i; 

	for (i = 0; i < MAXLIST; i++) { 
		parsed[i] = strsep(&str, " "); 

		if (parsed[i] == NULL) 
			break; 
		if (strlen(parsed[i]) == 0) 
			i--; 
	} 
} 

int processString(char* str, char** parsed) //function which calls parsing func and checks if command is internal or external
{ 
	parseSpace(str, parsed); 
	 
	if (ownCmdHandler(parsed)) 
		return 0; //if internal command return 0
	else
		return 1; //else return 1

} 

void printLine() //fucntion to print shell line
{
	char cwd[1024]; 

    getcwd(cwd, sizeof(cwd));
    
    printf("ShrutiShell@%s:\n", cwd);  	

    return ; 
}


int main() 
{ 

	char inputString[MAXLEN]; //input string
	char *parsedArgs[MAXLIST]; //array of parsed strings

	strcat(getcwd(ls_path, sizeof(ls_path)),"/myls"); //get path of files of external commands
	strcat(getcwd(cat_path, sizeof(cat_path)),"/mycat");
	strcat(getcwd(date_path, sizeof(date_path)),"/mydate");
	strcat(getcwd(rm_path, sizeof(rm_path)),"/myrm");
	strcat(getcwd(mkdir_path, sizeof(mkdir_path)),"/mymkdir");
	strcat(getcwd(hist_file_path, sizeof(hist_file_path)),"/historyfile.txt");
   	
	int cmdFlag = 0; //to check if command is internal or not
	int extFlag=0; //to check if command is external 
	intro_shell(); 

	while (1) { 
		// print shell line 
		printLine(); 
		// take input 
		if (takeInput(inputString)) //if input string is empty or null, print shell line again
			continue; 
		// process 
		cmdFlag = processString(inputString, 
		parsedArgs); 
		// cmdFlag returns zero if there is no command or it is internal command, 
		// 1 if it is external command 

		// execute 
		if (cmdFlag == 1) 
		{
			extFlag=execArgs(parsedArgs); //if command is external

			if(extFlag==0) //if command is not even external then it is not supported
			{
				printf("%s\n", "Shruti Shell: No such command exists");
			}

		}

	} 
	return 0; 
} 
