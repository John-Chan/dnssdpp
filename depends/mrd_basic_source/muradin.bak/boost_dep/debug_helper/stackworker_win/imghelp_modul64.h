#ifndef MURADIN_DBG_HELPER_IMGHELP_MODULE64_H__
#define MURADIN_DBG_HELPER_IMGHELP_MODULE64_H__

#include <Windows.h>

/************************************************************************
//  MSDN says:
//  The size of the IMAGEHLP_MODULE64 structure used by this function has changed over the years.
//  --------------------------------------------------------------------
//
************************************************************************/
#pragma pack(push,8)  // be sure it has the right packing! THIS IS MISSING IN DBGHELP.H!!!!

typedef struct IMAGEHLP_MODULE64_V1_ {
    DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;            // base load address of module
    DWORD    ImageSize;              // virtual size of the loaded module
    DWORD    TimeDateStamp;          // date/time stamp from pe header
    DWORD    CheckSum;               // checksum from the pe header
    DWORD    NumSyms;                // number of symbols in the symbol table
    SYM_TYPE SymType;                // type of symbols loaded
    CHAR     ModuleName[32];         // module name
    CHAR     ImageName[256];         // image name
    CHAR     LoadedImageName[256];   // symbol file name
} IMAGEHLP_MODULE64_V1;

typedef struct IMAGEHLP_MODULE64_V2_ {
    DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;            // base load address of module
    DWORD    ImageSize;              // virtual size of the loaded module
    DWORD    TimeDateStamp;          // date/time stamp from pe header
    DWORD    CheckSum;               // checksum from the pe header
    DWORD    NumSyms;                // number of symbols in the symbol table
    SYM_TYPE SymType;                // type of symbols loaded
    CHAR     ModuleName[32];         // module name
    CHAR     ImageName[256];         // image name
    CHAR     LoadedImageName[256];   // symbol file name
    // new elements: 07-Jun-2002
    CHAR     LoadedPdbName[256];     // pdb file name
    DWORD    CVSig;                  // Signature of the CV record inthe debug directories
    CHAR     CVData[MAX_PATH * 3];   // Contents of the CV record
    DWORD    PdbSig;                 // Signature of PDB
    GUID     PdbSig70;               // Signature of PDB (VC 7 and up)
    DWORD    PdbAge;                 // DBI age of pdb
    BOOL     PdbUnmatched;           // loaded an unmatched pdb
    BOOL     DbgUnmatched;           // loaded an unmatched dbg
    BOOL     LineNumbers;            // we have line number information
    BOOL     GlobalSymbols;          // we have internal symbol information
    BOOL     TypeInfo;               // we have type information
} IMAGEHLP_MODULE64_V2;

typedef struct IMAGEHLP_MODULE64_V3_ {
    DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;            // base load address of module
    DWORD    ImageSize;              // virtual size of the loaded module
    DWORD    TimeDateStamp;          // date/time stamp from pe header
    DWORD    CheckSum;               // checksum from the pe header
    DWORD    NumSyms;                // number of symbols in the symboltable
    SYM_TYPE SymType;                // type of symbols loaded
    CHAR     ModuleName[32];         // module name
    CHAR     ImageName[256];         // image name
    CHAR     LoadedImageName[256];   // symbol file name
    // new elements: 07-Jun-2002
    CHAR     LoadedPdbName[256];     // pdb file name
    DWORD    CVSig;                  // Signature of the CV record in the debug directories
    CHAR     CVData[MAX_PATH * 3];   // Contents of the CV record
    DWORD    PdbSig;                 // Signature of PDB
    GUID     PdbSig70;               // Signature of PDB (VC 7 and up)
    DWORD    PdbAge;                 // DBI age of pdb
    BOOL     PdbUnmatched;           // loaded an unmatched pdb
    BOOL     DbgUnmatched;           // loaded an unmatched dbg
    BOOL     LineNumbers;            // we have line number information
    BOOL     GlobalSymbols;          // we have internal symbol information
    BOOL     TypeInfo;               // we have type information
    // new elements: 17-Dec-2003
    BOOL     SourceIndexed;          // pdb supports source server
    BOOL     Publics;                // contains public symbols
} IMAGEHLP_MODULE64_V3;

