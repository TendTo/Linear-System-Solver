# Benchmarks

The following are the mean value found by running the specified implementation multiple times on the default matrix of the chosen size generated by application.

Times are expressed in milliseconds.  
The speedup refers to the corrisponding serial implementation.

### **Matrix size:** 1023 X 1024, for a total of 1047552 elements

| (serial) Intel i5-7600 CPU @ 3.50GHz | Gaussian elimination kernel | Solution vector kernel | TOT | Speedup |
| - | - | - | - | :-: |
| No pivot | / | / | 940.37 | / |
| Partial pivot | / | / | 1007.63 | / |

| (pocl) Intel i5-7600 CPU @ 3.50GHz | Gaussian elimination kernel | Solution vector kernel | TOT | Speedup |
| - | - | - | - | :-: |
| No pivot texture vec 4 | 1363.69 | 23.95 | 1387.64 | x0.7 |
| No pivot buffer | 357.83 | 17.42 | 375.25 | x2.5 |
| Partial pivot texture vec 4 | 3002.90 | 23.81 | 3026.71 | x0.3 |
| Partial pivot buffer | 3546.58 | 18.58 | 3565.16 | x0.3 |

| Nvidia GeForce GTX 1060 3GB | Gaussian elimination kernel | Solution vector kernel | TOT | Speedup |
| - | - | - | - | :-: |
| No pivot texture | 46.25 | 2.31 | 48.56 | x19.4 |
| No pivot texture vec 4 | 26.95 | 2.28 | 29.23 | x32.2 |
| No pivot buffer | 58.13 | 3.65 | 61.78 | x15.2 |
| No pivot buffer vec 4 | 39.36 | 3.91 | 43.27 | x21.7 |
| Partial pivot texture | 188.69 | 4.19 | 190.5 | x5.3 |
| Partial pivot texture vec 4 | 45.86 | 4.77 | 50.63 | x19.9 |
| Partial pivot buffer | 408.19 | 5.24 | 413.43 | x2.4 |
| Partial pivot buffer vec 4 | 60.01 | 5.14 | 65.15 | x15.5 |

---

### **Matrix size:** 2047 X 2048, for a total of 4192256 elements

| (serial) Intel i5-7600 CPU @ 3.50GHz | Gaussian elimination kernel | Solution vector kernel | TOT | Speedup |
| - | - | - | - | :-: |
| No pivot | / | / | 7462.43 | / |
| Parital pivot | / | / | 7442.95 | / |

| (pocl) Intel i5-7600 CPU @ 3.50GHz | Gaussian elimination kernel | Solution vector kernel | TOT | Speedup |
| - | - | - | - | :-: |
| No pivot texture vec 4 | 8748.75 | 139.74 | 8888.49 | x0.8 |
| No pivot buffer | 2891.91 | 96.99 | 2988.90 | x2.5 |
| Partial pivot texture vec 4 | 30451.27 | 137.12 | 30588.39 | x0.3 |
| Partial pivot buffer | 44296.28 | 100.44 | 44396.72 | x0.2 |

| Nvidia GeForce GTX 1060 3GB | Gaussian elimination kernel | Solution vector kernel | TOT | Speedup |
| - | - | - | - | :-: |
| No pivot texture | 303.44 | 9.10 | 330.39 | x22.6 |
| No pivot texture vec 4 | 171.56 | 10.33 | 181.89 | x41.0 |
| No pivot buffer | 390.51 | 10.54 | 401.05 | x18.6 |
| No pivot buffer vec 4 | 296.29 | 10.74 | 307.03 | x24.3 |
| Partial pivot texture | 2421.75 | 8.87 | 2430.62 | x3.1 |
| Partial pivot texture vec 4 | 365.82 | 9.23 | 355.05 | x21.0 |
| Partial pivot buffer | 6082.43 | 10.48 | 6092.91 | x1.2 |
| Partial pivot buffer vec 4 | 702.32 | 10.39 | 712.71 | x10.4 |

---

### **Results**

| Category | Implementation | Result |
| - | - | - |
| **Greater speedup** | (NVidia) No pivot texture vec 4 | x41.0 |
| **Smaller positive speedup** | (NVidia) Partial pivot buffer | x1.2 |
| **Biggest vectorization impact** | (NVidia) Partial pivot buffer vec 4 | x8.6 |
| **Worst speedup** | (pocl) Partial pivot buffer | x0.2 |

---

[Back to table of contents](Table-of-contents.md)