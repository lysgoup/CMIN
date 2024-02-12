#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef DEBUG
  #define debug(fn) fn
#else
  #define debug(fn)
#endif

int c2p_pipe[2];
int p2c_pipe[2];
pid_t test_pid;

void signal_handler(int signum);
int bytencat(unsigned char *input1, unsigned char *input2, int input1_size, int input1_len, int input2_len);
int bytencpy(unsigned char *new, unsigned char *old, int size);
int write_bytes(int fd, void *buf, size_t len);
int check_error(unsigned char *input_buf, int input_buf_len, char *target, char *error_msg);
int Reduced(unsigned char *input_buf, int input_buf_len, char *target, char *error_msg, char *output_file);

void signal_handler(int signum) {
  if (signum == SIGALRM) {
    fprintf(stderr, "The execution time of the target program exceeded 3 seconds!!!\n");
    kill(test_pid, SIGKILL);
    exit(EXIT_FAILURE);
  }
}

int bytencat(unsigned char *input1, unsigned char *input2, int input1_size, int input1_len, int input2_len){
  int i=0;
  if((input1_len + input2_len) > input1_size) return -1;
  for(i=0;i<input2_len;i++){
    input1[input1_len+i] = input2[i];
  }
  return input1_len + input2_len;
}

//TODO: error check
int bytencpy(unsigned char *new, unsigned char *old, int size){
  for(int i=0;i<size;i++){
    new[i] = old[i];
  }
  return 0;
}

int write_bytes(int fd, void *buf, size_t len){
  char *p = (char *)buf ;
  size_t acc = 0 ;

  while (acc < len) {
    size_t written ;
    if ((written = write(fd, p, len - acc)) == -1)
      return 1 ;
    p += written ;
    acc += written ;
  }
  return 0 ;
}

int check_error(unsigned char *input_buf, int input_buf_len, char *target, char *error_msg){
  debug(printf("error msg: %s\n",error_msg);)

  //make pipe
  if (pipe(c2p_pipe) == -1) {
    perror("pipe error\n");
    exit(EXIT_FAILURE);
  }
  if (pipe(p2c_pipe) == -1) {
    perror("pipe error\n");
    exit(EXIT_FAILURE);
  }

  char *test_argv[] = {"/bin/bash","-c",target, NULL};
  test_pid = fork();
  switch(test_pid){
    case -1:
      perror("fork error\n");
      exit(EXIT_FAILURE);
    //child process
    case 0:
      close(c2p_pipe[0]);
      close(p2c_pipe[1]);
			close(STDOUT_FILENO);
      dup2(c2p_pipe[1],STDERR_FILENO);
      dup2(p2c_pipe[0],STDIN_FILENO);
      if(execv("/bin/bash",test_argv) == -1){
        perror("exec error\n");
        exit(EXIT_FAILURE);
      };
    //parent process
    default:
      signal(SIGALRM,signal_handler);
      alarm(3);
      close(c2p_pipe[1]);
      close(p2c_pipe[0]);
      //write input buf into pipe
      write_bytes(p2c_pipe[1],input_buf,input_buf_len);
      close(p2c_pipe[1]);
      wait(NULL);
      int read_len;
  		char error_buf[4096];
      char *p = error_buf;
      while((read_len = read(c2p_pipe[0], p+read_len, sizeof(error_buf)-read_len)) > 0){
        p += read_len;
      }
      *p = '\0';
      close(c2p_pipe[0]);
      char *find_error_msg;
			find_error_msg = strstr(error_buf,error_msg);
      //if there is a error: return 1
			if(find_error_msg == NULL){
				return 0;
			}
			else{
				return 1;
			}
  }
}

