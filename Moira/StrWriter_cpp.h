// -----------------------------------------------------------------------------
// This file is part of Moira - A Motorola 68k emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include <cmath>

static const char *instrLower[]
{
    "???",   "???",   "???",
    "abcd" , "add",   "adda",  "addi",  "addq",  "addx",  "and",    "andi",
    "andi" , "andi",  "asl",   "asr",
    "bcc",   "bcs",   "beq",   "bge",   "bgt",   "bhi",   "ble",    "bls",
    "blt",   "bmi",   "bne",   "bpl",   "bvc",   "bvs",
    "bfchg", "bfclr", "bfexts","bfextu","bfffo", "bfins", "bfset",  "bftst",
    "bchg",  "bclr",
    "bkpt",  "bra",   "bset",  "bsr",   "btst",
    "callm", "cas",   "cas2",  "chk",   "chk2",  "clr",   "cmp",    "cmp2",
    "cmpa",  "cmpi",  "cmpm",
    "dbcc",  "dbcs",  "dbeq",  "dbge",  "dbgt",  "dbhi",  "dble",   "dbls",
    "dblt",  "dbmi",  "dbne",  "dbpl",  "dbvc",  "dbvs",  "dbra",   "dbt",
    "divl",  "divs",  "divu",
    "eor",   "eori",  "eori",  "eori",  "exg",   "ext",   "extb",
    "b",     "db",    "gen",   "restore", "save", "s",    "trap",
    "jmp",   "jsr",
    "lea",   "link",  "lsl",   "lsr",
    "move" , "movea", "movec", "move",  "move",  "move",  "move",  "move",
    "movem", "movep", "moveq", "moves", "mull",  "muls",  "mulu",
    "nbcd",  "neg",   "negx",  "nop",   "not",
    "or"   , "ori",   "ori",   "ori",
    "pack",  "pea",
    "reset", "rol",   "ror",   "roxl",  "roxr",  "rtd",   "rte",   "rtm",
    "rtr",   "rts",
    "sbcd",  "scc",   "scs",   "seq",   "sge",   "sgt",   "shi",   "sle",
    "sls",   "slt",   "smi",   "sne",   "spl",   "svc",   "svs",   "sf",
    "st",    "stop",  "sub",   "suba",  "subi",  "subq",  "subx",  "swap",
    "tas",   "trap",  "trapv",
    "trapcc","trapcs","trapeq","trapge","trapgt","traphi","traple","trapls",
    "traplt","trapmi","trapne","trappl","trapvc","trapvs","trapf", "trapt",
    "tst",   "unlk",  "unpk"
};

