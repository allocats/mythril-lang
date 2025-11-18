fn void va_args_func(...[u32 n: 5]) {
    for i = 0; i < n; i++ {
        println("Arg {}: found {}", i, va_args[i]);
    }
}
