// Author : Abdullah Bukhari
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>

#define MAX_LEN 512
#define MAXARGS 10  
#define ARGLEN 30
#define MAXLIST 100
// add infinite piping and history feature 
char *my_itoa(int num, char *str)
{
        if(str == NULL)
        {
                return NULL;
        }
        sprintf(str, "%d", num);
        return str;
}
char* read_cmd(FILE* fp){
  int c; //input character
  int pos = 0; //position of character in cmdline
  char* cmdline = (char*) malloc(sizeof(char)*MAX_LEN);
  while((c = getc(fp)) != EOF){
      if(c == '\n')
         break;
      cmdline[pos++] = c;       
  }
  cmdline[pos] = '\0';
  return cmdline;
}

char** tokenize(char* cmdline){
  char** arglist = (char**)malloc(sizeof(char*)* (MAXARGS+1));
  for(int i=0; i < MAXARGS+1; i++){
	   arglist[i] = (char*)malloc(sizeof(char)* ARGLEN);
      bzero(arglist[i],ARGLEN);
  }
  char* cp = cmdline; // pos in string
  char* start;
  int len;
  int argnum = 0; //slots used
  while(*cp != '\0'){
      while(*cp == ' ' || *cp == '\t') //skip leading spaces
          cp++;
      start = cp; //start of the word
      len = 1;   //initialize length of the word to 1
      //find the end of the word
      while(*++cp != '\0' && !(*cp ==' ' || *cp == '\t'))
              len++;
      strncpy(arglist[argnum], start, len);
      arglist[argnum][len] = '\0';
      argnum++;
  }
  arglist[argnum] = NULL;
  return arglist;
}      

int execute(char* arglist[]){
   int status;
   int cpid = fork();
   switch(cpid){
      case -1:
         perror("fork failed");
	      exit(1);
      case 0:
	      execvp(arglist[0], arglist);
 	      perror("Command not found...");
	      return -1;
      default:
	      waitpid(cpid, &status, 0);
        //  printf("child exited with status %d \n", status >> 8);
         return 0;
   }
}
int takeInput(char* str)
{
    char* buf;
  
    buf = readline("");
    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}
char *command_names[] = {
    "ls",
    "touch",
    "rm",
    "cd",
    "users",
    "whoami",
    "man",
    "ls.c",
    "exit_status.txt",
    "output2.txt",
    "a.out",
    "sample_file.txt",
    NULL
};
char **command_name_completion(const char *, int, int);
char *command_name_generator(const char *, int);

char **command_name_completion(const char *text, int start, int end)
{
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, command_name_generator);
}

