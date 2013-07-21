#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<errno.h>
#include<wait.h>
#include<pwd.h>

extern int errno;

int z=0,x=0,y=0;

char input[100],prompt[100],prompt1[50],history[500][100];

typedef struct entry{
	pid_t p;
	char name[100];
	int flag;
}process;
process p_all[100],p_cur[100];

void sig_handler(int signum){
	int pid,status;
	pid=waitpid(WAIT_ANY,&status,WNOHANG);
	if(pid > 0){
		int i=0,j,k;
		for(i=0;i<x;i++){
			if(p_cur[i].p==pid)
				break;
		}
		for(k=0;k<z;k++){
			if(p_all[k].p==pid)
				break;
		}
		if(status!=0){
			printf("\nUnknown command (press enter to comtinue)\n");
			for(j=k;j<z-1;j++){
				p_all[j].p=p_all[j+1].p;
				strcpy(p_all[j].name,p_all[j+1].name);
				p_all[j].flag=p_all[j+1].flag;
			}
			z--;
		}
		else
			printf("\n%s %d exited normally (press enter to comtinue)\n",p_cur[i].name,pid);
		for(j=i;j<x-1;j++){
			p_cur[j].p=p_cur[j+1].p;
			strcpy(p_cur[j].name,p_cur[j+1].name);
			p_cur[j].flag=p_cur[j+1].flag;
		}
		x--;
		return;
	}
	fflush(stdout);
	signal(SIGCHLD,sig_handler);
	return;
}


int main(void){


	signal(SIGCHLD,sig_handler);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);

	pid_t pid=getpid();


//	if(pid1==-1){
//		perror("Error in creating fork : ");
//		exit(-1);
//	}

//	else if(pid1==0){

		//	printf("Child %d\n",getpid());
		//	printf("Child %d\n",getppid());

		char *arg[100];

		prompt[0]='<';
		prompt[1]=0;

		struct passwd *pwd;
		char *name;
		pwd=getpwuid(getuid());
		if(pwd == NULL){
			perror("Username could not be set : " );
			exit(-1);
		}

		strcat(prompt,pwd->pw_name);

		strcat(prompt,"@");

		if(gethostname(prompt1,50)!=0){
			perror("Host name could not be set : ");
			exit(-1);
		}

		strcat(prompt,prompt1);

		strcat(prompt,":~");


		if(setenv("HOME",getcwd(NULL,100),1)!=0){
			perror("$HOME could not be changed : ");
			exit(-1);
		}
		
