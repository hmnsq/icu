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

#जर !UCONFIG_NO_BREAK_ITERATION

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
स्थिर पूर्णांक  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(गाठ_Type t, UErrorCode& status) : UMemory() {
    जर (U_FAILURE(status)) {
        परत;
    }
#ifdef RBBI_DEBUG
    f_Serial_अंक    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_डावा_Child    = nullptr;
    f_उजवा_Child   = nullptr;
    f_आदान_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = असत्य;
    fLookAheadEnd = असत्य;
    f_Rule_मूळ     = असत्य;
    fChainIn      = असत्य;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = नवीन UVector(status);
    fLastPosSet   = नवीन UVector(status);
    fFollowPos    = नवीन UVector(status);
    जर (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    जर      (t==opCat)    {fPrecedence = precOpCat;}
    अन्यथा जर (t==opOr)     {fPrecedence = precOpOr;}
    अन्यथा जर (t==opStart)  {fPrecedence = precStart;}
    अन्यथा जर (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(स्थिरांक RBBINode &other, UErrorCode& status) : UMemory(other) {
    जर (U_FAILURE(status)) {
        परत;
    }
#ifdef RBBI_DEBUG
    f_Serial_अंक   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_डावा_Child   = nullptr;
    f_उजवा_Child  = nullptr;
    f_आदान_Set    = other.f_आदान_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_मूळ    = असत्य;
    fChainIn     = other.fChainIn;
    fFirstPosSet = नवीन UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = नवीन UVector(status);
    fFollowPos   = नवीन UVector(status);
    जर (U_SUCCESS(status) &&
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
    delete f_आदान_Set;
    f_आदान_Set = nullptr;

    निवड (हे->fType) {
    स्थिती varRef:
    स्थिती setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        थांबा;

    पूर्वनिर्धारित:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_गाठ(f_डावा_Child);
        f_डावा_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_गाठ(f_उजवा_Child);
        f_उजवा_Child = nullptr;
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
शून्य RBBINode::NRDelete_गाठ(RBBINode *गाठ) {
    जर (गाठ == nullptr) {
        परत;
    }

    RBBINode *stop_गाठ = गाठ->fParent;
    RBBINode *next_गाठ = गाठ;
    पर्यंत (next_गाठ != stop_गाठ && next_गाठ != nullptr) {
        RBBINode *current_गाठ = next_गाठ;

        जर ((current_गाठ->f_डावा_Child == nullptr && current_गाठ->f_उजवा_Child == nullptr) ||
                current_गाठ->fType == varRef ||      // varRef and setRef nodes do not
                current_गाठ->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_गाठ = current_गाठ->fParent;
            जर (next_गाठ) {
                जर (next_गाठ->f_डावा_Child == current_गाठ) {
                    next_गाठ->f_डावा_Child = nullptr;
                } अन्यथा जर (next_गाठ->f_उजवा_Child == current_गाठ) {
                    next_गाठ->f_उजवा_Child = nullptr;
                }
            }
            delete current_गाठ;
        } अन्यथा जर (current_गाठ->f_डावा_Child) {
            next_गाठ = current_गाठ->f_डावा_Child;
            जर (next_गाठ->fParent == nullptr) {
                next_गाठ->fParent = current_गाठ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_गाठ->fParent == current_गाठ);
        } अन्यथा जर (current_गाठ->f_उजवा_Child) {
            next_गाठ = current_गाठ->f_उजवा_Child;
            जर (next_गाठ->fParent == nullptr) {
                next_गाठ->fParent = current_गाठ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_गाठ->fParent == current_गाठ);
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
constexpr पूर्णांक kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_वृक्ष(UErrorCode &status, पूर्णांक depth) {
    जर (U_FAILURE(status)) {
        परत nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    जर (depth > kRecursiveDepthLimit) {
        status = U_आदान_TOO_LONG_ERROR;
        परत nullptr;
    }
    RBBINode    *n;

    जर (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_डावा_Child->clone_वृक्ष(status, depth+1);
        जर (U_FAILURE(status)) {
            परत nullptr;
        }
    } अन्यथा जर (fType == RBBINode::uset) {
        n = हे;
    } अन्यथा {
        n = नवीन RBBINode(*हे, status);
        जर (U_FAILURE(status)) {
            delete n;
            परत nullptr;
        }
        // Check for null pointer.
        जर (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            परत nullptr;
        }
        जर (f_डावा_Child != nullptr) {
            n->f_डावा_Child          = f_डावा_Child->clone_वृक्ष(status, depth+1);
            जर (U_FAILURE(status)) {
                delete n;
                परत nullptr;
            }
            n->f_डावा_Child->fParent = n;
        }
        जर (f_उजवा_Child != nullptr) {
            n->f_उजवा_Child          = f_उजवा_Child->clone_वृक्ष(status, depth+1);
            जर (U_FAILURE(status)) {
                delete n;
                परत nullptr;
            }
            n->f_उजवा_Child->fParent = n;
        }
    }
    परत n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, पूर्णांक depth) {
    जर (U_FAILURE(status)) {
        परत हे;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    जर (depth > kRecursiveDepthLimit) {
        status = U_आदान_TOO_LONG_ERROR;
        परत हे;
    }
    जर (fType == varRef) {
        RBBINode *ret_गाठ  = f_डावा_Child->clone_वृक्ष(status, depth+1);
        जर (U_FAILURE(status)) {
            परत हे;
        }
        ret_गाठ->f_Rule_मूळ = हे->f_Rule_मूळ;
        ret_गाठ->fChainIn  = हे->fChainIn;
        delete हे;   // TODO: undefined behavior. Fix.
        परत ret_गाठ;
    }

    जर (f_डावा_Child != nullptr) {
        f_डावा_Child = f_डावा_Child->flattenVariables(status, depth+1);
        जर (f_डावा_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        जर (U_FAILURE(status)) {
            परत हे;
        }
        f_डावा_Child->fParent  = हे;
    }
    जर (f_उजवा_Child != nullptr) {
        f_उजवा_Child = f_उजवा_Child->flattenVariables(status, depth+1);
        जर (f_उजवा_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        जर (U_FAILURE(status)) {
            परत हे;
        }
        f_उजवा_Child->fParent = हे;
    }
    परत हे;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
शून्य RBBINode::flattenSets(UErrorCode &status, पूर्णांक depth) {
    जर (U_FAILURE(status)) {
        परत;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    जर (depth > kRecursiveDepthLimit) {
        status = U_आदान_TOO_LONG_ERROR;
        परत;
    }
    U_ASSERT(fType != setRef);

    जर (f_डावा_Child != nullptr) {
        जर (f_डावा_Child->fType==setRef) {
            RBBINode *set_Ref_गाठ = f_डावा_Child;
            RBBINode *uset_गाठ   = set_Ref_गाठ->f_डावा_Child;
            RBBINode *repl_वृक्ष   = uset_गाठ->f_डावा_Child;
            f_डावा_Child           = repl_वृक्ष->clone_वृक्ष(status, depth+1);
            जर (U_FAILURE(status)) {
                delete set_Ref_गाठ;
                परत;
            }
            f_डावा_Child->fParent  = हे;
            delete set_Ref_गाठ;
        } अन्यथा {
            f_डावा_Child->flattenSets(status, depth+1);
        }
    }

    जर (f_उजवा_Child != nullptr) {
        जर (f_उजवा_Child->fType==setRef) {
            RBBINode *set_Ref_गाठ = f_उजवा_Child;
            RBBINode *uset_गाठ   = set_Ref_गाठ->f_डावा_Child;
            RBBINode *repl_वृक्ष   = uset_गाठ->f_डावा_Child;
            f_उजवा_Child           = repl_वृक्ष->clone_वृक्ष(status, depth+1);
            जर (U_FAILURE(status)) {
                delete set_Ref_गाठ;
                परत;
            }
            f_उजवा_Child->fParent  = हे;
            delete set_Ref_गाठ;
        } अन्यथा {
            f_उजवा_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
शून्य   RBBINode::findNodes(UVector *dest, RBBINode::गाठ_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    जर (U_FAILURE(status)) {
        परत;
    }
    U_ASSERT(!dest->hasDeleter());
    जर (fType == kind) {
        dest->addElement(हे, status);
    }
    जर (f_डावा_Child != nullptr) {
        f_डावा_Child->findNodes(dest, kind, status);
    }
    जर (f_उजवा_Child != nullptr) {
        f_उजवा_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

स्थिर int32_t serial(स्थिरांक RBBINode *गाठ) {
    परत (गाठ == nullptr? -1 : गाठ->f_Serial_अंक);
}


शून्य RBBINode::print_गाठ(स्थिरांक RBBINode *गाठ) {
    स्थिर स्थिरांक अक्षर * स्थिरांक गाठ_Type_Names[] = {
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

    जर (गाठ==nullptr) {
        RBBIDebugPrintf("%10p", (शून्य *)गाठ);
    } अन्यथा {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (शून्य *)गाठ, गाठ->f_Serial_अंक, गाठ_Type_Names[गाठ->fType],
            गाठ->f_Rule_मूळ?'R':' ', गाठ->fChainIn?'C':' ',
            serial(गाठ->f_डावा_Child), serial(गाठ->f_उजवा_Child), serial(गाठ->fParent),
            गाठ->fFirstPos, गाठ->fVal);
        जर (गाठ->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(गाठ->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC शून्य RBBI_DEBUG_printUnicodeString(स्थिरांक UnicodeString &s, पूर्णांक minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
शून्य RBBINode::print_गाठ_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
शून्य RBBINode::print_वृक्ष(स्थिरांक RBBINode *गाठ, UBool printHeading) {
    जर (printHeading) {
        print_गाठ_Header();
    }
    print_गाठ(गाठ);
    जर (गाठ != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        जर (गाठ->fType != varRef) {
            जर (गाठ->f_डावा_Child != nullptr) {
                print_वृक्ष(गाठ->f_डावा_Child, असत्य);
            }
            
            जर (गाठ->f_उजवा_Child != nullptr) {
                print_वृक्ष(गाठ->f_उजवा_Child, असत्य);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