char *command_name_generator(const char *text, int state)
{
    static int list_index, len;
    char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = command_names[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

void execute_exit_cmd(int shell_pid, char ch)
{       
    char** arglist;
    char pid[15] = "";
    int fd6[2];
    int frk7;
    char some_char[3] = "";
    char kill_statement[200] = "";
    char number[20] = "";
    char data_pid[200] = "";
    int number_counter = 0;

    pipe(fd6);
    frk7 = fork();


    if (frk7 == 0) // child
    {
        dup2(fd6[0], 0);
        close(fd6[1]);
        read(fd6[0], data_pid, 500);

        strcpy(some_char, "");
        strcpy(number, "");
        strcpy(kill_statement, "");
        strcat(kill_statement, "kill -9 ");

        my_itoa(shell_pid, pid);
        // dup2(stdout_copy, 1);

        for (int pindex = 0; pindex < strlen(data_pid); pindex++)
        {
            if (data_pid[pindex] >= 48 && data_pid[pindex] <= 57)
            {
                some_char[0] = data_pid[pindex];
                strcat(number, some_char);
            }
            else
            {
            if (strcmp(number, "") != 0 && strcmp(pid, number) != 0)
            {
                strcat(kill_statement, " ");
                strcat(kill_statement, number);
            }
            strcpy(number, "");
            }
        }
        strcat(kill_statement, " ");
        strcat(kill_statement, pid);

        // printf("%s\n", data_pid);
        // printf("kill statement was : %s\n", kill_statement);
        
        if (ch == 'e')
        {
            if((arglist = tokenize(kill_statement)) != NULL)
            { 
                execute(arglist); 
            }
        }

        else if (ch == 'j')
        {
            strcpy(number, "");
            // [31]+  Stopped                 ./a.out
            for (int counter = 0; counter < strlen(kill_statement); counter++)
            {
                if ((char)kill_statement[counter] >= 48 && (char)kill_statement[counter] <= 57)
                {
                    some_char[0] = kill_statement[counter];
                    strcat(number, some_char);
                }

                else
                {
                    if (strcmp(number, "") != 0)
                    { printf("[%d]+ Stopped          ./a.out \n", number_counter); number_counter++;}
             
                    strcpy(number, "");

                    
                }
            }

            printf("[%d]+ Running          ./a.out \n", number_counter);
        }

        exit(0);

    }

    if (frk7 > 0)
    {
        dup2(fd6[1], 1);
        close(fd6[0]);
        my_itoa(shell_pid, pid);

        execlp("pstree", "pstree", "-p", pid, NULL);

    }
    return;
}

int main() 
{
    

    rl_attempted_completion_function = command_name_completion;
    rl_attempted_completion_over  = 1;
    // close(0);
    // int fd = open("out.txt", O_RDONLY);
    int shell_pid = getpid();
    int filed;
    int status;
    char pid[100];
    char some_char[4] = ""; 

    char kill_statement[300] = "";

    if (access("output.txt", F_OK) != 0 && access("output2.txt", F_OK) != 0 && 
        access("exit_status.txt", F_OK) != 0)
    { system("touch output.txt output2.txt exit_status.txt"); }


     
    FILE *fptr = fopen("output.txt", "r+");
    
    FILE *fptr2 = fopen("output2.txt", "r+");

    FILE *fptr3 = fopen("exit_status.txt", "r+");
    FILE *fptr4;
    // system("false; echo $? 1> exit_status.txt");
    char *buffer = "";
    char c = ' '; char ch;
    int line_num = 0;
    int num_commands = 0;
    char cmdline[100] = "";
    char cmdline2[100] = "";
    char sing_char[5] = "";
    char** arglist; 
    int frk = 0;
    int frk2 = 0;
    int status2 = 0;
    int has_and = 0; // tells us if cmd has &, 0 means no, 1 means yes
    int has_pipe = 0;
    char number[50] = "";
    char cd_path[100] = "";
    char data[10000] = "";
    
    char cmd_after_then[100] = "";
    char cmd_after_else[100] = "";
    
    char cmd_bef_pipe_1[100] = "";
    char cmd_af_pipe_1[100] = "";
    char cmd_af_pipe_2[100] = "";
    char data_pid[500] = "";
    char create_file_cmd[100] = "";
    
    int stdin_copy = dup(0);
    int stdout_copy = dup(1);
    
    int has_then = 0;
    int has_if = 0; // tells if command was an if else, 0 means no, 1 means yes
    int exit_status = -1;
    int has_fi = 0; // fi means end of if else block
    
    char red_in_cmd[100] = "";
    
    int num_pipes = 0;
    
    int pipe_counter = 0;
    int num_newline_chars = 0;
    

    int index_then = 0;
    int index_after_then = 0;
    
    int has_else = 0;
    int index_old_variable = 0;
    char variables[100] = "";
    char values[100] = "";
    int num_variables = 0;
    int is_locally_equal = 0;
    int was_old_variable = 0;
    int has_exit = 0;
    int fd6[2];
    int fd[2]; // fd[1] is write, fd[0] is read end
    pipe(fd);
    
    int fd2[2];
    pipe(fd2);
    
    int fd3[2];
    pipe(fd3);
    
    int fd4[2];
    pipe(fd4);
    
    int frk3, frk4;
    int is_out = 0; int is_in = 0;
    
    char redirect_in[100] = "";
    char redirect_out[100] = "";
    
    char cmd_number[100] = "";
    int has_redirection = 0;
    int index = 0;
    
    char redirect_output[1000] = "";
    char cmd_bef_redirect_out[1000] = "";
    char redirect_program[15] = "";
    int space_num = 0;
    
    int frk5, frk6;
    int file_out_desc;
    int has_equal = 0;

    int has_dollar = 0;
    int has_exclam = 0;
    int filedesc;

    char num_cmd[3] = "";
    int has_cd = 0;
    char current_path[100] = "";
    char var1 = ' ';
    int frk7;
    int fd7;
    int has_help = 0;
    int help_counter = 0;
    char help_cmd[50] = "";
    int has_jobs = 0;
    int has_echo = 0;
    printf("\033[7m");
    printf("\n\t\t***Welcome to my Custom Shell***\n\n");
    printf("\033[0m");
    while(1)
    {
        printf("\033[0;31m");
        printf("Enter the command : ");
        printf("\033[0m");
        

        if ( access("output.txt", F_OK) == 0 && access("output2.txt", F_OK) == 0 )
         { remove("output.txt"); remove("output2.txt");}
        
        if (access("output.txt", F_OK) != 0 && access("output2.txt", F_OK) != 0)
         { system("touch output.txt output2.txt"); }
        
        
        fptr = fopen("output.txt", "r+");
        
        fptr2 = fopen("output2.txt", "r+");
        dup2(stdin_copy, 0);
        
        while( (ch = getchar()) != '\n' ) 
        {  
            
            if (ch == ';')
            { putc('\n', fptr); num_commands++; }
            
            else
            { putc(ch, fptr); }
            
        
        }
        
        num_commands++;  // num_commands is count of semi_colons + 1
    
      // code above writes commands to output.txt
        
      // don't forget to readjust file ptr
        fclose(fptr);
        fclose(fptr2);

        
        // printf("num commands is : %d\n", num_commands);
        close(0);
                
        fptr = fopen("output.txt", "r+");
        fptr2 = fopen("output2.txt", "r+");
        
        for (int counter = 0; counter < num_commands; counter++)
        {
            frk = fork();
            line_num = 0;
            has_and = 0;
            has_pipe = 0;
            num_pipes = 0;
            has_redirection = 0;
            has_dollar = 0;
            has_exclam = 0;
            has_cd = 0;
            has_equal = 0;
            has_exit = 0;
            has_help = 0;
            help_counter = 0;
            has_jobs = 0;
            has_echo = 0;
            // printf("Fork value was %d \n", frk); 
            if (frk == 0) // child
            {
                // printf("child\n");
                strcpy(sing_char, "");
                strcpy(cmdline, "");
                
                strcpy(help_cmd, "");
                buffer = readline("");

                if (strlen(buffer) != 0)
                { add_history(buffer); }
                // takeInput(buffer);
           

                index = 0;
                
                while ( index < strlen(buffer) && (( ch = buffer[index] ) != '\n' && (ch != EOF)) )
                {

                    if (ch == 'h' )
                    {                 
                       for (int hindex = index; hindex < strlen(buffer); hindex++)
                       {
                        
                        if (buffer[hindex] != ' ')
                        {
                            sing_char[0] = buffer[hindex];
                            strcat(help_cmd, sing_char);
                        }

                        if (strcmp(help_cmd, "help") == 0 && help_counter == 0)
                        {   
                            help_counter++;
                            has_help = 1;

                            strcpy(help_cmd, "");
                        }
                       }
                
                    }

                    if (ch == 'e' && ( (index + 3) < strlen(buffer)))
                    {                 
                        if (buffer[index + 1] == 'x' && buffer[index + 2] == 'i'
                            && buffer[index + 3] == 't')
                            { has_exit = 1; }

                        else if (buffer[index + 1] == 'c' && buffer[index + 2] == 'h'
                            && buffer[index + 3] == 'o')
                            { has_echo = 1; }
                
                    }

                    if (ch == 'j' && ( (index + 3) < strlen(buffer)))
                    {                 
                        if (buffer[index + 1] == 'o' && buffer[index + 2] == 'b'
                            && buffer[index + 3] == 's')
                            { has_jobs = 1; }
                
                    }

                    if (ch == '=')
                    { 
                        has_equal = 1;
                    }
                    if (ch == '<' || ch == '>')
                    {
                        has_redirection = 1;
                    }
                    if (ch == '!')
                    { has_exclam = 1;}
                    
                    if (ch == '|')
                        num_pipes++;
                    
                    if (ch == '|' || has_pipe == 1)
                    {
                        has_pipe = 1;
                        sing_char[0] = ch;
                        strcat(cmdline, sing_char);
                    }
                    
                    if (ch == '&' && has_pipe == 0)
                    {
                        has_and = 1;
                    }
                    
                    if (has_and != 1 && has_pipe == 0)
                    {
                        sing_char[0] = ch;
                        strcat(cmdline, sing_char);
                    }
                    
                    if (strcmp(cmdline, "if") == 0 && has_pipe == 0)
                    {
                        strcpy(cmdline, "");
                        has_if = 1;
                    }

                    if (strcmp(cmdline, "cd") == 0 && has_pipe == 0)
                    {
                        strcpy(cmdline, "");
                        has_cd = 1;
                    }
                    
                     if (strcmp(cmdline, "then") == 0 && has_pipe == 0)
                    {
                        strcpy(cmdline, "");
                        has_then = 1;
                    }
                    
                     if (strcmp(cmdline, "else") == 0 && has_pipe == 0)
                    {
                        strcpy(cmdline, "");
                        has_else = 1;
                    }
                    
                    if (strcmp(cmdline, "fi") == 0 && has_pipe == 0)
                    {
                        strcpy(cmdline, "");
                        has_fi = 1;
                    }
                    
                    if (ch == '$')
                    {
                        has_dollar = 1;
                    }
                    index++; 
     
                }
                
                if (has_jobs == 1 && has_help == 0)
                {
                //   printf("has jobs was 1\n");
                  execute_exit_cmd(shell_pid, 'j');
                }

                else if (has_help == 1)
                {

                    if (strcmp(help_cmd, "help") == 0)
                    printf("Displays information about shell builtin commands\n");

                    else if (strcmp(help_cmd, "exit") == 0)
                    printf("Kills the shell and all of its descendants too\n");
                    
                    else if (strcmp(help_cmd, "jobs") == 0)
                    printf("Displays the status of jobs\n");

                    else if (strcmp(help_cmd, "cd") == 0)
                    printf("Changes the shell current working directory\n");
                    exit(0);
                }

                else if (has_exit == 1)
                {
                    // printf("has_exit was 1\n");

                    // printf("shell pid was %d\n", shell_pid);
                    execute_exit_cmd(shell_pid, 'e'); 
                    
                }
                else if (has_equal == 1) // shell variable part
                {
                    is_locally_equal = 0;
                    was_old_variable = 0;
                    // printf("has_equal command was : %s\n", cmdline);
                    
                    // check if variable was an old variable

                    for (int counter = 0; counter < strlen(cmdline); counter++)
                    {
                        if (cmdline[counter] == '=')
                        { is_locally_equal = 1; }

                        if (is_locally_equal == 0)
                        {
                           // assume variable is single char

                           for (int index = 0; index < num_variables; index++)
                           {
                            if (variables[index] == cmdline[counter])
                            { was_old_variable = 1; index_old_variable = counter;}

                           }
                           
                           if (was_old_variable == 0)
                           { variables[num_variables] = cmdline[counter]; }       
                        }

                        if (is_locally_equal == 1 && cmdline[counter] != '=' && was_old_variable == 0)
                        {   
                                values[num_variables] = cmdline[counter];
                                num_variables++;
                        }

                        if (is_locally_equal == 1 && cmdline[counter] != '=' && was_old_variable == 1)
                        {   
                           values[index_old_variable] = cmdline[counter];
                        }
                    }

                    if (was_old_variable == 0)
                    {
                        // printf("variable and value was : %c %c\n", variables[num_variables - 1],
                        // values[num_variables - 1]);
                    }

                    else if (was_old_variable == 1)
                    {
                        // printf("variable and value was : %c %c\n", variables[index_old_variable],
                        // values[index_old_variable]);
                    }

                }
                else if (has_dollar == 1 && has_echo == 1)
                {

                    // printf("has dollar and has echo was 1\n");
                    // printf("num variables is :%d\n", num_variables);
                    
                    for (int i = 0; i < strlen(cmdline); i++)
                    {
                        if (cmdline[i] == '$')
                        var1 = cmdline[i + 1];
                    }
                    // printf("var1 was : %c\n", var1);

                    for (int index = 0; index < num_variables; index++)
                    {
                        if (variables[index] == var1)
                        { printf("%c\n", values[index]); }

                    }
                }
                else if (has_cd == 1)
                {
                    // printf("cd cmd path was : %s\n", cmdline);
                    strcpy(current_path, "");
                    strcpy(cd_path, "");

                    for (int i = 0, num_chars = 0; i < strlen(cmdline); i++)
                    {
                        if (cmdline[i] != ' ' && cmdline[i] != '\n')
                        {
                           cd_path[num_chars] = cmdline[i];
                           num_chars++;
                        }
                    }
                    chdir(cd_path);
                    printf("current working directory is : %s\n", getcwd(current_path, 100));

                }
                else if (has_exclam == 1)
                {
                    // printf("history command was : %s\n", cmdline);
                    strcpy(cmd_number, "");

                    if (cmdline[1] != '-')
                    {   
                        strcpy(cmd_number, "history|tail -");
                        num_cmd[0] = cmdline[1];
                        strcat(cmd_number, num_cmd);
                        // printf("history cmd is :%s\n", cmd_number);
                        system(cmd_number);
                    }
                    
                }
                else if (has_redirection == 1)
                {
                    // printf("redirect command was : %s\n", cmdline);
                    strcpy(cmdline2, "");
                    
                    is_out = 0;
                    is_in = 0;
                    strcpy(redirect_in, "");
                    strcpy(redirect_out, "");
                    
                    space_num = 0;
                    strcpy(redirect_program, "");
                    strcpy(cmd_bef_redirect_out, "");
                    
                    for (int counter = 0; counter < strlen(cmdline); counter++)
                    {
                        if (cmdline[counter] == ' ')
                        {
                            space_num++;
                        }
                        
                        if (space_num == 0 && cmdline[counter] != ' ')
                        {
                            sing_char[0] = cmdline[counter];
                            strcat(redirect_program, sing_char);
                        }
                        
                        if (cmdline[counter] == '>')
                        { is_out = 1; }
                        
                        if (cmdline[counter] == '<')
                        { is_in = 1; }
                        
                        if (is_out != 1)
                        { sing_char[0] = cmdline[counter]; strcat(cmd_bef_redirect_out, sing_char); }
                        
                        if (is_out == 1 && cmdline[counter] != '>')
                        {
                            sing_char[0] = cmdline[counter];
                            
                            if (strcmp(redirect_out, "") != 0 && cmdline[counter] == ' ')
                            {is_out = 0;}
                            
                            if (cmdline[counter] != ' ' && is_out == 1)
                            {
                                strcat(redirect_out, sing_char);
                            }
                        }
                        
                         if (is_in == 1 && cmdline[counter] != '<')
                        {
                            sing_char[0] = cmdline[counter];
                            
                            if (strcmp(redirect_in, "") != 0 && cmdline[counter] == ' ')
                            {is_in = 0;}
                            
                            if (cmdline[counter] != ' ' && is_in == 1)
                            {
                                strcat(redirect_in, sing_char);
                            }
                        }             
                    }
                    
                    if (strcmp(redirect_in, "") != 0 && strcmp(redirect_out, "") == 0)
                    {
                      frk5 = fork();
                       
                      if (frk5 == 0) // child
                      {
                        dup2(fd3[0], 0);
                        close(fd3[1]);
                        if((arglist = tokenize(redirect_program)) != NULL)
                        { 
                            execute(arglist); 
                        }
                        // execlp(redirect_program, redirect_program, NULL);
                        exit(0);
                       
                      }
                      else if (frk5 > 0) // parent
                      {
                        dup2(fd3[1], 1);
                        close(fd3[0]);
                        execlp("cat", "cat", redirect_in, NULL);
                        exit(0);
                      }
                      
                   
                    }
                    
                    else if (strcmp(redirect_out, "") != 0 && strcmp(redirect_in, "") == 0)
                    {
                         
                    if (access(redirect_out, F_OK) == 0)
                    { remove(redirect_out); }

                     strcpy(create_file_cmd, "");
                     
                     strcat(create_file_cmd, "touch"); strcat(create_file_cmd, " ");
                     strcat(create_file_cmd, redirect_out);
                     
                     system(create_file_cmd);
                     
                     frk5 = fork();
                       
                    
                      if (frk5 > 0) // parent
                      {
                
                        file_out_desc = open(redirect_out, O_RDWR);
                        
                        dup2(file_out_desc, 1);
                        
                        if((arglist = tokenize(cmd_bef_redirect_out)) != NULL)
                        { 
                            execute(arglist); 
                        }
                        exit(0);
                        
                      }

                      if (frk5 == 0)
                      { exit(0); }
                      
                    }
                    
                    else if (strcmp(redirect_out, "") != 0 && strcmp(redirect_in, "") != 0)
                    {
      
                      if (access(redirect_out, F_OK) == 0)
                      { remove(redirect_out); }

                      strcpy(create_file_cmd, "");
                    
                      strcat(create_file_cmd, "touch"); strcat(create_file_cmd, " ");
                      strcat(create_file_cmd, redirect_out);
                      system(create_file_cmd);
                      
                      pipe(fd3);
                      frk5 = fork();
                       
                      if (frk5 == 0) // child
                      {
                        dup2(fd3[0], 0);
                        close(fd3[1]);
                        close(fd3[0]);
                                               
                        file_out_desc = open(redirect_out, O_RDWR);
                        
                        dup2(file_out_desc, 1);
                        
                        if((arglist = tokenize(redirect_program)) != NULL)
                        { 
                            execute(arglist); 
                        }
                        // execlp(redirect_program, redirect_program, NULL);

                        exit(0);
                       
                      }
                      else if (frk5 > 0) // parent
                      {
                        dup2(fd3[1], 1);
                        close(fd3[0]);
                        close(fd3[1]);
                        execlp("cat", "cat", redirect_in, NULL);
                        exit(0);
                      }
                     
                    }

                }
                
                else if (has_pipe == 1)
                {
                    dup2(stdin_copy, 0);
                    dup2(stdout_copy, 1);
                    strcpy(cmdline2, "");
                    pipe_counter = 0;
                    strcpy(cmd_bef_pipe_1, "");
                    // printf("num_pipes are : %d\n", num_pipes);
                    
                    for (int counter = 0; counter < strlen(cmdline); counter++)
                    {
                        if (cmdline[counter] == '|')
                        {
                            pipe_counter++;
                        }
                        
                        if (cmdline[counter] != '|')
                        {
                            sing_char[0] = cmdline[counter];
                            strcat(cmdline2, sing_char);
                            
                        }
                        
                        if (cmdline[counter] == '|' && pipe_counter == 1)
                        {
                            strcpy(cmd_bef_pipe_1, cmdline2);
                            // printf("pipe separated comamnd is : %s\n", cmd_bef_pipe_1);
                            strcpy(cmdline2, "");
                            
                        }
                        
                         if (cmdline[counter] == '|' && pipe_counter == 2)
                        {
                            strcpy(cmd_af_pipe_1, cmdline2);
                            // printf("pipe separated comamnd is : %s\n", cmd_af_pipe_1);
                            strcpy(cmdline2, "");
                            
                        }
                        
                         if (counter == strlen(cmdline) - 1)
                        {
                            strcpy(cmd_af_pipe_2, cmdline2);
                            // printf("pipe separated comamnd is : %s\n", cmd_af_pipe_2);
                            strcpy(cmdline2, "");
                        }
                     
                        
                    }
               
                    
                    // printf("pipe command was %s\n", cmdline);
                    pipe(fd); pipe(fd2);

                    frk3 = fork();
                    
                    if (num_pipes == 1)
                    {

                        if (frk3 == 0)
                        {
                            dup2(fd[1], 1);
                            close(fd[0]);
                            close(fd[1]);
                            if((arglist = tokenize(cmd_af_pipe_1)) != NULL)
                            { 
                                execute(arglist); 
                            }
                            // execlp(cmd_af_pipe_1, cmd_af_pipe_1, NULL);
                            exit(0);
                       
                        }
                        else if (frk3 > 0)
                        {
                            dup2(fd[0], 0);
                            close(fd[0]);
                            close(fd[1]);
                            if((arglist = tokenize(cmd_bef_pipe_1)) != NULL)
                            { 
                                execute(arglist); 
                            }
                            // execlp(cmd_bef_pipe_1, cmd_bef_pipe_1, NULL);
                            exit(0);
                       
                        }
                    }
                    if (num_pipes == 2)
                    {

                        if (frk3 == 0) // child
                        {
                            dup2(fd[0], 0);
                            close(fd[1]);
                            close(fd[0]);

                            frk4 = fork();

                            if (frk4 > 0) // parent 
                            {
                                close(fd2[0]);
                                dup2(fd2[1], 1);
                                close(fd2[1]);
                                if((arglist = tokenize(cmd_af_pipe_1)) != NULL)
                                { 
                                    execute(arglist); 
                                }
                                // execlp(cmd_af_pipe_1, cmd_af_pipe_1, NULL);
                                exit(0);
                            }
                            if (frk4 == 0) // child
                            {
                                close(fd2[1]);
                                dup2(fd2[0], 0);
                                close(fd2[0]);
                                if((arglist = tokenize(cmd_af_pipe_2)) != NULL)
                                { 
                                    execute(arglist); 
                                }
                                // execlp(cmd_af_pipe_2, cmd_af_pipe_2, NULL);
                                exit(0);
                            }

                            exit(0);
                        }

                        if (frk3 > 0)
                        {
                            dup2(fd[1], 1);
                            close(fd[0]);
                            if((arglist = tokenize(cmd_bef_pipe_1)) != NULL)
                            { 
                                execute(arglist); 
                            }
                            exit(0);
                            // execlp(cmd_bef_pipe_1, cmd_bef_pipe_1, NULL);


                        }
                    }
                    
                    

                }
                
                
                else if (has_fi == 1)
                {
                    has_fi = 0;
                    
                    if (exit_status == 0)
                    {
                        if((arglist = tokenize(cmd_after_then)) != NULL)
                        { 
                            execute(arglist); 
                        }
                    }
                    
                    else
                    {
                        if((arglist = tokenize(cmd_after_else)) != NULL)
                        { 
                            execute(arglist); 
                        }
                    }
                }
                else if (has_else == 1 && strcmp(cmdline, "") != 0)
                {
                    has_else = 0;
                    strcpy(cmd_after_else, cmdline);
                }
                
                else if (has_then == 1 && strcmp(cmdline, "") != 0)
                {
                    has_then = 0;
                    strcpy(cmd_after_then, cmdline);
                }
                
                else if (has_if == 1)
                {
                    // printf("entered in has if \n");
                    remove("exit_status.txt");
                    system(strcat(strcat(cmdline, " ;"), "echo $? 1> exit_status.txt") );
                    fptr3 = fopen("exit_status.txt", "r+");
                    char chr = fgetc(fptr3);
                    
                    has_if = 0;
                    
                    if (chr =='0') // success 
                    {
                        // printf("exit status was 0\n");
                        exit_status = 0;
                    }
                    
                    else if (chr !='0') // unsuccessful
                    {
                        // printf("exit status was non-zero\n");
                        exit_status = 1;
                    }
                }
                
                else if (has_and != 1)
                {
                    if((arglist = tokenize(cmdline)) != NULL)
                    { 
                        execute(arglist); 
                    }
                    
                    if((arglist = tokenize(cmdline)) == NULL)
                    { printf("\n arglist was null \n"); }
                }
                
                else if (has_and == 1) // fork another process if has_and is 1
                {
                    // printf("has_and was 1 \n");
                    frk2 = fork();
                    
                    if (frk2 == 0) // child
                    {
                        if((arglist = tokenize(cmdline)) != NULL)
                        { execute(arglist); }
                        
                        if((arglist = tokenize(cmdline)) == NULL)
                        { printf("\n arglist was null \n"); }
                        exit(0);
                    }
                    
                }
                    
                
            }
            
            else if (frk > 0) // parent
            {
                
              // first wait for child to terminate
                waitpid(frk, &status, 0);
                
                if (access("output2.txt", F_OK) == 0)
                    { remove("output2.txt"); }
                
                if (access("output2.txt", F_OK) != 0)
                    { system("touch output2.txt"); }
                    
                fptr2 = fopen("output2.txt", "r+");
              
              while ((c = fgetc(fptr)) != EOF)
              {
              if (line_num >= 1)
                {
                    putc(c, fptr2);
                }
                
                if (c == '\n')
                line_num++;
    
              }
              
                if (access("output.txt", F_OK) == 0)
                    { remove("output.txt"); }
                    
                if (access("output.txt", F_OK) != 0)
                    { system("touch output.txt"); }
                    
                fptr = fopen("output.txt", "r+");
                while ((c = fgetc(fptr2)) != EOF)
                {
                    putc(c, fptr);
                }
    
              
            }
            
            
        }
    
    }
    
    
    return 0;
}