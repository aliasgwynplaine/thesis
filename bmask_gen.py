# -*- coding: utf-8 -*-
import sys

int_sz = 64

def cerr(msg) :
    sys.stderr.write(msg +'\n')

def gen_bmask(nvars, v) :
    mask    = (1<<64) - 1
    step    = 64 // nvars
    submask = (1<<step) - 1
    mstr    = "\n/* masks list for %d vars*/\n" % nvars
    mstr   += "static const u64 MASK_LIST_%dV[] = {\n    " % nvars
    c       = 0         # counter for mstr
    m       = 3         # modulo for mstr
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
            mstr += 'BMASK_{}V_{} '.format(nvars, i)
        else :
            mstr += 'BMASK_{}V_{}, '.format(nvars, i)
        
        print("#define BMASK_{}V_{} 0x{:016x}".format(nvars, nvars - i - 1, bmask))
        c = (c + 1) % 3
        if c == 0 : mstr += '\n    '
    
    mstr += '\n};'
    print(mstr)

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
    
    for i in range(nvars) :
        gen_bmask(i + 1, v)
    
