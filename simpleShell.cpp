#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

using namespace std;

int parse_argv(stringstream &in_command, char** &argv);
void sig_handle(int sig);

int main()
{
    
    while(true){
        string input, output;
        stringstream ss;
        
        char **myArgv;
        int myArgc;
        bool back_ground = false;
        cout << ">";
        
        getline(cin, input);
        ss << input;
        
        myArgc = parse_argv(ss, myArgv);    //get argument!
        if(strcmp(myArgv[myArgc-1], "&") == 0){        //check if & exit
            back_ground = true;
            myArgv[myArgc-1] = NULL;
        }
        pid_t pid = fork();


        if(pid < 0){
            cout << "Fork process error!" << endl;
        }
        else if(pid == 0){ //child process
            execvp(myArgv[0], myArgv);
            
            exit(0);
        }
        else{    //parent process
            if(back_ground == false)
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

int parse_argv(stringstream &in_command, char** &argv){
    vector<string> tmp;
    string tmp_s;

    tmp.reserve(10);

    while(in_command >> tmp_s){
        tmp.push_back(tmp_s);
    }

    argv = new char* [tmp.size()+1];        //dynamic allocate true arugments array
    for(int i=0; i<tmp.size(); i++){
        argv[i] = new char[strlen(tmp[i].c_str())];
        strcpy(argv[i], tmp[i].c_str());
    }
    argv[tmp.size()] = NULL;                //argv should terminated by NULL
    return tmp.size();
}

void sig_handle(int sig){
    int status;
    while(waitpid(-1, &status, WNOHANG) > 0);
}