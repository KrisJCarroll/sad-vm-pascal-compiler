######################################################################################
# SAD_VM.py
# Author: Kristopher J. Carroll
# Description: 
#   SAD (Simple and Dumb) VM is a simulated machine featuring a 32-bit RISC
#   instruction set architecture. It has 16 accessible registers including a program
#   counter, dedicated count register, and 14 general-use registers. Additionally,
#   it has two inaccessible registers, a conditional flag register for use with
#   boolean expressions and conditional jump statements as well as a return address
#   register that is automatically set with the JMPR (jump and return) instruction
#   for function calls. SAD VM has separate code and program memory and a stack
#   that is accessible to the programmer.
#
#   OP codes have been defined as their representative hexadecimal values for ease of
#   writing instructions, their structure and layout can be viewed in further detail
#   in the SAD_VM.pdf document.
#
#   Although the instruction set was designed as a fixed-width RISC style, the current
#   implementation does not enforce this width. However, all operations of the machine
#   expect to find instructions according to their proper format.
#
#   Instructions for a small subset of the Pascal programming language can be compiled
#   for pasting directly into SAD_VM.py with the use of the pascal.l, pascal.y and AST.h
#   files found in this repository, which can be built with the included MAKEFILE.
######################################################################################


# op codes
MOV = 0x0
MEM = 0x1
LIMM = 0x2
MATH = 0x3
MATHI = 0x4
COMP = 0x5
LOG = 0x6
CNT = 0x7
LOOP = 0x8
JMP = 0x9
JMPC = 0xa
JMPR = 0xb
RET = 0xc
INC = 0xd
DEC = 0xe
STCK = 0xf


# math ops
ADD = 0x0
SUB = 0x1
MULT = 0x2
DIV = 0x3

# mem ops
LOAD = 0x0
STOR = 0x1
# stack ops
PUSH = 0x0
POP = 0x1

# comparative ops
EQ = 0x0
NEQ = 0x1
LT = 0x2
GT = 0x3
LTE = 0x4
GTE = 0x5

# accessible registers
PC = 0x0
R_CNT = 0x1
R_0 = 0x2
R_1 = 0x3
R_2 = 0x4
R_3 = 0x5
R_4 = 0x6
R_5 = 0x7
R_6 = 0x8
R_7 = 0x9
R_8 = 0xa
R_9 = 0xb
R_10 = 0xc
R_11 = 0xd
R_12 = 0xe
R_13 = 0xf

IO_OUT = 0xffff0000

