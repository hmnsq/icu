// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//  rbbiscan.h
//
//  Copyright (C) 2002-2016, International Business Machines Corporation and others.
//  All Rights Reserved.
//
//  This file contains declarations for class RBBIRuleScanner
//


#جيڪڏهن_وصف_نه_ٿيل RBBISCAN_H
#وصف_ڪريو RBBISCAN_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/rbbi.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/parseerr.h"
#ضمن "uhash.h"
#ضمن "uvector.h"
#ضمن "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#ضمن "rbbinode.h"
#ضمن "rbbirpt.h"

U_NAMESPACE_BEGIN

class   RBBIRuleBuilder;
class   RBBISymbol_جدول;


//--------------------------------------------------------------------------------
//
//  class RBBIRuleScanner does the lowest level, character-at-a-time
//                        scanning of break iterator rules.  
//
//                        The output of the scanner is parse trees for
//                        the rule expressions and a list of all Unicode Sets
//                        encountered.
//
//--------------------------------------------------------------------------------

class RBBIRuleScanner : عام UMemory {
عام:

    تعداد {
        k_ڍير_سائيز = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    بنية RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    مجازي    ~RBBIRuleScanner();

    فراغ        ايندڙ_Char(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       ڌڪو(ثابت RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    فراغ        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     انگ_قاعدا();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    ساكن UnicodeString strip_قاعدا(ثابت UnicodeString &قاعدا);
خاص:

    UBool       doParseActions(int32_t a);
    فراغ        error(UError_ڪوڊ e);                   // error reporting convenience function.
    فراغ        fix_Op_ڍير(RBBINode::OpPrecedence p);
                                                       //   a character.
    فراغ        ڳوليو_سيٽ_For(ثابت UnicodeString &s, RBBINode *ڳنڍ, Unicode_سيٽ *سيٽ_منزل_Adopt = nullptr);

    UChar32     ايندڙ_Char_LL();
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    فراغ        print_ڳنڍ_ڍير(ثابت حرف *title);
#جيڪڏهن_پورو
    RBBINode    *ڌڪو_New_ڳنڍ(RBBINode::ڳنڍ_Type  t);
    فراغ        جاچ_سيٽ();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       f_جاچ_اشاريو;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       f_ايندڙ_اشاريو;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         f_Quote_گهڻائي;        // Scan is in a 'quoted region'
    int32_t                       f_سٽ_انگ;          // Line number in input file.
    int32_t                       f_Char_انگ;          // Char position within the line.
    UChar32                       f_آخري_Char;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRule_جدول_El               **f_State_جدول;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_ڍير[k_ڍير_سائيز];  // State stack, holds state pushes
    int32_t                       f_ڍير_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_ڳنڍ_ڍير[k_ڍير_سائيز]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_ڳنڍ_ڍير_Ptr;


    UBool                          f_ابتو_قاعدو;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          f_Look_Ahead_قاعدو;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          f_No_Chain_اندريون_قاعدو;   // True if the current rule starts with a '^'.

    RBBISymbol_جدول               *f_نشان_جدول;     // symbol table, holds definitions of
                                                     //   $variable symbols.

    UHashtable                    *f_سيٽ_جدول;        // UnicocodeSet hash table, holds indexes to
                                                     //   the sets created while parsing rules.
                                                     //   The key is the string used for creating
                                                     //   the set.

    Unicode_سيٽ                     f_قاعدو_Sets[10];    // Unicode Sets that are needed during
                                                     //  the scanning of RBBI rules.  The
                                                     //  indices for these are assigned by the
                                                     //  perl script that builds the state tables.
                                                     //  See rbbirpt.h.

    int32_t                        f_قاعدو_انگ;         // Counts each rule as it is scanned.

    int32_t                        f_Option_شروعات;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    Unicode_سيٽ *g_قاعدو_سيٽ_قاعدو_char;
    Unicode_سيٽ *g_قاعدو_سيٽ_white_space;
    Unicode_سيٽ *g_قاعدو_سيٽ_name_char;
    Unicode_سيٽ *g_قاعدو_سيٽ_name_شروعات_char;

    RBBIRuleScanner(ثابت RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &عامل=(ثابت RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
