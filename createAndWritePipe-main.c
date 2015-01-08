
#include "common.h"

static char * PROGRAM_NAME;

const char * get_program_name()
{
	return PROGRAM_NAME;
}

int main(int argc, char** argv)
{
	PROGRAM_NAME = argv[0];

	if(argc != 2) {
		pinfo("Usage: %s PIPENAME", get_program_name());
		pinfo("Creates a named pipe at %sPIPENAME and write to it from stdin.", PIPE_PATH_PREFIX);
		return EXIT_CODE_USAGE;
	}

	HANDLE pipe, inh;
	BOOL is_connected;
	int exit_code;
	char * pipe_path = malloc_pipe_prefix(argv[1]);

	if(pipe_path == NULL) {
		perror("Out of memory.");
		return EXIT_CODE_MEMORY;
	}

	// Get stdin for input.
	inh = GetStdHandle(STD_INPUT_HANDLE);

	if(!create_pipe(&pipe, pipe_path, FALSE, TRUE)) {
		return EXIT_CODE_CREATE_FAILED;
	}

	free(pipe_path);

	pinfo("Waiting for client...");

	is_connected = wait_for_pipe_client(pipe);

	if(is_connected) {
		pinfo("Connected.");

		if(run_passthrough(inh, pipe)) {
			exit_code = 0;
		}
		else {
			exit_code = EXIT_CODE_TRANSFER_FAILED;
		}
	}
	else {

		exit_code = EXIT_CODE_CONNECT_FAILED;
	}


	pinfo("Closing stdin.");
	CloseHandle(inh);
	pinfo("Closing pipe.");
	CloseHandle(pipe);

}
