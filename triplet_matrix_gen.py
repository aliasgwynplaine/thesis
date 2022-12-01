# -*- coding: utf-8 -*-

import sys
from random import random, randint

def cerr(msg : str) :
    sys.stderr.write(msg +'\n')



if __name__ == '__main__' :
    if sys.argv.__len__() < 5 :
        cerr(f"usage: {sys.argv[0]} <m> <n> <d> <filename>")
        exit(0)
    
    m  = int(sys.argv[1])
    n  = int(sys.argv[2])
    d  = float(sys.argv[3])
    fn = sys.argv[4]

    if d > 1 or d <= 0 :
        cerr("density is betwen 0 and 1")
        exit(0)

    print(f"args: {m} {n} {d} {fn}")

    with open(fn, "w") as fh :
        print(f"{m} {n}", file=fh)

        for i in range(m) :
            for j in range(n) :
                if random() <= d :
                    print(f"{i} {j} {random() * 100 : .4f}", file=fh)
    
    print("Done!")

   