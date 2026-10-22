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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"

#ಅನ್ತರ್ಭಾವ "cstr.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "rbbinode.h"

#ಅನ್ತರ್ಭಾವ "uassert.h"


U_NAMESPACE_BEGIN

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  g_ಕೊನೆಯ_Serial = 0;
#ಆದರೆ_ಅಂತ್ಯ


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(ಗಂಟು_Type t, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) : UMemory() {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    f_Serial_ಅಂಕೆ    = ++g_ಕೊನೆಯ_Serial;
#ಆದರೆ_ಅಂತ್ಯ
    fType         = t;
    f_ಪೋಷಕ       = ಶೂನ್ಯಸೂಚಕ;
    f_ಎಡ_ಮಗು    = ಶೂನ್ಯಸೂಚಕ;
    f_ಬಲ_ಮಗು   = ಶೂನ್ಯಸೂಚಕ;
    f_ಒಳಹರಿವು_ಗಣ     = ಶೂನ್ಯಸೂಚಕ;
    f_ಮೊದಲ_Pos     = 0;
    f_ಕೊನೆಯ_Pos      = 0;
    fNullable     = ಸುಳ್ಳು;
    f_Look_Ahead_ಕೊನೆ = ಸುಳ್ಳು;
    f_ನಿಯಮ_ಬೇರು     = ಸುಳ್ಳು;
    f_Chain_ಒಳ      = ಸುಳ್ಳು;
    fVal          = 0;
    fPrecedence   = precZero;

    f_ಮೊದಲ_Pos_ಗಣ  = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);
    f_ಕೊನೆಯ_Pos_ಗಣ   = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);
    fFollowPos    = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) &&
        (f_ಮೊದಲ_Pos_ಗಣ == ಶೂನ್ಯಸೂಚಕ || f_ಕೊನೆಯ_Pos_ಗಣ == ಶೂನ್ಯಸೂಚಕ || fFollowPos == ಶೂನ್ಯಸೂಚಕ)) {
        ಸ್ಥಿತಿ_2 =  U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ      (t==opCat)    {fPrecedence = precOpCat;}
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t==opOr)     {fPrecedence = precOpOr;}
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t==op_ಆರಂಭ)  {fPrecedence = prec_ಆರಂಭ;}
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ಸ್ಥಿರಾಂಕ RBBINode &other, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) : UMemory(other) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    f_Serial_ಅಂಕೆ   = ++g_ಕೊನೆಯ_Serial;
#ಆದರೆ_ಅಂತ್ಯ
    fType        = other.fType;
    f_ಪೋಷಕ      = ಶೂನ್ಯಸೂಚಕ;
    f_ಎಡ_ಮಗು   = ಶೂನ್ಯಸೂಚಕ;
    f_ಬಲ_ಮಗು  = ಶೂನ್ಯಸೂಚಕ;
    f_ಒಳಹರಿವು_ಗಣ    = other.f_ಒಳಹರಿವು_ಗಣ;
    fPrecedence  = other.fPrecedence;
    f_ಪಠ್ಯ        = other.f_ಪಠ್ಯ;
    f_ಮೊದಲ_Pos    = other.f_ಮೊದಲ_Pos;
    f_ಕೊನೆಯ_Pos     = other.f_ಕೊನೆಯ_Pos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_ನಿಯಮ_ಬೇರು    = ಸುಳ್ಳು;
    f_Chain_ಒಳ     = other.f_Chain_ಒಳ;
    f_ಮೊದಲ_Pos_ಗಣ = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);   // TODO - get a real status from somewhere
    f_ಕೊನೆಯ_Pos_ಗಣ  = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);
    fFollowPos   = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) &&
        (f_ಮೊದಲ_Pos_ಗಣ == ಶೂನ್ಯಸೂಚಕ || f_ಕೊನೆಯ_Pos_ಗಣ == ಶೂನ್ಯಸೂಚಕ || fFollowPos == ಶೂನ್ಯಸೂಚಕ)) {
        ಸ್ಥಿತಿ_2 =  U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
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
    ಅಳಿಸು f_ಒಳಹರಿವು_ಗಣ;
    f_ಒಳಹರಿವು_ಗಣ = ಶೂನ್ಯಸೂಚಕ;

    ಆಯ್ಕೆ (ಇದು->fType) {
    ಸ್ಥಿತಿ varRef:
    ಸ್ಥಿತಿ ಗಣ_Ref:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_ಗಂಟು(f_ಎಡ_ಮಗು);
        f_ಎಡ_ಮಗು =   ಶೂನ್ಯಸೂಚಕ;
        // delete        fRightChild;
        NRDelete_ಗಂಟು(f_ಬಲ_ಮಗು);
        f_ಬಲ_ಮಗು = ಶೂನ್ಯಸೂಚಕ;
    }

    ಅಳಿಸು f_ಮೊದಲ_Pos_ಗಣ;
    ಅಳಿಸು f_ಕೊನೆಯ_Pos_ಗಣ;
    ಅಳಿಸು fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
ಶೂನ್ಯ RBBINode::NRDelete_ಗಂಟು(RBBINode *ಗಂಟು) {
    ಆದರೆ (ಗಂಟು == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    RBBINode *stop_ಗಂಟು = ಗಂಟು->f_ಪೋಷಕ;
    RBBINode *ಮುಂದಿನ_ಗಂಟು = ಗಂಟು;
    ಆಗ (ಮುಂದಿನ_ಗಂಟು != stop_ಗಂಟು && ಮುಂದಿನ_ಗಂಟು != ಶೂನ್ಯಸೂಚಕ) {
        RBBINode *current_ಗಂಟು = ಮುಂದಿನ_ಗಂಟು;

        ಆದರೆ ((current_ಗಂಟು->f_ಎಡ_ಮಗು == ಶೂನ್ಯಸೂಚಕ && current_ಗಂಟು->f_ಬಲ_ಮಗು == ಶೂನ್ಯಸೂಚಕ) ||
                current_ಗಂಟು->fType == varRef ||      // varRef and setRef nodes do not
                current_ಗಂಟು->fType == ಗಣ_Ref) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            ಮುಂದಿನ_ಗಂಟು = current_ಗಂಟು->f_ಪೋಷಕ;
            ಆದರೆ (ಮುಂದಿನ_ಗಂಟು) {
                ಆದರೆ (ಮುಂದಿನ_ಗಂಟು->f_ಎಡ_ಮಗು == current_ಗಂಟು) {
                    ಮುಂದಿನ_ಗಂಟು->f_ಎಡ_ಮಗು = ಶೂನ್ಯಸೂಚಕ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಂದಿನ_ಗಂಟು->f_ಬಲ_ಮಗು == current_ಗಂಟು) {
                    ಮುಂದಿನ_ಗಂಟು->f_ಬಲ_ಮಗು = ಶೂನ್ಯಸೂಚಕ;
                }
            }
            ಅಳಿಸು current_ಗಂಟು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (current_ಗಂಟು->f_ಎಡ_ಮಗು) {
            ಮುಂದಿನ_ಗಂಟು = current_ಗಂಟು->f_ಎಡ_ಮಗು;
            ಆದರೆ (ಮುಂದಿನ_ಗಂಟು->f_ಪೋಷಕ == ಶೂನ್ಯಸೂಚಕ) {
                ಮುಂದಿನ_ಗಂಟು->f_ಪೋಷಕ = current_ಗಂಟು;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(ಮುಂದಿನ_ಗಂಟು->f_ಪೋಷಕ == current_ಗಂಟು);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (current_ಗಂಟು->f_ಬಲ_ಮಗು) {
            ಮುಂದಿನ_ಗಂಟು = current_ಗಂಟು->f_ಬಲ_ಮಗು;
            ಆದರೆ (ಮುಂದಿನ_ಗಂಟು->f_ಪೋಷಕ == ಶೂನ್ಯಸೂಚಕ) {
                ಮುಂದಿನ_ಗಂಟು->f_ಪೋಷಕ = current_ಗಂಟು;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(ಮುಂದಿನ_ಗಂಟು->f_ಪೋಷಕ == current_ಗಂಟು);
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
constexpr ಪೂರ್ಣಾಂಕ k_Recursive_ಆಳ_ಎಲ್ಲೆ = 3500;
RBBINode *RBBINode::clone_ವೃಕ್ಷ(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ ಆಳ) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ಆದರೆ (ಆಳ > k_Recursive_ಆಳ_ಎಲ್ಲೆ) {
        ಸ್ಥಿತಿ_2 = U_ಒಳಹರಿವು_TOO_LONG_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    RBBINode    *n;

    ಆದರೆ (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_ಎಡ_ಮಗು->clone_ವೃಕ್ಷ(ಸ್ಥಿತಿ_2, ಆಳ+1);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fType == RBBINode::uset) {
        n = ಇದು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        n = ಹೊಸ RBBINode(*ಇದು, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಅಳಿಸು n;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        // Check for null pointer.
        ಆದರೆ (n == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 =  U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಆದರೆ (f_ಎಡ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
            n->f_ಎಡ_ಮಗು          = f_ಎಡ_ಮಗು->clone_ವೃಕ್ಷ(ಸ್ಥಿತಿ_2, ಆಳ+1);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಅಳಿಸು n;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            n->f_ಎಡ_ಮಗು->f_ಪೋಷಕ = n;
        }
        ಆದರೆ (f_ಬಲ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
            n->f_ಬಲ_ಮಗು          = f_ಬಲ_ಮಗು->clone_ವೃಕ್ಷ(ಸ್ಥಿತಿ_2, ಆಳ+1);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಅಳಿಸು n;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            n->f_ಬಲ_ಮಗು->f_ಪೋಷಕ = n;
        }
    }
    ಹಿಂತಿರುಗಿ n;
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
RBBINode *RBBINode::flattenVariables(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ ಆಳ) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಇದು;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ಆದರೆ (ಆಳ > k_Recursive_ಆಳ_ಎಲ್ಲೆ) {
        ಸ್ಥಿತಿ_2 = U_ಒಳಹರಿವು_TOO_LONG_ERROR;
        ಹಿಂತಿರುಗಿ ಇದು;
    }
    ಆದರೆ (fType == varRef) {
        RBBINode *ret_ಗಂಟು  = f_ಎಡ_ಮಗು->clone_ವೃಕ್ಷ(ಸ್ಥಿತಿ_2, ಆಳ+1);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಇದು;
        }
        ret_ಗಂಟು->f_ನಿಯಮ_ಬೇರು = ಇದು->f_ನಿಯಮ_ಬೇರು;
        ret_ಗಂಟು->f_Chain_ಒಳ  = ಇದು->f_Chain_ಒಳ;
        ಅಳಿಸು ಇದು;   // TODO: undefined behavior. Fix.
        ಹಿಂತಿರುಗಿ ret_ಗಂಟು;
    }

    ಆದರೆ (f_ಎಡ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
        f_ಎಡ_ಮಗು = f_ಎಡ_ಮಗು->flattenVariables(ಸ್ಥಿತಿ_2, ಆಳ+1);
        ಆದರೆ (f_ಎಡ_ಮಗು == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಇದು;
        }
        f_ಎಡ_ಮಗು->f_ಪೋಷಕ  = ಇದು;
    }
    ಆದರೆ (f_ಬಲ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
        f_ಬಲ_ಮಗು = f_ಬಲ_ಮಗು->flattenVariables(ಸ್ಥಿತಿ_2, ಆಳ+1);
        ಆದರೆ (f_ಬಲ_ಮಗು == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಇದು;
        }
        f_ಬಲ_ಮಗು->f_ಪೋಷಕ = ಇದು;
    }
    ಹಿಂತಿರುಗಿ ಇದು;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
ಶೂನ್ಯ RBBINode::flattenSets(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2, ಪೂರ್ಣಾಂಕ ಆಳ) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ಆದರೆ (ಆಳ > k_Recursive_ಆಳ_ಎಲ್ಲೆ) {
        ಸ್ಥಿತಿ_2 = U_ಒಳಹರಿವು_TOO_LONG_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    U_ASSERT(fType != ಗಣ_Ref);

    ಆದರೆ (f_ಎಡ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (f_ಎಡ_ಮಗು->fType==ಗಣ_Ref) {
            RBBINode *ಗಣ_Ref_ಗಂಟು = f_ಎಡ_ಮಗು;
            RBBINode *uset_ಗಂಟು   = ಗಣ_Ref_ಗಂಟು->f_ಎಡ_ಮಗು;
            RBBINode *repl_ವೃಕ್ಷ   = uset_ಗಂಟು->f_ಎಡ_ಮಗು;
            f_ಎಡ_ಮಗು           = repl_ವೃಕ್ಷ->clone_ವೃಕ್ಷ(ಸ್ಥಿತಿ_2, ಆಳ+1);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಅಳಿಸು ಗಣ_Ref_ಗಂಟು;
                ಹಿಂತಿರುಗಿ;
            }
            f_ಎಡ_ಮಗು->f_ಪೋಷಕ  = ಇದು;
            ಅಳಿಸು ಗಣ_Ref_ಗಂಟು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            f_ಎಡ_ಮಗು->flattenSets(ಸ್ಥಿತಿ_2, ಆಳ+1);
        }
    }

    ಆದರೆ (f_ಬಲ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (f_ಬಲ_ಮಗು->fType==ಗಣ_Ref) {
            RBBINode *ಗಣ_Ref_ಗಂಟು = f_ಬಲ_ಮಗು;
            RBBINode *uset_ಗಂಟು   = ಗಣ_Ref_ಗಂಟು->f_ಎಡ_ಮಗು;
            RBBINode *repl_ವೃಕ್ಷ   = uset_ಗಂಟು->f_ಎಡ_ಮಗು;
            f_ಬಲ_ಮಗು           = repl_ವೃಕ್ಷ->clone_ವೃಕ್ಷ(ಸ್ಥಿತಿ_2, ಆಳ+1);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಅಳಿಸು ಗಣ_Ref_ಗಂಟು;
                ಹಿಂತಿರುಗಿ;
            }
            f_ಬಲ_ಮಗು->f_ಪೋಷಕ  = ಇದು;
            ಅಳಿಸು ಗಣ_Ref_ಗಂಟು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            f_ಬಲ_ಮಗು->flattenSets(ಸ್ಥಿತಿ_2, ಆಳ+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
ಶೂನ್ಯ   RBBINode::ಕಂಡುಹಿಡಿ_Nodes(UVector *dest, RBBINode::ಗಂಟು_Type ಬಗೆ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    /* test for buffer overflows */
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    U_ASSERT(!dest->hasDeleter());
    ಆದರೆ (fType == ಬಗೆ) {
        dest->ಕೂಡಿಸು_Element(ಇದು, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (f_ಎಡ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
        f_ಎಡ_ಮಗು->ಕಂಡುಹಿಡಿ_Nodes(dest, ಬಗೆ, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (f_ಬಲ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
        f_ಬಲ_ಮಗು->ಕಂಡುಹಿಡಿ_Nodes(dest, ಬಗೆ, ಸ್ಥಿತಿ_2);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG

ಸ್ಥಿರ int32_t serial(ಸ್ಥಿರಾಂಕ RBBINode *ಗಂಟು) {
    ಹಿಂತಿರುಗಿ (ಗಂಟು == ಶೂನ್ಯಸೂಚಕ? -1 : ಗಂಟು->f_Serial_ಅಂಕೆ);
}


ಶೂನ್ಯ RBBINode::print_ಗಂಟು(ಸ್ಥಿರಾಂಕ RBBINode *ಗಂಟು) {
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಗಂಟು_Type_ಹೆಸರುಗಳು[] = {
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

    ಆದರೆ (ಗಂಟು==ಶೂನ್ಯಸೂಚಕ) {
        RBBIDebugPrintf("%10p", (ಶೂನ್ಯ *)ಗಂಟು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (ಶೂನ್ಯ *)ಗಂಟು, ಗಂಟು->f_Serial_ಅಂಕೆ, ಗಂಟು_Type_ಹೆಸರುಗಳು[ಗಂಟು->fType],
            ಗಂಟು->f_ನಿಯಮ_ಬೇರು?'R':' ', ಗಂಟು->f_Chain_ಒಳ?'C':' ',
            serial(ಗಂಟು->f_ಎಡ_ಮಗು), serial(ಗಂಟು->f_ಬಲ_ಮಗು), serial(ಗಂಟು->f_ಪೋಷಕ),
            ಗಂಟು->f_ಮೊದಲ_Pos, ಗಂಟು->fVal);
        ಆದರೆ (ಗಂಟು->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(ಗಂಟು->f_ಪಠ್ಯ);
        }
    }
    RBBIDebugPrintf("\n");
}
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
U_CFUNC ಶೂನ್ಯ RBBI_DEBUG_printUnicodeString(ಸ್ಥಿರಾಂಕ UnicodeString &s, ಪೂರ್ಣಾಂಕ min_ಅಗಲ) {
    RBBIDebugPrintf("%*s", min_ಅಗಲ, CStr(s)());
}
#ಆದರೆ_ಅಂತ್ಯ


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBINode::print_ಗಂಟು_ಶೀರ್ಷಿಕೆ() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
ಶೂನ್ಯ RBBINode::print_ವೃಕ್ಷ(ಸ್ಥಿರಾಂಕ RBBINode *ಗಂಟು, UBool printHeading) {
    ಆದರೆ (printHeading) {
        print_ಗಂಟು_ಶೀರ್ಷಿಕೆ();
    }
    print_ಗಂಟು(ಗಂಟು);
    ಆದರೆ (ಗಂಟು != ಶೂನ್ಯಸೂಚಕ) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        ಆದರೆ (ಗಂಟು->fType != varRef) {
            ಆದರೆ (ಗಂಟು->f_ಎಡ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
                print_ವೃಕ್ಷ(ಗಂಟು->f_ಎಡ_ಮಗು, ಸುಳ್ಳು);
            }
            
            ಆದರೆ (ಗಂಟು->f_ಬಲ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
                print_ವೃಕ್ಷ(ಗಂಟು->f_ಬಲ_ಮಗು, ಸುಳ್ಳು);
            }
        }
    }
}
#ಆದರೆ_ಅಂತ್ಯ



U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
