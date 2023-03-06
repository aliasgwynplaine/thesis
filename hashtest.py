# -*- coding: utf-8 -*-

def compute_hash(s) :
    h    = 0
    p    = 47
    m    = 701
    ppow = 1

    for c in s :
        h = (h + (ord(c) - ord('a') + 1) * ppow) % m
        ppow = (ppow * p) % m
    
    return h


if __name__ == '__main__' :
    common = ['f', 'g', 'h', 'f1', 'f2', 'f3', 'f4', 'p', 'p0', 'p_0', 'p1', 'q', 'pol', 'pol1', 'pol0', 'x', 'a', 'x1', 'x2', 'x0', 'aa', 'm4']

    for n in common :
        print("{} -> {} | {}".format(n, compute_hash(n), hash(n)))
    
    print("bye!")