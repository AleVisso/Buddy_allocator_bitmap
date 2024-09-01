#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#pragma once

//int free_mmap(void* ptr,int memoria);
void free_mmap(void* ptr,int memoria);
//int init_mmap(int memoria);
void init_mmap(int memoria);
