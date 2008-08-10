//util.c:

#include <libroarpulse/libroarpulse.h>

/** Return the binary file name of the current process. This is not
 * supported on all architectures, in which case NULL is returned. */
char *pa_get_binary_name(char *s, size_t l) {
#ifdef __linux__
 int ret;
 char path[PATH_MAX];

 if ( (ret = readlink("/proc/self/exe", path, PATH_MAX-1)) != -1 ) {
  path[ret] = 0;
  return strncpy(s, pa_path_get_filename(path), l);
 }
#endif

 return NULL;
}

/** Return a pointer to the filename inside a path (which is the last
 * component). */
const char *pa_path_get_filename(const char *p) {
 char * r;

 if ( (r = strrchr(p, '/')) ) {
  return (const char *) r+1;
 } else {
  return p;
 }

}

//ll
