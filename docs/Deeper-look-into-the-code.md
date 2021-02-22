# Deeper look into the code

### **Introduction**
This section is meant to give an overview on the code used in the various implementations of the Gaussian elimination, tring to exapain more cleary what each piece of code is supposed to do.  

The **lmem** implementation will not be shown since it is the simplest one. It was used to get some ideas on how to approach the problem, but it is since been deprecated.

### **General principles**
- **Memory object ping-pong:** since each iteration of the Gaussian elimination algorithm depens on the iteration before, data needs to be shynchronized.   
To achieve this even for very large matrices, the solution is to use two memory objects that will be swapped at each iteration. One will provide the input, the other will store the output.  
This also means that all the fully reduced even rows will be on one memory objects, while the odd ones will be on the other.  
The reason is that in the elimination phase only one of the two mem-obj is used to write only the lines that have been modified, and then the two are inverted in the next.  
Let's put ourselves in the case with no pivot for simplicity, and call j1 the first mem-obj, which currently contains the entire matrix, and j2 the second, which contains nothing.  
At the end of the first iteration, only a submatrix is ​​written to j2, which contains one row and one column less than the original matrix and which starts from index (1,1). The pivot row is excluded.  
At the end of the second iteration, a submatrix with two fewer rows and columns than the original one will be overwritten on j1, starting from index (2,2), leaving out the new pivot row, index 1. And so on.  
Since the rows that make up the matrix in echelon form are precisely the various pivot rows found in the previous steps, it follows that the "final" rows will be found only on j1 ​​or j2, depending on whether they were even or odd.  
An exeption to this rules occurs in the **partial pivot** versions, since there two rows are carried over to the output memory object, meaning that the final row will be on the same location as the previous row.

- **Local memory size in solve kernels:** in the solve kernels, the local memory is actually divided into two "logical" sections, and the offset between one and the other is exactly _rows_, that is the number of rows (and therefore of solutions) of the system. The first _rows_ positions are used for the reduction, the last _rows_ contain the results, as they are calculated.  
The local memory, in total, contains _2rows_ elements.

- **Sliding window:** this means that all the work-items in a work-group are able to work on a vector that has more elements than the numer of work-items in the work-group.  
The method to achieve this is simple: let us assume say the local work size is **n** and the number of elements to elaborate is **l * n**.  
With the sliding window, the first **n** elements are evaluated, and then the next **n** and so on, **l** times.  
If **l** is not an integer, it is better to round the number of elements to the next multiple of **n** bigger than **l * n**, to make sure all work-items meets all the local memory fences.

Only two exaples will be shown, since most of the code is very similar in all the implementations and those two are the most significative.

### **No pivot Gaussian elimination texture**
<table>
<tr>
<th>Code</th>
<th>Explanation</th>
</tr>
<tr>
<td>

```c
const int col = get_global_id(0) + pivot + 1;
const int row = get_global_id(1) + pivot + 1;
const int cols = get_image_width(in_U);
const int rows = get_image_height(in_U);
```

</td>
<td>

**Initial constants:**  
on its first lauch, the kernel has (**cols - 1** X **rows - 1**) work-items.  
This kernel is launched **n - 1** times and with each iteration the number of work-item in each dimention decreases br y one.  
The pivot's value, on the other hand, starst at 0 and increases by one each time.

</td>
</tr>
<tr>
<td>

```c
float pivot_val = read_imagef(in_U, s, (int2)(pivot, pivot)).s0;
float head_row_val = read_imagef(in_U, s, (int2)(pivot, row)).s0;
float mult = -head_row_val / pivot_val;

float pivot_row_val = read_imagef(in_U, s, (int2)(col, pivot)).s0;
float old_val = read_imagef(in_U, s, (int2)(col, row)).s0;
float new_val = old_val + pivot_row_val * mult;
write_imagef(out_U, (int2)(col, row), (float4)(new_val, 0.0f, 0.0f, 1.0f));
```

</td>
<td>

**Row simplification:**  
in this version, each pixel of the texture contains exactly one value of the matrix, so, even though a float4 is read, only the first value is needed.

Each work-item reads the value at the pivot position, *pivot_val* and the value on the position that will be set to 0, *head_ro_val*.  
With those two, *mult* can be calculated (mind the minus sign).  

Finally, *pivot_row_val*, which is on the *pivot_row* but in the same column as the work-item is multipied with by the *mult* and the result is added to the current *old_val*.  
The *new_value* found will replace the old one and will be written in the output texture.

</td>
</tr>
</table>

