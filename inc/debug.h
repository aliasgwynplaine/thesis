#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef _DEBUG

#define debug(fmt, ...) \
    do {        \
        fprintf(stdout, "%s %s %d: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__); \
    } while (0)

#else

#define debug(fmt, ...) do { /* empty */ } while (0)

#endif

#define dbgerr(fmt, ...) \
    do {        \
        fprintf(stderr, "%s %s %d: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__); \
    } while (0)

    
#endif