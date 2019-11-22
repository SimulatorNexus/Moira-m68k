// -----------------------------------------------------------------------------
// This file is part of Moira - A Motorola 68k emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include "MoiraBase.h"

struct Reg {

    uint32_t value;

    template<Size S> uint32_t read() { return CLIP<S>(value); }
    template<Size S> void write(uint32_t v) { value = CLIP<S>(v); }
};

struct Registers {

    // Data and address registers
    union {
        struct {
            Reg d[8]; // D0, D1 ... D7
            Reg a[8]; // A0, A1 ... A7
        };
        Reg r[16];
    };

    
    // template<Size S> uint32_t readRn(unsigned n) { return CLIP<S>(r[n]); }
    // template<Size S> uint32_t readDn(unsigned n) { return CLIP<S>(d[n]); }
    // template<Size S> uint32_t readAn(unsigned n) { return CLIP<S>(a[n]); }
};

struct Flags {

    bool s;       // Supervisor flag
    bool m;       // Master / Interrupt state
    bool x;       // Extend flag
    bool n;       // Negative flag
    bool z;       // Zero flag
    bool v;       // Overflow flag
    bool c;       // Carry flag
};

class Moira {

    // The emulated CPU type
    // MoiraCpu type;

    // The data and address registers
    Registers reg;

    // The program counter  MOVE TO reg
    uint32_t pc;

    // Flags
    Flags flags;

    // Jump table storing all instruction handlers
    void (Moira::*exec[65536])(uint16_t);

    // Jump table storing all disassebler handlers
    void (Moira::*dasm[65536])(uint16_t, char *, bool);

    // Jump table storing all time information handlers
    int (Moira::*sync[65536])(uint16_t, int);

    // Disassembler
    bool hex = true;
    char str[256];

    // Instruction logic
#include "MoiraLogic.h"

    // Instruction handlers
#include "MoiraExec.h"
#include "MoiraDasm.h"
#include "MoiraSync.h"


public:
    
    Moira();

    void init();
    void process(uint16_t reg_ird);


    //
    // Running the disassembler
    //

    void disassemble(uint16_t addr, char *str);
};
