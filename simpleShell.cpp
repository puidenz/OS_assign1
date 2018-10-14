#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

using namespace std;

int parse_argv(vector<string> &in_command, char** &argv);
void sig_handle(int sig);

int main()
{
    
    while(true){
        string input;       //store the input line
        stringstream ss;    //stringstream for string handling
        
        char **myArgv;      //arguments array
        int myArgc;         //number of arguments
        string filename;	//store filename for redirection
		bool back_ground = false;          				 //flag for idicatiing if back_ground mode "&"
        bool redirection = false;
		

		cout << ">";
		getline(cin, input);
        ss << input;
        
        vector<string> tmp;
        string tmp_s;
        tmp.reserve(10);

        while(ss >> tmp_s){
			if(tmp_s == ">"){
				redirection = true;
				ss >> filename;
			}
            else
                tmp.push_back(tmp_s);
        }

        myArgc = parse_argv(tmp, myArgv);    			//get argument!
        if(strcmp(myArgv[myArgc-1], "&") == 0){			//check if & exit
            back_ground = true;
            myArgv[myArgc-1] = NULL;
        }
        pid_t pid = fork();


        if(pid < 0)
            cout << "Fork process error!" << endl;
        
        else if(pid == 0){ 					 //child process
            int fd = 0;
            
            if(redirection == true){
                fd = open(filename.c_str(), O_RDWR);

                if(fd < 0){                 //check if correctly open file
                    cout << "open file error!" << endl;
                    exit(-1);
                }
                
                close(1);
                dup2(fd, 1);
                close(fd);
            }
            

            execvp(myArgv[0], myArgv);
            
            exit(0);
        }
		
        else{								 //parent process
            if(back_ground == false)         //check is necessary to wait child
                wait(&pid);
            else
                signal(SIGCHLD, sig_handle);
            
                                             //Free memory!!
            for(int i=0; i<=myArgc; i++)     //size of myArgv is myArgc+1
                delete[] myArgv[i];
            delete[] myArgv;
        }
    }
    return 0;
}

int parse_argv(vector<string> &in_command, char** &argv){
    argv = new char* [in_command.size()+1];        //dynamic allocate true arugments array
	for(int i=0; i<in_command.size(); i++){
        argv[i] = new char[strlen(in_command[i].c_str())];
        strcpy(argv[i], in_command[i].c_str());
    }

    argv[in_command.size()] = NULL;                //argv should terminated by NULL
    return in_command.size();
}

void sig_handle(int sig){
    int status;
    while(waitpid(-1, &status, WNOHANG) > 0);
}