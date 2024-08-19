; file pro4.as - syntax errors
	  
.entry LOOP
.entry LENGTH
.extern L3
.external W
MAIN:          mov S1.3, W
               add r2,STR,r3
LOOP:          jmp W
	       macro m
		     ins K
		
	       	     mov S1.2,r3
               endmacro
               prn #-a5
               sub r1,r8
               m
               bne L3
E#ND:           hlt
STR:           .string "abcdef"
LENGTH:        .data	6,,-9,15
K:             .data	22.4
S1:            .struct	8,ab"
