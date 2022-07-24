// -----------------------------------------------------------------------------
// This file is part of Moira - A Motorola 68k emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// Assembles an instruction handler name
#define EXEC_IMS(func,I,M,S) &Moira::exec##func <I,M,S>
#define EXEC_CIMS(func,T,I,M,S) &Moira::exec##func <T,I,M,S>
#define DASM_IMS(func,I,M,S) &Moira::dasm##func <I,M,S>

// Registers an instruction handler
#define IMS(id,name,I,M,S) { \
exec[id] = EXEC_IMS(name,I,M,S); \
if (dasm) dasm[id] = DASM_IMS(name,I,M,S); \
if (info) info[id] = InstrInfo {I,M,S}; \
}

#define CIMS(id,name,I,M,S) { \
if (model == M68000) { \
exec[id] = EXEC_CIMS(name,M68000,I,M,S); } else { \
exec[id] = EXEC_CIMS(name,M68010,I,M,S); } \
if (dasm) dasm[id] = DASM_IMS(name,I,M,S); \
if (info) info[id] = InstrInfo {I,M,S}; \
}

// Registers a special loop-mode instruction handler
#define IMSloop(id,name,I,M,S) { \
assert(loop[id] == nullptr); \
loop[id] = EXEC_IMS(name,I##_LOOP,M,S); \
}

#define CIMSloop(id,name,I,M,S) { \
assert(loop[id] == nullptr); \
loop[id] = EXEC_CIMS(name,M68010,I##_LOOP,M,S); \
}

// Registers an instruction in one of the standard instruction formats:
//
//     ____ ____ ____ ____
//     ____ ____ ____ _XXX    XXX : Operand parameter (Register number etc.)
//     ____ ____ ____ XXXX      S : Size information  (Word or Long)
//     ____ ____ XXXX XXXX     SS : Size information  (Byte or Word or Long)
//     ____ XXX_ ____ _XXX
//     ____ XXX_ XXXX XXXX
//     ____ ____ __MM MXXX
//     ____ XXX_ __MM MXXX
//     ____ XXX_ SS__ _XXX
//     ____ ____ SSMM MXXX
//     ____ XXX_ SSMM MXXX
//     ____ XXXS __MM MXXX
//     __SS ____ __MM MXXX
//     __SS XXX_ __MM MXXX

#define ________________(op,I,M,S,f,func) { func(op, f, I, M, S); }

#define _____________XXX(op,I,M,S,f,func) { \
for (int j = 0; j < 8; j++) func((op) | j, f, I, M, S); }

#define ____________XXXX(op,I,M,S,f,func) { \
for (int j = 0; j < 16; j++) func((op) | j, f, I, M, S); }

#define __________XXXXXX(op,I,M,S,f,func) { \
for (int j = 0; j < 64; j++) func((op) | j, f, I, M, S); }

#define ________XXXXXXXX(op,I,M,S,f,func) { \
for (int j = 0; j < 256; j++) func((op) | j, f, I, M, S); }

#define ____XXXXXXXXXXXX(op,I,M,S,f,func) { \
for (int j = 0; j < 4096; j++) func((op) | j, f, I, M, S); }

#define XXXXXXXXXXXXXXXX(I,M,S,f,func) { \
for (int j = 0; j < 65536; j++) func(j, f, I, M, S); }

#define __________XXX___(op,I,M,S,f,func) { \
for (int j = 0; j < 8; j++) func((op) | j << 3, f, I, M, S); }

#define ____XXX______XXX(op,I,M,S,f,func) { \
for (int i = 0; i < 8; i++) _____________XXX((op) | i << 9, I, M, S, f, func); }

#define ____XXX___XXX___(op,I,M,S,f,func) { \
for (int i = 0; i < 8; i++) __________XXX___((op) | i << 9, I, M, S, f, func); }

#define ____XXX___XXXXXX(op,I,M,S,f,func) { \
for (int i = 0; i < 8; i++) __________XXXXXX((op) | i << 9, I, M, S, f, func); }

#define ____XXX_XXXXXXXX(op,I,M,S,f,func) { \
for (int i = 0; i < 8; i++) ________XXXXXXXX((op) | i << 9, I, M, S, f, func); }

#define __________MMMXXX(op,I,m,S,f,func) { \
for (int j = 0; j < 8; j++) { \
if ((m) & 0b100000000000) func((op) | 0 << 3 | j, f, I,  (Mode)0, S); \
if ((m) & 0b010000000000) func((op) | 1 << 3 | j, f, I,  (Mode)1, S); \
if ((m) & 0b001000000000) func((op) | 2 << 3 | j, f, I,  (Mode)2, S); \
if ((m) & 0b000100000000) func((op) | 3 << 3 | j, f, I,  (Mode)3, S); \
if ((m) & 0b000010000000) func((op) | 4 << 3 | j, f, I,  (Mode)4, S); \
if ((m) & 0b000001000000) func((op) | 5 << 3 | j, f, I,  (Mode)5, S); \
if ((m) & 0b000000100000) func((op) | 6 << 3 | j, f, I,  (Mode)6, S); \
} \
if ((m) & 0b000000010000) func((op) | 7 << 3 | 0, f, I,  (Mode)7, S); \
if ((m) & 0b000000001000) func((op) | 7 << 3 | 1, f, I,  (Mode)8, S); \
if ((m) & 0b000000000100) func((op) | 7 << 3 | 2, f, I,  (Mode)9, S); \
if ((m) & 0b000000000010) func((op) | 7 << 3 | 3, f, I, (Mode)10, S); \
if ((m) & 0b000000000001) func((op) | 7 << 3 | 4, f, I, (Mode)11, S); }

#define ____XXX___MMMXXX(op,I,m,S,f,func) { \
for (int i = 0; i < 8; i++) __________MMMXXX((op) | i << 9, I, m, S, f, func) }

#define ____XXX_SS___XXX(op,I,M,s,f,func) { \
if ((s) & 0b100) ____XXX______XXX((op) | 2 << 6, I, M, Long, f, func); \
if ((s) & 0b010) ____XXX______XXX((op) | 1 << 6, I, M, Word, f, func); \
if ((s) & 0b001) ____XXX______XXX((op) | 0 << 6, I, M, Byte, f, func); }

#define ________SSMMMXXX(op,I,m,s,f,func) { \
if ((s) & 0b100) __________MMMXXX((op) | 2 << 6, I, m, Long, f, func); \
if ((s) & 0b010) __________MMMXXX((op) | 1 << 6, I, m, Word, f, func); \
if ((s) & 0b001) __________MMMXXX((op) | 0 << 6, I, m, Byte, f, func); }

#define ____XXX_SSMMMXXX(op,I,m,s,f,func) { \
if ((s) & 0b100) ____XXX___MMMXXX((op) | 2 << 6, I, m, Long, f, func); \
if ((s) & 0b010) ____XXX___MMMXXX((op) | 1 << 6, I, m, Word, f, func); \
if ((s) & 0b001) ____XXX___MMMXXX((op) | 0 << 6, I, m, Byte, f, func); }

#define ____XXXS__MMMXXX(op,I,m,s,f,func) { \
if ((s) & 0b100) ____XXX___MMMXXX((op) | 1 << 8, I, m, Long, f, func); \
if ((s) & 0b010) ____XXX___MMMXXX((op) | 0 << 8, I, m, Word, f, func); \
if ((s) & 0b001) assert(false); }

#define _____SS___MMMXXX(op,I,m,s,f,func) { \
if ((s) & 0b100) __________MMMXXX((op) | 2 << 9, I, m, Long, f, func); \
if ((s) & 0b010) __________MMMXXX((op) | 3 << 9, I, m, Word, f, func); \
if ((s) & 0b001) __________MMMXXX((op) | 1 << 9, I, m, Byte, f, func); }

#define __SS______MMMXXX(op,I,m,s,f,func) { \
if ((s) & 0b100) __________MMMXXX((op) | 2 << 12, I, m, Long, f, func); \
if ((s) & 0b010) __________MMMXXX((op) | 3 << 12, I, m, Word, f, func); \
if ((s) & 0b001) __________MMMXXX((op) | 1 << 12, I, m, Byte, f, func); }