static const char *instrUpper[]
{
    "???",   "???",   "???",
    "ABCD" , "ADD",   "ADDA",  "ADDI",  "ADDQ",  "ADDX",  "AND",    "ANDI",
    "ANDI" , "ANDI",  "ASL",   "ASR",
    "BCC",   "BCS",   "BEQ",   "BGE",   "BGT",   "BHI",   "BLE",    "BLS",
    "BLT",   "BMI",   "BNE",   "BPL",   "BVC",   "BVS",
    "BFCHG", "BFCLR", "BFEXTS","BFEXTU","BFFFO", "BFINS", "BFSET",  "BFTST",
    "BCHG",  "BCLR",
    "BKPT",  "BRA",   "BSET",  "BSR",   "BTST",
    "CALLM", "CAS",   "CAS2",  "CHK",   "CHK2",  "CLR",   "CMP",    "CMP2",
    "CMPA",  "CMPI",  "CMPM",
    "DBCC",  "DBCS",  "DBEQ",  "DBGE",  "DBGT",  "DBHI",  "DBLE",   "DBLS",
    "DBLT",  "DBMI",  "DBNE",  "DBPL",  "DBVC",  "DBVS",  "DBRA",   "DBT",
    "DIVL",  "DIVS",  "DIVU",
    "EOR",   "EORI",  "EORI",  "EORI",  "EXG",   "EXT",   "EXTB",
    "B",     "DB",    "GEN",   "RESTORE", "SAVE", "S", "TRAP",
    "JMP",   "JSR",
    "LEA",   "LINK",  "LSL",   "LSR",
    "MOVE" , "MOVEA", "MOVEC", "MOVE",  "MOVE",  "MOVE",  "MOVE",  "MOVE",
    "MOVEM", "MOVEP", "MOVEQ", "MOVES", "MULL",  "MULS",  "MULU",
    "NBCD",  "NEG",   "NEGX",  "NOP",   "NOT",
    "OR"   , "ORI",   "ORI",   "ORI",
    "PACK",  "PEA",
    "RESET", "ROL",   "ROR",   "ROXL",  "ROXR",  "RTD",   "RTE",   "RTM",
    "RTR",   "RTS",
    "SBCD",  "SCC",   "SCS",   "SEQ",   "SGE",   "SGT",   "SHI",   "SLE",
    "SLS",   "SLT",   "SMI",   "SNE",   "SPL",   "SVC",   "SVS",   "SF",
    "ST",    "STOP",  "SUB",   "SUBA",  "SUBI",  "SUBQ",  "SUBX",  "SWAP",
    "TAS",   "TRAP",  "TRAPV",
    "TRAPCC","TRAPCS","TRAPEQ","TRAPGE","TRAPGT","TRAPHI","TRAPLE","TRAPLS",
    "TRAPLT","TRAPMI","TRAPNE","TRAPPL","TRAPVC","TRAPVS","TRAPF", "TRAPT",
    "TST",   "UNLK",  "UNPK"
};

static int decDigits(u64 value) { return value ? 1 + (int)log10(value) : 1; }
static int binDigits(u64 value) { return value ? 1 + (int)log2(value) : 1; }
static int hexDigits(u64 value) { return (binDigits(value) + 3) / 4; }

static void sprintd(char *&s, u64 value, int digits)
{
    for (int i = digits - 1; i >= 0; i--) {
        u8 digit = value % 10;
        s[i] = '0' + digit;
        value /= 10;
    }
    s += digits;
}

static void sprintd(char *&s, u64 value)
{
    sprintd(s, value, decDigits(value));
}

static void sprintd_signed(char *&s, i64 value)
{
    if (value < 0) { *s++ = '-'; value *= -1; }
    sprintd(s, value, decDigits(value));
}

static void sprintx(char *&s, u64 value, bool upper, const char *prefix, int digits)
{
    char a = (upper ? 'A' : 'a') - 10;

    if (prefix[0]) { *s++ = prefix[0]; if (prefix[1]) { *s++ = prefix[1]; }}

    for (int i = digits - 1; i >= 0; i--) {
        char digit = (char)(value % 16);
        s[i] = (digit <= 9) ? ('0' + digit) : (a + digit);
        value /= 16;
    }
    s += digits;
}

static void sprintx(char *&s, u64 value, bool upper, const char *prefix)
{
    sprintx(s, value, upper, prefix, hexDigits(value));
}

static void sprintx_signed(char *&s, i64 value, bool upper, const char *prefix)
{
    if (value < 0) { *s++ = '-'; value *= -1; }
    sprintx(s, value, upper, prefix, hexDigits(value));
}

StrWriter&
StrWriter::operator<<(const char *str)
{
    while (*str) { *ptr++ = *str++; };
    return *this;
}

StrWriter&
StrWriter::operator<<(int value)
{
    sprintd(ptr, value);
    return *this;
}

StrWriter&
StrWriter::operator<<(Int i)
{
    switch (nf) {

        case DASM_HEX:      sprintx_signed(ptr, i.raw, upper, "$"); break;
        case DASM_HEX_0X:   sprintx_signed(ptr, i.raw, upper, i.raw ? "0x" : ""); break;
        case DASM_DEC:      sprintd_signed(ptr, i.raw); break;
    }

    return *this;
}

