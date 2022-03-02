; file hasErrors.as

; this file will demonstrate the various errors
; that the assembler can handle with
; because this file has intentional errors no output
; files will be created for it

; For further use, no problem here
.extern     EXT
VALID:      .data 5

; Invalid labels errors
%LABEL1:    inc r3
mov:        stop
LABEL:      add #42, r3
LABEL:      sub #42, r3

; Invalid names for directives and instructions
            muv *r6, r3
X:          .dataa 2, -11, -22

; Invalid operands for directives
M1:         .string
M2:         .data
I1:         .data 11, 3a, hello
I2:         .string "abcd

; Invalid number of operands for instructions
A1:         inc r3, #45
B1:         stop r5
C1:         add *r3

; Invalid addressing modes of operands for instructions
A2:         add #41, #42
B2:         jsr r0
C2:         lea r5, EXT

; Invalid extern / entry
            .extern $$L
            .extern VALID

           ; Error for this particular example because the assembler
           ; stops after the first pass, but it is treated in the second pass
            .entry I_DO_NOT_EXIST

; Invalid syntax
H:          cmp *r5,, *r3
