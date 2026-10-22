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
ساكن فراغ U_CALLCONV RBBISymbol_جدول_مدخل_deleter(فراغ *p) {
    icu::RBBISymbol_جدول_مدخل *px = (icu::RBBISymbol_جدول_مدخل *)p;
    حذف px;
}
U_CDECL_انتها



U_NAMESPACE_BEGIN

RBBISymbol_جدول::RBBISymbol_جدول(RBBIRuleScanner *rs, ثابت UnicodeString &قواعد, UError_کد &وضعیت)
    : f_قواعد(قواعد), f_قاعده_Scanner(rs)
{
    f_Hash_جدول       = اشاره_تهی;

    f_Hash_جدول = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, اشاره_تهی, &وضعیت);
    // uhash_open checks status
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    uhash_مجموعه_مقدار_Deleter(f_Hash_جدول, RBBISymbol_جدول_مدخل_deleter);
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
    ثابت RBBISymbol_جدول_مدخل* ثابت el =
        static_cast<ثابت RBBISymbol_جدول_مدخل*>(uhash_get(f_Hash_جدول, &s));
    إذا (el == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    ثابت RBBINode& عبارت_گره = *el->val->f_چپ_فرزند; // Root node of expression for variable
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
    أرجع &عبارت_گره.f_متن;
}

ثابت Unicode_مجموعه* RBBISymbol_جدول::lookup_مجموعه(ثابت UnicodeString& s) ثابت {
    ثابت RBBISymbol_جدول_مدخل* ثابت el = static_cast<ثابت RBBISymbol_جدول_مدخل*>(uhash_get(f_Hash_جدول, &s));
    إذا (el == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    ثابت RBBINode& عبارت_گره = *el->val->f_چپ_فرزند;
    إذا (عبارت_گره.fType == RBBINode::مجموعه_Ref) {
        أرجع عبارت_گره.f_چپ_فرزند->f_درون_داد_مجموعه;
    } وإلا {
        أرجع اشاره_تهی;
    }
}



//  No longer used, see ICU-23297.
ثابت UnicodeFunctor* RBBISymbol_جدول::lookupMatcher(UChar32 /*ch*/) ثابت {
    أرجع اشاره_تهی;
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
    int32_t شروع = pos.get_شاخص();
    int32_t i = شروع;
    UnicodeString نتیجه;
    طالما (i < حد) {
        char16_t c = متن.charAt(i);
        إذا ((i==شروع && !u_isIDStart(c)) || !u_isIDPart(c)) {
            اكسر;
        }
        ++i;
    }
    إذا (i == شروع) { // No valid name chars
        أرجع نتیجه; // Indicate failure with empty string
    }
    pos.مجموعه_شاخص(i);
    متن.استخراج_Between(شروع, i, نتیجه);
    أرجع نتیجه;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbol_جدول::lookup_گره(ثابت UnicodeString &کلید) ثابت{

    RBBINode             *ret_گره = اشاره_تهی;
    RBBISymbol_جدول_مدخل *el;

    el = static_cast<RBBISymbol_جدول_مدخل*>(uhash_get(f_Hash_جدول, &کلید));
    إذا (el != اشاره_تهی) {
        ret_گره = el->val;
    }
    أرجع ret_گره;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
فراغ            RBBISymbol_جدول::جمع_کن_مدخل  (ثابت UnicodeString &کلید, RBBINode *val, UError_کد &err) {
    RBBISymbol_جدول_مدخل *e;
    /* test for buffer overflows */
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    e = static_cast<RBBISymbol_جدول_مدخل*>(uhash_get(f_Hash_جدول, &کلید));
    إذا (e != اشاره_تهی) {
        err = U_BRK_VARIABLE_REDFINITION;
        أرجع;
    }

    e = جديد RBBISymbol_جدول_مدخل;
    إذا (e == اشاره_تهی) {
        err = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    e->کلید = کلید;
    e->val = val;
    uhash_put( f_Hash_جدول, &e->کلید, e, &err);
}


RBBISymbol_جدول_مدخل::RBBISymbol_جدول_مدخل() : UMemory(), کلید(), val(اشاره_تهی) {}

RBBISymbol_جدول_مدخل::~RBBISymbol_جدول_مدخل() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    حذف val->f_چپ_فرزند;
    val->f_چپ_فرزند = اشاره_تهی;

    حذف  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBISymbol_جدول::rbbiSymtablePrint() ثابت {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_اولی;
    ثابت UHashElement  *e   = اشاره_تهی;
    لكل (;;) {
        e = uhash_بعدی_Element(f_Hash_جدول,  &pos);
        إذا (e == اشاره_تهی ) {
            اكسر;
        }
        RBBISymbol_جدول_مدخل  *s   = (RBBISymbol_جدول_مدخل *)e->value.اشاره_گر;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->کلید)(), (فراغ *)s->val, s->val->f_Serial_رقم);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_چپ_فرزند->f_متن)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    لكل (;;) {
        e = uhash_بعدی_Element(f_Hash_جدول,  &pos);
        إذا (e == اشاره_تهی ) {
            اكسر;
        }
        RBBISymbol_جدول_مدخل  *s   = (RBBISymbol_جدول_مدخل *)e->value.اشاره_گر;
        RBBIDebugPrintf("%s\n", CStr(s->کلید)());
        RBBINode::print_درخت(s->val, درست);
        RBBINode::print_درخت(s->val->f_چپ_فرزند, خطأ);
        RBBIDebugPrintf("\n");
    }
}
#پایان_اگر





U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */
