#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

static void put_err(const char* s) {
    int len = 0;
    while (s[len]) len++;
    write(2, s, len);
}

static void write_sum(int fd, float sum, int decimals) {
    if (decimals > 7) {
        decimals = 7;
    }
    char buf[64]; int len = 0;
    if (sum < 0) { buf[len++] = '-'; sum = -sum; }
    long whole = (long)sum;
    long scale = 1;
    for (int i = 0; i < decimals; i++) scale *= 10;
    long frac = (long)((float)(sum - (float)whole) * (float)scale + 0.5);
    if (frac == scale) { whole += 1; frac = 0; }
    char tmp[48]; int t = 0;
    if (whole == 0) tmp[t++] = '0';
    while (whole > 0) { tmp[t++] = (char)('0' + whole % 10); whole /= 10; }
    for (int i = t - 1; i >= 0; i--) buf[len++] = tmp[i];
    if (decimals > 0) {
        buf[len++] = '.';
        t = 0;
        for (int i = 0; i < decimals; i++) { tmp[t++] = (char)('0' + frac % 10); frac /= 10; }
        for (int i = t - 1; i >= 0; i--) buf[len++] = tmp[i];
    }
    buf[len++] = '\n';
    write(fd, buf, len);
}



int main(int argc, char* argv[]) {
    if (argc < 2) {
        put_err("usage: child filename\n");
        return 1;
    }
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        put_err("child: cannot open file\n");
        return 1;
    }
    float sum = 0.0f;
    char tok[64];
    int tlen = 0;
    char c;
    while (read(0, &c, 1) == 1) {
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
            if (tlen > 0) {                 /* токен закончился */
                tok[tlen] = '\0';
                sum += (float)atof(tok);
                tlen = 0;
            }
        } else if (tlen < 63) {
            tok[tlen++] = c;
        }
    }
    if (tlen > 0) {
        tok[tlen] = '\0';
        sum += (float)atof(tok);
    }
    write_sum(fd, sum, 6);
    close(fd);
    return 0;
}