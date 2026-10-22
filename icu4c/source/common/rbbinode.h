// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#اگر_تعریف_نشده RBBINODE_H
#تعریف_کن RBBINODE_H

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

class    Unicode_مجموعه;
class    UVector;

class RBBINode : عام UMemory {
    عام:
        تعداد گره_Type {
            مجموعه_Ref,
            uset,
            varRef,
            برگ_Char,
            lookAhead,
            برچسب,
            انتها_Mark,
            op_شروع,
            opCat,
            opOr,
            op_ستاره,
            opPlus,
            opQuestion,
            opBreak,
            op_معکوس,
            opLParen
        };

        تعداد OpPrecedence {      
            precZero,
            prec_شروع,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        گره_Type      fType;
        RBBINode      *f_والد;
        RBBINode      *f_چپ_فرزند;
        RBBINode      *f_راست_فرزند;
        Unicode_مجموعه    *f_درون_داد_مجموعه;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString f_متن;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        صحيح           f_اولی_Pos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        صحيح           f_آخرین_Pos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         f_Look_Ahead_انتها;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_قاعده_ریشه;            // True if this node is the root of a rule.
        UBool         f_Chain_درون;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *f_اولی_Pos_مجموعه;
        UVector       *f_آخرین_Pos_مجموعه;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(گره_Type t, UError_کد& وضعیت);
        RBBINode(ثابت RBBINode &other, UError_کد& وضعیت);
        ~RBBINode();
        ساكن فراغ  NRDelete_گره(RBBINode *گره);
        
        RBBINode    *clone_درخت(UError_کد &وضعیت, صحيح عمق=0);
        RBBINode    *flattenVariables(UError_کد &وضعیت, صحيح عمق=0);
        فراغ         flattenSets(UError_کد &وضعیت, صحيح عمق=0);
        فراغ         پیدا_کن_Nodes(UVector *dest, RBBINode::گره_Type نوع, UError_کد &وضعیت);

#اگر_تعریف_شده RBBI_DEBUG
        ساكن فراغ print_گره_سرآیند();
        ساكن فراغ print_گره(ثابت RBBINode *n);
        ساكن فراغ print_درخت(ثابت RBBINode *n, UBool withHeading);
#پایان_اگر

    خاص:
        RBBINode &عملگر = (ثابت RBBINode &other); // No defs.
        bool عملگر == (ثابت RBBINode &other);     // Private, so these functions won't accidentally be used.

#اگر_تعریف_شده RBBI_DEBUG
    عام:
        صحيح           f_Serial_رقم;           //  Debugging aids.
#پایان_اگر
};

#اگر_تعریف_شده RBBI_DEBUG
U_CFUNC فراغ 
RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح min_عرض=0);
#پایان_اگر

U_NAMESPACE_انتها

#پایان_اگر

