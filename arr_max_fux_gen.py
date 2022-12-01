# -*- coding: utf-8 -*-
import sys

def cerr(msg) :
    sys.stderr.write(msg +'\n')


def gen_arr_data_max_fux(data) :
    str  = 'inline %s %s_max(%s * a, int p, int q)  {\n' %(data,data,data)
    str += '    if (p > q) SAYNEXITWERROR("check your bounds!");\n'
    str += '    %s mx = a[p];' % data
    str += '    int i = 1;\n'
    str += '\n'
    str += '    for ( ; i <= q; i++)\n'
    str += '        mx = __max(mx, a[i]);\n'
    str += '\n'
    str += '    return mx;\n'
    str += '}\n'
    print(str);



def gen_arr_data_max_idx_fux(data) :
    str  = 'inline %s %s_max_idx(%s * a, int p, int q)  {\n' %(data,data,data)
    str += '    if (p > q) SAYNEXITWERROR("check your bounds!");\n'
    str += '    %s mx_idx = p;\n' % data
    str += '    int i = 1;\n'
    str += '\n'
    str += '    for ( ; i <= q; i++)\n'
    str += '        mx_idx = a[mx_idx] > a[i] ? mx_idx : i;\n'
    str += '\n'
    str += '    return mx_idx;\n'
    str += '}\n'
    print(str);


if __name__ == '__main__' :
    for data in ['int', 'float', 'u8', 'u16', 'u32', 'u64'] :
        gen_arr_data_max_fux(data)
        gen_arr_data_max_idx_fux(data)