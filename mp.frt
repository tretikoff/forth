: IMMEDIATE push_lastword @ cfa 1 - dup c@ 1 or swap c! ;

: if ' branchifz , here 0 , ; IMMEDIATE
: else ' branch , here 0 , swap here swap ! ; IMMEDIATE
: then here swap ! ; IMMEDIATE
: endif ' then initcmd ; IMMEDIATE

: abs dup 0 < if -1 * then ;
5 abs . printnl
-3 abs . printnl
-199 abs . printnl printnl
