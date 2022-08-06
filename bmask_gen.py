# -*- coding: utf-8 -*-
import sys

int_sz = 64

def cerr(msg) :
    sys.stderr.write(msg +'\n')

def gen_bmask(nvars, v) :
    mask    = (1<<64) - 1
    step    = 64 // nvars
    submask = (1<<step) - 1
    print(f"\n/* {nvars} variables */")

    if v :
        print("Bin:")

        for i in range(nvars) :
            bmask = mask&(submask<<(i*step))

            if i + 1 == nvars :
                bmask = bmask|((1<<64%nvars) - 1)<<(i+1)*step
            
            print("{:064b}".format(bmask))

    for i in range(nvars) :
        bmask = mask&(submask<<(i*step))

        if i + 1 == nvars :
            bmask = bmask|((1<<64%nvars) - 1)<<(i+1)*step
        
        print("#define BMASK_{}V_{} 0x{:016x}".format(nvars, i, bmask))
    
    if v :
        print("Dec:")
        for i in range(nvars) :
            bmask = mask&(submask<<(i*step))

            if i + 1 == nvars :
                bmask = bmask|((1<<64%nvars) - 1)<<(i+1)*step
            
            print("{:20d}".format(bmask))


if __name__ == '__main__' :
    if sys.argv.__len__() < 2 :
        cerr(f"Usage: {sys.argv[0]} <nvars> [ v ]")
        exit(1)
    
    try :
        v = sys.argv[2]
    except IndexError :
        v = False

    nvars   = int(sys.argv[1])
    
    for i in {1,2,3,4,5,6,7,8} :
        gen_bmask(i, v)
    
