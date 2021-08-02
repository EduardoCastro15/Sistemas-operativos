#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <pwd.h>

/*Variables globales*/
int pipe_contador=0, fd;
static char* argumentos[512];
char *history_archivo;
char input_buffer[1024];
char *cmd_exec[100];
int bandera, len;
char cwd[1024];
int bandera_pipe=1;
pid_t pid;
int numlineas;
int ambiente_bandera;
int bandera_pipe, bandera_sin_pipe,  output_redireccion, input_redireccion;
int bang_bandera;
int pid, status;
char history_datos[1000][1000];
char directorio_actual[1000];
char ret_file[3000];
char his_var[2000];
char *input_redireccion_archivo;
char *output_redireccion_archivo;
extern char** environ;

                        /*Prototipos de funciones*/
void resetVariables();
void procesoArchivo ();
void escrituraArchivo();
void bangEjecut();
void ambiente();
void variablesAmbiente();
void cambiarDirectorio();
void padreDirectorio();
void echoLlamada(char *echo_val);
void historyEjecutarConstantes();
static char* skipwhite(char* s);
void tokenise_commands(char *com_exec);
void tokenise_redirect_input_output(char *cmd_exec);
void tokenise_redirect_input(char *cmd_exec);
void tokenise_redirect_output(char *cmd_exec);
char* skipcomma(char* str);
static int split(char *cmd_exec, int, int, int);
void ejecutaConPipe();
static int command(int, int, int, char *cmd_exec);
void prompt();
void sigintHandler(int sig_num);

/*************************************************************************/
void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    fflush(stdout);
}
void resetVariables()
{
  fd =0;
  bandera=0;
  len=0;
  numlineas=0;
  pipe_contador=0;
  bandera_pipe=0;
  bandera_sin_pipe=0;
  output_redireccion=0;
  input_redireccion=0;
  input_buffer[0]='\0';
  cwd[0] = '\0';
  pid=0;
  ambiente_bandera=0;
  bang_bandera=0;
}
  
void procesoArchivo()
{
  int fd;
  history_archivo=(char *)malloc(100*sizeof(char));
  strcpy(history_archivo,directorio_actual);
  strcat(history_archivo, "/");
  strcat(history_archivo, "history.txt"); 
  fd=open(history_archivo, O_RDONLY|O_CREAT,S_IRUSR|S_IWUSR);
  int bytesleer=0, i=0, x=0, index=0;
  char buffer[1], temp_datos[1000];
    do 
    {
        bytesleer = read (fd, buffer, sizeof (buffer));
        for (i=0; i<bytesleer; ++i) 
                  {
                    temp_datos[index]=buffer[i];
                    index++;
                    if(buffer[i]=='\n')
                        {
                          temp_datos[index-1]='\0';
                          numlineas++; 
                          index=0;
                          strcpy(history_datos[x],temp_datos);
                          x++;
                          temp_datos[0]='\0';
                        }
                  }
    }
    while (bytesleer == sizeof (buffer)); 
    close (fd); 
}

