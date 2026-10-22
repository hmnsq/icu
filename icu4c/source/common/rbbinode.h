// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#यदि_अपरिभाषितम् RBBINODE_H
#परिभाषय RBBINODE_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    Unicode_समुच्चयः;
class    UVector;

class RBBINode : सार्वजनिक UMemory {
    सार्वजनिक:
        गणना ग्रन्थिः_Type {
            समुच्चयः_Ref,
            uset,
            varRef,
            पर्णम्_Char,
            lookAhead,
            चिह्नकम्,
            अन्तः_Mark,
            opStart,
            opCat,
            opOr,
            op_तारकम्,
            opPlus,
            opQuestion,
            opBreak,
            opReverse,
            opLParen
        };

        गणना OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        ग्रन्थिः_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_वामः_Child;
        RBBINode      *f_दक्षिणः_Child;
        Unicode_समुच्चयः    *f_निवेशः_समुच्चयः;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        पूर्णाङ्क           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        पूर्णाङ्क           f_चरमः_Pos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         f_Look_Ahead_अन्तः;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_नियमः_मूलम्;            // True if this node is the root of a rule.
        UBool         f_Chain_अन्तर्;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *f_First_Pos_समुच्चयः;
        UVector       *f_चरमः_Pos_समुच्चयः;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(ग्रन्थिः_Type t, UError_संहिता& स्थितिः);
        RBBINode(ध्रुव RBBINode &other, UError_संहिता& स्थितिः);
        ~RBBINode();
        स्थिर शून्य  NRDelete_ग्रन्थिः(RBBINode *ग्रन्थिः);
        
        RBBINode    *clone_वृक्षः(UError_संहिता &स्थितिः, पूर्णाङ्क गभीरता=0);
        RBBINode    *flattenVariables(UError_संहिता &स्थितिः, पूर्णाङ्क गभीरता=0);
        शून्य         flattenSets(UError_संहिता &स्थितिः, पूर्णाङ्क गभीरता=0);
        शून्य         findNodes(UVector *dest, RBBINode::ग्रन्थिः_Type विधा, UError_संहिता &स्थितिः);

#यदि_परिभाषितम् RBBI_DEBUG
        स्थिर शून्य print_ग्रन्थिः_शीर्षकम्();
        स्थिर शून्य print_ग्रन्थिः(ध्रुव RBBINode *n);
        स्थिर शून्य print_वृक्षः(ध्रुव RBBINode *n, UBool withHeading);
#यदि_अन्तः

    गुप्त:
        RBBINode &संक्रियकः = (ध्रुव RBBINode &other); // No defs.
        bool संक्रियकः == (ध्रुव RBBINode &other);     // Private, so these functions won't accidentally be used.

#यदि_परिभाषितम् RBBI_DEBUG
    सार्वजनिक:
        पूर्णाङ्क           f_Serial_अङ्कः;           //  Debugging aids.
#यदि_अन्तः
};

#यदि_परिभाषितम् RBBI_DEBUG
U_CFUNC शून्य 
RBBI_DEBUG_printUnicodeString(ध्रुव UnicodeString &s, पूर्णाङ्क min_विस्तारः=0);
#यदि_अन्तः

U_NAMESPACE_अन्तः

#यदि_अन्तः

