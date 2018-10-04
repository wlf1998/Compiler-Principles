# PL/0 language compiler

This is the developing project for the compiler of language PL/0, which is a light-weight programming language.

## BUILD

This project is managed by `cmake`, and the mininum version required is CMake 2.8. Follow the ordinary steps to build this project.

### 1 Generate Makefile
It is strongly recommended to run `do_cmake.sh`, which does everything automatically.
```
./do_cmake.sh
```

If manual operations are needed, try the following steps:
```
mkdir build
cd build
cmake ..
```

### 2 Compilation
If you use `do_cmake.sh` in the last step, change working directory to `build/` with:
```
cd build/
```

Now there should be `Makefile` in the current directory. Build the project with `make`:
```
make
```

Done!
