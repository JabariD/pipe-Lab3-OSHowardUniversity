// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

// 0 - reading
// 1 - writing

void concatFixedStringToPipedString(char concat_str[100], char fixed_str[]) {
  int k = strlen(concat_str); 
  int i; 
  for (i = 0; i < strlen(fixed_str); i++) 
    concat_str[k++] = fixed_str[i]; 

  concat_str[k] = '\0';   // string ends with '\0' 

  printf("Concatenated string %s\n", concat_str);
}
  
int main() { 
  // We use two pipes 
  // First pipe to send input string from parent 
  // Second pipe to send concatenated string from child 
  
  int fd1[2];  // Used to store two ends of first pipe 
  int fd2[2];  // Used to store two ends of second pipe 

  char fixed_str[] = "howard.edu"; 
  char fixed_str2[] = "gobison.org";

  // Create 2 pipes and check failure.
  if ( pipe(fd1) == -1 || pipe(fd2) == -1) { 
    fprintf(stderr, "Pipe Failed" ); 
    return 1; 
  }

  // Read input string
  char input_str[100]; 
  printf("Enter a string to concatenate: ");
  scanf("%s", input_str); 

  // Create 2 processes
  pid_t p = fork(); 

  if (p < 0) { 
    // Fork failed
    fprintf(stderr, "fork Failed" ); 
    return 1; 
  } 
  else if (p > 0)  { 
    // Parent process 

    // The parent will only write using Pipe1 and read using Pipe2.
    close(fd1[0]); 
    close(fd2[1]);

    // Write input string and close writing end of Pipe1. 
    write(fd1[1], input_str, strlen(input_str) + 1); 
    
    // Wait for child to send a string and exit
    wait(NULL); 

    // Read a string from Pipe2
    char concat_str[100]; 
    read(fd2[0], concat_str, 100); 

    // Concatenate a fixed string with string from Pipe1
    concatFixedStringToPipedString(concat_str, fixed_str2);

    close(fd1[1]);
    close(fd2[0]);
  } else { 
    // child process

    // The child will only read using Pipe1 and write using Pipe2.
    close(fd1[1]);  
    close(fd2[0]); 
  
    // Read the string from Pipe1 and store it in concat_str.
    char concat_str[100]; 
    read(fd1[0], concat_str, 100); 

    // Print concatenated_str + fixed string
    concatFixedStringToPipedString(concat_str, fixed_str);

    // Read input string
    char input_str[100]; 
    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 

    // 
    concatFixedStringToPipedString(concat_str, input_str);

    // Write input string and close writing end of first pipe. 
    write(fd2[1], concat_str, strlen(concat_str) + 1); 

    close(fd1[0]);
    close(fd2[1]);

    exit(0); 
  } 
} 