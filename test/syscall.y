const u32 SYS_READ  = 0;
const u32 SYS_WRITE = 1;
const u32 SYS_OPEN  = 2;
const u32 SYS_CLOSE = 3;
const u32 SYS_EXIT  = 60;

fn i64 write(i32 fd, char* buffer, usize count) {
    return syscall(SYS_WRITE, fd, buffer, count, 0, 0, 0);
}

fn void exit(i32 code) {
    syscall(SYS_EXIT, code, 0, 0, 0, 0, 0);
}