StrWriter&
StrWriter::operator<<(UInt u)
{
    switch (nf) {

        case DASM_HEX:      sprintx(ptr, u.raw, upper, "$"); break;
        case DASM_HEX_0X:   sprintx(ptr, u.raw, upper, u.raw ? "0x" : ""); break;
        case DASM_DEC:      sprintd(ptr, u.raw); break;
    }

    return *this;
}

StrWriter&
StrWriter::operator<<(UInt8 u)
{
    switch (nf) {

        case DASM_HEX:      sprintx(ptr, u.raw, upper, "$", 2); break;
        case DASM_HEX_0X:   sprintx(ptr, u.raw, upper, u.raw ? "0x" : "", 2); break;
        case DASM_DEC:      sprintd(ptr, u.raw, 3); break;
    }

    return *this;
}

StrWriter&
StrWriter::operator<<(UInt16 u)
{
    switch (nf) {

        case DASM_HEX:      sprintx(ptr, u.raw, upper, "$", 4); break;
        case DASM_HEX_0X:   sprintx(ptr, u.raw, upper, u.raw ? "0x" : "", 4); break;
        case DASM_DEC:      sprintd(ptr, u.raw, 5); break;
    }

    return *this;
}

StrWriter&
StrWriter::operator<<(UInt32 u)
{
    switch (nf) {

        case DASM_HEX:      sprintx(ptr, u.raw, upper, "$", 8); break;
        case DASM_HEX_0X:   sprintx(ptr, u.raw, upper, u.raw ? "0x" : "", 8); break;
        case DASM_DEC:      sprintd(ptr, u.raw, 10); break;
    }

    return *this;
}

StrWriter&
StrWriter::operator<<(Dn dn)
{
    if (style == DASM_MUSASHI) {

        *ptr++ = 'D';
        *ptr++ = '0' + (char)dn.raw;

    } else {

        *ptr++ = 'd';
        *ptr++ = '0' + (char)dn.raw;
    }

    return *this;
}

StrWriter&
StrWriter::operator<<(An an)
{
    if (style == DASM_MUSASHI) {

        *ptr++ = 'A';
        *ptr++ = '0' + (char)an.raw;

    } else {

        if (an.raw == 7) {

            *ptr++ = 's';
            *ptr++ = 'p';

        } else {

            *ptr++ = 'a';
            *ptr++ = '0' + (char)an.raw;
        }
    }

    return *this;
}

