/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */
#ifndef VALUE_H
#define VALUE_H

#include "FloatUtils.h"
#include "Address.h"
#include "LogModule.h"

enum Memory		// Possible values of a vkMemory Value
{
	mConstant	// Memory region representing immutable values
	// Most memory regions are mutable, but they can never be constants inside a Value,
	// so they don't have constant values in the Memory enum.
};


enum Condition	// Possible values of a vkCond Value generated by comparing arg1 with arg2
{
	cLt = 1,	// arg1 < arg2
	cEq = 2,	// arg1 = arg2
	cGt = 4,	// arg1 > arg2
	cUn = 8		// either arg1 or arg2 is a NaN
};

#ifdef DEBUG_LOG
int print(LogModuleObject &f, Condition c);
#endif


enum Condition2	// Two-way conditional
{
	cond0,		// 0000  Always false
	condLt,		// 0001  arg1 < arg2
	condEq,		// 0010  arg1 = arg2
	condLe,		// 0011  arg1 <= arg2
	condGt,		// 0100  arg1 > arg2
	condLgt,	// 0101  arg1 <> arg2
	condGe,		// 0110  arg1 >= arg2
	condOrd,	// 0111  arg1 <=> arg2 (i.e. arg1 and arg2 are ordered)
	condUnord,	// 1000  arg1 ? arg2 (i.e. arg1 and arg2 are unordered)
	condULt,	// 1001  arg1 ?< arg2
	condUEq,	// 1010  arg1 ?= arg2
	condULe,	// 1011  arg1 ?<= arg2
	condUGt,	// 1100  arg1 ?> arg2
	condNe,		// 1101  arg1 != arg2
	condUGe,	// 1110  arg1 ?>= arg2
	cond1		// 1111  Always true
};

// Return a condition c' such that for all arg1, arg2:  (arg1 c arg2) = (arg2 c' arg1)
inline Condition2 reverse(Condition2 c) {return (Condition2)(c&0xA | (c&4)>>2 | (c&1)<<2);}

// Return a condition c' such that for all arg1, arg2:  !(arg1 c arg2) = (arg1 c' arg2)
inline Condition2 invert(Condition2 c) {return (Condition2)(c^cond1);}

// Return the result of applying the Condition2 to the Condition.
inline bool applyCondition(Condition2 comp, Condition c) {return (comp & c) != 0;}


enum Condition3	// Three-way conditional
{				// lt eq gt un
	cond3L,		// -1  0  1 -1
	cond3G,		// -1  0  1  1
	cond3CL,	//  1  0 -1 -1
	cond3CG		//  1  0 -1  1
};

// Return a condition c' such that for all arg1, arg2:  (arg1 c arg2) = (arg2 c' arg1)
inline Condition3 reverse(Condition3 c) {return (Condition3)(c^2);}

// Return a condition c' such that for all arg1, arg2:  -(arg1 c arg2) = (arg1 c' arg2)
inline Condition3 invert(Condition3 c) {return (Condition3)(c^cond3CG);}

// Return the result of applying the Condition3 to the Condition.
inline int applyCondition(Condition3 comp, Condition c) {return c == cEq ? 0 : ((0x91c4>>(comp<<2) & c) != 0)*2 - 1;}


// ----------------------------------------------------------------------------

enum ValueKind	// Concrete?  RegOrMem?      #words (as defined by the Java stack model)
{				//        Java?      Storable?
	vkVoid,		//   yes   yes   no    no      N/A		// No value
	vkInt,		//   yes   yes   yes   yes      1		// 32-bit integer or high or low half of a long
	vkLong,		//   yes   yes   yes   yes      2		// 64-bit integer
	vkFloat,	//   yes   yes   yes   yes      1		// 32-bit float
	vkDouble,	//   yes   yes   yes   yes      2		// 64-bit float
	vkAddr,		//   yes   yes   yes   yes      1		// Pointer
	vkCond,		//   yes   no    yes   no      N/A		// The result of a compare
	vkMemory,	//   no    no    yes   no      N/A		// Value representing all of memory in data flow edges
	vkTuple		//   no    no    no    no      N/A		// Value representing a tuple of other values
};
const uint nValueKinds = vkTuple + 1;

