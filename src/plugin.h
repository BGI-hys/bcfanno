/*  
    Copyright (C) 2016,2017  BGI Research

    Author: Shi Quan (shiquan@genomics.cn)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE. 
*/

#ifndef VCFANNO_PLUGIN_HEADER
#define VCFANNO_PLUGIN_HEADER

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <htslib/vcf.h>
#include <dlfcn.h>
#include "anno.h"

struct header_cols {
    const char *key;
    int col; // column number in the process() returned
    const char *hdr_line;
};

// process function only return a point to the results structure. Do NOT free it anyway.
// To free this structure, just refer to final() at the last.
struct process_results {
    int num_fields;
    int n_row, m_row;
    char **data;
};

// init function 
// input: columns, column number
// output: column struct
// return state
typedef void* (*dl_init_func)(char*, int *); 
// about function
// return short description
typedef char * (*dl_about_func)(void);
// test function
// only test the plugion works
typedef int (*dl_test_func)(void);
// process function
// return information on this position
typedef void *(*dl_process_func)(char *chrom, int start, int end);
// final function
// close and release memory
typedef int (*dl_final_func)(void);

struct plugin_funcs {
    dl_init_func init;
    dl_about_func about;
    dl_process_func process;
    dl_final_func final;
};

struct plugin_spec {
    int ncols;
    struct anno_col *cols;
    struct plugin_funcs funcs;
};

struct plugin_specs {
    int n, m;
    struct plugin_spec *specs;
};

extern int plugins_init(const char *name, struct plugin_spec *spec);
extern bcf1_t* plugins_process(bcf_hdr_t *hdr, bcf1_t *line);
extern void close_plugins();

#endif
