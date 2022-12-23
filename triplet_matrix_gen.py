# -*- coding: utf-8 -*-

import sys
from random import random

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
        rndlst = [random() <= d for _ in range(m*n)]
        print(f"{m} {n} {sum(rndlst)}", file=fh)

        for i in range(m) :
            for j in range(n) :
                if rndlst[i*n + j] :
                    print(f"{i} {j} {int(random() * 9 + 1)}", file=fh)
            
    
    print("Done!")

   