inline ValueKind typeKindToValueKind(TypeKind tk);

inline bool isConcreteKind(ValueKind vk) {return vk <= vkCond;}
inline bool isJavaKind(ValueKind vk) {return vk <= vkAddr;}
inline bool isRegOrMemKind(ValueKind vk) {return vk != vkVoid && vk <= vkMemory;}
inline bool isStorableKind(ValueKind vk) {return vk != vkVoid && vk <= vkAddr;}

inline bool isVoidKind(ValueKind vk) {return vk == vkVoid;}
inline bool isIntegerKind(ValueKind vk) {return vk == vkInt || vk == vkLong;}
inline bool isFloatingPointKind(ValueKind vk) {return vk == vkFloat || vk == vkDouble;}
inline bool isMemoryKind(ValueKind vk) {return vk == vkMemory;}
// isWordKind and isDoublewordKind indicate whether a Java ValueKind takes
// one or two words in the Java stack model; this does not necessarily mean
// that that value's actual machine representation takes one or two machine words.
// For example, the Java stack model states that an address is one word, but on
// a 64-bit implementation an address takes two *machine words*.
inline bool isWordKind(ValueKind vk) {return vk == vkInt || vk == vkFloat || vk == vkAddr;}
inline bool isDoublewordKind(ValueKind vk) {return vk == vkLong || vk == vkDouble;}

#ifdef DEBUG_LOG
char valueKindShortName(ValueKind vk);
int print(LogModuleObject &f, ValueKind vk);
#endif


union Value
{
	Int32 i;									// Integer value
	Int64 l;									// Long value
	Flt32 f;									// Float value
	Flt64 d;									// Double value
	addr a;										// Pointer value
	Condition c;								// Condition value
	Memory m;									// Memory region value

	bool isNonzero(ValueKind kind) const;
	bool eq(ValueKind kind, const Value &v2) const;

	// This should be based on a template instead of a
	// dummy argument, but some compilers don't support that yet.
	Int32 getValueContents(Int32 *) const {return i;}
  #ifdef __MWERKS__
	const Int64 &getValueContents(Int64 *) const {return l;}
  #else
	Int64 getValueContents(Int64 *) const {return l;}
  #endif
	Flt32 getValueContents(Flt32 *) const {return f;}
	Flt64 getValueContents(Flt64 *) const {return d;}
	addr getValueContents(addr *) const {return a;}
	Condition getValueContents(Condition *) const {return c;}
	Memory getValueContents(Memory *) const {return m;}
	#define TypeGetValueContents(T) getValueContents((T *)0)

	void setValueContents(Int32 v) {i = v;}
	void setValueContents(Int64 v) {l = v;}
	void setValueContents(Flt32 v) {f = v;}
	void setValueContents(Flt64 v) {d = v;}
	void setValueContents(addr v) {a = v;}
	void setValueContents(Condition v) {c = v;}
	void setValueContents(Memory v) {m = v;}

  #ifdef DEBUG_LOG
	int print(LogModuleObject &f, ValueKind vk) const;
  #endif
};


// Return the kind of the value.
#if defined __MWERKS__ || defined __GNUC__  || defined WIN32
 // This should be based on a template instead of a dummy argument, but some compilers don't support that yet.
 inline ValueKind valueKind(Int32 *) {return vkInt;}
 inline ValueKind valueKind(Int64 *) {return vkLong;}
 inline ValueKind valueKind(Flt32 *) {return vkFloat;}
 inline ValueKind valueKind(Flt64 *) {return vkDouble;}
 inline ValueKind valueKind(addr *) {return vkAddr;}
 inline ValueKind valueKind(Condition *) {return vkCond;}
 inline ValueKind valueKind(Memory *) {return vkMemory;}
 #define TypeValueKind(T) valueKind((T *)0)
