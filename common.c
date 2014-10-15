
#include "common.h"

char * malloc_and_cat(const char * left, const char * right)
{
	size_t left_len = strlen(left);
	size_t right_len = strlen(right);

	char * buffer = malloc(left_len + right_len + 1);
	char * cursor = buffer;

	if(buffer != NULL) {
		memcpy(cursor, left, left_len);
		cursor += left_len;

		memcpy(cursor, right, right_len);
		cursor += right_len;

		*cursor = 0;
		++cursor;
	}

	return buffer;
}

#define NAME_OF_ACCESS_MODE(m) ( \
	(m == PIPE_ACCESS_INBOUND) ? "read" : \
	(m == PIPE_ACCESS_OUTBOUND) ? "write" : \
	(m == PIPE_ACCESS_DUPLEX) ? "read and write" : \
	"(unknown)" \
	)

BOOL create_pipe(HANDLE* out_pipe, const char * path, BOOL inbound, BOOL outbound)
{
	DWORD direction;

	if(inbound) {
		direction = outbound ? PIPE_ACCESS_DUPLEX : PIPE_ACCESS_INBOUND;
	}
	else if(outbound) {
		direction = PIPE_ACCESS_OUTBOUND;
	}
	else {
		perror("Pipe must be inbound, outbound, or both");
		return FALSE;
	}

	*out_pipe = CreateNamedPipe(
		path,
		direction,
		PIPE_TYPE_BYTE,
		1, // max instances
		0, // out buffer size
		0, // in buffer size
		0, // default timeout
		NULL // security attributes
		);

	if(*out_pipe == INVALID_HANDLE_VALUE) {
		int err = GetLastError();

		if(err == ERROR_ACCESS_DENIED) {
			perror("Could not open pipe '%s': access was denied.", path);
		}
		else {
			perror("Could not open pipe '%s'; error code was 0x%08x.", path, err);
		}
		return FALSE;
	}

	pinfo("Successfully created %s for %s.", path, NAME_OF_ACCESS_MODE(direction));
	return TRUE;
}

BOOL wait_for_pipe_client(HANDLE pipe)
{
	if(ConnectNamedPipe(pipe, NULL)) {
		return TRUE;
	}

	int err = GetLastError();

	if(err == ERROR_PIPE_CONNECTED) {
		// Not actually an error
		return TRUE;
	}

	perror("Error waiting for client; error code was 0x%08x.", err);
	return FALSE;
}

BOOL run_passthrough(HANDLE reader, HANDLE writer)
{
	char buffer[PASSTHROUGH_BUFFER_SIZE];

	DWORD readLen;
	DWORD wroteLen;
	BOOL result;

	for(;;)
	{
		result = ReadFile(reader, buffer, sizeof(buffer), &readLen, NULL);

		if(result) {
			result = WriteFile(writer, buffer, readLen, &wroteLen, NULL);

			if(result) {
				FlushFileBuffers(writer);
				// continue
			}
			else {
				int err = GetLastError();
				if(err == ERROR_BROKEN_PIPE) {
					pinfo("Output stream disconnected.");
					return TRUE;
				}
				else if(err == ERROR_NO_DATA) {
					pinfo("Output stream closed on receiving side.");
					return TRUE;
				}
				else {
					perror("Error writing to output stream; error code was 0x%08x.", err);
					return FALSE;
				}
			}
		}
		else {
			int err = GetLastError();
			if(err == ERROR_BROKEN_PIPE) {
				// Client disconnected.
				// This is more like an EOF than an error.
				pinfo("Input stream disconnected.");
				return TRUE;
			}
			else {
				perror("Error reading from input stream; error code was 0x%08x.", err);
				return FALSE;
			}
		}
	}
}
