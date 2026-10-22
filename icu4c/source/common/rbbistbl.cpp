// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//  file:  rbbistbl.cpp    Implementation of the ICU RBBISymbolTable class
//
/*
***************************************************************************
*   Copyright (C) 2002-2014 International Business Machines Corporation
*   and others. All rights reserved.
***************************************************************************
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/unistr.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/parsepos.h"

#ضمن "cstr.h"
#ضمن "rbbinode.h"
#ضمن "rbbirb.h"
#ضمن "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV RBBISymbol_جدول_ثبت_deleter(فراغ *p) {
    icu::RBBISymbol_جدول_ثبت *px = (icu::RBBISymbol_جدول_ثبت *)p;
    ړنګول px;
}
U_CDECL_پای_ټکی



U_NAMESPACE_BEGIN

RBBISymbol_جدول::RBBISymbol_جدول(RBBIRuleScanner *rs, ثابت UnicodeString &قواعد, UError_کوډ &حالت)
    : f_قواعد(قواعد), f_قاعده_Scanner(rs)
{
    f_Hash_جدول       = تش_اشاره;

    f_Hash_جدول = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, تش_اشاره, &حالت);
    // uhash_open checks status
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_سېټ_ارزښت_Deleter(f_Hash_جدول, RBBISymbol_جدول_ثبت_deleter);
}



RBBISymbol_جدول::~RBBISymbol_جدول()
{
    uhash_close(f_Hash_جدول);
}


//
//  RBBISymbolTable::lookup       This function from the abstract symbol table interface
//                                looks up a variable name and returns a UnicodeString
//                                containing the substitution text.
//
//                                The variable name does NOT include the leading $.
//
ثابت UnicodeString  *RBBISymbol_جدول::lookup(ثابت UnicodeString& s) ثابت
{
    ثابت RBBISymbol_جدول_ثبت* ثابت el =
        static_cast<ثابت RBBISymbol_جدول_ثبت*>(uhash_get(f_Hash_جدول, &s));
    إذا (el == تش_اشاره) {
        أرجع تش_اشاره;
    }
    ثابت RBBINode& بیان_غوټه = *el->val->f_کیڼ_ماشوم; // Root node of expression for variable
    // Return the original source string for the expression.
    // Note that for set-valued variables used in UnicodeSet expressions, this would be rejected by
    // the UnicodeSet parser if the source itself contains variable references.  For instance, with
    //     $CaseIgnorable   = [[:Mn:][:Me:][:Cf:][:Lm:][:Sk:] \u0027 \u00AD \u2019];
    //     $Cased = [[:Upper_Case:][:Lower_Case:][:Lt:] - $CaseIgnorable];
    // If lookupSet were not overridden, when parsing the right-hand side of
    //     $NotCased        = [[^ $Cased] - $CaseIgnorable];
    // there would be a call to lookup("Cased") which would return
    //     "[[:Upper_Case:][:Lower_Case:][:Lt:]-$CaseIgnorable]". This contains a variable, which is
    // disallowed by the UnicodeSet parser inside a variable expansion.
    // However, set-valued variables are pre-parsed, and returned by lookupSet instead, so this call
    // to lookup() never happens; instead, lookupSet("CaseIgnorable") is called when computing
    // $Cased and returns the non-null value of $CaseIgnorable, and then when computing $NotCased,
    // lookupSet("Cased") returns the value computed for $Cased.
    أرجع &بیان_غوټه.f_متن;
}

ثابت Unicode_سېټ* RBBISymbol_جدول::lookup_سېټ(ثابت UnicodeString& s) ثابت {
    ثابت RBBISymbol_جدول_ثبت* ثابت el = static_cast<ثابت RBBISymbol_جدول_ثبت*>(uhash_get(f_Hash_جدول, &s));
    إذا (el == تش_اشاره) {
        أرجع تش_اشاره;
    }
    ثابت RBBINode& بیان_غوټه = *el->val->f_کیڼ_ماشوم;
    إذا (بیان_غوټه.fType == RBBINode::سېټ_Ref) {
        أرجع بیان_غوټه.f_کیڼ_ماشوم->f_ننوت_سېټ;
    } وإلا {
        أرجع تش_اشاره;
    }
}



//  No longer used, see ICU-23297.
ثابت UnicodeFunctor* RBBISymbol_جدول::lookupMatcher(UChar32 /*ch*/) ثابت {
    أرجع تش_اشاره;
}