#else
 template<class T> ValueKind valueKind();
 template<> inline ValueKind valueKind<Int32>() {return vkInt;}
 template<> inline ValueKind valueKind<Int64>() {return vkLong;}
 template<> inline ValueKind valueKind<Flt32>() {return vkFloat;}
 template<> inline ValueKind valueKind<Flt64>() {return vkDouble;}
 template<> inline ValueKind valueKind<addr>() {return vkAddr;}
 template<> inline ValueKind valueKind<Condition>() {return vkCond;}
 template<> inline ValueKind valueKind<Memory>() {return vkMemory;}
 #define TypeValueKind(T) valueKind<T>()
#endif


// Return true if v is (positive or negative) zero or cEq.
inline bool isZero(Int32 v) {return v == 0;}
inline bool isZero(Int64 v) {return v == 0;}
inline bool isZero(Flt32 v) {return v == 0.0f;}
inline bool isZero(Flt64 v) {return v == 0.0;}
inline bool isZero(addr v) {return !v;}
inline bool isZero(Condition v) {return v == cEq;}
inline bool isZero(Memory) {return true;}	// Only constant memory regions can be represented as values

// Return true if v is infinite. (Equivalent functions for floating-point values located in FloatUtils.h)
inline bool isInfinite(Int32) {return false;}
inline bool isInfinite(Int64) {return false;}
inline bool isInfinite(addr) {return false;}
inline bool isInfinite(Condition) {return false;}
inline bool isInfinite(Memory) {return false;}

// Return true if v is a NaN.
inline bool isNaN(Int32) {return false;}
inline bool isNaN(Int64) {return false;}
//inline bool isNaN(Flt32) // Defined in FloatUtils.h
//inline bool isNaN(Flt64) // Defined in FloatUtils.h
inline bool isNaN(addr) {return false;}
inline bool isNaN(Condition) {return false;}
inline bool isNaN(Memory) {return false;}


#define VALUE_FITS(mask, v, out, cast) 	\
	PR_BEGIN_MACRO					\
		if ((~mask & v.i) > 0)		\
			return (false);			\
		else						\
		{							\
			out = (cast)(mask & v.i);		\
			return (true);			\
		}							\
	PR_END_MACRO
	
inline bool extractU8(const Value& v, Uint8& out) { VALUE_FITS(0xFF, v, out, Uint8); } 
inline bool extractU16(const Value& v, Uint16& out) { VALUE_FITS(0xFFFF, v, out, Uint16); }
inline bool extractS16(const Value& v, Int16& out) 
{ 
	if ((v.i >= -32768 && v.i < 32767))
	{
		out = (Int16) v.i;
		return (true);
	}
	else
		return (false);
}


inline bool extractU32(const Value& v, Uint32& out) { out = v.i; return (true); }
inline bool isPowerOfTwo(const Uint32 v) { return (!(v & (v-1)) && v); }

inline Uint8 leadingZeros(const Uint32 v) 
{
#ifdef __MWERKS__
	return (__cntlzw(v));
#else
	Uint32	mask = 0x80000000;
	Uint8	leadingZeros;
	
	for (leadingZeros = 0; (v & mask) != v; leadingZeros++, mask>>=1)
		;
		
	return (leadingZeros);
#endif
}

#ifdef _WIN32
#pragma warning( disable : 4035 )
inline uint leastSigBit(Uint32 v)
{
	__asm bsf eax, v
}
#pragma warning( default : 4035 )
#endif //_WIN32
inline bool valueIsOneByteSigned(Uint32 inValue)
{
	uint temp = inValue >> 7;
	if(temp == 0 || temp == 0x01ffffff)
		return true;
	return false;
}


// Return true if v+x is identical to v for all x of the same type as v.
inline bool isAdditiveAnnihilator(Int32) {return false;}
inline bool isAdditiveAnnihilator(Int64) {return false;}
inline bool isAdditiveAnnihilator(Flt32 v) {return isNaN(v);}
inline bool isAdditiveAnnihilator(Flt64 v) {return isNaN(v);}

// Return true if v+x is identical to x for all x of the same type as v.
inline bool isAdditiveIdentity(Int32 v) {return v == 0;}
inline bool isAdditiveIdentity(Int64 v) {return v == 0;}
inline bool isAdditiveIdentity(Flt32 v) {return isNegativeZero(v);} // v=+0.0 doesn't work because 0.0+-0.0 !eq -0.0
inline bool isAdditiveIdentity(Flt64 v) {return isNegativeZero(v);} // v=+0.0 doesn't work because 0.0+-0.0 !eq -0.0

