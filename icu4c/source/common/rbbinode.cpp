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

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

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

#ifdef RBBI_DEBUG
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(ಗಂಟು_Type t, UErrorCode& status) : UMemory() {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
#ifdef RBBI_DEBUG
    f_Serial_ಅಂಕೆ    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_ಎಡ_Child    = nullptr;
    f_ಬಲ_Child   = nullptr;
    f_ಒಳಹರಿವು_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = ಅಸತ್ಯ;
    fLookAheadEnd = ಅಸತ್ಯ;
    f_Rule_ಬೇರು     = ಅಸತ್ಯ;
    fChainIn      = ಅಸತ್ಯ;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = ನೂತನ UVector(status);
    fLastPosSet   = ನೂತನ UVector(status);
    fFollowPos    = ನೂತನ UVector(status);
    ಯದಿ (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ      (t==opCat)    {fPrecedence = precOpCat;}
    ಅನ್ಯಥಾ ಯದಿ (t==opOr)     {fPrecedence = precOpOr;}
    ಅನ್ಯಥಾ ಯದಿ (t==opStart)  {fPrecedence = precStart;}
    ಅನ್ಯಥಾ ಯದಿ (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ಧ್ರುವ RBBINode &other, UErrorCode& status) : UMemory(other) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
#ifdef RBBI_DEBUG
    f_Serial_ಅಂಕೆ   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_ಎಡ_Child   = nullptr;
    f_ಬಲ_Child  = nullptr;
    f_ಒಳಹರಿವು_Set    = other.f_ಒಳಹರಿವು_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_ಬೇರು    = ಅಸತ್ಯ;
    fChainIn     = other.fChainIn;
    fFirstPosSet = ನೂತನ UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = ನೂತನ UVector(status);
    fFollowPos   = ನೂತನ UVector(status);
    ಯದಿ (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
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
    delete f_ಒಳಹರಿವು_Set;
    f_ಒಳಹರಿವು_Set = nullptr;

    ವಿಕಲ್ಪ (ಏತತ್->fType) {
    ಪಕ್ಷ varRef:
    ಪಕ್ಷ setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        ಭಙ್ಗ;

    ಸ್ವಭಾವ:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_ಗಂಟು(f_ಎಡ_Child);
        f_ಎಡ_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_ಗಂಟು(f_ಬಲ_Child);
        f_ಬಲ_Child = nullptr;
    }

    delete fFirstPosSet;
    delete fLastPosSet;
    delete fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
ಶೂನ್ಯ RBBINode::NRDelete_ಗಂಟು(RBBINode *ಗಂಟು) {
    ಯದಿ (ಗಂಟು == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    RBBINode *stop_ಗಂಟು = ಗಂಟು->fParent;
    RBBINode *next_ಗಂಟು = ಗಂಟು;
    ಯಾವತ್ (next_ಗಂಟು != stop_ಗಂಟು && next_ಗಂಟು != nullptr) {
        RBBINode *current_ಗಂಟು = next_ಗಂಟು;

        ಯದಿ ((current_ಗಂಟು->f_ಎಡ_Child == nullptr && current_ಗಂಟು->f_ಬಲ_Child == nullptr) ||
                current_ಗಂಟು->fType == varRef ||      // varRef and setRef nodes do not
                current_ಗಂಟು->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_ಗಂಟು = current_ಗಂಟು->fParent;
            ಯದಿ (next_ಗಂಟು) {
                ಯದಿ (next_ಗಂಟು->f_ಎಡ_Child == current_ಗಂಟು) {
                    next_ಗಂಟು->f_ಎಡ_Child = nullptr;
                } ಅನ್ಯಥಾ ಯದಿ (next_ಗಂಟು->f_ಬಲ_Child == current_ಗಂಟು) {
                    next_ಗಂಟು->f_ಬಲ_Child = nullptr;
                }
            }
            delete current_ಗಂಟು;
        } ಅನ್ಯಥಾ ಯದಿ (current_ಗಂಟು->f_ಎಡ_Child) {
            next_ಗಂಟು = current_ಗಂಟು->f_ಎಡ_Child;
            ಯದಿ (next_ಗಂಟು->fParent == nullptr) {
                next_ಗಂಟು->fParent = current_ಗಂಟು;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_ಗಂಟು->fParent == current_ಗಂಟು);
        } ಅನ್ಯಥಾ ಯದಿ (current_ಗಂಟು->f_ಬಲ_Child) {
            next_ಗಂಟು = current_ಗಂಟು->f_ಬಲ_Child;
            ಯದಿ (next_ಗಂಟು->fParent == nullptr) {
                next_ಗಂಟು->fParent = current_ಗಂಟು;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_ಗಂಟು->fParent == current_ಗಂಟು);
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
constexpr ಪೂರ್ಣಾಙ್ಕ kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_ವೃಕ್ಷ(UErrorCode &status, ಪೂರ್ಣಾಙ್ಕ depth) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ಯದಿ (depth > kRecursiveDepthLimit) {
        status = U_ಒಳಹರಿವು_TOO_LONG_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    RBBINode    *n;

    ಯದಿ (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_ಎಡ_Child->clone_ವೃಕ್ಷ(status, depth+1);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
    } ಅನ್ಯಥಾ ಯದಿ (fType == RBBINode::uset) {
        n = ಏತತ್;
    } ಅನ್ಯಥಾ {
        n = ನೂತನ RBBINode(*ಏತತ್, status);
        ಯದಿ (U_FAILURE(status)) {
            delete n;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        // Check for null pointer.
        ಯದಿ (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಯದಿ (f_ಎಡ_Child != nullptr) {
            n->f_ಎಡ_Child          = f_ಎಡ_Child->clone_ವೃಕ್ಷ(status, depth+1);
            ಯದಿ (U_FAILURE(status)) {
                delete n;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            n->f_ಎಡ_Child->fParent = n;
        }
        ಯದಿ (f_ಬಲ_Child != nullptr) {
            n->f_ಬಲ_Child          = f_ಬಲ_Child->clone_ವೃಕ್ಷ(status, depth+1);
            ಯದಿ (U_FAILURE(status)) {
                delete n;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            n->f_ಬಲ_Child->fParent = n;
        }
    }
    ಪ್ರತ್ಯಾಗಮ n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, ಪೂರ್ಣಾಙ್ಕ depth) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಏತತ್;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ಯದಿ (depth > kRecursiveDepthLimit) {
        status = U_ಒಳಹರಿವು_TOO_LONG_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಏತತ್;
    }
    ಯದಿ (fType == varRef) {
        RBBINode *ret_ಗಂಟು  = f_ಎಡ_Child->clone_ವೃಕ್ಷ(status, depth+1);
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ ಏತತ್;
        }
        ret_ಗಂಟು->f_Rule_ಬೇರು = ಏತತ್->f_Rule_ಬೇರು;
        ret_ಗಂಟು->fChainIn  = ಏತತ್->fChainIn;
        delete ಏತತ್;   // TODO: undefined behavior. Fix.
        ಪ್ರತ್ಯಾಗಮ ret_ಗಂಟು;
    }

    ಯದಿ (f_ಎಡ_Child != nullptr) {
        f_ಎಡ_Child = f_ಎಡ_Child->flattenVariables(status, depth+1);
        ಯದಿ (f_ಎಡ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ ಏತತ್;
        }
        f_ಎಡ_Child->fParent  = ಏತತ್;
    }
    ಯದಿ (f_ಬಲ_Child != nullptr) {
        f_ಬಲ_Child = f_ಬಲ_Child->flattenVariables(status, depth+1);
        ಯದಿ (f_ಬಲ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ಯದಿ (U_FAILURE(status)) {
            ಪ್ರತ್ಯಾಗಮ ಏತತ್;
        }
        f_ಬಲ_Child->fParent = ಏತತ್;
    }
    ಪ್ರತ್ಯಾಗಮ ಏತತ್;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
ಶೂನ್ಯ RBBINode::flattenSets(UErrorCode &status, ಪೂರ್ಣಾಙ್ಕ depth) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ಯದಿ (depth > kRecursiveDepthLimit) {
        status = U_ಒಳಹರಿವು_TOO_LONG_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    U_ASSERT(fType != setRef);

    ಯದಿ (f_ಎಡ_Child != nullptr) {
        ಯದಿ (f_ಎಡ_Child->fType==setRef) {
            RBBINode *set_Ref_ಗಂಟು = f_ಎಡ_Child;
            RBBINode *uset_ಗಂಟು   = set_Ref_ಗಂಟು->f_ಎಡ_Child;
            RBBINode *repl_ವೃಕ್ಷ   = uset_ಗಂಟು->f_ಎಡ_Child;
            f_ಎಡ_Child           = repl_ವೃಕ್ಷ->clone_ವೃಕ್ಷ(status, depth+1);
            ಯದಿ (U_FAILURE(status)) {
                delete set_Ref_ಗಂಟು;
                ಪ್ರತ್ಯಾಗಮ;
            }
            f_ಎಡ_Child->fParent  = ಏತತ್;
            delete set_Ref_ಗಂಟು;
        } ಅನ್ಯಥಾ {
            f_ಎಡ_Child->flattenSets(status, depth+1);
        }
    }

    ಯದಿ (f_ಬಲ_Child != nullptr) {
        ಯದಿ (f_ಬಲ_Child->fType==setRef) {
            RBBINode *set_Ref_ಗಂಟು = f_ಬಲ_Child;
            RBBINode *uset_ಗಂಟು   = set_Ref_ಗಂಟು->f_ಎಡ_Child;
            RBBINode *repl_ವೃಕ್ಷ   = uset_ಗಂಟು->f_ಎಡ_Child;
            f_ಬಲ_Child           = repl_ವೃಕ್ಷ->clone_ವೃಕ್ಷ(status, depth+1);
            ಯದಿ (U_FAILURE(status)) {
                delete set_Ref_ಗಂಟು;
                ಪ್ರತ್ಯಾಗಮ;
            }
            f_ಬಲ_Child->fParent  = ಏತತ್;
            delete set_Ref_ಗಂಟು;
        } ಅನ್ಯಥಾ {
            f_ಬಲ_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
ಶೂನ್ಯ   RBBINode::findNodes(UVector *dest, RBBINode::ಗಂಟು_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    U_ASSERT(!dest->hasDeleter());
    ಯದಿ (fType == kind) {
        dest->addElement(ಏತತ್, status);
    }
    ಯದಿ (f_ಎಡ_Child != nullptr) {
        f_ಎಡ_Child->findNodes(dest, kind, status);
    }
    ಯದಿ (f_ಬಲ_Child != nullptr) {
        f_ಬಲ_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

ಸ್ಥಿರ int32_t serial(ಧ್ರುವ RBBINode *ಗಂಟು) {
    ಪ್ರತ್ಯಾಗಮ (ಗಂಟು == nullptr? -1 : ಗಂಟು->f_Serial_ಅಂಕೆ);
}


ಶೂನ್ಯ RBBINode::print_ಗಂಟು(ಧ್ರುವ RBBINode *ಗಂಟು) {
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ಗಂಟು_Type_Names[] = {
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

    ಯದಿ (ಗಂಟು==nullptr) {
        RBBIDebugPrintf("%10p", (ಶೂನ್ಯ *)ಗಂಟು);
    } ಅನ್ಯಥಾ {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (ಶೂನ್ಯ *)ಗಂಟು, ಗಂಟು->f_Serial_ಅಂಕೆ, ಗಂಟು_Type_Names[ಗಂಟು->fType],
            ಗಂಟು->f_Rule_ಬೇರು?'R':' ', ಗಂಟು->fChainIn?'C':' ',
            serial(ಗಂಟು->f_ಎಡ_Child), serial(ಗಂಟು->f_ಬಲ_Child), serial(ಗಂಟು->fParent),
            ಗಂಟು->fFirstPos, ಗಂಟು->fVal);
        ಯದಿ (ಗಂಟು->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(ಗಂಟು->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC ಶೂನ್ಯ RBBI_DEBUG_printUnicodeString(ಧ್ರುವ UnicodeString &s, ಪೂರ್ಣಾಙ್ಕ minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ಶೂನ್ಯ RBBINode::print_ಗಂಟು_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
ಶೂನ್ಯ RBBINode::print_ವೃಕ್ಷ(ಧ್ರುವ RBBINode *ಗಂಟು, UBool printHeading) {
    ಯದಿ (printHeading) {
        print_ಗಂಟು_Header();
    }
    print_ಗಂಟು(ಗಂಟು);
    ಯದಿ (ಗಂಟು != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        ಯದಿ (ಗಂಟು->fType != varRef) {
            ಯದಿ (ಗಂಟು->f_ಎಡ_Child != nullptr) {
                print_ವೃಕ್ಷ(ಗಂಟು->f_ಎಡ_Child, ಅಸತ್ಯ);
            }
            
            ಯದಿ (ಗಂಟು->f_ಬಲ_Child != nullptr) {
                print_ವೃಕ್ಷ(ಗಂಟು->f_ಬಲ_Child, ಅಸತ್ಯ);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
