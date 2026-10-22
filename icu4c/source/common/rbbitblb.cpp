// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (c) 2002-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
//
//  rbbitblb.cpp
//

#अन्तर्भाव <functional>

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "rbbitblb.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "rbbiscan.h"
#अन्तर्भाव "rbbisetb.h"
#अन्तर्भाव "rbbidata.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uvectr32.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_BEGIN

नामाकाश {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
तार्किक reachableByTransitions(ध्रुव UVector &अवस्थाः, ध्रुव int32_t source,
                            ध्रुव std::function<तार्किक(int32_t)> isSink,
                            ध्रुव std::function<तार्किक(int32_t)> excludedState, UError_संहिता &स्थितिः) {
    UStack boundary(स्थितिः);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(अवस्थाः.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        प्रति (int32_t चिह्नम् = 0; चिह्नम् < transitionsFromSource.size(); ++चिह्नम्) {
            ध्रुव int32_t state = transitionsFromSource.elementAti(चिह्नम्);
            यदि (state != 0) {
                boundary.push(state, स्थितिः);
            }
        }
    }
    ध्रुव स्वचालित visited = prv::make_unique<तार्किक[]>(अवस्थाः.size(), स्थितिः);
    यावत् (U_SUCCESS(स्थितिः) && !boundary.empty()) {
        ध्रुव int32_t s = boundary.popi();
        यदि (isSink(s)) {
            प्रत्यागम सत्य;
        }
        यदि (excludedState(s) || visited[s]) {
            अनुवर्त;
        }
        visited[s] = सत्य;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(अवस्थाः.elementAt(s))->fDtran;
        प्रति (int32_t चिह्नम् = 0; चिह्नम् < transitions.size(); ++चिह्नम्) {
            ध्रुव int32_t t = transitions.elementAti(चिह्नम्);
            यदि (t != 0 && !visited[t]) {
                boundary.push(t, स्थितिः);
            }
        }
    }
    प्रत्यागम असत्य;
}

} // namespace

