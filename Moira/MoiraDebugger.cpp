// -----------------------------------------------------------------------------
// This file is part of Moira - A Motorola 68k emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "Moira.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

namespace moira {

bool
Guard::eval(u32 addr)
{
    if (this->addr == addr && this->enabled) {
        if (++hits > skip) {
            hits = 0;
            return true;
        }
    }
    return false;
}

Guard *
Guards::guardWithNr(long nr)
{
    return nr < count ? &guards[nr] : NULL;
}

Guard *
Guards::guardAtAddr(u32 addr)
{
    for (int i = 0; i < count; i++) {
        if (guards[i].addr == addr) return &guards[i];
    }

    return NULL;
}

bool
Guards::isSetAt(u32 addr)
{
    Guard *guard = guardAtAddr(addr);

    return guard != NULL;
}

bool
Guards::isSetAndEnabledAt(u32 addr)
{
    Guard *guard = guardAtAddr(addr);

    return guard != NULL && guard->enabled;
}

bool
Guards::isSetAndDisabledAt(u32 addr)
{
    Guard *guard = guardAtAddr(addr);

    return guard != NULL && !guard->enabled;
}

bool
Guards::isSetAndConditionalAt(u32 addr)
{
    Guard *guard = guardAtAddr(addr);

    return guard != NULL && guard->skip != 0;
}

void
Guards::addAt(u32 addr, long skip)
{
    if (isSetAt(addr)) return;

    if (count >= capacity) {

        Guard *newguards = new Guard[2 * capacity];
        for (long i = 0; i < capacity; i++) newguards[i] = guards[i];
        delete [] guards;
        guards = newguards;
        capacity *= 2;
    }

    guards[count].addr = addr;
    guards[count].enabled = true;
    guards[count].hits = 0;
    guards[count].skip = skip;
    count++;
    needsCheck = true;
}

void
Guards::remove(long nr)
{
    if (nr < count) removeAt(guards[nr].addr);
}

void
Guards::removeAt(uint32_t addr)
{
    for (int i = 0; i < count; i++) {

        if (guards[i].addr == addr) {

            for (int j = i; j + 1 < count; j++) guards[j] = guards[j + 1];
            count--;
            break;
        }
    }
    needsCheck = count != 0;
}

bool
Guards::isEnabled(long nr)
{
    return nr < count ? guards[nr].enabled : false;
}

void
Guards::setEnable(long nr, bool val)
{
    if (nr < count) guards[nr].enabled = val;
}

void
Guards::setEnableAt(uint32_t addr, bool value)
{
    Guard *guard = guardAtAddr(addr);
    if (guard) guard->enabled = value;
}

bool
Guards::eval(u32 addr)
{
    for (int i = 0; i < count; i++)
        if (guards[i].eval(addr)) return true;

    return false;
}

Debugger::Debugger(Moira& ref) : moira(ref)
{
    // REMOVE ASAP
    // watchpoints.add(0xDFF180);
}

void
Debugger::stepInto()
{
    softStop = UINT64_MAX;
    breakpoints.needsCheck = true;
}

void
Debugger::stepOver()
{
    char tmp[64];
    softStop = moira.getPC() + moira.disassemble(moira.getPC(), tmp);
    breakpoints.needsCheck = true;
}

bool
Debugger::breakpointMatches(u32 addr)
{
    // Check if a soft breakpoint has been reached
    if (addr == softStop || softStop == UINT64_MAX) {

        // Soft breakpoints are deleted when reached
        softStop = UINT64_MAX - 1;
        breakpoints.needsCheck = breakpoints.elements() != 0;

        return true;
    }

    return breakpoints.eval(addr);
}

bool
Debugger::watchpointMatches(u32 addr)
{
    return watchpoints.eval(addr);
}

void
Debugger::enableLogging()
{
    moira.flags |= Moira::CPU_LOG_INSTRUCTION;
}

void
Debugger::disableLogging()
{
    moira.flags &= ~Moira::CPU_LOG_INSTRUCTION;
}

int
Debugger::loggedInstructions()
{
    return logCnt < LOG_BUFFER_CAPACITY ? logCnt : LOG_BUFFER_CAPACITY;
}

void
Debugger::logInstruction()
{
    logBuffer[logCnt % LOG_BUFFER_CAPACITY] = moira.reg;
    logCnt++;
}

Registers
Debugger::logEntry(int n)
{
    assert(n < loggedInstructions());

    // n == 0 returns the most recently recorded entry
    int offset = (logCnt - 1 - n) % LOG_BUFFER_CAPACITY;

    return logBuffer[offset];
}

Registers
Debugger::logEntryAbs(int n)
{
    assert(n < loggedInstructions());

    // n == 0 returns the oldest entry
    return logEntry(loggedInstructions() - n - 1);
}

}