class Machine:
    def __init__(self, prog):
        self.program = prog # program instructions as an array of tuples
        self.cond = 0 # conditional register
        self.ra = 0 # return address register
        self.regs = {i: 0 for i in range(16)} # registers: 0 is PC, 1 is CNT
        self.stack = [] # blank stack
        self.mem = {} # data memory
        self.ops = {
            MOV: self.mov_op,
            MEM: self.mem_op,
            LIMM: self.limm_op,
            MATH: self.math_op,
            MATHI: self.mathi_op,
            COMP: self.comp_op,
            CNT: self.cnt_op,
            LOOP: self.loop_op,
            JMP: self.jmp_op,
            JMPC: self.jmpc_op,
            JMPR: self.jmpr_op,
            RET: self.ret_op,
            INC: self.inc_op,
            DEC: self.dec_op,
            STCK: self.stck_op,
        }

    def mov_op(self, rest):
        dst = rest[0]
        src = rest[1]
        self.regs[dst] = self.regs[src]

    def math_op(self, rest):
        dst = rest[0]
        src1 = rest[1]
        src2 = rest[2]
        mode = rest[3]
        if mode == ADD: # add
            self.regs[dst] = self.regs[src1] + self.regs[src2]
        elif mode == SUB: # sub
            self.regs[dst] = self.regs[src1] - self.regs[src2]
        elif mode == MULT: # mult
            self.regs[dst] = self.regs[src1] * self.regs[src2]
        elif mode == DIV: # div
            self.regs[dst] = self.regs[src1] // self.regs[src2]
        else:
            print("ERROR(math_op): math mode flag not found")
            quit()
    
    def mathi_op(self, rest):
        dst = rest[0]
        mode = rest[1]
        imm = rest[2]
        if mode == ADD: # add
            self.regs[dst] += imm
        elif mode == SUB: # sub
            self.regs[dst] -= imm
        elif mode == MULT: # mult
            self.regs[dst] *= imm
        elif mode == DIV: # div
            self.regs[dst] //= imm
        else:
            print("ERROR(mathi_op): math mode flag not found ({})".format(mode))
            quit()
    
    def mem_op(self, rest):
        dst = rest[0]
        src = rest[1]
        mode = rest[2]
        if mode == LOAD:
            if src == 0xff00:
                self.regs[dst] = int(input())
            else:
                self.regs[dst] = self.mem[self.regs[src]]
        elif mode == STOR:
            if dst == 0xffff0000:
                print(self.regs[src])
                return
            if dst == 0xffff0001:
                if self.regs[src] == 0:
                    print()
                else:
                    print(chr(self.regs[src]), end='')
            else:
                self.mem[self.regs[dst]] = self.regs[src]

    def limm_op(self, rest):
        dst = rest[0]
        self.regs[dst] = rest[1]
    
    def jmp_op(self, rest):
        self.regs[PC] = rest[0]
    
    def jmpc_op(self, rest):
        if not self.cond:
            self.regs[PC] = rest[0]
    
    def jmpr_op(self, rest):
        self.ra = self.regs[PC]
        self.regs[PC] = rest[0]
    
    def ret_op(self, rest):
        self.regs[PC] = self.ra
            
    def comp_op(self, rest):
        reg1 = rest[0]
        reg2 = rest[1]
        mode = rest[2]
        if mode == EQ:
            self.cond = int(self.regs[reg1] == self.regs[reg2])
        elif mode == NEQ:
            self.cond = int(self.regs[reg1] != self.regs[reg2])
        elif mode == LT:
            self.cond = int(self.regs[reg1] < self.regs[reg2])
        elif mode == GT:
            self.cond = int(self.regs[reg1] > self.regs[reg2])
        elif mode == LTE:
            self.cond = int(self.regs[reg1] <= self.regs[reg2])
        elif mode == GTE:
            self.cond = int(self.regs[reg1] >= self.regs[reg2])
        else:
            print("ERROR(comp_op): comparison mode flag not found.")
            quit()
        
    def cnt_op(self, rest):
        self.regs[R_CNT] = rest[0]

    def loop_op(self, rest):
        self.regs[R_CNT] -= 1
        if self.regs[R_CNT]:
            self.regs[PC] = rest[0]
    
    def inc_op(self, rest):
        self.regs[rest[0]] += 1
    
    def dec_op(self, rest):
        self.regs[rest[0]] -= 1
    
    def stck_op(self, rest):
        reg = rest[0]
        mode = rest[1]
        if mode == PUSH:
            self.stack.append(self.regs[reg])
        elif mode == POP:
            self.regs[reg] = self.stack.pop()

    def run(self):
        try:
            while self.regs[PC] is not None:
                instruct = self.program[self.regs[PC]]
                self.regs[PC] += 1
                op = instruct[0]
                rest = instruct[1:]
                self.ops[op](rest)
        except IndexError:
            return 0
    

