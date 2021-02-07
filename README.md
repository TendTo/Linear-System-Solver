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
- [gcc](https://gcc.gnu.org/) or an equivalent c compiler
- [An OpenCL sdk](https://www.khronos.org/opencl/)

#### Steps
-   ```bash
    make main
    ```
- The executable will be found in the bin folder, while all the object files will be in the build folder

## :bar_chart: _[Optional]_ Testing

#### Requirements
- [check](https://libcheck.github.io/check/)

#### Steps
-   ```bash
    make test
    ```
- The test runner will be found in the bin folder, while all the object files will be in the build folder

## :books: Documentation
Check the gh-pages branch

[Link to the documentation](https://tendto.github.io/system-solver/)

## Credits
- [Giuseppe Bilotta](https://github.com/Oblomov) for the _ocl_boiler.h_ (and for teaching me all the OpenCL fundamentals)
- [c-project-template](https://github.com/tiborsimon/c-project-template) for the structure of the project and of the Makefile