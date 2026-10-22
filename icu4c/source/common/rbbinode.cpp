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

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/parsepos.h"

#अन्तर्भाव "cstr.h"
#अन्तर्भाव "uvector.h"

#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "rbbinode.h"

#अन्तर्भाव "uassert.h"


U_NAMESPACE_BEGIN

#ifdef RBBI_DEBUG
स्थिर पूर्णाङ्क  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(ग्रन्थिः_Type t, UErrorCode& status) : UMemory() {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
#ifdef RBBI_DEBUG
    f_Serial_अङ्कः    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_वामः_Child    = nullptr;
    f_दक्षिणः_Child   = nullptr;
    f_निवेशः_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = मिच्छा;
    fLookAheadEnd = मिच्छा;
    f_Rule_मूलम्     = मिच्छा;
    fChainIn      = मिच्छा;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = नूतन UVector(status);
    fLastPosSet   = नूतन UVector(status);
    fFollowPos    = नूतन UVector(status);
    सचे (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    सचे      (t==opCat)    {fPrecedence = precOpCat;}
    अञ्ञथा सचे (t==opOr)     {fPrecedence = precOpOr;}
    अञ्ञथा सचे (t==opStart)  {fPrecedence = precStart;}
    अञ्ञथा सचे (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ध्रुव RBBINode &other, UErrorCode& status) : UMemory(other) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
#ifdef RBBI_DEBUG
    f_Serial_अङ्कः   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_वामः_Child   = nullptr;
    f_दक्षिणः_Child  = nullptr;
    f_निवेशः_Set    = other.f_निवेशः_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_मूलम्    = मिच्छा;
    fChainIn     = other.fChainIn;
    fFirstPosSet = नूतन UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = नूतन UVector(status);
    fFollowPos   = नूतन UVector(status);
    सचे (U_SUCCESS(status) &&
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
    delete f_निवेशः_Set;
    f_निवेशः_Set = nullptr;

    विकल्प (एतत्->fType) {
    पक्ष varRef:
    पक्ष setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        भङ्ग;

    स्वभाव:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_ग्रन्थिः(f_वामः_Child);
        f_वामः_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_ग्रन्थिः(f_दक्षिणः_Child);
        f_दक्षिणः_Child = nullptr;
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
शून्य RBBINode::NRDelete_ग्रन्थिः(RBBINode *ग्रन्थिः) {
    सचे (ग्रन्थिः == nullptr) {
        पटिनिवत्त;
    }

    RBBINode *stop_ग्रन्थिः = ग्रन्थिः->fParent;
    RBBINode *next_ग्रन्थिः = ग्रन्थिः;
    याव (next_ग्रन्थिः != stop_ग्रन्थिः && next_ग्रन्थिः != nullptr) {
        RBBINode *current_ग्रन्थिः = next_ग्रन्थिः;

        सचे ((current_ग्रन्थिः->f_वामः_Child == nullptr && current_ग्रन्थिः->f_दक्षिणः_Child == nullptr) ||
                current_ग्रन्थिः->fType == varRef ||      // varRef and setRef nodes do not
                current_ग्रन्थिः->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_ग्रन्थिः = current_ग्रन्थिः->fParent;
            सचे (next_ग्रन्थिः) {
                सचे (next_ग्रन्थिः->f_वामः_Child == current_ग्रन्थिः) {
                    next_ग्रन्थिः->f_वामः_Child = nullptr;
                } अञ्ञथा सचे (next_ग्रन्थिः->f_दक्षिणः_Child == current_ग्रन्थिः) {
                    next_ग्रन्थिः->f_दक्षिणः_Child = nullptr;
                }
            }
            delete current_ग्रन्थिः;
        } अञ्ञथा सचे (current_ग्रन्थिः->f_वामः_Child) {
            next_ग्रन्थिः = current_ग्रन्थिः->f_वामः_Child;
            सचे (next_ग्रन्थिः->fParent == nullptr) {
                next_ग्रन्थिः->fParent = current_ग्रन्थिः;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_ग्रन्थिः->fParent == current_ग्रन्थिः);
        } अञ्ञथा सचे (current_ग्रन्थिः->f_दक्षिणः_Child) {
            next_ग्रन्थिः = current_ग्रन्थिः->f_दक्षिणः_Child;
            सचे (next_ग्रन्थिः->fParent == nullptr) {
                next_ग्रन्थिः->fParent = current_ग्रन्थिः;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_ग्रन्थिः->fParent == current_ग्रन्थिः);
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
constexpr पूर्णाङ्क kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_वृक्षः(UErrorCode &status, पूर्णाङ्क depth) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    सचे (depth > kRecursiveDepthLimit) {
        status = U_निवेशः_TOO_LONG_ERROR;
        पटिनिवत्त nullptr;
    }
    RBBINode    *n;

    सचे (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_वामः_Child->clone_वृक्षः(status, depth+1);
        सचे (U_FAILURE(status)) {
            पटिनिवत्त nullptr;
        }
    } अञ्ञथा सचे (fType == RBBINode::uset) {
        n = एतत्;
    } अञ्ञथा {
        n = नूतन RBBINode(*एतत्, status);
        सचे (U_FAILURE(status)) {
            delete n;
            पटिनिवत्त nullptr;
        }
        // Check for null pointer.
        सचे (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त nullptr;
        }
        सचे (f_वामः_Child != nullptr) {
            n->f_वामः_Child          = f_वामः_Child->clone_वृक्षः(status, depth+1);
            सचे (U_FAILURE(status)) {
                delete n;
                पटिनिवत्त nullptr;
            }
            n->f_वामः_Child->fParent = n;
        }
        सचे (f_दक्षिणः_Child != nullptr) {
            n->f_दक्षिणः_Child          = f_दक्षिणः_Child->clone_वृक्षः(status, depth+1);
            सचे (U_FAILURE(status)) {
                delete n;
                पटिनिवत्त nullptr;
            }
            n->f_दक्षिणः_Child->fParent = n;
        }
    }
    पटिनिवत्त n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, पूर्णाङ्क depth) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त एतत्;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    सचे (depth > kRecursiveDepthLimit) {
        status = U_निवेशः_TOO_LONG_ERROR;
        पटिनिवत्त एतत्;
    }
    सचे (fType == varRef) {
        RBBINode *ret_ग्रन्थिः  = f_वामः_Child->clone_वृक्षः(status, depth+1);
        सचे (U_FAILURE(status)) {
            पटिनिवत्त एतत्;
        }
        ret_ग्रन्थिः->f_Rule_मूलम् = एतत्->f_Rule_मूलम्;
        ret_ग्रन्थिः->fChainIn  = एतत्->fChainIn;
        delete एतत्;   // TODO: undefined behavior. Fix.
        पटिनिवत्त ret_ग्रन्थिः;
    }

    सचे (f_वामः_Child != nullptr) {
        f_वामः_Child = f_वामः_Child->flattenVariables(status, depth+1);
        सचे (f_वामः_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        सचे (U_FAILURE(status)) {
            पटिनिवत्त एतत्;
        }
        f_वामः_Child->fParent  = एतत्;
    }
    सचे (f_दक्षिणः_Child != nullptr) {
        f_दक्षिणः_Child = f_दक्षिणः_Child->flattenVariables(status, depth+1);
        सचे (f_दक्षिणः_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        सचे (U_FAILURE(status)) {
            पटिनिवत्त एतत्;
        }
        f_दक्षिणः_Child->fParent = एतत्;
    }
    पटिनिवत्त एतत्;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
शून्य RBBINode::flattenSets(UErrorCode &status, पूर्णाङ्क depth) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    सचे (depth > kRecursiveDepthLimit) {
        status = U_निवेशः_TOO_LONG_ERROR;
        पटिनिवत्त;
    }
    U_ASSERT(fType != setRef);

    सचे (f_वामः_Child != nullptr) {
        सचे (f_वामः_Child->fType==setRef) {
            RBBINode *set_Ref_ग्रन्थिः = f_वामः_Child;
            RBBINode *uset_ग्रन्थिः   = set_Ref_ग्रन्थिः->f_वामः_Child;
            RBBINode *repl_वृक्षः   = uset_ग्रन्थिः->f_वामः_Child;
            f_वामः_Child           = repl_वृक्षः->clone_वृक्षः(status, depth+1);
            सचे (U_FAILURE(status)) {
                delete set_Ref_ग्रन्थिः;
                पटिनिवत्त;
            }
            f_वामः_Child->fParent  = एतत्;
            delete set_Ref_ग्रन्थिः;
        } अञ्ञथा {
            f_वामः_Child->flattenSets(status, depth+1);
        }
    }

    सचे (f_दक्षिणः_Child != nullptr) {
        सचे (f_दक्षिणः_Child->fType==setRef) {
            RBBINode *set_Ref_ग्रन्थिः = f_दक्षिणः_Child;
            RBBINode *uset_ग्रन्थिः   = set_Ref_ग्रन्थिः->f_वामः_Child;
            RBBINode *repl_वृक्षः   = uset_ग्रन्थिः->f_वामः_Child;
            f_दक्षिणः_Child           = repl_वृक्षः->clone_वृक्षः(status, depth+1);
            सचे (U_FAILURE(status)) {
                delete set_Ref_ग्रन्थिः;
                पटिनिवत्त;
            }
            f_दक्षिणः_Child->fParent  = एतत्;
            delete set_Ref_ग्रन्थिः;
        } अञ्ञथा {
            f_दक्षिणः_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
शून्य   RBBINode::findNodes(UVector *dest, RBBINode::ग्रन्थिः_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    U_ASSERT(!dest->hasDeleter());
    सचे (fType == kind) {
        dest->addElement(एतत्, status);
    }
    सचे (f_वामः_Child != nullptr) {
        f_वामः_Child->findNodes(dest, kind, status);
    }
    सचे (f_दक्षिणः_Child != nullptr) {
        f_दक्षिणः_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

स्थिर int32_t serial(ध्रुव RBBINode *ग्रन्थिः) {
    पटिनिवत्त (ग्रन्थिः == nullptr? -1 : ग्रन्थिः->f_Serial_अङ्कः);
}


शून्य RBBINode::print_ग्रन्थिः(ध्रुव RBBINode *ग्रन्थिः) {
    स्थिर ध्रुव वर्ण * ध्रुव ग्रन्थिः_Type_Names[] = {
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

    सचे (ग्रन्थिः==nullptr) {
        RBBIDebugPrintf("%10p", (शून्य *)ग्रन्थिः);
    } अञ्ञथा {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (शून्य *)ग्रन्थिः, ग्रन्थिः->f_Serial_अङ्कः, ग्रन्थिः_Type_Names[ग्रन्थिः->fType],
            ग्रन्थिः->f_Rule_मूलम्?'R':' ', ग्रन्थिः->fChainIn?'C':' ',
            serial(ग्रन्थिः->f_वामः_Child), serial(ग्रन्थिः->f_दक्षिणः_Child), serial(ग्रन्थिः->fParent),
            ग्रन्थिः->fFirstPos, ग्रन्थिः->fVal);
        सचे (ग्रन्थिः->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(ग्रन्थिः->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC शून्य RBBI_DEBUG_printUnicodeString(ध्रुव UnicodeString &s, पूर्णाङ्क minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
शून्य RBBINode::print_ग्रन्थिः_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
शून्य RBBINode::print_वृक्षः(ध्रुव RBBINode *ग्रन्थिः, UBool printHeading) {
    सचे (printHeading) {
        print_ग्रन्थिः_Header();
    }
    print_ग्रन्थिः(ग्रन्थिः);
    सचे (ग्रन्थिः != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        सचे (ग्रन्थिः->fType != varRef) {
            सचे (ग्रन्थिः->f_वामः_Child != nullptr) {
                print_वृक्षः(ग्रन्थिः->f_वामः_Child, मिच्छा);
            }
            
            सचे (ग्रन्थिः->f_दक्षिणः_Child != nullptr) {
                print_वृक्षः(ग्रन्थिः->f_दक्षिणः_Child, मिच्छा);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
