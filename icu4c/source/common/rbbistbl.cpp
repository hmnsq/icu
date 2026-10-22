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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/unistr.h"
#شامل "unicode/uniset.h"
#شامل "unicode/uchar.h"
#شامل "unicode/parsepos.h"

#شامل "cstr.h"
#شامل "rbbinode.h"
#شامل "rbbirb.h"
#شامل "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
ساکن خالی U_CALLCONV RBBISymbol_جدول_اندراج_deleter(خالی *p) {
    icu::RBBISymbol_جدول_اندراج *px = (icu::RBBISymbol_جدول_اندراج *)p;
    حذف px;
}
U_CDECL_انتہا



U_NAMESPACE_BEGIN

RBBISymbol_جدول::RBBISymbol_جدول(RBBIRuleScanner *rs, ثابت UnicodeString &قواعد, UError_رمز &حالت_2)
    : f_قواعد(قواعد), f_قاعدہ_Scanner(rs)
{
    f_Hash_جدول       = خالی_اشاریہ;

    f_Hash_جدول = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, خالی_اشاریہ, &حالت_2);
    // uhash_open checks status
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    uhash_سیٹ_قدر_Deleter(f_Hash_جدول, RBBISymbol_جدول_اندراج_deleter);
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
    ثابت RBBISymbol_جدول_اندراج* ثابت el =
        static_cast<ثابت RBBISymbol_جدول_اندراج*>(uhash_get(f_Hash_جدول, &s));
    اگر (el == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    ثابت RBBINode& اظہاریہ_گرہ = *el->val->f_بایاں_بچہ; // Root node of expression for variable
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
    واپس &اظہاریہ_گرہ.f_متن;
}

ثابت Unicode_سیٹ* RBBISymbol_جدول::lookup_سیٹ(ثابت UnicodeString& s) ثابت {
    ثابت RBBISymbol_جدول_اندراج* ثابت el = static_cast<ثابت RBBISymbol_جدول_اندراج*>(uhash_get(f_Hash_جدول, &s));
    اگر (el == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    ثابت RBBINode& اظہاریہ_گرہ = *el->val->f_بایاں_بچہ;
    اگر (اظہاریہ_گرہ.fType == RBBINode::سیٹ_Ref) {
        واپس اظہاریہ_گرہ.f_بایاں_بچہ->f_داخلہ_سیٹ;
    } ورنہ {
        واپس خالی_اشاریہ;
    }
}



//  No longer used, see ICU-23297.
ثابت UnicodeFunctor* RBBISymbol_جدول::lookupMatcher(UChar32 /*ch*/) ثابت {
    واپس خالی_اشاریہ;
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
                                                ParsePosition& pos, int32_t حد_بندی) ثابت
{
    int32_t آغاز = pos.get_اشاریہ();
    int32_t i = آغاز;
    UnicodeString نتیجہ;
    جب (i < حد_بندی) {
        char16_t c = متن.charAt(i);
        اگر ((i==آغاز && !u_isIDStart(c)) || !u_isIDPart(c)) {
            توڑو;
        }
        ++i;
    }
    اگر (i == آغاز) { // No valid name chars
        واپس نتیجہ; // Indicate failure with empty string
    }
    pos.سیٹ_اشاریہ(i);
    متن.نکالو_Between(آغاز, i, نتیجہ);
    واپس نتیجہ;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbol_جدول::lookup_گرہ(ثابت UnicodeString &کلید) ثابت{

    RBBINode             *ret_گرہ = خالی_اشاریہ;
    RBBISymbol_جدول_اندراج *el;

    el = static_cast<RBBISymbol_جدول_اندراج*>(uhash_get(f_Hash_جدول, &کلید));
    اگر (el != خالی_اشاریہ) {
        ret_گرہ = el->val;
    }
    واپس ret_گرہ;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
خالی            RBBISymbol_جدول::جوڑو_اندراج  (ثابت UnicodeString &کلید, RBBINode *val, UError_رمز &err) {
    RBBISymbol_جدول_اندراج *e;
    /* test for buffer overflows */
    اگر (U_FAILURE(err)) {
        واپس;
    }
    e = static_cast<RBBISymbol_جدول_اندراج*>(uhash_get(f_Hash_جدول, &کلید));
    اگر (e != خالی_اشاریہ) {
        err = U_BRK_VARIABLE_REDFINITION;
        واپس;
    }

    e = نیا RBBISymbol_جدول_اندراج;
    اگر (e == خالی_اشاریہ) {
        err = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    e->کلید = کلید;
    e->val = val;
    uhash_put( f_Hash_جدول, &e->کلید, e, &err);
}


RBBISymbol_جدول_اندراج::RBBISymbol_جدول_اندراج() : UMemory(), کلید(), val(خالی_اشاریہ) {}

RBBISymbol_جدول_اندراج::~RBBISymbol_جدول_اندراج() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    حذف val->f_بایاں_بچہ;
    val->f_بایاں_بچہ = خالی_اشاریہ;

    حذف  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#اگر_تعریف_شدہ RBBI_DEBUG
خالی RBBISymbol_جدول::rbbiSymtablePrint() ثابت {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_پہلا;
    ثابت UHashElement  *e   = خالی_اشاریہ;
    برائے (;;) {
        e = uhash_اگلا_Element(f_Hash_جدول,  &pos);
        اگر (e == خالی_اشاریہ ) {
            توڑو;
        }
        RBBISymbol_جدول_اندراج  *s   = (RBBISymbol_جدول_اندراج *)e->value.اشارہ_گر;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->کلید)(), (خالی *)s->val, s->val->f_Serial_عدد);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_بایاں_بچہ->f_متن)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    برائے (;;) {
        e = uhash_اگلا_Element(f_Hash_جدول,  &pos);
        اگر (e == خالی_اشاریہ ) {
            توڑو;
        }
        RBBISymbol_جدول_اندراج  *s   = (RBBISymbol_جدول_اندراج *)e->value.اشارہ_گر;
        RBBIDebugPrintf("%s\n", CStr(s->کلید)());
        RBBINode::print_درخت(s->val, سچ);
        RBBINode::print_درخت(s->val->f_بایاں_بچہ, جھوٹ);
        RBBIDebugPrintf("\n");
    }
}
#اگر_ختم





U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */
