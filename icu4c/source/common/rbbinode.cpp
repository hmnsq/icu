// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 2002-2016 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

//
//  File:  rbbinode.cpp
//
//         Implementation of class RBBINode, which represents a node in the
//         tree generated when parsing the Rules Based Break Iterator rules.
//
//         This "Class" is actually closer to a struct.
//         Code using it is expected to directly access fields much of the time.
//

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "cstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbinode.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"


U_NAMESPACE_BEGIN

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਸਥਿਰ ਪੂਰਨ  g_ਆਖਰੀ_Serial = 0;
#ਜੇ_ਅੰਤ


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(ਗੰਢ_Type t, UError_ਕੋਡ& ਸਥਿਤੀ) : UMemory() {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    f_Serial_ਅੰਕ    = ++g_ਆਖਰੀ_Serial;
#ਜੇ_ਅੰਤ
    fType         = t;
    f_ਮਾਪਾ       = ਖਾਲੀ_ਸੂਚਕ;
    f_ਖੱਬਾ_ਬੱਚਾ    = ਖਾਲੀ_ਸੂਚਕ;
    f_ਸੱਜਾ_ਬੱਚਾ   = ਖਾਲੀ_ਸੂਚਕ;
    f_ਨਿਵੇਸ਼_ਸੈੱਟ     = ਖਾਲੀ_ਸੂਚਕ;
    f_ਪਹਿਲਾ_Pos     = 0;
    f_ਆਖਰੀ_Pos      = 0;
    fNullable     = ਝੂਠ;
    f_Look_Ahead_ਸਿਰਾ = ਝੂਠ;
    f_ਨਿਯਮ_ਜੜ੍ਹ     = ਝੂਠ;
    f_Chain_ਵਿੱਚ      = ਝੂਠ;
    fVal          = 0;
    fPrecedence   = precZero;

    f_ਪਹਿਲਾ_Pos_ਸੈੱਟ  = ਨਵਾਂ UVector(ਸਥਿਤੀ);
    f_ਆਖਰੀ_Pos_ਸੈੱਟ   = ਨਵਾਂ UVector(ਸਥਿਤੀ);
    fFollowPos    = ਨਵਾਂ UVector(ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ) &&
        (f_ਪਹਿਲਾ_Pos_ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ || f_ਆਖਰੀ_Pos_ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ || fFollowPos == ਖਾਲੀ_ਸੂਚਕ)) {
        ਸਥਿਤੀ =  U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ      (t==opCat)    {fPrecedence = precOpCat;}
    ਨਹੀਂਤਾਂ ਜੇ (t==opOr)     {fPrecedence = precOpOr;}
    ਨਹੀਂਤਾਂ ਜੇ (t==op_ਸ਼ੁਰੂ)  {fPrecedence = prec_ਸ਼ੁਰੂ;}
    ਨਹੀਂਤਾਂ ਜੇ (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ਅਚਲ RBBINode &other, UError_ਕੋਡ& ਸਥਿਤੀ) : UMemory(other) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    f_Serial_ਅੰਕ   = ++g_ਆਖਰੀ_Serial;
#ਜੇ_ਅੰਤ
    fType        = other.fType;
    f_ਮਾਪਾ      = ਖਾਲੀ_ਸੂਚਕ;
    f_ਖੱਬਾ_ਬੱਚਾ   = ਖਾਲੀ_ਸੂਚਕ;
    f_ਸੱਜਾ_ਬੱਚਾ  = ਖਾਲੀ_ਸੂਚਕ;
    f_ਨਿਵੇਸ਼_ਸੈੱਟ    = other.f_ਨਿਵੇਸ਼_ਸੈੱਟ;
    fPrecedence  = other.fPrecedence;
    f_ਪਾਠ        = other.f_ਪਾਠ;
    f_ਪਹਿਲਾ_Pos    = other.f_ਪਹਿਲਾ_Pos;
    f_ਆਖਰੀ_Pos     = other.f_ਆਖਰੀ_Pos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_ਨਿਯਮ_ਜੜ੍ਹ    = ਝੂਠ;
    f_Chain_ਵਿੱਚ     = other.f_Chain_ਵਿੱਚ;
    f_ਪਹਿਲਾ_Pos_ਸੈੱਟ = ਨਵਾਂ UVector(ਸਥਿਤੀ);   // TODO - get a real status from somewhere
    f_ਆਖਰੀ_Pos_ਸੈੱਟ  = ਨਵਾਂ UVector(ਸਥਿਤੀ);
    fFollowPos   = ਨਵਾਂ UVector(ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ) &&
        (f_ਪਹਿਲਾ_Pos_ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ || f_ਆਖਰੀ_Pos_ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ || fFollowPos == ਖਾਲੀ_ਸੂਚਕ)) {
        ਸਥਿਤੀ =  U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}


//-------------------------------------------------------------------------
//
//    Destructor.   Deletes both this node AND any child nodes,
//                  except in the case of variable reference nodes.  For
//                  these, the l. child points back to the definition, which
//                  is common for all references to the variable, meaning
//                  it can't be deleted here.
//
//-------------------------------------------------------------------------
RBBINode::~RBBINode() {
    // printf("deleting node %8x   serial %4d\n", this, this->fSerialNum);
    ਮਿਟਾਓ f_ਨਿਵੇਸ਼_ਸੈੱਟ;
    f_ਨਿਵੇਸ਼_ਸੈੱਟ = ਖਾਲੀ_ਸੂਚਕ;

    ਚੋਣ (ਇਹ->fType) {
    ਹਾਲਤ varRef:
    ਹਾਲਤ ਸੈੱਟ_Ref:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        ਰੋਕੋ;

    ਮੂਲ:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_ਗੰਢ(f_ਖੱਬਾ_ਬੱਚਾ);
        f_ਖੱਬਾ_ਬੱਚਾ =   ਖਾਲੀ_ਸੂਚਕ;
        // delete        fRightChild;
        NRDelete_ਗੰਢ(f_ਸੱਜਾ_ਬੱਚਾ);
        f_ਸੱਜਾ_ਬੱਚਾ = ਖਾਲੀ_ਸੂਚਕ;
    }

    ਮਿਟਾਓ f_ਪਹਿਲਾ_Pos_ਸੈੱਟ;
    ਮਿਟਾਓ f_ਆਖਰੀ_Pos_ਸੈੱਟ;
    ਮਿਟਾਓ fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
ਖਾਲੀ RBBINode::NRDelete_ਗੰਢ(RBBINode *ਗੰਢ) {
    ਜੇ (ਗੰਢ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    RBBINode *stop_ਗੰਢ = ਗੰਢ->f_ਮਾਪਾ;
    RBBINode *ਅਗਲਾ_ਗੰਢ = ਗੰਢ;
    ਜਦੋਂ (ਅਗਲਾ_ਗੰਢ != stop_ਗੰਢ && ਅਗਲਾ_ਗੰਢ != ਖਾਲੀ_ਸੂਚਕ) {
        RBBINode *current_ਗੰਢ = ਅਗਲਾ_ਗੰਢ;

        ਜੇ ((current_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ == ਖਾਲੀ_ਸੂਚਕ && current_ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ == ਖਾਲੀ_ਸੂਚਕ) ||
                current_ਗੰਢ->fType == varRef ||      // varRef and setRef nodes do not
                current_ਗੰਢ->fType == ਸੈੱਟ_Ref) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            ਅਗਲਾ_ਗੰਢ = current_ਗੰਢ->f_ਮਾਪਾ;
            ਜੇ (ਅਗਲਾ_ਗੰਢ) {
                ਜੇ (ਅਗਲਾ_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ == current_ਗੰਢ) {
                    ਅਗਲਾ_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ = ਖਾਲੀ_ਸੂਚਕ;
                } ਨਹੀਂਤਾਂ ਜੇ (ਅਗਲਾ_ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ == current_ਗੰਢ) {
                    ਅਗਲਾ_ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ = ਖਾਲੀ_ਸੂਚਕ;
                }
            }
            ਮਿਟਾਓ current_ਗੰਢ;
        } ਨਹੀਂਤਾਂ ਜੇ (current_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ) {
            ਅਗਲਾ_ਗੰਢ = current_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ;
            ਜੇ (ਅਗਲਾ_ਗੰਢ->f_ਮਾਪਾ == ਖਾਲੀ_ਸੂਚਕ) {
                ਅਗਲਾ_ਗੰਢ->f_ਮਾਪਾ = current_ਗੰਢ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(ਅਗਲਾ_ਗੰਢ->f_ਮਾਪਾ == current_ਗੰਢ);
        } ਨਹੀਂਤਾਂ ਜੇ (current_ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ) {
            ਅਗਲਾ_ਗੰਢ = current_ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ;
            ਜੇ (ਅਗਲਾ_ਗੰਢ->f_ਮਾਪਾ == ਖਾਲੀ_ਸੂਚਕ) {
                ਅਗਲਾ_ਗੰਢ->f_ਮਾਪਾ = current_ਗੰਢ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(ਅਗਲਾ_ਗੰਢ->f_ਮਾਪਾ == current_ਗੰਢ);
        }
    }
}

//-------------------------------------------------------------------------
//
//    cloneTree     Make a copy of the subtree rooted at this node.
//                  Discard any variable references encountered along the way,
//                  and replace with copies of the variable's definitions.
//                  Used to replicate the expression underneath variable
//                  references in preparation for generating the DFA tables.
//
//-------------------------------------------------------------------------
constexpr ਪੂਰਨ k_Recursive_ਡੂੰਘਾਈ_ਹੱਦਬੰਦੀ = 3500;
RBBINode *RBBINode::clone_ਰੁੱਖ(UError_ਕੋਡ &ਸਥਿਤੀ, ਪੂਰਨ ਡੂੰਘਾਈ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ਜੇ (ਡੂੰਘਾਈ > k_Recursive_ਡੂੰਘਾਈ_ਹੱਦਬੰਦੀ) {
        ਸਥਿਤੀ = U_ਨਿਵੇਸ਼_TOO_LONG_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    RBBINode    *n;

    ਜੇ (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_ਖੱਬਾ_ਬੱਚਾ->clone_ਰੁੱਖ(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    } ਨਹੀਂਤਾਂ ਜੇ (fType == RBBINode::uset) {
        n = ਇਹ;
    } ਨਹੀਂਤਾਂ {
        n = ਨਵਾਂ RBBINode(*ਇਹ, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਮਿਟਾਓ n;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        // Check for null pointer.
        ਜੇ (n == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ =  U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਜੇ (f_ਖੱਬਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
            n->f_ਖੱਬਾ_ਬੱਚਾ          = f_ਖੱਬਾ_ਬੱਚਾ->clone_ਰੁੱਖ(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਮਿਟਾਓ n;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            n->f_ਖੱਬਾ_ਬੱਚਾ->f_ਮਾਪਾ = n;
        }
        ਜੇ (f_ਸੱਜਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
            n->f_ਸੱਜਾ_ਬੱਚਾ          = f_ਸੱਜਾ_ਬੱਚਾ->clone_ਰੁੱਖ(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਮਿਟਾਓ n;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            n->f_ਸੱਜਾ_ਬੱਚਾ->f_ਮਾਪਾ = n;
        }
    }
    ਵਾਪਸ n;
}



//-------------------------------------------------------------------------
//
//   flattenVariables   Walk a parse tree, replacing any variable
//                      references with a copy of the variable's definition.
//                      Aside from variables, the tree is not changed.
//
//                      Return the root of the tree.  If the root was not a variable
//                      reference, it remains unchanged - the root we started with
//                      is the root we return.  If, however, the root was a variable
//                      reference, the root of the newly cloned replacement tree will
//                      be returned, and the original tree deleted.
//
//                      This function works by recursively walking the tree
//                      without doing anything until a variable reference is
//                      found, then calling cloneTree() at that point.  Any
//                      nested references are handled by cloneTree(), not here.
//
//-------------------------------------------------------------------------
RBBINode *RBBINode::flattenVariables(UError_ਕੋਡ& ਸਥਿਤੀ, ਪੂਰਨ ਡੂੰਘਾਈ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਇਹ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ਜੇ (ਡੂੰਘਾਈ > k_Recursive_ਡੂੰਘਾਈ_ਹੱਦਬੰਦੀ) {
        ਸਥਿਤੀ = U_ਨਿਵੇਸ਼_TOO_LONG_ERROR;
        ਵਾਪਸ ਇਹ;
    }
    ਜੇ (fType == varRef) {
        RBBINode *ret_ਗੰਢ  = f_ਖੱਬਾ_ਬੱਚਾ->clone_ਰੁੱਖ(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਇਹ;
        }
        ret_ਗੰਢ->f_ਨਿਯਮ_ਜੜ੍ਹ = ਇਹ->f_ਨਿਯਮ_ਜੜ੍ਹ;
        ret_ਗੰਢ->f_Chain_ਵਿੱਚ  = ਇਹ->f_Chain_ਵਿੱਚ;
        ਮਿਟਾਓ ਇਹ;   // TODO: undefined behavior. Fix.
        ਵਾਪਸ ret_ਗੰਢ;
    }

    ਜੇ (f_ਖੱਬਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
        f_ਖੱਬਾ_ਬੱਚਾ = f_ਖੱਬਾ_ਬੱਚਾ->flattenVariables(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
        ਜੇ (f_ਖੱਬਾ_ਬੱਚਾ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਇਹ;
        }
        f_ਖੱਬਾ_ਬੱਚਾ->f_ਮਾਪਾ  = ਇਹ;
    }
    ਜੇ (f_ਸੱਜਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
        f_ਸੱਜਾ_ਬੱਚਾ = f_ਸੱਜਾ_ਬੱਚਾ->flattenVariables(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
        ਜੇ (f_ਸੱਜਾ_ਬੱਚਾ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਇਹ;
        }
        f_ਸੱਜਾ_ਬੱਚਾ->f_ਮਾਪਾ = ਇਹ;
    }
    ਵਾਪਸ ਇਹ;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
ਖਾਲੀ RBBINode::flattenSets(UError_ਕੋਡ &ਸਥਿਤੀ, ਪੂਰਨ ਡੂੰਘਾਈ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ਜੇ (ਡੂੰਘਾਈ > k_Recursive_ਡੂੰਘਾਈ_ਹੱਦਬੰਦੀ) {
        ਸਥਿਤੀ = U_ਨਿਵੇਸ਼_TOO_LONG_ERROR;
        ਵਾਪਸ;
    }
    U_ASSERT(fType != ਸੈੱਟ_Ref);

    ਜੇ (f_ਖੱਬਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (f_ਖੱਬਾ_ਬੱਚਾ->fType==ਸੈੱਟ_Ref) {
            RBBINode *ਸੈੱਟ_Ref_ਗੰਢ = f_ਖੱਬਾ_ਬੱਚਾ;
            RBBINode *uset_ਗੰਢ   = ਸੈੱਟ_Ref_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ;
            RBBINode *repl_ਰੁੱਖ   = uset_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ;
            f_ਖੱਬਾ_ਬੱਚਾ           = repl_ਰੁੱਖ->clone_ਰੁੱਖ(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਮਿਟਾਓ ਸੈੱਟ_Ref_ਗੰਢ;
                ਵਾਪਸ;
            }
            f_ਖੱਬਾ_ਬੱਚਾ->f_ਮਾਪਾ  = ਇਹ;
            ਮਿਟਾਓ ਸੈੱਟ_Ref_ਗੰਢ;
        } ਨਹੀਂਤਾਂ {
            f_ਖੱਬਾ_ਬੱਚਾ->flattenSets(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
        }
    }

    ਜੇ (f_ਸੱਜਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (f_ਸੱਜਾ_ਬੱਚਾ->fType==ਸੈੱਟ_Ref) {
            RBBINode *ਸੈੱਟ_Ref_ਗੰਢ = f_ਸੱਜਾ_ਬੱਚਾ;
            RBBINode *uset_ਗੰਢ   = ਸੈੱਟ_Ref_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ;
            RBBINode *repl_ਰੁੱਖ   = uset_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ;
            f_ਸੱਜਾ_ਬੱਚਾ           = repl_ਰੁੱਖ->clone_ਰੁੱਖ(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਮਿਟਾਓ ਸੈੱਟ_Ref_ਗੰਢ;
                ਵਾਪਸ;
            }
            f_ਸੱਜਾ_ਬੱਚਾ->f_ਮਾਪਾ  = ਇਹ;
            ਮਿਟਾਓ ਸੈੱਟ_Ref_ਗੰਢ;
        } ਨਹੀਂਤਾਂ {
            f_ਸੱਜਾ_ਬੱਚਾ->flattenSets(ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
ਖਾਲੀ   RBBINode::ਲੱਭੋ_Nodes(UVector *dest, RBBINode::ਗੰਢ_Type ਕਿਸਮ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    /* test for buffer overflows */
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    U_ASSERT(!dest->hasDeleter());
    ਜੇ (fType == ਕਿਸਮ) {
        dest->ਜੋੜੋ_Element(ਇਹ, ਸਥਿਤੀ);
    }
    ਜੇ (f_ਖੱਬਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
        f_ਖੱਬਾ_ਬੱਚਾ->ਲੱਭੋ_Nodes(dest, ਕਿਸਮ, ਸਥਿਤੀ);
    }
    ਜੇ (f_ਸੱਜਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
        f_ਸੱਜਾ_ਬੱਚਾ->ਲੱਭੋ_Nodes(dest, ਕਿਸਮ, ਸਥਿਤੀ);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG

ਸਥਿਰ int32_t serial(ਅਚਲ RBBINode *ਗੰਢ) {
    ਵਾਪਸ (ਗੰਢ == ਖਾਲੀ_ਸੂਚਕ? -1 : ਗੰਢ->f_Serial_ਅੰਕ);
}


ਖਾਲੀ RBBINode::print_ਗੰਢ(ਅਚਲ RBBINode *ਗੰਢ) {
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ਗੰਢ_Type_ਨਾਂ[] = {
                "setRef",
                "uset",
                "varRef",
                "leafChar",
                "lookAhead",
                "tag",
                "endMark",
                "opStart",
                "opCat",
                "opOr",
                "opStar",
                "opPlus",
                "opQuestion",
                "opBreak",
                "opReverse",
                "opLParen"
    };

    ਜੇ (ਗੰਢ==ਖਾਲੀ_ਸੂਚਕ) {
        RBBIDebugPrintf("%10p", (ਖਾਲੀ *)ਗੰਢ);
    } ਨਹੀਂਤਾਂ {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (ਖਾਲੀ *)ਗੰਢ, ਗੰਢ->f_Serial_ਅੰਕ, ਗੰਢ_Type_ਨਾਂ[ਗੰਢ->fType],
            ਗੰਢ->f_ਨਿਯਮ_ਜੜ੍ਹ?'R':' ', ਗੰਢ->f_Chain_ਵਿੱਚ?'C':' ',
            serial(ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ), serial(ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ), serial(ਗੰਢ->f_ਮਾਪਾ),
            ਗੰਢ->f_ਪਹਿਲਾ_Pos, ਗੰਢ->fVal);
        ਜੇ (ਗੰਢ->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(ਗੰਢ->f_ਪਾਠ);
        }
    }
    RBBIDebugPrintf("\n");
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
U_CFUNC ਖਾਲੀ RBBI_DEBUG_printUnicodeString(ਅਚਲ UnicodeString &s, ਪੂਰਨ min_ਚੌੜਾਈ) {
    RBBIDebugPrintf("%*s", min_ਚੌੜਾਈ, CStr(s)());
}
#ਜੇ_ਅੰਤ


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBINode::print_ਗੰਢ_ਸਿਰਲੇਖ() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
ਖਾਲੀ RBBINode::print_ਰੁੱਖ(ਅਚਲ RBBINode *ਗੰਢ, UBool printHeading) {
    ਜੇ (printHeading) {
        print_ਗੰਢ_ਸਿਰਲੇਖ();
    }
    print_ਗੰਢ(ਗੰਢ);
    ਜੇ (ਗੰਢ != ਖਾਲੀ_ਸੂਚਕ) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        ਜੇ (ਗੰਢ->fType != varRef) {
            ਜੇ (ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
                print_ਰੁੱਖ(ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ, ਝੂਠ);
            }
            
            ਜੇ (ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
                print_ਰੁੱਖ(ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ, ਝੂਠ);
            }
        }
    }
}
#ਜੇ_ਅੰਤ



U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
