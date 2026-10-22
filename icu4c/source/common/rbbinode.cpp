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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/unistr.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/parsepos.h"

#समावेश "cstr.h"
#समावेश "uvector.h"

#समावेश "rbbirb.h"
#समावेश "rbbinode.h"

#समावेश "uassert.h"


U_NAMESPACE_BEGIN

#अगर_घोषित RBBI_DEBUG
अटल पूर्णांक  gLastSerial = 0;
#पूर्ण_अगर


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(गाँठ_Type t, UErrorCode& status) : UMemory() {
    अगर (U_FAILURE(status)) {
        वापस;
    }
#अगर_घोषित RBBI_DEBUG
    f_Serial_अंक    = ++gLastSerial;
#पूर्ण_अगर
    fType         = t;
    fParent       = nullptr;
    f_बायाँ_Child    = nullptr;
    f_दायाँ_Child   = nullptr;
    f_निवेश_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = false;
    fLookAheadEnd = false;
    f_Rule_मूल     = false;
    fChainIn      = false;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = नया UVector(status);
    fLastPosSet   = नया UVector(status);
    fFollowPos    = नया UVector(status);
    अगर (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    अगर      (t==opCat)    {fPrecedence = precOpCat;}
    अन्यथा अगर (t==opOr)     {fPrecedence = precOpOr;}
    अन्यथा अगर (t==opStart)  {fPrecedence = precStart;}
    अन्यथा अगर (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(स्थिर RBBINode &other, UErrorCode& status) : UMemory(other) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
#अगर_घोषित RBBI_DEBUG
    f_Serial_अंक   = ++gLastSerial;
#पूर्ण_अगर
    fType        = other.fType;
    fParent      = nullptr;
    f_बायाँ_Child   = nullptr;
    f_दायाँ_Child  = nullptr;
    f_निवेश_Set    = other.f_निवेश_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_मूल    = false;
    fChainIn     = other.fChainIn;
    fFirstPosSet = नया UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = नया UVector(status);
    fFollowPos   = नया UVector(status);
    अगर (U_SUCCESS(status) &&
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
    मिटाओ f_निवेश_Set;
    f_निवेश_Set = nullptr;

    चयन (यह->fType) {
    हाल varRef:
    हाल setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        अवरोध;

    शेष:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_गाँठ(f_बायाँ_Child);
        f_बायाँ_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_गाँठ(f_दायाँ_Child);
        f_दायाँ_Child = nullptr;
    }

    मिटाओ fFirstPosSet;
    मिटाओ fLastPosSet;
    मिटाओ fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
व्योम RBBINode::NRDelete_गाँठ(RBBINode *गाँठ) {
    अगर (गाँठ == nullptr) {
        वापस;
    }

    RBBINode *stop_गाँठ = गाँठ->fParent;
    RBBINode *अग्र_गाँठ = गाँठ;
    जबतक (अग्र_गाँठ != stop_गाँठ && अग्र_गाँठ != nullptr) {
        RBBINode *current_गाँठ = अग्र_गाँठ;

        अगर ((current_गाँठ->f_बायाँ_Child == nullptr && current_गाँठ->f_दायाँ_Child == nullptr) ||
                current_गाँठ->fType == varRef ||      // varRef and setRef nodes do not
                current_गाँठ->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            अग्र_गाँठ = current_गाँठ->fParent;
            अगर (अग्र_गाँठ) {
                अगर (अग्र_गाँठ->f_बायाँ_Child == current_गाँठ) {
                    अग्र_गाँठ->f_बायाँ_Child = nullptr;
                } अन्यथा अगर (अग्र_गाँठ->f_दायाँ_Child == current_गाँठ) {
                    अग्र_गाँठ->f_दायाँ_Child = nullptr;
                }
            }
            मिटाओ current_गाँठ;
        } अन्यथा अगर (current_गाँठ->f_बायाँ_Child) {
            अग्र_गाँठ = current_गाँठ->f_बायाँ_Child;
            अगर (अग्र_गाँठ->fParent == nullptr) {
                अग्र_गाँठ->fParent = current_गाँठ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(अग्र_गाँठ->fParent == current_गाँठ);
        } अन्यथा अगर (current_गाँठ->f_दायाँ_Child) {
            अग्र_गाँठ = current_गाँठ->f_दायाँ_Child;
            अगर (अग्र_गाँठ->fParent == nullptr) {
                अग्र_गाँठ->fParent = current_गाँठ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(अग्र_गाँठ->fParent == current_गाँठ);
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
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    अगर (depth > kRecursiveDepthLimit) {
        status = U_निवेश_TOO_LONG_ERROR;
        वापस nullptr;
    }
    RBBINode    *n;

    अगर (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_बायाँ_Child->clone_वृक्ष(status, depth+1);
        अगर (U_FAILURE(status)) {
            वापस nullptr;
        }
    } अन्यथा अगर (fType == RBBINode::uset) {
        n = यह;
    } अन्यथा {
        n = नया RBBINode(*यह, status);
        अगर (U_FAILURE(status)) {
            मिटाओ n;
            वापस nullptr;
        }
        // Check for null pointer.
        अगर (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        }
        अगर (f_बायाँ_Child != nullptr) {
            n->f_बायाँ_Child          = f_बायाँ_Child->clone_वृक्ष(status, depth+1);
            अगर (U_FAILURE(status)) {
                मिटाओ n;
                वापस nullptr;
            }
            n->f_बायाँ_Child->fParent = n;
        }
        अगर (f_दायाँ_Child != nullptr) {
            n->f_दायाँ_Child          = f_दायाँ_Child->clone_वृक्ष(status, depth+1);
            अगर (U_FAILURE(status)) {
                मिटाओ n;
                वापस nullptr;
            }
            n->f_दायाँ_Child->fParent = n;
        }
    }
    वापस n;
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
    अगर (U_FAILURE(status)) {
        वापस यह;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    अगर (depth > kRecursiveDepthLimit) {
        status = U_निवेश_TOO_LONG_ERROR;
        वापस यह;
    }
    अगर (fType == varRef) {
        RBBINode *ret_गाँठ  = f_बायाँ_Child->clone_वृक्ष(status, depth+1);
        अगर (U_FAILURE(status)) {
            वापस यह;
        }
        ret_गाँठ->f_Rule_मूल = यह->f_Rule_मूल;
        ret_गाँठ->fChainIn  = यह->fChainIn;
        मिटाओ यह;   // TODO: undefined behavior. Fix.
        वापस ret_गाँठ;
    }

    अगर (f_बायाँ_Child != nullptr) {
        f_बायाँ_Child = f_बायाँ_Child->flattenVariables(status, depth+1);
        अगर (f_बायाँ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        अगर (U_FAILURE(status)) {
            वापस यह;
        }
        f_बायाँ_Child->fParent  = यह;
    }
    अगर (f_दायाँ_Child != nullptr) {
        f_दायाँ_Child = f_दायाँ_Child->flattenVariables(status, depth+1);
        अगर (f_दायाँ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        अगर (U_FAILURE(status)) {
            वापस यह;
        }
        f_दायाँ_Child->fParent = यह;
    }
    वापस यह;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
व्योम RBBINode::flattenSets(UErrorCode &status, पूर्णांक depth) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    अगर (depth > kRecursiveDepthLimit) {
        status = U_निवेश_TOO_LONG_ERROR;
        वापस;
    }
    U_ASSERT(fType != setRef);

    अगर (f_बायाँ_Child != nullptr) {
        अगर (f_बायाँ_Child->fType==setRef) {
            RBBINode *set_Ref_गाँठ = f_बायाँ_Child;
            RBBINode *uset_गाँठ   = set_Ref_गाँठ->f_बायाँ_Child;
            RBBINode *repl_वृक्ष   = uset_गाँठ->f_बायाँ_Child;
            f_बायाँ_Child           = repl_वृक्ष->clone_वृक्ष(status, depth+1);
            अगर (U_FAILURE(status)) {
                मिटाओ set_Ref_गाँठ;
                वापस;
            }
            f_बायाँ_Child->fParent  = यह;
            मिटाओ set_Ref_गाँठ;
        } अन्यथा {
            f_बायाँ_Child->flattenSets(status, depth+1);
        }
    }

    अगर (f_दायाँ_Child != nullptr) {
        अगर (f_दायाँ_Child->fType==setRef) {
            RBBINode *set_Ref_गाँठ = f_दायाँ_Child;
            RBBINode *uset_गाँठ   = set_Ref_गाँठ->f_बायाँ_Child;
            RBBINode *repl_वृक्ष   = uset_गाँठ->f_बायाँ_Child;
            f_दायाँ_Child           = repl_वृक्ष->clone_वृक्ष(status, depth+1);
            अगर (U_FAILURE(status)) {
                मिटाओ set_Ref_गाँठ;
                वापस;
            }
            f_दायाँ_Child->fParent  = यह;
            मिटाओ set_Ref_गाँठ;
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
व्योम   RBBINode::findNodes(UVector *dest, RBBINode::गाँठ_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    अगर (U_FAILURE(status)) {
        वापस;
    }
    U_ASSERT(!dest->hasDeleter());
    अगर (fType == kind) {
        dest->addElement(यह, status);
    }
    अगर (f_बायाँ_Child != nullptr) {
        f_बायाँ_Child->findNodes(dest, kind, status);
    }
    अगर (f_दायाँ_Child != nullptr) {
        f_दायाँ_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG

अटल int32_t serial(स्थिर RBBINode *गाँठ) {
    वापस (गाँठ == nullptr? -1 : गाँठ->f_Serial_अंक);
}


व्योम RBBINode::print_गाँठ(स्थिर RBBINode *गाँठ) {
    अटल स्थिर अक्षर * स्थिर गाँठ_Type_Names[] = {
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

    अगर (गाँठ==nullptr) {
        RBBIDebugPrintf("%10p", (व्योम *)गाँठ);
    } अन्यथा {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (व्योम *)गाँठ, गाँठ->f_Serial_अंक, गाँठ_Type_Names[गाँठ->fType],
            गाँठ->f_Rule_मूल?'R':' ', गाँठ->fChainIn?'C':' ',
            serial(गाँठ->f_बायाँ_Child), serial(गाँठ->f_दायाँ_Child), serial(गाँठ->fParent),
            गाँठ->fFirstPos, गाँठ->fVal);
        अगर (गाँठ->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(गाँठ->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#पूर्ण_अगर


#अगर_घोषित RBBI_DEBUG
U_CFUNC व्योम RBBI_DEBUG_printUnicodeString(स्थिर UnicodeString &s, पूर्णांक minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#पूर्ण_अगर


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम RBBINode::print_गाँठ_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
व्योम RBBINode::print_वृक्ष(स्थिर RBBINode *गाँठ, UBool printHeading) {
    अगर (printHeading) {
        print_गाँठ_Header();
    }
    print_गाँठ(गाँठ);
    अगर (गाँठ != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        अगर (गाँठ->fType != varRef) {
            अगर (गाँठ->f_बायाँ_Child != nullptr) {
                print_वृक्ष(गाँठ->f_बायाँ_Child, false);
            }
            
            अगर (गाँठ->f_दायाँ_Child != nullptr) {
                print_वृक्ष(गाँठ->f_दायाँ_Child, false);
            }
        }
    }
}
#पूर्ण_अगर



U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
