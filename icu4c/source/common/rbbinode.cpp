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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parsepos.h"

#ଅନ୍ତର୍ଭାଵ "cstr.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

#ଅନ୍ତର୍ଭାଵ "rbbirb.h"
#ଅନ୍ତର୍ଭାଵ "rbbinode.h"

#ଅନ୍ତର୍ଭାଵ "uassert.h"


U_NAMESPACE_BEGIN

#ifdef RBBI_DEBUG
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(ଗଣ୍ଠି_Type t, UErrorCode& status) : UMemory() {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
#ifdef RBBI_DEBUG
    f_Serial_ଅଙ୍କ    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_ବାମ_Child    = nullptr;
    f_ଡାହାଣ_Child   = nullptr;
    f_ନିବେଶ_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = ମିଥ୍ୟା;
    fLookAheadEnd = ମିଥ୍ୟା;
    f_Rule_ମୂଳ     = ମିଥ୍ୟା;
    fChainIn      = ମିଥ୍ୟା;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = ନୂତନ UVector(status);
    fLastPosSet   = ନୂତନ UVector(status);
    fFollowPos    = ନୂତନ UVector(status);
    ଯଦି (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    ଯଦି      (t==opCat)    {fPrecedence = precOpCat;}
    ଅନ୍ୟଥା ଯଦି (t==opOr)     {fPrecedence = precOpOr;}
    ଅନ୍ୟଥା ଯଦି (t==opStart)  {fPrecedence = precStart;}
    ଅନ୍ୟଥା ଯଦି (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ଅଚଳ RBBINode &other, UErrorCode& status) : UMemory(other) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
#ifdef RBBI_DEBUG
    f_Serial_ଅଙ୍କ   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_ବାମ_Child   = nullptr;
    f_ଡାହାଣ_Child  = nullptr;
    f_ନିବେଶ_Set    = other.f_ନିବେଶ_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_ମୂଳ    = ମିଥ୍ୟା;
    fChainIn     = other.fChainIn;
    fFirstPosSet = ନୂତନ UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = ନୂତନ UVector(status);
    fFollowPos   = ନୂତନ UVector(status);
    ଯଦି (U_SUCCESS(status) &&
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
    delete f_ନିବେଶ_Set;
    f_ନିବେଶ_Set = nullptr;

    ବାଛ (ଏହି->fType) {
    ସ୍ଥିତି varRef:
    ସ୍ଥିତି setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_ଗଣ୍ଠି(f_ବାମ_Child);
        f_ବାମ_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_ଗଣ୍ଠି(f_ଡାହାଣ_Child);
        f_ଡାହାଣ_Child = nullptr;
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
ଶୂନ୍ୟ RBBINode::NRDelete_ଗଣ୍ଠି(RBBINode *ଗଣ୍ଠି) {
    ଯଦି (ଗଣ୍ଠି == nullptr) {
        ଫେରନ୍ତୁ;
    }

    RBBINode *stop_ଗଣ୍ଠି = ଗଣ୍ଠି->fParent;
    RBBINode *next_ଗଣ୍ଠି = ଗଣ୍ଠି;
    ଯେତେବେଳେ (next_ଗଣ୍ଠି != stop_ଗଣ୍ଠି && next_ଗଣ୍ଠି != nullptr) {
        RBBINode *current_ଗଣ୍ଠି = next_ଗଣ୍ଠି;

        ଯଦି ((current_ଗଣ୍ଠି->f_ବାମ_Child == nullptr && current_ଗଣ୍ଠି->f_ଡାହାଣ_Child == nullptr) ||
                current_ଗଣ୍ଠି->fType == varRef ||      // varRef and setRef nodes do not
                current_ଗଣ୍ଠି->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_ଗଣ୍ଠି = current_ଗଣ୍ଠି->fParent;
            ଯଦି (next_ଗଣ୍ଠି) {
                ଯଦି (next_ଗଣ୍ଠି->f_ବାମ_Child == current_ଗଣ୍ଠି) {
                    next_ଗଣ୍ଠି->f_ବାମ_Child = nullptr;
                } ଅନ୍ୟଥା ଯଦି (next_ଗଣ୍ଠି->f_ଡାହାଣ_Child == current_ଗଣ୍ଠି) {
                    next_ଗଣ୍ଠି->f_ଡାହାଣ_Child = nullptr;
                }
            }
            delete current_ଗଣ୍ଠି;
        } ଅନ୍ୟଥା ଯଦି (current_ଗଣ୍ଠି->f_ବାମ_Child) {
            next_ଗଣ୍ଠି = current_ଗଣ୍ଠି->f_ବାମ_Child;
            ଯଦି (next_ଗଣ୍ଠି->fParent == nullptr) {
                next_ଗଣ୍ଠି->fParent = current_ଗଣ୍ଠି;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_ଗଣ୍ଠି->fParent == current_ଗଣ୍ଠି);
        } ଅନ୍ୟଥା ଯଦି (current_ଗଣ୍ଠି->f_ଡାହାଣ_Child) {
            next_ଗଣ୍ଠି = current_ଗଣ୍ଠି->f_ଡାହାଣ_Child;
            ଯଦି (next_ଗଣ୍ଠି->fParent == nullptr) {
                next_ଗଣ୍ଠି->fParent = current_ଗଣ୍ଠି;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_ଗଣ୍ଠି->fParent == current_ଗଣ୍ଠି);
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
constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_ବୃକ୍ଷ(UErrorCode &status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ଯଦି (depth > kRecursiveDepthLimit) {
        status = U_ନିବେଶ_TOO_LONG_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    RBBINode    *n;

    ଯଦି (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_ବାମ_Child->clone_ବୃକ୍ଷ(status, depth+1);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ nullptr;
        }
    } ଅନ୍ୟଥା ଯଦି (fType == RBBINode::uset) {
        n = ଏହି;
    } ଅନ୍ୟଥା {
        n = ନୂତନ RBBINode(*ଏହି, status);
        ଯଦି (U_FAILURE(status)) {
            delete n;
            ଫେରନ୍ତୁ nullptr;
        }
        // Check for null pointer.
        ଯଦି (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        ଯଦି (f_ବାମ_Child != nullptr) {
            n->f_ବାମ_Child          = f_ବାମ_Child->clone_ବୃକ୍ଷ(status, depth+1);
            ଯଦି (U_FAILURE(status)) {
                delete n;
                ଫେରନ୍ତୁ nullptr;
            }
            n->f_ବାମ_Child->fParent = n;
        }
        ଯଦି (f_ଡାହାଣ_Child != nullptr) {
            n->f_ଡାହାଣ_Child          = f_ଡାହାଣ_Child->clone_ବୃକ୍ଷ(status, depth+1);
            ଯଦି (U_FAILURE(status)) {
                delete n;
                ଫେରନ୍ତୁ nullptr;
            }
            n->f_ଡାହାଣ_Child->fParent = n;
        }
    }
    ଫେରନ୍ତୁ n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ଏହି;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ଯଦି (depth > kRecursiveDepthLimit) {
        status = U_ନିବେଶ_TOO_LONG_ERROR;
        ଫେରନ୍ତୁ ଏହି;
    }
    ଯଦି (fType == varRef) {
        RBBINode *ret_ଗଣ୍ଠି  = f_ବାମ_Child->clone_ବୃକ୍ଷ(status, depth+1);
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ ଏହି;
        }
        ret_ଗଣ୍ଠି->f_Rule_ମୂଳ = ଏହି->f_Rule_ମୂଳ;
        ret_ଗଣ୍ଠି->fChainIn  = ଏହି->fChainIn;
        delete ଏହି;   // TODO: undefined behavior. Fix.
        ଫେରନ୍ତୁ ret_ଗଣ୍ଠି;
    }

    ଯଦି (f_ବାମ_Child != nullptr) {
        f_ବାମ_Child = f_ବାମ_Child->flattenVariables(status, depth+1);
        ଯଦି (f_ବାମ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ ଏହି;
        }
        f_ବାମ_Child->fParent  = ଏହି;
    }
    ଯଦି (f_ଡାହାଣ_Child != nullptr) {
        f_ଡାହାଣ_Child = f_ଡାହାଣ_Child->flattenVariables(status, depth+1);
        ଯଦି (f_ଡାହାଣ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ଯଦି (U_FAILURE(status)) {
            ଫେରନ୍ତୁ ଏହି;
        }
        f_ଡାହାଣ_Child->fParent = ଏହି;
    }
    ଫେରନ୍ତୁ ଏହି;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
ଶୂନ୍ୟ RBBINode::flattenSets(UErrorCode &status, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ଯଦି (depth > kRecursiveDepthLimit) {
        status = U_ନିବେଶ_TOO_LONG_ERROR;
        ଫେରନ୍ତୁ;
    }
    U_ASSERT(fType != setRef);

    ଯଦି (f_ବାମ_Child != nullptr) {
        ଯଦି (f_ବାମ_Child->fType==setRef) {
            RBBINode *set_Ref_ଗଣ୍ଠି = f_ବାମ_Child;
            RBBINode *uset_ଗଣ୍ଠି   = set_Ref_ଗଣ୍ଠି->f_ବାମ_Child;
            RBBINode *repl_ବୃକ୍ଷ   = uset_ଗଣ୍ଠି->f_ବାମ_Child;
            f_ବାମ_Child           = repl_ବୃକ୍ଷ->clone_ବୃକ୍ଷ(status, depth+1);
            ଯଦି (U_FAILURE(status)) {
                delete set_Ref_ଗଣ୍ଠି;
                ଫେରନ୍ତୁ;
            }
            f_ବାମ_Child->fParent  = ଏହି;
            delete set_Ref_ଗଣ୍ଠି;
        } ଅନ୍ୟଥା {
            f_ବାମ_Child->flattenSets(status, depth+1);
        }
    }

    ଯଦି (f_ଡାହାଣ_Child != nullptr) {
        ଯଦି (f_ଡାହାଣ_Child->fType==setRef) {
            RBBINode *set_Ref_ଗଣ୍ଠି = f_ଡାହାଣ_Child;
            RBBINode *uset_ଗଣ୍ଠି   = set_Ref_ଗଣ୍ଠି->f_ବାମ_Child;
            RBBINode *repl_ବୃକ୍ଷ   = uset_ଗଣ୍ଠି->f_ବାମ_Child;
            f_ଡାହାଣ_Child           = repl_ବୃକ୍ଷ->clone_ବୃକ୍ଷ(status, depth+1);
            ଯଦି (U_FAILURE(status)) {
                delete set_Ref_ଗଣ୍ଠି;
                ଫେରନ୍ତୁ;
            }
            f_ଡାହାଣ_Child->fParent  = ଏହି;
            delete set_Ref_ଗଣ୍ଠି;
        } ଅନ୍ୟଥା {
            f_ଡାହାଣ_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
ଶୂନ୍ୟ   RBBINode::findNodes(UVector *dest, RBBINode::ଗଣ୍ଠି_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    U_ASSERT(!dest->hasDeleter());
    ଯଦି (fType == kind) {
        dest->addElement(ଏହି, status);
    }
    ଯଦି (f_ବାମ_Child != nullptr) {
        f_ବାମ_Child->findNodes(dest, kind, status);
    }
    ଯଦି (f_ଡାହାଣ_Child != nullptr) {
        f_ଡାହାଣ_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

ସ୍ଥିର int32_t serial(ଅଚଳ RBBINode *ଗଣ୍ଠି) {
    ଫେରନ୍ତୁ (ଗଣ୍ଠି == nullptr? -1 : ଗଣ୍ଠି->f_Serial_ଅଙ୍କ);
}


ଶୂନ୍ୟ RBBINode::print_ଗଣ୍ଠି(ଅଚଳ RBBINode *ଗଣ୍ଠି) {
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ ଗଣ୍ଠି_Type_Names[] = {
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

    ଯଦି (ଗଣ୍ଠି==nullptr) {
        RBBIDebugPrintf("%10p", (ଶୂନ୍ୟ *)ଗଣ୍ଠି);
    } ଅନ୍ୟଥା {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (ଶୂନ୍ୟ *)ଗଣ୍ଠି, ଗଣ୍ଠି->f_Serial_ଅଙ୍କ, ଗଣ୍ଠି_Type_Names[ଗଣ୍ଠି->fType],
            ଗଣ୍ଠି->f_Rule_ମୂଳ?'R':' ', ଗଣ୍ଠି->fChainIn?'C':' ',
            serial(ଗଣ୍ଠି->f_ବାମ_Child), serial(ଗଣ୍ଠି->f_ଡାହାଣ_Child), serial(ଗଣ୍ଠି->fParent),
            ଗଣ୍ଠି->fFirstPos, ଗଣ୍ଠି->fVal);
        ଯଦି (ଗଣ୍ଠି->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(ଗଣ୍ଠି->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC ଶୂନ୍ୟ RBBI_DEBUG_printUnicodeString(ଅଚଳ UnicodeString &s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ଶୂନ୍ୟ RBBINode::print_ଗଣ୍ଠି_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
ଶୂନ୍ୟ RBBINode::print_ବୃକ୍ଷ(ଅଚଳ RBBINode *ଗଣ୍ଠି, UBool printHeading) {
    ଯଦି (printHeading) {
        print_ଗଣ୍ଠି_Header();
    }
    print_ଗଣ୍ଠି(ଗଣ୍ଠି);
    ଯଦି (ଗଣ୍ଠି != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        ଯଦି (ଗଣ୍ଠି->fType != varRef) {
            ଯଦି (ଗଣ୍ଠି->f_ବାମ_Child != nullptr) {
                print_ବୃକ୍ଷ(ଗଣ୍ଠି->f_ବାମ_Child, ମିଥ୍ୟା);
            }
            
            ଯଦି (ଗଣ୍ଠି->f_ଡାହାଣ_Child != nullptr) {
                print_ବୃକ୍ଷ(ଗଣ୍ଠି->f_ଡାହାଣ_Child, ମିଥ୍ୟା);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
