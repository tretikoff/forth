: IMMEDIATE push_lastword @ cfa 1 - dup c@ 1 or swap c! ;

: if ' branchifz , here 0 , ; IMMEDIATE
: else ' branch , here 0 , swap here swap ! ; IMMEDIATE
: then here swap ! ; IMMEDIATE
: endif ' then initcmd ; IMMEDIATE

: abs dup 0 < if -1 * then ;
5 abs . printnl
-3 abs . printnl
-199 abs . printnl printnl


: repeat here ; IMMEDIATE
: until ' branchifz , , ; IMMEDIATE

: odd 2 % abs ;
: sqr dup * ;
: over >r dup r> swap ;
: 2dup over over ;
: -rot swap >r swap  r> ;
: >= < not ;

: prime
dup 1 > if
  dup 2 = not if
    dup odd if
    1
      repeat
      2 +
      2dup 2dup sqr >=
      -rot % land not
      until
    sqr <
    else drop 0 then
  else drop 1 then
else drop 0 then ;
2 prime . printnl
11 prime . printnl
191 prime . printnl
201 prime . printnl printnl
