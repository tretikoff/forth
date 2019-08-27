%define pc r15
%define w r14
%define rstack r13
%define link 0

%assign link_size 8

%macro create_link 0
  %%link: dq link        ; %% creates new local (example: @1234.link)
  %define link %%link
%endmacro

%macro native 2
  section .data
    w_ %+ %2:            ; w_COMMAND label
	    create_link        ; create link for dictionary search
	    db %1, 0           ; put name of COMMAND with null-terminator
	  xt_ %+ %2:           ; xt_COMMAND label
      dq %2 %+ _impl     ; COMMAND_impl address
  section .text
	 %2 %+ _impl:           ; COMMAND_impl label
%endmacro

%macro colon 2
  section .data
	 w_ %+ %2:
	  create_link
	  db %1, 0
  section .text
	 xt_ %+ %2:
	  dq docol_impl             ; The `docol` address −− one level of indirection
%endmacro

%macro branch0 1
  dq xt_0branch
  dq %1
%endmacro

%macro branch 1
  dq xt_branch
  dq %1
%endmacro
