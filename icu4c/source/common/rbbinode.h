// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RBBINODE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RBBINODE_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    Unicode_ਸੈੱਟ;
class    UVector;

class RBBINode : ਜਨਤਕ UMemory {
    ਜਨਤਕ:
        ਗਿਣਤੀ ਗੰਢ_Type {
            ਸੈੱਟ_Ref,
            uset,
            varRef,
            ਪੱਤੀ_Char,
            lookAhead,
            ਟੈਗ,
            ਸਿਰਾ_Mark,
            op_ਸ਼ੁਰੂ,
            opCat,
            opOr,
            op_ਤਾਰਾ,
            opPlus,
            opQuestion,
            opBreak,
            op_ਉਲਟ,
            opLParen
        };

        ਗਿਣਤੀ OpPrecedence {      
            precZero,
            prec_ਸ਼ੁਰੂ,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        ਗੰਢ_Type      fType;
        RBBINode      *f_ਮਾਪਾ;
        RBBINode      *f_ਖੱਬਾ_ਬੱਚਾ;
        RBBINode      *f_ਸੱਜਾ_ਬੱਚਾ;
        Unicode_ਸੈੱਟ    *f_ਨਿਵੇਸ਼_ਸੈੱਟ;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString f_ਪਾਠ;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        ਪੂਰਨ           f_ਪਹਿਲਾ_Pos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        ਪੂਰਨ           f_ਆਖਰੀ_Pos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         f_Look_Ahead_ਸਿਰਾ;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_ਨਿਯਮ_ਜੜ੍ਹ;            // True if this node is the root of a rule.
        UBool         f_Chain_ਵਿੱਚ;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *f_ਪਹਿਲਾ_Pos_ਸੈੱਟ;
        UVector       *f_ਆਖਰੀ_Pos_ਸੈੱਟ;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(ਗੰਢ_Type t, UError_ਕੋਡ& ਸਥਿਤੀ);
        RBBINode(ਅਚਲ RBBINode &other, UError_ਕੋਡ& ਸਥਿਤੀ);
        ~RBBINode();
        ਸਥਿਰ ਖਾਲੀ  NRDelete_ਗੰਢ(RBBINode *ਗੰਢ);
        
        RBBINode    *clone_ਰੁੱਖ(UError_ਕੋਡ &ਸਥਿਤੀ, ਪੂਰਨ ਡੂੰਘਾਈ=0);
        RBBINode    *flattenVariables(UError_ਕੋਡ &ਸਥਿਤੀ, ਪੂਰਨ ਡੂੰਘਾਈ=0);
        ਖਾਲੀ         flattenSets(UError_ਕੋਡ &ਸਥਿਤੀ, ਪੂਰਨ ਡੂੰਘਾਈ=0);
        ਖਾਲੀ         ਲੱਭੋ_Nodes(UVector *dest, RBBINode::ਗੰਢ_Type ਕਿਸਮ, UError_ਕੋਡ &ਸਥਿਤੀ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
        ਸਥਿਰ ਖਾਲੀ print_ਗੰਢ_ਸਿਰਲੇਖ();
        ਸਥਿਰ ਖਾਲੀ print_ਗੰਢ(ਅਚਲ RBBINode *n);
        ਸਥਿਰ ਖਾਲੀ print_ਰੁੱਖ(ਅਚਲ RBBINode *n, UBool withHeading);
#ਜੇ_ਅੰਤ

    ਨਿਜੀ:
        RBBINode &ਸੰਚਾਲਕ = (ਅਚਲ RBBINode &other); // No defs.
        bool ਸੰਚਾਲਕ == (ਅਚਲ RBBINode &other);     // Private, so these functions won't accidentally be used.

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਜਨਤਕ:
        ਪੂਰਨ           f_Serial_ਅੰਕ;           //  Debugging aids.
#ਜੇ_ਅੰਤ
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
U_CFUNC ਖਾਲੀ 
RBBI_DEBUG_printUnicodeString(ਅਚਲ UnicodeString &s, ਪੂਰਨ min_ਚੌੜਾਈ=0);
#ਜੇ_ਅੰਤ

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ

