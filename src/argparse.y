%{
  #include <stdio.h> 
  #include <string.h> 
  #include <stdlib.h>
  #include <unistd.h>
  #include <stdarg.h>
  #include <fcntl.h>
  #include "mcell_structs.h"
  #include "strfunc.h"
  #include "argparse.h"

  #define YYPARSE_PARAM argparse_param
  #define YYLEX_PARAM argparse_param

  #define argpvp ((struct argparse_vars *)YYPARSE_PARAM)
  #define volp argpvp->vol
%}


%union {
int tok;
char *str;
double dbl;
} 


%{
  #include "arglex.flex.c"
%}


%pure_parser

%name-prefix="arg"
%output="argparse.bison.c"

%token <tok> REAL INTEGER HELP_OPT LOG_FILE_OPT LOG_FREQ_OPT FILE_NAME
%token <tok> INFO_OPT SEED_OPT ITERATIONS_OPT CHECKPOINT_OPT
%token <tok> EOF_TOK
%type <dbl> int_arg
/*
%type <dbl> real_arg num_arg 
*/

%right '='
%left '+' '-'
%left '*' '/'
%left UNARYMINUS

%%

option_format: 
	option_list
;

option_list: option
	| option_list option
;

option: HELP_OPT
{
  return(1);
}
	| INFO_OPT
{
  volp->info_opt=1;
}
	| mdl_infile_cmd
{
}
	| seed_cmd 
{
}
	| iterations_cmd 
{
}
	| checkpoint_cmd
{
}
	| log_file_cmd
{
}
	| log_freq_cmd
{
}
	| EOF_TOK
{
  if (volp->mdl_infile_name==NULL) {
    sprintf(argpvp->arg_err_msg,"No MDL file name specified");
    argerror(argpvp->arg_err_msg,argpvp);
    return(1);
  }
  return(0);
};

mdl_infile_cmd: FILE_NAME
{
  if (volp->mdl_infile_name!=NULL) {
    sprintf(argpvp->arg_err_msg,"Multiple MDL file names specified: %s",argpvp->cval);
    argerror(argpvp->arg_err_msg,argpvp);
    return(1);
  }
  volp->mdl_infile_name=my_strdup(argpvp->cval);
  free((void *)argpvp->cval);
};


seed_cmd: SEED_OPT int_arg
{
  volp->seed_seq=(int) $<dbl>2;
  if (volp->seed_seq < 1 || volp->seed_seq > 3000) {
    sprintf(argpvp->arg_err_msg,"Random sequence number %d not in range 1 to 3000",
      volp->seed_seq);
    argerror(argpvp->arg_err_msg,argpvp);
    return(1);
  }
};

iterations_cmd: ITERATIONS_OPT int_arg
{
  volp->iterations=(int) $<dbl>2;
  if (volp->iterations < 0) {
    sprintf(argpvp->arg_err_msg,"Iterations %d is less than 0",
      volp->iterations);
    argerror(argpvp->arg_err_msg,argpvp);
    return(1);
  }
};


checkpoint_cmd: CHECKPOINT_OPT FILE_NAME
{
  volp->chkpt_infile = my_strdup(argpvp->cval);
  free((void *)argpvp->cval);
  if ((volp->chkpt_signal_file_tmp=fopen(volp->chkpt_infile,"rb"))==NULL) {
    sprintf(argpvp->arg_err_msg,"Cannot open input checkpoint file (Resume from USR Signal) : %s",volp->chkpt_infile);
    argerror(argpvp->arg_err_msg,argpvp);
    volp->chkpt_init=1;
    return(1);  
  }
  volp->chkpt_init=0;
  volp->chkpt_flag = 1;
  fclose(volp->chkpt_signal_file_tmp);

};

log_file_cmd: LOG_FILE_OPT FILE_NAME
{
  volp->log_file_name=my_strdup(argpvp->cval);
  free((void *)argpvp->cval);
  if ((volp->log_file=fopen(volp->log_file_name,"w"))==NULL) {
    sprintf(argpvp->arg_err_msg,"Cannot open output log file: %s",
      volp->log_file_name);
    argerror(argpvp->arg_err_msg,argpvp);
    return(1);
  }
};

log_freq_cmd: LOG_FREQ_OPT int_arg
{
  volp->log_freq=(int) $<dbl>2;
};

int_arg: INTEGER {$$=(double)argpvp->ival;}
;

/*
real_arg: REAL {$$=argpvp->rval;}
;

num_arg: INTEGER {$$=(double)argpvp->ival;}
	| REAL {$$=argpvp->rval;}
;
*/

%%



void argerror(char *s,...)
{
  va_list ap;
  FILE *log_file;
  struct argparse_vars *apvp;

  va_start(ap,s);
  apvp=va_arg(ap,struct argparse_vars *);
  va_end(ap);

  log_file=stderr;
  if (apvp->vol->log_file!=NULL) {
    log_file=apvp->vol->log_file;
  }

  fprintf(log_file,"\nMCell: command-line argument syntax error: %s\n",s);
  fflush(log_file);
  return;
}

int argparse_init(int argc, char *argv[], struct volume *vol)
{
  struct argparse_vars arg_var;
  FILE *fs1,*fs2;
  int fildes[2];
  int i;


  arg_var.vol=vol;

  vol->log_freq=100;
  vol->log_file_name=NULL;
  vol->log_file=stderr;
  vol->seed_seq=1;
  vol->info_opt=0;
  vol->mdl_infile_name=NULL;

  if ((arg_var.arg_err_msg=(char *)malloc(1024*sizeof(char)))==NULL) {
    fprintf(vol->log_file,"MCell: out of memory storing arg_err_msg\n");
    return(1);
  }

  if (pipe(fildes)) {
    fprintf(vol->log_file,"MCell: cannot open pipe\n");
    return(1);
  }
  if ((fs1 = fdopen(fildes[0],"r"))==NULL) {
    fprintf(vol->log_file,"MCell: cannot open pipe\n");
    return(1);
  }
  if ((fs2 = fdopen(fildes[1],"w"))==NULL) {
    fprintf(vol->log_file,"MCell: cannot open pipe\n");
    return(1);
  }

  for (i=1;i<argc;i++) {
    fprintf(fs2,"%s ",argv[i]);
    fflush(fs2);
  }
  fclose(fs2);

  argrestart(fs1);
  if (argparse((void *) &arg_var)) {
     return(1);
  }
  fclose(fs1);

  return(0);
}
