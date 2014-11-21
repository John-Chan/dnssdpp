
#ifndef MURADIN_DBG_HELPER_DATA_TYPES_H__
#define MURADIN_DBG_HELPER_DATA_TYPES_H__

#include <Windows.h>

// If VC7 and later, then use the  'dbghelp.h'
#if _MSC_VER >= 1300
#include <dbghelp.h>
#else
//////////////////////////////////////////////////////////////////////////
/// inline the important dbghelp.h declarations define 
/// for those who dont have dbghelp.h

//
// symbol type enumeration
//
typedef enum {
	SymNone = 0,		// No symbols are loaded.
	SymCoff,			// COFF symbols.	
	SymCv,				// CodeView symbols.
	SymPdb,				// PDB symbols.
	SymExport,			// Symbols generated from a DLL export table.
	SymDeferred,		// Symbol loading deferred.
	SymSym,				// .sym file
	SymDia,				// DIA symbols.
	SymVirtual,			// The virtual module created by SymLoadModuleEx with SLMFLAG_VIRTUAL.
	NumSymTypes
} SYM_TYPE;


//
// source file line data structure
//

typedef struct _IMAGEHLP_LINEA64 {
	DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_LINE64)
	PVOID    Key;                    // internal
	DWORD    LineNumber;             // line number in file
	PCHAR    FileName;               // full filename
	DWORD64  Address;                // first instruction of line
} IMAGEHLP_LINEA64, *PIMAGEHLP_LINEA64;

typedef struct _IMAGEHLP_LINEW64 {
	DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_LINE64)
	PVOID    Key;                    // internal
	DWORD    LineNumber;             // line number in file
	PWSTR    FileName;               // full filename
	DWORD64  Address;                // first instruction of line
} IMAGEHLP_LINEW64, *PIMAGEHLP_LINEW64;

#ifdef _UNICODE
#define IMAGEHLP_LINEA64	IMAGEHLP_LINE64;
#define IMAGEHLP_LINEA64*	PIMAGEHLP_LINE64;
#else
#define IMAGEHLP_LINEW64	IMAGEHLP_LINE64;
#define IMAGEHLP_LINEW64*	PIMAGEHLP_LINE64;
#endif // UNOCODE

typedef struct _IMAGEHLP_MODULE64 {
    DWORD                       SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64                     BaseOfImage;            // base load address of module
    DWORD                       ImageSize;              // virtual size of the loaded module
    DWORD                       TimeDateStamp;          // date/time stamp from pe header
    DWORD                       CheckSum;               // checksum from the pe header
    DWORD                       NumSyms;                // number of symbols in the symbol table
    SYM_TYPE                    SymType;                // type of symbols loaded
    CHAR                        ModuleName[32];         // module name
    CHAR                        ImageName[256];         // image name
    CHAR                        LoadedImageName[256];   // symbol file name
} IMAGEHLP_MODULE64, *PIMAGEHLP_MODULE64;



typedef struct _IMAGEHLP_SYMBOL {
	DWORD SizeOfStruct;           // set to sizeof(IMAGEHLP_SYMBOL)
	DWORD Address;                // virtual address including dll base address
	DWORD Size;                   // estimated size of symbol, can be zero
	DWORD Flags;                  // info about the symbols, see the SYMF defines
	DWORD                       MaxNameLength;          // maximum size of symbol name in 'Name'
	CHAR                        Name[1];                // symbol name (null terminated string)
} IMAGEHLP_SYMBOL, *PIMAGEHLP_SYMBOL;


typedef struct _IMAGEHLP_SYMBOLW {
	DWORD SizeOfStruct;           // set to sizeof(IMAGEHLP_SYMBOLW)
	DWORD Address;                // virtual address including dll base address
	DWORD Size;                   // estimated size of symbol, can be zero
	DWORD Flags;                  // info about the symbols, see the SYMF defines
	DWORD                       MaxNameLength;          // maximum size of symbol name in 'Name'
	WCHAR                       Name[1];                // symbol name (null terminated string)
} IMAGEHLP_SYMBOLW, *PIMAGEHLP_SYMBOLW;

#ifdef _UNICODE
#define IMAGEHLP_SYMBOLW	IMAGEHLP_SYMBOL64;
#define IMAGEHLP_SYMBOLW*	PIMAGEHLP_SYMBOL64;
#else
#define IMAGEHLP_SYMBOL		IMAGEHLP_SYMBOL64;
#define IMAGEHLP_SYMBOL*	PIMAGEHLP_SYMBOL64;
#endif // UNOCODE

