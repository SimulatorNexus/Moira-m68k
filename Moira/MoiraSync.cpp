// -----------------------------------------------------------------------------
// This file is part of Moira - A Motorola 68k emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

int
CPU::syncLineA(u16 opcode, int i)
{
    return 0;
}

int
CPU::syncLineF(u16 opcode, int i)
{
    return 0;
}

int
CPU::syncIllegal(u16 opcode, int i)
{
    return 0;
}

template<Instr I, Size S> int CPU::syncRegShift(u16 opcode, int i) {
    printf("***** syncRegShift<%d,%d>(%d,%d)\n", I, S, opcode, i);
    return i;
}

template<Instr I, Size S> int CPU::syncImmShift(u16 opcode, int i) {
    printf("***** syncImmShift<%d,%d>(%d,%d)\n", I, S, opcode, i);
    return i;
}

template<Instr I, Mode M> int CPU::syncEaShift(u16 opcode, int i) {
    printf("***** syncEaShift<%d,%d>(%d,%d)\n", I, M, opcode, i);
    return i;
}

template<Mode M> int CPU::syncLea(u16 opcode, int i) {
    printf("***** syncLea<%d>(%d,%d)\n", M, opcode, i);
    return i;
}