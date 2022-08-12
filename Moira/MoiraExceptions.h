// -----------------------------------------------------------------------------
// This file is part of Moira - A Motorola 68k emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// Emulates a group 0 exception (DEPRECATED)
void saveToStack0(StackFrame &frame);
template <Core C> void saveToStack0(StackFrame &frame);

// Emulates a group 1 exception (DEPRECATED)
// void saveToStack1(u16 nr, u16 sr, u32 pc);
// template <Core C> void saveToStack1(u16 nr, u16 sr, u32 pc);

// Emulates a group 2 exception (DEPRECATED)
void saveToStack2(u16 nr, u16 sr, u32 pc);
template <Core C> void saveToStack2(u16 nr, u16 sr, u32 pc);

/* Creates a stack frame of a certain format
 *
 * Format  Description                  68000  68010  68020
 * ----------------------------------------------------------------------------
 * 0b0000  Short format                   X      X      X
 * 0b0001  Throwaway                                    X
 * 0b0010  Instruction exception                        X
 * 0b1000  Bus fault                             X
 * 0b1001  Coprocessor mid-instruction                  X
 * 0b1010  Short bus fault                              X
 * 0b1011  Long bus fault                               X
 */
template <Core C> void writeStackFrame0000(u16 sr, u32 pc, u16 nr);
template <Core C> void writeStackFrame0001(u16 sr, u32 pc, u16 nr);
template <Core C> void writeStackFrame0010(u16 sr, u32 pc, u32 ia, u16 nr);
template <Core C> void writeStackFrame1000(u16 sr, u32 pc, u16 nr);
template <Core C> void writeStackFrame1001(u16 sr, u32 pc, u32 ia, u16 nr);
template <Core C> void writeStackFrame1010(u16 sr, u32 pc, u16 nr);
template <Core C> void writeStackFrame1011(u16 sr, u32 pc, u16 nr);

// Emulates an exception
void execException(ExceptionType exc, int nr = 0);
template <Core C> void execException(ExceptionType exc, int nr = 0);

// Emulates an address error exception
void execAddressError(StackFrame frame, int delay = 0);
template <Core C> void execAddressError(StackFrame frame, int delay = 0);

// Emulates the execution of unimplemented and illegal instructions (DEPRECATED)
void execUnimplemented(int nr);
template <Core C> void execUnimplemented(int nr);

// Emulates an interrupt exception
void execIrqException(u8 level);
template <Core C> void execIrqException(u8 level);