typedef enum {
    AddrMode1616,
    AddrMode1632,
    AddrModeReal,
    AddrModeFlat
} ADDRESS_MODE;
typedef struct _tagADDRESS64 {
    DWORD64       Offset;
    WORD          Segment;
    ADDRESS_MODE  Mode;
} ADDRESS64, *LPADDRESS64;
typedef struct _KDHELP64 {
    DWORD64   Thread;
    DWORD   ThCallbackStack;
    DWORD   ThCallbackBStore;
    DWORD   NextCallback;
    DWORD   FramePointer;
    DWORD64   KiCallUserMode;
    DWORD64   KeUserCallbackDispatcher;
    DWORD64   SystemRangeStart;
    DWORD64  Reserved[8];
} KDHELP64, *PKDHELP64;
typedef struct _tagSTACKFRAME64 {
    ADDRESS64   AddrPC;               // program counter
    ADDRESS64   AddrReturn;           // return address
    ADDRESS64   AddrFrame;            // frame pointer
    ADDRESS64   AddrStack;            // stack pointer
    ADDRESS64   AddrBStore;           // backing store pointer
    PVOID       FuncTableEntry;       // pointer to pdata/fpo or NULL
    DWORD64     Params[4];            // possible arguments to the function
    BOOL        Far;                  // WOW far call
    BOOL        Virtual;              // is this a virtual frame?
    DWORD64     Reserved[3];
    KDHELP64    KdHelp;
} STACKFRAME64, *LPSTACKFRAME64;
typedef
BOOL
(__stdcall *PREAD_PROCESS_MEMORY_ROUTINE64)(
    HANDLE      hProcess,
    DWORD64     qwBaseAddress,
    PVOID       lpBuffer,
    DWORD       nSize,
    LPDWORD     lpNumberOfBytesRead
);
typedef
PVOID
(__stdcall *PFUNCTION_TABLE_ACCESS_ROUTINE64)(
    HANDLE  hProcess,
    DWORD64 AddrBase
);
typedef
DWORD64
(__stdcall *PGET_MODULE_BASE_ROUTINE64)(
    HANDLE  hProcess,
    DWORD64 Address
);
typedef
DWORD64
(__stdcall *PTRANSLATE_ADDRESS_ROUTINE64)(
    HANDLE    hProcess,
    HANDLE    hThread,
    LPADDRESS64 lpaddr
);
#define SYMOPT_CASE_INSENSITIVE         0x00000001
#define SYMOPT_UNDNAME                  0x00000002
#define SYMOPT_DEFERRED_LOADS           0x00000004
#define SYMOPT_NO_CPP                   0x00000008
#define SYMOPT_LOAD_LINES               0x00000010
#define SYMOPT_OMAP_FIND_NEAREST        0x00000020
#define SYMOPT_LOAD_ANYTHING            0x00000040
#define SYMOPT_IGNORE_CVREC             0x00000080
#define SYMOPT_NO_UNQUALIFIED_LOADS     0x00000100
#define SYMOPT_FAIL_CRITICAL_ERRORS     0x00000200
#define SYMOPT_EXACT_SYMBOLS            0x00000400
#define SYMOPT_ALLOW_ABSOLUTE_SYMBOLS   0x00000800
#define SYMOPT_IGNORE_NT_SYMPATH        0x00001000
#define SYMOPT_INCLUDE_32BIT_MODULES    0x00002000
#define SYMOPT_PUBLICS_ONLY             0x00004000
#define SYMOPT_NO_PUBLICS               0x00008000
#define SYMOPT_AUTO_PUBLICS             0x00010000
#define SYMOPT_NO_IMAGE_SEARCH          0x00020000
#define SYMOPT_SECURE                   0x00040000
#define SYMOPT_DEBUG                    0x80000000
#define UNDNAME_COMPLETE                 (0x0000)  // Enable full undecoration
#define UNDNAME_NAME_ONLY                (0x1000)  // Crack only the name for primary declaration;
#endif  // _MSC_VER < 1300

// Some missing defines (for VC5/6):
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif


// secure-CRT_functions are only available starting with VC8
#if _MSC_VER < 1400
#define strcpy_s strcpy
#define strcat_s(dst, len, src) strcat(dst, src)
#define _snprintf_s _snprintf
#define _tcscat_s _tcscat
#endif

/// Char count for IMAGEHLP_SYMBOL.Name
#define		IMAGEHLP_SYMBOL64_NAME_COUNT		1024

#include <muradin/boost_dep/debug_helper/stackworker_win/imghelp_modul64.h>

#endif