# This code is for performing Eratosthenes' sieve to find all primes less than MAX_NUM and print them in their hexadecimal format
MAX_NUM = 1000
prog = [
    (CNT, MAX_NUM), # 0 looping NUM_PRIMES times to initialize array of integers in memory
    (LIMM, R_0, 1), # 1 initializing all values to true
    (MEM, R_CNT, R_0, STOR), # 2 initializing NUM_PRIMES places in memory to store primes
    (LOOP, 1), # 3

    (LIMM, R_0, MAX_NUM), # 4 using R_0 to hold max value
    (LIMM, R_1, 1), # 5 i = 1
    (LIMM, R_2, 0), # 6 comparing to 0
    # i-loop
    (INC, R_1), # 7 increment i
    (COMP, R_1, R_0, LT), # 8 while i < MAX_NUM
    (JMPC, None), # 9
    (MEM, R_3, R_1, LOAD), # 10 load boolean value at i into R_3
    (COMP, R_2, R_3, NEQ), # 11 
    (JMPC, 7), # 12 array boolean was false, skip to next iteration
    (STCK, R_1, PUSH), # 13 pushing i onto stack
    (JMPR, 24), # 14 calling hex function
    (MATH, R_4, R_1, R_1, MULT), # 15 j = i^2
    (COMP, R_4, R_0, LT), # 16 is j > MAX_NUM?
    (JMPC, 7), # 17 go back to i-loop
    (MEM, R_4, R_2, STOR), # 18 storing false
    # j-loop
    (MATH, R_4, R_4, R_1, ADD), # 19 incrementing j by i
    (COMP, R_4, R_0, LT), # 20 checking if j > MAX_NUM
    (JMPC, 7), # 21 back to i-loop
    (MEM, R_4, R_2, STOR), # 22 storing false
    (JMP, 18), # 23 go back to j-loop
    # convert to hex
    (LIMM, R_5, 10), # 24 loading R_5 with 10 for comparison operations
    (LIMM, R_CNT, 0), # 25 loading R_CNT with 0 for recording how many chars to print
    (LIMM, R_6, 16), # 26 loading R_6 with 16 for divisor
    # hex-loop
    (STCK, R_7, POP), # 27 popping value to convert off stack into R_7
    (MATH, R_8, R_7, R_6, DIV), # 28 R_8(quotient) = num / 16
    (MATH, R_9, R_8, R_6, MULT), # 29 R_9(temp) = quotient * divisor
    (MATH, R_9, R_7, R_9, SUB), # 30 R_9(remainder) = R_6 - R_8
    (COMP, R_9, R_5, LT), # 31 if remainder > 10
    (JMPC, 35), # 32 skip to 35
    (MATHI, R_9, ADD, 48), # 33 else add 48 to remainder for correct ASCII value
    (JMP, 36), # 34 and continue
    (MATHI, R_9, ADD, 55), # 35 add 55 to remainder for correct ASCII value
    (STCK, R_9, PUSH), # 36 push value to stack
    (INC, R_CNT), # 37 increment count
    (COMP, R_8, R_2, NEQ), # 38 if quotient == 0
    (JMPC, 42), # 39 we're done, time to print
    (STCK, R_8, PUSH), # 40 else push quotient to stack for loop
    (JMP, 27), # 41 back to hex-loop
    (STCK, R_9, POP), # 42 pop top value into R_9
    (MEM, 0xffff0001, R_9, STOR), # 43 string printer of value at R_9
    (LOOP, 42), # 44 loop back to 41
    (LIMM, R_9, 0), # 45 load ASCII value of null
    (MEM, 0xffff0001, R_9, STOR), # 46 print null character and cause newline
    (RET,)
]

# prog = [
#     (LIMM, R_4, 10),
#     (MOV, R_0, R_4),
#     (LIMM, R_4, 3),
#     (MOV, R_1, R_4),
#     (LIMM, R_4, 1),
#     (MOV, R_2, R_4),
#     (LIMM, R_5, 0),
#     (COMP, R_2, R_5, GT),
#     (JMPC, 16),
#     (MOV, R_3, R_1),
#     (COMP, R_3, R_0, LT),
#     (JMPC, 16),
#     (MEM, IO_OUT, R_3, STOR),
#     (MATH, R_7, R_3, R_1, ADD),
#     (MOV, R_3, R_7),
#     (JMP, 10),
#     (JMP, None)
# ]

cpu = Machine(prog)
cpu.run()