### **No pivot solve texture**
<table>
<tr>
<th>Code</th>
<th>Explanation</th>
</tr>
<tr>
<td>

```c
const int cols = get_image_width(even_U);
const int rows = get_image_height(even_U);

const int li = get_local_id(0);
const int n_li = get_local_size(0);
const int rows_li = ((rows + n_li - 1) / n_li) * n_li;

//For each row of the matrix, starting from the last one...
for (int row = rows - 1; row >= 0; --row) { 
  ...
```

</td>
<td>

**Initial constants:**  
the kernel contains **n** work-items.  
The local work size can be altered at will, but bigger size should lead to a faster execution.  

*rows_li* is used to ensure that all work-items reach the various barriers inside the for loop.

The following code is execuded **n** times to calculate all **n** solutions.

</td>
</tr>
<tr>
<td>

```c
//Sliding window
for (int col = li; col < rows_li; col += n_li) {
  //If the col index is in the range of interest
  if (col > row && col < rows) {
    //The texture from which to get the coefficient depends on whether
    //the row index is even or odd
    float coefficient = (row & 1) ? read_imagef(odd_U, s, (int2)(col, row)).s0 
                                  : read_imagef(even_U, s, (int2)(col, row)).s0;
    lmem[col] = lmem[rows + col] * coefficient;
  }
}
barrier(CLK_LOCAL_MEM_FENCE);
```

</td>
<td>

**Filling the local memory:**  
the for loop implements a sliding window, so that to the work-items in the work-group will be assigned each possible column value.  

Then, only if the column is in the correct range, the corrisponding spot in the local memory is filled.  
The input texture from which the value will be taken depends on whethre or not the row is odd.

</td>
</tr>
<tr>
<td>

```c
//Number of cols to the right
int cols_remaining = rows - 1 - row;
int is_odd = cols_remaining & 1;
//Until the number of cols to the right divided by 2 each time is greater than 0...
for (int i = cols_remaining >> 1; i > 0; i = i >> 1) {
  // Sliding window
  for (int col = li; col < rows_li; col += n_li) {
    if (col < i) {
      //Calculate the correct index based on the current row
      int temp_col = row + 1 + col;
      lmem[temp_col] += lmem[temp_col + i];
      //Compensate for an odd number of values to sum
      if (col == i - 1 && is_odd)
        lmem[temp_col] += lmem[temp_col + i + 1];
    }
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  is_odd = i & 1;
}
```

</td>
<td>

**Summing up the values:**  
the next steps involve summing all the values from all the columns with index greater of the current *row*, ignoring the last one.  
The result is to be stored in local memory, at the position *row* + 1.

To achieve this in a O(log(n)) time complexity, in each iteration *i* work-items with an assigned index *col* < *i* calculates the sum between the element *col* element and the *i + col* element.  
The value of *i* is then halved.
In the case that the number of elements to sum was not even, the last element wil be added by the last work-item.
</td>
</tr>
<tr>
<td>

```c
//Sliding window
for (int col = li; col < rows_li; col += n_li) {
  //Only the diagonal col 
  if (col == row) {
    float coefficient;
    float sum;
    if (row & 1) {
      coefficient = read_imagef(odd_U, s, (int2)(col, row)).s0;
      sum = read_imagef(odd_U, s, (int2)(cols - 1, row)).s0;
    } else {
      coefficient = read_imagef(even_U, s, (int2)(col, row)).s0;
      sum = read_imagef(even_U, s, (int2)(cols - 1, row)).s0;
    }
    sum -= lmem[row + 1];
    //Storing the result value
    lmem[rows + col] = sum / coefficient;
  }
}
barrier(CLK_LOCAL_MEM_FENCE);
```

</td>
<td>

**Calculating the result:**  
finally, the row-th unknown is calculated, retrieving the needed values from the correct texture, and storing the result in local memory, with an offset of *rows* position to avoid overlapping with the part of the local memory used for the calculation of the sums.

</td>
</tr>
</table>

---

### **Partial pivot Gaussian elimination buffer vec 4**
<table>
<tr>
<th>Code</th>
<th>Explaination</th>
</tr>
<tr>
<td>

```c
//Used to access single values in the buffers
global read_only const double *s_in_U = in_U;
global write_only double *s_out_U = out_U;

const int pivot_quarts = pivot / 4;
const int col = get_global_id(0) + pivot / 4;
const int row = get_global_id(1) + pivot + 1;
const int cols = cols_quarts * 4;

const int li = get_local_id(1) * get_local_size(0) + get_local_id(0);
const int n_li = get_local_size(0) * get_local_size(1);
```

