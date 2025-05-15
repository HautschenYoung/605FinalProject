# 605FinalProject
## How to set up the environment
This project does not depend on any external libraries. It only requires c++ and CUDA compilers. Note that the c++ compiler must support modern c++ features, since key word `auto` is used in this project.

Another notable thing is that when compiling CUDA files, i.e. running the `Makefile`, you may need to modify the line `GENCODE_FLAGS  = -gencode arch=compute_89,code=sm_89` in `Makefile`. The part `arch=compute_89,code=sm_89` is related to your local GPU version. My GPU
is NVIDIA GeForce RTX 4070 Laptop, so the corresponding arch and code postfix is 89. If your GPU version is older, this postfix may not work for your machine and you need to find your proper postfix.

## How to run the project
The repository has 3 branches: 

the main branch is the single-threading implementation; 

the multithread branch is the multi-threading implementation; 

the cuda branch is the CUDA implementation.

Please switch to corresponding branch to run the related codes.

Once on the right branch, compilation is very easy: simply run
```
make
```
in the terminal.

Then there will be an executable in the directory. For CPU implementations, it is named `rt`, whereas for CUDA implementation, it is named `cudart`.

These executables can receive only 1 parameter that indicates samples per pixel (SPP).

You can run it by typing
```
./rt 20
./cudart 20
```
Note that the default value for spp is 10, if you do not provide any parameter.

Once execution is done, there will be a `.ppm` file in the directory, which is the rendered result. 

Since most Linux OS defaultly installed the `ImageMagick` tool, you can display the `.ppm` file by terminal instruction:
```
display *.ppm
```
Or you can convert `.ppm` file to a `.png` file by terminal instruction:
```
convert *.ppm *.png
```

## How to get the measurement results
For CPU implementations, CPU time and CPU memory usage will directly be printed to the terminal. 

For CUDA implementation, to get measurement results, run
```
nvprof --metrics achieved_occupancy,inst_executed,inst_fp_32,inst_fp_64,inst_integer ./cudart
```
, since the measurement results are printed by `cerr` instead of normal `cout`.