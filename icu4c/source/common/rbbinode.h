// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RBBINODE_H
#ವ್ಯಾಖ್ಯಾನಿಸು RBBINODE_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    Unicode_ಗಣ;
class    UVector;

class RBBINode : ಸಾರ್ವಜನಿಕ UMemory {
    ಸಾರ್ವಜನಿಕ:
        ಎಣಿಕೆ ಗಂಟು_Type {
            ಗಣ_Ref,
            uset,
            varRef,
            ಎಲೆ_Char,
            lookAhead,
            ಟ್ಯಾಗ್,
            ಕೊನೆ_Mark,
            op_ಆರಂಭ,
            opCat,
            opOr,
            op_ನಕ್ಷತ್ರ,
            opPlus,
            opQuestion,
            opBreak,
            op_ಹಿಮ್ಮುಖ,
            opLParen
        };

        ಎಣಿಕೆ OpPrecedence {      
            precZero,
            prec_ಆರಂಭ,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        ಗಂಟು_Type      fType;
        RBBINode      *f_ಪೋಷಕ;
        RBBINode      *f_ಎಡ_ಮಗು;
        RBBINode      *f_ಬಲ_ಮಗು;
        Unicode_ಗಣ    *f_ಒಳಹರಿವು_ಗಣ;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString f_ಪಠ್ಯ;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        ಪೂರ್ಣಾಂಕ           f_ಮೊದಲ_Pos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        ಪೂರ್ಣಾಂಕ           f_ಕೊನೆಯ_Pos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         f_Look_Ahead_ಕೊನೆ;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_ನಿಯಮ_ಬೇರು;            // True if this node is the root of a rule.
        UBool         f_Chain_ಒಳ;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *f_ಮೊದಲ_Pos_ಗಣ;
        UVector       *f_ಕೊನೆಯ_Pos_ಗಣ;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(ಗಂಟು_Type t, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);
        RBBINode(ಸ್ಥಿರಾಂಕ RBBINode &other, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);
        ~RBBINode();
        ಸ್ಥಿರ ಶೂನ್ಯ  NRDelete_ಗಂಟು(RBBINode *ಗಂಟು);
        
        RBBINode    *clone_ವೃಕ್ಷ(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ ಆಳ=0);
        RBBINode    *flattenVariables(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ ಆಳ=0);
        ಶೂನ್ಯ         flattenSets(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ ಆಳ=0);
        ಶೂನ್ಯ         ಕಂಡುಹಿಡಿ_Nodes(UVector *dest, RBBINode::ಗಂಟು_Type ಬಗೆ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
        ಸ್ಥಿರ ಶೂನ್ಯ print_ಗಂಟು_ಶೀರ್ಷಿಕೆ();
        ಸ್ಥಿರ ಶೂನ್ಯ print_ಗಂಟು(ಸ್ಥಿರಾಂಕ RBBINode *n);
        ಸ್ಥಿರ ಶೂನ್ಯ print_ವೃಕ್ಷ(ಸ್ಥಿರಾಂಕ RBBINode *n, UBool withHeading);
#ಆದರೆ_ಅಂತ್ಯ

    ಖಾಸಗಿ:
        RBBINode &ಕ್ರಿಯಾಕಾರಕ = (ಸ್ಥಿರಾಂಕ RBBINode &other); // No defs.
        bool ಕ್ರಿಯಾಕಾರಕ == (ಸ್ಥಿರಾಂಕ RBBINode &other);     // Private, so these functions won't accidentally be used.

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ಸಾರ್ವಜನಿಕ:
        ಪೂರ್ಣಾಂಕ           f_Serial_ಅಂಕೆ;           //  Debugging aids.
#ಆದರೆ_ಅಂತ್ಯ
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
U_CFUNC ಶೂನ್ಯ 
RBBI_DEBUG_printUnicodeString(ಸ್ಥಿರಾಂಕ UnicodeString &s, ಪೂರ್ಣಾಂಕ min_ಅಗಲ=0);
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

