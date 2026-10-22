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

#यदि_परिभाषितम् RBBI_DEBUG
स्थिर पूर्णाङ्क  g_चरमः_Serial = 0;
#यदि_अन्तः


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(ग्रन्थिः_Type t, UError_संहिता& स्थितिः) : UMemory() {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
#यदि_परिभाषितम् RBBI_DEBUG
    f_Serial_अङ्कः    = ++g_चरमः_Serial;
#यदि_अन्तः
    fType         = t;
    fParent       = शून्यसूचकः;
    f_वामः_Child    = शून्यसूचकः;
    f_दक्षिणः_Child   = शून्यसूचकः;
    f_निवेशः_समुच्चयः     = शून्यसूचकः;
    fFirstPos     = 0;
    f_चरमः_Pos      = 0;
    fNullable     = असत्य;
    f_Look_Ahead_अन्तः = असत्य;
    f_नियमः_मूलम्     = असत्य;
    f_Chain_अन्तर्      = असत्य;
    fVal          = 0;
    fPrecedence   = precZero;

    f_First_Pos_समुच्चयः  = नूतन UVector(स्थितिः);
    f_चरमः_Pos_समुच्चयः   = नूतन UVector(स्थितिः);
    fFollowPos    = नूतन UVector(स्थितिः);
    यदि (U_SUCCESS(स्थितिः) &&
        (f_First_Pos_समुच्चयः == शून्यसूचकः || f_चरमः_Pos_समुच्चयः == शून्यसूचकः || fFollowPos == शून्यसूचकः)) {
        स्थितिः =  U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि      (t==opCat)    {fPrecedence = precOpCat;}
    अन्यथा यदि (t==opOr)     {fPrecedence = precOpOr;}
    अन्यथा यदि (t==opStart)  {fPrecedence = precStart;}
    अन्यथा यदि (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ध्रुव RBBINode &other, UError_संहिता& स्थितिः) : UMemory(other) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
#यदि_परिभाषितम् RBBI_DEBUG
    f_Serial_अङ्कः   = ++g_चरमः_Serial;
#यदि_अन्तः
    fType        = other.fType;
    fParent      = शून्यसूचकः;
    f_वामः_Child   = शून्यसूचकः;
    f_दक्षिणः_Child  = शून्यसूचकः;
    f_निवेशः_समुच्चयः    = other.f_निवेशः_समुच्चयः;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    f_चरमः_Pos     = other.f_चरमः_Pos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_नियमः_मूलम्    = असत्य;
    f_Chain_अन्तर्     = other.f_Chain_अन्तर्;
    f_First_Pos_समुच्चयः = नूतन UVector(स्थितिः);   // TODO - get a real status from somewhere
    f_चरमः_Pos_समुच्चयः  = नूतन UVector(स्थितिः);
    fFollowPos   = नूतन UVector(स्थितिः);
    यदि (U_SUCCESS(स्थितिः) &&
        (f_First_Pos_समुच्चयः == शून्यसूचकः || f_चरमः_Pos_समुच्चयः == शून्यसूचकः || fFollowPos == शून्यसूचकः)) {
        स्थितिः =  U_स्मृतिः_आवण्टनम्_ERROR;
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
    विलोपय f_निवेशः_समुच्चयः;
    f_निवेशः_समुच्चयः = शून्यसूचकः;

    विकल्प (एतत्->fType) {
    पक्ष varRef:
    पक्ष समुच्चयः_Ref:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        भङ्ग;

    स्वभाव:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_ग्रन्थिः(f_वामः_Child);
        f_वामः_Child =   शून्यसूचकः;
        // delete        fRightChild;
        NRDelete_ग्रन्थिः(f_दक्षिणः_Child);
        f_दक्षिणः_Child = शून्यसूचकः;
    }

    विलोपय f_First_Pos_समुच्चयः;
    विलोपय f_चरमः_Pos_समुच्चयः;
    विलोपय fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
शून्य RBBINode::NRDelete_ग्रन्थिः(RBBINode *ग्रन्थिः) {
    यदि (ग्रन्थिः == शून्यसूचकः) {
        प्रत्यागम;
    }

    RBBINode *stop_ग्रन्थिः = ग्रन्थिः->fParent;
    RBBINode *अग्र_ग्रन्थिः = ग्रन्थिः;
    यावत् (अग्र_ग्रन्थिः != stop_ग्रन्थिः && अग्र_ग्रन्थिः != शून्यसूचकः) {
        RBBINode *current_ग्रन्थिः = अग्र_ग्रन्थिः;

        यदि ((current_ग्रन्थिः->f_वामः_Child == शून्यसूचकः && current_ग्रन्थिः->f_दक्षिणः_Child == शून्यसूचकः) ||
                current_ग्रन्थिः->fType == varRef ||      // varRef and setRef nodes do not
                current_ग्रन्थिः->fType == समुच्चयः_Ref) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            अग्र_ग्रन्थिः = current_ग्रन्थिः->fParent;
            यदि (अग्र_ग्रन्थिः) {
                यदि (अग्र_ग्रन्थिः->f_वामः_Child == current_ग्रन्थिः) {
                    अग्र_ग्रन्थिः->f_वामः_Child = शून्यसूचकः;
                } अन्यथा यदि (अग्र_ग्रन्थिः->f_दक्षिणः_Child == current_ग्रन्थिः) {
                    अग्र_ग्रन्थिः->f_दक्षिणः_Child = शून्यसूचकः;
                }
            }
            विलोपय current_ग्रन्थिः;
        } अन्यथा यदि (current_ग्रन्थिः->f_वामः_Child) {
            अग्र_ग्रन्थिः = current_ग्रन्थिः->f_वामः_Child;
            यदि (अग्र_ग्रन्थिः->fParent == शून्यसूचकः) {
                अग्र_ग्रन्थिः->fParent = current_ग्रन्थिः;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(अग्र_ग्रन्थिः->fParent == current_ग्रन्थिः);
        } अन्यथा यदि (current_ग्रन्थिः->f_दक्षिणः_Child) {
            अग्र_ग्रन्थिः = current_ग्रन्थिः->f_दक्षिणः_Child;
            यदि (अग्र_ग्रन्थिः->fParent == शून्यसूचकः) {
                अग्र_ग्रन्थिः->fParent = current_ग्रन्थिः;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(अग्र_ग्रन्थिः->fParent == current_ग्रन्थिः);
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
constexpr पूर्णाङ्क k_Recursive_गभीरता_सीमा = 3500;
RBBINode *RBBINode::clone_वृक्षः(UError_संहिता &स्थितिः, पूर्णाङ्क गभीरता) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    यदि (गभीरता > k_Recursive_गभीरता_सीमा) {
        स्थितिः = U_निवेशः_TOO_LONG_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    RBBINode    *n;

    यदि (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_वामः_Child->clone_वृक्षः(स्थितिः, गभीरता+1);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम शून्यसूचकः;
        }
    } अन्यथा यदि (fType == RBBINode::uset) {
        n = एतत्;
    } अन्यथा {
        n = नूतन RBBINode(*एतत्, स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            विलोपय n;
            प्रत्यागम शून्यसूचकः;
        }
        // Check for null pointer.
        यदि (n == शून्यसूचकः) {
            स्थितिः =  U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        यदि (f_वामः_Child != शून्यसूचकः) {
            n->f_वामः_Child          = f_वामः_Child->clone_वृक्षः(स्थितिः, गभीरता+1);
            यदि (U_FAILURE(स्थितिः)) {
                विलोपय n;
                प्रत्यागम शून्यसूचकः;
            }
            n->f_वामः_Child->fParent = n;
        }
        यदि (f_दक्षिणः_Child != शून्यसूचकः) {
            n->f_दक्षिणः_Child          = f_दक्षिणः_Child->clone_वृक्षः(स्थितिः, गभीरता+1);
            यदि (U_FAILURE(स्थितिः)) {
                विलोपय n;
                प्रत्यागम शून्यसूचकः;
            }
            n->f_दक्षिणः_Child->fParent = n;
        }
    }
    प्रत्यागम n;
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
RBBINode *RBBINode::flattenVariables(UError_संहिता& स्थितिः, पूर्णाङ्क गभीरता) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम एतत्;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    यदि (गभीरता > k_Recursive_गभीरता_सीमा) {
        स्थितिः = U_निवेशः_TOO_LONG_ERROR;
        प्रत्यागम एतत्;
    }
    यदि (fType == varRef) {
        RBBINode *ret_ग्रन्थिः  = f_वामः_Child->clone_वृक्षः(स्थितिः, गभीरता+1);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम एतत्;
        }
        ret_ग्रन्थिः->f_नियमः_मूलम् = एतत्->f_नियमः_मूलम्;
        ret_ग्रन्थिः->f_Chain_अन्तर्  = एतत्->f_Chain_अन्तर्;
        विलोपय एतत्;   // TODO: undefined behavior. Fix.
        प्रत्यागम ret_ग्रन्थिः;
    }

    यदि (f_वामः_Child != शून्यसूचकः) {
        f_वामः_Child = f_वामः_Child->flattenVariables(स्थितिः, गभीरता+1);
        यदि (f_वामः_Child == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        }
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम एतत्;
        }
        f_वामः_Child->fParent  = एतत्;
    }
    यदि (f_दक्षिणः_Child != शून्यसूचकः) {
        f_दक्षिणः_Child = f_दक्षिणः_Child->flattenVariables(स्थितिः, गभीरता+1);
        यदि (f_दक्षिणः_Child == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        }
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम एतत्;
        }
        f_दक्षिणः_Child->fParent = एतत्;
    }
    प्रत्यागम एतत्;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
शून्य RBBINode::flattenSets(UError_संहिता &स्थितिः, पूर्णाङ्क गभीरता) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    यदि (गभीरता > k_Recursive_गभीरता_सीमा) {
        स्थितिः = U_निवेशः_TOO_LONG_ERROR;
        प्रत्यागम;
    }
    U_ASSERT(fType != समुच्चयः_Ref);

    यदि (f_वामः_Child != शून्यसूचकः) {
        यदि (f_वामः_Child->fType==समुच्चयः_Ref) {
            RBBINode *समुच्चयः_Ref_ग्रन्थिः = f_वामः_Child;
            RBBINode *uset_ग्रन्थिः   = समुच्चयः_Ref_ग्रन्थिः->f_वामः_Child;
            RBBINode *repl_वृक्षः   = uset_ग्रन्थिः->f_वामः_Child;
            f_वामः_Child           = repl_वृक्षः->clone_वृक्षः(स्थितिः, गभीरता+1);
            यदि (U_FAILURE(स्थितिः)) {
                विलोपय समुच्चयः_Ref_ग्रन्थिः;
                प्रत्यागम;
            }
            f_वामः_Child->fParent  = एतत्;
            विलोपय समुच्चयः_Ref_ग्रन्थिः;
        } अन्यथा {
            f_वामः_Child->flattenSets(स्थितिः, गभीरता+1);
        }
    }

    यदि (f_दक्षिणः_Child != शून्यसूचकः) {
        यदि (f_दक्षिणः_Child->fType==समुच्चयः_Ref) {
            RBBINode *समुच्चयः_Ref_ग्रन्थिः = f_दक्षिणः_Child;
            RBBINode *uset_ग्रन्थिः   = समुच्चयः_Ref_ग्रन्थिः->f_वामः_Child;
            RBBINode *repl_वृक्षः   = uset_ग्रन्थिः->f_वामः_Child;
            f_दक्षिणः_Child           = repl_वृक्षः->clone_वृक्षः(स्थितिः, गभीरता+1);
            यदि (U_FAILURE(स्थितिः)) {
                विलोपय समुच्चयः_Ref_ग्रन्थिः;
                प्रत्यागम;
            }
            f_दक्षिणः_Child->fParent  = एतत्;
            विलोपय समुच्चयः_Ref_ग्रन्थिः;
        } अन्यथा {
            f_दक्षिणः_Child->flattenSets(स्थितिः, गभीरता+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
शून्य   RBBINode::findNodes(UVector *dest, RBBINode::ग्रन्थिः_Type विधा, UError_संहिता &स्थितिः) {
    /* test for buffer overflows */
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    U_ASSERT(!dest->hasDeleter());
    यदि (fType == विधा) {
        dest->सङ्कलय_Element(एतत्, स्थितिः);
    }
    यदि (f_वामः_Child != शून्यसूचकः) {
        f_वामः_Child->findNodes(dest, विधा, स्थितिः);
    }
    यदि (f_दक्षिणः_Child != शून्यसूचकः) {
        f_दक्षिणः_Child->findNodes(dest, विधा, स्थितिः);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG

स्थिर int32_t serial(ध्रुव RBBINode *ग्रन्थिः) {
    प्रत्यागम (ग्रन्थिः == शून्यसूचकः? -1 : ग्रन्थिः->f_Serial_अङ्कः);
}


शून्य RBBINode::print_ग्रन्थिः(ध्रुव RBBINode *ग्रन्थिः) {
    स्थिर ध्रुव वर्ण * ध्रुव ग्रन्थिः_Type_नामानि[] = {
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

    यदि (ग्रन्थिः==शून्यसूचकः) {
        RBBIDebugPrintf("%10p", (शून्य *)ग्रन्थिः);
    } अन्यथा {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (शून्य *)ग्रन्थिः, ग्रन्थिः->f_Serial_अङ्कः, ग्रन्थिः_Type_नामानि[ग्रन्थिः->fType],
            ग्रन्थिः->f_नियमः_मूलम्?'R':' ', ग्रन्थिः->f_Chain_अन्तर्?'C':' ',
            serial(ग्रन्थिः->f_वामः_Child), serial(ग्रन्थिः->f_दक्षिणः_Child), serial(ग्रन्थिः->fParent),
            ग्रन्थिः->fFirstPos, ग्रन्थिः->fVal);
        यदि (ग्रन्थिः->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(ग्रन्थिः->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#यदि_अन्तः


#यदि_परिभाषितम् RBBI_DEBUG
U_CFUNC शून्य RBBI_DEBUG_printUnicodeString(ध्रुव UnicodeString &s, पूर्णाङ्क min_विस्तारः) {
    RBBIDebugPrintf("%*s", min_विस्तारः, CStr(s)());
}
#यदि_अन्तः


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBINode::print_ग्रन्थिः_शीर्षकम्() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
शून्य RBBINode::print_वृक्षः(ध्रुव RBBINode *ग्रन्थिः, UBool printHeading) {
    यदि (printHeading) {
        print_ग्रन्थिः_शीर्षकम्();
    }
    print_ग्रन्थिः(ग्रन्थिः);
    यदि (ग्रन्थिः != शून्यसूचकः) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        यदि (ग्रन्थिः->fType != varRef) {
            यदि (ग्रन्थिः->f_वामः_Child != शून्यसूचकः) {
                print_वृक्षः(ग्रन्थिः->f_वामः_Child, असत्य);
            }
            
            यदि (ग्रन्थिः->f_दक्षिणः_Child != शून्यसूचकः) {
                print_वृक्षः(ग्रन्थिः->f_दक्षिणः_Child, असत्य);
            }
        }
    }
}
#यदि_अन्तः



U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