StrWriter&
StrWriter::operator<<(Rn rn)
{
    if (rn.raw < 8) {
        *this << Dn{rn.raw};
    } else {
        *this << An{rn.raw - 8};
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Cn cn)
{
    switch(cn.raw) {

        case 0x000: *this << "SFC";   break;
        case 0x001: *this << "DFC";   break;
        case 0x800: *this << "USP";   break;
        case 0x801: *this << "VBR";   break;
        case 0x002: *this << "CACR";  break;
        case 0x802: *this << "CAAR";  break;
        case 0x803: *this << "MSP";   break;
        case 0x804: *this << "ISP";   break;
        case 0x003: *this << "TC";    break;
        case 0x004: *this << "ITT0";  break;
        case 0x005: *this << "ITT1";  break;
        case 0x006: *this << "DTT0";  break;
        case 0x007: *this << "DTT1";  break;
        case 0x805: *this << "MMUSR"; break;
        case 0x806: *this << "URP";   break;
        case 0x807: *this << "SRP";   break;

        default:
            *this << UInt(cn.raw);
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Cnd cnd)
{
    switch(Cond(cnd.raw)) {

        case COND_BT: *this << (upper ? "BT" : "t");   break;
        case COND_BF: *this << (upper ? "BF" : "f");   break;
        case COND_HI: *this << (upper ? "HI" : "hi");  break;
        case COND_LS: *this << (upper ? "LS" : "ls");  break;
        case COND_CC: *this << (upper ? "CC" : "cc");  break;
        case COND_CS: *this << (upper ? "CS" : "cs");  break;
        case COND_NE: *this << (upper ? "NE" : "ne");  break;
        case COND_EQ: *this << (upper ? "EQ" : "eq");  break;
        case COND_VC: *this << (upper ? "VC" : "vc");  break;
        case COND_VS: *this << (upper ? "VS" : "vs");  break;
        case COND_PL: *this << (upper ? "PL" : "pl");  break;
        case COND_MI: *this << (upper ? "MI" : "mi");  break;
        case COND_GE: *this << (upper ? "GE" : "ge");  break;
        case COND_LT: *this << (upper ? "LT" : "lt");  break;
        case COND_GT: *this << (upper ? "GT" : "gt");  break;
        case COND_LE: *this << (upper ? "LE" : "le");  break;
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Cpcc cpcc)
{
    switch(cpcc.raw) {

        case 0: *this << (upper ? "F" : "f");           break;
        case 1: *this << (upper ? "EQ" : "eq");         break;
        case 2: *this << (upper ? "OGT" : "ogt");       break;
        case 3: *this << (upper ? "OGE" : "oge");       break;
        case 4: *this << (upper ? "OLT" : "olt");       break;
        case 5: *this << (upper ? "OLE" : "ole");       break;
        case 6: *this << (upper ? "OGL" : "ogl");       break;
        case 7: *this << (upper ? "OR" : "or");         break;
        case 8: *this << (upper ? "UN" : "un");         break;
        case 9: *this << (upper ? "UEQ" : "ueq");       break;
        case 10: *this << (upper ? "UGT" : "ugt");      break;
        case 11: *this << (upper ? "UGE" : "uge");      break;
        case 12: *this << (upper ? "ULT" : "ult");      break;
        case 13: *this << (upper ? "ULE" : "ule");      break;
        case 14: *this << (upper ? "NE" : "ne");        break;
        case 15: *this << (upper ? "T" : "t");          break;
        case 16: *this << (upper ? "SF" : "sf");        break;
        case 17: *this << (upper ? "SEQ" : "seq");      break;
        case 18: *this << (upper ? "GT" : "gt");        break;
        case 19: *this << (upper ? "GE" : "ge");        break;
        case 20: *this << (upper ? "LT" : "lt");        break;
        case 21: *this << (upper ? "LE" : "le");        break;
        case 22: *this << (upper ? "GL" : "gl");        break;
        case 23: *this << (upper ? "GLE" : "gle");      break;
        case 24: *this << (upper ? "NGLE" : "ngle");    break;
        case 25: *this << (upper ? "NGL" : "ngl");      break;
        case 26: *this << (upper ? "NLE" : "nle");      break;
        case 27: *this << (upper ? "NLT" : "nlt");      break;
        case 28: *this << (upper ? "NGE" : "nge");      break;
        case 29: *this << (upper ? "NGT" : "ngt");      break;
        case 30: *this << (upper ? "SNE" : "sne");      break;
        case 31: *this << (upper ? "ST" : "st");        break;

        default:
            *this << "?";
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Imu im)
{
    *ptr++ = '#';
    *this << UInt(im.raw);
    return *this;
}

StrWriter&
StrWriter::operator<<(Ims im)
{
    *ptr++ = '#';
    *this << Int(im.raw);
    return *this;
}

StrWriter&
StrWriter::operator<<(Imd im)
{
    *ptr++ = '#';
    sprintd(ptr, im.raw);
    return *this;
}

StrWriter&
StrWriter::operator<<(Scale s)
{
    if (s.raw) {
        *ptr++ = '*';
        *ptr++ = '0' + (char)(1 << s.raw);
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Align align)
{
    while (ptr < base + align.raw) *ptr++ = ' ';
    return *this;
}

StrWriter&
StrWriter::operator<<(RegList l)
{
    int r[16];

    // Step 1: Fill array r with the register list bits, e.g., 11101101
    for (int i = 0; i <= 15; i++) { r[i] = !!(l.raw & (1 << i)); }

    // Step 2: Convert 11101101 to 12301201
    for (int i = 1; i <= 15; i++) { if (r[i]) r[i] = r[i-1] + 1; }

    // Step 3: Convert 12301201 to 33302201
    for (int i = 14; i >= 0; i--) { if (r[i] && r[i+1]) r[i] = r[i+1]; }

    // Step 4: Convert 33302201 to "D0-D2/D4/D5/D7"
    bool first = true;
    for (int i = 0; i <= 15; i += r[i] + 1) {

        if (r[i] == 0) continue;

        // Print delimiter
        if (first) { first = false; } else { *this << "/"; }

        // Format variant 1: Single register
        if (r[i] == 1) { *this << Rn{i}; }

        // Format variant 2: Register range
        else { *this << Rn{i} << "-" << Rn{i+r[i]-1}; }
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(RegRegList l)
{
    u16 regsD = l.raw & 0x00FF;
    u16 regsA = l.raw & 0xFF00;

    *this << RegList { regsD };
    if (regsD && regsA) *this << "/";
    *this << RegList { regsA };

    return *this;
}

template <Instr I> StrWriter&
StrWriter::operator<<(Ins<I> i)
{
    *this << (upper ? instrUpper[I] : instrLower[I]);
    return *this;
}

template <Size S> StrWriter&
StrWriter::operator<<(Sz<S>)
{
    if (upper) {
        *this << ((S == Byte) ? ".B" : (S == Word) ? ".W" : ".L");
    } else {
        *this << ((S == Byte) ? ".b" : (S == Word) ? ".w" : ".l");
    }
    return *this;
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(const Ea<M,S> &ea)
{
    switch (M) {

        case 0: // Dn
        {
            *this << Dn{ea.reg};
            break;
        }
        case 1: // An
        {
            *this << An{ea.reg};
            break;
        }
        case 2: // (An)
        {
            *this << "(" << An{ea.reg} << ")";
            break;
        }
        case 3:  // (An)+
        {
            *this << "(" << An{ea.reg} << ")+";
            break;
        }
        case 4: // -(An)
        {
            *this << "-(" << An{ea.reg} << ")";
            break;
        }
        case 5: // (d,An)
        {
            if (style == DASM_MUSASHI) {

                *this << "(" << Int{(i16)ea.ext1};
                *this << "," << An{ea.reg} << ")";

            } else {

                *this << Int{(i16)ea.ext1};
                *this << "(" << An{ea.reg} << ")";
            }
            break;
        }
        case 6: // (d,An,Xi)
        {
            (ea.ext1 & 0x100) ? fullExtension(ea) : briefExtension(ea);
            break;
        }
        case 7: // ABS.W
        {
            *this << UInt(ea.ext1);
            *this << (upper ? ".W" : ".w");
            break;
        }
        case 8: // ABS.L
        {
            *this << UInt(ea.ext1);
            *this << (upper ? ".L" : ".l");
            break;
        }
        case 9: // (d,PC)
        {
            if (style == DASM_MUSASHI) {

                *this << "(" << Int{(i16)ea.ext1} << ",PC)";
                auto resolved = U32_ADD(U32_ADD(ea.pc, (i16)ea.ext1), 2);
                StrWriter(moira, comment, style, nf, upper) << "; (" << UInt(resolved) << ")" << Finish{};

            } else {

                auto resolved = U32_ADD(U32_ADD(ea.pc, (i16)ea.ext1), 2);
                *this << UInt(resolved) << "(pc)";
            }
            break;
        }
        case 10: // (d,PC,Xi)
        {
            (ea.ext1 & 0x100) ? fullExtension(ea) : briefExtension(ea);
            break;
        }
        case 11: // Imm
        {
            if (style == DASM_MUSASHI) {
                *this << Imu(ea.ext1);
            } else {
                if constexpr (S != 0) {
                    *this << Ims(SEXT<S>(ea.ext1));
                }
            }
            break;
        }
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Finish)
{
    for (int i = 0; comment[i] != 0; i++) *ptr++ = comment[i];
    *ptr = 0;
    return *this;
}

StrWriter&
StrWriter::operator<<(Sep)
{
    *ptr++ = ',';
    if (style == DASM_MUSASHI) *ptr++ = ' ';

    return *this;
}

template <Mode M, Size S> void
StrWriter::briefExtension(const Ea <M,S> &ea)
{
    assert(M == 6 || M == 10);

    //   15 - 12    11   10   09   08   07   06   05   04   03   02   01   00
    // -----------------------------------------------------------------------
    // | REGISTER | LW | SCALE   | 0  | DISPLACEMENT                         |
    // -----------------------------------------------------------------------

    u16 reg   = xxxx____________ (ea.ext1);
    u16 lw    = ____x___________ (ea.ext1);
    u16 scale = _____xx_________ (ea.ext1);
    u16 disp  = ________xxxxxxxx (ea.ext1);

    if (style == DASM_MUSASHI) {

        *this << "(";
        if (disp) *this << Int{(i8)disp} << ",";
        M == 10 ? *this << "PC" : *this << An{ea.reg};
        *this << "," << Rn{reg};
        lw ? *this << Sz<Long>{} : *this << Sz<Word>{};
        *this << Scale{scale} << ")";

    } else {

        *this << "(";
        *this << Int{(i8)disp} << ",";
        M == 10 ? *this << "pc" : *this << An{ea.reg};
        *this << "," << Rn{reg};
        lw ? *this << Sz<Long>{} : *this << Sz<Word>{};
        *this << Scale{scale} << ")";
    }
}

template <Mode M, Size S> void
StrWriter::fullExtension(const Ea <M,S> &ea)
{
    assert(M == 6 || M == 10);

    //   15 - 12    11   10   09   08   07   06   05   04   03   02   01   00
    // -----------------------------------------------------------------------
    // | REGISTER | LW | SCALE   | 1  | BS | IS | BD SIZE  | 0  | IIS        |
    // -----------------------------------------------------------------------

    u16  reg   = xxxx____________ (ea.ext1);
    u16  lw    = ____x___________ (ea.ext1);
    u16  scale = _____xx_________ (ea.ext1);
    u16  bs    = ________x_______ (ea.ext1);
    u16  is    = _________x______ (ea.ext1);
    u16  size  = __________xx____ (ea.ext1);
    u16  iis   = _____________xxx (ea.ext1);
    u32  base  = ea.ext2;
    u32  outer = ea.ext3;

    bool preindex      = (iis > 0 && iis < 4);
    bool postindex     = (iis > 4);
    bool effectiveZero = (ea.ext1 & 0xe4) == 0xC4 || (ea.ext1 & 0xe2) == 0xC0;

    if (effectiveZero) {
        *this << "0";
        return;
    }

    *this << "(";

    bool comma = false;
    if (preindex || postindex) {

        *this << "[";
    }
    if (base) {

        size == 3 ? (*this << Int{(i32)base}) : (*this << Int{(i16)base});
        comma = true;
    }
    if (style == DASM_VDA68K) {

        if (comma) *this << ",z";
        M == 10 ? (*this << "pc") : *this << An{ea.reg};
        comma = true;

    } else if (!bs) {

        if (comma) *this << ",";
        M == 10 ? *this << "PC" : *this << An{ea.reg};
        comma = true;
    }
    if (postindex) {

        *this << "]";
        comma = true;
    }
    if (!is) {

        if (comma) *this << ",";
        *this << Rn{reg};
        lw ? (*this << Sz<Long>{}) : (*this << Sz<Word>{});
        *this << Scale{scale};
        comma = true;
    } else {
        // EXPERIMENTAL
        if (style == DASM_VDA68K) {
            if (comma) *this << ",0";
            comma = true;
        }
    }
    if (preindex) {

        *this << "]";
        comma = true;
    }
    if(outer)
    {
        if (comma) *this << ",";
        *this << Int(outer);
    }

    *this << ")";
}