</td>
<td>

**Initial constants:**  
on its first lauch, and since wher are using the vectorized version, the kernel has (**(cols - 1) / 4** X **rows - 1**) work-items.  
This kernel is launched **n - 1** times and with each iteration the number of work-item in the second dimension decreases by one, while in the other dimensions does so only every four iterations.  
The pivot's value, on the other hand, starst at 0 and increases by one each time.

</td>
</tr>
<tr>
<td>

```c
//Sliding window
for (int i = li; pivot + i < rows; i += n_li) {
  lmem[i] = fabs(s_in_U[(pivot + i) * cols + pivot]);
  i_lmem[i] = i + pivot;
}
barrier(CLK_LOCAL_MEM_FENCE);
```

</td>
<td>

**Filling the local memory:**  
this step fills both the *lmem* buffer and the *i_lmem* buffer with the values contained in the rows under the *pivot* and their row index, respectively.

</td>
</tr>
<tr>
<td>

```c
//Number of rows below the pivot
int rows_remaining = rows - pivot;
int is_odd = rows_remaining & 1;
//Until the number of row below divided by 2 each time is greater than 0...
for (int i = rows_remaining >> 1; i > 0; i = i >> 1) {
  //Sliding window
  for (int temp_col = li; temp_col < i; temp_col += n_li) {
    int index_max = i_lmem[temp_col];
    int old_index_max = index_max;
    double old_max = lmem[temp_col];
    double new_max = lmem[temp_col + i];
    //If the value in the position temp_col + i is greater
    if (new_max > old_max) {
      old_max = new_max;
      index_max = i_lmem[temp_col + i];
    }
    if (li == i - 1 && is_odd) {
      double odd_last_max = lmem[temp_col + i + 1];
      //If the value in the last position is greater and i was odd
      if (odd_last_max > old_max) {
        old_max = odd_last_max;
        index_max = i_lmem[temp_col + i + 1];
      }
    }
    //If a new max was found, update the local memory
    if (index_max != old_index_max) {
      lmem[temp_col] = old_max;
      i_lmem[temp_col] = index_max;
    }
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  is_odd = i & 1;
}
//Retrieve the index of the best pivot
int pivot_row = i_lmem[0];
```

</td>
<td>

**Finding the best pivot:**  
the next steps involve finding the row below the *pivot* that contains the greatest absolute value.  
The index of this *row* is to be stored in local memory, at the position *row* + 1.

To achieve this in a O(log(n)) time complexity, in each iteration *i* work-items with an assigned index *col* < *i* stores the greater element between the one with index *col* and the one of index *i + col*.  
The value of *i* is then halved.
In the case that the number of elements to check was not even, the last element wil be considered by the last work-item.

</td>
</tr>
<tr>
<td>

```c
//Read the vectorized values
double4 old_val;
double head_row_val;
//The work-items in the row of the pivot_row calculate the
//simplified row of index pivot instead
if (row != pivot_row) {
  head_row_val = s_in_U[row * cols + pivot];
  old_val = in_U[row * cols_quarts + col];
} else {
  head_row_val = s_in_U[pivot * cols + pivot];
  old_val = in_U[pivot * cols_quarts + col];
}
double pivot_val = s_in_U[pivot_row * cols + pivot];
double mult = -head_row_val / pivot_val;

double4 pivot_row_val = in_U[pivot_row * cols_quarts + col];
double4 new_val = old_val + pivot_row_val * (double4)(mult);
out_U[row * cols_quarts + col] = new_val;

//Copy the pivot row on the output
if (row == pivot + 1) {
  out_U[pivot * cols_quarts + col] = pivot_row_val;
  if (col == pivot + 1)
    s_out_U[pivot * cols + pivot] = pivot_val;
}
```

</td>
<td>

**Finding the best pivot:**  
in this version, each element of the input buffer contains 4 double.

Each work-item reads the value at the pivot position, *pivot_val* and the value on the position that will be set to 0, *head_ro_val*  
With those two, *mult* can be calculated (mind the minus sign).  

Then, *pivot_row_val*, which is on the *pivot_row* but in the same column as the work-item is multipied with by the *mult* and the result is added to the current *old_val*, in a vectorized operation.  
The *new_value* found will replace the old one and will be written in the output texture.  
In addition, the pivot row is also written in the ouput buffer, to simulate the possible swap that may have occurred.

</td>
</tr>
</table>

---

[Back to table of contents](Table-of-contents.md)
