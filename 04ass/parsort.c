#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int compare_i64 (void const *left, void const *right) {
  int64_t l = *(const uint64_t*) left;
  int64_t r = *(const uint64_t*) right;
  if (l < r) return -1;
  if (l > r) return 1;
  return 0;
}

// Merge the elements in the sorted ranges [begin, mid) and [mid, end),
// copying the result into temparr.
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) break;

    if (at_end_l)
      *dst++ = *right++;
    else if (at_end_r)
      *dst++ = *left++;
    else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0)
        *dst++ = *left++;
      else
        *dst++ = *right++;
    }
  }
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  int64_t mid = (end + begin) / 2;
  // if number of elements is <= threshold, sort sequentially
  if ((end - begin) <= threshold) { //?? + 1
    qsort(arr+begin, end-begin, sizeof(int64_t), compare_i64); // using comparator
    return;
  } else {
    // child process for left half
    pid_t pid_l = fork();
    if (pid_l == -1) {
      fprintf(stderr, "Error: fork failed to start a new process");
      exit(-1);
    } else if (pid_l == 0) { // now in the child process
      merge_sort(arr, begin, mid, threshold); // sort left half
      exit(0); // exit child process
    }

    // child process for right half
    pid_t pid_r = fork();
    if (pid_r == -1) {
      fprintf(stderr, "Error: fork failed to start a new process\n");
      pid_t actual_pid_l = waitpid(pid_l, NULL, 0); // pick up the left child process to prevent leak
      exit(-1);
    } else if (pid_r == 0) { // now in the child process
      merge_sort(arr, mid, end, threshold); // sort right half
      exit(0); // exit child process
    }

    // wait for left child process to complete
    int wstatus_l;
    pid_t actual_pid_l = waitpid(pid_l, &wstatus_l, 0); 
    if (actual_pid_l == -1) {
      fprintf(stderr, "Error: waitpid failure");
      exit(-1);
    }
    if (!WIFEXITED(wstatus_l)) {
      fprintf(stderr, "Error: subprocess crashed, was interrupted, or did not exit normally\n");
      exit(-1);
    }
    if (WEXITSTATUS(wstatus_l) != 0) {
      fprintf(stderr, "Error: subprocess returned a non-zero exit code\n");
      exit(-1);
    }

    // wait for right child process to complete
    int wstatus_r;
    pid_t actual_pid_r = waitpid(pid_r, &wstatus_r, 0);
    if (actual_pid_r == -1) {
      fprintf(stderr, "Error: waitpid failure\n");
      exit(-1);
    }
    if (!WIFEXITED(wstatus_r)) {
      fprintf(stderr, "Error: subprocess crashed, was interrupted, or did not exit normally\n");
      exit(-1);
    }
    if (WEXITSTATUS(wstatus_r) != 0) {
      fprintf(stderr, "Error: subprocess returned a non-zero exit code\n");
      exit(-1);
    }
    
  }

  // merge the sorted left and right halves together
  int64_t* temparr = (int64_t*) malloc((end-begin) * sizeof(int64_t)); // create a temp array on the heap
  merge(arr, begin, mid, end, temparr); // merge left and right into temparr
  for (int64_t i = begin; i < end; i++) {
    arr[i] = temparr[i-begin];
  }
  free(temparr);
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2])) {
    fprintf(stderr, "Error: invalid threshold value\n");
    return -1;
  }
  // open the file
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Error: file could not be opened\n");
    return -1;
  }

  // use fstat to determine the size of the file
  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
    fprintf(stderr, "Error: could not read number of bytes in file\n");
    return -1;
  }
  size_t file_size_in_bytes = statbuf.st_size;

  // map the file into memory using mmap
  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    fprintf(stderr, "Error: could not map data\n");
    return -1;
  }
  // sort the data!
  merge_sort(data, 0, file_size_in_bytes/sizeof(int64_t), threshold);
  //merge_sort1(data, 0, file_size_in_bytes/sizeof(int64_t), threshold);

  // unmap data
  int64_t unmapped = munmap(data, file_size_in_bytes);
  if (unmapped != 0) {
    fprintf(stderr, "Error: could not unmap data\n");
    return -1;
  }
  // close the file descriptor
  uint64_t closed = close(fd);
  if (closed != 0) {
    fprintf(stderr, "Error: could not close file\n");
    return -1;
  }
  
  // exit with a 0 exit code if sort was successful
  return 0;
}