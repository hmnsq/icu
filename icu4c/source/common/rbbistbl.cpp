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
ساكن فراغ U_CALLCONV RBBISymbol_جدول_داخلا_deleter(فراغ *p) {
    icu::RBBISymbol_جدول_داخلا *px = (icu::RBBISymbol_جدول_داخلا *)p;
    ميٽيو px;
}
U_CDECL_پڇاڙي



U_NAMESPACE_BEGIN

RBBISymbol_جدول::RBBISymbol_جدول(RBBIRuleScanner *rs, ثابت UnicodeString &قاعدا, UError_ڪوڊ &حالت)
    : f_قاعدا(قاعدا), f_قاعدو_Scanner(rs)
{
    f_Hash_جدول       = خالي_اشارو;

    f_Hash_جدول = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, خالي_اشارو, &حالت);
    // uhash_open checks status
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_سيٽ_قدر_Deleter(f_Hash_جدول, RBBISymbol_جدول_داخلا_deleter);
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
    ثابت RBBISymbol_جدول_داخلا* ثابت el =
        static_cast<ثابت RBBISymbol_جدول_داخلا*>(uhash_get(f_Hash_جدول, &s));
    إذا (el == خالي_اشارو) {
        أرجع خالي_اشارو;
    }
    ثابت RBBINode& اظهار_ڳنڍ = *el->val->f_کاٻو_ٻار; // Root node of expression for variable
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
    أرجع &اظهار_ڳنڍ.f_متن;
}

ثابت Unicode_سيٽ* RBBISymbol_جدول::lookup_سيٽ(ثابت UnicodeString& s) ثابت {
    ثابت RBBISymbol_جدول_داخلا* ثابت el = static_cast<ثابت RBBISymbol_جدول_داخلا*>(uhash_get(f_Hash_جدول, &s));
    إذا (el == خالي_اشارو) {
        أرجع خالي_اشارو;
    }
    ثابت RBBINode& اظهار_ڳنڍ = *el->val->f_کاٻو_ٻار;
    إذا (اظهار_ڳنڍ.fType == RBBINode::سيٽ_Ref) {
        أرجع اظهار_ڳنڍ.f_کاٻو_ٻار->f_داخلا_سيٽ;
    } وإلا {
        أرجع خالي_اشارو;
    }
}



//  No longer used, see ICU-23297.
ثابت UnicodeFunctor* RBBISymbol_جدول::lookupMatcher(UChar32 /*ch*/) ثابت {
    أرجع خالي_اشارو;
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
                                                ParsePosition& pos, int32_t حد_بندي) ثابت
{
    int32_t شروعات = pos.get_اشاريو();
    int32_t i = شروعات;
    UnicodeString نتيجو;
    طالما (i < حد_بندي) {
        char16_t c = متن.charAt(i);
        إذا ((i==شروعات && !u_isIDStart(c)) || !u_isIDPart(c)) {
            اكسر;
        }
        ++i;
    }
    إذا (i == شروعات) { // No valid name chars
        أرجع نتيجو; // Indicate failure with empty string
    }
    pos.سيٽ_اشاريو(i);
    متن.ڪڍو_Between(شروعات, i, نتيجو);
    أرجع نتيجو;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbol_جدول::lookup_ڳنڍ(ثابت UnicodeString &ڪنجي) ثابت{

    RBBINode             *ret_ڳنڍ = خالي_اشارو;
    RBBISymbol_جدول_داخلا *el;

    el = static_cast<RBBISymbol_جدول_داخلا*>(uhash_get(f_Hash_جدول, &ڪنجي));
    إذا (el != خالي_اشارو) {
        ret_ڳنڍ = el->val;
    }
    أرجع ret_ڳنڍ;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
فراغ            RBBISymbol_جدول::جوڙيو_داخلا  (ثابت UnicodeString &ڪنجي, RBBINode *val, UError_ڪوڊ &err) {
    RBBISymbol_جدول_داخلا *e;
    /* test for buffer overflows */
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    e = static_cast<RBBISymbol_جدول_داخلا*>(uhash_get(f_Hash_جدول, &ڪنجي));
    إذا (e != خالي_اشارو) {
        err = U_BRK_VARIABLE_REDFINITION;
        أرجع;
    }

    e = جديد RBBISymbol_جدول_داخلا;
    إذا (e == خالي_اشارو) {
        err = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    e->ڪنجي = ڪنجي;
    e->val = val;
    uhash_put( f_Hash_جدول, &e->ڪنجي, e, &err);
}


RBBISymbol_جدول_داخلا::RBBISymbol_جدول_داخلا() : UMemory(), ڪنجي(), val(خالي_اشارو) {}

RBBISymbol_جدول_داخلا::~RBBISymbol_جدول_داخلا() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    ميٽيو val->f_کاٻو_ٻار;
    val->f_کاٻو_ٻار = خالي_اشارو;

    ميٽيو  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBISymbol_جدول::rbbiSymtablePrint() ثابت {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_پهريون;
    ثابت UHashElement  *e   = خالي_اشارو;
    لكل (;;) {
        e = uhash_ايندڙ_Element(f_Hash_جدول,  &pos);
        إذا (e == خالي_اشارو ) {
            اكسر;
        }
        RBBISymbol_جدول_داخلا  *s   = (RBBISymbol_جدول_داخلا *)e->value.اشارو;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->ڪنجي)(), (فراغ *)s->val, s->val->f_Serial_انگ);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_کاٻو_ٻار->f_متن)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    لكل (;;) {
        e = uhash_ايندڙ_Element(f_Hash_جدول,  &pos);
        إذا (e == خالي_اشارو ) {
            اكسر;
        }
        RBBISymbol_جدول_داخلا  *s   = (RBBISymbol_جدول_داخلا *)e->value.اشارو;
        RBBIDebugPrintf("%s\n", CStr(s->ڪنجي)());
        RBBINode::print_وڻ(s->val, سچ);
        RBBINode::print_وڻ(s->val->f_کاٻو_ٻار, خطأ);
        RBBIDebugPrintf("\n");
    }
}
#جيڪڏهن_پورو





U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
