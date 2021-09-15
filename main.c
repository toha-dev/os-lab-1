#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <sys/wait.h>

#define FORK_DATA_SIZE 7
int fork_data[FORK_DATA_SIZE] = { 0, 1, 2, 1, 3, 4, 5 };

int exec_index = 1;

int parent_process_number = -1;
int current_process_number = 1;

/* Output */ 

void log_error(const char *message)
{
    printf("%s", message);
}

void print_process_info()
{
    printf("Process initialized. Info: \n  ppid: %d\n  pid: %d\n  ppn: %d\n  pn: %d\n\n", 
        getppid(), getpid(), parent_process_number, current_process_number);
}

/* End output */

void custom_exit()
{
    if (current_process_number == exec_index)
    {
        custom_exec();
    }

    printf("Proccess with id %d and number %d terminated\n\n", getpid(), current_process_number);
    _exit(0);
}

void custom_exec()
{
    printf("Calling custom exec from process number %d before termination:\n", current_process_number);
    
    char *args[] = {"./exec", NULL};
    execvp(args[0], args);
}
 
int get_child_count(int child_number)
{
    int result = 0;
    
    for (int i = 0; i < FORK_DATA_SIZE; ++i)
    {
        result += fork_data[i] == child_number;
    }
     
    return result;
}

int* get_child_numbers(int child_count, int child_number)
{
    int *result = (int*)malloc(sizeof(int) * child_count);
    for (int i = 0, j = 0; i < FORK_DATA_SIZE; ++i)
    {
        if (fork_data[i] == child_number)
        {
            result[j++] = i + 1;
        }
    }

    return result;
}
 
void create_child_processes(int *arr, int count) 
{
    pid_t *children = (pid_t*)malloc(sizeof(pid_t) * count);

    for (int i = 0; i < count; ++i)
    {
        auto pid = fork();
        
        if (pid == -1)
        {
            log_error("Fork process failed");
        }
        else if (pid == 0)
        {
            parent_process_number = current_process_number;
            current_process_number = arr[i];

            print_process_info();
         
            int child_count = get_child_count(current_process_number); 
            if (child_count != 0)
            {
                create_child_processes(get_child_numbers(child_count, current_process_number), child_count);
            }
            
            custom_exit();
        }
        
        children[i] = pid;
    }

    for (int i = 0; i < count; ++i)
    {
        if (children[i] == 0 || children[i] == -1)
        {
            continue;
        }

        int status;
        (void)waitpid(children[i], &status, 0);
    }

    free(children);
}

int main() 
{
    print_process_info();

    int count = get_child_count(current_process_number);
    create_child_processes(get_child_numbers(count, current_process_number), count);
    
    custom_exit();
}
