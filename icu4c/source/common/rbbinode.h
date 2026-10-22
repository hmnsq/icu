// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ifndef RBBINODE_H
#define RBBINODE_H

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    UnicodeSet;
class    UVector;

class RBBINode : ସର୍ବସାଧାରଣ UMemory {
    ସର୍ବସାଧାରଣ:
        ଗଣନା ଗଣ୍ଠି_Type {
            setRef,
            uset,
            varRef,
            leafChar,
            lookAhead,
            tag,
            endMark,
            opStart,
            opCat,
            opOr,
            opStar,
            opPlus,
            opQuestion,
            opBreak,
            opReverse,
            opLParen
        };

        ଗଣନା OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        ଗଣ୍ଠି_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_ବାମ_Child;
        RBBINode      *f_ଡାହାଣ_Child;
        UnicodeSet    *f_ନିବେଶ_Set;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା           fLastPos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         fLookAheadEnd;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_Rule_ମୂଳ;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(ଗଣ୍ଠି_Type t, UErrorCode& status);
        RBBINode(ଅଚଳ RBBINode &other, UErrorCode& status);
        ~RBBINode();
        ସ୍ଥିର ଶୂନ୍ୟ  NRDelete_ଗଣ୍ଠି(RBBINode *ଗଣ୍ଠି);
        
        RBBINode    *clone_ବୃକ୍ଷ(UErrorCode &status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth=0);
        ଶୂନ୍ୟ         flattenSets(UErrorCode &status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth=0);
        ଶୂନ୍ୟ         findNodes(UVector *dest, RBBINode::ଗଣ୍ଠି_Type kind, UErrorCode &status);

#ifdef RBBI_DEBUG
        ସ୍ଥିର ଶୂନ୍ୟ print_ଗଣ୍ଠି_Header();
        ସ୍ଥିର ଶୂନ୍ୟ print_ଗଣ୍ଠି(ଅଚଳ RBBINode *n);
        ସ୍ଥିର ଶୂନ୍ୟ print_ବୃକ୍ଷ(ଅଚଳ RBBINode *n, UBool withHeading);
#endif

    ବ୍ୟକ୍ତିଗତ:
        RBBINode &operator = (ଅଚଳ RBBINode &other); // No defs.
        bool operator == (ଅଚଳ RBBINode &other);     // Private, so these functions won't accidentally be used.

#ifdef RBBI_DEBUG
    ସର୍ବସାଧାରଣ:
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା           f_Serial_ଅଙ୍କ;           //  Debugging aids.
#endif
};

#ifdef RBBI_DEBUG
U_CFUNC ଶୂନ୍ୟ 
RBBI_DEBUG_printUnicodeString(ଅଚଳ UnicodeString &s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା minWidth=0);
#endif

U_NAMESPACE_END

#endif

