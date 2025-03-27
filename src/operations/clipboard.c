#include "clipboard.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../include/enums.h"
#include <sys/wait.h>

void copy_to_clipboard(Clipboard *cb, const char *path, OperationType type) {
    strncpy(cb->source, path, MAX_PATH_LEN-1);
    cb->op_type = type;
}

int paste_from_clipboard(Clipboard *cb, const char *dest) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("cp", "cp", "-r", cb->source, dest, NULL);
        exit(EXIT_FAILURE);
    }
    waitpid(pid, NULL, 0);
    return 0;
}

void clear_clipboard(Clipboard *cb) {
    memset(cb->source, 0, MAX_PATH_LEN);
    cb->op_type = OP_COPY;
}