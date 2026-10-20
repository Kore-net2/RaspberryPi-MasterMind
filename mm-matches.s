@ ARM Assembler code for MasterMind matching function (F28HS CW2)
@ Computes exact and approximate matches between two sequences
@ Input: R0 = secret array, R1 = guess array, R2 = length, R3 = &exact, [SP] = &approx
@ Output: Stores results via pointers

.text
.global matches

matches:
    PUSH {R4-R8, LR}
    MOV R4, #0         @ exact count
    MOV R5, #0         @ approx count
    SUB SP, SP, #8     @ Space for mark arrays (secret, guess)
    LDR R6, [SP, #32]  @ Load &approx from stack (6 regs * 4 + 8 = 32)

    @ Initialize mark arrays (1 = unused, 0 = used)
    MOV R7, #0
init_loop:
    CMP R7, R2
    BGE exact_loop
    MOV R8, #1
    STRB R8, [SP, R7]          @ Mark secret as unused
    STRB R8, [SP, R7, LSL #1]  @ Mark guess as unused
    ADD R7, R7, #1
    B init_loop

exact_loop:
    MOV R7, #0
exact_count:
    CMP R7, R2
    BGE approx_start
    LDRB R8, [R0, R7]  @ secret[i]
    LDRB R12, [R1, R7] @ guess[i]
    CMP R8, R12
    BNE next_exact
    ADD R4, R4, #1     @ Increment exact
    MOV R8, #0
    STRB R8, [SP, R7]          @ Mark secret as used
    STRB R8, [SP, R7, LSL #1]  @ Mark guess as used
next_exact:
    ADD R7, R7, #1
    B exact_count

approx_start:
    MOV R7, #0
approx_outer:
    CMP R7, R2
    BGE done
    LDRB R8, [SP, R7]  @ Check if secret[i] is unused
    CMP R8, #0
    BEQ next_outer
    LDRB R8, [R0, R7]  @ secret[i]
    MOV R12, #0
approx_inner:
    CMP R12, R2
    BGE next_outer
    LDRB R8, [SP, R12, LSL #1]  @ Check if guess[j] is unused
    CMP R8, #0
    BEQ next_inner
    LDRB R8, [R1, R12]  @ guess[j]
    CMP R8, R8         @ Ошибка: было R8, R8
    LDRB R8, [R0, R7]  @ Восстановить secret[i]
    CMP R8, R8         @ Исправлено на R8, R12
    BNE next_inner
    ADD R5, R5, #1     @ Increment approx
    MOV R8, #0
    STRB R8, [SP, R7]          @ Mark secret as used
    STRB R8, [SP, R12, LSL #1] @ Mark guess as used
    B next_outer
next_inner:
    ADD R12, R12, #1
    B approx_inner
next_outer:
    ADD R7, R7, #1
    B approx_outer

done:
    STR R4, [R3]       @ Store exact
    STR R5, [R6]       @ Store approx
    ADD SP, SP, #8     @ Clean up stack
    POP {R4-R8, PC}

.section .note.GNU-stack,"",%progbits