// Return true if v*x is identical to v for all x of the same type as v.
inline bool isMultiplicativeAnnihilator(Int32 v) {return v == 0;}
inline bool isMultiplicativeAnnihilator(Int64 v) {return v == 0;}
inline bool isMultiplicativeAnnihilator(Flt32 v) {return isNaN(v);} // v=+0.0 or -0.0 doesn't work because 0.0*-1.0 !eq 0.0*1.0
inline bool isMultiplicativeAnnihilator(Flt64 v) {return isNaN(v);} // v=+0.0 or -0.0 doesn't work because 0.0*-1.0 !eq 0.0*1.0

// Return true if v*x is identical to x for all x of the same type as v.
inline bool isMultiplicativeIdentity(Int32 v) {return v == 1;}
inline bool isMultiplicativeIdentity(Int64 v) {return v == 1;}
inline bool isMultiplicativeIdentity(Flt32 v) {return v == 1.0f;}
inline bool isMultiplicativeIdentity(Flt64 v) {return v == 1.0;}

// Return true if v*x is identical to -x for all x of the same type as v.
inline bool isMultiplicativeNegator(Int32 v) {return v == -1;}
inline bool isMultiplicativeNegator(Int64 v) {return v == -1;}
inline bool isMultiplicativeNegator(Flt32 v) {return v == -1.0f;}
inline bool isMultiplicativeNegator(Flt64 v) {return v == -1.0;}

// Return the (positive) zero element v of the given type.
// This should be based on a template instead of a dummy argument, but some compilers don't support that yet.
inline Int32 getZero(Int32 *) {return 0;}
inline Int64 getZero(Int64 *) {return 0;}
inline Flt32 getZero(Flt32 *) {return 0.0f;}
inline Flt64 getZero(Flt64 *) {return 0.0;}
inline addr getZero(addr *) {return nullAddr;}
#define TypeGetZero(T) getZero((T *)0)

// Return the element v of the given type such that v+x is identical to x for all x of the same type.
// This should be based on a template instead of a dummy argument, but some compilers don't support that yet.
inline Int32 getAdditiveIdentity(Int32 *) {return 0;}
inline Int64 getAdditiveIdentity(Int64 *) {return 0;}
inline Flt32 getAdditiveIdentity(Flt32 *) {return floatNegativeZero;}
inline Flt64 getAdditiveIdentity(Flt64 *) {return doubleNegativeZero;}
#define TypeGetAdditiveIdentity(T) getAdditiveIdentity((T *)0)

// Return the NaN of the given type.
// This should be based on a template instead of a dummy argument, but some compilers don't support that yet.
inline Int32 getNaN(Int32 *) {trespass("No Int32 NaN"); return 0;}
inline Int64 getNaN(Int64 *) {trespass("No Int64 NaN"); return 0;}
inline Flt32 getNaN(Flt32 *) {return floatNaN;}
inline Flt64 getNaN(Flt64 *) {return doubleNaN;}
#define TypeGetNaN(T) getNaN((T *)0)


// Convert the signed integer to unsigned.
inline Uint8 toUnsigned(Int8 i) {return (Uint8)i;}
inline Uint16 toUnsigned(Int16 i) {return (Uint16)i;}
inline Uint32 toUnsigned(Int32 i) {return (Uint32)i;}
inline Uint64 toUnsigned(Int64 i) {return (Uint64)i;}

// Convert the unsigned integer to signed.
inline Int8 toSigned(Uint8 i) {return (Int8)i;}
inline Int16 toSigned(Uint16 i) {return (Int16)i;}
inline Int32 toSigned(Uint32 i) {return (Int32)i;}
inline Int64 toSigned(Uint64 i) {return (Int64)i;}

