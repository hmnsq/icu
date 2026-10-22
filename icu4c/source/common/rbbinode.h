// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#جيڪڏهن_وصف_نه_ٿيل RBBINODE_H
#وصف_ڪريو RBBINODE_H

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

class    Unicode_سيٽ;
class    UVector;

class RBBINode : عام UMemory {
    عام:
        تعداد ڳنڍ_Type {
            سيٽ_Ref,
            uset,
            varRef,
            پن_Char,
            lookAhead,
            ٽيگ,
            پڇاڙي_Mark,
            op_شروعات,
            opCat,
            opOr,
            op_تارو,
            opPlus,
            opQuestion,
            opBreak,
            op_ابتو,
            opLParen
        };

        تعداد OpPrecedence {      
            precZero,
            prec_شروعات,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        ڳنڍ_Type      fType;
        RBBINode      *f_والد;
        RBBINode      *f_کاٻو_ٻار;
        RBBINode      *f_ساڄو_ٻار;
        Unicode_سيٽ    *f_داخلا_سيٽ;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString f_متن;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        صحيح           f_پهريون_Pos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        صحيح           f_آخري_Pos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         f_Look_Ahead_پڇاڙي;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_قاعدو_پاڙ;            // True if this node is the root of a rule.
        UBool         f_Chain_اندريون;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *f_پهريون_Pos_سيٽ;
        UVector       *f_آخري_Pos_سيٽ;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(ڳنڍ_Type t, UError_ڪوڊ& حالت);
        RBBINode(ثابت RBBINode &other, UError_ڪوڊ& حالت);
        ~RBBINode();
        ساكن فراغ  NRDelete_ڳنڍ(RBBINode *ڳنڍ);
        
        RBBINode    *clone_وڻ(UError_ڪوڊ &حالت, صحيح اونهائي=0);
        RBBINode    *flattenVariables(UError_ڪوڊ &حالت, صحيح اونهائي=0);
        فراغ         flattenSets(UError_ڪوڊ &حالت, صحيح اونهائي=0);
        فراغ         ڳوليو_Nodes(UVector *dest, RBBINode::ڳنڍ_Type قسم, UError_ڪوڊ &حالت);

#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
        ساكن فراغ print_ڳنڍ_سرو();
        ساكن فراغ print_ڳنڍ(ثابت RBBINode *n);
        ساكن فراغ print_وڻ(ثابت RBBINode *n, UBool withHeading);
#جيڪڏهن_پورو

    خاص:
        RBBINode &عامل = (ثابت RBBINode &other); // No defs.
        bool عامل == (ثابت RBBINode &other);     // Private, so these functions won't accidentally be used.

#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    عام:
        صحيح           f_Serial_انگ;           //  Debugging aids.
#جيڪڏهن_پورو
};

#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
U_CFUNC فراغ 
RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح min_ويڪر=0);
#جيڪڏهن_پورو

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو

