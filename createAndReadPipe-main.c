
#include "common.h"

static char * PROGRAM_NAME;

const char * get_program_name()
{
	return PROGRAM_NAME;
}

int main(int argc, char** argv)
{
	PROGRAM_NAME = argv[0];

	if(argc < 2 || argc > 3) {
		pinfo("Usage: %s PIPENAME", get_program_name());
		pinfo("Creates a named pipe at %sPIPENAME and reads it to stdout.", PIPE_PATH_PREFIX);
		return EXIT_CODE_USAGE;
	}

    unsigned long buffer_size = 1024*1024*10; // 10 MiB
    if (argc == 3) {
        buffer_size = strtoul(argv[2], NULL, 10);
    }

	HANDLE pipe, outh;
	BOOL is_connected;
	int exit_code;
	char * pipe_path = malloc_pipe_prefix(argv[1]);

	// Get stdout for output.
	outh = GetStdHandle(STD_OUTPUT_HANDLE);

	if(pipe_path == NULL) {
		perror("Out of memory.");
		return EXIT_CODE_MEMORY;
	}

	if(!create_pipe(&pipe, pipe_path, TRUE, FALSE)) {
		return EXIT_CODE_CREATE_FAILED;
	}

	free(pipe_path);

	pinfo("Waiting for client...");

	is_connected = wait_for_pipe_client(pipe);

	if(is_connected) {
		pinfo("Connected.");

		if(run_passthrough(pipe, outh, buffer_size)) {
			exit_code = 0;
		}
		else {
			exit_code = EXIT_CODE_TRANSFER_FAILED;
		}
	}
	else {

		exit_code = EXIT_CODE_CONNECT_FAILED;
	}


	pinfo("Closing stdout.");
	CloseHandle(outh);
	pinfo("Closing pipe.");
	CloseHandle(pipe);

}
