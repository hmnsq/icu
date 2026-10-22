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

#ಅನ್ತರ್ಭಾವ <functional>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "rbbitblb.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "rbbiscan.h"
#ಅನ್ತರ್ಭಾವ "rbbisetb.h"
#ಅನ್ತರ್ಭಾವ "rbbidata.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
ತಾರ್ಕಿಕ reachableByTransitions(ಧ್ರುವ UVector &states, ಧ್ರುವ int32_t source,
                            ಧ್ರುವ std::function<ತಾರ್ಕಿಕ(int32_t)> isSink,
                            ಧ್ರುವ std::function<ತಾರ್ಕಿಕ(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        ಪ್ರತಿ (int32_t ಚಿಹ್ನೆ = 0; ಚಿಹ್ನೆ < transitionsFromSource.size(); ++ಚಿಹ್ನೆ) {
            ಧ್ರುವ int32_t state = transitionsFromSource.elementAti(ಚಿಹ್ನೆ);
            ಯದಿ (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    ಧ್ರುವ auto visited = prv::make_unique<ತಾರ್ಕಿಕ[]>(states.size(), status);
    ಯಾವತ್ (U_SUCCESS(status) && !boundary.empty()) {
        ಧ್ರುವ int32_t s = boundary.popi();
        ಯದಿ (isSink(s)) {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
        ಯದಿ (excludedState(s) || visited[s]) {
            ಅನುವರ್ತ;
        }
        visited[s] = ಸತ್ಯ;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        ಪ್ರತಿ (int32_t ಚಿಹ್ನೆ = 0; ಚಿಹ್ನೆ < transitions.size(); ++ಚಿಹ್ನೆ) {
            ಧ್ರುವ int32_t t = transitions.elementAti(ಚಿಹ್ನೆ);
            ಯದಿ (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

} // namespace

ಧ್ರುವ int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **ಬೇರು_ಗಂಟು, UErrorCode &status) :
        fRB(rb),
        f_ವೃಕ್ಷ(*ಬೇರು_ಗಂಟು),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = ನೂತನ UVector(status);
    ಯದಿ (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ (i=0; i<fDStates->size(); i++) {
        delete static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    delete fDStates;
    delete fSafeTable;
    delete fLookAheadRuleMap;
}


//-----------------------------------------------------------------------------
//
//   RBBITableBuilder::buildForwardTable  -  This is the main function for building
//                               the DFA state transition table from the RBBI rules parse tree.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ  RBBITableBuilder::buildForwardTable() {

    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    ಯದಿ (f_ವೃಕ್ಷ==nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_ವೃಕ್ಷ = f_ವೃಕ್ಷ->flattenVariables(*fStatus, 0);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
#ifdef RBBI_DEBUG
    ಯದಿ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_ವೃಕ್ಷ(f_ವೃಕ್ಷ, ಸತ್ಯ);
    }
#endif

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = ನೂತನ RBBINode(RBBINode::opCat, *fStatus);
    // Exit if memory allocation failed.
    ಯದಿ (cn == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ (U_FAILURE(*fStatus)) {
        delete cn;
        ಪ್ರತ್ಯಾಗಮ;
    }
    cn->f_ಎಡ_Child = f_ವೃಕ್ಷ;
    f_ವೃಕ್ಷ->fParent = cn;
    RBBINode *end_Marker_ಗಂಟು = cn->f_ಬಲ_Child = ನೂತನ RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    ಯದಿ (cn->f_ಬಲ_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ (U_FAILURE(*fStatus)) {
        delete cn;
        ಪ್ರತ್ಯಾಗಮ;
    }
    cn->f_ಬಲ_Child->fParent = cn;
    f_ವೃಕ್ಷ = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_ವೃಕ್ಷ->flattenSets(*fStatus, 0);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
#ifdef RBBI_DEBUG
    ಯದಿ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_ವೃಕ್ಷ(f_ವೃಕ್ಷ, ಸತ್ಯ);
    }
#endif


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_ವೃಕ್ಷ);
    calcFirstPos(f_ವೃಕ್ಷ);
    calcLastPos(f_ವೃಕ್ಷ);
    calcFollowPos(f_ವೃಕ್ಷ);
    ಯದಿ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_ವೃಕ್ಷ);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    ಯದಿ (fRB->fChainRules) {
        calcChainedFollowPos(f_ವೃಕ್ಷ, end_Marker_ಗಂಟು);
    }

    //
    // Build the DFA state transition tables.
    //
    buildStateTable();
    mapLookAheadRules();
    flagAcceptingStates();
    flagLookAheadStates();
    flagTaggedStates();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    mergeRuleStatusVals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calcNullable(RBBINode *n) {
    ಯದಿ (n == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = ಅಸತ್ಯ;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = ಸತ್ಯ;
        ಪ್ರತ್ಯಾಗಮ;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_ಎಡ_Child);
    calcNullable(n->f_ಬಲ_Child);

    // Apply functions from table 3.40 in Aho
    ಯದಿ (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_ಎಡ_Child->fNullable || n->f_ಬಲ_Child->fNullable;
    }
    ಅನ್ಯಥಾ ಯದಿ (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_ಎಡ_Child->fNullable && n->f_ಬಲ_Child->fNullable;
    }
    ಅನ್ಯಥಾ ಯದಿ (n->fType == RBBINode::opStar || n->fType == RBBINode::opQuestion) {
        n->fNullable = ಸತ್ಯ;
    }
    ಅನ್ಯಥಾ {
        n->fNullable = ಅಸತ್ಯ;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calcFirstPos(RBBINode *n) {
    ಯದಿ (n == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fFirstPosSet->addElement(n, *fStatus);
        ಪ್ರತ್ಯಾಗಮ;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_ಎಡ_Child);
    calcFirstPos(n->f_ಬಲ_Child);

    // Apply functions from table 3.40 in Aho
    ಯದಿ (n->fType == RBBINode::opOr) {
        setAdd(n->fFirstPosSet, n->f_ಎಡ_Child->fFirstPosSet);
        setAdd(n->fFirstPosSet, n->f_ಬಲ_Child->fFirstPosSet);
    }
    ಅನ್ಯಥಾ ಯದಿ (n->fType == RBBINode::opCat) {
        setAdd(n->fFirstPosSet, n->f_ಎಡ_Child->fFirstPosSet);
        ಯದಿ (n->f_ಎಡ_Child->fNullable) {
            setAdd(n->fFirstPosSet, n->f_ಬಲ_Child->fFirstPosSet);
        }
    }
    ಅನ್ಯಥಾ ಯದಿ (n->fType == RBBINode::opStar ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fFirstPosSet, n->f_ಎಡ_Child->fFirstPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calcLastPos(RBBINode *n) {
    ಯದಿ (n == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fLastPosSet->addElement(n, *fStatus);
        ಪ್ರತ್ಯಾಗಮ;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calcLastPos(n->f_ಎಡ_Child);
    calcLastPos(n->f_ಬಲ_Child);

    // Apply functions from table 3.40 in Aho
    ಯದಿ (n->fType == RBBINode::opOr) {
        setAdd(n->fLastPosSet, n->f_ಎಡ_Child->fLastPosSet);
        setAdd(n->fLastPosSet, n->f_ಬಲ_Child->fLastPosSet);
    }
    ಅನ್ಯಥಾ ಯದಿ (n->fType == RBBINode::opCat) {
        setAdd(n->fLastPosSet, n->f_ಬಲ_Child->fLastPosSet);
        ಯದಿ (n->f_ಬಲ_Child->fNullable) {
            setAdd(n->fLastPosSet, n->f_ಎಡ_Child->fLastPosSet);
        }
    }
    ಅನ್ಯಥಾ ಯದಿ (n->fType == RBBINode::opStar     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fLastPosSet, n->f_ಎಡ_Child->fLastPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calcFollowPos(RBBINode *n) {
    ಯದಿ (n == nullptr ||
        n->fType == RBBINode::leafChar ||
        n->fType == RBBINode::endMark) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    calcFollowPos(n->f_ಎಡ_Child);
    calcFollowPos(n->f_ಬಲ_Child);

    // Aho rule #1
    ಯದಿ (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_ಎಡ_Child = n->f_ಎಡ_Child->fLastPosSet;

        ಪ್ರತಿ (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_ಎಡ_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_ಎಡ_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_ಬಲ_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    ಯದಿ (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        ಪ್ರತಿ (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
            i = static_cast<RBBINode*>(n->fLastPosSet->elementAt(ix));
            setAdd(i->fFollowPos, n->fFirstPosSet);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::add_Rule_ಬೇರು_Nodes(UVector *dest, RBBINode *ಗಂಟು) {
    ಯದಿ (ಗಂಟು == nullptr || U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    U_ASSERT(!dest->hasDeleter());
    ಯದಿ (ಗಂಟು->f_Rule_ಬೇರು) {
        dest->addElement(ಗಂಟು, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        ಪ್ರತ್ಯಾಗಮ;
    }
    add_Rule_ಬೇರು_Nodes(dest, ಗಂಟು->f_ಎಡ_Child);
    add_Rule_ಬೇರು_Nodes(dest, ಗಂಟು->f_ಬಲ_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calcChainedFollowPos(RBBINode *ವೃಕ್ಷ, RBBINode *end_Mark_ಗಂಟು) {

    UVector         leafNodes(*fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    // get a list all leaf nodes
    ವೃಕ್ಷ->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_ಬೇರು_Nodes(*fStatus);
    add_Rule_ಬೇರು_Nodes(&rule_ಬೇರು_Nodes, ವೃಕ್ಷ);

    UVector matchStartNodes(*fStatus);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j=0; j<rule_ಬೇರು_Nodes.size(); ++j) {
        RBBINode *ಗಂಟು = static_cast<RBBINode *>(rule_ಬೇರು_Nodes.elementAt(j));
        ಯದಿ (ಗಂಟು->fChainIn) {
            setAdd(&matchStartNodes, ಗಂಟು->fFirstPosSet);
        }
    }
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t  end_ಗಂಟು_Ix;
    int32_t  start_ಗಂಟು_Ix;

    ಪ್ರತಿ (end_ಗಂಟು_Ix=0; end_ಗಂಟು_Ix<leafNodes.size(); end_ಗಂಟು_Ix++) {
        RBBINode* end_ಗಂಟು = static_cast<RBBINode*>(leafNodes.elementAt(end_ಗಂಟು_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        ಯದಿ (!end_ಗಂಟು->fFollowPos->contains(end_Mark_ಗಂಟು)) {
            ಅನುವರ್ತ;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_ಗಂಟು;
        ಪ್ರತಿ (start_ಗಂಟು_Ix = 0; start_ಗಂಟು_Ix<matchStartNodes.size(); start_ಗಂಟು_Ix++) {
            start_ಗಂಟು = static_cast<RBBINode*>(matchStartNodes.elementAt(start_ಗಂಟು_Ix));
            ಯದಿ (start_ಗಂಟು->fType != RBBINode::leafChar) {
                ಅನುವರ್ತ;
            }

            ಯದಿ (end_ಗಂಟು->fVal == start_ಗಂಟು->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_ಗಂಟು->fFollowPos, start_ಗಂಟು->fFollowPos);
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
ಶೂನ್ಯ RBBITableBuilder::bofFixup() {

    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
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
    RBBINode  *bof_ಗಂಟು = f_ವೃಕ್ಷ->f_ಎಡ_Child->f_ಎಡ_Child;
    U_ASSERT(bof_ಗಂಟು->fType == RBBINode::leafChar);
    U_ASSERT(bof_ಗಂಟು->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_ವೃಕ್ಷ->f_ಎಡ_Child->f_ಬಲ_Child->fFirstPosSet;

    RBBINode *start_ಗಂಟು;
    ಪೂರ್ಣಾಙ್ಕ       start_ಗಂಟು_Ix;
    ಪ್ರತಿ (start_ಗಂಟು_Ix = 0; start_ಗಂಟು_Ix<matchStartNodes->size(); start_ಗಂಟು_Ix++) {
        start_ಗಂಟು = static_cast<RBBINode*>(matchStartNodes->elementAt(start_ಗಂಟು_Ix));
        ಯದಿ (start_ಗಂಟು->fType != RBBINode::leafChar) {
            ಅನುವರ್ತ;
        }

        ಯದಿ (start_ಗಂಟು->fVal == bof_ಗಂಟು->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_ಗಂಟು->fFollowPos, start_ಗಂಟು->fFollowPos);
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
ಶೂನ್ಯ RBBITableBuilder::buildStateTable() {
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    ಪೂರ್ಣಾಙ್ಕ      last_ಒಳಹರಿವು_ಚಿಹ್ನೆ = fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories() - 1;
    failState = ನೂತನ RBBIStateDescriptor(last_ಒಳಹರಿವು_ಚಿಹ್ನೆ, fStatus);
    ಯದಿ (failState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ಗಚ್ಛ ExitBuildSTdeleteall;
    }
    failState->fPositions = ನೂತನ UVector(*fStatus);
    ಯದಿ (failState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ (failState->fPositions == nullptr || U_FAILURE(*fStatus)) {
        ಗಚ್ಛ ExitBuildSTdeleteall;
    }
    fDStates->addElement(failState, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಗಚ್ಛ ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = ನೂತನ RBBIStateDescriptor(last_ಒಳಹರಿವು_ಚಿಹ್ನೆ, fStatus);
    ಯದಿ (initialState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಗಚ್ಛ ExitBuildSTdeleteall;
    }
    initialState->fPositions = ನೂತನ UVector(*fStatus);
    ಯದಿ (initialState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಗಚ್ಛ ExitBuildSTdeleteall;
    }
    setAdd(initialState->fPositions, f_ವೃಕ್ಷ->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಗಚ್ಛ ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    ಪ್ರತಿ (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        ಪ್ರತಿ (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *temp;
            temp = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            ಯದಿ (temp->fMarked == ಅಸತ್ಯ) {
                T = temp;
                ಭಙ್ಗ;
            }
        }
        ಯದಿ (T == nullptr) {
            ಭಙ್ಗ;
        }

        // mark T;
        T->fMarked = ಸತ್ಯ;

        // for each input symbol a do begin
        int32_t  a;
        ಪ್ರತಿ (a = 1; a<=last_ಒಳಹರಿವು_ಚಿಹ್ನೆ; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            ಪ್ರತಿ (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                ಯದಿ ((p->fType == RBBINode::leafChar) &&  (p->fVal == a)) {
                    ಯದಿ (U == nullptr) {
                        U = ನೂತನ UVector(*fStatus);
                        ಯದಿ (U == nullptr) {
                        	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                        	ಗಚ್ಛ ExitBuildSTdeleteall;
                        }
                    }
                    setAdd(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = ಅಸತ್ಯ;
            ಯದಿ (U != nullptr) {
                U_ASSERT(U->size() > 0);
                ಪೂರ್ಣಾಙ್ಕ  ix;
                ಪ್ರತಿ (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    ಯದಿ (setEquals(U, temp2->fPositions)) {
                        delete U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = ಸತ್ಯ;
                        ಭಙ್ಗ;
                    }
                }

                // Add U as an unmarked state to Dstates
                ಯದಿ (!UinDstates)
                {
                    RBBIStateDescriptor *newState = ನೂತನ RBBIStateDescriptor(last_ಒಳಹರಿವು_ಚಿಹ್ನೆ, fStatus);
                    ಯದಿ (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    ಯದಿ (U_FAILURE(*fStatus)) {
                        ಗಚ್ಛ ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    ಯದಿ (U_FAILURE(*fStatus)) {
                        ಪ್ರತ್ಯಾಗಮ;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    delete initialState;
    delete failState;
}


/**
 * mapLookAheadRules
 *
 */
ಶೂನ್ಯ RBBITableBuilder::mapLookAheadRules() {
    fLookAheadRuleMap =  ನೂತನ UVector32(fRB->fScanner->ಅಂಕೆ_Rules() + 1, *fStatus);
    ಯದಿ (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    fLookAheadRuleMap->setSize(fRB->fScanner->ಅಂಕೆ_Rules() + 1);

    ಪ್ರತಿ (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        ತಾರ್ಕಿಕ saw_Look_Ahead_ಗಂಟು = ಅಸತ್ಯ;
        ಪ್ರತಿ (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ಗಂಟು = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            ಯದಿ (ಗಂಟು->fType != RBBINode::ಗಂಟು_Type::lookAhead) {
                ಅನುವರ್ತ;
            }
            saw_Look_Ahead_ಗಂಟು = ಸತ್ಯ;
            int32_t rule_ಅಂಕೆ = ಗಂಟು->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_ಅಂಕೆ < fLookAheadRuleMap->size());
            U_ASSERT(rule_ಅಂಕೆ > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_ಅಂಕೆ);
            ಯದಿ (laSlot != 0) {
                ಯದಿ (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } ಅನ್ಯಥಾ {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        ಯದಿ (!saw_Look_Ahead_ಗಂಟು) {
            ಅನುವರ್ತ;
        }

        ಯದಿ (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        ಪ್ರತಿ (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ಗಂಟು = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            ಯದಿ (ಗಂಟು->fType != RBBINode::ಗಂಟು_Type::lookAhead) {
                ಅನುವರ್ತ;
            }
            int32_t rule_ಅಂಕೆ = ಗಂಟು->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = fLookAheadRuleMap->elementAti(rule_ಅಂಕೆ);
            (ಶೂನ್ಯ)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            fLookAheadRuleMap->setElementAt(laSlotForState, rule_ಅಂಕೆ);
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
ಶೂನ್ಯ     RBBITableBuilder::flagAcceptingStates() {
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    f_ವೃಕ್ಷ->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        ಪ್ರತಿ (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            ಯದಿ (sd->fPositions->indexOf(endMarker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                ಯದಿ (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
                    ಯದಿ (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                ಯದಿ (sd->fAccepting==ACCEPTING_UNCONDITIONAL && endMarker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
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
ಶೂನ್ಯ     RBBITableBuilder::flagLookAheadStates() {
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_ಗಂಟು;
    int32_t     i;
    int32_t     n;

    f_ವೃಕ್ಷ->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_ಗಂಟು = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_ಗಂಟು->fType == RBBINode::ಗಂಟು_Type::lookAhead);

        ಪ್ರತಿ (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->indexOf(look_Ahead_ಗಂಟು);
            ಯದಿ (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_ಗಂಟು == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_ಗಂಟು->fVal);
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
ಶೂನ್ಯ     RBBITableBuilder::flagTaggedStates() {
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_ಗಂಟು;
    int32_t     i;
    int32_t     n;

    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    f_ವೃಕ್ಷ->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_ಗಂಟು = static_cast<RBBINode*>(tagNodes.elementAt(i));

        ಪ್ರತಿ (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            ಯದಿ (sd->fPositions->indexOf(tag_ಗಂಟು) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_ಗಂಟು->fVal);
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
ಶೂನ್ಯ  RBBITableBuilder::mergeRuleStatusVals() {
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
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    ಯದಿ (fRB->fRuleStatusVals->size() == 0) {
        fRB->fRuleStatusVals->addElement(1, *fStatus);  // Num of statuses in group
        fRB->fRuleStatusVals->addElement(static_cast<int32_t>(0), *fStatus); // and our single status of zero
    }

    //    For each state
    ಪ್ರತಿ (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_ಮೌಲ್ಯಗಳು = sd->fTagVals;
        ಯದಿ (this_States_Tag_ಮೌಲ್ಯಗಳು == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            ಅನುವರ್ತ;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  nextTagGroupStart = 0;

        // Loop runs once per group of tags in the global list
        ಯಾವತ್ (nextTagGroupStart < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = nextTagGroupStart;
            nextTagGroupStart += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            ಯದಿ (this_States_Tag_ಮೌಲ್ಯಗಳು->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                ಅನುವರ್ತ;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            ಪ್ರತಿ (i=0; i<this_States_Tag_ಮೌಲ್ಯಗಳು->size(); i++) {
                ಯದಿ (this_States_Tag_ಮೌಲ್ಯಗಳು->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    ಭಙ್ಗ;
                }
            }

            ಯದಿ (i == this_States_Tag_ಮೌಲ್ಯಗಳು->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                ಭಙ್ಗ;
            }
        }

        ಯದಿ (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_ಮೌಲ್ಯಗಳು->size(), *fStatus);
            ಪ್ರತಿ (i=0; i<this_States_Tag_ಮೌಲ್ಯಗಳು->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_ಮೌಲ್ಯಗಳು->elementAti(i), *fStatus);
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

ಶೂನ್ಯ RBBITableBuilder::minimizeStates() {
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

#ಯದಿ UPRV_HAS_SANITIZER
    ಯದಿ (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    ಸಂರಚನಾ StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        ತಾರ್ಕಿಕ operator==(StateType ಧ್ರುವ &other) ಧ್ರುವ {
            ಪ್ರತ್ಯಾಗಮ fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   fTagsIdx == other.fTagsIdx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    ಸಂರಚನಾ TypeToStates : UMemory {
        TypeToStates(ಧ್ರುವ StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](ಶೂನ್ಯ *p) { delete static_cast<TypeToStates *>(p); });
    ಪ್ರತಿ (int32_t i = 0; i < fDStates->size(); ++i) {
        ಧ್ರುವ RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ಧ್ರುವ StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        ಪ್ರತಿ (; j < initialPartition.size(); ++j) {
            auto &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            ಯದಿ (type_j == type) {
                states->addElement(i, *fStatus);
                ಭಙ್ಗ;
            }
        }
        ಯದಿ (j == initialPartition.size()) {
            auto newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            ಯದಿ (U_FAILURE(*fStatus)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    auto partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    partition->setDeleter(
        [](ಶೂನ್ಯ *p) { delete static_cast<UVector*>(p); });
    
    ಪ್ರತಿ (int32_t i = 0; i < initialPartition.size(); ++i) {
        partition->adoptElement(
            static_cast<TypeToStates*>(initialPartition.elementAt(i))->states.release(),
            *fStatus);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    auto partition_signature = [&partition, ಏತತ್](int32_t stateIndex) -> prv::unique_ptr<UVector> {
        ಧ್ರುವ RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(stateIndex));
        auto result = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        ಯದಿ (U_FAILURE(*fStatus)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಪ್ರತಿ (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            ಪ್ರತಿ (int32_t partIndex = 0; partIndex < partition->size(); ++partIndex) {
                ಧ್ರುವ UVector &part = *static_cast<UVector *>(partition->elementAt(partIndex));
                ಯದಿ (part.contains(toState)) {
                    result->addElement(partIndex, *fStatus);
                    ಭಙ್ಗ;
                }
            }
        }
        result->setComparer([](ಧ್ರುವ UElement left, ಧ್ರುವ UElement ಬಲ) -> UBool {
            ಪ್ರತ್ಯಾಗಮ left.integer == ಬಲ.integer;
        });
        ಪ್ರತ್ಯಾಗಮ result;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    ಪ್ರತಿ (;;) {
        // Π_new.
        auto partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        ಯದಿ (U_FAILURE(*fStatus)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        partitionNew->setDeleter(
            [](ಶೂನ್ಯ *p) { delete static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        ತಾರ್ಕಿಕ refined = ಅಸತ್ಯ;
        ಪ್ರತಿ (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ಧ್ರುವ UVector &group = *static_cast<UVector*>(partition->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            ಸಂರಚನಾ SignatureToStates : UMemory {
                SignatureToStates(prv::unique_ptr<UVector> signature, UErrorCode &status)
                    : signature(std::move(signature)), states(prv::make_unique<UVector>(status, status)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> states;
            };
            UVector groupRefinement(*fStatus);
            groupRefinement.setDeleter([](ಶೂನ್ಯ *p) { delete static_cast<SignatureToStates*>(p); });
            ಪ್ರತಿ (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                ಧ್ರುವ int32_t s = group.elementAti(j);
                auto signature = partition_signature(s);
                int32_t k = 0;
                ಪ್ರತಿ (; k < groupRefinement.size(); ++k) {
                    ಧ್ರುವ auto &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    ಯದಿ (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        ಭಙ್ಗ;
                    }
                }
                ಯದಿ (k == groupRefinement.size()) {
                    ಧ್ರುವ auto newEntry =
                        ನೂತನ SignatureToStates(std::move(signature), *fStatus);
                    ಯದಿ (U_FAILURE(*fStatus)) {
                        ಪ್ರತ್ಯಾಗಮ;
                    }
                    ಯದಿ (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      ಪ್ರತ್ಯಾಗಮ;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            ಪ್ರತಿ (int32_t j = 0; j < groupRefinement.size(); ++j) {
                auto &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
            }
        }
        ಯದಿ (refined) {
            partition = std::move(partitionNew);
        } ಅನ್ಯಥಾ {
            ಭಙ್ಗ;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < partition->size(); ++i) {
        UVector *ಧ್ರುವ part_i = static_cast<UVector *>(partition->elementAt(i));
        ಯದಿ (part_i->contains(0)) {
            ಶೂನ್ಯ *ಧ್ರುವ part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ಧ್ರುವ auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } ಅನ್ಯಥಾ ಯದಿ (part_i->contains(1)) {
            ಶೂನ್ಯ *ಧ್ರುವ part_1 = partition->elementAt(1);
            ಧ್ರುವ auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    auto oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < partition->size(); ++i) {
        ಧ್ರುವ UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    auto ಧ್ರುವ oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < partition->size(); ++i) {
        ಧ್ರುವ UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *ಧ್ರುವ state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->setElementAt(oldStateToPart[state->fDtran->elementAti(j)], j);
        }
        oldStates->setElementAt(nullptr, part.elementAti(0));
    }
    oldStates->setDeleter([](ಶೂನ್ಯ *p) { delete static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::sortedAdd(UVector **vector, int32_t val) {
    int32_t i;

    ಯದಿ (*vector == nullptr) {
        *vector = ನೂತನ UVector(*fStatus);
    }
    ಯದಿ (*vector == nullptr || U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    UVector *vec = *vector;
    int32_t  vSize = vec->size();
    ಪ್ರತಿ (i=0; i<vSize; i++) {
        int32_t valAtI = vec->elementAti(i);
        ಯದಿ (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (valAtI > val) {
            ಭಙ್ಗ;
        }
    }
    vec->ಸೇರಿಸು_Element_At(val, i, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::setAdd(UVector *dest, UVector *source) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!source->hasDeleter());
    int32_t destOriginalSize = dest->size();
    int32_t sourceSize       = source->size();
    int32_t di           = 0;
    Maybe_ರಾಶಿ_Array<ಶೂನ್ಯ *, 16> destArray, sourceArray;  // Handle small cases without malloc
    ಶೂನ್ಯ **destPtr, **sourcePtr;
    ಶೂನ್ಯ **destLim, **sourceLim;

    ಯದಿ (destOriginalSize > destArray.getCapacity()) {
        ಯದಿ (destArray.resize(destOriginalSize) == nullptr) {
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + destOriginalSize;  // destArray.getArrayLimit()?

    ಯದಿ (sourceSize > sourceArray.getCapacity()) {
        ಯದಿ (sourceArray.resize(sourceSize) == nullptr) {
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + sourceSize;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (ಶೂನ್ಯ) dest->toArray(destPtr);
    (ಶೂನ್ಯ) source->toArray(sourcePtr);

    dest->setSize(sourceSize+destOriginalSize, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯಾವತ್ (sourcePtr < sourceLim && destPtr < destLim) {
        ಯದಿ (*destPtr == *sourcePtr) {
            dest->setElementAt(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        ಅನ್ಯಥಾ ಯದಿ (uprv_memcmp(destPtr, sourcePtr, ಪರಿಮಾಣ(ಶೂನ್ಯ *)) < 0) {
            dest->setElementAt(*destPtr++, di++);
        }
        ಅನ್ಯಥಾ { /* *sourcePtr < *destPtr */
            dest->setElementAt(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    ಯಾವತ್ (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    ಯಾವತ್ (sourcePtr < sourceLim) {
        dest->setElementAt(*sourcePtr++, di++);
    }

    dest->setSize(di, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::setEquals(UVector *a, UVector *b) {
    ಪ್ರತ್ಯಾಗಮ a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::printPosSets(RBBINode *n) {
    ಯದಿ (n==nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    printf("\n");
    RBBINode::print_ಗಂಟು_Header();
    RBBINode::print_ಗಂಟು(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    printSet(n->fFirstPosSet);

    RBBIDebugPrintf("         lastpos:   ");
    printSet(n->fLastPosSet);

    RBBIDebugPrintf("         followpos: ");
    printSet(n->fFollowPos);

    printPosSets(n->f_ಎಡ_Child);
    printPosSets(n->f_ಬಲ_Child);
}
#endif

//
//    findDuplCharClassFrom()
//
ತಾರ್ಕಿಕ RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t ಅಂಕೆ_States = fDStates->size();
    int32_t ಅಂಕೆ_Cols = fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories();

    ಪ್ರತಿ (; categories->first < ಅಂಕೆ_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        ಪೂರ್ಣಾಙ್ಕ limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : ಅಂಕೆ_Cols;
        ಪ್ರತಿ (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            ಪ್ರತಿ (int32_t state=0; state<ಅಂಕೆ_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                ಯದಿ (table_base != table_dupl) {
                    ಭಙ್ಗ;
                }
            }
            ಯದಿ (table_base == table_dupl) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}


//
//    removeColumn()
//
ಶೂನ್ಯ RBBITableBuilder::removeColumn(int32_t column) {
    int32_t ಅಂಕೆ_States = fDStates->size();
    ಪ್ರತಿ (int32_t state=0; state<ಅಂಕೆ_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


ತಾರ್ಕಿಕ RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t ಅಂಕೆ_States = fSafeTable->size();

    ಪ್ರತಿ (; states->first<ಅಂಕೆ_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        ಪ್ರತಿ (states->second=states->first+1; states->second<ಅಂಕೆ_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            ತಾರ್ಕಿಕ rowsMatch = ಸತ್ಯ;
            int32_t ಅಂಕೆ_Cols = firstRow->length();
            ಪ್ರತಿ (int32_t col=0; col < ಅಂಕೆ_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                ಯದಿ (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = ಅಸತ್ಯ;
                    ಭಙ್ಗ;
                }
            }
            ಯದಿ (rowsMatch) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ RBBITableBuilder::removeSafeState(IntPair duplStates) {
    ಧ್ರುವ int32_t keepState = duplStates.first;
    ಧ್ರುವ int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t ಅಂಕೆ_States = fSafeTable->size();
    ಪ್ರತಿ (int32_t state=0; state<ಅಂಕೆ_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t ಅಂಕೆ_Cols = sd->length();
        ಪ್ರತಿ (int32_t col=0; col<ಅಂಕೆ_Cols; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            ಯದಿ (existingVal == duplState) {
                newVal = keepState;
            } ಅನ್ಯಥಾ ಯದಿ (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->setCharAt(col, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::removeDuplicateStates() {
    ಧ್ರುವ int32_t old_State_ಎಣಿಕೆ = fDStates->size();
    minimizeStates();
    ಪ್ರತ್ಯಾಗಮ fDStates->size() - old_State_ಎಣಿಕೆ;
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
ಶೂನ್ಯ RBBITableBuilder::minimizeLookaheads() {
    ಯದಿ (fLASlotsInUse == ACCEPTING_UNCONDITIONAL) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಧ್ರುವ int32_t lookahead_ಎಣಿಕೆ = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ಧ್ರುವ int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    ಧ್ರುವ auto stateDescriptor = [ಏತತ್](ಧ್ರುವ int32_t state) -> RBBIStateDescriptor & {
        ಪ್ರತ್ಯಾಗಮ *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ಧ್ರುವ auto lookaheadReachability =
        prv::make_unique<ತಾರ್ಕಿಕ[]>(lookahead_ಎಣಿಕೆ * lookahead_ಎಣಿಕೆ, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        ಪ್ರತಿ (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
            ಯದಿ (k != l) {
                ಪ್ರತಿ (int32_t source = 1; source < fDStates->size(); ++source) {
                    ಯದಿ (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        ಅನುವರ್ತ;
                    }
                    ಯದಿ (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](ಧ್ರುವ int32_t state) {
                                ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ಧ್ರುವ int32_t state) {
                                ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_ಎಣಿಕೆ + k -
                                              firstLookahead] = ಸತ್ಯ;
                    }
                }
            }
        }
    }

    ಧ್ರುವ auto colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_ಎಣಿಕೆ, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    ಪ್ರತಿ (int32_t chromatic_ಸಂಖ್ಯೆ = 1;; ++chromatic_ಸಂಖ್ಯೆ) {
        uprv_memset(colours.get(), 0, ಪರಿಮಾಣ(int32_t) * lookahead_ಎಣಿಕೆ);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        ಪ್ರತಿ (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ source = 0; source < lookahead_ಎಣಿಕೆ; ++source) {
                ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ sink = 0; sink < lookahead_ಎಣಿಕೆ; ++sink) {
                    ಯದಿ (lookaheadReachability[source * lookahead_ಎಣಿಕೆ + sink] &&
                        colours[source] == colours[sink]) {
                        ಗಚ್ಛ nextColouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < fDStates->size(); ++i) {
                ಯದಿ (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        firstLookahead +
                        colours[stateDescriptor(i).fAccepting - firstLookahead];
                }
                ಯದಿ (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - firstLookahead];
                }
            }
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_ಸಂಖ್ಯೆ;
            ಪ್ರತ್ಯಾಗಮ;

          nextColouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            ಪ್ರತಿ (int32_t i = 0; i < lookahead_ಎಣಿಕೆ - 1 && colours[i] == chromatic_ಸಂಖ್ಯೆ;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            ಯದಿ (colours[lookahead_ಎಣಿಕೆ - 1] == chromatic_ಸಂಖ್ಯೆ) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                ಭಙ್ಗ;
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
int32_t  RBBITableBuilder::getTableSize() ಧ್ರುವ {
    int32_t    size = 0;
    int32_t    ಅಂಕೆ_Rows;
    int32_t    ಅಂಕೆ_Cols;
    int32_t    rowSize;

    ಯದಿ (f_ವೃಕ್ಷ == nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    ಅಂಕೆ_Rows = fDStates->size();
    ಅಂಕೆ_Cols = fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories();

    ಯದಿ (use8BitsForTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + ಪರಿಮಾಣ(int8_t)*ಅಂಕೆ_Cols;
    } ಅನ್ಯಥಾ {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + ಪರಿಮಾಣ(int16_t)*ಅಂಕೆ_Cols;
    }
    size   += ಅಂಕೆ_Rows * rowSize;
    ಪ್ರತ್ಯಾಗಮ size;
}

ತಾರ್ಕಿಕ RBBITableBuilder::use8BitsForTable() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ fDStates->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::exportTable(ಶೂನ್ಯ *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    ಪೂರ್ಣಾಙ್ಕ                col;

    ಯದಿ (U_FAILURE(*fStatus) || f_ವೃಕ್ಷ == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t cat_ಎಣಿಕೆ = fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories();
    ಯದಿ (cat_ಎಣಿಕೆ > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    table->f_ಅಂಕೆ_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->fLookAheadResultsSize = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    ಯದಿ (use8BitsForTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + ಪರಿಮಾಣ(uint8_t) * cat_ಎಣಿಕೆ;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } ಅನ್ಯಥಾ {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + ಪರಿಮಾಣ(int16_t) * cat_ಎಣಿಕೆ;
    }
    ಯದಿ (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    ಪ್ರತಿ (state=0; state<table->f_ಅಂಕೆ_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        ಯದಿ (use8BitsForTable()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 255);
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->fTagsIdx   = sd->fTagsIdx;
            ಪ್ರತಿ (col=0; col<cat_ಎಣಿಕೆ; col++) {
                U_ASSERT (sd->fDtran->elementAti(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = sd->fDtran->elementAti(col);
            }
        } ಅನ್ಯಥಾ {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 0xffff);
            row->r16.fAccepting = sd->fAccepting;
            row->r16.fLookAhead = sd->fLookAhead;
            row->r16.fTagsIdx   = sd->fTagsIdx;
            ಪ್ರತಿ (col=0; col<cat_ಎಣಿಕೆ; col++) {
                row->r16.fNextState[col] = sd->fDtran->elementAti(col);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
ಶೂನ್ಯ RBBITableBuilder::buildSafeReverseTable(UErrorCode &status) {
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
    UnicodeString safePairs;

    int32_t ಅಂಕೆ_Char_Classes = fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories();
    int32_t ಅಂಕೆ_States = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ಧ್ರುವ auto lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    ಧ್ರುವ auto wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    ಪ್ರತಿ (int32_t c1 = 1; c1 < ಅಂಕೆ_Char_Classes; ++c1) {
        ಯದಿ (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          ಅನುವರ್ತ;
        }
        ಪ್ರತಿ (int32_t c2 = 1; c2 < ಅಂಕೆ_Char_Classes; ++c2) {
            ಯದಿ (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                ಅನುವರ್ತ;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            ತಾರ್ಕಿಕ mustCheckConsistentWithStartC2 = ಅಸತ್ಯ;
            ಧ್ರುವ int32_t text[2] = {c1, c2};
            ಪ್ರತಿ (int32_t injectedState = START_STATE; injectedState < ಅಂಕೆ_States; ++injectedState) {
                ಪ್ರತಿ (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    ಯದಿ (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ಧ್ರುವ RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        ಯದಿ (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
                        } ಅನ್ಯಥಾ ಯದಿ (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            ಅನುವರ್ತ;
                        }
                        ಯದಿ (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    ಗಚ್ಛ dfaStep;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    ಪ್ರತಿ (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        ಪ್ರತಿ (;;) {
                        dfaStep:
                            ಯದಿ (i == BEFORE_PAIR || i == 2) {
                                ಗಚ್ಛ inspectFinalState;
                            }
                            ಧ್ರುವ int32_t ಚಿಹ್ನೆ_Ahead = text[i];
                            ++i;
                            ಧ್ರುವ RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            ಯದಿ (sDescriptor->fDtran->elementAti(ಚಿಹ್ನೆ_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(ಚಿಹ್ನೆ_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } ಅನ್ಯಥಾ {
                                ಯದಿ (lastBreak != BEFORE_PAIR &&
                                    lastAcceptingPosition == lastBreak) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    ಗಚ್ಛ next_ಚಿಹ್ನೆ_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                ಭಙ್ಗ;
                            }

                            ಯದಿ (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } ಅನ್ಯಥಾ ಯದಿ (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
                                ಭಙ್ಗ;
                            }
                            ಯದಿ (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    ಯದಿ (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        ಅನುವರ್ತ;
                    }
                    ಯದಿ (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        ಪ್ರತಿ (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                            ಯದಿ (lookaheadPositions[l] != LOOKAHEAD_NOT_SET &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wantedLookaheadsInPair[l] = lookaheadPositions[i];
                            } ಅನ್ಯಥಾ {
                                wantedLookaheadsInPair[l] = LOOKAHEAD_NOT_SET;
                            }
                        }
                    } ಅನ್ಯಥಾ {
                        ಯದಿ (wantedEndState != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            ಗಚ್ಛ next_ಚಿಹ್ನೆ_Pair;
                        }
                        ಯದಿ (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            ಗಚ್ಛ next_ಚಿಹ್ನೆ_Pair;
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
                    ಪ್ರತಿ (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        ಯದಿ (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            ಅನುವರ್ತ;
                        }
                        ಯದಿ (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            ಯದಿ (wantedEndState != START_STATE) {
                                ಗಚ್ಛ next_ಚಿಹ್ನೆ_Pair;
                            }
                        }
                        ಯದಿ (lookaheadPositions[l] == 1 || wantedLookaheadsInPair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            mustCheckConsistentWithStartC2 = ಸತ್ಯ;
                        }
                    }
                }
            }
            ಯದಿ (mustCheckConsistentWithStartC2) {
                ಧ್ರುವ RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                ಧ್ರುವ auto c2State = startDescriptor.fDtran->elementAti(c2);
                ಯದಿ (c2State != wantedEndState) {
                    ಗಚ್ಛ next_ಚಿಹ್ನೆ_Pair;
                }
                ಧ್ರುವ RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                ಯದಿ (wantedLastAcceptingPosition != 2) {
                    ಗಚ್ಛ next_ಚಿಹ್ನೆ_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                ಪ್ರತಿ (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    ಯದಿ (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        ಗಚ್ಛ next_ಚಿಹ್ನೆ_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        next_ಚಿಹ್ನೆ_Pair:
            ಅನುವರ್ತ;
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

    U_ASSERT(fSafeTable == nullptr);
    LocalPointer<UVector> lpSafeTable(
        ನೂತನ UVector(uprv_deleteUObject, uhash_compareUnicodeString, ಅಂಕೆ_Char_Classes + 2, status), status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    fSafeTable = lpSafeTable.orphan();
    ಪ್ರತಿ (int32_t row=0; row<ಅಂಕೆ_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(ನೂತನ UnicodeString(ಅಂಕೆ_Char_Classes, 0, ಅಂಕೆ_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    ಪ್ರತಿ (int32_t charClass=0; charClass < ಅಂಕೆ_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    ಪ್ರತಿ (int32_t row=2; row<ಅಂಕೆ_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    ಪ್ರತಿ (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    ಯಾವತ್ (findDuplicateSafeState(&states)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        removeSafeState(states);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::getSafeTableSize() ಧ್ರುವ {
    int32_t    size = 0;
    int32_t    ಅಂಕೆ_Rows;
    int32_t    ಅಂಕೆ_Cols;
    int32_t    rowSize;

    ಯದಿ (fSafeTable == nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    ಅಂಕೆ_Rows = fSafeTable->size();
    ಅಂಕೆ_Cols = fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories();

    ಯದಿ (use8BitsForSafeTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + ಪರಿಮಾಣ(int8_t)*ಅಂಕೆ_Cols;
    } ಅನ್ಯಥಾ {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + ಪರಿಮಾಣ(int16_t)*ಅಂಕೆ_Cols;
    }
    size   += ಅಂಕೆ_Rows * rowSize;
    ಪ್ರತ್ಯಾಗಮ size;
}

ತಾರ್ಕಿಕ RBBITableBuilder::use8BitsForSafeTable() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ fSafeTable->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::exportSafeTable(ಶೂನ್ಯ *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    ಪೂರ್ಣಾಙ್ಕ                col;

    ಯದಿ (U_FAILURE(*fStatus) || fSafeTable == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t cat_ಎಣಿಕೆ = fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories();
    ಯದಿ (cat_ಎಣಿಕೆ > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    table->f_ಅಂಕೆ_States = fSafeTable->size();
    table->fFlags     = 0;
    ಯದಿ (use8BitsForSafeTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + ಪರಿಮಾಣ(uint8_t) * cat_ಎಣಿಕೆ;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } ಅನ್ಯಥಾ {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + ಪರಿಮಾಣ(int16_t) * cat_ಎಣಿಕೆ;
    }

    ಪ್ರತಿ (state=0; state<table->f_ಅಂಕೆ_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        ಯದಿ (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            ಪ್ರತಿ (col=0; col<cat_ಎಣಿಕೆ; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } ಅನ್ಯಥಾ {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            ಪ್ರತಿ (col=0; col<cat_ಎಣಿಕೆ; col++) {
                row->r16.fNextState[col] = rowString->charAt(col);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::printSet(UVector *s) {
    int32_t  i;
    ಪ್ರತಿ (i=0; i<s->size(); i++) {
        ಧ್ರುವ RBBINode *v = static_cast<ಧ್ರುವ RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==nullptr? -1 : v->f_Serial_ಅಂಕೆ);
    }
    RBBIDebugPrintf("\n");
}
#endif


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::printStates() {
    ಪೂರ್ಣಾಙ್ಕ     c;    // input "character"
    ಪೂರ್ಣಾಙ್ಕ     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    ಪ್ರತಿ (c=0; c<fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    ಪ್ರತಿ (c=0; c<fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    ಪ್ರತಿ (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        ಪ್ರತಿ (c=0; c<fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#endif


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::printReverseTable() {
    ಪೂರ್ಣಾಙ್ಕ     c;    // input "character"
    ಪೂರ್ಣಾಙ್ಕ     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    ಯದಿ (fSafeTable == nullptr) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        ಪ್ರತ್ಯಾಗಮ;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    ಪ್ರತಿ (c=0; c<fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    ಪ್ರತಿ (c=0; c<fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    ಪ್ರತಿ (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        ಪ್ರತಿ (c=0; c<fRB->fSetBuilder->get_ಅಂಕೆ_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", rowString->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#endif



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::printRuleStatusTable() {
    int32_t  thisRecord = 0;
    int32_t  nextRecord = 0;
    ಪೂರ್ಣಾಙ್ಕ      i;
    UVector  *tbl = fRB->fRuleStatusVals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    ಯಾವತ್ (nextRecord < tbl->size()) {
        thisRecord = nextRecord;
        nextRecord = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        ಪ್ರತಿ (i=thisRecord+1; i<nextRecord; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#endif


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(ಪೂರ್ಣಾಙ್ಕ last_ಒಳಹರಿವು_ಚಿಹ್ನೆ, UErrorCode *fStatus) {
    fMarked    = ಅಸತ್ಯ;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = ನೂತನ UVector32(last_ಒಳಹರಿವು_ಚಿಹ್ನೆ+1, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    fDtran->setSize(last_ಒಳಹರಿವು_ಚಿಹ್ನೆ+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    delete       fPositions;
    delete       fDtran;
    delete       fTagVals;
    fPositions = nullptr;
    fDtran     = nullptr;
    fTagVals   = nullptr;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
