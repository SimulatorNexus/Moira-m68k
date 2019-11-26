// -----------------------------------------------------------------------------
// This file is part of Moira - A Motorola 68k emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef MOIRA_INIT_H
#define MOIRA_INIT_H

void init();

template<Instr I> void registerShiftCmd(const char *patternReg,
                                        const char *patternImm,
                                        const char *patternEa);

void registerASL();
void registerASR();
void registerLEA();
void registerLSL();
void registerLSR();
void registerROL();
void registerROR();
void registerROXL();
void registerROXR();



#endif
