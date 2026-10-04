#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

static void put_err(const char* s) {
    int len = 0;
    while (s[len]) len++;
    write(2, s, len);
}

static int read_line(int fd, char* buf, int cap) {
    int len = 0;
    char c;
    while (len < cap - 1 && read(fd, &c, 1) == 1) {
        if (c == '\n') break;
        buf[len++] = c;
    }
    buf[len] = '\0';
    return len;
}

int main() {
    char fname[256];
    if (read_line(0, fname, sizeof(fname)) <= 0) {
        put_err("parent: expected filename on first line\n");
        return 1;
    }
    int p[2];
    if (pipe(p) == -1) {
        put_err("parent: pipe failed\n");
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        put_err("parent: fork failed\n");
        return 1;
    }
    if (pid == 0) {
        close(p[1]);
        if (dup2(p[0], 0) == -1) {
            put_err("child branch: dup2 failed\n");
            _exit(1);
        }
        close(p[0]);
        execl("./child", "child", fname, (char *)NULL);
        put_err("parent: exec ./child failed\n");
        _exit(1);
    }
    close(p[0]);
    char buf[4096];
    ssize_t n;
    while ((n = read(0, buf, sizeof(buf))) > 0) {
        if (write(p[1], buf, (size_t)n) != n) {
            put_err("parent: write to pipe failed\n");
            break;
        }
    }
    close(p[1]);
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        put_err("parent: waitpid failed\n");
        return 1;
    }
    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        if (code != 0) {
            put_err("parent: child reported error\n");
            return code;
        }
        return 0;
    }
    put_err("parent: child terminated abnormally\n");
    return 1;
}