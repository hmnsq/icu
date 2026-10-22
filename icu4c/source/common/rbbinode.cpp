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

#यदि !UCONFIG_NO_BREAK_ITERATION

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
RBBINode::RBBINode(गाँठो_Type t, UErrorCode& status) : UMemory() {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
#ifdef RBBI_DEBUG
    f_Serial_अङ्क    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_बायाँ_Child    = nullptr;
    f_दायाँ_Child   = nullptr;
    f_आगत_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = असत्य;
    fLookAheadEnd = असत्य;
    f_Rule_जरा     = असत्य;
    fChainIn      = असत्य;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = नूतन UVector(status);
    fLastPosSet   = नूतन UVector(status);
    fFollowPos    = नूतन UVector(status);
    यदि (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    यदि      (t==opCat)    {fPrecedence = precOpCat;}
    अन्यथा यदि (t==opOr)     {fPrecedence = precOpOr;}
    अन्यथा यदि (t==opStart)  {fPrecedence = precStart;}
    अन्यथा यदि (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(अचर RBBINode &other, UErrorCode& status) : UMemory(other) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
#ifdef RBBI_DEBUG
    f_Serial_अङ्क   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_बायाँ_Child   = nullptr;
    f_दायाँ_Child  = nullptr;
    f_आगत_Set    = other.f_आगत_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_जरा    = असत्य;
    fChainIn     = other.fChainIn;
    fFirstPosSet = नूतन UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = नूतन UVector(status);
    fFollowPos   = नूतन UVector(status);
    यदि (U_SUCCESS(status) &&
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
    delete f_आगत_Set;
    f_आगत_Set = nullptr;

    विकल्प (एतत्->fType) {
    पक्ष varRef:
    पक्ष setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        रोक;

    स्वभाव:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_गाँठो(f_बायाँ_Child);
        f_बायाँ_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_गाँठो(f_दायाँ_Child);
        f_दायाँ_Child = nullptr;
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
शून्य RBBINode::NRDelete_गाँठो(RBBINode *गाँठो) {
    यदि (गाँठो == nullptr) {
        फर्काऊ;
    }

    RBBINode *stop_गाँठो = गाँठो->fParent;
    RBBINode *next_गाँठो = गाँठो;
    जबसम्म (next_गाँठो != stop_गाँठो && next_गाँठो != nullptr) {
        RBBINode *current_गाँठो = next_गाँठो;

        यदि ((current_गाँठो->f_बायाँ_Child == nullptr && current_गाँठो->f_दायाँ_Child == nullptr) ||
                current_गाँठो->fType == varRef ||      // varRef and setRef nodes do not
                current_गाँठो->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_गाँठो = current_गाँठो->fParent;
            यदि (next_गाँठो) {
                यदि (next_गाँठो->f_बायाँ_Child == current_गाँठो) {
                    next_गाँठो->f_बायाँ_Child = nullptr;
                } अन्यथा यदि (next_गाँठो->f_दायाँ_Child == current_गाँठो) {
                    next_गाँठो->f_दायाँ_Child = nullptr;
                }
            }
            delete current_गाँठो;
        } अन्यथा यदि (current_गाँठो->f_बायाँ_Child) {
            next_गाँठो = current_गाँठो->f_बायाँ_Child;
            यदि (next_गाँठो->fParent == nullptr) {
                next_गाँठो->fParent = current_गाँठो;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_गाँठो->fParent == current_गाँठो);
        } अन्यथा यदि (current_गाँठो->f_दायाँ_Child) {
            next_गाँठो = current_गाँठो->f_दायाँ_Child;
            यदि (next_गाँठो->fParent == nullptr) {
                next_गाँठो->fParent = current_गाँठो;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_गाँठो->fParent == current_गाँठो);
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
RBBINode *RBBINode::clone_रूख(UErrorCode &status, पूर्णाङ्क depth) {
    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    यदि (depth > kRecursiveDepthLimit) {
        status = U_आगत_TOO_LONG_ERROR;
        फर्काऊ nullptr;
    }
    RBBINode    *n;

    यदि (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_बायाँ_Child->clone_रूख(status, depth+1);
        यदि (U_FAILURE(status)) {
            फर्काऊ nullptr;
        }
    } अन्यथा यदि (fType == RBBINode::uset) {
        n = एतत्;
    } अन्यथा {
        n = नूतन RBBINode(*एतत्, status);
        यदि (U_FAILURE(status)) {
            delete n;
            फर्काऊ nullptr;
        }
        // Check for null pointer.
        यदि (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ nullptr;
        }
        यदि (f_बायाँ_Child != nullptr) {
            n->f_बायाँ_Child          = f_बायाँ_Child->clone_रूख(status, depth+1);
            यदि (U_FAILURE(status)) {
                delete n;
                फर्काऊ nullptr;
            }
            n->f_बायाँ_Child->fParent = n;
        }
        यदि (f_दायाँ_Child != nullptr) {
            n->f_दायाँ_Child          = f_दायाँ_Child->clone_रूख(status, depth+1);
            यदि (U_FAILURE(status)) {
                delete n;
                फर्काऊ nullptr;
            }
            n->f_दायाँ_Child->fParent = n;
        }
    }
    फर्काऊ n;
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
    यदि (U_FAILURE(status)) {
        फर्काऊ एतत्;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    यदि (depth > kRecursiveDepthLimit) {
        status = U_आगत_TOO_LONG_ERROR;
        फर्काऊ एतत्;
    }
    यदि (fType == varRef) {
        RBBINode *ret_गाँठो  = f_बायाँ_Child->clone_रूख(status, depth+1);
        यदि (U_FAILURE(status)) {
            फर्काऊ एतत्;
        }
        ret_गाँठो->f_Rule_जरा = एतत्->f_Rule_जरा;
        ret_गाँठो->fChainIn  = एतत्->fChainIn;
        delete एतत्;   // TODO: undefined behavior. Fix.
        फर्काऊ ret_गाँठो;
    }

    यदि (f_बायाँ_Child != nullptr) {
        f_बायाँ_Child = f_बायाँ_Child->flattenVariables(status, depth+1);
        यदि (f_बायाँ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        यदि (U_FAILURE(status)) {
            फर्काऊ एतत्;
        }
        f_बायाँ_Child->fParent  = एतत्;
    }
    यदि (f_दायाँ_Child != nullptr) {
        f_दायाँ_Child = f_दायाँ_Child->flattenVariables(status, depth+1);
        यदि (f_दायाँ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        यदि (U_FAILURE(status)) {
            फर्काऊ एतत्;
        }
        f_दायाँ_Child->fParent = एतत्;
    }
    फर्काऊ एतत्;
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
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    यदि (depth > kRecursiveDepthLimit) {
        status = U_आगत_TOO_LONG_ERROR;
        फर्काऊ;
    }
    U_ASSERT(fType != setRef);

    यदि (f_बायाँ_Child != nullptr) {
        यदि (f_बायाँ_Child->fType==setRef) {
            RBBINode *set_Ref_गाँठो = f_बायाँ_Child;
            RBBINode *uset_गाँठो   = set_Ref_गाँठो->f_बायाँ_Child;
            RBBINode *repl_रूख   = uset_गाँठो->f_बायाँ_Child;
            f_बायाँ_Child           = repl_रूख->clone_रूख(status, depth+1);
            यदि (U_FAILURE(status)) {
                delete set_Ref_गाँठो;
                फर्काऊ;
            }
            f_बायाँ_Child->fParent  = एतत्;
            delete set_Ref_गाँठो;
        } अन्यथा {
            f_बायाँ_Child->flattenSets(status, depth+1);
        }
    }

    यदि (f_दायाँ_Child != nullptr) {
        यदि (f_दायाँ_Child->fType==setRef) {
            RBBINode *set_Ref_गाँठो = f_दायाँ_Child;
            RBBINode *uset_गाँठो   = set_Ref_गाँठो->f_बायाँ_Child;
            RBBINode *repl_रूख   = uset_गाँठो->f_बायाँ_Child;
            f_दायाँ_Child           = repl_रूख->clone_रूख(status, depth+1);
            यदि (U_FAILURE(status)) {
                delete set_Ref_गाँठो;
                फर्काऊ;
            }
            f_दायाँ_Child->fParent  = एतत्;
            delete set_Ref_गाँठो;
        } अन्यथा {
            f_दायाँ_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
शून्य   RBBINode::findNodes(UVector *dest, RBBINode::गाँठो_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    U_ASSERT(!dest->hasDeleter());
    यदि (fType == kind) {
        dest->addElement(एतत्, status);
    }
    यदि (f_बायाँ_Child != nullptr) {
        f_बायाँ_Child->findNodes(dest, kind, status);
    }
    यदि (f_दायाँ_Child != nullptr) {
        f_दायाँ_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

स्थिर int32_t serial(अचर RBBINode *गाँठो) {
    फर्काऊ (गाँठो == nullptr? -1 : गाँठो->f_Serial_अङ्क);
}


शून्य RBBINode::print_गाँठो(अचर RBBINode *गाँठो) {
    स्थिर अचर वर्ण * अचर गाँठो_Type_Names[] = {
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

    यदि (गाँठो==nullptr) {
        RBBIDebugPrintf("%10p", (शून्य *)गाँठो);
    } अन्यथा {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (शून्य *)गाँठो, गाँठो->f_Serial_अङ्क, गाँठो_Type_Names[गाँठो->fType],
            गाँठो->f_Rule_जरा?'R':' ', गाँठो->fChainIn?'C':' ',
            serial(गाँठो->f_बायाँ_Child), serial(गाँठो->f_दायाँ_Child), serial(गाँठो->fParent),
            गाँठो->fFirstPos, गाँठो->fVal);
        यदि (गाँठो->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(गाँठो->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC शून्य RBBI_DEBUG_printUnicodeString(अचर UnicodeString &s, पूर्णाङ्क minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
शून्य RBBINode::print_गाँठो_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
शून्य RBBINode::print_रूख(अचर RBBINode *गाँठो, UBool printHeading) {
    यदि (printHeading) {
        print_गाँठो_Header();
    }
    print_गाँठो(गाँठो);
    यदि (गाँठो != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        यदि (गाँठो->fType != varRef) {
            यदि (गाँठो->f_बायाँ_Child != nullptr) {
                print_रूख(गाँठो->f_बायाँ_Child, असत्य);
            }
            
            यदि (गाँठो->f_दायाँ_Child != nullptr) {
                print_रूख(गाँठो->f_दायाँ_Child, असत्य);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