void escrituraArchivo()
{
  
  int fd_out,ret_write,str_len=0;
  char input_datos[2000];
  numlineas++;
  char no[10];
  sprintf (no, "%d",numlineas); 
  strcpy(input_datos, " ");
  strcat(input_datos, no);
  strcat(input_datos, " ");
  strcat(input_datos, input_buffer);

  str_len = strlen(input_datos);
  fd_out=open(history_archivo,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
  len=strlen(input_buffer);
  ret_write=write(fd_out,input_datos,str_len);
  if(ret_write<0) 
        {
          printf("Error in writing file\n");
          return;
        }
  close(fd_out);

}
void bangEjecut()
{
  char bang_val[1000];
  char *tokenise_bang[100], *num_ch[10];
  int i, n=1, num, index=0;
  i=1;
  if(input_buffer[i]=='!')
           strcpy(bang_val, history_datos[numlineas-1]);
        
  else if(input_buffer[i]=='-')
    {
        n=1;
        num_ch[0]=strtok(input_buffer,"-");
        while ((num_ch[n]=strtok(NULL,"-"))!=NULL)
              n++;
        num_ch[n]=NULL;
        num = atoi(num_ch[1]);

        index = numlineas-num;
        strcpy(bang_val, history_datos[index]);
              
    }
  else 
    {
      num_ch[0]=strtok(input_buffer,"!");
      num = atoi(num_ch[0]);
      strcpy(bang_val, history_datos[num-1]);
    }
  tokenise_bang[0]=strtok(bang_val," ");
  while ((tokenise_bang[n]=strtok(NULL,""))!=NULL)
              n++;
  tokenise_bang[n]=NULL;
  strcpy(bang_val, tokenise_bang[1]);
  printf("%s\n", bang_val );
  strcpy(input_buffer, bang_val);   
}

void ambiente()
{
  int i =1, index=0;
  char env_val[1000], *value;
  while(argumentos[1][i]!='\0')
              {
                   env_val[index]=argumentos[1][i];
                   index++;
                    i++;
              }
  env_val[index]='\0';
  value=getenv(env_val);

  if(!value)
      printf("\n");
  else printf("%s\n", value);
}

void variablesAmbiente()
{  
int n=1;
char *left_right[100];
if(argumentos[1]==NULL)
   {
        char** env;
          for (env = environ; *env != 0; env++)
          {
            char* value = *env;
            printf("declara -x %s\n", value);    
          }  
         return; 
    }
left_right[0]=strtok(argumentos[1],"=");
while ((left_right[n]=strtok(NULL,"="))!=NULL)
      n++;
left_right[n]=NULL;
setenv(left_right[0], left_right[1], 0);
}


 
void cambiarDirectorio()
{
char *h="/home";   
if(argumentos[1]==NULL)
        chdir(h);
else if ((strcmp(argumentos[1], "~")==0) || (strcmp(argumentos[1], "~/")==0))
        chdir(h);
else if(chdir(argumentos[1])<0)
    printf("bash: cd: %s: No such file or directory\n", argumentos[1]);

}

void padreDirectorio()
{
if (getcwd(cwd, sizeof(cwd)) != NULL)
         printf("%s\n", cwd );
        
else
       perror("getcwd() error");
}

void echoLlamada(char *echo_val)
{
  int i=0, index=0;
  ambiente_bandera=0;
  char nuevosarg[1024],env_val[1000], *str[10];
  str[0]=strtok(echo_val," ");
  str[1]=strtok(NULL, "");
  strcpy(env_val, argumentos[1]);
  if(str[1]==NULL)
         {
          printf("\n");
          return;
         } 
  if (strchr(str[1], '$')) 
    ambiente_bandera=1;
                    
  memset(nuevosarg, '\0', sizeof(nuevosarg));
  i=0; 
  while(str[1][i]!='\0')
    {
      if(str[1][i]=='"')
      {
      index=0;     
      while(str[1][i]!='\0')
          {
          if(str[1][i]!='"')
                {
                nuevosarg[index]=str[1][i];
                 bandera=1;
                index++;
                }
          i++;
          }             
      }
      else if(str[1][i]==39)
      {
      index=0;
      while(str[1][i]!='\0')
          {
          if(str[1][i]!=39)
                {
                nuevosarg[index]=str[1][i];
                 bandera=1;
                index++;
                }
          i++;
          }               
      }
      else if(str[1][i]!='"')
        {
          nuevosarg[index]=str[1][i];
          index++;
          i++;
        }
      else 
        i++;    
    }

nuevosarg[index]='\0';
if((strcmp(argumentos[1], nuevosarg)==0)&&(ambiente_bandera==0))
    printf("%s\n",nuevosarg);
else 
  {
     strcpy(argumentos[1],nuevosarg);
      if(ambiente_bandera==1)
          ambiente();
      else if(ambiente_bandera==0)
              printf("%s\n",nuevosarg); 
  }  
}
void historyEjecutarConstantes()
{
  int num, i, start_index;
  if(bang_bandera==1)
        {
         for(i=0; i<numlineas; i++)
            printf("%s\n", history_datos[i]); 
        }
  else if(argumentos[1]==NULL)
      {
        for(i=0; i<numlineas-1; i++)
            printf("%s\n", history_datos[i] );
        printf(" %d %s\n", numlineas, his_var );
      }
  else
      {
        if(argumentos[1]!=NULL)
        num = atoi(argumentos[1]);
        if(num>numlineas) 
        {
          for(i=0; i<numlineas-1; i++)
            printf("%s\n", history_datos[i] );
          printf(" %d %s\n", numlineas, his_var );
        }
        start_index=numlineas-num;
        for(i=start_index; i<numlineas-1; i++)
            printf("%s\n", history_datos[i] );
          printf(" %d %s\n", numlineas, his_var );
      } 
}

static char* skipwhite(char* s)
{
  while (isspace(*s)) ++s;
  return s;
}

void tokenise_commands(char *com_exec)
{
int m=1;
argumentos[0]=strtok(com_exec," ");       
while((argumentos[m]=strtok(NULL," "))!=NULL)
        m++;
}
void tokenise_redirect_input_output(char *cmd_exec)
{
  char *io_token[100];
  char *new_cmd_exec1;  
  new_cmd_exec1=strdup(cmd_exec);
  int m=1;
  io_token[0]=strtok(new_cmd_exec1,"<");       
  while((io_token[m]=strtok(NULL,">"))!=NULL)
        m++;
  io_token[1]=skipwhite(io_token[1]);
  io_token[2]=skipwhite(io_token[2]);
  input_redireccion_archivo=strdup(io_token[1]);
  output_redireccion_archivo=strdup(io_token[2]);
  tokenise_commands(io_token[0]);
  
}
void tokenise_redirect_input(char *cmd_exec)
{
  char *i_token[100];
  char *new_cmd_exec1;  
  new_cmd_exec1=strdup(cmd_exec);
  int m=1;
  i_token[0]=strtok(new_cmd_exec1,"<");       
  while((i_token[m]=strtok(NULL,"<"))!=NULL)
        m++;
  i_token[1]=skipwhite(i_token[1]);
  input_redireccion_archivo=strdup(i_token[1]);
  tokenise_commands(i_token[0]);
}
void tokenise_redirect_output(char *cmd_exec)
{
  char *o_token[100];
  char *new_cmd_exec1;  
  new_cmd_exec1=strdup(cmd_exec);
  int m=1;
  o_token[0]=strtok(new_cmd_exec1,">");       
  while((o_token[m]=strtok(NULL,">"))!=NULL)
          m++;
  o_token[1]=skipwhite(o_token[1]);
  output_redireccion_archivo=strdup(o_token[1]); 
  tokenise_commands(o_token[0]);   
  
}
char* skipcomma(char* str)
{
  int i=0, j=0;
  char temp[1000];
  while(str[i++]!='\0')
            {
              if(str[i-1]!='"')
                    temp[j++]=str[i-1];
            }
        temp[j]='\0';
        str = strdup(temp);
  
  return str;
}
static int split(char *cmd_exec, int input, int first, int last)
{
    char *new_cmd_exec1;  
    new_cmd_exec1=strdup(cmd_exec);
   //else
      {
        int m=1;
        argumentos[0]=strtok(cmd_exec," ");       
        while((argumentos[m]=strtok(NULL," "))!=NULL)
              m++;
        argumentos[m]=NULL;
        if (argumentos[0] != NULL) 
            {

            if (strcmp(argumentos[0], "exit") == 0) 
                    exit(0);
            if (strcmp(argumentos[0], "echo") != 0) 
                    {
                      cmd_exec = skipcomma(new_cmd_exec1);
                      int m=1;
                      argumentos[0]=strtok(cmd_exec," ");       
                      while((argumentos[m]=strtok(NULL," "))!=NULL)
                                m++;
                      argumentos[m]=NULL;
                    }
            if(strcmp("cd",argumentos[0])==0)
                    {
                    cambiarDirectorio();
                    return 1;
                    }
            else if(strcmp("pwd",argumentos[0])==0)
                    {
                    padreDirectorio();
                    return 1;
                    }
            }
        }
    return command(input, first, last, new_cmd_exec1);
}


void ejecutaConPipe()
{

int i, n=1, input, first;

input=0;
first= 1;

cmd_exec[0]=strtok(input_buffer,"|");

while ((cmd_exec[n]=strtok(NULL,"|"))!=NULL)
      n++;
cmd_exec[n]=NULL;
pipe_contador=n-1;
for(i=0; i<n-1; i++)
    {
      input = split(cmd_exec[i], input, first, 0);
      first=0;
    }
input=split(cmd_exec[i], input, first, 1);
input=0;
return;
}

static int command(int input, int first, int last, char *cmd_exec)
{
  int mypipefd[2], ret, input_fd, output_fd;
  ret = pipe(mypipefd);
  if(ret == -1)
      {
        perror("Error en pipe");
        return 1;
      }
  pid = fork();
 
  if (pid == 0) 
  {
    if (first==1 && last==0 && input==0) 
    {
      dup2( mypipefd[1], 1 );
    } 
    else if (first==0 && last==0 && input!=0) 
    {
      dup2(input, 0);
      dup2(mypipefd[1], 1);
    } 
    else 
    {
      dup2(input, 0);
    }
    if (strchr(cmd_exec, '<') && strchr(cmd_exec, '>')) 
            {
              input_redireccion=1;
              output_redireccion=1;
              tokenise_redirect_input_output(cmd_exec);
            }
   else if (strchr(cmd_exec, '<')) 
        {
          input_redireccion=1;
          tokenise_redirect_input(cmd_exec);
        }
   else if (strchr(cmd_exec, '>')) 
        {
          output_redireccion=1;
          tokenise_redirect_output(cmd_exec);
        }
    if(output_redireccion== 1)
                {                    
                        output_fd= creat(output_redireccion_archivo, 0644);
                        if (output_fd < 0)
                          {
                          fprintf(stderr, "Error al abrir %s para escritura\n",output_redireccion_archivo);
                          return(EXIT_FAILURE);
                          }
                        dup2(output_fd, 1);
                        close(output_fd);
                        output_redireccion=0;
                }
    if(input_redireccion  == 1)
                  {
                         input_fd=open(input_redireccion_archivo,O_RDONLY, 0);
                         if (input_fd < 0)
                          {
                          fprintf(stderr, "Error al abrir %s para lectura\n", input_redireccion_archivo);
                          return(EXIT_FAILURE);
                          }
                        dup2(input_fd, 0);
                        close(input_fd);
                        input_redireccion=0;
                  }
     if (strcmp(argumentos[0], "export") == 0)
                  {
                  variablesAmbiente();
                  return 1;
                  }
    if (strcmp(argumentos[0], "echo") == 0)
              {
              echoLlamada(cmd_exec);
              } 
    else if (strcmp(argumentos[0], "history") == 0)
             {
              historyEjecutarConstantes();
              } 
 
    else if(execvp(argumentos[0],argumentos)<0) printf("%s: comando no encontrado\n", argumentos[0]);
              exit(0);
  }
  else 
  {
     waitpid(pid, 0, 0);  
   }
 
  if (last == 1)
    close(mypipefd[0]);
  if (input != 0) 
    close(input);
  close(mypipefd[1]);
  return mypipefd[0];
}

void prompt()
{
  char hostname[256],directorio[256];
  gethostname(hostname,255);/*Obtener host name o nombre del equipo*/
  getcwd(directorio,255); /*Obtener directorio actual*/
  struct passwd *p = getpwuid(getuid());/*Obtiene el nombre del usuario */
  strcat(p->pw_name,"@");/*Concateno @ al nombre del usuario*/
  strcat(p->pw_name,hostname);/*Concateno el hostname a la cadena de usuario + @ */
  strcat(p->pw_name,":"); /*Concateno : */
  strcat(p->pw_name,directorio); /*Concateno el directorio actual a la cadena resultante */
  strcat(p->pw_name,"$"); /*Concateno un $ */
  strcat(p->pw_name," ");/*Concateno un espacio en blanco*/
  printf("%s",p->pw_name); 
}

int main()
{   
    int status;
    char ch[2]={"\n"};
    getcwd(directorio_actual, sizeof(directorio_actual));
    signal(SIGINT, sigintHandler);
    while (1)
    {
      //PARTE1 Obtención de información, reset de variables y entrada de datos. 
      resetVariables();
      prompt();
      fgets(input_buffer, 1024, stdin);
      //PARTE2 Evaluación de cadena y actuar en caso de que esta sea un salto de línea
      if(strcmp(input_buffer, ch)==0)
            {
              continue;
            }
      //PARTE3 Escritura del historial de comandos. 
      if(input_buffer[0]!='!')
            {
                procesoArchivo();
                escrituraArchivo(); 
            }
      len = strlen(input_buffer);
      input_buffer[len-1]='\0';
      strcpy(his_var, input_buffer);
      //PARTE4 Ejecución del comando exit. 
      if(strcmp(input_buffer, "exit") == 0) 
            {
              bandera = 1;
              break;
            }
      //PARTE5
      if(input_buffer[0]=='!')  
              {
                procesoArchivo();
                bang_bandera=1;
                bangEjecut();
              }
      ejecutaConPipe();
      waitpid(pid,&status,0);
    }  
    if(bandera==1)
        exit(0);       
      
return 0;
}