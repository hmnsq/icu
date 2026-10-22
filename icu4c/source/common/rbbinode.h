// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#که_نه_تعریف_شوی RBBINODE_H
#تعریف_کړه RBBINODE_H

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

class    Unicode_سېټ;
class    UVector;

class RBBINode : عام UMemory {
    عام:
        تعداد غوټه_Type {
            سېټ_Ref,
            uset,
            varRef,
            ونې_پاڼه_Char,
            lookAhead,
            نښان,
            پای_ټکی_Mark,
            op_پیل,
            opCat,
            opOr,
            op_ستوری,
            opPlus,
            opQuestion,
            opBreak,
            op_سرچپه,
            opLParen
        };

        تعداد OpPrecedence {      
            precZero,
            prec_پیل,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        غوټه_Type      fType;
        RBBINode      *f_پلار;
        RBBINode      *f_کیڼ_ماشوم;
        RBBINode      *f_ښی_ماشوم;
        Unicode_سېټ    *f_ننوت_سېټ;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString f_متن;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        صحيح           f_لومړی_Pos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        صحيح           f_وروستی_Pos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         f_Look_Ahead_پای_ټکی;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_قاعده_ریښه;            // True if this node is the root of a rule.
        UBool         f_Chain_دننه;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *f_لومړی_Pos_سېټ;
        UVector       *f_وروستی_Pos_سېټ;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(غوټه_Type t, UError_کوډ& حالت);
        RBBINode(ثابت RBBINode &other, UError_کوډ& حالت);
        ~RBBINode();
        ساكن فراغ  NRDelete_غوټه(RBBINode *غوټه);
        
        RBBINode    *clone_ونه(UError_کوډ &حالت, صحيح ژوروالی=0);
        RBBINode    *flattenVariables(UError_کوډ &حالت, صحيح ژوروالی=0);
        فراغ         flattenSets(UError_کوډ &حالت, صحيح ژوروالی=0);
        فراغ         پیدا_کړه_Nodes(UVector *dest, RBBINode::غوټه_Type ډول, UError_کوډ &حالت);

#که_تعریف_شوی RBBI_DEBUG
        ساكن فراغ print_غوټه_سرلیک();
        ساكن فراغ print_غوټه(ثابت RBBINode *n);
        ساكن فراغ print_ونه(ثابت RBBINode *n, UBool withHeading);
#د_که_پای

    خاص:
        RBBINode &چلوونکی = (ثابت RBBINode &other); // No defs.
        bool چلوونکی == (ثابت RBBINode &other);     // Private, so these functions won't accidentally be used.

#که_تعریف_شوی RBBI_DEBUG
    عام:
        صحيح           f_Serial_عدد;           //  Debugging aids.
#د_که_پای
};

#که_تعریف_شوی RBBI_DEBUG
U_CFUNC فراغ 
RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح min_پلنوالی=0);
#د_که_پای

U_NAMESPACE_پای_ټکی

#د_که_پای

