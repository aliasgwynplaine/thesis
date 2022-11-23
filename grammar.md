# EBNF

```enbf
pol  = term , { sign , term };

term = coef
     | [ coef ] , mvar;

mvar = varx
     | varx
     | varx , {'*' , varx };

varx = var
     | var , exp;

coef = num;

var  = alph , num;

exp  = "^" , num;

num  = dnz , { dig };

dnz  = "1" | "2" | ... | "9";

dig  = "0" | dnz;

alph = "a" | "b" | ... | "z";

sign = "+" | "-";

```