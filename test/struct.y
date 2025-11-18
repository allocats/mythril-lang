struct MyStruct {
    i32 a;
    i32 b;
}

MyStruct {
    // All structs are intialized to zero, this leads to 0 * 2 
    fn i32 do_nothing(&self) {
        my_struct -> a *= 2;
        return my_struct -> a;
    }

    fn i32 do_something(&self) {
        my_struct -> b += 2;
        return my_struct -> b;
    }
}

fn void main(i32 argc, char* argv[]) {
    MyStruct my_struct;
    
    i32 a = my_struct.do_nothing();
    i32 b = my_struct.do_something();

    print("a={} b={}", a, b);
}
