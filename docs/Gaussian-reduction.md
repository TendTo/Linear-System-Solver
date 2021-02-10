# Gaussian reduction

## Complete explanation
[Wikipedia](https://en.wikipedia.org/wiki/Gaussian_elimination)

## Brief explanation

Gaussian reduction (also known as Gaussian elimination or row reduction) is an algorithm in linear algebra for solving a system of linear equations.  
To perform row reduction on a matrix, one uses a sequence of elementary row operations to modify the matrix until the lower left-hand corner of the matrix is filled with zeros, as much as possible. There are three types of elementary row operations:
- Swapping two rows
- Multiplying a row by a nonzero number
- Adding a multiple of one row to another row

If the system of equation is solvable and admits only one solution vector, the matrix can be reduced to an upper triangular matrix.  
Once in this form, the system can be solved easily with some substitutions.

---

## OpenCL implementations

The challenge of an OpenlCL implementation arise from the fact that to set to zero all the values below the pivot, it is necessary to do some calculation depending on the pivot row.  
The issue is that this process is to be repeated "rows" times, and the pivot row has to be the one calculated with the last iteration.  
Since there is no way to force syncronization in the global memory, the only two options I see are to either launch "rows - 1" kernel plus the one to finalize the result vector or, alternatively, to store the whole matrix in local memory.

### **No pivot naive**

Simplest implementation of a Gaussian reduction where the pivots are chosen in based on the order of rows.

**Time complexity: O(n^2)**  
**Spacial complexity: O(n^2)**

| :heavy_check_mark: Pro | :x: Cons |
| - | - |
| The theory behind is easy to grasp | Not efficient at all |
| The solution, when found, is as exact as it can get | The "no pivot" version may not be able to solve some solvable systems |
| | Then dimension of the matrix is limited by the amount of local memory available to the work-group |

---

[Back to table of contents](Table-of-contents.md)