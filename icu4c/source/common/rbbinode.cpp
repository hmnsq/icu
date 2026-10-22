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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/parsepos.h"

#અન્તર્ભાવ "cstr.h"
#અન્તર્ભાવ "uvector.h"

#અન્તર્ભાવ "rbbirb.h"
#અન્તર્ભાવ "rbbinode.h"

#અન્તર્ભાવ "uassert.h"


U_NAMESPACE_BEGIN

#ifdef RBBI_DEBUG
સ્થિર પૂર્ણાંક  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(ગાંઠ_Type t, UErrorCode& status) : UMemory() {
    જો (U_FAILURE(status)) {
        પાછા;
    }
#ifdef RBBI_DEBUG
    f_Serial_અંક    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_ડાબું_Child    = nullptr;
    f_જમણું_Child   = nullptr;
    f_આગત_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = ખોટું;
    fLookAheadEnd = ખોટું;
    f_Rule_મૂળ     = ખોટું;
    fChainIn      = ખોટું;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = નવું UVector(status);
    fLastPosSet   = નવું UVector(status);
    fFollowPos    = નવું UVector(status);
    જો (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    જો      (t==opCat)    {fPrecedence = precOpCat;}
    નહીંતર જો (t==opOr)     {fPrecedence = precOpOr;}
    નહીંતર જો (t==opStart)  {fPrecedence = precStart;}
    નહીંતર જો (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(અચલ RBBINode &other, UErrorCode& status) : UMemory(other) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
#ifdef RBBI_DEBUG
    f_Serial_અંક   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_ડાબું_Child   = nullptr;
    f_જમણું_Child  = nullptr;
    f_આગત_Set    = other.f_આગત_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_મૂળ    = ખોટું;
    fChainIn     = other.fChainIn;
    fFirstPosSet = નવું UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = નવું UVector(status);
    fFollowPos   = નવું UVector(status);
    જો (U_SUCCESS(status) &&
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
    delete f_આગત_Set;
    f_આગત_Set = nullptr;

    પસંદગી (આ->fType) {
    સ્થિતિ varRef:
    સ્થિતિ setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        તોડો;

    મૂળભૂત:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_ગાંઠ(f_ડાબું_Child);
        f_ડાબું_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_ગાંઠ(f_જમણું_Child);
        f_જમણું_Child = nullptr;
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
શૂન્ય RBBINode::NRDelete_ગાંઠ(RBBINode *ગાંઠ) {
    જો (ગાંઠ == nullptr) {
        પાછા;
    }

    RBBINode *stop_ગાંઠ = ગાંઠ->fParent;
    RBBINode *next_ગાંઠ = ગાંઠ;
    જ્યારે (next_ગાંઠ != stop_ગાંઠ && next_ગાંઠ != nullptr) {
        RBBINode *current_ગાંઠ = next_ગાંઠ;

        જો ((current_ગાંઠ->f_ડાબું_Child == nullptr && current_ગાંઠ->f_જમણું_Child == nullptr) ||
                current_ગાંઠ->fType == varRef ||      // varRef and setRef nodes do not
                current_ગાંઠ->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_ગાંઠ = current_ગાંઠ->fParent;
            જો (next_ગાંઠ) {
                જો (next_ગાંઠ->f_ડાબું_Child == current_ગાંઠ) {
                    next_ગાંઠ->f_ડાબું_Child = nullptr;
                } નહીંતર જો (next_ગાંઠ->f_જમણું_Child == current_ગાંઠ) {
                    next_ગાંઠ->f_જમણું_Child = nullptr;
                }
            }
            delete current_ગાંઠ;
        } નહીંતર જો (current_ગાંઠ->f_ડાબું_Child) {
            next_ગાંઠ = current_ગાંઠ->f_ડાબું_Child;
            જો (next_ગાંઠ->fParent == nullptr) {
                next_ગાંઠ->fParent = current_ગાંઠ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_ગાંઠ->fParent == current_ગાંઠ);
        } નહીંતર જો (current_ગાંઠ->f_જમણું_Child) {
            next_ગાંઠ = current_ગાંઠ->f_જમણું_Child;
            જો (next_ગાંઠ->fParent == nullptr) {
                next_ગાંઠ->fParent = current_ગાંઠ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_ગાંઠ->fParent == current_ગાંઠ);
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
constexpr પૂર્ણાંક kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_વૃક્ષ(UErrorCode &status, પૂર્ણાંક depth) {
    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    જો (depth > kRecursiveDepthLimit) {
        status = U_આગત_TOO_LONG_ERROR;
        પાછા nullptr;
    }
    RBBINode    *n;

    જો (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_ડાબું_Child->clone_વૃક્ષ(status, depth+1);
        જો (U_FAILURE(status)) {
            પાછા nullptr;
        }
    } નહીંતર જો (fType == RBBINode::uset) {
        n = આ;
    } નહીંતર {
        n = નવું RBBINode(*આ, status);
        જો (U_FAILURE(status)) {
            delete n;
            પાછા nullptr;
        }
        // Check for null pointer.
        જો (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            પાછા nullptr;
        }
        જો (f_ડાબું_Child != nullptr) {
            n->f_ડાબું_Child          = f_ડાબું_Child->clone_વૃક્ષ(status, depth+1);
            જો (U_FAILURE(status)) {
                delete n;
                પાછા nullptr;
            }
            n->f_ડાબું_Child->fParent = n;
        }
        જો (f_જમણું_Child != nullptr) {
            n->f_જમણું_Child          = f_જમણું_Child->clone_વૃક્ષ(status, depth+1);
            જો (U_FAILURE(status)) {
                delete n;
                પાછા nullptr;
            }
            n->f_જમણું_Child->fParent = n;
        }
    }
    પાછા n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, પૂર્ણાંક depth) {
    જો (U_FAILURE(status)) {
        પાછા આ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    જો (depth > kRecursiveDepthLimit) {
        status = U_આગત_TOO_LONG_ERROR;
        પાછા આ;
    }
    જો (fType == varRef) {
        RBBINode *ret_ગાંઠ  = f_ડાબું_Child->clone_વૃક્ષ(status, depth+1);
        જો (U_FAILURE(status)) {
            પાછા આ;
        }
        ret_ગાંઠ->f_Rule_મૂળ = આ->f_Rule_મૂળ;
        ret_ગાંઠ->fChainIn  = આ->fChainIn;
        delete આ;   // TODO: undefined behavior. Fix.
        પાછા ret_ગાંઠ;
    }

    જો (f_ડાબું_Child != nullptr) {
        f_ડાબું_Child = f_ડાબું_Child->flattenVariables(status, depth+1);
        જો (f_ડાબું_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        જો (U_FAILURE(status)) {
            પાછા આ;
        }
        f_ડાબું_Child->fParent  = આ;
    }
    જો (f_જમણું_Child != nullptr) {
        f_જમણું_Child = f_જમણું_Child->flattenVariables(status, depth+1);
        જો (f_જમણું_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        જો (U_FAILURE(status)) {
            પાછા આ;
        }
        f_જમણું_Child->fParent = આ;
    }
    પાછા આ;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
શૂન્ય RBBINode::flattenSets(UErrorCode &status, પૂર્ણાંક depth) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    જો (depth > kRecursiveDepthLimit) {
        status = U_આગત_TOO_LONG_ERROR;
        પાછા;
    }
    U_ASSERT(fType != setRef);

    જો (f_ડાબું_Child != nullptr) {
        જો (f_ડાબું_Child->fType==setRef) {
            RBBINode *set_Ref_ગાંઠ = f_ડાબું_Child;
            RBBINode *uset_ગાંઠ   = set_Ref_ગાંઠ->f_ડાબું_Child;
            RBBINode *repl_વૃક્ષ   = uset_ગાંઠ->f_ડાબું_Child;
            f_ડાબું_Child           = repl_વૃક્ષ->clone_વૃક્ષ(status, depth+1);
            જો (U_FAILURE(status)) {
                delete set_Ref_ગાંઠ;
                પાછા;
            }
            f_ડાબું_Child->fParent  = આ;
            delete set_Ref_ગાંઠ;
        } નહીંતર {
            f_ડાબું_Child->flattenSets(status, depth+1);
        }
    }

    જો (f_જમણું_Child != nullptr) {
        જો (f_જમણું_Child->fType==setRef) {
            RBBINode *set_Ref_ગાંઠ = f_જમણું_Child;
            RBBINode *uset_ગાંઠ   = set_Ref_ગાંઠ->f_ડાબું_Child;
            RBBINode *repl_વૃક્ષ   = uset_ગાંઠ->f_ડાબું_Child;
            f_જમણું_Child           = repl_વૃક્ષ->clone_વૃક્ષ(status, depth+1);
            જો (U_FAILURE(status)) {
                delete set_Ref_ગાંઠ;
                પાછા;
            }
            f_જમણું_Child->fParent  = આ;
            delete set_Ref_ગાંઠ;
        } નહીંતર {
            f_જમણું_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
શૂન્ય   RBBINode::findNodes(UVector *dest, RBBINode::ગાંઠ_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    જો (U_FAILURE(status)) {
        પાછા;
    }
    U_ASSERT(!dest->hasDeleter());
    જો (fType == kind) {
        dest->addElement(આ, status);
    }
    જો (f_ડાબું_Child != nullptr) {
        f_ડાબું_Child->findNodes(dest, kind, status);
    }
    જો (f_જમણું_Child != nullptr) {
        f_જમણું_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

સ્થિર int32_t serial(અચલ RBBINode *ગાંઠ) {
    પાછા (ગાંઠ == nullptr? -1 : ગાંઠ->f_Serial_અંક);
}


શૂન્ય RBBINode::print_ગાંઠ(અચલ RBBINode *ગાંઠ) {
    સ્થિર અચલ અક્ષર * અચલ ગાંઠ_Type_Names[] = {
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

    જો (ગાંઠ==nullptr) {
        RBBIDebugPrintf("%10p", (શૂન્ય *)ગાંઠ);
    } નહીંતર {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (શૂન્ય *)ગાંઠ, ગાંઠ->f_Serial_અંક, ગાંઠ_Type_Names[ગાંઠ->fType],
            ગાંઠ->f_Rule_મૂળ?'R':' ', ગાંઠ->fChainIn?'C':' ',
            serial(ગાંઠ->f_ડાબું_Child), serial(ગાંઠ->f_જમણું_Child), serial(ગાંઠ->fParent),
            ગાંઠ->fFirstPos, ગાંઠ->fVal);
        જો (ગાંઠ->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(ગાંઠ->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC શૂન્ય RBBI_DEBUG_printUnicodeString(અચલ UnicodeString &s, પૂર્ણાંક minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
શૂન્ય RBBINode::print_ગાંઠ_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
શૂન્ય RBBINode::print_વૃક્ષ(અચલ RBBINode *ગાંઠ, UBool printHeading) {
    જો (printHeading) {
        print_ગાંઠ_Header();
    }
    print_ગાંઠ(ગાંઠ);
    જો (ગાંઠ != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        જો (ગાંઠ->fType != varRef) {
            જો (ગાંઠ->f_ડાબું_Child != nullptr) {
                print_વૃક્ષ(ગાંઠ->f_ડાબું_Child, ખોટું);
            }
            
            જો (ગાંઠ->f_જમણું_Child != nullptr) {
                print_વૃક્ષ(ગાંઠ->f_જમણું_Child, ખોટું);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
