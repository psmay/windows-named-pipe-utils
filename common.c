
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

BOOL run_passthrough(HANDLE reader, HANDLE writer, unsigned long buffer_size)
{
    size_t _buffer_size = sizeof(char)*buffer_size;
	char* buffer = malloc(_buffer_size);

	DWORD readLen;
	DWORD wroteLen;
	BOOL result;
    BOOL final_result;

    char* current_buffer_pos = buffer;
    size_t max_allowed_read_size = _buffer_size;
	for(;;)
	{
		result = ReadFile(reader, current_buffer_pos, max_allowed_read_size, &readLen, NULL);

        if (!result) {
            int err = GetLastError();
            if(err == ERROR_BROKEN_PIPE) {
                // Client disconnected.
                // This is more like an EOF than an error.
                pinfo("Input stream disconnected.");

                // dump any leftover data on the buffer
                WriteFile(writer, buffer, current_buffer_pos - buffer, &wroteLen, NULL);
                FlushFileBuffers(writer);

                final_result = TRUE;
                break;
            }
            else {
                perror("Error reading from input stream; error code was 0x%08x.", err);
                break;
            }
        }

        // advance current_buffer_pos to end of read data, if we didn't read as much as we wanted we set new limit
        // with the remaining space on the buffer
        current_buffer_pos += readLen;
        if (readLen < max_allowed_read_size) {
            max_allowed_read_size -= readLen;
            continue;
        }
        result = WriteFile(writer, buffer, current_buffer_pos - buffer, &wroteLen, NULL);
        // Reset current_buffer_pos to start of buffer and max_allowed_read_size to full buffer size
        current_buffer_pos = buffer;
        max_allowed_read_size = _buffer_size;

        if(result) {
            FlushFileBuffers(writer);
            // continue
        }
        else {
            int err = GetLastError();
            if(err == ERROR_BROKEN_PIPE) {
                pinfo("Output stream disconnected.");
                final_result = TRUE;
                break;
            }
            else if(err == ERROR_NO_DATA) {
                pinfo("Output stream closed on receiving side.");
                final_result = TRUE;
                break;
            }
            else {
                perror("Error writing to output stream; error code was 0x%08x.", err);
                break;
            }
        }
	}

    free(buffer);
    return final_result;
}
