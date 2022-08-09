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

static void sprintx(char *&s, u64 value, const DasmNumberFormat &fmt, int digits)
{
    char a = (fmt.upperCase ? 'A' : 'a') - 10;

    if (value || !fmt.plainZero) {
        for (int i = 0; fmt.prefix[i]; i++) *s++ = fmt.prefix[i];
    }

    for (int i = digits - 1; i >= 0; i--) {
        char digit = (char)(value % 16);
        s[i] = (digit <= 9) ? ('0' + digit) : (a + digit);
        value /= 16;
    }
    s += digits;
}

static void sprintx(char *&s, u64 value, const DasmNumberFormat &fmt)
{
    sprintx(s, value, fmt, hexDigits(value));
}

static void sprintx_signed(char *&s, i64 value, const DasmNumberFormat &fmt)
{
    if (value < 0) { *s++ = '-'; value *= -1; }
    sprintx(s, value, fmt, hexDigits(value));
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
    sprintd_signed(ptr, value);
    return *this;
}

StrWriter&
StrWriter::operator<<(unsigned int value)
{
    sprintd(ptr, value);
    return *this;
}

StrWriter&
StrWriter::operator<<(Int i)
{
    sprintx_signed(ptr, i.raw, nf);
    return *this;
}

StrWriter&
StrWriter::operator<<(UInt u)
{
    sprintx(ptr, u.raw, nf);
    return *this;
}

StrWriter&
StrWriter::operator<<(UInt8 u)
{
    sprintx(ptr, u.raw, nf, nf.radix == 16 ? 2 : 3);
    return *this;
}

StrWriter&
StrWriter::operator<<(UInt16 u)
{
    sprintx(ptr, u.raw, nf, nf.radix == 16 ? 4 : 5);
    return *this;
}

