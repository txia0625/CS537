#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int cd(char*);
void ErrMsg();
    int err;
    int commandNumber;  // # of command
    int commandCap;
    char** commandArray;   //command array

    int redirection; //0-> off 1 -> on
    
int main(int argc, char** argv) {
//############################ PRE-PROCESSING{  #################################
int mode = 0; // 0 -> interactive, 1 -> batch mode
    FILE* fp = NULL;
    //printf("\n%d\n",argc);
    if(argc == 2){
        mode = 1;
        fp = fopen(argv[1], "r");
        if(fp == NULL)
        {
            ErrMsg();
            exit(1);   //bad file. exit w/ code 1
        }
        if(mode == 1 && feof(fp)){
            fclose(fp);
            ErrMsg();
            exit(0);
        }
        

        //printf("\n%s\n","open good");

    }else if(argc > 2)
    {
        ErrMsg();
        exit(1); //multiple files not allowed, so exit code 1
    }
    //if(fp!= NULL)printf("\n%s\n","file good");

    char** path = malloc(sizeof(char**)); //defualt path
    path[0] = "/bin";
    int pathSize = 1;
    int lineNumber = 0;

//############################ }PRE-PROCESSING  #################################
while(1)
{
        if(mode == 1 && feof(fp)){
            fclose(fp);
            exit(0);
        }
     err = 0;
     commandNumber = 0;  // # of command
     commandCap = 32;
     commandArray = malloc(32 * sizeof(char**));   //command array

     redirection = 0; //0-> off 1 -> on
    

    
    
    if(mode == 0) printf("wish> ");

    //############################ HANDLING INPUT COMMAND LINE{ #################################
    char* line = NULL;  //current line
    int lineSize;   //current line size
    size_t bufferSize;

    if(mode == 0)lineSize = getline(&line, &bufferSize, stdin);
    else lineSize = getline(&line, &bufferSize, fp);
    lineNumber++;
    
    char* command = NULL;  //separate into commands and parameters
   
    
   //printf("\n command line: %s\n",line);
    
    char* outerPtr = NULL;
    char* innerPtr = NULL;

    char* cmdLine = NULL;
    char* redirFile = NULL;

    cmdLine = strtok(line, ">");
    redirFile = strtok(NULL, ">");
    if(redirFile != NULL) redirection = 1;
    if(strtok(NULL, ">") != NULL)
    {
        ErrMsg();
        continue;
    }




    char* buffer = cmdLine;

    
     
    
    while(cmdLine != NULL && (commandArray[commandNumber] = strtok_r(buffer, " ", &outerPtr)) != NULL)   //copy to the command array
    {
        buffer = commandArray[commandNumber];
        

        while( (commandArray[commandNumber] = strtok_r(buffer, "\t", &innerPtr)) != NULL)
        {
            

            commandNumber++;
            buffer = NULL;
            
        }

        
        
        buffer = NULL;
        
        if(commandNumber == commandCap - 5) //expand the array if it's almost fulfiled
        {
            commandArray = realloc(commandArray, commandCap * 2 * sizeof(char**));
            commandCap *= 2;
        }
    }

    if(redirection == 1 && strcmp(commandArray[0], "if") == 0)
    {
        redirection = 0;
        commandArray[commandNumber++] = ">";

    }



    buffer = redirFile;
    int fileCount = 0;


    outerPtr = NULL;
    innerPtr = NULL;

   while(redirFile != NULL && (commandArray[commandNumber] = strtok_r(buffer, " ", &outerPtr)) != NULL)   //copy to the command array
    {
        buffer = commandArray[commandNumber];
        

        while( (commandArray[commandNumber] = strtok_r(buffer, "\t", &innerPtr)) != NULL)
        {
            
            if(strcmp(commandArray[commandNumber], "\n") != 0) fileCount++;

            if(strcmp(commandArray[0], "if") !=0 && fileCount > 1)
            {
                ErrMsg();
                err = 1;
                break;
                
            }



            commandNumber++;
            buffer = NULL;
            
        }

        if(err) break;
        
        buffer = NULL;
        
        if(commandNumber == commandCap - 5) //expand the array if it's almost fulfiled
        {
            commandArray = realloc(commandArray, commandCap * 2 * sizeof(char**));
            commandCap *= 2;
        }
    }

    if(err)
    {
        continue;
    }

    

    if(redirection == 1 && fileCount == 0)
    {
        ErrMsg();
        continue;
    }

     if(commandNumber == 0) continue;

    
    //printf("command # : %d\n", commandNumber);
    commandArray[commandNumber-1] = strtok(commandArray[commandNumber-1], "\n");    //delete the nextline operator if it exists
    if(commandArray[commandNumber-1] == NULL) 
    {
        commandArray[commandNumber - 1] = '\0';
        commandNumber--;
    }
    else commandArray[commandNumber] = '\0';

  
    if(commandArray[0] == NULL) continue;

 
   
  /*
   for(int i = 0; i < commandNumber; i++)
    {
        printf("command %d: >%s<\n",i,(commandArray[i]));
    }
*/

    

    //############################ } HANDLING INPUT COMMAND LINE #################################



    //############################ BUILT IN-FUNCTION{  #################################
    
    //printf("here \n");
    if(strcmp(commandArray[0], "exit") == 0){
        if(commandNumber > 1)
        {
           // printf("EXIT ERROR\n");
            ErrMsg();
            continue;
        }
        
        exit(0);
    }else if(strcmp(commandArray[0], "cd") == 0){
        if(commandNumber == 1 || commandNumber > 2)
        {
           // printf("CD ERROR\n");
            ErrMsg();
            continue;
        }

        if(cd(commandArray[1]) == -1)
        {
           // printf("CD NO SUCH PATH\n");
            continue;
        }
    }else if(strcmp(commandArray[0], "path") == 0){

        path = malloc((commandNumber-1) * sizeof(char**));
        pathSize = commandNumber-1;

        for(int i = 1; i < commandNumber; i++)
        {
            path[i-1] = commandArray[i];
        }

    }else if(strcmp(commandArray[0], "if") == 0){
        //printf("if frame\n");
        int left = 0;
        int right = commandNumber-1;
        int errorSignal = 0;    
        while(right - left > 0)
        {
            int startId = left;
            int endId = right;
            int hasIfClause = 0;
            

            if((strcmp(commandArray[left], "if") != 0 && strcmp(commandArray[left], "then") != 0) || (strcmp(commandArray[right], "fi") != 0))
            {
                err = 1;
                ErrMsg();
                break;
            }

            
            if(strcmp(commandArray[startId], "if") == 0) hasIfClause = 1;
            else if(strcmp(commandArray[startId], "then") == 0) endId--;
            
            if(strcmp(commandArray[startId], "then") == 0 && startId == endId) break;

            startId++;
            int thenId = startId;

            if(strcmp(commandArray[startId], "if") == 0 && strcmp(commandArray[endId], "fi") == 0)
            {
                left = startId;
                right = endId;
                continue;
            }



           // printf("start: >%d<\n", startId);
           // printf("then: >%d<\n", thenId);
           // printf("end: >%d<\n", endId);
            while(hasIfClause == 1 && thenId <= endId && strcmp(commandArray[thenId], "then") != 0) thenId++;
            if(hasIfClause == 1 && (thenId > endId ||strcmp(commandArray[thenId], "then") != 0))
            {
                err = 1;
                ErrMsg();
                break;
            }
            

            if(hasIfClause == 1 && startId == thenId)
            {
              // printf("EMPTY IF CONDITION 1st arg\n");
                ErrMsg();
                err = 1;
                break;
            }

            if(hasIfClause == 1 &&  thenId == endId)
            {
             //  printf("EMPTY ELSE CONDITION \n");
                ErrMsg();
                err = 1;
                break;
            }
            
            int operatorId = startId;

            while(hasIfClause == 1 && operatorId < thenId && strcmp(commandArray[operatorId], "!=") != 0 && strcmp(commandArray[operatorId], "==") != 0) 
            {
                //printf("operator iteration: >%s<\n", commandArray[operatorId]);
                operatorId++;
                //debug here
            }

            if(hasIfClause == 1 && strcmp(commandArray[operatorId], "!=") != 0 && strcmp(commandArray[operatorId], "==") != 0)
            {
                ErrMsg();
                err = 1;
                break;
            }

            int equal = 1;
            if(hasIfClause == 1 && strcmp(commandArray[operatorId], "!=") == 0) equal = 0;
            //app test

        if(hasIfClause == 0) operatorId = endId + 1;

        int found = 0;
        char* paramList[operatorId - startId];
        int returnVal;

        int localRedirection = 0;
       //if(redirection) commandNumber -= 2;
        char* localRedFile;
        for(int i = startId; i < operatorId ; i++)
        {
            paramList[i - startId] = strdup(commandArray[i]);
            if(strcmp(paramList[i-startId], ">") == 0 )
            {
                localRedirection = 1;
                localRedFile = strdup(commandArray[i+1]);
                paramList[i-startId] = NULL;
                break;
            }

          // printf("paramList #%d: >%s<\n", i-startId, paramList[i - startId]);
        }

        
            

            

        

        paramList[operatorId - startId] = NULL;
        if(strcmp(paramList[0], "cd") == 0){
            if(operatorId - startId  == 2)
            {
               // printf("cd arg # correct\n");
               // printf("cd param : >%s<\n", paramList[1]);
                if(cd(paramList[1]) == -1)
                {
                    ErrMsg();
                    err = 1;
                    break;
                }
            }else{
                //printf("cd error: too many arguments \n");
                ErrMsg();
                err = 1;
                break;
            }

        }else{
        
        for(int i = 0 ; i < pathSize; i++)
        {
            char* tempPath = strdup(path[i]);
            int pathlen = strlen(tempPath);
            if(tempPath[pathlen-1] != '/') tempPath = strcat(tempPath, "/");
            tempPath = strcat(tempPath, paramList[0]);

            
            //printf("tempPath %d: >%s<\n", i,tempPath);

                if(access(tempPath, X_OK) == 0)
                {
                    
                    found = 1;
                    int pid = fork();
                    if(pid){
                        int status;
                        waitpid(pid, &status, 0);
                       if(WIFEXITED(status))
                       {
                        returnVal = WEXITSTATUS(status);
                        //printf("status: %d\n", status);
                       }
                       else{
                           // printf("FILE EXECUTION ERROR\n");
                           // ErrMsg();
                            found = 0;
                            break;
                       }
                    }else{
                        if(localRedirection){
                            //printf("in the process...\n");
                            int fd = open(localRedFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            
                            dup2(fd, fileno(stdout));
                            close(fd);
                        }

                        execv(tempPath, paramList);
                        return 0;
                    }
                    
                }

                if(found) break;
        }


        if(found == 0)
        {
           // printf("NO SUCH EXECUTION FILE\n");
            ErrMsg();
            errorSignal = 1;
            break;
        }

        }
        if(hasIfClause == 0) break;

        char* p2 = commandArray[operatorId + 1];
        int p2Length = strlen(p2);
        int p2Val = 0;
        

        for(int i = 0; i < p2Length; i++)
        {
            
            p2Val *= 10;
            if(p2[i] > '9' || p2[i] < '0')
            {
               // printf("NOT A NUMBER\n");
                ErrMsg();
                errorSignal = 1;
                break;
            }
            p2Val += p2[i]-'0';
        }
        
        if(errorSignal) break;
        
        if(equal == 1 && p2Val == returnVal){
            //printf("if condition satisfied \n");
            left = thenId;
            
        }else if(equal == 0 && p2Val != returnVal){
           // printf("if condition satisfied \n");
            left = thenId;
            
        }else break;            


    }

        if(errorSignal || err) continue;
    }else{  //normal app

        //printf("in the app branch...\n");
        int found = 0;
        char* paramList[commandNumber];
        int returnVal;

        if(redirection) commandNumber -= 1;

        for(int i = 0; i < commandNumber ; i++)
        {
            paramList[i] = strdup(commandArray[i]);
            //printf("paramList %d: >%s<\n", i,commandArray[i]);
        }

        paramList[commandNumber] = NULL;
        
        
        for(int i = 0 ; i < pathSize; i++)
        {
            char* tempPath = strdup(path[i]);
            tempPath = strcat(tempPath, "/");
            tempPath = strcat(tempPath, commandArray[0]);
            

            //printf("tempPath %d: >%s<\n", i,tempPath);

                if(access(tempPath, X_OK) == 0)
                {
                    
                    found = 1;
                    int pid = fork();
                    if(pid){
                        int status;
                        waitpid(pid, &status, 0);
                       if(WIFEXITED(status)) returnVal = WEXITSTATUS(status);
                       else{
                           // printf("FILE EXECUTION ERROR\n");
                            //ErrMsg();
                            found = 0;
                            break;
                       }
                    }else{
                        if(redirection){
                           // printf("in the process...\n");
                            int fd = open(commandArray[commandNumber], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            
                            dup2(fd, fileno(stdout));
                            close(fd);
                        }

                        execv(tempPath, paramList);
                        return 0;
                    }
                    
                }

                if(found) break;
        }

        if(found == 0)
        {
           // printf("CAN'T FIND THE FILE\n");
            ErrMsg();
            continue;
        }
    }

    //############################ }BUILT IN-FUNCTION  #################################

    //free(commandArray); //release arguments array
    
    //printf("tail here\n");
        if(mode == 1 && feof(fp)){
            fclose(fp);
            exit(0);
        }

}

free(path);

}


void ErrMsg()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

int cd(char* dir)
{


        if(chdir(dir) == -1)
        {
           // printf("CD NO SUCH PATH\n");
            ErrMsg();
            return -1;
        }

        return 0;    
}





