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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/parsepos.h"

#അന്തര്ഭാവ "cstr.h"
#അന്തര്ഭാവ "uvector.h"

#അന്തര്ഭാവ "rbbirb.h"
#അന്തര്ഭാവ "rbbinode.h"

#അന്തര്ഭാവ "uassert.h"


U_NAMESPACE_BEGIN

#ifdef RBBI_DEBUG
സ്ഥിതം പൂർണ്ണസംഖ്യ  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(കണ്ണി_Type t, UErrorCode& status) : UMemory() {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
#ifdef RBBI_DEBUG
    f_Serial_അക്കം    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_ഇടത്_Child    = nullptr;
    f_വലത്_Child   = nullptr;
    f_നിവേശം_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = അസത്യം;
    fLookAheadEnd = അസത്യം;
    f_Rule_വേര്     = അസത്യം;
    fChainIn      = അസത്യം;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = പുതിയ UVector(status);
    fLastPosSet   = പുതിയ UVector(status);
    fFollowPos    = പുതിയ UVector(status);
    എങ്കിൽ (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ      (t==opCat)    {fPrecedence = precOpCat;}
    അല്ലെങ്കിൽ എങ്കിൽ (t==opOr)     {fPrecedence = precOpOr;}
    അല്ലെങ്കിൽ എങ്കിൽ (t==opStart)  {fPrecedence = precStart;}
    അല്ലെങ്കിൽ എങ്കിൽ (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(സ്ഥിരം RBBINode &other, UErrorCode& status) : UMemory(other) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
#ifdef RBBI_DEBUG
    f_Serial_അക്കം   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_ഇടത്_Child   = nullptr;
    f_വലത്_Child  = nullptr;
    f_നിവേശം_Set    = other.f_നിവേശം_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_വേര്    = അസത്യം;
    fChainIn     = other.fChainIn;
    fFirstPosSet = പുതിയ UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = പുതിയ UVector(status);
    fFollowPos   = പുതിയ UVector(status);
    എങ്കിൽ (U_SUCCESS(status) &&
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
    delete f_നിവേശം_Set;
    f_നിവേശം_Set = nullptr;

    തിരഞ്ഞെടുപ്പ് (ഇത്->fType) {
    അവസ്ഥ varRef:
    അവസ്ഥ setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_കണ്ണി(f_ഇടത്_Child);
        f_ഇടത്_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_കണ്ണി(f_വലത്_Child);
        f_വലത്_Child = nullptr;
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
ശൂന്യം RBBINode::NRDelete_കണ്ണി(RBBINode *കണ്ണി) {
    എങ്കിൽ (കണ്ണി == nullptr) {
        തിരികെ;
    }

    RBBINode *stop_കണ്ണി = കണ്ണി->fParent;
    RBBINode *next_കണ്ണി = കണ്ണി;
    ആകുമ്പോൾ (next_കണ്ണി != stop_കണ്ണി && next_കണ്ണി != nullptr) {
        RBBINode *current_കണ്ണി = next_കണ്ണി;

        എങ്കിൽ ((current_കണ്ണി->f_ഇടത്_Child == nullptr && current_കണ്ണി->f_വലത്_Child == nullptr) ||
                current_കണ്ണി->fType == varRef ||      // varRef and setRef nodes do not
                current_കണ്ണി->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_കണ്ണി = current_കണ്ണി->fParent;
            എങ്കിൽ (next_കണ്ണി) {
                എങ്കിൽ (next_കണ്ണി->f_ഇടത്_Child == current_കണ്ണി) {
                    next_കണ്ണി->f_ഇടത്_Child = nullptr;
                } അല്ലെങ്കിൽ എങ്കിൽ (next_കണ്ണി->f_വലത്_Child == current_കണ്ണി) {
                    next_കണ്ണി->f_വലത്_Child = nullptr;
                }
            }
            delete current_കണ്ണി;
        } അല്ലെങ്കിൽ എങ്കിൽ (current_കണ്ണി->f_ഇടത്_Child) {
            next_കണ്ണി = current_കണ്ണി->f_ഇടത്_Child;
            എങ്കിൽ (next_കണ്ണി->fParent == nullptr) {
                next_കണ്ണി->fParent = current_കണ്ണി;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_കണ്ണി->fParent == current_കണ്ണി);
        } അല്ലെങ്കിൽ എങ്കിൽ (current_കണ്ണി->f_വലത്_Child) {
            next_കണ്ണി = current_കണ്ണി->f_വലത്_Child;
            എങ്കിൽ (next_കണ്ണി->fParent == nullptr) {
                next_കണ്ണി->fParent = current_കണ്ണി;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_കണ്ണി->fParent == current_കണ്ണി);
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
constexpr പൂർണ്ണസംഖ്യ kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_വൃക്ഷം(UErrorCode &status, പൂർണ്ണസംഖ്യ depth) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    എങ്കിൽ (depth > kRecursiveDepthLimit) {
        status = U_നിവേശം_TOO_LONG_ERROR;
        തിരികെ nullptr;
    }
    RBBINode    *n;

    എങ്കിൽ (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_ഇടത്_Child->clone_വൃക്ഷം(status, depth+1);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ nullptr;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (fType == RBBINode::uset) {
        n = ഇത്;
    } അല്ലെങ്കിൽ {
        n = പുതിയ RBBINode(*ഇത്, status);
        എങ്കിൽ (U_FAILURE(status)) {
            delete n;
            തിരികെ nullptr;
        }
        // Check for null pointer.
        എങ്കിൽ (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            തിരികെ nullptr;
        }
        എങ്കിൽ (f_ഇടത്_Child != nullptr) {
            n->f_ഇടത്_Child          = f_ഇടത്_Child->clone_വൃക്ഷം(status, depth+1);
            എങ്കിൽ (U_FAILURE(status)) {
                delete n;
                തിരികെ nullptr;
            }
            n->f_ഇടത്_Child->fParent = n;
        }
        എങ്കിൽ (f_വലത്_Child != nullptr) {
            n->f_വലത്_Child          = f_വലത്_Child->clone_വൃക്ഷം(status, depth+1);
            എങ്കിൽ (U_FAILURE(status)) {
                delete n;
                തിരികെ nullptr;
            }
            n->f_വലത്_Child->fParent = n;
        }
    }
    തിരികെ n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, പൂർണ്ണസംഖ്യ depth) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ ഇത്;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    എങ്കിൽ (depth > kRecursiveDepthLimit) {
        status = U_നിവേശം_TOO_LONG_ERROR;
        തിരികെ ഇത്;
    }
    എങ്കിൽ (fType == varRef) {
        RBBINode *ret_കണ്ണി  = f_ഇടത്_Child->clone_വൃക്ഷം(status, depth+1);
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ ഇത്;
        }
        ret_കണ്ണി->f_Rule_വേര് = ഇത്->f_Rule_വേര്;
        ret_കണ്ണി->fChainIn  = ഇത്->fChainIn;
        delete ഇത്;   // TODO: undefined behavior. Fix.
        തിരികെ ret_കണ്ണി;
    }

    എങ്കിൽ (f_ഇടത്_Child != nullptr) {
        f_ഇടത്_Child = f_ഇടത്_Child->flattenVariables(status, depth+1);
        എങ്കിൽ (f_ഇടത്_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ ഇത്;
        }
        f_ഇടത്_Child->fParent  = ഇത്;
    }
    എങ്കിൽ (f_വലത്_Child != nullptr) {
        f_വലത്_Child = f_വലത്_Child->flattenVariables(status, depth+1);
        എങ്കിൽ (f_വലത്_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        എങ്കിൽ (U_FAILURE(status)) {
            തിരികെ ഇത്;
        }
        f_വലത്_Child->fParent = ഇത്;
    }
    തിരികെ ഇത്;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
ശൂന്യം RBBINode::flattenSets(UErrorCode &status, പൂർണ്ണസംഖ്യ depth) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    എങ്കിൽ (depth > kRecursiveDepthLimit) {
        status = U_നിവേശം_TOO_LONG_ERROR;
        തിരികെ;
    }
    U_ASSERT(fType != setRef);

    എങ്കിൽ (f_ഇടത്_Child != nullptr) {
        എങ്കിൽ (f_ഇടത്_Child->fType==setRef) {
            RBBINode *set_Ref_കണ്ണി = f_ഇടത്_Child;
            RBBINode *uset_കണ്ണി   = set_Ref_കണ്ണി->f_ഇടത്_Child;
            RBBINode *repl_വൃക്ഷം   = uset_കണ്ണി->f_ഇടത്_Child;
            f_ഇടത്_Child           = repl_വൃക്ഷം->clone_വൃക്ഷം(status, depth+1);
            എങ്കിൽ (U_FAILURE(status)) {
                delete set_Ref_കണ്ണി;
                തിരികെ;
            }
            f_ഇടത്_Child->fParent  = ഇത്;
            delete set_Ref_കണ്ണി;
        } അല്ലെങ്കിൽ {
            f_ഇടത്_Child->flattenSets(status, depth+1);
        }
    }

    എങ്കിൽ (f_വലത്_Child != nullptr) {
        എങ്കിൽ (f_വലത്_Child->fType==setRef) {
            RBBINode *set_Ref_കണ്ണി = f_വലത്_Child;
            RBBINode *uset_കണ്ണി   = set_Ref_കണ്ണി->f_ഇടത്_Child;
            RBBINode *repl_വൃക്ഷം   = uset_കണ്ണി->f_ഇടത്_Child;
            f_വലത്_Child           = repl_വൃക്ഷം->clone_വൃക്ഷം(status, depth+1);
            എങ്കിൽ (U_FAILURE(status)) {
                delete set_Ref_കണ്ണി;
                തിരികെ;
            }
            f_വലത്_Child->fParent  = ഇത്;
            delete set_Ref_കണ്ണി;
        } അല്ലെങ്കിൽ {
            f_വലത്_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
ശൂന്യം   RBBINode::findNodes(UVector *dest, RBBINode::കണ്ണി_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    U_ASSERT(!dest->hasDeleter());
    എങ്കിൽ (fType == kind) {
        dest->addElement(ഇത്, status);
    }
    എങ്കിൽ (f_ഇടത്_Child != nullptr) {
        f_ഇടത്_Child->findNodes(dest, kind, status);
    }
    എങ്കിൽ (f_വലത്_Child != nullptr) {
        f_വലത്_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

സ്ഥിതം int32_t serial(സ്ഥിരം RBBINode *കണ്ണി) {
    തിരികെ (കണ്ണി == nullptr? -1 : കണ്ണി->f_Serial_അക്കം);
}


ശൂന്യം RBBINode::print_കണ്ണി(സ്ഥിരം RBBINode *കണ്ണി) {
    സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം കണ്ണി_Type_Names[] = {
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

    എങ്കിൽ (കണ്ണി==nullptr) {
        RBBIDebugPrintf("%10p", (ശൂന്യം *)കണ്ണി);
    } അല്ലെങ്കിൽ {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (ശൂന്യം *)കണ്ണി, കണ്ണി->f_Serial_അക്കം, കണ്ണി_Type_Names[കണ്ണി->fType],
            കണ്ണി->f_Rule_വേര്?'R':' ', കണ്ണി->fChainIn?'C':' ',
            serial(കണ്ണി->f_ഇടത്_Child), serial(കണ്ണി->f_വലത്_Child), serial(കണ്ണി->fParent),
            കണ്ണി->fFirstPos, കണ്ണി->fVal);
        എങ്കിൽ (കണ്ണി->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(കണ്ണി->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC ശൂന്യം RBBI_DEBUG_printUnicodeString(സ്ഥിരം UnicodeString &s, പൂർണ്ണസംഖ്യ minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ശൂന്യം RBBINode::print_കണ്ണി_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
ശൂന്യം RBBINode::print_വൃക്ഷം(സ്ഥിരം RBBINode *കണ്ണി, UBool printHeading) {
    എങ്കിൽ (printHeading) {
        print_കണ്ണി_Header();
    }
    print_കണ്ണി(കണ്ണി);
    എങ്കിൽ (കണ്ണി != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        എങ്കിൽ (കണ്ണി->fType != varRef) {
            എങ്കിൽ (കണ്ണി->f_ഇടത്_Child != nullptr) {
                print_വൃക്ഷം(കണ്ണി->f_ഇടത്_Child, അസത്യം);
            }
            
            എങ്കിൽ (കണ്ണി->f_വലത്_Child != nullptr) {
                print_വൃക്ഷം(കണ്ണി->f_വലത്_Child, അസത്യം);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
