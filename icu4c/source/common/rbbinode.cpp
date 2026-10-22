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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/parsepos.h"

#அந்தர்பாவ "cstr.h"
#அந்தர்பாவ "uvector.h"

#அந்தர்பாவ "rbbirb.h"
#அந்தர்பாவ "rbbinode.h"

#அந்தர்பாவ "uassert.h"


U_NAMESPACE_BEGIN

#ifdef RBBI_DEBUG
நிலையான முழு  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(முனை_Type t, UErrorCode& status) : UMemory() {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
#ifdef RBBI_DEBUG
    f_Serial_இலக்கம்    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_இடது_Child    = nullptr;
    f_வலது_Child   = nullptr;
    f_உள்ளீடு_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = பொய்;
    fLookAheadEnd = பொய்;
    f_Rule_வேர்     = பொய்;
    fChainIn      = பொய்;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = புதிய UVector(status);
    fLastPosSet   = புதிய UVector(status);
    fFollowPos    = புதிய UVector(status);
    எனில் (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    எனில்      (t==opCat)    {fPrecedence = precOpCat;}
    இல்லையெனில் எனில் (t==opOr)     {fPrecedence = precOpOr;}
    இல்லையெனில் எனில் (t==opStart)  {fPrecedence = precStart;}
    இல்லையெனில் எனில் (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(மாறிலி RBBINode &other, UErrorCode& status) : UMemory(other) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
#ifdef RBBI_DEBUG
    f_Serial_இலக்கம்   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_இடது_Child   = nullptr;
    f_வலது_Child  = nullptr;
    f_உள்ளீடு_Set    = other.f_உள்ளீடு_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_வேர்    = பொய்;
    fChainIn     = other.fChainIn;
    fFirstPosSet = புதிய UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = புதிய UVector(status);
    fFollowPos   = புதிய UVector(status);
    எனில் (U_SUCCESS(status) &&
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
    delete f_உள்ளீடு_Set;
    f_உள்ளீடு_Set = nullptr;

    தேர்வு (இது->fType) {
    நிலை varRef:
    நிலை setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        நிறுத்து;

    இயல்பு:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_முனை(f_இடது_Child);
        f_இடது_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_முனை(f_வலது_Child);
        f_வலது_Child = nullptr;
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
வெறுமை RBBINode::NRDelete_முனை(RBBINode *முனை) {
    எனில் (முனை == nullptr) {
        திரும்பு;
    }

    RBBINode *stop_முனை = முனை->fParent;
    RBBINode *next_முனை = முனை;
    போது (next_முனை != stop_முனை && next_முனை != nullptr) {
        RBBINode *current_முனை = next_முனை;

        எனில் ((current_முனை->f_இடது_Child == nullptr && current_முனை->f_வலது_Child == nullptr) ||
                current_முனை->fType == varRef ||      // varRef and setRef nodes do not
                current_முனை->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_முனை = current_முனை->fParent;
            எனில் (next_முனை) {
                எனில் (next_முனை->f_இடது_Child == current_முனை) {
                    next_முனை->f_இடது_Child = nullptr;
                } இல்லையெனில் எனில் (next_முனை->f_வலது_Child == current_முனை) {
                    next_முனை->f_வலது_Child = nullptr;
                }
            }
            delete current_முனை;
        } இல்லையெனில் எனில் (current_முனை->f_இடது_Child) {
            next_முனை = current_முனை->f_இடது_Child;
            எனில் (next_முனை->fParent == nullptr) {
                next_முனை->fParent = current_முனை;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_முனை->fParent == current_முனை);
        } இல்லையெனில் எனில் (current_முனை->f_வலது_Child) {
            next_முனை = current_முனை->f_வலது_Child;
            எனில் (next_முனை->fParent == nullptr) {
                next_முனை->fParent = current_முனை;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_முனை->fParent == current_முனை);
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
constexpr முழு kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_மரம்(UErrorCode &status, முழு depth) {
    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    எனில் (depth > kRecursiveDepthLimit) {
        status = U_உள்ளீடு_TOO_LONG_ERROR;
        திரும்பு nullptr;
    }
    RBBINode    *n;

    எனில் (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_இடது_Child->clone_மரம்(status, depth+1);
        எனில் (U_FAILURE(status)) {
            திரும்பு nullptr;
        }
    } இல்லையெனில் எனில் (fType == RBBINode::uset) {
        n = இது;
    } இல்லையெனில் {
        n = புதிய RBBINode(*இது, status);
        எனில் (U_FAILURE(status)) {
            delete n;
            திரும்பு nullptr;
        }
        // Check for null pointer.
        எனில் (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            திரும்பு nullptr;
        }
        எனில் (f_இடது_Child != nullptr) {
            n->f_இடது_Child          = f_இடது_Child->clone_மரம்(status, depth+1);
            எனில் (U_FAILURE(status)) {
                delete n;
                திரும்பு nullptr;
            }
            n->f_இடது_Child->fParent = n;
        }
        எனில் (f_வலது_Child != nullptr) {
            n->f_வலது_Child          = f_வலது_Child->clone_மரம்(status, depth+1);
            எனில் (U_FAILURE(status)) {
                delete n;
                திரும்பு nullptr;
            }
            n->f_வலது_Child->fParent = n;
        }
    }
    திரும்பு n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, முழு depth) {
    எனில் (U_FAILURE(status)) {
        திரும்பு இது;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    எனில் (depth > kRecursiveDepthLimit) {
        status = U_உள்ளீடு_TOO_LONG_ERROR;
        திரும்பு இது;
    }
    எனில் (fType == varRef) {
        RBBINode *ret_முனை  = f_இடது_Child->clone_மரம்(status, depth+1);
        எனில் (U_FAILURE(status)) {
            திரும்பு இது;
        }
        ret_முனை->f_Rule_வேர் = இது->f_Rule_வேர்;
        ret_முனை->fChainIn  = இது->fChainIn;
        delete இது;   // TODO: undefined behavior. Fix.
        திரும்பு ret_முனை;
    }

    எனில் (f_இடது_Child != nullptr) {
        f_இடது_Child = f_இடது_Child->flattenVariables(status, depth+1);
        எனில் (f_இடது_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        எனில் (U_FAILURE(status)) {
            திரும்பு இது;
        }
        f_இடது_Child->fParent  = இது;
    }
    எனில் (f_வலது_Child != nullptr) {
        f_வலது_Child = f_வலது_Child->flattenVariables(status, depth+1);
        எனில் (f_வலது_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        எனில் (U_FAILURE(status)) {
            திரும்பு இது;
        }
        f_வலது_Child->fParent = இது;
    }
    திரும்பு இது;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
வெறுமை RBBINode::flattenSets(UErrorCode &status, முழு depth) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    எனில் (depth > kRecursiveDepthLimit) {
        status = U_உள்ளீடு_TOO_LONG_ERROR;
        திரும்பு;
    }
    U_ASSERT(fType != setRef);

    எனில் (f_இடது_Child != nullptr) {
        எனில் (f_இடது_Child->fType==setRef) {
            RBBINode *set_Ref_முனை = f_இடது_Child;
            RBBINode *uset_முனை   = set_Ref_முனை->f_இடது_Child;
            RBBINode *repl_மரம்   = uset_முனை->f_இடது_Child;
            f_இடது_Child           = repl_மரம்->clone_மரம்(status, depth+1);
            எனில் (U_FAILURE(status)) {
                delete set_Ref_முனை;
                திரும்பு;
            }
            f_இடது_Child->fParent  = இது;
            delete set_Ref_முனை;
        } இல்லையெனில் {
            f_இடது_Child->flattenSets(status, depth+1);
        }
    }

    எனில் (f_வலது_Child != nullptr) {
        எனில் (f_வலது_Child->fType==setRef) {
            RBBINode *set_Ref_முனை = f_வலது_Child;
            RBBINode *uset_முனை   = set_Ref_முனை->f_இடது_Child;
            RBBINode *repl_மரம்   = uset_முனை->f_இடது_Child;
            f_வலது_Child           = repl_மரம்->clone_மரம்(status, depth+1);
            எனில் (U_FAILURE(status)) {
                delete set_Ref_முனை;
                திரும்பு;
            }
            f_வலது_Child->fParent  = இது;
            delete set_Ref_முனை;
        } இல்லையெனில் {
            f_வலது_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
வெறுமை   RBBINode::findNodes(UVector *dest, RBBINode::முனை_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    U_ASSERT(!dest->hasDeleter());
    எனில் (fType == kind) {
        dest->addElement(இது, status);
    }
    எனில் (f_இடது_Child != nullptr) {
        f_இடது_Child->findNodes(dest, kind, status);
    }
    எனில் (f_வலது_Child != nullptr) {
        f_வலது_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

நிலையான int32_t serial(மாறிலி RBBINode *முனை) {
    திரும்பு (முனை == nullptr? -1 : முனை->f_Serial_இலக்கம்);
}


வெறுமை RBBINode::print_முனை(மாறிலி RBBINode *முனை) {
    நிலையான மாறிலி எழுத்து * மாறிலி முனை_Type_Names[] = {
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

    எனில் (முனை==nullptr) {
        RBBIDebugPrintf("%10p", (வெறுமை *)முனை);
    } இல்லையெனில் {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (வெறுமை *)முனை, முனை->f_Serial_இலக்கம், முனை_Type_Names[முனை->fType],
            முனை->f_Rule_வேர்?'R':' ', முனை->fChainIn?'C':' ',
            serial(முனை->f_இடது_Child), serial(முனை->f_வலது_Child), serial(முனை->fParent),
            முனை->fFirstPos, முனை->fVal);
        எனில் (முனை->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(முனை->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC வெறுமை RBBI_DEBUG_printUnicodeString(மாறிலி UnicodeString &s, முழு minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
வெறுமை RBBINode::print_முனை_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
வெறுமை RBBINode::print_மரம்(மாறிலி RBBINode *முனை, UBool printHeading) {
    எனில் (printHeading) {
        print_முனை_Header();
    }
    print_முனை(முனை);
    எனில் (முனை != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        எனில் (முனை->fType != varRef) {
            எனில் (முனை->f_இடது_Child != nullptr) {
                print_மரம்(முனை->f_இடது_Child, பொய்);
            }
            
            எனில் (முனை->f_வலது_Child != nullptr) {
                print_மரம்(முனை->f_வலது_Child, பொய்);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
