// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#جے_بنا_تعریف RBBINODE_H
#تعریف_کرو RBBINODE_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    Unicode_سیٹ;
class    UVector;

class RBBINode : عام UMemory {
    عام:
        تعداد گرہ_Type {
            سیٹ_Ref,
            uset,
            varRef,
            پتی_Char,
            lookAhead,
            ٹیگ,
            سرا_Mark,
            op_شروع,
            opCat,
            opOr,
            op_تارا,
            opPlus,
            opQuestion,
            opBreak,
            op_الٹ,
            opLParen
        };

        تعداد OpPrecedence {      
            precZero,
            prec_شروع,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        گرہ_Type      fType;
        RBBINode      *f_ماپا;
        RBBINode      *f_بایاں_بچہ;
        RBBINode      *f_دایاں_بچہ;
        Unicode_سیٹ    *f_داخلہ_سیٹ;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString f_متن;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        صحيح           f_پہلا_Pos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        صحيح           f_آخری_Pos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         f_Look_Ahead_سرا;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_قاعدہ_جڑ;            // True if this node is the root of a rule.
        UBool         f_Chain_وچ;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *f_پہلا_Pos_سیٹ;
        UVector       *f_آخری_Pos_سیٹ;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(گرہ_Type t, UError_کوڈ& حالت);
        RBBINode(ثابت RBBINode &other, UError_کوڈ& حالت);
        ~RBBINode();
        ساكن فراغ  NRDelete_گرہ(RBBINode *گرہ);
        
        RBBINode    *clone_درخت(UError_کوڈ &حالت, صحيح ڈونگھائی=0);
        RBBINode    *flattenVariables(UError_کوڈ &حالت, صحيح ڈونگھائی=0);
        فراغ         flattenSets(UError_کوڈ &حالت, صحيح ڈونگھائی=0);
        فراغ         لبھو_Nodes(UVector *dest, RBBINode::گرہ_Type قسم, UError_کوڈ &حالت);

#جے_تعریف_شدہ RBBI_DEBUG
        ساكن فراغ print_گرہ_سرخی();
        ساكن فراغ print_گرہ(ثابت RBBINode *n);
        ساكن فراغ print_درخت(ثابت RBBINode *n, UBool withHeading);
#جے_انت

    خاص:
        RBBINode &عامل = (ثابت RBBINode &other); // No defs.
        bool عامل == (ثابت RBBINode &other);     // Private, so these functions won't accidentally be used.

#جے_تعریف_شدہ RBBI_DEBUG
    عام:
        صحيح           f_Serial_عدد;           //  Debugging aids.
#جے_انت
};

#جے_تعریف_شدہ RBBI_DEBUG
U_CFUNC فراغ 
RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح min_چوڑائی=0);
#جے_انت

U_NAMESPACE_سرا

#جے_انت