typedef struct IMAGEHLP_MODULE64_V4_ {
    DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
    DWORD64  BaseOfImage;            // base load address of module
    DWORD    ImageSize;              // virtual size of the loaded module
    DWORD    TimeDateStamp;          // date/time stamp from pe header
    DWORD    CheckSum;               // checksum from the pe header
    DWORD    NumSyms;                // number of symbols in the symbol table
    SYM_TYPE SymType;                // type of symbols loaded
    CHAR     ModuleName[32];         // module name
    CHAR     ImageName[256];         // image name
    // new elements: 07-Jun-2002
    CHAR     LoadedImageName[256];   // symbol file name
    CHAR     LoadedPdbName[256];     // pdb file name
    DWORD    CVSig;                  // Signature of the CV record in the debug directories
    CHAR     CVData[MAX_PATH * 3];   // Contents of the CV record
    DWORD    PdbSig;                 // Signature of PDB
    GUID     PdbSig70;               // Signature of PDB (VC 7 and up)
    DWORD    PdbAge;                 // DBI age of pdb
    BOOL     PdbUnmatched;           // loaded an unmatched pdb
    BOOL     DbgUnmatched;           // loaded an unmatched dbg
    BOOL     LineNumbers;            // we have line number information
    BOOL     GlobalSymbols;          // we have internal symbol information
    BOOL     TypeInfo;               // we have type information
    // new elements: 17-Dec-2003
    BOOL     SourceIndexed;          // pdb supports source server
    BOOL     Publics;                // contains public symbols
    // new element: 15-Jul-2009
    DWORD    MachineType;            // IMAGE_FILE_MACHINE_XXX from ntimage.h and winnt.h
    DWORD    Reserved;               // Padding - don't remove.
} IMAGEHLP_MODULE64_V4;


typedef struct IMAGEHLP_MODULEW64_V1_ {
	DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
	DWORD64  BaseOfImage;            // base load address of module
	DWORD    ImageSize;              // virtual size of the loaded module
	DWORD    TimeDateStamp;          // date/time stamp from pe header
	DWORD    CheckSum;               // checksum from the pe header
	DWORD    NumSyms;                // number of symbols in the symbol table
	SYM_TYPE SymType;                // type of symbols loaded
	WCHAR    ModuleName[32];         // module name
	WCHAR    ImageName[256];         // image name
	WCHAR    LoadedImageName[256];   // symbol file name
} IMAGEHLP_MODULEW64_V1;

typedef struct IMAGEHLP_MODULEW64_V2_ {
	DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
	DWORD64  BaseOfImage;            // base load address of module
	DWORD    ImageSize;              // virtual size of the loaded module
	DWORD    TimeDateStamp;          // date/time stamp from pe header
	DWORD    CheckSum;               // checksum from the pe header
	DWORD    NumSyms;                // number of symbols in the symbol table
	SYM_TYPE SymType;                // type of symbols loaded
	WCHAR    ModuleName[32];         // module name
	WCHAR    ImageName[256];         // image name
	WCHAR    LoadedImageName[256];   // symbol file name
	// new elements: 07-Jun-2002
	WCHAR    LoadedPdbName[256];     // pdb file name
	DWORD    CVSig;                  // Signature of the CV record inthe debug directories
	WCHAR    CVData[MAX_PATH * 3];   // Contents of the CV record
	DWORD    PdbSig;                 // Signature of PDB
	GUID     PdbSig70;               // Signature of PDB (VC 7 and up)
	DWORD    PdbAge;                 // DBI age of pdb
	BOOL     PdbUnmatched;           // loaded an unmatched pdb
	BOOL     DbgUnmatched;           // loaded an unmatched dbg
	BOOL     LineNumbers;            // we have line number information
	BOOL     GlobalSymbols;          // we have internal symbol information
	BOOL     TypeInfo;               // we have type information
} IMAGEHLP_MODULEW64_V2;

