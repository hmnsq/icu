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


#که_نه_تعریف_شوی RBBISCAN_H
#تعریف_کړه RBBISCAN_H

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
        k_ډېری_اندازه = 100            // The size of the state stack for
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

    فراغ        بل_Char(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       ټېل(ثابت RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    فراغ        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     عدد_قواعد();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    ساكن UnicodeString strip_قواعد(ثابت UnicodeString &قواعد);
خاص:

    UBool       doParseActions(int32_t a);
    فراغ        error(UError_کوډ e);                   // error reporting convenience function.
    فراغ        fix_Op_ډېری(RBBINode::OpPrecedence p);
                                                       //   a character.
    فراغ        پیدا_کړه_سېټ_For(ثابت UnicodeString &s, RBBINode *غوټه, Unicode_سېټ *سېټ_منزل_Adopt = nullptr);

    UChar32     بل_Char_LL();
#که_تعریف_شوی RBBI_DEBUG
    فراغ        print_غوټه_ډېری(ثابت حرف *title);
#د_که_پای
    RBBINode    *ټېل_New_غوټه(RBBINode::غوټه_Type  t);
    فراغ        لټون_سېټ();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       f_لټون_شاخص;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       f_بل_شاخص;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         f_Quote_ډېرپېښ;        // Scan is in a 'quoted region'
    int32_t                       f_کرښه_عدد;          // Line number in input file.
    int32_t                       f_Char_عدد;          // Char position within the line.
    UChar32                       f_وروستی_Char;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRule_جدول_El               **f_State_جدول;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_ډېری[k_ډېری_اندازه];  // State stack, holds state pushes
    int32_t                       f_ډېری_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_غوټه_ډېری[k_ډېری_اندازه]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_غوټه_ډېری_Ptr;


    UBool                          f_سرچپه_قاعده;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          f_Look_Ahead_قاعده;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          f_No_Chain_دننه_قاعده;   // True if the current rule starts with a '^'.

    RBBISymbol_جدول               *f_نښه_جدول;     // symbol table, holds definitions of
                                                     //   $variable symbols.

    UHashtable                    *f_سېټ_جدول;        // UnicocodeSet hash table, holds indexes to
                                                     //   the sets created while parsing rules.
                                                     //   The key is the string used for creating
                                                     //   the set.

    Unicode_سېټ                     f_قاعده_Sets[10];    // Unicode Sets that are needed during
                                                     //  the scanning of RBBI rules.  The
                                                     //  indices for these are assigned by the
                                                     //  perl script that builds the state tables.
                                                     //  See rbbirpt.h.

    int32_t                        f_قاعده_عدد;         // Counts each rule as it is scanned.

    int32_t                        f_Option_پیل;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    Unicode_سېټ *g_قاعده_سېټ_قاعده_char;
    Unicode_سېټ *g_قاعده_سېټ_white_space;
    Unicode_سېټ *g_قاعده_سېټ_name_char;
    Unicode_سېټ *g_قاعده_سېټ_name_پیل_char;

    RBBIRuleScanner(ثابت RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &چلوونکی=(ثابت RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_پای_ټکی

#د_که_پای