ध्रुव int32_t k_Max_State_For8_द्वयङ्काः_सारणी = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **मूलम्_ग्रन्थिः, UError_संहिता &स्थितिः) :
        fRB(rb),
        f_वृक्षः(*मूलम्_ग्रन्थिः),
        f_स्थितिः(&स्थितिः),
        fDStates(शून्यसूचकः),
        f_Safe_सारणी(शून्यसूचकः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = नूतन UVector(स्थितिः);
    यदि (U_SUCCESS(स्थितिः) && fDStates == शून्यसूचकः ) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    पूर्णाङ्क i;
    प्रति (i=0; i<fDStates->size(); i++) {
        विलोपय static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    विलोपय fDStates;
    विलोपय f_Safe_सारणी;
    विलोपय f_Look_Ahead_नियमः_Map;
}


//-----------------------------------------------------------------------------
//
//   RBBITableBuilder::buildForwardTable  -  This is the main function for building
//                               the DFA state transition table from the RBBI rules parse tree.
//
//-----------------------------------------------------------------------------
शून्य  RBBITableBuilder::build_Forward_सारणी() {

    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    यदि (f_वृक्षः==शून्यसूचकः) {
        प्रत्यागम;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_वृक्षः = f_वृक्षः->flattenVariables(*f_स्थितिः, 0);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
#यदि_परिभाषितम् RBBI_DEBUG
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_वृक्षः(f_वृक्षः, सत्य);
    }
#यदि_अन्तः

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = नूतन RBBINode(RBBINode::opCat, *f_स्थितिः);
    // Exit if memory allocation failed.
    यदि (cn == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि (U_FAILURE(*f_स्थितिः)) {
        विलोपय cn;
        प्रत्यागम;
    }
    cn->f_वामः_Child = f_वृक्षः;
    f_वृक्षः->fParent = cn;
    RBBINode *अन्तः_Marker_ग्रन्थिः = cn->f_दक्षिणः_Child = नूतन RBBINode(RBBINode::अन्तः_Mark, *f_स्थितिः);
    // Delete and exit if memory allocation failed.
    यदि (cn->f_दक्षिणः_Child == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि (U_FAILURE(*f_स्थितिः)) {
        विलोपय cn;
        प्रत्यागम;
    }
    cn->f_दक्षिणः_Child->fParent = cn;
    f_वृक्षः = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_वृक्षः->flattenSets(*f_स्थितिः, 0);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
#यदि_परिभाषितम् RBBI_DEBUG
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_वृक्षः(f_वृक्षः, सत्य);
    }
#यदि_अन्तः


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_वृक्षः);
    calcFirstPos(f_वृक्षः);
    calc_चरमः_Pos(f_वृक्षः);
    calcFollowPos(f_वृक्षः);
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_वृक्षः);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    यदि (fRB->f_Chain_नियमाः) {
        calcChainedFollowPos(f_वृक्षः, अन्तः_Marker_ग्रन्थिः);
    }

    //
    // Build the DFA state transition tables.
    //
    build_State_सारणी();
    map_Look_Ahead_नियमाः();
    flag_Accepting_अवस्थाः();
    flag_Look_Ahead_अवस्थाः();
    flag_Tagged_अवस्थाः();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    merge_नियमः_स्थितिः_Vals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcNullable(RBBINode *n) {
    यदि (n == शून्यसूचकः) {
        प्रत्यागम;
    }
    यदि (n->fType == RBBINode::समुच्चयः_Ref ||
        n->fType == RBBINode::अन्तः_Mark ) {
        // These are non-empty leaf node types.
        n->fNullable = असत्य;
        प्रत्यागम;
    }

    यदि (n->fType == RBBINode::lookAhead || n->fType == RBBINode::चिह्नकम्) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = सत्य;
        प्रत्यागम;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_वामः_Child);
    calcNullable(n->f_दक्षिणः_Child);

    // Apply functions from table 3.40 in Aho
    यदि (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_वामः_Child->fNullable || n->f_दक्षिणः_Child->fNullable;
    }
    अन्यथा यदि (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_वामः_Child->fNullable && n->f_दक्षिणः_Child->fNullable;
    }
    अन्यथा यदि (n->fType == RBBINode::op_तारकम् || n->fType == RBBINode::opQuestion) {
        n->fNullable = सत्य;
    }
    अन्यथा {
        n->fNullable = असत्य;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcFirstPos(RBBINode *n) {
    यदि (n == शून्यसूचकः) {
        प्रत्यागम;
    }
    यदि (n->fType == RBBINode::पर्णम्_Char  ||
        n->fType == RBBINode::अन्तः_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::चिह्नकम्) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_First_Pos_समुच्चयः->सङ्कलय_Element(n, *f_स्थितिः);
        प्रत्यागम;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_वामः_Child);
    calcFirstPos(n->f_दक्षिणः_Child);

    // Apply functions from table 3.40 in Aho
    यदि (n->fType == RBBINode::opOr) {
        समुच्चयः_सङ्कलय_2(n->f_First_Pos_समुच्चयः, n->f_वामः_Child->f_First_Pos_समुच्चयः);
        समुच्चयः_सङ्कलय_2(n->f_First_Pos_समुच्चयः, n->f_दक्षिणः_Child->f_First_Pos_समुच्चयः);
    }
    अन्यथा यदि (n->fType == RBBINode::opCat) {
        समुच्चयः_सङ्कलय_2(n->f_First_Pos_समुच्चयः, n->f_वामः_Child->f_First_Pos_समुच्चयः);
        यदि (n->f_वामः_Child->fNullable) {
            समुच्चयः_सङ्कलय_2(n->f_First_Pos_समुच्चयः, n->f_दक्षिणः_Child->f_First_Pos_समुच्चयः);
        }
    }
    अन्यथा यदि (n->fType == RBBINode::op_तारकम् ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        समुच्चयः_सङ्कलय_2(n->f_First_Pos_समुच्चयः, n->f_वामः_Child->f_First_Pos_समुच्चयः);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calc_चरमः_Pos(RBBINode *n) {
    यदि (n == शून्यसूचकः) {
        प्रत्यागम;
    }
    यदि (n->fType == RBBINode::पर्णम्_Char  ||
        n->fType == RBBINode::अन्तः_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::चिह्नकम्) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_चरमः_Pos_समुच्चयः->सङ्कलय_Element(n, *f_स्थितिः);
        प्रत्यागम;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calc_चरमः_Pos(n->f_वामः_Child);
    calc_चरमः_Pos(n->f_दक्षिणः_Child);

    // Apply functions from table 3.40 in Aho
    यदि (n->fType == RBBINode::opOr) {
        समुच्चयः_सङ्कलय_2(n->f_चरमः_Pos_समुच्चयः, n->f_वामः_Child->f_चरमः_Pos_समुच्चयः);
        समुच्चयः_सङ्कलय_2(n->f_चरमः_Pos_समुच्चयः, n->f_दक्षिणः_Child->f_चरमः_Pos_समुच्चयः);
    }
    अन्यथा यदि (n->fType == RBBINode::opCat) {
        समुच्चयः_सङ्कलय_2(n->f_चरमः_Pos_समुच्चयः, n->f_दक्षिणः_Child->f_चरमः_Pos_समुच्चयः);
        यदि (n->f_दक्षिणः_Child->fNullable) {
            समुच्चयः_सङ्कलय_2(n->f_चरमः_Pos_समुच्चयः, n->f_वामः_Child->f_चरमः_Pos_समुच्चयः);
        }
    }
    अन्यथा यदि (n->fType == RBBINode::op_तारकम्     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        समुच्चयः_सङ्कलय_2(n->f_चरमः_Pos_समुच्चयः, n->f_वामः_Child->f_चरमः_Pos_समुच्चयः);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcFollowPos(RBBINode *n) {
    यदि (n == शून्यसूचकः ||
        n->fType == RBBINode::पर्णम्_Char ||
        n->fType == RBBINode::अन्तः_Mark) {
        प्रत्यागम;
    }

    calcFollowPos(n->f_वामः_Child);
    calcFollowPos(n->f_दक्षिणः_Child);

    // Aho rule #1
    यदि (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *चरमः_Pos_सम्बन्धि_वामः_Child = n->f_वामः_Child->f_चरमः_Pos_समुच्चयः;

        प्रति (ix = 0; ix < static_cast<uint32_t>(चरमः_Pos_सम्बन्धि_वामः_Child->size()); ix++) {
            i = static_cast<RBBINode*>(चरमः_Pos_सम्बन्धि_वामः_Child->elementAt(ix));
            समुच्चयः_सङ्कलय_2(i->fFollowPos, n->f_दक्षिणः_Child->f_First_Pos_समुच्चयः);
        }
    }

    // Aho rule #2
    यदि (n->fType == RBBINode::op_तारकम् ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        प्रति (ix = 0; ix < static_cast<uint32_t>(n->f_चरमः_Pos_समुच्चयः->size()); ix++) {
            i = static_cast<RBBINode*>(n->f_चरमः_Pos_समुच्चयः->elementAt(ix));
            समुच्चयः_सङ्कलय_2(i->fFollowPos, n->f_First_Pos_समुच्चयः);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::सङ्कलय_नियमः_मूलम्_Nodes(UVector *dest, RBBINode *ग्रन्थिः) {
    यदि (ग्रन्थिः == शून्यसूचकः || U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    U_ASSERT(!dest->hasDeleter());
    यदि (ग्रन्थिः->f_नियमः_मूलम्) {
        dest->सङ्कलय_Element(ग्रन्थिः, *f_स्थितिः);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        प्रत्यागम;
    }
    सङ्कलय_नियमः_मूलम्_Nodes(dest, ग्रन्थिः->f_वामः_Child);
    सङ्कलय_नियमः_मूलम्_Nodes(dest, ग्रन्थिः->f_दक्षिणः_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcChainedFollowPos(RBBINode *वृक्षः, RBBINode *अन्तः_Mark_ग्रन्थिः) {

    UVector         पर्णम्_Nodes(*f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

    // get a list all leaf nodes
    वृक्षः->findNodes(&पर्णम्_Nodes, RBBINode::पर्णम्_Char, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector नियमः_मूलम्_Nodes(*f_स्थितिः);
    सङ्कलय_नियमः_मूलम्_Nodes(&नियमः_मूलम्_Nodes, वृक्षः);

    UVector मेलनम्_Start_Nodes(*f_स्थितिः);
    प्रति (पूर्णाङ्क j=0; j<नियमः_मूलम्_Nodes.size(); ++j) {
        RBBINode *ग्रन्थिः = static_cast<RBBINode *>(नियमः_मूलम्_Nodes.elementAt(j));
        यदि (ग्रन्थिः->f_Chain_अन्तर्) {
            समुच्चयः_सङ्कलय_2(&मेलनम्_Start_Nodes, ग्रन्थिः->f_First_Pos_समुच्चयः);
        }
    }
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

    int32_t  अन्तः_ग्रन्थिः_Ix;
    int32_t  start_ग्रन्थिः_Ix;

    प्रति (अन्तः_ग्रन्थिः_Ix=0; अन्तः_ग्रन्थिः_Ix<पर्णम्_Nodes.size(); अन्तः_ग्रन्थिः_Ix++) {
        RBBINode* अन्तः_ग्रन्थिः = static_cast<RBBINode*>(पर्णम्_Nodes.elementAt(अन्तः_ग्रन्थिः_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        यदि (!अन्तः_ग्रन्थिः->fFollowPos->अन्तर्भवति(अन्तः_Mark_ग्रन्थिः)) {
            अनुवर्त;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_ग्रन्थिः;
        प्रति (start_ग्रन्थिः_Ix = 0; start_ग्रन्थिः_Ix<मेलनम्_Start_Nodes.size(); start_ग्रन्थिः_Ix++) {
            start_ग्रन्थिः = static_cast<RBBINode*>(मेलनम्_Start_Nodes.elementAt(start_ग्रन्थिः_Ix));
            यदि (start_ग्रन्थिः->fType != RBBINode::पर्णम्_Char) {
                अनुवर्त;
            }

            यदि (अन्तः_ग्रन्थिः->fVal == start_ग्रन्थिः->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                समुच्चयः_सङ्कलय_2(अन्तः_ग्रन्थिः->fFollowPos, start_ग्रन्थिः->fFollowPos);
            }
        }
    }
}


//-----------------------------------------------------------------------------
//
//   bofFixup.    Fixup for state tables that include {bof} beginning of input testing.
//                Do an swizzle similar to chaining, modifying the followPos set of
//                the bofNode to include the followPos nodes from other {bot} nodes
//                scattered through the tree.
//
//                This function has much in common with calcChainedFollowPos().
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::bofFixup() {

    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

    //   The parse tree looks like this ...
    //         fTree root  --->       <cat>
    //                               /     \       .
    //                            <cat>   <#end node>
    //                           /     \  .
    //                     <bofNode>   rest
    //                               of tree
    //
    //    We will be adding things to the followPos set of the <bofNode>
    //
    RBBINode  *bof_ग्रन्थिः = f_वृक्षः->f_वामः_Child->f_वामः_Child;
    U_ASSERT(bof_ग्रन्थिः->fType == RBBINode::पर्णम्_Char);
    U_ASSERT(bof_ग्रन्थिः->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *मेलनम्_Start_Nodes = f_वृक्षः->f_वामः_Child->f_दक्षिणः_Child->f_First_Pos_समुच्चयः;

    RBBINode *start_ग्रन्थिः;
    पूर्णाङ्क       start_ग्रन्थिः_Ix;
    प्रति (start_ग्रन्थिः_Ix = 0; start_ग्रन्थिः_Ix<मेलनम्_Start_Nodes->size(); start_ग्रन्थिः_Ix++) {
        start_ग्रन्थिः = static_cast<RBBINode*>(मेलनम्_Start_Nodes->elementAt(start_ग्रन्थिः_Ix));
        यदि (start_ग्रन्थिः->fType != RBBINode::पर्णम्_Char) {
            अनुवर्त;
        }

        यदि (start_ग्रन्थिः->fVal == bof_ग्रन्थिः->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            समुच्चयः_सङ्कलय_2(bof_ग्रन्थिः->fFollowPos, start_ग्रन्थिः->fFollowPos);
        }
    }
}

//-----------------------------------------------------------------------------
//
//   buildStateTable()    Determine the set of runtime DFA states and the
//                        transition tables for these states, by the algorithm
//                        of fig. 3.44 in Aho.
//
//                        Most of the comments are quotes of Aho's pseudo-code.
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::build_State_सारणी() {
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = शून्यसूचकः;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    पूर्णाङ्क      चरमः_निवेशः_चिह्नम् = fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories() - 1;
    failState = नूतन RBBIStateDescriptor(चरमः_निवेशः_चिह्नम्, f_स्थितिः);
    यदि (failState == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        गच्छ ExitBuildSTdeleteall;
    }
    failState->fPositions = नूतन UVector(*f_स्थितिः);
    यदि (failState->fPositions == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि (failState->fPositions == शून्यसूचकः || U_FAILURE(*f_स्थितिः)) {
        गच्छ ExitBuildSTdeleteall;
    }
    fDStates->सङ्कलय_Element(failState, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        गच्छ ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = नूतन RBBIStateDescriptor(चरमः_निवेशः_चिह्नम्, f_स्थितिः);
    यदि (initialState == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि (U_FAILURE(*f_स्थितिः)) {
        गच्छ ExitBuildSTdeleteall;
    }
    initialState->fPositions = नूतन UVector(*f_स्थितिः);
    यदि (initialState->fPositions == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि (U_FAILURE(*f_स्थितिः)) {
        गच्छ ExitBuildSTdeleteall;
    }
    समुच्चयः_सङ्कलय_2(initialState->fPositions, f_वृक्षः->f_First_Pos_समुच्चयः);
    fDStates->सङ्कलय_Element(initialState, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        गच्छ ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    प्रति (;;) {
        RBBIStateDescriptor *T = शून्यसूचकः;
        int32_t              tx;
        प्रति (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *अस्थायी;
            अस्थायी = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            यदि (अस्थायी->fMarked == असत्य) {
                T = अस्थायी;
                भङ्ग;
            }
        }
        यदि (T == शून्यसूचकः) {
            भङ्ग;
        }

        // mark T;
        T->fMarked = सत्य;

        // for each input symbol a do begin
        int32_t  a;
        प्रति (a = 1; a<=चरमः_निवेशः_चिह्नम्; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = शून्यसूचकः;
            RBBINode   *p;
            int32_t     px;
            प्रति (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                यदि ((p->fType == RBBINode::पर्णम्_Char) &&  (p->fVal == a)) {
                    यदि (U == शून्यसूचकः) {
                        U = नूतन UVector(*f_स्थितिः);
                        यदि (U == शून्यसूचकः) {
                        	*f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                        	गच्छ ExitBuildSTdeleteall;
                        }
                    }
                    समुच्चयः_सङ्कलय_2(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = असत्य;
            यदि (U != शून्यसूचकः) {
                U_ASSERT(U->size() > 0);
                पूर्णाङ्क  ix;
                प्रति (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    यदि (समुच्चयः_Equals(U, temp2->fPositions)) {
                        विलोपय U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = सत्य;
                        भङ्ग;
                    }
                }

                // Add U as an unmarked state to Dstates
                यदि (!UinDstates)
                {
                    RBBIStateDescriptor *newState = नूतन RBBIStateDescriptor(चरमः_निवेशः_चिह्नम्, f_स्थितिः);
                    यदि (newState == शून्यसूचकः) {
                    	*f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                    }
                    यदि (U_FAILURE(*f_स्थितिः)) {
                        गच्छ ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->सङ्कलय_Element(newState, *f_स्थितिः);
                    यदि (U_FAILURE(*f_स्थितिः)) {
                        प्रत्यागम;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->समुच्चयः_Element_At(ux, a);
            }
        }
    }
    प्रत्यागम;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    विलोपय initialState;
    विलोपय failState;
}


/**
 * mapLookAheadRules
 *
 */
शून्य RBBITableBuilder::map_Look_Ahead_नियमाः() {
    f_Look_Ahead_नियमः_Map =  नूतन UVector32(fRB->fScanner->अङ्कः_नियमाः() + 1, *f_स्थितिः);
    यदि (f_Look_Ahead_नियमः_Map == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    f_Look_Ahead_नियमः_Map->समुच्चयः_आकार(fRB->fScanner->अङ्कः_नियमाः() + 1);

    प्रति (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        तार्किक saw_Look_Ahead_ग्रन्थिः = असत्य;
        प्रति (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ग्रन्थिः = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            यदि (ग्रन्थिः->fType != RBBINode::ग्रन्थिः_Type::lookAhead) {
                अनुवर्त;
            }
            saw_Look_Ahead_ग्रन्थिः = सत्य;
            int32_t नियमः_अङ्कः = ग्रन्थिः->fVal;     // Set when rule was originally parsed.
            U_ASSERT(नियमः_अङ्कः < f_Look_Ahead_नियमः_Map->size());
            U_ASSERT(नियमः_अङ्कः > 0);
            int32_t laSlot = f_Look_Ahead_नियमः_Map->elementAti(नियमः_अङ्कः);
            यदि (laSlot != 0) {
                यदि (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } अन्यथा {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        यदि (!saw_Look_Ahead_ग्रन्थिः) {
            अनुवर्त;
        }

        यदि (laSlotForState == 0) {
            laSlotForState = ++f_LASlots_अन्तर्_Use;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        प्रति (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ग्रन्थिः = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            यदि (ग्रन्थिः->fType != RBBINode::ग्रन्थिः_Type::lookAhead) {
                अनुवर्त;
            }
            int32_t नियमः_अङ्कः = ग्रन्थिः->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = f_Look_Ahead_नियमः_Map->elementAti(नियमः_अङ्कः);
            (शून्य)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            f_Look_Ahead_नियमः_Map->समुच्चयः_Element_At(laSlotForState, नियमः_अङ्कः);
        }
    }

}

//-----------------------------------------------------------------------------
//
//   flagAcceptingStates    Identify accepting states.
//                          First get a list of all of the end marker nodes.
//                          Then, for each state s,
//                              if s contains one of the end marker nodes in its list of tree positions then
//                                  s is an accepting state.
//
//-----------------------------------------------------------------------------
शून्य     RBBITableBuilder::flag_Accepting_अवस्थाः() {
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    UVector     अन्तः_Marker_Nodes(*f_स्थितिः);
    RBBINode    *अन्तः_Marker;
    int32_t     i;
    int32_t     n;

    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

    f_वृक्षः->findNodes(&अन्तः_Marker_Nodes, RBBINode::अन्तः_Mark, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

    प्रति (i=0; i<अन्तः_Marker_Nodes.size(); i++) {
        अन्तः_Marker = static_cast<RBBINode*>(अन्तः_Marker_Nodes.elementAt(i));
        प्रति (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            यदि (sd->fPositions->सूचकांक_सम्बन्धि(अन्तः_Marker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                यदि (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = f_Look_Ahead_नियमः_Map->elementAti(अन्तः_Marker->fVal);
                    यदि (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                यदि (sd->fAccepting==ACCEPTING_UNCONDITIONAL && अन्तः_Marker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = f_Look_Ahead_नियमः_Map->elementAti(अन्तः_Marker->fVal);
                }
                // implicit else:
                // if sd->fAccepting already had a value other than 0 or 1, leave it be.
            }
        }
    }
}


//-----------------------------------------------------------------------------
//
//    flagLookAheadStates   Very similar to flagAcceptingStates, above.
//
//-----------------------------------------------------------------------------
शून्य     RBBITableBuilder::flag_Look_Ahead_अवस्थाः() {
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    UVector     lookAheadNodes(*f_स्थितिः);
    RBBINode    *look_Ahead_ग्रन्थिः;
    int32_t     i;
    int32_t     n;

    f_वृक्षः->findNodes(&lookAheadNodes, RBBINode::lookAhead, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    प्रति (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_ग्रन्थिः = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_ग्रन्थिः->fType == RBBINode::ग्रन्थिः_Type::lookAhead);

        प्रति (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->सूचकांक_सम्बन्धि(look_Ahead_ग्रन्थिः);
            यदि (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_ग्रन्थिः == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = f_Look_Ahead_नियमः_Map->elementAti(look_Ahead_ग्रन्थिः->fVal);
                U_ASSERT(sd->fLookAhead == 0 || sd->fLookAhead == lookaheadSlot);
                // if (sd->fLookAhead != 0 && sd->fLookAhead != lookaheadSlot) {
                //     printf("%s:%d Bingo. sd->fLookAhead:%d   lookaheadSlot:%d\n",
                //            __FILE__, __LINE__, sd->fLookAhead, lookaheadSlot);
                // }
                sd->fLookAhead = lookaheadSlot;
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//    flagTaggedStates
//
//-----------------------------------------------------------------------------
शून्य     RBBITableBuilder::flag_Tagged_अवस्थाः() {
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    UVector     चिह्नकम्_Nodes(*f_स्थितिः);
    RBBINode    *चिह्नकम्_ग्रन्थिः;
    int32_t     i;
    int32_t     n;

    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    f_वृक्षः->findNodes(&चिह्नकम्_Nodes, RBBINode::चिह्नकम्, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    प्रति (i=0; i<चिह्नकम्_Nodes.size(); i++) {                   // For each tag node t (all of 'em)
        चिह्नकम्_ग्रन्थिः = static_cast<RBBINode*>(चिह्नकम्_Nodes.elementAt(i));

        प्रति (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            यदि (sd->fPositions->सूचकांक_सम्बन्धि(चिह्नकम्_ग्रन्थिः) >= 0) {  //       if  s include the tag node t
                sorted_सङ्कलय(&sd->f_चिह्नकम्_Vals, चिह्नकम्_ग्रन्थिः->fVal);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//  mergeRuleStatusVals
//
//      Update the global table of rule status {tag} values
//      The rule builder has a global vector of status values that are common
//      for all tables.  Merge the ones from this table into the global set.
//
//-----------------------------------------------------------------------------
शून्य  RBBITableBuilder::merge_नियमः_स्थितिः_Vals() {
    //
    //  The basic outline of what happens here is this...
    //
    //    for each state in this state table
    //       if the status tag list for this state is in the global statuses list
    //           record where and
    //           continue with the next state
    //       else
    //           add the tag list for this state to the global list.
    //
    पूर्णाङ्क i;
    पूर्णाङ्क n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    यदि (fRB->f_नियमः_स्थितिः_Vals->size() == 0) {
        fRB->f_नियमः_स्थितिः_Vals->सङ्कलय_Element(1, *f_स्थितिः);  // Num of statuses in group
        fRB->f_नियमः_स्थितिः_Vals->सङ्कलय_Element(static_cast<int32_t>(0), *f_स्थितिः); // and our single status of zero
    }

    //    For each state
    प्रति (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_अवस्थाः_चिह्नकम्_मानानि = sd->f_चिह्नकम्_Vals;
        यदि (this_अवस्थाः_चिह्नकम्_मानानि == शून्यसूचकः) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->f_चिह्नकानि_Idx = 0;
            अनुवर्त;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->f_चिह्नकानि_Idx = -1;
        int32_t  this_चिह्नकम्_गणः_Start = 0;   // indexes into the global rule status vals list
        int32_t  अग्र_चिह्नकम्_गणः_Start = 0;

        // Loop runs once per group of tags in the global list
        यावत् (अग्र_चिह्नकम्_गणः_Start < fRB->f_नियमः_स्थितिः_Vals->size()) {
            this_चिह्नकम्_गणः_Start = अग्र_चिह्नकम्_गणः_Start;
            अग्र_चिह्नकम्_गणः_Start += fRB->f_नियमः_स्थितिः_Vals->elementAti(this_चिह्नकम्_गणः_Start) + 1;
            यदि (this_अवस्थाः_चिह्नकम्_मानानि->size() != fRB->f_नियमः_स्थितिः_Vals->elementAti(this_चिह्नकम्_गणः_Start)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                अनुवर्त;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            प्रति (i=0; i<this_अवस्थाः_चिह्नकम्_मानानि->size(); i++) {
                यदि (this_अवस्थाः_चिह्नकम्_मानानि->elementAti(i) !=
                    fRB->f_नियमः_स्थितिः_Vals->elementAti(this_चिह्नकम्_गणः_Start + 1 + i) ) {
                    // Mismatch.
                    भङ्ग;
                }
            }

            यदि (i == this_अवस्थाः_चिह्नकम्_मानानि->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->f_चिह्नकानि_Idx = this_चिह्नकम्_गणः_Start;
                भङ्ग;
            }
        }

        यदि (sd->f_चिह्नकानि_Idx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->f_चिह्नकानि_Idx = fRB->f_नियमः_स्थितिः_Vals->size();
            fRB->f_नियमः_स्थितिः_Vals->सङ्कलय_Element(this_अवस्थाः_चिह्नकम्_मानानि->size(), *f_स्थितिः);
            प्रति (i=0; i<this_अवस्थाः_चिह्नकम्_मानानि->size(); i++) {
                fRB->f_नियमः_स्थितिः_Vals->सङ्कलय_Element(this_अवस्थाः_चिह्नकम्_मानानि->elementAti(i), *f_स्थितिः);
            }
        }
    }
}





//-----------------------------------------------------------------------------
//
//    minimizeStates    Minimize the number of states of the DFA, by Algorithm
//                      3.6 in Aho.  The one twist is that instead of starting
//                      from a partition in two groups (accepting and non-
//                      accepting states), we partition according to all
//                      relevant properties of the states, namely the values of:
//                      - fAccepting—which may be non-accepting,
//                        unconditionally accepting, or the index of a
//                        lookahead—;
//                      - fLookAhead;
//                      - fTagsIdx.
//
//-----------------------------------------------------------------------------

शून्य RBBITableBuilder::minimize_अवस्थाः() {
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

#यदि UPRV_HAS_SANITIZER
    यदि (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *f_स्थितिः = U_REGEX_आदर्शः_TOO_BIG;
        प्रत्यागम;
    }
#यदि_अन्तः
    संरचना StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t f_चिह्नकानि_Idx;
        तार्किक संक्रियकः==(StateType ध्रुव &other) ध्रुव {
            प्रत्यागम fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   f_चिह्नकानि_Idx == other.f_चिह्नकानि_Idx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    संरचना Type_गन्तव्यम्_अवस्थाः : UMemory {
        Type_गन्तव्यम्_अवस्थाः(ध्रुव StateType &type, UError_संहिता &स्थितिः)
            : type(type), अवस्थाः(prv::make_unique<UVector>(स्थितिः, स्थितिः)) {}
        StateType type;
        prv::unique_ptr<UVector> अवस्थाः;
    };
    UVector initialPartition(*f_स्थितिः);
    initialPartition.समुच्चयः_Deleter(
        [](शून्य *p) { विलोपय static_cast<Type_गन्तव्यम्_अवस्थाः *>(p); });
    प्रति (int32_t i = 0; i < fDStates->size(); ++i) {
        ध्रुव RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ध्रुव StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.f_चिह्नकानि_Idx};
        int32_t j = 0;
        प्रति (; j < initialPartition.size(); ++j) {
            स्वचालित &[type_j, अवस्थाः] =
                *static_cast<Type_गन्तव्यम्_अवस्थाः *>(initialPartition.elementAt(j));
            यदि (type_j == type) {
                अवस्थाः->सङ्कलय_Element(i, *f_स्थितिः);
                भङ्ग;
            }
        }
        यदि (j == initialPartition.size()) {
            स्वचालित new_प्रविष्टिः = prv::make_unique<Type_गन्तव्यम्_अवस्थाः>(type, *f_स्थितिः, *f_स्थितिः);
            यदि (U_FAILURE(*f_स्थितिः)) {
                प्रत्यागम;
            }
            new_प्रविष्टिः->अवस्थाः->सङ्कलय_Element(i, *f_स्थितिः);
            initialPartition.adoptElement(new_प्रविष्टिः.release(), *f_स्थितिः);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    स्वचालित partition = prv::make_unique<UVector>(*f_स्थितिः, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    partition->समुच्चयः_Deleter(
        [](शून्य *p) { विलोपय static_cast<UVector*>(p); });
    
    प्रति (int32_t i = 0; i < initialPartition.size(); ++i) {
        partition->adoptElement(
            static_cast<Type_गन्तव्यम्_अवस्थाः*>(initialPartition.elementAt(i))->अवस्थाः.release(),
            *f_स्थितिः);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    स्वचालित partition_signature = [&partition, एतत्](int32_t state_सूचकांक) -> prv::unique_ptr<UVector> {
        ध्रुव RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state_सूचकांक));
        स्वचालित परिणाम = prv::make_unique<UVector>(state.fDtran->size(), *f_स्थितिः, *f_स्थितिः);
        यदि (U_FAILURE(*f_स्थितिः)) {
            प्रत्यागम शून्यसूचकः;
        }
        प्रति (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t गन्तव्यम्_State = state.fDtran->elementAti(i);
            प्रति (int32_t part_सूचकांक = 0; part_सूचकांक < partition->size(); ++part_सूचकांक) {
                ध्रुव UVector &part = *static_cast<UVector *>(partition->elementAt(part_सूचकांक));
                यदि (part.अन्तर्भवति(गन्तव्यम्_State)) {
                    परिणाम->सङ्कलय_Element(part_सूचकांक, *f_स्थितिः);
                    भङ्ग;
                }
            }
        }
        परिणाम->समुच्चयः_Comparer([](ध्रुव UElement वामतः, ध्रुव UElement दक्षिणतः) -> UBool {
            प्रत्यागम वामतः.integer == दक्षिणतः.integer;
        });
        प्रत्यागम परिणाम;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    प्रति (;;) {
        // Π_new.
        स्वचालित partitionNew = prv::make_unique<UVector>(*f_स्थितिः, *f_स्थितिः);
        यदि (U_FAILURE(*f_स्थितिः)) {
            प्रत्यागम;
        }
        partitionNew->समुच्चयः_Deleter(
            [](शून्य *p) { विलोपय static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        तार्किक refined = असत्य;
        प्रति (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ध्रुव UVector &गणः = *static_cast<UVector*>(partition->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            संरचना Signature_गन्तव्यम्_अवस्थाः : UMemory {
                Signature_गन्तव्यम्_अवस्थाः(prv::unique_ptr<UVector> signature, UError_संहिता &स्थितिः)
                    : signature(std::move(signature)), अवस्थाः(prv::make_unique<UVector>(स्थितिः, स्थितिः)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> अवस्थाः;
            };
            UVector गणः_Refinement(*f_स्थितिः);
            गणः_Refinement.समुच्चयः_Deleter([](शून्य *p) { विलोपय static_cast<Signature_गन्तव्यम्_अवस्थाः*>(p); });
            प्रति (int32_t j = 0; j < गणः.size(); ++j) {
                // Index of a state in the group.
                ध्रुव int32_t s = गणः.elementAti(j);
                स्वचालित signature = partition_signature(s);
                int32_t k = 0;
                प्रति (; k < गणः_Refinement.size(); ++k) {
                    ध्रुव स्वचालित &[subgroupSignature, subgroup] =
                        *static_cast<Signature_गन्तव्यम्_अवस्थाः*>(गणः_Refinement.elementAt(k));
                    यदि (*subgroupSignature == *signature) {
                        subgroup->सङ्कलय_Element(s, *f_स्थितिः);
                        भङ्ग;
                    }
                }
                यदि (k == गणः_Refinement.size()) {
                    ध्रुव स्वचालित new_प्रविष्टिः =
                        नूतन Signature_गन्तव्यम्_अवस्थाः(std::move(signature), *f_स्थितिः);
                    यदि (U_FAILURE(*f_स्थितिः)) {
                        प्रत्यागम;
                    }
                    यदि (new_प्रविष्टिः == शून्यसूचकः) {
                      *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                      प्रत्यागम;
                    }
                    new_प्रविष्टिः->अवस्थाः->सङ्कलय_Element(s, *f_स्थितिः);
                    गणः_Refinement.adoptElement(new_प्रविष्टिः, *f_स्थितिः);
                }
            }
            refined |= गणः_Refinement.size() > 1;
            प्रति (int32_t j = 0; j < गणः_Refinement.size(); ++j) {
                स्वचालित &[_, subgroup] = *static_cast<Signature_गन्तव्यम्_अवस्थाः*>(
                    गणः_Refinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *f_स्थितिः);
            }
        }
        यदि (refined) {
            partition = std::move(partitionNew);
        } अन्यथा {
            भङ्ग;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    प्रति (पूर्णाङ्क i = 0; i < partition->size(); ++i) {
        UVector *ध्रुव part_i = static_cast<UVector *>(partition->elementAt(i));
        यदि (part_i->अन्तर्भवति(0)) {
            शून्य *ध्रुव part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ध्रुव स्वचालित deleter = partition->समुच्चयः_Deleter(शून्यसूचकः);
            partition->समुच्चयः_Element_At(part_i, 0);
            partition->समुच्चयः_Element_At(part_0, i);
            partition->समुच्चयः_Deleter(deleter);
        } अन्यथा यदि (part_i->अन्तर्भवति(1)) {
            शून्य *ध्रुव part_1 = partition->elementAt(1);
            ध्रुव स्वचालित deleter = partition->समुच्चयः_Deleter(शून्यसूचकः);
            partition->समुच्चयः_Element_At(part_i, 1);
            partition->समुच्चयः_Element_At(part_1, i);
            partition->समुच्चयः_Deleter(deleter);
        }
    }
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    स्वचालित old_State_गन्तव्यम्_Part = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    प्रति (पूर्णाङ्क i = 0; i < partition->size(); ++i) {
        ध्रुव UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        प्रति (पूर्णाङ्क j = 0; j < part.size(); ++j) {
            old_State_गन्तव्यम्_Part[part.elementAti(j)] = i;
        }
    }
    स्वचालित ध्रुव old_अवस्थाः = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*f_स्थितिः, *f_स्थितिः).release();
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    प्रति (पूर्णाङ्क i = 0; i < partition->size(); ++i) {
        ध्रुव UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *ध्रुव state =
            static_cast<RBBIStateDescriptor*>(old_अवस्थाः->elementAt(part.elementAti(0)));
        fDStates->सङ्कलय_Element(state, *f_स्थितिः);
        प्रति (पूर्णाङ्क j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->समुच्चयः_Element_At(old_State_गन्तव्यम्_Part[state->fDtran->elementAti(j)], j);
        }
        old_अवस्थाः->समुच्चयः_Element_At(शून्यसूचकः, part.elementAti(0));
    }
    old_अवस्थाः->समुच्चयः_Deleter([](शून्य *p) { विलोपय static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::sorted_सङ्कलय(UVector **vector, int32_t val) {
    int32_t i;

    यदि (*vector == शून्यसूचकः) {
        *vector = नूतन UVector(*f_स्थितिः);
    }
    यदि (*vector == शून्यसूचकः || U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    UVector *vec = *vector;
    int32_t  v_आकार = vec->size();
    प्रति (i=0; i<v_आकार; i++) {
        int32_t valAtI = vec->elementAti(i);
        यदि (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            प्रत्यागम;
        }
        यदि (valAtI > val) {
            भङ्ग;
        }
    }
    vec->निवेशय_Element_At(val, i, *f_स्थितिः);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::समुच्चयः_सङ्कलय_2(UVector *dest, UVector *source) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!source->hasDeleter());
    int32_t dest_Original_आकार = dest->size();
    int32_t source_आकार       = source->size();
    int32_t di           = 0;
    Maybe_राशिः_Array<शून्य *, 16> destArray, sourceArray;  // Handle small cases without malloc
    शून्य **destPtr, **sourcePtr;
    शून्य **destLim, **sourceLim;

    यदि (dest_Original_आकार > destArray.get_क्षमता()) {
        यदि (destArray.resize(dest_Original_आकार) == शून्यसूचकः) {
            प्रत्यागम;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + dest_Original_आकार;  // destArray.getArrayLimit()?

    यदि (source_आकार > sourceArray.get_क्षमता()) {
        यदि (sourceArray.resize(source_आकार) == शून्यसूचकः) {
            प्रत्यागम;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + source_आकार;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (शून्य) dest->गन्तव्यम्_Array(destPtr);
    (शून्य) source->गन्तव्यम्_Array(sourcePtr);

    dest->समुच्चयः_आकार(source_आकार+dest_Original_आकार, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

    यावत् (sourcePtr < sourceLim && destPtr < destLim) {
        यदि (*destPtr == *sourcePtr) {
            dest->समुच्चयः_Element_At(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        अन्यथा यदि (uprv_memcmp(destPtr, sourcePtr, परिमाण(शून्य *)) < 0) {
            dest->समुच्चयः_Element_At(*destPtr++, di++);
        }
        अन्यथा { /* *sourcePtr < *destPtr */
            dest->समुच्चयः_Element_At(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    यावत् (destPtr < destLim) {
        dest->समुच्चयः_Element_At(*destPtr++, di++);
    }
    यावत् (sourcePtr < sourceLim) {
        dest->समुच्चयः_Element_At(*sourcePtr++, di++);
    }

    dest->समुच्चयः_आकार(di, *f_स्थितिः);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::समुच्चयः_Equals(UVector *a, UVector *b) {
    प्रत्यागम a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBITableBuilder::printPosSets(RBBINode *n) {
    यदि (n==शून्यसूचकः) {
        प्रत्यागम;
    }
    प्रारूप_लिख("\n");
    RBBINode::print_ग्रन्थिः_शीर्षकम्();
    RBBINode::print_ग्रन्थिः(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    print_समुच्चयः(n->f_First_Pos_समुच्चयः);

    RBBIDebugPrintf("         lastpos:   ");
    print_समुच्चयः(n->f_चरमः_Pos_समुच्चयः);

    RBBIDebugPrintf("         followpos: ");
    print_समुच्चयः(n->fFollowPos);

    printPosSets(n->f_वामः_Child);
    printPosSets(n->f_दक्षिणः_Child);
}
#यदि_अन्तः

//
//    findDuplCharClassFrom()
//
तार्किक RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t अङ्कः_अवस्थाः = fDStates->size();
    int32_t अङ्कः_स्तम्भसङ्ख्या = fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories();

    प्रति (; categories->first < अङ्कः_स्तम्भसङ्ख्या-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        पूर्णाङ्क सीमा_Second = categories->first < fRB->f_समुच्चयः_Builder->getDictCategoriesStart() ?
            fRB->f_समुच्चयः_Builder->getDictCategoriesStart() : अङ्कः_स्तम्भसङ्ख्या;
        प्रति (categories->second=categories->first+1; categories->second < सीमा_Second; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t सारणी_आधारः = 0;
            uint16_t सारणी_dupl = 1;
            प्रति (int32_t state=0; state<अङ्कः_अवस्थाः; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                सारणी_आधारः = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                सारणी_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                यदि (सारणी_आधारः != सारणी_dupl) {
                    भङ्ग;
                }
            }
            यदि (सारणी_आधारः == सारणी_dupl) {
                प्रत्यागम सत्य;
            }
        }
    }
    प्रत्यागम असत्य;
}


//
//    removeColumn()
//
शून्य RBBITableBuilder::removeColumn(int32_t column) {
    int32_t अङ्कः_अवस्थाः = fDStates->size();
    प्रति (int32_t state=0; state<अङ्कः_अवस्थाः; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


तार्किक RBBITableBuilder::findDuplicateSafeState(IntPair *अवस्थाः) {
    int32_t अङ्कः_अवस्थाः = f_Safe_सारणी->size();

    प्रति (; अवस्थाः->first<अङ्कः_अवस्थाः-1; अवस्थाः->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(f_Safe_सारणी->elementAt(अवस्थाः->first));
        प्रति (अवस्थाः->second=अवस्थाः->first+1; अवस्थाः->second<अङ्कः_अवस्थाः; अवस्थाः->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(f_Safe_सारणी->elementAt(अवस्थाः->second));
            तार्किक पङ्क्तिकाः_मेलनम् = सत्य;
            int32_t अङ्कः_स्तम्भसङ्ख्या = firstRow->length();
            प्रति (int32_t col=0; col < अङ्कः_स्तम्भसङ्ख्या; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                यदि (!((firstVal == duplVal) ||
                        ((firstVal == अवस्थाः->first || firstVal == अवस्थाः->second) &&
                        (duplVal  == अवस्थाः->first || duplVal  == अवस्थाः->second)))) {
                    पङ्क्तिकाः_मेलनम् = असत्य;
                    भङ्ग;
                }
            }
            यदि (पङ्क्तिकाः_मेलनम्) {
                प्रत्यागम सत्य;
            }
        }
    }
    प्रत्यागम असत्य;
}

शून्य RBBITableBuilder::removeSafeState(IntPair dupl_अवस्थाः) {
    ध्रुव int32_t keepState = dupl_अवस्थाः.first;
    ध्रुव int32_t duplState = dupl_अवस्थाः.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < f_Safe_सारणी->size());

    f_Safe_सारणी->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t अङ्कः_अवस्थाः = f_Safe_सारणी->size();
    प्रति (int32_t state=0; state<अङ्कः_अवस्थाः; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(f_Safe_सारणी->elementAt(state));
        int32_t अङ्कः_स्तम्भसङ्ख्या = sd->length();
        प्रति (int32_t col=0; col<अङ्कः_स्तम्भसङ्ख्या; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            यदि (existingVal == duplState) {
                newVal = keepState;
            } अन्यथा यदि (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->समुच्चयः_Char_At(col, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::remove_Duplicate_अवस्थाः() {
    ध्रुव int32_t old_State_गणनम् = fDStates->size();
    minimize_अवस्थाः();
    प्रत्यागम fDStates->size() - old_State_गणनम्;
}


/*
 * Minimizes the number of slots used by the lookaheads.
 *
 * When the state machine is first generated, every lookahead rule occupies a different slot, for instance,
 * given
 *   x / z;  # Lookahead 1 below.
 *   y / z;  # Lookahead 2 below.
 *   [xyz]*;
 * the following state machine is produced,
 *           x
 *         x ⮏  z
 *         → 𝑠₁ → 𝑎₁
 *   START  x⇅y
 *     ↻z  → 𝑠₂ → 𝑎₂
 *         y ↻  z
 *           y
 *
 * where 𝑠ᵢ sets the current position for lookahead i (assigning pᵢ=p_current), and 𝑎ᵢ accepts
 * lookahead i if set, i.e., on state 𝑎ᵢ,the break iterator returns pᵢ if set. Because they accept
 * different lookaheads, states 𝑎₁ and 𝑎₂ are distinct in the initial partition constructed by
 * `minimizeStates`, and cannot be merged, and the state machine cannot be simplified.
 *
 * However, there is no need for lookaheads 1 and 2 to occupy different slots, i.e., there is no
 * need for the state machine to separately store the positions set by 𝑠₁ and 𝑠₂ (for p₁ and p₂ to
 * be distinct variables). Indeed, if state 𝑠₁ is encountered, state 𝑎₂ cannot be reached without
 * going through state 𝑠₂, and vice versa, so if p₁ and p₂ are backed by the same variable p, the
 * value set by p=p₁=p_current on 𝑠₁ will have been overriden by p=p₂=p_current on 𝑠₂, so that the
 * p₁-p₂ merger has no effect.
 *
 * If the lookaheads are merged, states 𝑎₁ and 𝑎₂ are no longer initially distinct in
 * `minimizeStates` (they both accept the only lookahead), and indeed the whole state machine
 * simplifies to
 *         [xy]     z
 *   START   →  𝑠  →  𝑎
 *     ↻z       ↻
 *             [xy]
 *
 * Lookaheads i and j can be merged if:
 * 1. From any state that sets lookahead i, no state that accepts lookahead j can be reached without
 *    going through a state that sets lookahead j; and
 * 2. From any state that sets lookahead j, no state that accepts lookahead i can be reached without
 *    going through a state that sets lookahead i.
 * This reachability relation defines a directed graph of lookaheads, and an optimal merging of
 * lookaheads is then a colouring of that graph. (In the example above, the graph of lookaheads has
 * no edges, and is thus 1-colourable; more interesting examples can be found in the test
 * TestLookaheadPolychromy.)
 *
 * This function first computes the adjacency matrix of lookahead reachability, and then colours the
 * graph of lookaheads and reassigns lookahead slots accordingly.
 */
शून्य RBBITableBuilder::minimizeLookaheads() {
    यदि (f_LASlots_अन्तर्_Use == ACCEPTING_UNCONDITIONAL) {
        प्रत्यागम;
    }
    ध्रुव int32_t lookahead_गणनम् = f_LASlots_अन्तर्_Use - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ध्रुव int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    ध्रुव स्वचालित stateDescriptor = [एतत्](ध्रुव int32_t state) -> RBBIStateDescriptor & {
        प्रत्यागम *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ध्रुव स्वचालित lookaheadReachability =
        prv::make_unique<तार्किक[]>(lookahead_गणनम् * lookahead_गणनम्, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    प्रति (int32_t l = firstLookahead; l <= f_LASlots_अन्तर्_Use; ++l) {
        प्रति (int32_t k = firstLookahead; k <= f_LASlots_अन्तर्_Use; ++k) {
            यदि (k != l) {
                प्रति (int32_t source = 1; source < fDStates->size(); ++source) {
                    यदि (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        अनुवर्त;
                    }
                    यदि (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](ध्रुव int32_t state) {
                                प्रत्यागम static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ध्रुव int32_t state) {
                                प्रत्यागम static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *f_स्थितिः)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_गणनम् + k -
                                              firstLookahead] = सत्य;
                    }
                }
            }
        }
    }

    ध्रुव स्वचालित colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_गणनम्, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    प्रति (int32_t chromatic_सङ्ख्या = 1;; ++chromatic_सङ्ख्या) {
        uprv_memset(colours.get(), 0, परिमाण(int32_t) * lookahead_गणनम्);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        प्रति (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            प्रति (पूर्णाङ्क source = 0; source < lookahead_गणनम्; ++source) {
                प्रति (पूर्णाङ्क sink = 0; sink < lookahead_गणनम्; ++sink) {
                    यदि (lookaheadReachability[source * lookahead_गणनम् + sink] &&
                        colours[source] == colours[sink]) {
                        गच्छ अग्र_Colouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            प्रति (पूर्णाङ्क i = 0; i < fDStates->size(); ++i) {
                यदि (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        firstLookahead +
                        colours[stateDescriptor(i).fAccepting - firstLookahead];
                }
                यदि (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - firstLookahead];
                }
            }
            f_LASlots_अन्तर्_Use = ACCEPTING_UNCONDITIONAL + chromatic_सङ्ख्या;
            प्रत्यागम;

          अग्र_Colouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            प्रति (int32_t i = 0; i < lookahead_गणनम् - 1 && colours[i] == chromatic_सङ्ख्या;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            यदि (colours[lookahead_गणनम् - 1] == chromatic_सङ्ख्या) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                भङ्ग;
            }
        }
    }
}


//-----------------------------------------------------------------------------
//
//   getTableSize()    Calculate the size of the runtime form of this
//                     state transition table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_सारणी_आकार() ध्रुव {
    int32_t    size = 0;
    int32_t    अङ्कः_पङ्क्तिकाः;
    int32_t    अङ्कः_स्तम्भसङ्ख्या;
    int32_t    row_आकार;

    यदि (f_वृक्षः == शून्यसूचकः) {
        प्रत्यागम 0;
    }

    size    = offsetof(RBBIState_सारणी, f_सारणी_दत्त);    // The header, with no rows to the table.

    अङ्कः_पङ्क्तिकाः = fDStates->size();
    अङ्कः_स्तम्भसङ्ख्या = fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories();

    यदि (use8_द्वयङ्काः_For_सारणी()) {
        row_आकार = offsetof(RBBIState_सारणी_Row8, f_अग्र_State) + परिमाण(int8_t)*अङ्कः_स्तम्भसङ्ख्या;
    } अन्यथा {
        row_आकार = offsetof(RBBIState_सारणी_Row16, f_अग्र_State) + परिमाण(int16_t)*अङ्कः_स्तम्भसङ्ख्या;
    }
    size   += अङ्कः_पङ्क्तिकाः * row_आकार;
    प्रत्यागम size;
}

तार्किक RBBITableBuilder::use8_द्वयङ्काः_For_सारणी() ध्रुव {
    प्रत्यागम fDStates->size() <= k_Max_State_For8_द्वयङ्काः_सारणी;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::export_सारणी(शून्य *where) {
    RBBIState_सारणी* सारणी = static_cast<RBBIState_सारणी*>(where);
    uint32_t           state;
    पूर्णाङ्क                col;

    यदि (U_FAILURE(*f_स्थितिः) || f_वृक्षः == शून्यसूचकः) {
        प्रत्यागम;
    }

    int32_t cat_गणनम् = fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories();
    यदि (cat_गणनम् > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *f_स्थितिः = U_BRK_आन्तरिकम्_ERROR;
        प्रत्यागम;
    }

    सारणी->f_अङ्कः_अवस्थाः = fDStates->size();
    सारणी->fDictCategoriesStart = fRB->f_समुच्चयः_Builder->getDictCategoriesStart();
    सारणी->f_Look_Ahead_Results_आकार = f_LASlots_अन्तर्_Use == ACCEPTING_UNCONDITIONAL ? 0 : f_LASlots_अन्तर्_Use + 1;
    सारणी->fFlags     = 0;
    यदि (use8_द्वयङ्काः_For_सारणी()) {
        सारणी->fRowLen    = offsetof(RBBIState_सारणी_Row8, f_अग्र_State) + परिमाण(uint8_t) * cat_गणनम्;
        सारणी->fFlags  |= RBBI_8_द्वयङ्काः_पङ्क्तिकाः;
    } अन्यथा {
        सारणी->fRowLen    = offsetof(RBBIState_सारणी_Row16, f_अग्र_State) + परिमाण(int16_t) * cat_गणनम्;
    }
    यदि (fRB->fLookAheadHardBreak) {
        सारणी->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    प्रति (state=0; state<सारणी->f_अङ्कः_अवस्थाः; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIState_सारणी_Row* row = reinterpret_cast<RBBIState_सारणी_Row*>(सारणी->f_सारणी_दत्त + state * सारणी->fRowLen);
        यदि (use8_द्वयङ्काः_For_सारणी()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->f_चिह्नकानि_Idx && sd->f_चिह्नकानि_Idx <= 255);
            RBBIState_सारणी_Row8* r8 = reinterpret_cast<RBBIState_सारणी_Row8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->f_चिह्नकानि_Idx   = sd->f_चिह्नकानि_Idx;
            प्रति (col=0; col<cat_गणनम्; col++) {
                U_ASSERT (sd->fDtran->elementAti(col) <= k_Max_State_For8_द्वयङ्काः_सारणी);
                r8->f_अग्र_State[col] = sd->fDtran->elementAti(col);
            }
        } अन्यथा {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->f_चिह्नकानि_Idx && sd->f_चिह्नकानि_Idx <= 0xffff);
            row->r16.fAccepting = sd->fAccepting;
            row->r16.fLookAhead = sd->fLookAhead;
            row->r16.f_चिह्नकानि_Idx   = sd->f_चिह्नकानि_Idx;
            प्रति (col=0; col<cat_गणनम्; col++) {
                row->r16.f_अग्र_State[col] = sd->fDtran->elementAti(col);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
शून्य RBBITableBuilder::build_Safe_Reverse_सारणी(UError_संहिता &स्थितिः) {
    // The safe table creation has three steps:

    // 1. Identify pairs of character classes that are "safe." Safe means that boundaries
    // following the pair do not depend on context or state before the pair. To test
    // whether a pair is safe, run it through the main forward state table, starting
    // from each possible configuration. If the final configuration is the same, no matter what the
    // starting configuration, the pair is safe.
    // Note that the configuration includes not only the state, but also the last accepting position
    // and the lookaheads.  Some differences in final lookaheads can be ignored when it can be shown
    // that they do not affect breaks after the pair: this happens when we come back out of the pair
    // with a matching configuration when the lookahead matches.
    // TODO(egg): It is possible for pairs to be safe even if the outgoing states differ depending
    // on the state before the pair; this would require a more thorough analysis to handle, probably
    // in the style of what happens in minimizeStates.
    //
    // 2. Build a state table that recognizes the safe pairs. It's similar to their
    // forward table, with a column for each input character [class], and a row for
    // each state. Row 1 is the start state, and row 0 is the stop state. Initially
    // create an additional state for each input character category; being in
    // one of these states means that the character has been seen, and is potentially
    // the first of a pair. In each of these rows, the entry for the second character
    // of a safe pair is set to the stop state (0), indicating that a match was found.
    // All other table entries are set to the state corresponding the current input
    // character, allowing that character to be the of a start following pair.
    //
    // Because the safe rules are to be run in reverse, moving backwards in the text,
    // the first and second pair categories are swapped when building the table.
    //
    // 3. Compress the table. There are typically many rows (states) that are
    // equivalent - that have zeroes (match completed) in the same columns -
    // and can be folded together.

    // Each safe pair is stored as two UChars in the safePair string.
    UnicodeString safe_युग्मानि;

    int32_t अङ्कः_Char_Classes = fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories();
    int32_t अङ्कः_अवस्थाः = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ध्रुव स्वचालित lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_अन्तर्_Use + 1, स्थितिः);
    ध्रुव स्वचालित wanted_Lookaheads_अन्तर्_Pair =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_अन्तर्_Use + 1, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_समुच्चयः = -1;
    प्रति (int32_t c1 = 1; c1 < अङ्कः_Char_Classes; ++c1) {
        यदि (fRB->f_समुच्चयः_Builder->getFirstChar(c1) < 0) {
          अनुवर्त;
        }
        प्रति (int32_t c2 = 1; c2 < अङ्कः_Char_Classes; ++c2) {
            यदि (fRB->f_समुच्चयः_Builder->getFirstChar(c2) < 0) {
                अनुवर्त;
            }
            int32_t wanted_अन्तः_State = -1;
            int32_t wanted_चरमः_Accepting_Position;
            तार्किक mustCheckConsistentWithStartC2 = असत्य;
            ध्रुव int32_t text[2] = {c1, c2};
            प्रति (int32_t injectedState = START_STATE; injectedState < अङ्कः_अवस्थाः; ++injectedState) {
                प्रति (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= f_LASlots_अन्तर्_Use; ++injectedLookahead) {
                    int32_t चरमः_Break = BEFORE_PAIR;
                    int32_t चरमः_Accepting_Position = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_समुच्चयः, f_LASlots_अन्तर्_Use + 1);
                    यदि (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ध्रुव RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        यदि (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            चरमः_Accepting_Position = 0;
                        } अन्यथा यदि (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            अनुवर्त;
                        }
                        यदि (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    गच्छ dfa_चरणः;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    प्रति (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_समुच्चयः, f_LASlots_अन्तर्_Use + 1);
                        i = चरमः_Break;
                        // Loop Find_Next_Break in L2/26-135.
                        प्रति (;;) {
                        dfa_चरणः:
                            यदि (i == BEFORE_PAIR || i == 2) {
                                गच्छ inspectFinalState;
                            }
                            ध्रुव int32_t चिह्नम्_Ahead = text[i];
                            ++i;
                            ध्रुव RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            यदि (sDescriptor->fDtran->elementAti(चिह्नम्_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(चिह्नम्_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } अन्यथा {
                                यदि (चरमः_Break != BEFORE_PAIR &&
                                    चरमः_Accepting_Position == चरमः_Break) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    गच्छ अग्र_चिह्नम्_Pair;
                                }
                                चरमः_Break = चरमः_Accepting_Position;
                                भङ्ग;
                            }

                            यदि (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                चरमः_Accepting_Position = i;
                            } अन्यथा यदि (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_समुच्चयः) {
                                चरमः_Break = lookaheadPositions[sDescriptor->fAccepting];
                                भङ्ग;
                            }
                            यदि (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    यदि (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        अनुवर्त;
                    }
                    यदि (wanted_अन्तः_State < 0) {
                        wanted_अन्तः_State = s;
                        wanted_चरमः_Accepting_Position = चरमः_Accepting_Position;
                        प्रति (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_अन्तर्_Use; ++l) {
                            यदि (lookaheadPositions[l] != LOOKAHEAD_NOT_समुच्चयः &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wanted_Lookaheads_अन्तर्_Pair[l] = lookaheadPositions[i];
                            } अन्यथा {
                                wanted_Lookaheads_अन्तर्_Pair[l] = LOOKAHEAD_NOT_समुच्चयः;
                            }
                        }
                    } अन्यथा {
                        यदि (wanted_अन्तः_State != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            गच्छ अग्र_चिह्नम्_Pair;
                        }
                        यदि (wanted_चरमः_Accepting_Position != चरमः_Accepting_Position) {
                            गच्छ अग्र_चिह्नम्_Pair;
                        }
                    }
                    // A lookahead some distance before the pair (set to BEFORE_PAIR) means we
                    // will come back to the pair in a different configuration if it matches, a
                    // lookahead at position 0 means we get back to the pair on the start state
                    // if it matches; either way, this is covered by other iterations of the
                    // loop over injected states and lookaheads.
                    // For lookaheads at position 1 (in the middle of the pair) and 2 (after the
                    // pair), these have two ways of being safe: either they are always set, or it
                    // does not matter if they are set, because if they match we come back with the
                    // same state.
                    प्रति (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_अन्तर्_Use; ++l) {
                        यदि (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wanted_Lookaheads_अन्तर्_Pair[l]) {
                            अनुवर्त;
                        }
                        यदि (lookaheadPositions[l] == 2 || wanted_Lookaheads_अन्तर्_Pair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            यदि (wanted_अन्तः_State != START_STATE) {
                                गच्छ अग्र_चिह्नम्_Pair;
                            }
                        }
                        यदि (lookaheadPositions[l] == 1 || wanted_Lookaheads_अन्तर्_Pair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            mustCheckConsistentWithStartC2 = सत्य;
                        }
                    }
                }
            }
            यदि (mustCheckConsistentWithStartC2) {
                ध्रुव RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                ध्रुव स्वचालित c2State = startDescriptor.fDtran->elementAti(c2);
                यदि (c2State != wanted_अन्तः_State) {
                    गच्छ अग्र_चिह्नम्_Pair;
                }
                ध्रुव RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                यदि (wanted_चरमः_Accepting_Position != 2) {
                    गच्छ अग्र_चिह्नम्_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                प्रति (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_अन्तर्_Use; ++l) {
                    यदि (((wanted_Lookaheads_अन्तर्_Pair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wanted_अन्तः_State != START_STATE) {
                        गच्छ अग्र_चिह्नम्_Pair;
                    }
                }
            }
            safe_युग्मानि.append(static_cast<char16_t>(c1));
            safe_युग्मानि.append(static_cast<char16_t>(c2));
        अग्र_चिह्नम्_Pair:
            अनुवर्त;
        }
    }

    // Populate the initial safe table.
    // The table as a whole is UVector<UnicodeString>
    // Each row is represented by a UnicodeString, being used as a Vector<int16>.
    // Row 0 is the stop state.
    // Row 1 is the start state.
    // Row 2 and beyond are other states, initially one per char class, but
    //   after initial construction, many of the states will be combined, compacting the table.
    // The String holds the nextState data only. The four leading fields of a row, fAccepting,
    // fLookAhead, etc. are not needed for the safe table, and are omitted at this stage of building.

    U_ASSERT(f_Safe_सारणी == शून्यसूचकः);
    LocalPointer<UVector> lp_Safe_सारणी(
        नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, अङ्कः_Char_Classes + 2, स्थितिः), स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    f_Safe_सारणी = lp_Safe_सारणी.orphan();
    प्रति (int32_t row=0; row<अङ्कः_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(नूतन UnicodeString(अङ्कः_Char_Classes, 0, अङ्कः_Char_Classes+4), स्थितिः);
        f_Safe_सारणी->adoptElement(lpString.orphan(), स्थितिः);
    }
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(f_Safe_सारणी->elementAt(1));
    प्रति (int32_t charClass=0; charClass < अङ्कः_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.समुच्चयः_Char_At(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    प्रति (int32_t row=2; row<अङ्कः_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(f_Safe_सारणी->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    प्रति (int32_t pairIdx=0; pairIdx<safe_युग्मानि.length(); pairIdx+=2) {
        int32_t c1 = safe_युग्मानि.charAt(pairIdx);
        int32_t c2 = safe_युग्मानि.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(f_Safe_सारणी->elementAt(c2 + 2));
        rowState.समुच्चयः_Char_At(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair अवस्थाः = {1, 0};
    यावत् (findDuplicateSafeState(&अवस्थाः)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        removeSafeState(अवस्थाः);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_Safe_सारणी_आकार() ध्रुव {
    int32_t    size = 0;
    int32_t    अङ्कः_पङ्क्तिकाः;
    int32_t    अङ्कः_स्तम्भसङ्ख्या;
    int32_t    row_आकार;

    यदि (f_Safe_सारणी == शून्यसूचकः) {
        प्रत्यागम 0;
    }

    size    = offsetof(RBBIState_सारणी, f_सारणी_दत्त);    // The header, with no rows to the table.

    अङ्कः_पङ्क्तिकाः = f_Safe_सारणी->size();
    अङ्कः_स्तम्भसङ्ख्या = fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories();

    यदि (use8_द्वयङ्काः_For_Safe_सारणी()) {
        row_आकार = offsetof(RBBIState_सारणी_Row8, f_अग्र_State) + परिमाण(int8_t)*अङ्कः_स्तम्भसङ्ख्या;
    } अन्यथा {
        row_आकार = offsetof(RBBIState_सारणी_Row16, f_अग्र_State) + परिमाण(int16_t)*अङ्कः_स्तम्भसङ्ख्या;
    }
    size   += अङ्कः_पङ्क्तिकाः * row_आकार;
    प्रत्यागम size;
}

तार्किक RBBITableBuilder::use8_द्वयङ्काः_For_Safe_सारणी() ध्रुव {
    प्रत्यागम f_Safe_सारणी->size() <= k_Max_State_For8_द्वयङ्काः_सारणी;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::export_Safe_सारणी(शून्य *where) {
    RBBIState_सारणी* सारणी = static_cast<RBBIState_सारणी*>(where);
    uint32_t           state;
    पूर्णाङ्क                col;

    यदि (U_FAILURE(*f_स्थितिः) || f_Safe_सारणी == शून्यसूचकः) {
        प्रत्यागम;
    }

    int32_t cat_गणनम् = fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories();
    यदि (cat_गणनम् > 0x7fff ||
            f_Safe_सारणी->size() > 0x7fff) {
        *f_स्थितिः = U_BRK_आन्तरिकम्_ERROR;
        प्रत्यागम;
    }

    सारणी->f_अङ्कः_अवस्थाः = f_Safe_सारणी->size();
    सारणी->fFlags     = 0;
    यदि (use8_द्वयङ्काः_For_Safe_सारणी()) {
        सारणी->fRowLen    = offsetof(RBBIState_सारणी_Row8, f_अग्र_State) + परिमाण(uint8_t) * cat_गणनम्;
        सारणी->fFlags  |= RBBI_8_द्वयङ्काः_पङ्क्तिकाः;
    } अन्यथा {
        सारणी->fRowLen    = offsetof(RBBIState_सारणी_Row16, f_अग्र_State) + परिमाण(int16_t) * cat_गणनम्;
    }

    प्रति (state=0; state<सारणी->f_अङ्कः_अवस्थाः; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(f_Safe_सारणी->elementAt(state));
        RBBIState_सारणी_Row* row = reinterpret_cast<RBBIState_सारणी_Row*>(सारणी->f_सारणी_दत्त + state * सारणी->fRowLen);
        यदि (use8_द्वयङ्काः_For_Safe_सारणी()) {
            RBBIState_सारणी_Row8* r8 = reinterpret_cast<RBBIState_सारणी_Row8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->f_चिह्नकानि_Idx    = 0;
            प्रति (col=0; col<cat_गणनम्; col++) {
                U_ASSERT(rowString->charAt(col) <= k_Max_State_For8_द्वयङ्काः_सारणी);
                r8->f_अग्र_State[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } अन्यथा {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.f_चिह्नकानि_Idx    = 0;
            प्रति (col=0; col<cat_गणनम्; col++) {
                row->r16.f_अग्र_State[col] = rowString->charAt(col);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBITableBuilder::print_समुच्चयः(UVector *s) {
    int32_t  i;
    प्रति (i=0; i<s->size(); i++) {
        ध्रुव RBBINode *v = static_cast<ध्रुव RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==शून्यसूचकः? -1 : v->f_Serial_अङ्कः);
    }
    RBBIDebugPrintf("\n");
}
#यदि_अन्तः


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBITableBuilder::print_अवस्थाः() {
    पूर्णाङ्क     c;    // input "character"
    पूर्णाङ्क     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    प्रति (c=0; c<fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    प्रति (c=0; c<fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    प्रति (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->f_चिह्नकानि_Idx);
        प्रति (c=0; c<fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#यदि_अन्तः


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBITableBuilder::print_Reverse_सारणी() {
    पूर्णाङ्क     c;    // input "character"
    पूर्णाङ्क     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    यदि (f_Safe_सारणी == शून्यसूचकः) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        प्रत्यागम;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    प्रति (c=0; c<fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    प्रति (c=0; c<fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    प्रति (n=0; n<f_Safe_सारणी->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)f_Safe_सारणी->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        प्रति (c=0; c<fRB->f_समुच्चयः_Builder->get_अङ्कः_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", rowString->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#यदि_अन्तः



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBITableBuilder::print_नियमः_स्थितिः_सारणी() {
    int32_t  this_लेखः = 0;
    int32_t  अग्र_लेखः = 0;
    पूर्णाङ्क      i;
    UVector  *tbl = fRB->f_नियमः_स्थितिः_Vals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    यावत् (अग्र_लेखः < tbl->size()) {
        this_लेखः = अग्र_लेखः;
        अग्र_लेखः = this_लेखः + tbl->elementAti(this_लेखः) + 1;
        RBBIDebugPrintf("%4d   ", this_लेखः);
        प्रति (i=this_लेखः+1; i<अग्र_लेखः; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#यदि_अन्तः


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(पूर्णाङ्क चरमः_निवेशः_चिह्नम्, UError_संहिता *f_स्थितिः) {
    fMarked    = असत्य;
    fAccepting = 0;
    fLookAhead = 0;
    f_चिह्नकानि_Idx   = 0;
    f_चिह्नकम्_Vals   = शून्यसूचकः;
    fPositions = शून्यसूचकः;
    fDtran     = शून्यसूचकः;

    fDtran     = नूतन UVector32(चरमः_निवेशः_चिह्नम्+1, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }
    यदि (fDtran == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    fDtran->समुच्चयः_आकार(चरमः_निवेशः_चिह्नम्+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    विलोपय       fPositions;
    विलोपय       fDtran;
    विलोपय       f_चिह्नकम्_Vals;
    fPositions = शून्यसूचकः;
    fDtran     = शून्यसूचकः;
    f_चिह्नकम्_Vals   = शून्यसूचकः;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