// Convert the argument number to the result number using Java's conventions for conversion.
inline void convertNumber(Int32 arg, Int64 &result) {result = arg;}
inline void convertNumber(Int32 arg, Flt32 &result) {result = int32ToFlt32(arg);}
inline void convertNumber(Int32 arg, Flt64 &result) {result = int32ToFlt64(arg);}
inline void convertNumber(Int64 arg, Int32 &result) {result = (Int32)arg;}
inline void convertNumber(Int64 arg, Flt32 &result) {result = int64ToFlt32(arg);}
inline void convertNumber(Int64 arg, Flt64 &result) {result = int64ToFlt64(arg);}
inline void convertNumber(Flt32 arg, Int32 &result) {result = flt32ToInt32(arg);}
inline void convertNumber(Flt32 arg, Int64 &result) {result = flt32ToInt64(arg);}
inline void convertNumber(Flt32 arg, Flt64 &result) {result = arg;}
inline void convertNumber(Flt64 arg, Int32 &result) {result = flt64ToInt32(arg);}
inline void convertNumber(Flt64 arg, Int64 &result) {result = flt64ToInt64(arg);}
inline void convertNumber(Flt64 arg, Flt32 &result) {result = (Flt32)arg;}


// Compare arg1 with arg2.  If unsignedCompare is true, the values are treated as unsigned.
inline Condition compare(Int32 arg1, Int32 arg2, bool unsignedCompare)
	{return (Condition)((unsignedCompare ? (Uint32)arg1 > (Uint32)arg2 : arg1 > arg2)*(cGt-cLt) + (arg1 == arg2)*(cEq-cLt) + cLt);}
inline Condition compare(Int64 arg1, Int64 arg2, bool unsignedCompare)
	{return (Condition)((unsignedCompare ? (Uint64)arg1 > (Uint64)arg2 : arg1 > arg2)*(cGt-cLt) + (arg1 == arg2)*(cEq-cLt) + cLt);}
inline Condition compare(Flt32 arg1, Flt32 arg2, bool)
	{return isNaN(arg1) || isNaN(arg2) ? cUn : (Condition)((arg1 > arg2)*(cGt-cLt) + (arg1 == arg2)*(cEq-cLt) + cLt);}
inline Condition compare(Flt64 arg1, Flt64 arg2, bool)
	{return isNaN(arg1) || isNaN(arg2) ? cUn : (Condition)((arg1 > arg2)*(cGt-cLt) + (arg1 == arg2)*(cEq-cLt) + cLt);}
inline Condition compare(addr arg1, addr arg2, bool)
	{return (Condition)((arg1 > arg2)*(cGt-cLt) + (arg1 == arg2)*(cEq-cLt) + cLt);}


// --- INLINES ----------------------------------------------------------------


extern const ValueKind typeKindValueKinds[nTypeKinds];	// ValueKind for each TypeKind

inline ValueKind typeKindToValueKind(TypeKind tk)
{
	return typeKindValueKinds[tk];
}


//
// Return true if this value is not equal to zero.  This is a numerical
// comparison, so if this is a float or double, then this function will
// return true if this value is either +0.0 or -0.0.  A condition value
// is considered to be zero if and only if it is cEq.
//
inline bool Value::isNonzero(ValueKind kind) const
{
	switch (kind) {
		case vkInt:
			return i != 0;
		case vkLong:
			return l != 0;
		case vkFloat:
			return f != 0.0f;
		case vkDouble:
			return d != 0.0;
		case vkAddr:
			return !a;
		case vkCond:
			return c != cEq;
		default:
			return true;
	}
}


//
// Return true if this and v2 are identical values.  Both are assumed
// to have the given kind.
// Note that this is a bitwise identity comparison, not a numerical
// value comparison -- for example, a floating-point NaN will compare
// equal to itself, while the floating-point operator == would return
// false.  Moreover, comparing +0.0 with -0.0 will return false, while
// using == to compare them would return true.
//
inline bool Value::eq(ValueKind kind, const Value &v2) const
{
	switch (kind) {
		case vkInt:
		case vkFloat:
			return i == v2.i;
		case vkLong:
		case vkDouble:
			return l == v2.l;
		case vkAddr:
			return a == v2.a;
		case vkCond:
			return c == v2.c;
		default:
			return true;
	}
}

#endif
