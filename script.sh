#!/bin/bash

# I   refs
# I1  misses
# LLi misses
# I1  miss rate
# LLi miss rate
# D   refs
# D1  misses
# LLd misses
# D1  miss rate
# LLd miss rate
# LL refs
# LL misses
# LL miss rate
nums="$(make run 2>&1 | grep -E '^==[0-9]+==' | grep ':.* [0-9]' | cut -d':' -f2 | tr -d ' ' | grep -Eo '^[0-9.,]+' | tr ',' '.')"
awk '
NR==1 { I_refs=$0 }
NR==4 { I1_mr=$0/100 }
NR==5 { I2_mr=$0/100 }
NR==6 { D_refs=$0/100 }
NR==9 { D1_mr=$0/100 }
NR==10 { D2_mr=$0/100 }
END {
    cost=(I_refs * (1.25 + I1_mr*(20.3+I2_mr*144))) + (D_refs * (1.25 * D1_mr*(20.3+I2_mr*144)));
    print "Cost = " cost;
    score = 10 * (9-cost)/(9-2);
    if (score < 0) score = 0;
    if (score > 10) score = 10;
    print "Score = " score;
}
' <<<"$nums"
