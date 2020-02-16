#pragma once
#include "Seed.Core.Exceptions.ABI.Base.h"

struct FuncInfoHeader
{
    union
    {
#pragma warning(push)
#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
        struct
        {
            Seed::UInt8 isCatch        : 1;  // 1 if this represents a catch funclet, 0 otherwise
            Seed::UInt8 isSeparated    : 1;  // 1 if this function has separated code segments, 0 otherwise
            Seed::UInt8 BBT            : 1;  // Flags set by Basic Block Transformations
            Seed::UInt8 UnwindMap      : 1;  // Existence of Unwind Map RVA
            Seed::UInt8 TryBlockMap    : 1;  // Existence of Try Block Map RVA
            Seed::UInt8 EHs            : 1;  // EHs flag set
            Seed::UInt8 NoExcept       : 1;  // NoExcept flag set
            Seed::UInt8 reserved       : 1;
        };
#pragma warning(pop)
        Seed::UInt8 value;
    };

    FuncInfoHeader()
    {
        value = 0;
    }
};

static_assert(sizeof(FuncInfoHeader) == sizeof(Seed::Byte), "Size of FuncInfoHeader not 1 Byte");

struct FuncInfo4
{
    FuncInfoHeader         header;
    Seed::UInt32            bbtFlags;            // flags that may be set by BBT processing

    Seed::Int32             dispUnwindMap;       // Image relative offset of the unwind map
    Seed::Int32             dispTryBlockMap;     // Image relative offset of the handler map
    Seed::Int32             dispIPtoStateMap;    // Image relative offset of the IP to state map
    Seed::UInt32            dispFrame;           // displacement of address of function frame wrt establisher frame, only used for catch funclets

    FuncInfo4()
    {
        header.value = 0;
        bbtFlags = 0;
        dispUnwindMap = 0;
        dispTryBlockMap = 0;
        dispIPtoStateMap = 0;
        dispFrame = 0;
    }

};

typedef const struct _s_ESTypeList
{
	int 			nCount;			// how many types are there
	int				dispTypeArray;	// offset of list of types in exception specification
}ESTypeList;

//
// PMD - Pointer to Member Data: generalized pointer-to-member descriptor
//
typedef struct PMD
{
	int	mdisp;	// Offset of intended data within base
	int	pdisp;	// Displacement to virtual base pointer
	int	vdisp;	// Index within vbTable to offset of base
	} PMD;


//
// PMFN - Pointer to Member Function
//
typedef	int	PMFN;					// Image relative offset of Member Function

//
// CatchableType - description of a type that can be caught.
//
// Note:  although isSimpleType can be part of ThrowInfo, it is more
//		  convenient for the run-time to have it here.
//
typedef const struct _s_CatchableType {
	unsigned int		properties;			// Catchable Type properties (Bit field)

	int					pType;				// Image relative offset of TypeDescriptor

	PMD 				thisDisplacement;	// Pointer to instance of catch type within thrown object.
	int					sizeOrOffset;		// Size of simple-type object or offset into
											//  buffer of 'this' pointer for catch object
	PMFN				copyFunction;		// Copy constructor or CC-closure
} CatchableType;

//
// CatchableTypeArray - array of pointers to catchable types, with length
//
#pragma warning(push)
#pragma warning(disable:4200)	// nonstandard extension used: array of runtime bound
typedef const struct _s_CatchableTypeArray {
	int	nCatchableTypes;
	int				arrayOfCatchableTypes[];	// Image relative offset of Catchable Types
	} CatchableTypeArray;
#pragma warning(pop)

//
// HandlerType - description of a single 'catch'
//
typedef const struct _s_HandlerType {
	unsigned int	adjectives;			// Handler Type adjectives (bitfield)

	int				dispType;			// Image relative offset of the corresponding type descriptor
	int				dispCatchObj;		// Displacement of catch object from base
	int				dispOfHandler;		// Image relative offset of 'catch' code

	int				dispFrame;			// displacement of address of function frame wrt establisher frame
} HandlerType;

//
// TypeDescriptor - per-type record which uniquely identifies the type.
//
// Each type has a decorated name which uniquely identifies it, and a hash
// value which is computed by the compiler.  The hash function used is not
// important; the only thing which is essential is that it be the same for
// all time.
//
// The special type '...' (ellipsis) is represented by a null name.
//
#pragma warning(push)
#pragma warning(disable:4200)	// nonstandard extension used: array of runtime bound

#if defined(_M_X64) || defined(_M_ARM64) || defined(BUILDING_C1XX_FORCEINCLUDE)
#pragma pack(push, TypeDescriptor, 8)
#endif

typedef struct TypeDescriptor
{
	const void * pVFTable;	// Field overloaded by RTTI
	void *	spare;			// reserved, possible for RTTI
	char			name[];			// The decorated name of the type; 0 terminated.
	} TypeDescriptor;

#if defined(_M_X64) || defined(_M_ARM64) || defined(BUILDING_C1XX_FORCEINCLUDE)
#pragma pack(pop, TypeDescriptor)
#endif
#pragma warning(pop)

// Link together all existing catch objects to determine when they should
// be destroyed
typedef struct FrameInfo
{
    PVOID              pExceptionObject;
    struct FrameInfo*  pNext;
} FRAMEINFO;
