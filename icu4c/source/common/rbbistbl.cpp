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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/parsepos.h"

#अन्तर्भाव "cstr.h"
#अन्तर्भाव "rbbinode.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV RBBISymbol_सारणी_प्रविष्टिः_deleter(शून्य *p) {
    icu::RBBISymbol_सारणी_प्रविष्टिः *px = (icu::RBBISymbol_सारणी_प्रविष्टिः *)p;
    विलोपय px;
}
U_CDECL_अन्तः



U_NAMESPACE_BEGIN

RBBISymbol_सारणी::RBBISymbol_सारणी(RBBIRuleScanner *rs, ध्रुव UnicodeString &नियमाः, UError_संहिता &स्थितिः)
    : f_नियमाः(नियमाः), f_नियमः_Scanner(rs)
{
    f_Hash_सारणी       = शून्यसूचकः;

    f_Hash_सारणी = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, शून्यसूचकः, &स्थितिः);
    // uhash_open checks status
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    uhash_समुच्चयः_मानम्_Deleter(f_Hash_सारणी, RBBISymbol_सारणी_प्रविष्टिः_deleter);
}



RBBISymbol_सारणी::~RBBISymbol_सारणी()
{
    uhash_close(f_Hash_सारणी);
}


//
//  RBBISymbolTable::lookup       This function from the abstract symbol table interface
//                                looks up a variable name and returns a UnicodeString
//                                containing the substitution text.
//
//                                The variable name does NOT include the leading $.
//
ध्रुव UnicodeString  *RBBISymbol_सारणी::lookup(ध्रुव UnicodeString& s) ध्रुव
{
    ध्रुव RBBISymbol_सारणी_प्रविष्टिः* ध्रुव el =
        static_cast<ध्रुव RBBISymbol_सारणी_प्रविष्टिः*>(uhash_get(f_Hash_सारणी, &s));
    यदि (el == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव RBBINode& व्यञ्जकम्_ग्रन्थिः = *el->val->f_वामः_Child; // Root node of expression for variable
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
    प्रत्यागम &व्यञ्जकम्_ग्रन्थिः.fText;
}

ध्रुव Unicode_समुच्चयः* RBBISymbol_सारणी::lookup_समुच्चयः(ध्रुव UnicodeString& s) ध्रुव {
    ध्रुव RBBISymbol_सारणी_प्रविष्टिः* ध्रुव el = static_cast<ध्रुव RBBISymbol_सारणी_प्रविष्टिः*>(uhash_get(f_Hash_सारणी, &s));
    यदि (el == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव RBBINode& व्यञ्जकम्_ग्रन्थिः = *el->val->f_वामः_Child;
    यदि (व्यञ्जकम्_ग्रन्थिः.fType == RBBINode::समुच्चयः_Ref) {
        प्रत्यागम व्यञ्जकम्_ग्रन्थिः.f_वामः_Child->f_निवेशः_समुच्चयः;
    } अन्यथा {
        प्रत्यागम शून्यसूचकः;
    }
}



//  No longer used, see ICU-23297.
ध्रुव UnicodeFunctor* RBBISymbol_सारणी::lookupMatcher(UChar32 /*ch*/) ध्रुव {
    प्रत्यागम शून्यसूचकः;
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
UnicodeString   RBBISymbol_सारणी::parseReference(ध्रुव UnicodeString& text,
                                                ParsePosition& pos, int32_t सीमा) ध्रुव
{
    int32_t start = pos.get_सूचकांक();
    int32_t i = start;
    UnicodeString परिणाम;
    यावत् (i < सीमा) {
        char16_t c = text.charAt(i);
        यदि ((i==start && !u_isIDStart(c)) || !u_isIDPart(c)) {
            भङ्ग;
        }
        ++i;
    }
    यदि (i == start) { // No valid name chars
        प्रत्यागम परिणाम; // Indicate failure with empty string
    }
    pos.समुच्चयः_सूचकांक(i);
    text.extractBetween(start, i, परिणाम);
    प्रत्यागम परिणाम;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbol_सारणी::lookup_ग्रन्थिः(ध्रुव UnicodeString &कुञ्चिका) ध्रुव{

    RBBINode             *ret_ग्रन्थिः = शून्यसूचकः;
    RBBISymbol_सारणी_प्रविष्टिः *el;

    el = static_cast<RBBISymbol_सारणी_प्रविष्टिः*>(uhash_get(f_Hash_सारणी, &कुञ्चिका));
    यदि (el != शून्यसूचकः) {
        ret_ग्रन्थिः = el->val;
    }
    प्रत्यागम ret_ग्रन्थिः;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
शून्य            RBBISymbol_सारणी::सङ्कलय_प्रविष्टिः  (ध्रुव UnicodeString &कुञ्चिका, RBBINode *val, UError_संहिता &err) {
    RBBISymbol_सारणी_प्रविष्टिः *e;
    /* test for buffer overflows */
    यदि (U_FAILURE(err)) {
        प्रत्यागम;
    }
    e = static_cast<RBBISymbol_सारणी_प्रविष्टिः*>(uhash_get(f_Hash_सारणी, &कुञ्चिका));
    यदि (e != शून्यसूचकः) {
        err = U_BRK_VARIABLE_REDFINITION;
        प्रत्यागम;
    }

    e = नूतन RBBISymbol_सारणी_प्रविष्टिः;
    यदि (e == शून्यसूचकः) {
        err = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    e->कुञ्चिका = कुञ्चिका;
    e->val = val;
    uhash_put( f_Hash_सारणी, &e->कुञ्चिका, e, &err);
}


RBBISymbol_सारणी_प्रविष्टिः::RBBISymbol_सारणी_प्रविष्टिः() : UMemory(), कुञ्चिका(), val(शून्यसूचकः) {}

RBBISymbol_सारणी_प्रविष्टिः::~RBBISymbol_सारणी_प्रविष्टिः() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    विलोपय val->f_वामः_Child;
    val->f_वामः_Child = शून्यसूचकः;

    विलोपय  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBISymbol_सारणी::rbbiSymtablePrint() ध्रुव {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_FIRST;
    ध्रुव UHashElement  *e   = शून्यसूचकः;
    प्रति (;;) {
        e = uhash_अग्र_Element(f_Hash_सारणी,  &pos);
        यदि (e == शून्यसूचकः ) {
            भङ्ग;
        }
        RBBISymbol_सारणी_प्रविष्टिः  *s   = (RBBISymbol_सारणी_प्रविष्टिः *)e->value.pointer;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->कुञ्चिका)(), (शून्य *)s->val, s->val->f_Serial_अङ्कः);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_वामः_Child->fText)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    प्रति (;;) {
        e = uhash_अग्र_Element(f_Hash_सारणी,  &pos);
        यदि (e == शून्यसूचकः ) {
            भङ्ग;
        }
        RBBISymbol_सारणी_प्रविष्टिः  *s   = (RBBISymbol_सारणी_प्रविष्टिः *)e->value.pointer;
        RBBIDebugPrintf("%s\n", CStr(s->कुञ्चिका)());
        RBBINode::print_वृक्षः(s->val, सत्य);
        RBBINode::print_वृक्षः(s->val->f_वामः_Child, असत्य);
        RBBIDebugPrintf("\n");
    }
}
#यदि_अन्तः





U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
