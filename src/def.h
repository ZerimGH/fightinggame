#ifndef DEF_H
#define DEF_H

#ifdef LOGS

#define PINFO(fmt, ...) fprintf(stdout, "(%s): " fmt, __func__, ##__VA_ARGS__)

#else

#define PINFO(fmt, ...)  ((void)0)

#endif

#define PERROR(fmt, ...) fprintf(stderr, "%s:%d(%s): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif

