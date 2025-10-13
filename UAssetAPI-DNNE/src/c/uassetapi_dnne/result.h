/*
MIT License

Copyright (c) 2025 Tuomo Kriikkula
Copyright (c) 2023 xnacly

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

------------------------------------------------------------------------------

The result.h component exposes utilities for a sophisticated error handling
approach, inspired by rust results. It allows for defining functions that
return CsResult instead of special values of their type that indicate failure,
such as -1 for numeric operations or an empty string for string operations.

Guide for using this component:

1. Define a function that returns a CsResult pointer:

  CsResult *squareRoot(double n) {
    if (n < 0) {
      return CsError(
          "can't compute square root of negative integer");
    }
    double *r = malloc(sizeof(double))
    *r = sqrt(n);
    if (r < 0) {
      return CsError("failed to compute square root");
    }
    return CsSuccess(r);
  }

(remember to allocate the value you want to escape to the heap and free it
afterwards)

2. Call this function

  CsResult* res = squareRoot(-5)

3. Use one of the following functions to extract the value of the
Result and thereby consume it:

  double res = *(double *) CsUnwrap(res)
  res = *(double *) CsExpect(res, "failed to compute sqrt")
  free(res)
*/

#ifndef UASSETAPI_DNNE_RESULT_H
#define UASSETAPI_DNNE_RESULT_H

#include <stdint.h>

#ifdef __cplusplus
#define UASSETAPI_DNNE_EXTERN_C extern "C"
UASSETAPI_DNNE_EXTERN_C
    {
#else
#define UASSETAPI_DNNE_EXTERN_C
#endif

typedef struct
{
    // If the Result holds no error, this contains the success value.
    void* value;
    // If the Result holds an error, this contains the error message.
    const char* error;
    // This indicates if the Result holds an error
    int8_t hasError;
} CsResult;

// Allocates a new Result, sets ->hasError to 0 and ->value to the given value.
CsResult* CsSuccess(void* value);

// Allocates a new Result, sets ->hasError to 1 and ->error to the given error.
CsResult* CsError(const char* error);

// Returns the value of the Result and destroys it. If the Result contains an
// error, the error message is printed to stderr and the process exists with
// EXIT_FAILURE.
void* CsUnwrap(CsResult* result);

// Returns the value of the Result and destroys it. If the Result contains an
// error, the provided error message is printed to stderr and the process exists
// with EXIT_FAILURE.
void* CsExpect(CsResult* result, const char* error);

// frees the allocated memory region for the given Result, sets the
// pointer to point to NULL
void CsFreeResult(CsResult* result);

#ifdef __cplusplus
}
#endif

#endif // UASSETAPI_DNNE_RESULT_H