typedef struct IMAGEHLP_MODULEW64_V3_ {
	DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
	DWORD64  BaseOfImage;            // base load address of module
	DWORD    ImageSize;              // virtual size of the loaded module
	DWORD    TimeDateStamp;          // date/time stamp from pe header
	DWORD    CheckSum;               // checksum from the pe header
	DWORD    NumSyms;                // number of symbols in the symboltable
	SYM_TYPE SymType;                // type of symbols loaded
	WCHAR    ModuleName[32];         // module name
	WCHAR    ImageName[256];         // image name
	WCHAR    LoadedImageName[256];   // symbol file name
	// new elements: 07-Jun-2002
	WCHAR    LoadedPdbName[256];     // pdb file name
	DWORD    CVSig;                  // Signature of the CV record in the debug directories
	WCHAR    CVData[MAX_PATH * 3];   // Contents of the CV record
	DWORD    PdbSig;                 // Signature of PDB
	GUID     PdbSig70;               // Signature of PDB (VC 7 and up)
	DWORD    PdbAge;                 // DBI age of pdb
	BOOL     PdbUnmatched;           // loaded an unmatched pdb
	BOOL     DbgUnmatched;           // loaded an unmatched dbg
	BOOL     LineNumbers;            // we have line number information
	BOOL     GlobalSymbols;          // we have internal symbol information
	BOOL     TypeInfo;               // we have type information
	// new elements: 17-Dec-2003
	BOOL     SourceIndexed;          // pdb supports source server
	BOOL     Publics;                // contains public symbols
} IMAGEHLP_MODULEW64_V3;

typedef struct IMAGEHLP_MODULEW64_V4_ {
	DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
	DWORD64  BaseOfImage;            // base load address of module
	DWORD    ImageSize;              // virtual size of the loaded module
	DWORD    TimeDateStamp;          // date/time stamp from pe header
	DWORD    CheckSum;               // checksum from the pe header
	DWORD    NumSyms;                // number of symbols in the symbol table
	SYM_TYPE SymType;                // type of symbols loaded
	WCHAR    ModuleName[32];         // module name
	WCHAR    ImageName[256];         // image name
	// new elements: 07-Jun-2002
	WCHAR    LoadedImageName[256];   // symbol file name
	WCHAR    LoadedPdbName[256];     // pdb file name
	DWORD    CVSig;                  // Signature of the CV record in the debug directories
	WCHAR    CVData[MAX_PATH * 3];   // Contents of the CV record
	DWORD    PdbSig;                 // Signature of PDB
	GUID     PdbSig70;               // Signature of PDB (VC 7 and up)
	DWORD    PdbAge;                 // DBI age of pdb
	BOOL     PdbUnmatched;           // loaded an unmatched pdb
	BOOL     DbgUnmatched;           // loaded an unmatched dbg
	BOOL     LineNumbers;            // we have line number information
	BOOL     GlobalSymbols;          // we have internal symbol information
	BOOL     TypeInfo;               // we have type information
	// new elements: 17-Dec-2003
	BOOL     SourceIndexed;          // pdb supports source server
	BOOL     Publics;                // contains public symbols
	// new element: 15-Jul-2009
	DWORD    MachineType;            // IMAGE_FILE_MACHINE_XXX from ntimage.h and winnt.h
	DWORD    Reserved;               // Padding - don't remove.
} IMAGEHLP_MODULEW64_V4;
#pragma pack(pop)

enum	IMAGEHLP_MODULE64_VERSION{
	imagehlp_module64_v1,	// origin
	imagehlp_module64_v2,	// 07-Jun-2002
	imagehlp_module64_v3,	// 17-Dec-2003
	imagehlp_module64_v4	// 15-Jul-2009
};

typedef struct V_IMAGEHLP_MODULE64_ {
	union	{
#ifdef _UNICODE
		IMAGEHLP_MODULEW64_V1	v1;
		IMAGEHLP_MODULEW64_V2	v2;
		IMAGEHLP_MODULEW64_V3	v3;
		IMAGEHLP_MODULEW64_V4	v4;
#else
		IMAGEHLP_MODULE64_V1	v1;
		IMAGEHLP_MODULE64_V2	v2;
		IMAGEHLP_MODULE64_V3	v3;
		IMAGEHLP_MODULE64_V4	v4;
#endif // UNOCODE
	}data;
	IMAGEHLP_MODULE64_VERSION	ver;
}V_IMAGEHLP_MODULE64;

#endif//MURADIN_DBG_HELPER_IMGHELP_MODULE64_H__