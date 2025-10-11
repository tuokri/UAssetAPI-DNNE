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
*/

#include <stdio.h>
#include <stdlib.h>

#include "uassetapi_dnne/result/result.h"

CsResult* CsSuccess(void* value)
{
    CsResult* result = NULL;
    if (result = malloc(sizeof(CsResult)), result == NULL)
    {
        fprintf(stderr, "UAssetAPI-DNNE-result: failed to allocate result for success\n");
        exit(EXIT_FAILURE);
    };
    result->value = value;
    result->hasError = 0;
    return result;
}

CsResult* CsError(const char* error)
{
    CsResult* result = NULL;
    if (result = malloc(sizeof(CsResult)), result == NULL)
    {
        fprintf(stderr, "UAssetAPI-DNNE-result: failed to allocate result for error\n");
        exit(EXIT_FAILURE);
    };
    result->error = error;
    result->hasError = 1;
    return result;
}

void* CsExpect(CsResult* result, const char* error)
{
    if (result == NULL)
        return NULL;
    if (result->hasError)
    {
        CsFreeResult(result);
        fprintf(stderr, "UAssetAPI-DNNE-result: %s\n", error);
        exit(EXIT_FAILURE);
    }
    void* value = result->value;
    CsFreeResult(result);
    return value;
}

void* CsUnwrap(CsResult* result)
{
    if (result == NULL)
        return NULL;
    if (result->hasError)
    {
        fprintf(stderr, "UAssetAPI-DNNE-result: %s\n", result->error);
        CsFreeResult(result);
        exit(EXIT_FAILURE);
    }
    void* value = result->value;
    CsFreeResult(result);
    return value;
}

void CsFreeResult(CsResult* result)
{
    if (result == NULL)
    {
        return;
    }
    free(result);
    result = NULL;
}