StrWriter&
StrWriter::operator<<(UInt32 u)
{
    sprintx(ptr, u.raw, nf, nf.radix == 16 ? 8 : 10);
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
StrWriter::operator<<(Anr an)
{
    if (style == DASM_MUSASHI) {

        *ptr++ = 'A';
        *ptr++ = '0' + (char)an.raw;

    } else {

        *ptr++ = 'a';
        *ptr++ = '0' + (char)an.raw;
    }

    return *this;
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(Ai<M,S> ea)
{
    switch (style) {

        case DASM_MIT:

            *this << An{ea.raw.reg} << "@";
            return *this;

        default:

            *this << "(" << An{ea.raw.reg} << ")";
            return *this;
    }
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(Pi<M,S> ea)
{
    switch (style) {

        case DASM_MIT:

            *this << An{ea.raw.reg} << "@+";
            return *this;

        default:

            *this << "(" << An{ea.raw.reg} << ")+";
            return *this;
    }
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(Pd<M,S> ea)
{
    switch (style) {

        case DASM_MIT:

            *this << An{ea.raw.reg} << "@-";
            return *this;

        default:

            *this << "-(" << An{ea.raw.reg} << ")";
            return *this;
    }
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(Ix<M,S> ea)
{
    return *this;

    switch (style) {

        case DASM_MOTOROLA:

            (ea.raw.ext1 & 0x100) ? fullExtensionVda68k(ea.raw) : briefExtension(ea.raw);
            return *this;

        case DASM_MIT:

            (ea.raw.ext1 & 0x100) ? fullExtensionVda68k(ea.raw) : briefExtension(ea.raw);
            return *this;

        default:

            (ea.raw.ext1 & 0x100) ? fullExtension(ea) : briefExtension(ea.raw);
            return *this;
    }
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(Aw<M,S> ea)
{
    return *this;

    switch (style) {

        case DASM_MIT:

            *this << UInt(ea.raw.ext1) << Sz<Word>{};
            return *this;

        default:

            *this << UInt(ea.raw.ext1) << Sz<Word>{};
            return *this;
    }
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(Al<M,S> ea)
{
    return *this;

    switch (style) {

        case DASM_MIT:

            *this << UInt(ea.raw.ext1) << Sz<Long>{};
            return *this;

        default:

            *this << UInt(ea.raw.ext1) << Sz<Long>{};
            return *this;
    }
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(DiPc<M,S> ea)
{
    return *this;

    u32 resolved;

    switch (style) {

        case DASM_MUSASHI:

            *this << "(" << Int{(i16)ea.raw.ext1} << ",PC)";
            resolved = U32_ADD(U32_ADD(ea.pc, (i16)ea.ext1), 2);
            StrWriter(moira, comment, style, nf) << "; (" << UInt(resolved) << ")" << Finish{};
            return *this;

        case DASM_MIT:

            resolved = U32_ADD(U32_ADD(ea.raw.pc, (i16)ea.ext1), 2);
            *this << UInt(resolved) << "(pc)";
            return *this;

        default:

            resolved = U32_ADD(U32_ADD(ea.raw.pc, (i16)ea.ext1), 2);
            *this << UInt(resolved) << "(pc)";
            return *this;
    }
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(IxPc<M,S> ea)
{
    // NOT NEEDED: Use Ix{...} instead
    return *this;
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(Im<M,S> ea)
{
    return *this;

    switch (style) {

        case DASM_MUSASHI:

            *this << Imu(ea.raw.ext1);
            return *this;

        case DASM_MIT:

            *this << "???";
            return *this;

        default:

            if constexpr (S != 0) {
                *this << Ims<S>(ea.raw.ext1);
            }
            return *this;
    }
}

template <Mode M, Size S> StrWriter&
StrWriter::operator<<(Ip<M,S> ea)
{
    return *this;

    switch (style) {

        case DASM_MIT:

            *this << An{ea.raw.reg} << "@-";
            return *this;

        default:

            *this << "-(" << An{ea.raw.reg} << ")";
            return *this;
    }

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
StrWriter::operator<<(Rnr rn)
{
    if (rn.raw < 8) {
        *this << Dn{rn.raw};
    } else {
        *this << Anr{rn.raw - 8};
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Ccr ccr)
{
    auto upper = style == DASM_MOTOROLA || style == DASM_MIT;

    *this << (upper ? "CCR" : "ccr");
    return *this;
}

StrWriter&
StrWriter::operator<<(Cn cn)
{
    bool upper = style == DASM_MOIRA || style == DASM_MUSASHI;

    switch (cn.raw) {

        case 0x000: *this << (upper ? "SFC" : "sfc");   break;
        case 0x001: *this << (upper ? "DFC" : "dfc");   break;
        case 0x800: *this << (upper ? "USP" : "usp");   break;
        case 0x801: *this << (upper ? "VBR" : "vbr");   break;
        case 0x002: *this << (upper ? "CACR" : "cacr");  break;
        case 0x802: *this << (upper ? "CAAR" : "caar");  break;
        case 0x803: *this << (upper ? "MSP" : "msp");   break;
        case 0x804: *this << (upper ? "ISP" : "isp");   break;
        case 0x003: *this << (upper ? "TC" : "tc");    break;
        case 0x004: *this << (upper ? "ITT0" : "itt0");  break;
        case 0x005: *this << (upper ? "ITT1" : "itt1");  break;
        case 0x006: *this << (upper ? "DTT0" : "dtt0");  break;
        case 0x007: *this << (upper ? "DTT1" : "dtt1");  break;
        case 0x805: *this << (upper ? "MMUSR" : "mmusr"); break;
        case 0x806: *this << (upper ? "URP" : "urp");   break;
        case 0x807: *this << (upper ? "SRP" : "srp");   break;

        default:

            style == DASM_MUSASHI ? *this << UInt(cn.raw) : *this << "INVALID";
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Cnd cnd)
{
    switch (Cond(cnd.raw)) {

        case COND_BT: *this << "t";   break;
        case COND_BF: *this << "f";   break;
        case COND_HI: *this << "hi";  break;
        case COND_LS: *this << "ls";  break;
        case COND_CC: *this << "cc";  break;
        case COND_CS: *this << "cs";  break;
        case COND_NE: *this << "ne";  break;
        case COND_EQ: *this << "eq";  break;
        case COND_VC: *this << "vc";  break;
        case COND_VS: *this << "vs";  break;
        case COND_PL: *this << "pl";  break;
        case COND_MI: *this << "mi";  break;
        case COND_GE: *this << "ge";  break;
        case COND_LT: *this << "lt";  break;
        case COND_GT: *this << "gt";  break;
        case COND_LE: *this << "le";  break;
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Cpcc cpcc)
{
    switch (cpcc.raw) {

        case 0: *this << "f";       break;
        case 1: *this << "eq";      break;
        case 2: *this << "ogt";     break;
        case 3: *this << "oge";     break;
        case 4: *this << "olt";     break;
        case 5: *this << "ole";     break;
        case 6: *this << "ogl";     break;
        case 7: *this << "or";      break;
        case 8: *this << "un";      break;
        case 9: *this << "ueq";     break;
        case 10: *this << "ugt";    break;
        case 11: *this << "uge";    break;
        case 12: *this << "ult";    break;
        case 13: *this << "ule";    break;
        case 14: *this << "ne";     break;
        case 15: *this << "t";      break;
        case 16: *this << "sf";     break;
        case 17: *this << "seq";    break;
        case 18: *this << "gt";     break;
        case 19: *this << "ge";     break;
        case 20: *this << "lt";     break;
        case 21: *this << "le";     break;
        case 22: *this << "gl";     break;
        case 23: *this << "gle";    break;
        case 24: *this << "ngle";   break;
        case 25: *this << "ngl";    break;
        case 26: *this << "nle";    break;
        case 27: *this << "nlt";    break;
        case 28: *this << "nge";    break;
        case 29: *this << "ngt";    break;
        case 30: *this << "sne";    break;
        case 31: *this << "st";     break;

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

template <Size S> StrWriter&
StrWriter::operator<<(Ims<S> im)
{
    *ptr++ = '#';
    *this << Int(SEXT<S>(im.raw));
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
StrWriter::operator<<(Sr _)
{
    auto upper = style == DASM_MOTOROLA || style == DASM_MIT;

    *this << (upper ? "SR" : "sr");
    return *this;
}

StrWriter&
StrWriter::operator<<(Usp _)
{
    auto upper = style == DASM_MOTOROLA || style == DASM_MIT;

    *this << (upper ? "USP" : "usp");
    return *this;
}

StrWriter&
StrWriter::operator<<(Tabulator align)
{
    // Write at least a single space
    *ptr++ = ' ';
    
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
        if (r[i] == 1) { *this << Rnr{i}; }

        // Format variant 2: Register range
        else { *this << Rnr{i} << "-" << Rnr{i+r[i]-1}; }
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
    if constexpr (I == DBF) {

        if (style == DASM_MOTOROLA || style == DASM_MIT) {
            *this << "dbf";
        } else {
            *this << "dbra";
        }

    } else {

        *this << instrLower[I];
    }

    return *this;
}

template <Size S> StrWriter&
StrWriter::operator<<(Sz<S>)
{
    *this << ((S == Byte) ? ".b" : (S == Word) ? ".w" : ".l");
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
            *this << Ai<M,S>{ea};
            // *this << "(" << An{ea.reg} << ")";
            break;
        }
        case 3:  // (An)+
        {
            *this << Pi<M,S>{ea};
            // *this << "(" << An{ea.reg} << ")+";
            break;
        }
        case 4: // -(An)
        {
            *this << Pd<M,S>{ea};
            // *this << "-(" << An{ea.reg} << ")";
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
            if (style == DASM_MOTOROLA) {
                (ea.ext1 & 0x100) ? fullExtensionVda68k(ea) : briefExtension(ea);
            } else {
                (ea.ext1 & 0x100) ? fullExtension(ea) : briefExtension(ea);
            }
            break;
        }
        case 7: // ABS.W
        {
            *this << UInt(ea.ext1);
            *this << Sz<Word>{};
            break;
        }
        case 8: // ABS.L
        {
            *this << UInt(ea.ext1);
            *this << Sz<Long>{};
            break;
        }
        case 9: // (d,PC)
        {
            if (style == DASM_MUSASHI) {

                *this << "(" << Int{(i16)ea.ext1} << ",PC)";
                auto resolved = U32_ADD(U32_ADD(ea.pc, (i16)ea.ext1), 2);
                StrWriter(moira, comment, style, nf) << "; (" << UInt(resolved) << ")" << Finish{};

            } else {

                auto resolved = U32_ADD(U32_ADD(ea.pc, (i16)ea.ext1), 2);
                *this << UInt(resolved) << "(pc)";
            }
            break;
        }
        case 10: // (d,PC,Xi)  TODO: Merge with case 6 (d,An,Xi) (?!)
        {
            if (style == DASM_MOTOROLA) {
                (ea.ext1 & 0x100) ? fullExtensionVda68k(ea) : briefExtension(ea);
            } else {
                (ea.ext1 & 0x100) ? fullExtension(ea) : briefExtension(ea);
            }
            break;
        }
        case 11: // Imm
        {
            if (style == DASM_MUSASHI) {
                *this << Imu(ea.ext1);
            } else {
                if constexpr (S != 0) {
                    *this << Ims<S>(ea.ext1);
                }
            }
            break;
        }
    }
    return *this;
}

template <Instr I, Mode M, Size S> StrWriter&
StrWriter::operator<<(const Av<I,M,S> &av)
{
    if (style == DASM_MOTOROLA || style == DASM_MIT) { return *this; }

    switch (I) {

        case BKPT:
        case MOVES:
        case MOVEFCCR:
        case RTD:

            *this << "; (1+)";
            break;

        case CMPI:

            *this << (isPrgMode(M) ? "; (1+)" : "");
            break;

        case CALLM:
        case RTM:

            *this << "; (2)";
            break;

        case cpGEN:
        case cpRESTORE:
        case cpSAVE:
        case cpScc:
        case cpTRAPcc:

            *this << "; (2-3)";
            break;

        case BFCHG:
        case BFCLR:
        case BFEXTS:
        case BFEXTU:
        case BFFFO:
        case BFINS:
        case BFSET:
        case BFTST:
        case CAS:
        case CAS2:
        case CHK2:
        case CMP2:
        case DIVL:
        case EXTB:
        case MULL:
        case PACK:
        case TRAPCC:
        case TRAPCS:
        case TRAPEQ:
        case TRAPGE:
        case TRAPGT:
        case TRAPHI:
        case TRAPLE:
        case TRAPLS:
        case TRAPLT:
        case TRAPMI:
        case TRAPNE:
        case TRAPPL:
        case TRAPVC:
        case TRAPVS:
        case TRAPF:
        case TRAPT:
        case UNPK:

            *this << "; (2+)";
            break;

        case CHK:
        case LINK:
        case BRA:
        case BHI:
        case BLS:
        case BCC:
        case BCS:
        case BNE:
        case BEQ:
        case BVC:
        case BVS:
        case BPL:
        case BMI:
        case BGE:
        case BLT:
        case BGT:
        case BLE:
        case BSR:

            *this << (S == Long ? "; (2+)" : "");
            break;

        case TST:
            *this << (M == 1 || M >= 9 ? "; (2+)" : "");
            break;

        case MOVEC:

            switch (av.ext1 & 0x0FFF) {

                case 0x000:
                case 0x001:
                case 0x800:
                case 0x801: *this << "; (1+)"; break;
                case 0x002:
                case 0x803:
                case 0x804: *this << "; (2+)"; break;
                case 0x802: *this << "; (2,3)"; break;
                case 0x003:
                case 0x004:
                case 0x005:
                case 0x006:
                case 0x007:
                case 0x805:
                case 0x806:
                case 0x807: *this << "; (4+)"; break;

                default:    *this << "; (?)";
            }

        default:
            break;
    }
    return *this;
}

StrWriter&
StrWriter::operator<<(Sep)
{
    *ptr++ = ',';
    if (style == DASM_MUSASHI) *ptr++ = ' ';

    return *this;
}

StrWriter&
StrWriter::operator<<(Finish)
{
    for (int i = 0; comment[i] != 0; i++) *ptr++ = comment[i];
    *ptr = 0;
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
    if (!bs) {

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

template <Mode M, Size S> void
StrWriter::fullExtensionVda68k(const Ea <M,S> &ea)
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
    u32  bd     = __________xx____(ea.ext1);
    u32  od     = ______________xx(ea.ext1);
    u32  base  = ea.ext2;
    u32  outer = ea.ext3;

    bool preindex      = (iis > 0 && iis < 4);
    bool postindex     = (iis > 4);
    // bool effectiveZero = (ea.ext1 & 0xe4) == 0xC4 || (ea.ext1 & 0xe2) == 0xC0;

    /*
    if (effectiveZero) {
        *this << "0";
        return;
    }
    */

    *this << "(";

    bool comma = false;
    if (preindex || postindex) {

        *this << "[";
    }

    size == 3 ? (*this << Int{(i32)base}) : (*this << Int{(i16)base});
    comma = true;

    if (comma) *this << ",";
    comma = true;

    if (bs && size) {
        M == 10 ? (*this << "zpc") : *this << "z" << An{ea.reg};
    } else {
        M == 10 ? (*this << "pc") : *this << An{ea.reg};
    }

    if (postindex) {

        *this << "]";
        comma = true;
    }
    if (is && bd) {

        if (comma) *this << ",0";
        comma = true;

    } else {

        if (comma) *this << ",";
        *this << Rn{reg};
        lw ? (*this << Sz<Long>{}) : (*this << Sz<Word>{});
        *this << Scale{scale};
        comma = true;
    }

    if (preindex) {

        *this << "]";
        comma = true;
    }
    if (od)
    {
        if (comma) *this << ",";
        *this << Int(outer);
    }

    *this << ")";
}
