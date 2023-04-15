#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)

int child_func(void *arg) {
	printf("Child thread: My TID is %ld\n", gettid());
	return 0;
}

int main(int argc, char *argv[]) {
	void *stack = NULL;
	pid_t tid;
	int status;

	stack = malloc(STACK_SIZE);
	if(stack == NULL) {
		perror("[ERROR]: fail to allocate memory for stack");
		exit(0);
	}

	tid = clone(child_func, stack + STACK_SIZE, CLONE_THREAD | CLONE_SIGHAND | CLONE_VM | SIGUSR1, NULL);
	if (tid == -1) {
		perror("[ERROR]: fail to create thread with clone");
		exit(0);
	}

	printf("Parent thread: My TID is %ld, child TID is %ld\n", gettid(), tid);
	
	// this will always fail.
	// it doesn't match the behavior described in man pages
	if(waitpid(tid, &status, __WCLONE) == -1) {
		perror("[ERROR]: fail to wait for child thread");
		exit(0);
	}

	free(stack);

	return 0;
}