int Reduced(unsigned char *input_buf, int input_buf_len, char *target, char *error_msg, char *output_file){
  int s = input_buf_len - 1;
  while(s > 0){
    printf("%d\n",s);
    //check head+tail
    for(int i=0;i<(input_buf_len - s + 1);i++){
      int head_size = i;
      int tail_size = input_buf_len-head_size-s;
      unsigned char *o = (unsigned char *)malloc(head_size + tail_size+1);
      int o_len = 0;
      if(head_size > 0){
        unsigned char *head = (unsigned char *)malloc(head_size+1);
        if(bytencpy(head,input_buf,head_size)){
          perror("error bytencpy\n");
          return 1;
        };
        head[head_size] = '\0';
        if((o_len = bytencat(o,head,(head_size+tail_size+1),o_len,head_size)) == -1){
          perror("error bytencat\n");
          return 1;
        };
        o[head_size+tail_size] = '\0';
        debug(printf("head: %d %s\n",head_size,head);)
        free(head);
      }
      if(tail_size > 0){
        unsigned char *tail = (unsigned char *)malloc(tail_size+1);
        if(bytencpy(tail,&input_buf[i+s],tail_size)){
          perror("error bytencpy\n");
          return 1;
        };
        tail[tail_size] = '\0';
        if((o_len = bytencat(o,tail,(head_size+tail_size+1),o_len,tail_size)) == -1){
          perror("error bytencat\n");
          return 1;
        };
        o[head_size+tail_size] = '\0';
        debug(printf("tail: %d %s\n",tail_size,tail);)
        free(tail);
      }
      debug(printf("o: %d %s\n",o_len,o);)
      //test
      if(check_error(o,o_len,target,error_msg)){
        free(input_buf);
        return Reduced(o,o_len,target,error_msg,output_file);
      }
      free(o);
    }
    //check mid
    for(int i=0;i<(input_buf_len - s + 1);i++){
      unsigned char *mid = (unsigned char *)malloc(s+1);
      int mid_len = s;
      if(bytencpy(mid,&input_buf[i],mid_len)){
        perror("error bytencpy\n");
        return 1;
      };
      mid[s] = '\0';
      debug(printf("mid: %s\n",mid);)
      //test
      if(check_error(mid,mid_len,target,error_msg)){
        free(input_buf);
        return Reduced(mid,mid_len,target,error_msg,output_file);
      }
      free(mid);
    }
    s -= 1;
  }
  FILE *fp = fopen(output_file,"wb");
  fwrite(input_buf, 1, input_buf_len, fp);
  fclose(fp);
  return 0;
}

int main(int argc, char *argv[]){
  int opt;
  char *input_file = NULL;
  char *error_msg = NULL;
  char *reduced = NULL;
  unsigned char *input_buf;
  struct stat file_info;
  int read_len = 0;
  int input_buf_len = 0;
	unsigned char buf[32];

  //option parsing
  if (argc < 8) {
		printf("Usage: %s -i <input> -m <error msg> -o <reduced> <target program> \n", argv[0]) ;
		exit(EXIT_FAILURE) ;
	}

  while((opt = getopt(argc, argv, "i:m:o:")) != -1) {
    switch(opt) {
      case 'i':
        input_file = optarg;
        break;
      case 'm':
        error_msg = optarg;
        break;
      case 'o':
        reduced = optarg;
        break;
      default:
        fprintf(stderr, "Usage: %s -i <input file> -m <error msg> -o <reduced> <target program>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
	debug(printf("error_msg: %s\n",error_msg);)
  char *target = argv[optind];

  //open input file
  FILE *fp = fopen(input_file,"rb");
  if (stat(input_file, &file_info) != 0) {
    perror("stat error\n");
    exit(EXIT_FAILURE);
  }
  long long file_size = file_info.st_size;
  debug(printf("file size: %lld bytes\n", file_size);)

  //read a input file and put it into input buffer
  input_buf = (unsigned char *)malloc(sizeof(unsigned char) * file_size);
  while (((read_len = fread(&buf,sizeof(unsigned char),sizeof(buf),fp)) > 0)){
    if((input_buf_len = bytencat(input_buf,buf,file_size,input_buf_len,read_len)) == -1){
      perror("bytencat error\n");
      exit(EXIT_FAILURE);
    }
  }
  debug(printf("input buf: %s\n",input_buf);)
  fclose(fp);

  //reducing algorithm
  if(Reduced(input_buf,input_buf_len,target,error_msg,reduced)){
    printf("error reducing algorithm\n");
    exit(EXIT_FAILURE);
  };
  return 0;
}
