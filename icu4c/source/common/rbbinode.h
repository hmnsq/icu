// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#اگر_غیر_تعریف_شدہ RBBINODE_H
#تعریف_کرو RBBINODE_H

#شامل "unicode/utypes.h"
#شامل "unicode/unistr.h"
#شامل "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    Unicode_سیٹ;
class    UVector;

class RBBINode : عوامی UMemory {
    عوامی:
        شمار گرہ_Type {
            سیٹ_Ref,
            uset,
            varRef,
            برگ_Char,
            lookAhead,
            لیبل,
            انتہا_Mark,
            op_آغاز,
            opCat,
            opOr,
            op_ستارہ,
            opPlus,
            opQuestion,
            opBreak,
            op_الٹ,
            opLParen
        };

        شمار OpPrecedence {      
            precZero,
            prec_آغاز,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        گرہ_Type      fType;
        RBBINode      *f_والد;
        RBBINode      *f_بایاں_بچہ;
        RBBINode      *f_دایاں_بچہ;
        Unicode_سیٹ    *f_داخلہ_سیٹ;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString f_متن;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        صحیح           f_پہلا_Pos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        صحیح           f_آخری_Pos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         f_Look_Ahead_انتہا;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_قاعدہ_جڑ;            // True if this node is the root of a rule.
        UBool         f_Chain_اندر;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *f_پہلا_Pos_سیٹ;
        UVector       *f_آخری_Pos_سیٹ;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(گرہ_Type t, UError_رمز& حالت_2);
        RBBINode(ثابت RBBINode &other, UError_رمز& حالت_2);
        ~RBBINode();
        ساکن خالی  NRDelete_گرہ(RBBINode *گرہ);
        
        RBBINode    *clone_درخت(UError_رمز &حالت_2, صحیح گہرائی=0);
        RBBINode    *flattenVariables(UError_رمز &حالت_2, صحیح گہرائی=0);
        خالی         flattenSets(UError_رمز &حالت_2, صحیح گہرائی=0);
        خالی         ڈھونڈو_Nodes(UVector *dest, RBBINode::گرہ_Type قسم, UError_رمز &حالت_2);

#اگر_تعریف_شدہ RBBI_DEBUG
        ساکن خالی print_گرہ_سرخی();
        ساکن خالی print_گرہ(ثابت RBBINode *n);
        ساکن خالی print_درخت(ثابت RBBINode *n, UBool withHeading);
#اگر_ختم

    نجی:
        RBBINode &عامل = (ثابت RBBINode &other); // No defs.
        bool عامل == (ثابت RBBINode &other);     // Private, so these functions won't accidentally be used.

#اگر_تعریف_شدہ RBBI_DEBUG
    عوامی:
        صحیح           f_Serial_عدد;           //  Debugging aids.
#اگر_ختم
};

#اگر_تعریف_شدہ RBBI_DEBUG
U_CFUNC خالی 
RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحیح min_چوڑائی=0);
#اگر_ختم

U_NAMESPACE_انتہا

#اگر_ختم

