
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <windows.h>

#define PASSTHROUGH_BUFFER_SIZE 4096

#define PIPE_PATH_PREFIX "\\\\.\\pipe\\"

#define EXIT_CODE_USAGE 1
#define EXIT_CODE_CREATE_FAILED 2
#define EXIT_CODE_CONNECT_FAILED 3
#define EXIT_CODE_TRANSFER_FAILED 4
#define EXIT_CODE_MEMORY 255

// To be implemented by the code unit that implements main():
//	static char * PROGRAM_NAME;
//
//	const char * get_program_name()
//	{
//		return PROGRAM_NAME;
//	}
//
//	int main(int argc, char** argv)
//	{
//		PROGRAM_NAME = argv[0];
//		...
//	}
extern const char * get_program_name();

#define errprintf(...) fprintf(stderr, __VA_ARGS__)
#define errflush() fflush(stderr)
#define errlnflush() { errprintf("\n"); errflush(); }
#define errprintprogprefix() errprintf("%s: ", get_program_name())
#define perror(...) { errprintprogprefix(); errprintf(__VA_ARGS__); errlnflush() }
#define pinfo(...) { errprintf(__VA_ARGS__); errlnflush(); }


char * malloc_and_cat(const char * left, const char * right);

#define malloc_pipe_prefix(name) malloc_and_cat(PIPE_PATH_PREFIX, name)

BOOL create_pipe(HANDLE* out_pipe, const char * path, BOOL inbound, BOOL outbound);
BOOL wait_for_pipe_client(HANDLE pipe);

// Note that this is implemented synchronously and will run until the input
// reaches EOF, the input is disconnected, or the output is disconnected (or
// an error occurs with a read or write). The streams must both be already
// open, and this function won't close them.
BOOL run_passthrough(HANDLE reader, HANDLE writer);

#endif
