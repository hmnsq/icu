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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "unicode/uniset.h"
#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/parsepos.h"

#অন্তর্ভাৱ "cstr.h"
#অন্তর্ভাৱ "uvector.h"

#অন্তর্ভাৱ "rbbirb.h"
#অন্তর্ভাৱ "rbbinode.h"

#অন্তর্ভাৱ "uassert.h"


U_NAMESPACE_BEGIN

#ifdef RBBI_DEBUG
স্থির পূর্ণাঙ্ক  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(গ্ৰন্থি_Type t, UErrorCode& status) : UMemory() {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
#ifdef RBBI_DEBUG
    f_Serial_অঙ্ক    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_বাম_Child    = nullptr;
    f_ডান_Child   = nullptr;
    f_প্ৰবেশ_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = মিছা;
    fLookAheadEnd = মিছা;
    f_Rule_মূল     = মিছা;
    fChainIn      = মিছা;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = নূতন UVector(status);
    fLastPosSet   = নূতন UVector(status);
    fFollowPos    = নূতন UVector(status);
    যদি (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    যদি      (t==opCat)    {fPrecedence = precOpCat;}
    অন্যথা যদি (t==opOr)     {fPrecedence = precOpOr;}
    অন্যথা যদি (t==opStart)  {fPrecedence = precStart;}
    অন্যথা যদি (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ধ্রুৱ RBBINode &other, UErrorCode& status) : UMemory(other) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
#ifdef RBBI_DEBUG
    f_Serial_অঙ্ক   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_বাম_Child   = nullptr;
    f_ডান_Child  = nullptr;
    f_প্ৰবেশ_Set    = other.f_প্ৰবেশ_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_মূল    = মিছা;
    fChainIn     = other.fChainIn;
    fFirstPosSet = নূতন UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = নূতন UVector(status);
    fFollowPos   = নূতন UVector(status);
    যদি (U_SUCCESS(status) &&
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
    delete f_প্ৰবেশ_Set;
    f_প্ৰবেশ_Set = nullptr;

    ৱিকল্প (এতত্->fType) {
    পক্ষ varRef:
    পক্ষ setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        ভঙ্গ;

    স্ৱভাৱ:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_গ্ৰন্থি(f_বাম_Child);
        f_বাম_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_গ্ৰন্থি(f_ডান_Child);
        f_ডান_Child = nullptr;
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
শূন্য RBBINode::NRDelete_গ্ৰন্থি(RBBINode *গ্ৰন্থি) {
    যদি (গ্ৰন্থি == nullptr) {
        ঘূৰাই;
    }

    RBBINode *stop_গ্ৰন্থি = গ্ৰন্থি->fParent;
    RBBINode *next_গ্ৰন্থি = গ্ৰন্থি;
    যেতিয়ালৈ (next_গ্ৰন্থি != stop_গ্ৰন্থি && next_গ্ৰন্থি != nullptr) {
        RBBINode *current_গ্ৰন্থি = next_গ্ৰন্থি;

        যদি ((current_গ্ৰন্থি->f_বাম_Child == nullptr && current_গ্ৰন্থি->f_ডান_Child == nullptr) ||
                current_গ্ৰন্থি->fType == varRef ||      // varRef and setRef nodes do not
                current_গ্ৰন্থি->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_গ্ৰন্থি = current_গ্ৰন্থি->fParent;
            যদি (next_গ্ৰন্থি) {
                যদি (next_গ্ৰন্থি->f_বাম_Child == current_গ্ৰন্থি) {
                    next_গ্ৰন্থি->f_বাম_Child = nullptr;
                } অন্যথা যদি (next_গ্ৰন্থি->f_ডান_Child == current_গ্ৰন্থি) {
                    next_গ্ৰন্থি->f_ডান_Child = nullptr;
                }
            }
            delete current_গ্ৰন্থি;
        } অন্যথা যদি (current_গ্ৰন্থি->f_বাম_Child) {
            next_গ্ৰন্থি = current_গ্ৰন্থি->f_বাম_Child;
            যদি (next_গ্ৰন্থি->fParent == nullptr) {
                next_গ্ৰন্থি->fParent = current_গ্ৰন্থি;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_গ্ৰন্থি->fParent == current_গ্ৰন্থি);
        } অন্যথা যদি (current_গ্ৰন্থি->f_ডান_Child) {
            next_গ্ৰন্থি = current_গ্ৰন্থি->f_ডান_Child;
            যদি (next_গ্ৰন্থি->fParent == nullptr) {
                next_গ্ৰন্থি->fParent = current_গ্ৰন্থি;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_গ্ৰন্থি->fParent == current_গ্ৰন্থি);
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
constexpr পূর্ণাঙ্ক kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_বৃক্ষ(UErrorCode &status, পূর্ণাঙ্ক depth) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    যদি (depth > kRecursiveDepthLimit) {
        status = U_প্ৰবেশ_TOO_LONG_ERROR;
        ঘূৰাই nullptr;
    }
    RBBINode    *n;

    যদি (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_বাম_Child->clone_বৃক্ষ(status, depth+1);
        যদি (U_FAILURE(status)) {
            ঘূৰাই nullptr;
        }
    } অন্যথা যদি (fType == RBBINode::uset) {
        n = এতত্;
    } অন্যথা {
        n = নূতন RBBINode(*এতত্, status);
        যদি (U_FAILURE(status)) {
            delete n;
            ঘূৰাই nullptr;
        }
        // Check for null pointer.
        যদি (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই nullptr;
        }
        যদি (f_বাম_Child != nullptr) {
            n->f_বাম_Child          = f_বাম_Child->clone_বৃক্ষ(status, depth+1);
            যদি (U_FAILURE(status)) {
                delete n;
                ঘূৰাই nullptr;
            }
            n->f_বাম_Child->fParent = n;
        }
        যদি (f_ডান_Child != nullptr) {
            n->f_ডান_Child          = f_ডান_Child->clone_বৃক্ষ(status, depth+1);
            যদি (U_FAILURE(status)) {
                delete n;
                ঘূৰাই nullptr;
            }
            n->f_ডান_Child->fParent = n;
        }
    }
    ঘূৰাই n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, পূর্ণাঙ্ক depth) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই এতত্;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    যদি (depth > kRecursiveDepthLimit) {
        status = U_প্ৰবেশ_TOO_LONG_ERROR;
        ঘূৰাই এতত্;
    }
    যদি (fType == varRef) {
        RBBINode *ret_গ্ৰন্থি  = f_বাম_Child->clone_বৃক্ষ(status, depth+1);
        যদি (U_FAILURE(status)) {
            ঘূৰাই এতত্;
        }
        ret_গ্ৰন্থি->f_Rule_মূল = এতত্->f_Rule_মূল;
        ret_গ্ৰন্থি->fChainIn  = এতত্->fChainIn;
        delete এতত্;   // TODO: undefined behavior. Fix.
        ঘূৰাই ret_গ্ৰন্থি;
    }

    যদি (f_বাম_Child != nullptr) {
        f_বাম_Child = f_বাম_Child->flattenVariables(status, depth+1);
        যদি (f_বাম_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        যদি (U_FAILURE(status)) {
            ঘূৰাই এতত্;
        }
        f_বাম_Child->fParent  = এতত্;
    }
    যদি (f_ডান_Child != nullptr) {
        f_ডান_Child = f_ডান_Child->flattenVariables(status, depth+1);
        যদি (f_ডান_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        যদি (U_FAILURE(status)) {
            ঘূৰাই এতত্;
        }
        f_ডান_Child->fParent = এতত্;
    }
    ঘূৰাই এতত্;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
শূন্য RBBINode::flattenSets(UErrorCode &status, পূর্ণাঙ্ক depth) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    যদি (depth > kRecursiveDepthLimit) {
        status = U_প্ৰবেশ_TOO_LONG_ERROR;
        ঘূৰাই;
    }
    U_ASSERT(fType != setRef);

    যদি (f_বাম_Child != nullptr) {
        যদি (f_বাম_Child->fType==setRef) {
            RBBINode *set_Ref_গ্ৰন্থি = f_বাম_Child;
            RBBINode *uset_গ্ৰন্থি   = set_Ref_গ্ৰন্থি->f_বাম_Child;
            RBBINode *repl_বৃক্ষ   = uset_গ্ৰন্থি->f_বাম_Child;
            f_বাম_Child           = repl_বৃক্ষ->clone_বৃক্ষ(status, depth+1);
            যদি (U_FAILURE(status)) {
                delete set_Ref_গ্ৰন্থি;
                ঘূৰাই;
            }
            f_বাম_Child->fParent  = এতত্;
            delete set_Ref_গ্ৰন্থি;
        } অন্যথা {
            f_বাম_Child->flattenSets(status, depth+1);
        }
    }

    যদি (f_ডান_Child != nullptr) {
        যদি (f_ডান_Child->fType==setRef) {
            RBBINode *set_Ref_গ্ৰন্থি = f_ডান_Child;
            RBBINode *uset_গ্ৰন্থি   = set_Ref_গ্ৰন্থি->f_বাম_Child;
            RBBINode *repl_বৃক্ষ   = uset_গ্ৰন্থি->f_বাম_Child;
            f_ডান_Child           = repl_বৃক্ষ->clone_বৃক্ষ(status, depth+1);
            যদি (U_FAILURE(status)) {
                delete set_Ref_গ্ৰন্থি;
                ঘূৰাই;
            }
            f_ডান_Child->fParent  = এতত্;
            delete set_Ref_গ্ৰন্থি;
        } অন্যথা {
            f_ডান_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
শূন্য   RBBINode::findNodes(UVector *dest, RBBINode::গ্ৰন্থি_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    U_ASSERT(!dest->hasDeleter());
    যদি (fType == kind) {
        dest->addElement(এতত্, status);
    }
    যদি (f_বাম_Child != nullptr) {
        f_বাম_Child->findNodes(dest, kind, status);
    }
    যদি (f_ডান_Child != nullptr) {
        f_ডান_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

স্থির int32_t serial(ধ্রুৱ RBBINode *গ্ৰন্থি) {
    ঘূৰাই (গ্ৰন্থি == nullptr? -1 : গ্ৰন্থি->f_Serial_অঙ্ক);
}


শূন্য RBBINode::print_গ্ৰন্থি(ধ্রুৱ RBBINode *গ্ৰন্থি) {
    স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ গ্ৰন্থি_Type_Names[] = {
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

    যদি (গ্ৰন্থি==nullptr) {
        RBBIDebugPrintf("%10p", (শূন্য *)গ্ৰন্থি);
    } অন্যথা {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (শূন্য *)গ্ৰন্থি, গ্ৰন্থি->f_Serial_অঙ্ক, গ্ৰন্থি_Type_Names[গ্ৰন্থি->fType],
            গ্ৰন্থি->f_Rule_মূল?'R':' ', গ্ৰন্থি->fChainIn?'C':' ',
            serial(গ্ৰন্থি->f_বাম_Child), serial(গ্ৰন্থি->f_ডান_Child), serial(গ্ৰন্থি->fParent),
            গ্ৰন্থি->fFirstPos, গ্ৰন্থি->fVal);
        যদি (গ্ৰন্থি->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(গ্ৰন্থি->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC শূন্য RBBI_DEBUG_printUnicodeString(ধ্রুৱ UnicodeString &s, পূর্ণাঙ্ক minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
শূন্য RBBINode::print_গ্ৰন্থি_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
শূন্য RBBINode::print_বৃক্ষ(ধ্রুৱ RBBINode *গ্ৰন্থি, UBool printHeading) {
    যদি (printHeading) {
        print_গ্ৰন্থি_Header();
    }
    print_গ্ৰন্থি(গ্ৰন্থি);
    যদি (গ্ৰন্থি != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        যদি (গ্ৰন্থি->fType != varRef) {
            যদি (গ্ৰন্থি->f_বাম_Child != nullptr) {
                print_বৃক্ষ(গ্ৰন্থি->f_বাম_Child, মিছা);
            }
            
            যদি (গ্ৰন্থি->f_ডান_Child != nullptr) {
                print_বৃক্ষ(গ্ৰন্থি->f_ডান_Child, মিছা);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