//
// RBBISymbolTable::parseReference   This function from the abstract symbol table interface
//                                   looks for a $variable name in the source text.
//                                   It does not look it up, only scans for it.
//                                   It is used by the UnicodeSet parser.
//
//                                   This implementation is lifted pretty much verbatim
//                                   from the rules based transliterator implementation.
//                                   I didn't see an obvious way of sharing it.
//
UnicodeString   RBBISymbol_جدول::parseReference(ثابت UnicodeString& متن,
                                                ParsePosition& pos, int32_t حد) ثابت
{
    int32_t پیل = pos.get_شاخص();
    int32_t i = پیل;
    UnicodeString پایله;
    طالما (i < حد) {
        char16_t c = متن.charAt(i);
        إذا ((i==پیل && !u_isIDStart(c)) || !u_isIDPart(c)) {
            اكسر;
        }
        ++i;
    }
    إذا (i == پیل) { // No valid name chars
        أرجع پایله; // Indicate failure with empty string
    }
    pos.سېټ_شاخص(i);
    متن.ایستل_Between(پیل, i, پایله);
    أرجع پایله;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbol_جدول::lookup_غوټه(ثابت UnicodeString &کلی) ثابت{

    RBBINode             *ret_غوټه = تش_اشاره;
    RBBISymbol_جدول_ثبت *el;

    el = static_cast<RBBISymbol_جدول_ثبت*>(uhash_get(f_Hash_جدول, &کلی));
    إذا (el != تش_اشاره) {
        ret_غوټه = el->val;
    }
    أرجع ret_غوټه;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
فراغ            RBBISymbol_جدول::جمع_کړه_ثبت  (ثابت UnicodeString &کلی, RBBINode *val, UError_کوډ &err) {
    RBBISymbol_جدول_ثبت *e;
    /* test for buffer overflows */
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    e = static_cast<RBBISymbol_جدول_ثبت*>(uhash_get(f_Hash_جدول, &کلی));
    إذا (e != تش_اشاره) {
        err = U_BRK_VARIABLE_REDFINITION;
        أرجع;
    }

    e = جديد RBBISymbol_جدول_ثبت;
    إذا (e == تش_اشاره) {
        err = U_حافظه_ویش_ERROR;
        أرجع;
    }
    e->کلی = کلی;
    e->val = val;
    uhash_put( f_Hash_جدول, &e->کلی, e, &err);
}


RBBISymbol_جدول_ثبت::RBBISymbol_جدول_ثبت() : UMemory(), کلی(), val(تش_اشاره) {}

RBBISymbol_جدول_ثبت::~RBBISymbol_جدول_ثبت() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    ړنګول val->f_کیڼ_ماشوم;
    val->f_کیڼ_ماشوم = تش_اشاره;

    ړنګول  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBISymbol_جدول::rbbiSymtablePrint() ثابت {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_لومړی;
    ثابت UHashElement  *e   = تش_اشاره;
    لكل (;;) {
        e = uhash_بل_Element(f_Hash_جدول,  &pos);
        إذا (e == تش_اشاره ) {
            اكسر;
        }
        RBBISymbol_جدول_ثبت  *s   = (RBBISymbol_جدول_ثبت *)e->value.اشاره_ګر;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->کلی)(), (فراغ *)s->val, s->val->f_Serial_عدد);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_کیڼ_ماشوم->f_متن)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    لكل (;;) {
        e = uhash_بل_Element(f_Hash_جدول,  &pos);
        إذا (e == تش_اشاره ) {
            اكسر;
        }
        RBBISymbol_جدول_ثبت  *s   = (RBBISymbol_جدول_ثبت *)e->value.اشاره_ګر;
        RBBIDebugPrintf("%s\n", CStr(s->کلی)());
        RBBINode::print_ونه(s->val, ریښتیا);
        RBBINode::print_ونه(s->val->f_کیڼ_ماشوم, خطأ);
        RBBIDebugPrintf("\n");
    }
}
#د_که_پای





U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */
