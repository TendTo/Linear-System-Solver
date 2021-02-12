# System Solver
Small application that aims to solve systems of linear equations using the general compute capabilities of the GPUs

## Table of contents

- **[:wrench: Setting up a local istance](#wrench-setting-up-a-local-istance)**
- **[:bar_chart: _\[Optional\]_ Setting up testing](#bar_chart-optional-testing)**
- **[:books: Documentation](#books-documentation)**
- **[Credits](#credits)**

---

## :wrench: Setting up a local istance

#### Requirements
- [POSIX](https://en.wikipedia.org/wiki/POSIX) compliant environment
- [gcc](https://gcc.gnu.org/) or an equivalent c compiler
- [An OpenCL sdk](https://www.khronos.org/opencl/)

#### Steps
- Make sure the path to the CL header is correct, or edit it accordingly.

-   ```bash
    make main
    ```
- The executable will be found in the bin folder, while all the object files will be in the build folder
- You can also run the compiler manually. All the c files are in the _src_ folder, while the headers are in the _include_ folder.

## :bar_chart: _[Optional]_ Testing

#### Requirements
- [check](https://libcheck.github.io/check/)

#### Steps
- Make sure the path to the check header is correct, or edit it accordingly.

-   ```bash
    make test
    ```
- The test runner will be found in the bin folder, while all the object files will be in the build folder

## :books: Documentation
[Table of contents](docs/Table-of-contents.md)  
[Code documentation](https://tendto.github.io/Linear-System-Solver/index.html)

## Credits
- [Giuseppe Bilotta](https://github.com/Oblomov) for the _ocl_boiler.h_ (and for teaching me all the OpenCL fundamentals and more)
- [c-project-template](https://github.com/tiborsimon/c-project-template) for the structure of the project and of the Makefile