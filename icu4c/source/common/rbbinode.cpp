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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/parsepos.h"

#అన్తర్భావ "cstr.h"
#అన్తర్భావ "uvector.h"

#అన్తర్భావ "rbbirb.h"
#అన్తర్భావ "rbbinode.h"

#అన్తర్భావ "uassert.h"


U_NAMESPACE_BEGIN

#నిర్వచితమైతే RBBI_DEBUG
స్థిర పూర్ణాంకం  g_చివరి_Serial = 0;
#అయితే_ముగింపు


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(కణుపు_Type t, UError_సంకేతం& స్థితి_2) : UMemory() {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
#నిర్వచితమైతే RBBI_DEBUG
    f_Serial_అంకె    = ++g_చివరి_Serial;
#అయితే_ముగింపు
    fType         = t;
    f_మాతృ       = శూన్యసూచిక;
    f_ఎడమ_బిడ్డ    = శూన్యసూచిక;
    f_కుడి_బిడ్డ   = శూన్యసూచిక;
    f_ఆదానం_సమితి     = శూన్యసూచిక;
    f_మొదటి_Pos     = 0;
    f_చివరి_Pos      = 0;
    fNullable     = అబద్ధం;
    f_Look_Ahead_చివర = అబద్ధం;
    f_నియమం_మూలం     = అబద్ధం;
    f_Chain_లోపల      = అబద్ధం;
    fVal          = 0;
    fPrecedence   = precZero;

    f_మొదటి_Pos_సమితి  = కొత్త UVector(స్థితి_2);
    f_చివరి_Pos_సమితి   = కొత్త UVector(స్థితి_2);
    fFollowPos    = కొత్త UVector(స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2) &&
        (f_మొదటి_Pos_సమితి == శూన్యసూచిక || f_చివరి_Pos_సమితి == శూన్యసూచిక || fFollowPos == శూన్యసూచిక)) {
        స్థితి_2 =  U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ      (t==opCat)    {fPrecedence = precOpCat;}
    లేకపోతే ఒకవేళ (t==opOr)     {fPrecedence = precOpOr;}
    లేకపోతే ఒకవేళ (t==op_ప్రారంభం)  {fPrecedence = prec_ప్రారంభం;}
    లేకపోతే ఒకవేళ (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(స్థిరరాశి RBBINode &other, UError_సంకేతం& స్థితి_2) : UMemory(other) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
#నిర్వచితమైతే RBBI_DEBUG
    f_Serial_అంకె   = ++g_చివరి_Serial;
#అయితే_ముగింపు
    fType        = other.fType;
    f_మాతృ      = శూన్యసూచిక;
    f_ఎడమ_బిడ్డ   = శూన్యసూచిక;
    f_కుడి_బిడ్డ  = శూన్యసూచిక;
    f_ఆదానం_సమితి    = other.f_ఆదానం_సమితి;
    fPrecedence  = other.fPrecedence;
    f_పాఠ్యం        = other.f_పాఠ్యం;
    f_మొదటి_Pos    = other.f_మొదటి_Pos;
    f_చివరి_Pos     = other.f_చివరి_Pos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_నియమం_మూలం    = అబద్ధం;
    f_Chain_లోపల     = other.f_Chain_లోపల;
    f_మొదటి_Pos_సమితి = కొత్త UVector(స్థితి_2);   // TODO - get a real status from somewhere
    f_చివరి_Pos_సమితి  = కొత్త UVector(స్థితి_2);
    fFollowPos   = కొత్త UVector(స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2) &&
        (f_మొదటి_Pos_సమితి == శూన్యసూచిక || f_చివరి_Pos_సమితి == శూన్యసూచిక || fFollowPos == శూన్యసూచిక)) {
        స్థితి_2 =  U_జ్ఞాపకం_కేటాయింపు_ERROR;
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
    తొలగించు f_ఆదానం_సమితి;
    f_ఆదానం_సమితి = శూన్యసూచిక;

    ఎంపిక (ఇది->fType) {
    స్థితి varRef:
    స్థితి సమితి_Ref:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        ఆపు;

    అప్రమేయ:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_కణుపు(f_ఎడమ_బిడ్డ);
        f_ఎడమ_బిడ్డ =   శూన్యసూచిక;
        // delete        fRightChild;
        NRDelete_కణుపు(f_కుడి_బిడ్డ);
        f_కుడి_బిడ్డ = శూన్యసూచిక;
    }

    తొలగించు f_మొదటి_Pos_సమితి;
    తొలగించు f_చివరి_Pos_సమితి;
    తొలగించు fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
శూన్య RBBINode::NRDelete_కణుపు(RBBINode *కణుపు) {
    ఒకవేళ (కణుపు == శూన్యసూచిక) {
        తిరిగి;
    }

    RBBINode *stop_కణుపు = కణుపు->f_మాతృ;
    RBBINode *తదుపరి_కణుపు = కణుపు;
    అయితే (తదుపరి_కణుపు != stop_కణుపు && తదుపరి_కణుపు != శూన్యసూచిక) {
        RBBINode *current_కణుపు = తదుపరి_కణుపు;

        ఒకవేళ ((current_కణుపు->f_ఎడమ_బిడ్డ == శూన్యసూచిక && current_కణుపు->f_కుడి_బిడ్డ == శూన్యసూచిక) ||
                current_కణుపు->fType == varRef ||      // varRef and setRef nodes do not
                current_కణుపు->fType == సమితి_Ref) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            తదుపరి_కణుపు = current_కణుపు->f_మాతృ;
            ఒకవేళ (తదుపరి_కణుపు) {
                ఒకవేళ (తదుపరి_కణుపు->f_ఎడమ_బిడ్డ == current_కణుపు) {
                    తదుపరి_కణుపు->f_ఎడమ_బిడ్డ = శూన్యసూచిక;
                } లేకపోతే ఒకవేళ (తదుపరి_కణుపు->f_కుడి_బిడ్డ == current_కణుపు) {
                    తదుపరి_కణుపు->f_కుడి_బిడ్డ = శూన్యసూచిక;
                }
            }
            తొలగించు current_కణుపు;
        } లేకపోతే ఒకవేళ (current_కణుపు->f_ఎడమ_బిడ్డ) {
            తదుపరి_కణుపు = current_కణుపు->f_ఎడమ_బిడ్డ;
            ఒకవేళ (తదుపరి_కణుపు->f_మాతృ == శూన్యసూచిక) {
                తదుపరి_కణుపు->f_మాతృ = current_కణుపు;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(తదుపరి_కణుపు->f_మాతృ == current_కణుపు);
        } లేకపోతే ఒకవేళ (current_కణుపు->f_కుడి_బిడ్డ) {
            తదుపరి_కణుపు = current_కణుపు->f_కుడి_బిడ్డ;
            ఒకవేళ (తదుపరి_కణుపు->f_మాతృ == శూన్యసూచిక) {
                తదుపరి_కణుపు->f_మాతృ = current_కణుపు;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(తదుపరి_కణుపు->f_మాతృ == current_కణుపు);
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
constexpr పూర్ణాంకం k_Recursive_లోతు_హద్దు = 3500;
RBBINode *RBBINode::clone_వృక్షం(UError_సంకేతం &స్థితి_2, పూర్ణాంకం లోతు) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ఒకవేళ (లోతు > k_Recursive_లోతు_హద్దు) {
        స్థితి_2 = U_ఆదానం_TOO_LONG_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    RBBINode    *n;

    ఒకవేళ (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_ఎడమ_బిడ్డ->clone_వృక్షం(స్థితి_2, లోతు+1);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి శూన్యసూచిక;
        }
    } లేకపోతే ఒకవేళ (fType == RBBINode::uset) {
        n = ఇది;
    } లేకపోతే {
        n = కొత్త RBBINode(*ఇది, స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తొలగించు n;
            తిరిగి శూన్యసూచిక;
        }
        // Check for null pointer.
        ఒకవేళ (n == శూన్యసూచిక) {
            స్థితి_2 =  U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        ఒకవేళ (f_ఎడమ_బిడ్డ != శూన్యసూచిక) {
            n->f_ఎడమ_బిడ్డ          = f_ఎడమ_బిడ్డ->clone_వృక్షం(స్థితి_2, లోతు+1);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తొలగించు n;
                తిరిగి శూన్యసూచిక;
            }
            n->f_ఎడమ_బిడ్డ->f_మాతృ = n;
        }
        ఒకవేళ (f_కుడి_బిడ్డ != శూన్యసూచిక) {
            n->f_కుడి_బిడ్డ          = f_కుడి_బిడ్డ->clone_వృక్షం(స్థితి_2, లోతు+1);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తొలగించు n;
                తిరిగి శూన్యసూచిక;
            }
            n->f_కుడి_బిడ్డ->f_మాతృ = n;
        }
    }
    తిరిగి n;
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
RBBINode *RBBINode::flattenVariables(UError_సంకేతం& స్థితి_2, పూర్ణాంకం లోతు) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి ఇది;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ఒకవేళ (లోతు > k_Recursive_లోతు_హద్దు) {
        స్థితి_2 = U_ఆదానం_TOO_LONG_ERROR;
        తిరిగి ఇది;
    }
    ఒకవేళ (fType == varRef) {
        RBBINode *ret_కణుపు  = f_ఎడమ_బిడ్డ->clone_వృక్షం(స్థితి_2, లోతు+1);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి ఇది;
        }
        ret_కణుపు->f_నియమం_మూలం = ఇది->f_నియమం_మూలం;
        ret_కణుపు->f_Chain_లోపల  = ఇది->f_Chain_లోపల;
        తొలగించు ఇది;   // TODO: undefined behavior. Fix.
        తిరిగి ret_కణుపు;
    }

    ఒకవేళ (f_ఎడమ_బిడ్డ != శూన్యసూచిక) {
        f_ఎడమ_బిడ్డ = f_ఎడమ_బిడ్డ->flattenVariables(స్థితి_2, లోతు+1);
        ఒకవేళ (f_ఎడమ_బిడ్డ == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి ఇది;
        }
        f_ఎడమ_బిడ్డ->f_మాతృ  = ఇది;
    }
    ఒకవేళ (f_కుడి_బిడ్డ != శూన్యసూచిక) {
        f_కుడి_బిడ్డ = f_కుడి_బిడ్డ->flattenVariables(స్థితి_2, లోతు+1);
        ఒకవేళ (f_కుడి_బిడ్డ == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి ఇది;
        }
        f_కుడి_బిడ్డ->f_మాతృ = ఇది;
    }
    తిరిగి ఇది;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
శూన్య RBBINode::flattenSets(UError_సంకేతం &స్థితి_2, పూర్ణాంకం లోతు) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ఒకవేళ (లోతు > k_Recursive_లోతు_హద్దు) {
        స్థితి_2 = U_ఆదానం_TOO_LONG_ERROR;
        తిరిగి;
    }
    U_ASSERT(fType != సమితి_Ref);

    ఒకవేళ (f_ఎడమ_బిడ్డ != శూన్యసూచిక) {
        ఒకవేళ (f_ఎడమ_బిడ్డ->fType==సమితి_Ref) {
            RBBINode *సమితి_Ref_కణుపు = f_ఎడమ_బిడ్డ;
            RBBINode *uset_కణుపు   = సమితి_Ref_కణుపు->f_ఎడమ_బిడ్డ;
            RBBINode *repl_వృక్షం   = uset_కణుపు->f_ఎడమ_బిడ్డ;
            f_ఎడమ_బిడ్డ           = repl_వృక్షం->clone_వృక్షం(స్థితి_2, లోతు+1);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తొలగించు సమితి_Ref_కణుపు;
                తిరిగి;
            }
            f_ఎడమ_బిడ్డ->f_మాతృ  = ఇది;
            తొలగించు సమితి_Ref_కణుపు;
        } లేకపోతే {
            f_ఎడమ_బిడ్డ->flattenSets(స్థితి_2, లోతు+1);
        }
    }

    ఒకవేళ (f_కుడి_బిడ్డ != శూన్యసూచిక) {
        ఒకవేళ (f_కుడి_బిడ్డ->fType==సమితి_Ref) {
            RBBINode *సమితి_Ref_కణుపు = f_కుడి_బిడ్డ;
            RBBINode *uset_కణుపు   = సమితి_Ref_కణుపు->f_ఎడమ_బిడ్డ;
            RBBINode *repl_వృక్షం   = uset_కణుపు->f_ఎడమ_బిడ్డ;
            f_కుడి_బిడ్డ           = repl_వృక్షం->clone_వృక్షం(స్థితి_2, లోతు+1);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                తొలగించు సమితి_Ref_కణుపు;
                తిరిగి;
            }
            f_కుడి_బిడ్డ->f_మాతృ  = ఇది;
            తొలగించు సమితి_Ref_కణుపు;
        } లేకపోతే {
            f_కుడి_బిడ్డ->flattenSets(స్థితి_2, లోతు+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
శూన్య   RBBINode::కనుగొను_Nodes(UVector *dest, RBBINode::కణుపు_Type రకం, UError_సంకేతం &స్థితి_2) {
    /* test for buffer overflows */
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    U_ASSERT(!dest->hasDeleter());
    ఒకవేళ (fType == రకం) {
        dest->కూడు_Element(ఇది, స్థితి_2);
    }
    ఒకవేళ (f_ఎడమ_బిడ్డ != శూన్యసూచిక) {
        f_ఎడమ_బిడ్డ->కనుగొను_Nodes(dest, రకం, స్థితి_2);
    }
    ఒకవేళ (f_కుడి_బిడ్డ != శూన్యసూచిక) {
        f_కుడి_బిడ్డ->కనుగొను_Nodes(dest, రకం, స్థితి_2);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG

స్థిర int32_t serial(స్థిరరాశి RBBINode *కణుపు) {
    తిరిగి (కణుపు == శూన్యసూచిక? -1 : కణుపు->f_Serial_అంకె);
}


శూన్య RBBINode::print_కణుపు(స్థిరరాశి RBBINode *కణుపు) {
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి కణుపు_Type_పేర్లు[] = {
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

    ఒకవేళ (కణుపు==శూన్యసూచిక) {
        RBBIDebugPrintf("%10p", (శూన్య *)కణుపు);
    } లేకపోతే {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (శూన్య *)కణుపు, కణుపు->f_Serial_అంకె, కణుపు_Type_పేర్లు[కణుపు->fType],
            కణుపు->f_నియమం_మూలం?'R':' ', కణుపు->f_Chain_లోపల?'C':' ',
            serial(కణుపు->f_ఎడమ_బిడ్డ), serial(కణుపు->f_కుడి_బిడ్డ), serial(కణుపు->f_మాతృ),
            కణుపు->f_మొదటి_Pos, కణుపు->fVal);
        ఒకవేళ (కణుపు->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(కణుపు->f_పాఠ్యం);
        }
    }
    RBBIDebugPrintf("\n");
}
#అయితే_ముగింపు


#నిర్వచితమైతే RBBI_DEBUG
U_CFUNC శూన్య RBBI_DEBUG_printUnicodeString(స్థిరరాశి UnicodeString &s, పూర్ణాంకం min_వెడల్పు) {
    RBBIDebugPrintf("%*s", min_వెడల్పు, CStr(s)());
}
#అయితే_ముగింపు


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBINode::print_కణుపు_శీర్షిక() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
శూన్య RBBINode::print_వృక్షం(స్థిరరాశి RBBINode *కణుపు, UBool printHeading) {
    ఒకవేళ (printHeading) {
        print_కణుపు_శీర్షిక();
    }
    print_కణుపు(కణుపు);
    ఒకవేళ (కణుపు != శూన్యసూచిక) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        ఒకవేళ (కణుపు->fType != varRef) {
            ఒకవేళ (కణుపు->f_ఎడమ_బిడ్డ != శూన్యసూచిక) {
                print_వృక్షం(కణుపు->f_ఎడమ_బిడ్డ, అబద్ధం);
            }
            
            ఒకవేళ (కణుపు->f_కుడి_బిడ్డ != శూన్యసూచిక) {
                print_వృక్షం(కణుపు->f_కుడి_బిడ్డ, అబద్ధం);
            }
        }
    }
}
#అయితే_ముగింపు



U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