/*		input[0]=0;
		printf("%s%s>",prompt,strstr(getcwd(NULL,99),getenv("HOME"))+strlen(getenv("HOME")));
		scanf("%[^\n]",input);
		getchar();
		while(!strcmp(input,"")){
			input[0]=0;
			printf("%s%s>",prompt,strstr(getcwd(NULL,99),getenv("HOME"))+strlen(getenv("HOME")));
			scanf("%[^\n]",input);
			getchar();
		}
*/

		while(1){
			
			input[0]=0;
			printf("%s%s>",prompt,strstr(getcwd(NULL,99),getenv("HOME"))+strlen(getenv("HOME")));
			scanf("%[^\n]",input);
			getchar();
			while(!strcmp(input,"")){
				input[0]=0;
				printf("%s%s>",prompt,strstr(getcwd(NULL,99),getenv("HOME"))+strlen(getenv("HOME")));
				scanf("%[^\n]",input);
				getchar();
			}
			
			if(!strcmp(input,"quit"))
				break;
			
			strcpy(history[y++],input);			
			
			int i=0;
			char *res;
			res=strtok(input," ");
			while(res!=NULL){					
				arg[i++]=res;
			//	count++;
				res=strtok(NULL," ");
			}
			arg[i]=(char *)NULL;
			//if(count==0){
			//	input[0]=0;
			//	printf("%s%s>",prompt,strstr(getcwd(NULL,99),getenv("HOME"))+strlen(getenv("HOME")));
			//	scanf("%[^\n]%*c",input);
			//	continue;
			//}

			pid_t pid1=fork();

			if(pid1==-1){
				perror("Error in creating fork : ");
				exit(-1);
			}

			else if(pid1==0){


				//	printf("Child Child %d\n",getpid());
				//	printf("Child Child %d\n",getppid());

				//	printf("%s\n",strstr(getcwd(NULL,99),getenv("HOME")));
				//	printf("%d\n",strlen(getenv("HOME")));

				//env[0]=NULL;
				
				int i;
				for(i=0;arg[i+1]!=NULL;i++);
				if(arg[i][strlen(arg[i])-1]=='&')
						_exit(1);
				
				
				int ret=execvp(arg[0],arg);
				if (ret == -1)
					_exit(1);
			
			}

			else {
				//	printf("Child Parent %d\n",getpid());
				//	printf("Child PArent %d\n",getppid());
				int status;
				if(waitpid(pid1,&status,0)==-1){
					perror("waitpid()");
					exit(1);
				}

				//printf("status is %d\n",status);
				//	if(status1==-1)
				//		perror("waitpid()");

				if(status!=0){
					
					if(!strcmp(arg[0],"cd")){
						char *a=getcwd(NULL,99);
						strcat(a,"/");
						if(chdir(strcat(a,arg[1]))!=0){
							perror("chdir()");
							exit(1);
						}
						p_all[z].p=getpid();
						strcpy(p_all[z].name,arg[0]);
						p_all[z++].flag=1;
						if (strlen(getcwd(NULL,99)) < strlen(getenv("HOME")))
							chdir(getenv("HOME"));
					}

					else if(!strcmp(arg[0],"pid")&&arg[1]==NULL){
						printf("command name: ./a.out process id: %d\n",pid);
					}

					else if(!strcmp(arg[0],"pid")&&(!strcmp(arg[1],"current"))&&arg[2]==NULL){
						if(x>0){
							printf("List of currently executing processes spawned from this shell:\n");
							int i;
							for(i=0;i<x;i++)
								printf("command name: %s process id: %d\n",p_cur[i].name,p_cur[i].p);
						}
						else
							printf("\nNo processes running in background\n");
					}	

					else if(!strcmp(arg[0],"pid")&&(!strcmp(arg[1],"all"))&&arg[2]==NULL){
						if(z>0){
							printf("List of all processes spawned from this shell:\n");
							int i;
							for(i=0;i<z;i++)
								printf("command name: %s process id: %d\n",p_all[i].name,p_all[i].p);
						}
						else
							printf("\nNo process started yet");
					}	

					else if(!strcmp(arg[0],"hist")&&arg[1]==NULL){
						int i;
						for(i=0;i<y;i++){
							printf("%d. %s\n",i+1,history[i]);
						}
					}
					else if(arg[0][0]=='h'&& arg[0][1]=='i' && arg[0][2]=='s' && arg[0][3]=='t' && arg[1]==NULL){
						int q=atoi(arg[0]+4);
						if(q==0)
							printf("Unknown Command\n");
						else{
							int i,j;
							for(i=y-q,j=1;i<y;i++){
								if(i>=0 && i<y){
									printf("%d. %s\n",j,history[i]);
									j++;
								}
							}
						}
					}
					else if(arg[0][0]=='!' && arg[0][1]=='h'&& arg[0][2]=='i' && arg[0][3]=='s' && arg[0][4]=='t' && arg[1]==NULL){
						int q=atoi(arg[0]+5);
						if(q==0)
							printf("Unknown Command\n");
						else{
							if(q<y && q>0)
								printf("%s\n",history[q-1]);
							else
								printf("There is no %d entry in history\n");
						}
					}

				
				
				/*	else{
						int i;
						for(i=0;arg[i+1]!=NULL;i++);
						
						if(arg[i][strlen(arg[i])-1]=='&'){
							
				*/		
					else{
						int i;
						for(i=0;arg[i+1]!=NULL;i++);
						if(arg[i][strlen(arg[i])-1]=='&'){
							
							if(!strcmp(arg[i],"&"))
								arg[i]=NULL;
							else{
								int j;
								for(j=0;arg[i][j]!='&';j++);
								arg[i][j]=arg[i][j+1];
							}
							
							pid_t pid2=fork();
								
							if(pid2==0){
								int ret=execvp(arg[0],arg);
								if(ret==-1){
								//	printf("Unknown Command\n");
								//	p_cur[x-1].flag=0;
								//	p_all[z-1].flag=0;
									_exit(1);
								}
							}
							else if(pid2>0){
								printf("command %s pid %d\n",arg[0],pid2);
								p_all[z].p=pid2;
								strcpy(p_all[z].name,arg[0]);
								p_all[z++].flag=1;
								p_cur[x].p=pid2;
								strcpy(p_cur[x].name,arg[0]);
								p_cur[x++].flag=1;
							}

						}	
						else{
						printf("Unknown Command..\n");
						}
					}			
				}
			
				
				else{
					p_all[z].p=pid1;
					strcpy(p_all[z].name,arg[0]);
					p_all[z++].flag=1;
				}
			
/*			input[0]=0;
			printf("%s%s>",prompt,strstr(getcwd(NULL,99),getenv("HOME"))+strlen(getenv("HOME")));
//			fflush(stdout);
			scanf("%[^\n]",input);
			getchar();
			while(!strcmp(input,"")){
				input[0]=0;
				printf("%s%s>",prompt,strstr(getcwd(NULL,99),getenv("HOME"))+strlen(getenv("HOME")));
				scanf("%[^\n]",input);
				getchar();
			
			}
*/		}
	}
	int i;
	for(i=0;i<x;i++){
		kill(p_cur[i].p,SIGTERM);
	}
//	}
//	}
//	else {
		//	printf("PArent %d\n",getpid());
		//	printf("Parent %d\n",getppid());
//		int status;
//		waitpid(pid1,&status,0);
//	}
	return 0;
}


			