#define __SSXXX___MMMXXX(op,I,m,s,f,func) { \
if ((s) & 0b100) ____XXX___MMMXXX((op) | 2 << 12, I, m, Long, f, func); \
if ((s) & 0b010) ____XXX___MMMXXX((op) | 3 << 12, I, m, Word, f, func); \
if ((s) & 0b001) ____XXX___MMMXXX((op) | 1 << 12, I, m, Byte, f, func); }


static constexpr u16
parse(const char *s, int sum = 0)
{
    return
    *s == ' ' ? parse(s + 1, sum) :
    *s == '-' ? parse(s + 1, sum << 1) :
    *s == '0' ? parse(s + 1, sum << 1) :
    *s == '1' ? parse(s + 1, (sum << 1) + 1) : (u16)sum;
}

void
Moira::createJumpTable()
{
    u16 opcode;

    //
    // Start with clean tables
    //

    XXXXXXXXXXXXXXXX(ILLEGAL, MODE_IP, (Size)0, Illegal, CIMS);

    for (int i = 0; i < 0x10000; i++) {
        loop[i] = nullptr;
    }


    // Unimplemented instructions
    //
    //       Format: 1010 ---- ---- ---- (Line A instructions)
    //               1111 ---- ---- ---- (Line F instructions)

    opcode = parse("1010 ---- ---- ----");
    ____XXXXXXXXXXXX(opcode, LINE_A, MODE_IP, (Size)0, LineA, CIMS);

    opcode = parse("1111 ---- ---- ----");
    ____XXXXXXXXXXXX(opcode, LINE_F, MODE_IP, (Size)0, LineF, CIMS);

    if (model >= M68020) {

        opcode = parse("1111 ---0 10-- ----");
        ____XXX___XXXXXX(opcode, cpBcc, MODE_IP, Word, CpBcc, CIMS);
        opcode = parse("1111 ---0 11-- ----");
        ____XXX___XXXXXX(opcode, cpBcc, MODE_IP, Long, CpBcc, CIMS);

        opcode = parse("1111 ---0 00-- ----");
        ____XXX___XXXXXX(opcode, cpGEN, MODE_IP, (Size)0, CpGen, CIMS);

        opcode = parse("1111 ---1 01-- ----");
        ____XXX___MMMXXX(opcode, cpRESTORE, 0b001101111000, Word, CpRestore, CIMS);

        opcode = parse("1111 ---1 00-- ----");
        ____XXX___MMMXXX(opcode, cpSAVE, 0b001011111000, Word, CpSave, CIMS);

        opcode = parse("1111 ---0 0111 1---");
        ____XXX___XXX___(opcode | 0b010, cpTRAPcc, MODE_IP, (Size)0, CpTrapcc, CIMS);
        ____XXX___XXX___(opcode | 0b011, cpTRAPcc, MODE_IP, (Size)0, CpTrapcc, CIMS);
        ____XXX___XXX___(opcode | 0b100, cpTRAPcc, MODE_IP, (Size)0, CpTrapcc, CIMS);

        opcode = parse("1111 ---0 01-- ----");
        ____XXX___MMMXXX(opcode, cpScc, 0b101111111000, (Size)0, CpScc, CIMS);

        opcode = parse("1111 ---0 0100 1---");
        ____XXX______XXX(opcode, cpDBcc, MODE_IP, (Size)0, CpDbcc, CIMS);

    }


    // ABCD
    //
    //       Syntax: (1) ABCD Dx,Dy
    //               (2) ABCD -(Ax),-(Ay)
    //         Size: Byte

    // Dx,Dy
    opcode = parse("1100 ---1 0000 0---");
    ____XXX______XXX(opcode, ABCD, MODE_DN, Byte, Abcd, IMS);

    // -(Ax),-(Ay)
    opcode = parse("1100 ---1 0000 1---");
    ____XXX______XXX(opcode, ABCD, MODE_PD, Byte, Abcd, IMS);
    ____XXX______XXX(opcode, ABCD, MODE_PD, Byte, Abcd, IMSloop);


    // ADD
    //
    //       Syntax: (1) ADD <ea>,Dy
    //               (2) ADD Dx,<ea>
    //         Size: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>,Dy       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X  (X)  X   X   X   X   X   X   X   X   X   X

    opcode = parse("1101 ---0 ---- ----");
    ____XXX_SSMMMXXX(opcode, ADD, 0b101111111111, Byte,        AddEaRg, IMS);
    ____XXX_SSMMMXXX(opcode, ADD, 0b111111111111, Word | Long, AddEaRg, IMS);
    ____XXX_SSMMMXXX(opcode, ADD, 0b001110000000, Byte,        AddEaRg, IMSloop);
    ____XXX_SSMMMXXX(opcode, ADD, 0b001110000000, Word | Long, AddEaRg, IMSloop);


    //               -------------------------------------------------
    // Dx,<ea>       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X   X   X   X   X   X   X

    opcode = parse("1101 ---1 ---- ----");
    ____XXX_SSMMMXXX(opcode, ADD, 0b001111111000, Byte | Word | Long, AddRgEa, IMS);
    ____XXX_SSMMMXXX(opcode, ADD, 0b001110000000, Byte | Word | Long, AddRgEa, IMSloop);


    // ADDA
    //
    //       Syntax: ADDA <ea>,Ay
    //         Size: Word, Longword
    //
    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X   X   X   X   X   X   X   X   X   X   X   X

    opcode = parse("1101 ---- 11-- ----");
    ____XXXS__MMMXXX(opcode, ADDA, 0b111111111111, Word | Long, Adda, IMS)
    ____XXXS__MMMXXX(opcode, ADDA, 0b001110000000, Word | Long, Adda, IMSloop)


    // ADDI
    //
    //       Syntax: ADDI #<data>,<ea>
    //         Size: Byte, Word, Longword
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 0110 ---- ----");
    ________SSMMMXXX(opcode, ADDI, 0b100000000000, Byte | Word | Long, AddiRg, CIMS);
    ________SSMMMXXX(opcode, ADDI, 0b001111111000, Byte | Word | Long, AddiEa, IMS);


    // ADDQ
    //
    //       Syntax: ADDQ #<data>,<ea>
    //         Size: Byte, Word, Longword
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X  (X)  X   X   X   X   X   X   X

    opcode = parse("0101 ---0 ---- ----");
    ____XXX_SSMMMXXX(opcode, ADDQ, 0b100000000000, Byte | Word | Long, AddqDn, IMS);
    ____XXX_SSMMMXXX(opcode, ADDQ, 0b010000000000, Word | Long,        AddqAn, IMS);
    ____XXX_SSMMMXXX(opcode, ADDQ, 0b001111111000, Byte | Word | Long, AddqEa, IMS);


    // ADDX
    //
    //       Syntax: (1) ADDX Dx,Dy
    //               (2) ADDX -(Ax),-(Ay)
    //         Size: Byte, Word, Longword

    // Dx,Dy
    opcode = parse("1101 ---1 --00 0---");
    ____XXX_SS___XXX(opcode, ADDX, MODE_DN, Byte | Word | Long, AddxRg, IMS);

    // -(Ax),-(Ay)
    opcode = parse("1101 ---1 --00 1---");
    ____XXX_SS___XXX(opcode, ADDX, MODE_PD, Byte | Word | Long, AddxEa, IMS);
    ____XXX_SS___XXX(opcode, ADDX, MODE_PD, Byte | Word | Long, AddxEa, IMSloop);


    // AND
    //
    //       Syntax: (1) AND <ea>,Dy
    //               (2) AND Dx,<ea>
    //         Size: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>,Dy       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X   X

    opcode = parse("1100 ---0 ---- ----");
    ____XXX_SSMMMXXX(opcode, AND, 0b101111111111, Byte | Word | Long, AndEaRg, IMS);
    ____XXX_SSMMMXXX(opcode, AND, 0b001110000000, Byte,        AddEaRg, IMSloop);
    ____XXX_SSMMMXXX(opcode, AND, 0b001110000000, Word | Long, AddEaRg, IMSloop);


    //               -------------------------------------------------
    // Dx,<ea>       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X   X   X   X   X   X   X

    opcode = parse("1100 ---1 ---- ----");
    ____XXX_SSMMMXXX(opcode, AND, 0b001111111000, Byte | Word | Long, AndRgEa, IMS);
    ____XXX_SSMMMXXX(opcode, AND, 0b001110000000, Byte | Word | Long, AndRgEa, IMSloop);


    // ANDI
    //
    //       Syntax: ANDI #<data>,<ea>
    //         Size: Byte, Word, Longword
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 0010 ---- ----");
    ________SSMMMXXX(opcode, ANDI, 0b100000000000, Byte | Word | Long, AndiRg, CIMS);
    ________SSMMMXXX(opcode, ANDI, 0b001111111000, Byte | Word | Long, AndiEa, IMS);


    // ANDI to CCR
    //
    //       Syntax: ANDI #<data>,CCR
    //         Size: Byte
    //

    opcode = parse("0000 0010 0011 1100");
    ________________(opcode, ANDICCR, MODE_IM, Byte, Andiccr, CIMS);


    // ANDI to SR
    //
    //       Syntax: ANDI #<data>,SR
    //         Size: Byte
    //

    opcode = parse("0000 0010 0111 1100");
    ________________(opcode, ANDISR, MODE_IM, Word, Andisr, CIMS);


    // ASL, ASR
    //
    //       Syntax: (1) ASx Dx,Dy
    //               (2) ASx #<data>,Dy
    //               (3) ASx <ea>
    //         Size: Byte, Word, Longword

    // Dx,Dy
    opcode = parse("1110 ---1 --10 0---");
    ____XXX_SS___XXX(opcode, ASL, MODE_DN,  Byte | Word | Long, ShiftRg, IMS);

    opcode = parse("1110 ---0 --10 0---");
    ____XXX_SS___XXX(opcode, ASR, MODE_DN,  Byte | Word | Long, ShiftRg, IMS);

    // #<data>,Dy
    opcode = parse("1110 ---1 --00 0---");
    ____XXX_SS___XXX(opcode, ASL, MODE_IM, Byte | Word | Long, ShiftIm, IMS);

    opcode = parse("1110 ---0 --00 0---");
    ____XXX_SS___XXX(opcode, ASR, MODE_IM, Byte | Word | Long, ShiftIm, IMS);

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X   X   X   X   X   X   X

    opcode = parse("1110 0001 11-- ----");
    __________MMMXXX(opcode, ASL, 0b001111111000, Word, ShiftEa, IMS);
    __________MMMXXX(opcode, ASL, 0b001110000000, Word, ShiftEa, IMSloop);

    opcode = parse("1110 0000 11-- ----");
    __________MMMXXX(opcode, ASR, 0b001111111000, Word, ShiftEa, IMS);
    __________MMMXXX(opcode, ASR, 0b001110000000, Word, ShiftEa, IMSloop);


    // Bcc
    //
    //       Syntax: Bcc <label>
    //         Size: Word

    // Dn,<label>

    opcode = parse("0110 ---- ---- ----");
    ________________(opcode | 0x000, BRA, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0x200, BHI, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0x300, BLS, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0x400, BCC, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0x500, BCS, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0x600, BNE, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0x700, BEQ, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0x800, BVC, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0x900, BVS, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0xA00, BPL, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0xB00, BMI, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0xC00, BGE, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0xD00, BLT, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0xE00, BGT, MODE_IP, Word, Bcc, IMS);
    ________________(opcode | 0xF00, BLE, MODE_IP, Word, Bcc, IMS);

    for (int i = 1; i <= 0xFF; i++) {

        ________________(opcode | 0x000 | i, BRA, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0x200 | i, BHI, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0x300 | i, BLS, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0x400 | i, BCC, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0x500 | i, BCS, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0x600 | i, BNE, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0x700 | i, BEQ, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0x800 | i, BVC, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0x900 | i, BVS, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0xA00 | i, BPL, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0xB00 | i, BMI, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0xC00 | i, BGE, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0xD00 | i, BLT, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0xE00 | i, BGT, MODE_IP, Byte, Bcc, IMS);
        ________________(opcode | 0xF00 | i, BLE, MODE_IP, Byte, Bcc, IMS);
    }

    if (model >= M68020) {

        ________________(opcode | 0x000 | 0xFF, BRA, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0x200 | 0xFF, BHI, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0x300 | 0xFF, BLS, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0x400 | 0xFF, BCC, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0x500 | 0xFF, BCS, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0x600 | 0xFF, BNE, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0x700 | 0xFF, BEQ, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0x800 | 0xFF, BVC, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0x900 | 0xFF, BVS, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0xA00 | 0xFF, BPL, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0xB00 | 0xFF, BMI, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0xC00 | 0xFF, BGE, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0xD00 | 0xFF, BLT, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0xE00 | 0xFF, BGT, MODE_IP, Long, Bcc, IMS);
        ________________(opcode | 0xF00 | 0xFF, BLE, MODE_IP, Long, Bcc, IMS);
    }


    // BFCHG, BFCLR, BFINS, BFSET
    //
    //       Syntax: BFxxx <ea> {offset:width}
    //         Size: Unsized

    //               -------------------------------------------------
    // <ea> {:}      | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X           X   X   X   X

    if (model >= M68020) {

        opcode = parse("1110 1010 11-- ----");
        __________MMMXXX(opcode, BFCHG, 0b101001111000, Word, BitField, CIMS);

        opcode = parse("1110 1100 11-- ----");
        __________MMMXXX(opcode, BFCLR, 0b101001111000, Word, BitField, CIMS);

        opcode = parse("1110 1111 11-- ----");
        __________MMMXXX(opcode, BFINS, 0b101001111000, Word, BitField, CIMS);

        opcode = parse("1110 1110 11-- ----");
        __________MMMXXX(opcode, BFSET, 0b101001111000, Word, BitField, CIMS);
    }


    // BFEXTS, BFEXTU, BFFFO, BFTST
    //
    //       Syntax: BFxxx <ea> {offset:width}
    //         Size: Unsized

    //               -------------------------------------------------
    // <ea> {:}      | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X           X   X   X   X   X   X   X

    if (model >= M68020) {

        opcode = parse("1110 1011 11-- ----");
        __________MMMXXX(opcode, BFEXTS, 0b101001111110, Word, BitField, CIMS);

        opcode = parse("1110 1001 11-- ----");
        __________MMMXXX(opcode, BFEXTU, 0b101001111110, Word, BitField, CIMS);

        opcode = parse("1110 1101 11-- ----");
        __________MMMXXX(opcode, BFFFO, 0b101001111110, Word, BitField, CIMS);

        opcode = parse("1110 1000 11-- ----");
        __________MMMXXX(opcode, BFTST, 0b101001111110, Word, BitField, CIMS);
    }


    // BCHG, BCLR
    //
    //       Syntax: (1) BCxx Dn,<ea>
    //               (2) BCxx #<data>,<ea>
    //         Size: Byte, Longword

    //               -------------------------------------------------
    // Dx,<ea>       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 ---1 01-- ----");
    ____XXX___MMMXXX(opcode, BCHG, 0b101111111000, Long, BitDxEa, IMS);
    opcode = parse("0000 ---1 10-- ----");
    ____XXX___MMMXXX(opcode, BCLR, 0b101111111000, Long, BitDxEa, IMS);

    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 1000 01-- ----");
    __________MMMXXX(opcode, BCHG, 0b101111111000, Byte, BitImEa, IMS);
    opcode = parse("0000 1000 10-- ----");
    __________MMMXXX(opcode, BCLR, 0b101111111000, Byte, BitImEa, IMS);


    // BKPT (68010+)
    //
    //       Syntax: BKPT #<vector>
    //        Sizes: Unsized

    if (model >= M68010) {

        opcode = parse("0100 1000 0100 1---");
        _____________XXX(opcode, BKPT, MODE_IP, Long, Bkpt, IMS);
    }


    // BSET
    //
    //       Syntax: (1) BSET Dx,<ea>
    //               (2) BSET #<data>,<ea>
    //         Size: Byte, Longword

    //               -------------------------------------------------
    // Dx,<ea>       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 ---1 11-- ----");
    ____XXX___MMMXXX(opcode, BSET, 0b101111111000, Long, BitDxEa, IMS);

    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 1000 11-- ----");
    __________MMMXXX(opcode, BSET, 0b101111111000, Byte, BitImEa, IMS);


    // BSR
    //
    //       Syntax: (1) BSR <label>
    //         Size: Byte, Word

    opcode = parse("0110 0001 ---- ----");
    ________________(opcode, BSR, MODE_IP, Word, Bsr, IMS);

    for (int i = 1; i <= 0xFF; i++) {
        ________________(opcode | i, BSR, MODE_IP, Byte, Bsr, IMS);
    }

    if (model >= M68020) {
        ________________(opcode | 0xFF, BSR, MODE_IP, Long, Bsr, IMS);
    }


    // BTST
    //
    //       Syntax: (1) BTST Dx,<ea>
    //               (2) BTST #<data>,<ea>
    //         Size: Byte, Longword

    //               -------------------------------------------------
    // Dx,<ea>       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X   X?

    opcode = parse("0000 ---1 00-- ----");
    ____XXX___MMMXXX(opcode, BTST, 0b101111111111, Byte, BitDxEa, IMS);

    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X

    opcode = parse("0000 1000 00-- ----");
    __________MMMXXX(opcode, BTST, 0b101111111110, Byte, BitImEa, IMS);


    // CALLM
    //
    //       Syntax: CALLM #<data>,<ea>
    //         Size: Unsized

    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                TODO

    if (model >= M68020) {

        opcode = parse("0000 0110 11-- ----");
        __________MMMXXX(opcode, CALLM, 0b001001111110, Byte, Callm, CIMS);
    }


    // CAS, CAS2
    //
    //       Syntax: CAS Dc,Du,<ea>
    //               CAS2 Dc1:Dc2,Du1:Du2,(Rn1):(Rn2)
    //         Size: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                TODO

    if (model >= M68020) {

        // CAS
        opcode = parse("0000 1010 11-- ----");
        __________MMMXXX(opcode, CAS, 0b001111111000, Byte, Cas, CIMS);
        opcode = parse("0000 1100 11-- ----");
        __________MMMXXX(opcode, CAS, 0b001111111000, Word, Cas, CIMS);
        opcode = parse("0000 1110 11-- ----");
        __________MMMXXX(opcode, CAS, 0b001111111000, Long, Cas, CIMS);

        // CAS2
        opcode = parse("0000 1100 1111 1100");
        ________________(opcode, CAS2, MODE_IM, Word, Cas2, CIMS);

        opcode = parse("0000 1110 1111 1100");
        ________________(opcode, CAS2, MODE_IM, Long, Cas2, CIMS);
    }


    // CHK
    //
    //       Syntax: CHK <ea>,Dy
    //         Size: Word, (Longword)

    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X   X

    opcode = parse("0100 ---1 10-- ----");
    ____XXX___MMMXXX(opcode, CHK, 0b101111111111, Word, Chk, CIMS);

    if (model >= M68020) {

        opcode = parse("0100 ---1 00-- ----");
        ____XXX___MMMXXX(opcode, CHK, 0b101111111111, Long, Chk, CIMS);
    }


    // CHK2 / CMP2
    //
    //       Syntax: CHK2 <ea>,Dy
    //         Size: Word

    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                TODO

    if (model >= M68020) {

        opcode = parse("0000 0000 11-- ----");
        __________MMMXXX(opcode, CHK2, 0b001001111110, Byte, Chk2, CIMS);

        opcode = parse("0000 0010 11-- ----");
        __________MMMXXX(opcode, CHK2, 0b001001111110, Word, Chk2, CIMS);

        opcode = parse("0000 0100 11-- ----");
        __________MMMXXX(opcode, CHK2, 0b001001111110, Long, Chk2, CIMS);
    }


    // CLR
    //
    //       Syntax: CLR <ea>
    //         Size: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0100 0010 ---- ----");
    ________SSMMMXXX(opcode, CLR, 0b101111111000, Byte | Word | Long, Clr, CIMS);
    ________SSMMMXXX(opcode, CLR, 0b001110000000, Byte | Word | Long, Clr, CIMSloop);


    // CMP
    //
    //       Syntax: CMP <ea>,Dy
    //         Size: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>,Dy       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X  (X)  X   X   X   X   X   X   X   X   X   X

    opcode = parse("1011 ---0 ---- ----");
    ____XXX_SSMMMXXX(opcode, CMP, 0b101111111111, Byte,        Cmp, IMS);
    ____XXX_SSMMMXXX(opcode, CMP, 0b111111111111, Word | Long, Cmp, IMS);


    // CMPA
    //
    //       Syntax: CMPA <ea>,Ay
    //         Size: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X   X   X   X   X   X   X   X   X   X   X   X

    opcode = parse("1011 ---- 11-- ----");
    ____XXXS__MMMXXX(opcode, CMPA, 0b111111111111, Word | Long, Cmpa, IMS);
    ____XXXS__MMMXXX(opcode, CMPA, 0b001110000000, Word | Long, Cmpa, IMSloop)


    // CMPI
    //
    //       Syntax: CMPI #<data>,<ea>
    //         Size: Byte, Word, Longword
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 1100 ---- ----");
    ________SSMMMXXX(opcode, CMPI, 0b100000000000, Byte | Word | Long, CmpiRg, CIMS);
    ________SSMMMXXX(opcode, CMPI, 0b001111111000, Byte | Word | Long, CmpiEa, IMS);

    if (model >= M68010) {

        ________SSMMMXXX(opcode, CMPI, 0b000000000110, Byte | Word | Long, CmpiEa, IMS);
    }


    // CMPM
    //
    //       Syntax: (1) CMPM (Ax)+,(Ay)+
    //         Size: Byte, Word, Longword

    // (Ax)+,(Ay)+
    opcode = parse("1011 ---1 --00 1---");
    ____XXX_SS___XXX(opcode, CMPM, MODE_PI, Byte | Word | Long, Cmpm, IMS);


    // DBcc
    //
    //       Syntax: DBcc Dn,<label>
    //         Size: Word

    // Dn,<label>
    opcode = parse("0101 ---- 1100 1---");
    _____________XXX(opcode | 0x000, DBT,  MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0x100, DBF,  MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0x200, DBHI, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0x300, DBLS, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0x400, DBCC, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0x500, DBCS, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0x600, DBNE, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0x700, DBEQ, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0x800, DBVC, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0x900, DBVS, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0xA00, DBPL, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0xB00, DBMI, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0xC00, DBGE, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0xD00, DBLT, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0xE00, DBGT, MODE_IP, Word, Dbcc, CIMS);
    _____________XXX(opcode | 0xF00, DBLE, MODE_IP, Word, Dbcc, CIMS);

    _____________XXX(opcode | 0x000, DBT,  MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0x100, DBF,  MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0x200, DBHI, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0x300, DBLS, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0x400, DBCC, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0x500, DBCS, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0x600, DBNE, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0x700, DBEQ, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0x800, DBVC, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0x900, DBVS, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0xA00, DBPL, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0xB00, DBMI, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0xC00, DBGE, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0xD00, DBLT, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0xE00, DBGT, MODE_IP, Word, Dbcc, CIMSloop);
    _____________XXX(opcode | 0xF00, DBLE, MODE_IP, Word, Dbcc, CIMSloop);


    // DIVS, DIVU
    //
    //       Syntax: DIVx <ea>,Dy
    //        Sizes: Longword, Word -> Longword

    //               -------------------------------------------------
    // <ea>,Dn       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X   X

    opcode = parse("1000 ---1 11-- ----");
    ____XXX___MMMXXX(opcode, DIVS, 0b101111111111, Word, Div, CIMS);

    opcode = parse("1000 ---0 11-- ----");
    ____XXX___MMMXXX(opcode, DIVU, 0b101111111111, Word, Div, CIMS);

    if (model >= M68020) {

        opcode = parse("0100 1100 01-- ----");
        __________MMMXXX(opcode, DIVL, 0b101111111111, Long, Divl, CIMS);
    }


    // EOR
    //
    //       Syntax: EOR Dx,<ea>
    //        Sizes: Byte, Word, Longword

    //               -------------------------------------------------
    // Dx,<ea>       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("1011 ---1 ---- ----");
    ____XXX_SSMMMXXX(opcode, EOR, 0b101111111000, Byte | Word | Long, AndRgEa, IMS);
    ____XXX_SSMMMXXX(opcode, EOR, 0b001110000000, Byte | Word | Long, AndRgEa, IMSloop);


    // EORI
    //
    //       Syntax: EORI #<data>,<ea>
    //         Size: Byte, Word, Longword
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 1010 ---- ----");
    ________SSMMMXXX(opcode, EORI, 0b100000000000, Byte | Word | Long, AndiRg, CIMS);
    ________SSMMMXXX(opcode, EORI, 0b001111111000, Byte | Word | Long, AndiEa, IMS);


    // EORI to CCR
    //
    //       Syntax: EORI #<data>,CCR
    //         Size: Byte
    //

    opcode = parse("0000 1010 0011 1100");
    ________________(opcode, EORICCR, MODE_IM, Byte, Andiccr, CIMS);


    // EORI to SR
    //
    //       Syntax: EORI #<data>,SR
    //         Size: Byte
    //

    opcode = parse("0000 1010 0111 1100");
    ________________(opcode, EORISR, MODE_IM, Word, Andisr, CIMS);


    // EXG
    //
    //       Syntax: EXG Dx,Dy
    //               EXG Ax,Dy
    //               EXG Ax,Ay
    //         Size: Longword

    opcode = parse("1100 ---1 0100 0---");
    ____XXX______XXX(opcode, EXG, MODE_IP, Long, ExgDxDy, IMS);

    opcode = parse("1100 ---1 1000 1---");
    ____XXX______XXX(opcode, EXG, MODE_IP, Long, ExgAxDy, IMS);

    opcode = parse("1100 ---1 0100 1---");
    ____XXX______XXX(opcode, EXG, MODE_IP, Long, ExgAxAy, IMS);


    // EXT
    //
    //       Syntax: EXT Dx
    //        Sizes: Word, Longword

    opcode = parse("0100 1000 --00 0---");
    _____________XXX(opcode | 2 << 6, EXT, MODE_DN, Word, Ext, IMS);
    _____________XXX(opcode | 3 << 6, EXT, MODE_DN, Long, Ext, IMS);


    // EXTB
    //
    //       Syntax: EXTB Dx
    //        Sizes: Longword

    if (model >= M68020) {

        opcode = parse("0100 1001 --00 0---");
        _____________XXX(opcode | 3 << 6, EXTB, MODE_DN, Long, Extb, IMS);
    }


    // LINK
    //
    //       Syntax: LINK An,#<displacement>
    //        Sizes: Word, (Longword)

    opcode = parse("0100 1110 0101 0---");
    _____________XXX(opcode, LINK, MODE_IP, Word, Link, IMS);

    if (model >= M68020) {

        opcode = parse("0100 1000 0000 1---");
        _____________XXX(opcode, LINK, MODE_IP, Long, Link, IMS);
    }


    // JMP
    //
    //       Syntax: JMP <ea>
    //        Sizes: Longword

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X           X   X   X   X   X   X

    opcode = parse("0100 1110 11-- ----");
    __________MMMXXX(opcode, JMP, 0b001001111110, Long, Jmp, IMS);


    // JSR
    //
    //       Syntax: JSR <ea>
    //        Sizes: Longword

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X           X   X   X   X   X   X

    opcode = parse("0100 1110 10-- ----");
    __________MMMXXX(opcode, JSR, 0b001001111110, Long, Jsr, IMS);


    // LEA
    //
    //       Syntax: LEA <ea>,Ay
    //        Sizes: Longword

    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X           X   X   X   X   X   X

    opcode = parse("0100 ---1 11-- ----");
    ____XXX___MMMXXX(opcode, LEA, 0b001001111110, Long, Lea, IMS);


    // LSL, LSR
    //
    //       Syntax: (1) LSx Dx,Dy
    //               (2) LSx #<data>,Dy
    //               (3) LSx <ea>
    //        Sizes: Byte, Word, Longword

    // Dx,Dy
    opcode = parse("1110 ---1 --10 1---");
    ____XXX_SS___XXX(opcode, LSL, MODE_DN, Byte | Word | Long, ShiftRg, IMS);

    opcode = parse("1110 ---0 --10 1---");
    ____XXX_SS___XXX(opcode, LSR, MODE_DN, Byte | Word | Long, ShiftRg, IMS);

    // #<data>,Dy
    opcode = parse("1110 ---1 --00 1---");
    ____XXX_SS___XXX(opcode, LSL, MODE_IM, Byte | Word | Long, ShiftIm, IMS);

    opcode = parse("1110 ---0 --00 1---");
    ____XXX_SS___XXX(opcode, LSR, MODE_IM, Byte | Word | Long, ShiftIm, IMS);

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X   X   X   X   X   X   X

    opcode = parse("1110 0011 11-- ----");
    __________MMMXXX(opcode, LSL, 0b001111111000, Word, ShiftEa, IMS);
    __________MMMXXX(opcode, LSL, 0b001110000000, Word, ShiftEa, IMSloop);

    opcode = parse("1110 0010 11-- ----");
    __________MMMXXX(opcode, LSR, 0b001111111000, Word, ShiftEa, IMS);
    __________MMMXXX(opcode, LSR, 0b001110000000, Word, ShiftEa, IMSloop);


    // MOVE
    //
    //       Syntax: MOVE <ea>,<e>
    //        Sizes: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X  (X)   X   X   X   X   X   X   X   X   X   X

    //               -------------------------------------------------
    // <e>           | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    // <ea>,Dy
    opcode = parse("00-- ---0 00-- ----");
    __SSXXX___MMMXXX(opcode, MOVE, 0b101111111111, Byte,        Move0, IMS);
    __SSXXX___MMMXXX(opcode, MOVE, 0b111111111111, Word | Long, Move0, IMS);

    // <ea>,(Ay)
    opcode = parse("00-- ---0 10-- ----");
    __SSXXX___MMMXXX(opcode, MOVE, 0b101111111111, Byte,        Move2, IMS);
    __SSXXX___MMMXXX(opcode, MOVE, 0b111111111111, Word | Long, Move2, IMS);
    __SSXXX___MMMXXX(opcode, MOVE, 0b101110000000, Byte,        Move2, IMSloop);
    __SSXXX___MMMXXX(opcode, MOVE, 0b111110000000, Word | Long, Move2, IMSloop);

    // <ea>,(Ay)+
    opcode = parse("00-- ---0 11-- ----");
    __SSXXX___MMMXXX(opcode, MOVE, 0b101111111111, Byte,        Move3, IMS);
    __SSXXX___MMMXXX(opcode, MOVE, 0b111111111111, Word | Long, Move3, IMS);
    __SSXXX___MMMXXX(opcode, MOVE, 0b101110000000, Byte,        Move3, IMSloop);
    __SSXXX___MMMXXX(opcode, MOVE, 0b111110000000, Word | Long, Move3, IMSloop);

    // <ea>,-(Ay)
    opcode = parse("00-- ---1 00-- ----");
    __SSXXX___MMMXXX(opcode, MOVE, 0b101111111111, Byte,        Move4, IMS);
    __SSXXX___MMMXXX(opcode, MOVE, 0b111111111111, Word | Long, Move4, IMS);
    __SSXXX___MMMXXX(opcode, MOVE, 0b101110000000, Byte,        Move4, IMSloop);
    __SSXXX___MMMXXX(opcode, MOVE, 0b111110000000, Word | Long, Move4, IMSloop);

    // <ea>,(d,Ay)
    opcode = parse("00-- ---1 01-- ----");
    __SSXXX___MMMXXX(opcode, MOVE, 0b101111111111, Byte,        Move5, IMS);
    __SSXXX___MMMXXX(opcode, MOVE, 0b111111111111, Word | Long, Move5, IMS);

    // <ea>,(d,Ay,Xi)
    opcode = parse("00-- ---1 10-- ----");
    __SSXXX___MMMXXX(opcode, MOVE, 0b101111111111, Byte,        Move6, IMS);
    __SSXXX___MMMXXX(opcode, MOVE, 0b111111111111, Word | Long, Move6, IMS);

    // <ea>,ABS.w
    opcode = parse("00-- 0001 11-- ----");
    __SS______MMMXXX(opcode, MOVE, 0b101111111111, Byte,        Move7, IMS);
    __SS______MMMXXX(opcode, MOVE, 0b111111111111, Word | Long, Move7, IMS);

    // <ea>,ABS.l
    opcode = parse("00-- 0011 11-- ----");
    __SS______MMMXXX(opcode, MOVE, 0b101111111111, Byte,        Move8, IMS);
    __SS______MMMXXX(opcode, MOVE, 0b111111111111, Word | Long, Move8, IMS);


    // MOVEA
    //
    //       Syntax: MOVEA <ea>,Ay
    //        Sizes: Word, Longword

    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X   X   X   X   X   X   X   X   X   X   X   X

    opcode = parse("001- ---0 01-- ----");

    ____XXX___MMMXXX(opcode | 0 << 12, MOVEA, 0b111111111111, Long, Movea, IMS)
    ____XXX___MMMXXX(opcode | 1 << 12, MOVEA, 0b111111111111, Word, Movea, IMS)


    // MOVEC
    //
    //       Syntax: MOVEC Rc,Rx
    //               MOVEC Rx,Rc
    //        Sizes: Longword

    if (model >= M68010) {

        opcode = parse("0100 1110 0111 101-");
        ________________(opcode | 0, MOVEC, MODE_IP, Long, MovecRcRx, IMS);
        ________________(opcode | 1, MOVEC, MODE_IP, Long, MovecRxRc, IMS);
    }


    // MOVEM
    //
    //       Syntax: MOVEM <ea>,<register list>
    //               MOVEM <register list>,<ea>
    //        Sizes: Word, Longword

    //               -------------------------------------------------
    // <ea>,<list>   | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X   X       X   X   X   X   X   X

    //               -------------------------------------------------
    // <list>,<ea>   | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X       X   X   X   X   X

    opcode = parse("0100 1100 1--- ----");
    __________MMMXXX(opcode | 0 << 6, MOVEM, 0b001101111110, Word, MovemEaRg, IMS);
    __________MMMXXX(opcode | 1 << 6, MOVEM, 0b001101111110, Long, MovemEaRg, IMS);

    opcode = parse("0100 1000 1--- ----");
    __________MMMXXX(opcode | 0 << 6, MOVEM, 0b001011111000, Word, MovemRgEa, IMS);
    __________MMMXXX(opcode | 1 << 6, MOVEM, 0b001011111000, Long, MovemRgEa, IMS);


    // MOVEP
    //
    //       Syntax: MOVEP Dx,(d,Ay)
    //               MOVEP (d,Ay),Dx
    //         Size: Word, Longword

    // MOVEP Dx,(d,Ay)
    opcode = parse("0000 ---1 1-00 1---");
    ____XXX______XXX(opcode | 0 << 6, MOVEP, MODE_DI, Word, MovepDxEa, IMS);
    ____XXX______XXX(opcode | 1 << 6, MOVEP, MODE_DI, Long, MovepDxEa, IMS);

    // MOVEP (d,Ay),Dx
    opcode = parse("0000 ---1 0-00 1---");
    ____XXX______XXX(opcode | 0 << 6, MOVEP, MODE_DI, Word, MovepEaDx, IMS);
    ____XXX______XXX(opcode | 1 << 6, MOVEP, MODE_DI, Long, MovepEaDx, IMS);


    // MOVEQ
    //
    //       Syntax: MOVEQ #<data>,Dn
    //        Sizes: Longword

    // #<data>,Dn
    opcode = parse("0111 ---0 ---- ----");
    ____XXX_XXXXXXXX(opcode, MOVEQ, MODE_IM, Long, Moveq, IMS);


    // MOVES
    //
    //       Syntax: MOVES Dx,<ea>
    //        Sizes: Byte, Word, Longword

    //               -------------------------------------------------
    // Dx,<ea>       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X   X   X   X   X   X   X

    if (model >= M68010) {

        opcode = parse("0000 1110 ---- ----");
        ________SSMMMXXX(opcode, MOVES, 0b001111111000, Byte | Word | Long, Moves, IMS);
    }


    // MOVE from CCR
    //
    //       Syntax: MOVE CCR,<ea>
    //         Size: Word
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    if (model >= M68010) {

        opcode = parse("0100 0010 11-- ----");
        __________MMMXXX(opcode, MOVEFCCR, 0b100000000000, Word, MoveFromCcrRg, IMS);
        __________MMMXXX(opcode, MOVEFCCR, 0b001111111000, Word, MoveFromCcrEa, IMS);
    }


    // MOVE to CCR
    //
    //       Syntax: MOVE <ea>,CCR
    //         Size: Word
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X   X

    opcode = parse("0100 0100 11-- ----");
    __________MMMXXX(opcode, MOVETCCR, 0b101111111111, Word, MoveToCcr, IMS);


    // MOVE from SR
    //
    //       Syntax: MOVE SR,<ea>
    //         Size: Word
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0100 0000 11-- ----");

    __________MMMXXX(opcode, MOVEFSR, 0b100000000000, Word, MoveFromSrRg, CIMS);
    __________MMMXXX(opcode, MOVEFSR, 0b001111111000, Word, MoveFromSrEa, CIMS);


    // MOVE to SR
    //
    //       Syntax: MOVE <ea>,SR
    //         Size: Word
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X   X

    opcode = parse("0100 0110 11-- ----");
    __________MMMXXX(opcode, MOVETSR, 0b101111111111, Word, MoveToSr, IMS);


    // MOVEUSP
    //
    //       Syntax: MOVE USP,An
    //               MOVE An,USP
    //        Sizes: Longword

    opcode = parse("0100 1110 0110 ----");
    _____________XXX(opcode | 1 << 3, MOVEUSP, MODE_IP, Long, MoveUspAn, IMS);
    _____________XXX(opcode | 0 << 3, MOVEUSP, MODE_IP, Long, MoveAnUsp, IMS);


    // MULS, MULU
    //
    //       Syntax: MULx <ea>,Dy
    //        Sizes: Word x Word -> Longword

    //               -------------------------------------------------
    // <ea>,Dy       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X   X

    opcode = parse("1100 ---1 11-- ----");
    ____XXX___MMMXXX(opcode, MULS, 0b101111111111, Word, Mul, CIMS);

    opcode = parse("1100 ---0 11-- ----");
    ____XXX___MMMXXX(opcode, MULU, 0b101111111111, Word, Mul, CIMS);

    if (model >= M68020) {

        opcode = parse("0100 1100 00-- ----");
        __________MMMXXX(opcode, MULL, 0b101111111111, Long, Mull, CIMS);
    }


    // NBCD
    //
    //       Syntax: NBCD <ea>
    //        Sizes: Byte

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0100 1000 00-- ----");
    __________MMMXXX(opcode, NBCD, 0b101111111000, Byte, Nbcd, IMS);
    __________MMMXXX(opcode, NBCD, 0b001110000000, Byte, Nbcd, IMSloop);


    // NEG, NEGX, NOT
    //
    //       Syntax: Nxx <ea>
    //        Sizes: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0100 0100 ---- ----");
    ________SSMMMXXX(opcode, NEG, 0b100000000000, Byte | Word | Long, NegRg, IMS);
    ________SSMMMXXX(opcode, NEG, 0b001111111000, Byte | Word | Long, NegEa, IMS);
    ________SSMMMXXX(opcode, NEG, 0b001110000000, Byte | Word | Long, NegEa, IMSloop);

    opcode = parse("0100 0000 ---- ----");
    ________SSMMMXXX(opcode, NEGX, 0b100000000000, Byte | Word | Long, NegRg, IMS);
    ________SSMMMXXX(opcode, NEGX, 0b001111111000, Byte | Word | Long, NegEa, IMS);
    ________SSMMMXXX(opcode, NEGX, 0b001110000000, Byte | Word | Long, NegEa, IMSloop);

    opcode = parse("0100 0110 ---- ----");
    ________SSMMMXXX(opcode, NOT, 0b100000000000, Byte | Word | Long, NegRg, IMS);
    ________SSMMMXXX(opcode, NOT, 0b001111111000, Byte | Word | Long, NegEa, IMS);
    ________SSMMMXXX(opcode, NOT, 0b001110000000, Byte | Word | Long, NegEa, IMSloop);


    // NOP
    //
    //       Syntax: NOP
    //        Sizes: Unsized

    opcode = parse("0100 1110 0111 0001");
    ________________(opcode, NOP, MODE_IP, Long, Nop, IMS);


    // OR
    //
    //       Syntax: OR <ea>,Dy
    //               OR Dx,<ea>
    //        Sizes: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>,Dy       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X   X

    opcode = parse("1000 ---0 ---- ----");
    ____XXX_SSMMMXXX(opcode, OR, 0b101111111111, Byte | Word | Long, AndEaRg, IMS);
    ____XXX_SSMMMXXX(opcode, OR, 0b001110000000, Byte,        AddEaRg, IMSloop);
    ____XXX_SSMMMXXX(opcode, OR, 0b001110000000, Word | Long, AddEaRg, IMSloop);


    //               -------------------------------------------------
    // Dx,<ea>       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X   X   X   X

    opcode = parse("1000 ---1 ---- ----");
    ____XXX_SSMMMXXX(opcode, OR, 0b001111111000, Byte | Word | Long, AndRgEa, IMS);
    ____XXX_SSMMMXXX(opcode, OR, 0b001110000000, Byte | Word | Long, AndRgEa, IMSloop);


    // ORI
    //
    //       Syntax: ORI #<data>,<ea>
    //         Size: Byte, Word, Longword
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 0000 ---- ----");
    ________SSMMMXXX(opcode, ORI, 0b100000000000, Byte | Word | Long, AndiRg, CIMS);
    ________SSMMMXXX(opcode, ORI, 0b001111111000, Byte | Word | Long, AndiEa, IMS);


    // ORI to CCR
    //
    //       Syntax: ORI #<data>,CCR
    //         Size: Byte
    //

    opcode = parse("0000 0000 0011 1100");
    ________________(opcode, ORICCR, MODE_IM, Byte, Andiccr, CIMS);


    // ORI to SR
    //
    //       Syntax: ORI #<data>,SR
    //         Size: Byte
    //

    opcode = parse("0000 0000 0111 1100");
    ________________(opcode, ORISR, MODE_IM, Word, Andisr, CIMS);


    // ROL, ROR, ROXL, ROXR
    //
    //       Syntax: (1) ROxx Dx,Dy
    //               (2) ROxx #<data>,Dy
    //               (3) ROxx <ea>
    //        Sizes: Byte, Word, Longword

    // Dx,Dy
    opcode = parse("1110 ---1 --11 1---");
    ____XXX_SS___XXX(opcode, ROL, MODE_DN, Byte | Word | Long, ShiftRg, IMS);

    opcode = parse("1110 ---0 --11 1---");
    ____XXX_SS___XXX(opcode, ROR, MODE_DN, Byte | Word | Long, ShiftRg, IMS);

    opcode = parse("1110 ---1 --11 0---");
    ____XXX_SS___XXX(opcode, ROXL, MODE_DN, Byte | Word | Long, ShiftRg, IMS);

    opcode = parse("1110 ---0 --11 0---");
    ____XXX_SS___XXX(opcode, ROXR, MODE_DN, Byte | Word | Long, ShiftRg, IMS);

    // #<data>,Dy
    opcode = parse("1110 ---1 --01 1---");
    ____XXX_SS___XXX(opcode, ROL, MODE_IM, Byte | Word | Long, ShiftIm, IMS);

    opcode = parse("1110 ---0 --01 1---");
    ____XXX_SS___XXX(opcode, ROR, MODE_IM, Byte | Word | Long, ShiftIm, IMS);

    opcode = parse("1110 ---1 --01 0---");
    ____XXX_SS___XXX(opcode, ROXL, MODE_IM, Byte | Word | Long, ShiftIm, IMS);

    opcode = parse("1110 ---0 --01 0---");
    ____XXX_SS___XXX(opcode, ROXR, MODE_IM, Byte | Word | Long, ShiftIm, IMS);

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X   X   X   X   X   X   X

    opcode = parse("1110 0111 11-- ----");
    __________MMMXXX(opcode, ROL, 0b001111111000, Word, ShiftEa, IMS);
    __________MMMXXX(opcode, ROL, 0b001110000000, Word, ShiftEa, IMSloop);

    opcode = parse("1110 0110 11-- ----");
    __________MMMXXX(opcode, ROR, 0b001111111000, Word, ShiftEa, IMS);
    __________MMMXXX(opcode, ROR, 0b001110000000, Word, ShiftEa, IMSloop);

    opcode = parse("1110 0101 11-- ----");
    __________MMMXXX(opcode, ROXL, 0b001111111000, Word, ShiftEa, IMS);
    __________MMMXXX(opcode, ROXL, 0b001110000000, Word, ShiftEa, IMSloop);

    opcode = parse("1110 0100 11-- ----");
    __________MMMXXX(opcode, ROXR, 0b001111111000, Word, ShiftEa, IMS);
    __________MMMXXX(opcode, ROXR, 0b001110000000, Word, ShiftEa, IMSloop);


    // PACK
    //
    //       Syntax: PACK -(Ax),-(Ay),#<adjustment>
    //               PACK DX,Dy,#<adjustment>
    //        Sizes: Unsized

    if (model >= M68020) {

        opcode = parse("1000 ---1 0100 0---");
        ____XXX______XXX(opcode, PACK, MODE_DN, Size(0), Pack, CIMS);

        opcode = parse("1000 ---1 0100 1---");
        ____XXX______XXX(opcode, PACK, MODE_PD, Size(0), Pack, CIMS);
    }


    // PEA
    //
    //       Syntax: PEA <ea>,Ay
    //        Sizes: Longword

    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X           X   X   X   X   X   X

    opcode = parse("0100 1000 01-- ----");
    __________MMMXXX(opcode, PEA, 0b001001111110, Long, Pea, IMS);


    // RESET
    //
    //       Syntax: RESET
    //        Sizes: Unsized

    opcode = parse("0100 1110 0111 0000");
    ________________(opcode, RESET, MODE_IP, Long, Reset, CIMS);


    // RTD
    //
    //       Syntax: RTD
    //        Sizes: Unsized

    if (model >= M68010) {

        opcode = parse("0100 1110 0111 0100");
        ________________(opcode, RTD, MODE_IP, Long, Rtd, IMS);
    }


    // RTE
    //
    //       Syntax: RTE
    //        Sizes: Unsized

    opcode = parse("0100 1110 0111 0011");
    ________________(opcode, RTE, MODE_IP, Long, Rte, CIMS);


    // RTM
    //
    //       Syntax: RTM Rn
    //        Sizes: Unsized

    if (model >= M68020) {

        opcode = parse("0000 0110 1100 ----");
        ____________XXXX(opcode, RTM, MODE_IP, Long, Rtm, CIMS);
    }


    // RTR
    //
    //       Syntax: RTR
    //        Sizes: Unsized

    opcode = parse("0100 1110 0111 0111");
    ________________(opcode, RTR, MODE_IP, Long, Rtr, IMS);


    // RTS
    //
    //       Syntax: RTS
    //        Sizes: Unsized

    opcode = parse("0100 1110 0111 0101");
    ________________(opcode, RTS, MODE_IP, Long, Rts, IMS);


    // SBCD
    //
    //       Syntax: (1) SBCD Dx,Dy
    //               (2) SBCD -(Ax),-(Ay)
    //         Size: Byte

    // Dx,Dy
    opcode = parse("1000 ---1 0000 0---");
    ____XXX______XXX(opcode, SBCD, MODE_DN, Byte, Abcd, IMS);

    // -(Ax),-(Ay)
    opcode = parse("1000 ---1 0000 1---");
    ____XXX______XXX(opcode, SBCD, MODE_PD, Byte, Abcd, IMS);
    ____XXX______XXX(opcode, SBCD, MODE_PD, Byte, Abcd, IMSloop);


    // Scc
    //
    //       Syntax: Scc <ea>
    //         Size: Word

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0101 ---- 11-- ----");
    __________MMMXXX(opcode | 0x000, ST,  0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0x100, SF,  0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0x200, SHI, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0x300, SLS, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0x400, SCC, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0x500, SCS, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0x600, SNE, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0x700, SEQ, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0x800, SVC, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0x900, SVS, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0xA00, SPL, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0xB00, SMI, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0xC00, SGE, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0xD00, SLT, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0xE00, SGT, 0b100000000000, Word, SccRg, IMS);
    __________MMMXXX(opcode | 0xF00, SLE, 0b100000000000, Word, SccRg, IMS);

    __________MMMXXX(opcode | 0x000, ST,  0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0x100, SF,  0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0x200, SHI, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0x300, SLS, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0x400, SCC, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0x500, SCS, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0x600, SNE, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0x700, SEQ, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0x800, SVC, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0x900, SVS, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0xA00, SPL, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0xB00, SMI, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0xC00, SGE, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0xD00, SLT, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0xE00, SGT, 0b001111111000, Word, SccEa, IMS);
    __________MMMXXX(opcode | 0xF00, SLE, 0b001111111000, Word, SccEa, IMS);

    // STOP
    //
    //       Syntax: STOP
    //        Sizes: Unsized

    opcode = parse("0100 1110 0111 0010");
    ________________(opcode, STOP, MODE_IP, Word, Stop, IMS);


    // SUB
    //
    //       Syntax: (1) SUB <ea>,Dy
    //               (2) SUB Dx,<ea>
    //         Size: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>,Dy       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X  (X)  X   X   X   X   X   X   X   X   X   X

    opcode = parse("1001 ---0 ---- ----");
    ____XXX_SSMMMXXX(opcode, SUB, 0b101111111111, Byte,        AddEaRg, IMS);
    ____XXX_SSMMMXXX(opcode, SUB, 0b111111111111, Word | Long, AddEaRg, IMS);
    ____XXX_SSMMMXXX(opcode, SUB, 0b001110000000, Byte,        AddEaRg, IMSloop);
    ____XXX_SSMMMXXX(opcode, SUB, 0b001110000000, Word | Long, AddEaRg, IMSloop);


    //               -------------------------------------------------
    // Dx,<ea>       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                         X   X   X   X   X   X   X

    opcode = parse("1001 ---1 ---- ----");
    ____XXX_SSMMMXXX(opcode, SUB, 0b001111111000, Byte | Word | Long, AddRgEa, IMS);
    ____XXX_SSMMMXXX(opcode, SUB, 0b001110000000, Byte | Word | Long, AddRgEa, IMSloop);


    // SUBA
    //
    //       Syntax: SUBA <ea>,Ay
    //         Size: Word, Longword
    //
    //               -------------------------------------------------
    // <ea>,Ay       | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X   X   X   X   X   X   X   X   X   X   X   X

    opcode = parse("1001 ---- 11-- ----");
    ____XXXS__MMMXXX(opcode, SUBA, 0b111111111111, Word | Long, Adda, IMS)
    ____XXXS__MMMXXX(opcode, SUBA, 0b001110000000, Word | Long, Adda, IMSloop)


    // SUBI
    //
    //       Syntax: SUBI #<data>,<ea>
    //         Size: Byte, Word, Longword
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0000 0100 ---- ----");
    ________SSMMMXXX(opcode, SUBI, 0b100000000000, Byte | Word | Long, AddiRg, CIMS);
    ________SSMMMXXX(opcode, SUBI, 0b001111111000, Byte | Word | Long, AddiEa, IMS);


    // SUBQ
    //
    //       Syntax: SUBQ #<data>,<ea>
    //         Size: Byte, Word, Longword
    //
    //               -------------------------------------------------
    // #<data>,<ea>  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X  (X)  X   X   X   X   X   X   X

    opcode = parse("0101 ---1 ---- ----");
    ____XXX_SSMMMXXX(opcode, SUBQ, 0b100000000000, Byte | Word | Long, AddqDn, IMS);
    ____XXX_SSMMMXXX(opcode, SUBQ, 0b010000000000, Word | Long,        AddqAn, IMS);
    ____XXX_SSMMMXXX(opcode, SUBQ, 0b001111111000, Byte | Word | Long, AddqEa, IMS);


    // SUBX
    //
    //       Syntax: (1) SUBX Dx,Dy
    //               (2) SUBX -(Ax),-(Ay)
    //         Size: Byte, Word, Longword

    // Dx,Dy
    opcode = parse("1001 ---1 --00 0---");
    ____XXX_SS___XXX(opcode, SUBX, MODE_DN, Byte | Word | Long, AddxRg, IMS);

    // -(Ax),-(Ay)
    opcode = parse("1001 ---1 --00 1---");
    ____XXX_SS___XXX(opcode, SUBX, MODE_PD, Byte | Word | Long, AddxEa, IMS);
    ____XXX_SS___XXX(opcode, SUBX, MODE_PD, Byte | Word | Long, AddxEa, IMSloop);


    // SWAP
    //
    //       Syntax: SWAP Dn
    //         Size: Word

    opcode = parse("0100 1000 0100 0---");
    _____________XXX(opcode, SWAP, MODE_DN, Word, Swap, IMS);


    // TAS
    //
    //       Syntax: TAS <ea>
    //         Size: Byte

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X       X   X   X   X   X   X   X

    opcode = parse("0100 1010 11-- ----");
    __________MMMXXX(opcode, TAS, 0b100000000000, Byte, TasRg, IMS);
    __________MMMXXX(opcode, TAS, 0b001111111000, Byte, TasEa, IMS);


    // TRAP
    //
    //       Syntax: TRAP #<vector>
    //        Sizes: Unsized

    opcode = parse("0100 1110 0100 ----");
    ____________XXXX(opcode, TRAP, MODE_IP, Long, Trap, IMS);


    // TRAPcc
    //
    //       Syntax: TRAPcc #<vector>
    //        Sizes: Unsized

    if (model >= M68020) {

        opcode = parse("0101 ---- 1111 1---");

        for (int m = 2; m <= 4; m++) {

            ________________(opcode | 0x000 | m, TRAPT,  MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0x100 | m, TRAPF,  MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0x200 | m, TRAPHI, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0x300 | m, TRAPLS, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0x400 | m, TRAPCC, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0x500 | m, TRAPCS, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0x600 | m, TRAPNE, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0x700 | m, TRAPEQ, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0x800 | m, TRAPVC, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0x900 | m, TRAPVS, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0xA00 | m, TRAPPL, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0xB00 | m, TRAPMI, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0xC00 | m, TRAPGE, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0xD00 | m, TRAPLT, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0xE00 | m, TRAPGT, MODE_IP, Word, Trapcc, CIMS);
            ________________(opcode | 0xF00 | m, TRAPLE, MODE_IP, Word, Trapcc, CIMS);
        }
    }


    // TRAPV
    //
    //       Syntax: TRAPV
    //        Sizes: Unsized

    opcode = parse("0100 1110 0111 0110");
    ________________(opcode, TRAPV, MODE_IP, Long, Trapv, IMS);


    // TST
    //
    //       Syntax: TST <ea>
    //         Size: Byte, Word, Longword

    //               -------------------------------------------------
    // <ea>          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B |
    //               -------------------------------------------------
    //                 X  (X)  X   X   X   X   X   X   X  (X) (X) (X)

    opcode = parse("0100 1010 ---- ----");
    ________SSMMMXXX(opcode, TST, 0b101111111000, Byte | Word | Long, Tst, IMS);
    ________SSMMMXXX(opcode, TST, 0b001110000000, Byte | Word | Long, Tst, IMSloop);

    if (model >= M68020) {

        ________SSMMMXXX(opcode, TST, 0b000000000111, Byte, Tst, IMS);
        ________SSMMMXXX(opcode, TST, 0b010000000111, Word | Long, Tst, IMS);
    }


    // UNLK
    //
    //       Syntax: UNLK An
    //        Sizes: Word

    opcode = parse("0100 1110 0101 1---");
    _____________XXX(opcode, UNLK, MODE_IP, Word, Unlk, IMS);


    // UNPK
    //
    //       Syntax: UNPK -(Ax),-(Ay),#<adjustment>
    //               UNPK DX,Dy,#<adjustment>
    //        Sizes: Unsized

    if (model >= M68020) {

        opcode = parse("1000 ---1 1000 0---");
        ____XXX______XXX(opcode, UNPK, MODE_DN, Size(0), Unpk, CIMS);

        opcode = parse("1000 ---1 1000 1---");
        ____XXX______XXX(opcode, UNPK, MODE_PD, Size(0), Unpk, CIMS);
    }
}

