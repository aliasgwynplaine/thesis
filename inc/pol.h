#ifndef _POL_H
#define _POL_H


typedef struct pol_t pol_t;

pol_t * str2pol(char *);
void free_pol_t(pol_t *);
void print_pol(pol_t * pol)

/* define polynomial operations here */


/* exponent's binary masks */
/* 1 variables */
#define BMASK_1V_0 0xffffffffffffffff

/* 2 variables */
#define BMASK_2V_0 0x00000000ffffffff
#define BMASK_2V_1 0xffffffff00000000

/* 3 variables */
#define BMASK_3V_0 0x00000000001fffff
#define BMASK_3V_1 0x000003ffffe00000
#define BMASK_3V_2 0xfffffc0000000000

/* 4 variables */
#define BMASK_4V_0 0x000000000000ffff
#define BMASK_4V_1 0x00000000ffff0000
#define BMASK_4V_2 0x0000ffff00000000
#define BMASK_4V_3 0xffff000000000000

/* 5 variables */
#define BMASK_5V_0 0x0000000000000fff
#define BMASK_5V_1 0x0000000000fff000
#define BMASK_5V_2 0x0000000fff000000
#define BMASK_5V_3 0x0000fff000000000
#define BMASK_5V_4 0xffff000000000000

/* 6 variables */
#define BMASK_6V_0 0x00000000000003ff
#define BMASK_6V_1 0x00000000000ffc00
#define BMASK_6V_2 0x000000003ff00000
#define BMASK_6V_3 0x000000ffc0000000
#define BMASK_6V_4 0x0003ff0000000000
#define BMASK_6V_5 0xfffc000000000000

/* 7 variables */
#define BMASK_7V_0 0x00000000000001ff
#define BMASK_7V_1 0x000000000003fe00
#define BMASK_7V_2 0x0000000007fc0000
#define BMASK_7V_3 0x0000000ff8000000
#define BMASK_7V_4 0x00001ff000000000
#define BMASK_7V_5 0x003fe00000000000
#define BMASK_7V_6 0xffc0000000000000

/* 8 variables */
#define BMASK_8V_0 0x00000000000000ff
#define BMASK_8V_1 0x000000000000ff00
#define BMASK_8V_2 0x0000000000ff0000
#define BMASK_8V_3 0x00000000ff000000
#define BMASK_8V_4 0x000000ff00000000
#define BMASK_8V_5 0x0000ff0000000000
#define BMASK_8V_6 0x00ff000000000000
#define BMASK_8V_7 0xff00000000000000


#endif