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

#அந்தர்பாவ <functional>

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "rbbitblb.h"
#அந்தர்பாவ "rbbirb.h"
#அந்தர்பாவ "rbbiscan.h"
#அந்தர்பாவ "rbbisetb.h"
#அந்தர்பாவ "rbbidata.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "uvectr32.h"
#அந்தர்பாவ "cmemory.h"

U_NAMESPACE_BEGIN

நாமாகாஶ {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
தருக்க reachableByTransitions(மாறிலி UVector &states, மாறிலி int32_t source,
                            மாறிலி std::function<தருக்க(int32_t)> isSink,
                            மாறிலி std::function<தருக்க(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        க்கு (int32_t குறியீடு = 0; குறியீடு < transitionsFromSource.size(); ++குறியீடு) {
            மாறிலி int32_t state = transitionsFromSource.elementAti(குறியீடு);
            எனில் (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    மாறிலி auto visited = prv::make_unique<தருக்க[]>(states.size(), status);
    போது (U_SUCCESS(status) && !boundary.empty()) {
        மாறிலி int32_t s = boundary.popi();
        எனில் (isSink(s)) {
            திரும்பு உண்மை;
        }
        எனில் (excludedState(s) || visited[s]) {
            தொடர்;
        }
        visited[s] = உண்மை;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        க்கு (int32_t குறியீடு = 0; குறியீடு < transitions.size(); ++குறியீடு) {
            மாறிலி int32_t t = transitions.elementAti(குறியீடு);
            எனில் (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    திரும்பு பொய்;
}

} // namespace

மாறிலி int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **வேர்_முனை, UErrorCode &status) :
        fRB(rb),
        f_மரம்(*வேர்_முனை),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = புதிய UVector(status);
    எனில் (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    முழு i;
    க்கு (i=0; i<fDStates->size(); i++) {
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
வெறுமை  RBBITableBuilder::buildForwardTable() {

    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    எனில் (f_மரம்==nullptr) {
        திரும்பு;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_மரம் = f_மரம்->flattenVariables(*fStatus, 0);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
#ifdef RBBI_DEBUG
    எனில் (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_மரம்(f_மரம், உண்மை);
    }
#endif

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = புதிய RBBINode(RBBINode::opCat, *fStatus);
    // Exit if memory allocation failed.
    எனில் (cn == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    எனில் (U_FAILURE(*fStatus)) {
        delete cn;
        திரும்பு;
    }
    cn->f_இடது_Child = f_மரம்;
    f_மரம்->fParent = cn;
    RBBINode *end_Marker_முனை = cn->f_வலது_Child = புதிய RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    எனில் (cn->f_வலது_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    எனில் (U_FAILURE(*fStatus)) {
        delete cn;
        திரும்பு;
    }
    cn->f_வலது_Child->fParent = cn;
    f_மரம் = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_மரம்->flattenSets(*fStatus, 0);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
#ifdef RBBI_DEBUG
    எனில் (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_மரம்(f_மரம், உண்மை);
    }
#endif


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_மரம்);
    calcFirstPos(f_மரம்);
    calcLastPos(f_மரம்);
    calcFollowPos(f_மரம்);
    எனில் (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_மரம்);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    எனில் (fRB->fChainRules) {
        calcChainedFollowPos(f_மரம், end_Marker_முனை);
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
வெறுமை RBBITableBuilder::calcNullable(RBBINode *n) {
    எனில் (n == nullptr) {
        திரும்பு;
    }
    எனில் (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = பொய்;
        திரும்பு;
    }

    எனில் (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = உண்மை;
        திரும்பு;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_இடது_Child);
    calcNullable(n->f_வலது_Child);

    // Apply functions from table 3.40 in Aho
    எனில் (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_இடது_Child->fNullable || n->f_வலது_Child->fNullable;
    }
    இல்லையெனில் எனில் (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_இடது_Child->fNullable && n->f_வலது_Child->fNullable;
    }
    இல்லையெனில் எனில் (n->fType == RBBINode::opStar || n->fType == RBBINode::opQuestion) {
        n->fNullable = உண்மை;
    }
    இல்லையெனில் {
        n->fNullable = பொய்;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
வெறுமை RBBITableBuilder::calcFirstPos(RBBINode *n) {
    எனில் (n == nullptr) {
        திரும்பு;
    }
    எனில் (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fFirstPosSet->addElement(n, *fStatus);
        திரும்பு;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_இடது_Child);
    calcFirstPos(n->f_வலது_Child);

    // Apply functions from table 3.40 in Aho
    எனில் (n->fType == RBBINode::opOr) {
        setAdd(n->fFirstPosSet, n->f_இடது_Child->fFirstPosSet);
        setAdd(n->fFirstPosSet, n->f_வலது_Child->fFirstPosSet);
    }
    இல்லையெனில் எனில் (n->fType == RBBINode::opCat) {
        setAdd(n->fFirstPosSet, n->f_இடது_Child->fFirstPosSet);
        எனில் (n->f_இடது_Child->fNullable) {
            setAdd(n->fFirstPosSet, n->f_வலது_Child->fFirstPosSet);
        }
    }
    இல்லையெனில் எனில் (n->fType == RBBINode::opStar ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fFirstPosSet, n->f_இடது_Child->fFirstPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
வெறுமை RBBITableBuilder::calcLastPos(RBBINode *n) {
    எனில் (n == nullptr) {
        திரும்பு;
    }
    எனில் (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fLastPosSet->addElement(n, *fStatus);
        திரும்பு;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calcLastPos(n->f_இடது_Child);
    calcLastPos(n->f_வலது_Child);

    // Apply functions from table 3.40 in Aho
    எனில் (n->fType == RBBINode::opOr) {
        setAdd(n->fLastPosSet, n->f_இடது_Child->fLastPosSet);
        setAdd(n->fLastPosSet, n->f_வலது_Child->fLastPosSet);
    }
    இல்லையெனில் எனில் (n->fType == RBBINode::opCat) {
        setAdd(n->fLastPosSet, n->f_வலது_Child->fLastPosSet);
        எனில் (n->f_வலது_Child->fNullable) {
            setAdd(n->fLastPosSet, n->f_இடது_Child->fLastPosSet);
        }
    }
    இல்லையெனில் எனில் (n->fType == RBBINode::opStar     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fLastPosSet, n->f_இடது_Child->fLastPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
வெறுமை RBBITableBuilder::calcFollowPos(RBBINode *n) {
    எனில் (n == nullptr ||
        n->fType == RBBINode::leafChar ||
        n->fType == RBBINode::endMark) {
        திரும்பு;
    }

    calcFollowPos(n->f_இடது_Child);
    calcFollowPos(n->f_வலது_Child);

    // Aho rule #1
    எனில் (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_இடது_Child = n->f_இடது_Child->fLastPosSet;

        க்கு (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_இடது_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_இடது_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_வலது_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    எனில் (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        க்கு (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
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
வெறுமை RBBITableBuilder::add_Rule_வேர்_Nodes(UVector *dest, RBBINode *முனை) {
    எனில் (முனை == nullptr || U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    U_ASSERT(!dest->hasDeleter());
    எனில் (முனை->f_Rule_வேர்) {
        dest->addElement(முனை, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        திரும்பு;
    }
    add_Rule_வேர்_Nodes(dest, முனை->f_இடது_Child);
    add_Rule_வேர்_Nodes(dest, முனை->f_வலது_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
வெறுமை RBBITableBuilder::calcChainedFollowPos(RBBINode *மரம், RBBINode *end_Mark_முனை) {

    UVector         leafNodes(*fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }

    // get a list all leaf nodes
    மரம்->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_வேர்_Nodes(*fStatus);
    add_Rule_வேர்_Nodes(&rule_வேர்_Nodes, மரம்);

    UVector matchStartNodes(*fStatus);
    க்கு (முழு j=0; j<rule_வேர்_Nodes.size(); ++j) {
        RBBINode *முனை = static_cast<RBBINode *>(rule_வேர்_Nodes.elementAt(j));
        எனில் (முனை->fChainIn) {
            setAdd(&matchStartNodes, முனை->fFirstPosSet);
        }
    }
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }

    int32_t  end_முனை_Ix;
    int32_t  start_முனை_Ix;

    க்கு (end_முனை_Ix=0; end_முனை_Ix<leafNodes.size(); end_முனை_Ix++) {
        RBBINode* end_முனை = static_cast<RBBINode*>(leafNodes.elementAt(end_முனை_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        எனில் (!end_முனை->fFollowPos->contains(end_Mark_முனை)) {
            தொடர்;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_முனை;
        க்கு (start_முனை_Ix = 0; start_முனை_Ix<matchStartNodes.size(); start_முனை_Ix++) {
            start_முனை = static_cast<RBBINode*>(matchStartNodes.elementAt(start_முனை_Ix));
            எனில் (start_முனை->fType != RBBINode::leafChar) {
                தொடர்;
            }

            எனில் (end_முனை->fVal == start_முனை->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_முனை->fFollowPos, start_முனை->fFollowPos);
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
வெறுமை RBBITableBuilder::bofFixup() {

    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
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
    RBBINode  *bof_முனை = f_மரம்->f_இடது_Child->f_இடது_Child;
    U_ASSERT(bof_முனை->fType == RBBINode::leafChar);
    U_ASSERT(bof_முனை->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_மரம்->f_இடது_Child->f_வலது_Child->fFirstPosSet;

    RBBINode *start_முனை;
    முழு       start_முனை_Ix;
    க்கு (start_முனை_Ix = 0; start_முனை_Ix<matchStartNodes->size(); start_முனை_Ix++) {
        start_முனை = static_cast<RBBINode*>(matchStartNodes->elementAt(start_முனை_Ix));
        எனில் (start_முனை->fType != RBBINode::leafChar) {
            தொடர்;
        }

        எனில் (start_முனை->fVal == bof_முனை->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_முனை->fFollowPos, start_முனை->fFollowPos);
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
வெறுமை RBBITableBuilder::buildStateTable() {
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    முழு      last_உள்ளீடு_குறியீடு = fRB->fSetBuilder->get_இலக்கம்_Char_Categories() - 1;
    failState = புதிய RBBIStateDescriptor(last_உள்ளீடு_குறியீடு, fStatus);
    எனில் (failState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        செல் ExitBuildSTdeleteall;
    }
    failState->fPositions = புதிய UVector(*fStatus);
    எனில் (failState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    எனில் (failState->fPositions == nullptr || U_FAILURE(*fStatus)) {
        செல் ExitBuildSTdeleteall;
    }
    fDStates->addElement(failState, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        செல் ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = புதிய RBBIStateDescriptor(last_உள்ளீடு_குறியீடு, fStatus);
    எனில் (initialState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    எனில் (U_FAILURE(*fStatus)) {
        செல் ExitBuildSTdeleteall;
    }
    initialState->fPositions = புதிய UVector(*fStatus);
    எனில் (initialState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    எனில் (U_FAILURE(*fStatus)) {
        செல் ExitBuildSTdeleteall;
    }
    setAdd(initialState->fPositions, f_மரம்->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        செல் ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    க்கு (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        க்கு (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *temp;
            temp = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            எனில் (temp->fMarked == பொய்) {
                T = temp;
                நிறுத்து;
            }
        }
        எனில் (T == nullptr) {
            நிறுத்து;
        }

        // mark T;
        T->fMarked = உண்மை;

        // for each input symbol a do begin
        int32_t  a;
        க்கு (a = 1; a<=last_உள்ளீடு_குறியீடு; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            க்கு (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                எனில் ((p->fType == RBBINode::leafChar) &&  (p->fVal == a)) {
                    எனில் (U == nullptr) {
                        U = புதிய UVector(*fStatus);
                        எனில் (U == nullptr) {
                        	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                        	செல் ExitBuildSTdeleteall;
                        }
                    }
                    setAdd(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = பொய்;
            எனில் (U != nullptr) {
                U_ASSERT(U->size() > 0);
                முழு  ix;
                க்கு (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    எனில் (setEquals(U, temp2->fPositions)) {
                        delete U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = உண்மை;
                        நிறுத்து;
                    }
                }

                // Add U as an unmarked state to Dstates
                எனில் (!UinDstates)
                {
                    RBBIStateDescriptor *newState = புதிய RBBIStateDescriptor(last_உள்ளீடு_குறியீடு, fStatus);
                    எனில் (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    எனில் (U_FAILURE(*fStatus)) {
                        செல் ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    எனில் (U_FAILURE(*fStatus)) {
                        திரும்பு;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    திரும்பு;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    delete initialState;
    delete failState;
}


/**
 * mapLookAheadRules
 *
 */
வெறுமை RBBITableBuilder::mapLookAheadRules() {
    fLookAheadRuleMap =  புதிய UVector32(fRB->fScanner->இலக்கம்_Rules() + 1, *fStatus);
    எனில் (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    fLookAheadRuleMap->setSize(fRB->fScanner->இலக்கம்_Rules() + 1);

    க்கு (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        தருக்க saw_Look_Ahead_முனை = பொய்;
        க்கு (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *முனை = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            எனில் (முனை->fType != RBBINode::முனை_Type::lookAhead) {
                தொடர்;
            }
            saw_Look_Ahead_முனை = உண்மை;
            int32_t rule_இலக்கம் = முனை->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_இலக்கம் < fLookAheadRuleMap->size());
            U_ASSERT(rule_இலக்கம் > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_இலக்கம்);
            எனில் (laSlot != 0) {
                எனில் (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } இல்லையெனில் {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        எனில் (!saw_Look_Ahead_முனை) {
            தொடர்;
        }

        எனில் (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        க்கு (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *முனை = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            எனில் (முனை->fType != RBBINode::முனை_Type::lookAhead) {
                தொடர்;
            }
            int32_t rule_இலக்கம் = முனை->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = fLookAheadRuleMap->elementAti(rule_இலக்கம்);
            (வெறுமை)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            fLookAheadRuleMap->setElementAt(laSlotForState, rule_இலக்கம்);
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
வெறுமை     RBBITableBuilder::flagAcceptingStates() {
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }

    f_மரம்->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }

    க்கு (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        க்கு (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            எனில் (sd->fPositions->indexOf(endMarker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                எனில் (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
                    எனில் (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                எனில் (sd->fAccepting==ACCEPTING_UNCONDITIONAL && endMarker->fVal != 0) {
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
வெறுமை     RBBITableBuilder::flagLookAheadStates() {
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_முனை;
    int32_t     i;
    int32_t     n;

    f_மரம்->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    க்கு (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_முனை = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_முனை->fType == RBBINode::முனை_Type::lookAhead);

        க்கு (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->indexOf(look_Ahead_முனை);
            எனில் (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_முனை == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_முனை->fVal);
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
வெறுமை     RBBITableBuilder::flagTaggedStates() {
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_முனை;
    int32_t     i;
    int32_t     n;

    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    f_மரம்->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    க்கு (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_முனை = static_cast<RBBINode*>(tagNodes.elementAt(i));

        க்கு (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            எனில் (sd->fPositions->indexOf(tag_முனை) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_முனை->fVal);
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
வெறுமை  RBBITableBuilder::mergeRuleStatusVals() {
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
    முழு i;
    முழு n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    எனில் (fRB->fRuleStatusVals->size() == 0) {
        fRB->fRuleStatusVals->addElement(1, *fStatus);  // Num of statuses in group
        fRB->fRuleStatusVals->addElement(static_cast<int32_t>(0), *fStatus); // and our single status of zero
    }

    //    For each state
    க்கு (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_மதிப்புகள் = sd->fTagVals;
        எனில் (this_States_Tag_மதிப்புகள் == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            தொடர்;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  nextTagGroupStart = 0;

        // Loop runs once per group of tags in the global list
        போது (nextTagGroupStart < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = nextTagGroupStart;
            nextTagGroupStart += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            எனில் (this_States_Tag_மதிப்புகள்->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                தொடர்;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            க்கு (i=0; i<this_States_Tag_மதிப்புகள்->size(); i++) {
                எனில் (this_States_Tag_மதிப்புகள்->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    நிறுத்து;
                }
            }

            எனில் (i == this_States_Tag_மதிப்புகள்->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                நிறுத்து;
            }
        }

        எனில் (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_மதிப்புகள்->size(), *fStatus);
            க்கு (i=0; i<this_States_Tag_மதிப்புகள்->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_மதிப்புகள்->elementAti(i), *fStatus);
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

வெறுமை RBBITableBuilder::minimizeStates() {
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }

#எனில் UPRV_HAS_SANITIZER
    எனில் (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        திரும்பு;
    }
#endif
    கட்டமைப்பு StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        தருக்க operator==(StateType மாறிலி &other) மாறிலி {
            திரும்பு fAccepting == other.fAccepting &&
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
    கட்டமைப்பு TypeToStates : UMemory {
        TypeToStates(மாறிலி StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](வெறுமை *p) { delete static_cast<TypeToStates *>(p); });
    க்கு (int32_t i = 0; i < fDStates->size(); ++i) {
        மாறிலி RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        மாறிலி StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        க்கு (; j < initialPartition.size(); ++j) {
            auto &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            எனில் (type_j == type) {
                states->addElement(i, *fStatus);
                நிறுத்து;
            }
        }
        எனில் (j == initialPartition.size()) {
            auto newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            எனில் (U_FAILURE(*fStatus)) {
                திரும்பு;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    auto partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    partition->setDeleter(
        [](வெறுமை *p) { delete static_cast<UVector*>(p); });
    
    க்கு (int32_t i = 0; i < initialPartition.size(); ++i) {
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
    auto partition_signature = [&partition, இது](int32_t stateIndex) -> prv::unique_ptr<UVector> {
        மாறிலி RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(stateIndex));
        auto result = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        எனில் (U_FAILURE(*fStatus)) {
            திரும்பு nullptr;
        }
        க்கு (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            க்கு (int32_t partIndex = 0; partIndex < partition->size(); ++partIndex) {
                மாறிலி UVector &part = *static_cast<UVector *>(partition->elementAt(partIndex));
                எனில் (part.contains(toState)) {
                    result->addElement(partIndex, *fStatus);
                    நிறுத்து;
                }
            }
        }
        result->setComparer([](மாறிலி UElement left, மாறிலி UElement வலது) -> UBool {
            திரும்பு left.integer == வலது.integer;
        });
        திரும்பு result;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    க்கு (;;) {
        // Π_new.
        auto partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        எனில் (U_FAILURE(*fStatus)) {
            திரும்பு;
        }
        partitionNew->setDeleter(
            [](வெறுமை *p) { delete static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        தருக்க refined = பொய்;
        க்கு (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            மாறிலி UVector &group = *static_cast<UVector*>(partition->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            கட்டமைப்பு SignatureToStates : UMemory {
                SignatureToStates(prv::unique_ptr<UVector> signature, UErrorCode &status)
                    : signature(std::move(signature)), states(prv::make_unique<UVector>(status, status)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> states;
            };
            UVector groupRefinement(*fStatus);
            groupRefinement.setDeleter([](வெறுமை *p) { delete static_cast<SignatureToStates*>(p); });
            க்கு (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                மாறிலி int32_t s = group.elementAti(j);
                auto signature = partition_signature(s);
                int32_t k = 0;
                க்கு (; k < groupRefinement.size(); ++k) {
                    மாறிலி auto &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    எனில் (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        நிறுத்து;
                    }
                }
                எனில் (k == groupRefinement.size()) {
                    மாறிலி auto newEntry =
                        புதிய SignatureToStates(std::move(signature), *fStatus);
                    எனில் (U_FAILURE(*fStatus)) {
                        திரும்பு;
                    }
                    எனில் (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      திரும்பு;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            க்கு (int32_t j = 0; j < groupRefinement.size(); ++j) {
                auto &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
            }
        }
        எனில் (refined) {
            partition = std::move(partitionNew);
        } இல்லையெனில் {
            நிறுத்து;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    க்கு (முழு i = 0; i < partition->size(); ++i) {
        UVector *மாறிலி part_i = static_cast<UVector *>(partition->elementAt(i));
        எனில் (part_i->contains(0)) {
            வெறுமை *மாறிலி part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            மாறிலி auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } இல்லையெனில் எனில் (part_i->contains(1)) {
            வெறுமை *மாறிலி part_1 = partition->elementAt(1);
            மாறிலி auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    auto oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    க்கு (முழு i = 0; i < partition->size(); ++i) {
        மாறிலி UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        க்கு (முழு j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    auto மாறிலி oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    க்கு (முழு i = 0; i < partition->size(); ++i) {
        மாறிலி UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *மாறிலி state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        க்கு (முழு j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->setElementAt(oldStateToPart[state->fDtran->elementAti(j)], j);
        }
        oldStates->setElementAt(nullptr, part.elementAti(0));
    }
    oldStates->setDeleter([](வெறுமை *p) { delete static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
வெறுமை RBBITableBuilder::sortedAdd(UVector **vector, int32_t val) {
    int32_t i;

    எனில் (*vector == nullptr) {
        *vector = புதிய UVector(*fStatus);
    }
    எனில் (*vector == nullptr || U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    UVector *vec = *vector;
    int32_t  vSize = vec->size();
    க்கு (i=0; i<vSize; i++) {
        int32_t valAtI = vec->elementAti(i);
        எனில் (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            திரும்பு;
        }
        எனில் (valAtI > val) {
            நிறுத்து;
        }
    }
    vec->செருகு_Element_At(val, i, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
வெறுமை RBBITableBuilder::setAdd(UVector *dest, UVector *source) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!source->hasDeleter());
    int32_t destOriginalSize = dest->size();
    int32_t sourceSize       = source->size();
    int32_t di           = 0;
    Maybe_அடுக்கு_Array<வெறுமை *, 16> destArray, sourceArray;  // Handle small cases without malloc
    வெறுமை **destPtr, **sourcePtr;
    வெறுமை **destLim, **sourceLim;

    எனில் (destOriginalSize > destArray.getCapacity()) {
        எனில் (destArray.resize(destOriginalSize) == nullptr) {
            திரும்பு;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + destOriginalSize;  // destArray.getArrayLimit()?

    எனில் (sourceSize > sourceArray.getCapacity()) {
        எனில் (sourceArray.resize(sourceSize) == nullptr) {
            திரும்பு;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + sourceSize;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (வெறுமை) dest->toArray(destPtr);
    (வெறுமை) source->toArray(sourcePtr);

    dest->setSize(sourceSize+destOriginalSize, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }

    போது (sourcePtr < sourceLim && destPtr < destLim) {
        எனில் (*destPtr == *sourcePtr) {
            dest->setElementAt(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        இல்லையெனில் எனில் (uprv_memcmp(destPtr, sourcePtr, அளவு(வெறுமை *)) < 0) {
            dest->setElementAt(*destPtr++, di++);
        }
        இல்லையெனில் { /* *sourcePtr < *destPtr */
            dest->setElementAt(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    போது (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    போது (sourcePtr < sourceLim) {
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
    திரும்பு a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
வெறுமை RBBITableBuilder::printPosSets(RBBINode *n) {
    எனில் (n==nullptr) {
        திரும்பு;
    }
    printf("\n");
    RBBINode::print_முனை_Header();
    RBBINode::print_முனை(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    printSet(n->fFirstPosSet);

    RBBIDebugPrintf("         lastpos:   ");
    printSet(n->fLastPosSet);

    RBBIDebugPrintf("         followpos: ");
    printSet(n->fFollowPos);

    printPosSets(n->f_இடது_Child);
    printPosSets(n->f_வலது_Child);
}
#endif

//
//    findDuplCharClassFrom()
//
தருக்க RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t இலக்கம்_States = fDStates->size();
    int32_t இலக்கம்_Cols = fRB->fSetBuilder->get_இலக்கம்_Char_Categories();

    க்கு (; categories->first < இலக்கம்_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        முழு limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : இலக்கம்_Cols;
        க்கு (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            க்கு (int32_t state=0; state<இலக்கம்_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                எனில் (table_base != table_dupl) {
                    நிறுத்து;
                }
            }
            எனில் (table_base == table_dupl) {
                திரும்பு உண்மை;
            }
        }
    }
    திரும்பு பொய்;
}


//
//    removeColumn()
//
வெறுமை RBBITableBuilder::removeColumn(int32_t column) {
    int32_t இலக்கம்_States = fDStates->size();
    க்கு (int32_t state=0; state<இலக்கம்_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


தருக்க RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t இலக்கம்_States = fSafeTable->size();

    க்கு (; states->first<இலக்கம்_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        க்கு (states->second=states->first+1; states->second<இலக்கம்_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            தருக்க rowsMatch = உண்மை;
            int32_t இலக்கம்_Cols = firstRow->length();
            க்கு (int32_t col=0; col < இலக்கம்_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                எனில் (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = பொய்;
                    நிறுத்து;
                }
            }
            எனில் (rowsMatch) {
                திரும்பு உண்மை;
            }
        }
    }
    திரும்பு பொய்;
}

வெறுமை RBBITableBuilder::removeSafeState(IntPair duplStates) {
    மாறிலி int32_t keepState = duplStates.first;
    மாறிலி int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t இலக்கம்_States = fSafeTable->size();
    க்கு (int32_t state=0; state<இலக்கம்_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t இலக்கம்_Cols = sd->length();
        க்கு (int32_t col=0; col<இலக்கம்_Cols; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            எனில் (existingVal == duplState) {
                newVal = keepState;
            } இல்லையெனில் எனில் (existingVal > duplState) {
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
    மாறிலி int32_t old_State_மொத்தம் = fDStates->size();
    minimizeStates();
    திரும்பு fDStates->size() - old_State_மொத்தம்;
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
வெறுமை RBBITableBuilder::minimizeLookaheads() {
    எனில் (fLASlotsInUse == ACCEPTING_UNCONDITIONAL) {
        திரும்பு;
    }
    மாறிலி int32_t lookahead_மொத்தம் = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    மாறிலி int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    மாறிலி auto stateDescriptor = [இது](மாறிலி int32_t state) -> RBBIStateDescriptor & {
        திரும்பு *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    மாறிலி auto lookaheadReachability =
        prv::make_unique<தருக்க[]>(lookahead_மொத்தம் * lookahead_மொத்தம், *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    க்கு (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        க்கு (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
            எனில் (k != l) {
                க்கு (int32_t source = 1; source < fDStates->size(); ++source) {
                    எனில் (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        தொடர்;
                    }
                    எனில் (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](மாறிலி int32_t state) {
                                திரும்பு static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](மாறிலி int32_t state) {
                                திரும்பு static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_மொத்தம் + k -
                                              firstLookahead] = உண்மை;
                    }
                }
            }
        }
    }

    மாறிலி auto colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_மொத்தம், *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    க்கு (int32_t chromatic_எண் = 1;; ++chromatic_எண்) {
        uprv_memset(colours.get(), 0, அளவு(int32_t) * lookahead_மொத்தம்);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        க்கு (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            க்கு (முழு source = 0; source < lookahead_மொத்தம்; ++source) {
                க்கு (முழு sink = 0; sink < lookahead_மொத்தம்; ++sink) {
                    எனில் (lookaheadReachability[source * lookahead_மொத்தம் + sink] &&
                        colours[source] == colours[sink]) {
                        செல் nextColouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            க்கு (முழு i = 0; i < fDStates->size(); ++i) {
                எனில் (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        firstLookahead +
                        colours[stateDescriptor(i).fAccepting - firstLookahead];
                }
                எனில் (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - firstLookahead];
                }
            }
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_எண்;
            திரும்பு;

          nextColouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            க்கு (int32_t i = 0; i < lookahead_மொத்தம் - 1 && colours[i] == chromatic_எண்;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            எனில் (colours[lookahead_மொத்தம் - 1] == chromatic_எண்) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                நிறுத்து;
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
int32_t  RBBITableBuilder::getTableSize() மாறிலி {
    int32_t    size = 0;
    int32_t    இலக்கம்_Rows;
    int32_t    இலக்கம்_Cols;
    int32_t    rowSize;

    எனில் (f_மரம் == nullptr) {
        திரும்பு 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    இலக்கம்_Rows = fDStates->size();
    இலக்கம்_Cols = fRB->fSetBuilder->get_இலக்கம்_Char_Categories();

    எனில் (use8BitsForTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + அளவு(int8_t)*இலக்கம்_Cols;
    } இல்லையெனில் {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + அளவு(int16_t)*இலக்கம்_Cols;
    }
    size   += இலக்கம்_Rows * rowSize;
    திரும்பு size;
}

தருக்க RBBITableBuilder::use8BitsForTable() மாறிலி {
    திரும்பு fDStates->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
வெறுமை RBBITableBuilder::exportTable(வெறுமை *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    முழு                col;

    எனில் (U_FAILURE(*fStatus) || f_மரம் == nullptr) {
        திரும்பு;
    }

    int32_t cat_மொத்தம் = fRB->fSetBuilder->get_இலக்கம்_Char_Categories();
    எனில் (cat_மொத்தம் > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        திரும்பு;
    }

    table->f_இலக்கம்_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->fLookAheadResultsSize = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    எனில் (use8BitsForTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + அளவு(uint8_t) * cat_மொத்தம்;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } இல்லையெனில் {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + அளவு(int16_t) * cat_மொத்தம்;
    }
    எனில் (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    க்கு (state=0; state<table->f_இலக்கம்_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        எனில் (use8BitsForTable()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 255);
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->fTagsIdx   = sd->fTagsIdx;
            க்கு (col=0; col<cat_மொத்தம்; col++) {
                U_ASSERT (sd->fDtran->elementAti(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = sd->fDtran->elementAti(col);
            }
        } இல்லையெனில் {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 0xffff);
            row->r16.fAccepting = sd->fAccepting;
            row->r16.fLookAhead = sd->fLookAhead;
            row->r16.fTagsIdx   = sd->fTagsIdx;
            க்கு (col=0; col<cat_மொத்தம்; col++) {
                row->r16.fNextState[col] = sd->fDtran->elementAti(col);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
வெறுமை RBBITableBuilder::buildSafeReverseTable(UErrorCode &status) {
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

    int32_t இலக்கம்_Char_Classes = fRB->fSetBuilder->get_இலக்கம்_Char_Categories();
    int32_t இலக்கம்_States = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    மாறிலி auto lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    மாறிலி auto wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    க்கு (int32_t c1 = 1; c1 < இலக்கம்_Char_Classes; ++c1) {
        எனில் (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          தொடர்;
        }
        க்கு (int32_t c2 = 1; c2 < இலக்கம்_Char_Classes; ++c2) {
            எனில் (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                தொடர்;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            தருக்க mustCheckConsistentWithStartC2 = பொய்;
            மாறிலி int32_t text[2] = {c1, c2};
            க்கு (int32_t injectedState = START_STATE; injectedState < இலக்கம்_States; ++injectedState) {
                க்கு (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    எனில் (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        மாறிலி RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        எனில் (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
                        } இல்லையெனில் எனில் (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            தொடர்;
                        }
                        எனில் (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    செல் dfaStep;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    க்கு (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        க்கு (;;) {
                        dfaStep:
                            எனில் (i == BEFORE_PAIR || i == 2) {
                                செல் inspectFinalState;
                            }
                            மாறிலி int32_t குறியீடு_Ahead = text[i];
                            ++i;
                            மாறிலி RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            எனில் (sDescriptor->fDtran->elementAti(குறியீடு_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(குறியீடு_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } இல்லையெனில் {
                                எனில் (lastBreak != BEFORE_PAIR &&
                                    lastAcceptingPosition == lastBreak) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    செல் next_குறியீடு_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                நிறுத்து;
                            }

                            எனில் (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } இல்லையெனில் எனில் (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
                                நிறுத்து;
                            }
                            எனில் (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    எனில் (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        தொடர்;
                    }
                    எனில் (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        க்கு (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                            எனில் (lookaheadPositions[l] != LOOKAHEAD_NOT_SET &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wantedLookaheadsInPair[l] = lookaheadPositions[i];
                            } இல்லையெனில் {
                                wantedLookaheadsInPair[l] = LOOKAHEAD_NOT_SET;
                            }
                        }
                    } இல்லையெனில் {
                        எனில் (wantedEndState != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            செல் next_குறியீடு_Pair;
                        }
                        எனில் (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            செல் next_குறியீடு_Pair;
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
                    க்கு (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        எனில் (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            தொடர்;
                        }
                        எனில் (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            எனில் (wantedEndState != START_STATE) {
                                செல் next_குறியீடு_Pair;
                            }
                        }
                        எனில் (lookaheadPositions[l] == 1 || wantedLookaheadsInPair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            mustCheckConsistentWithStartC2 = உண்மை;
                        }
                    }
                }
            }
            எனில் (mustCheckConsistentWithStartC2) {
                மாறிலி RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                மாறிலி auto c2State = startDescriptor.fDtran->elementAti(c2);
                எனில் (c2State != wantedEndState) {
                    செல் next_குறியீடு_Pair;
                }
                மாறிலி RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                எனில் (wantedLastAcceptingPosition != 2) {
                    செல் next_குறியீடு_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                க்கு (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    எனில் (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        செல் next_குறியீடு_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        next_குறியீடு_Pair:
            தொடர்;
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
        புதிய UVector(uprv_deleteUObject, uhash_compareUnicodeString, இலக்கம்_Char_Classes + 2, status), status);
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    fSafeTable = lpSafeTable.orphan();
    க்கு (int32_t row=0; row<இலக்கம்_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(புதிய UnicodeString(இலக்கம்_Char_Classes, 0, இலக்கம்_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    க்கு (int32_t charClass=0; charClass < இலக்கம்_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    க்கு (int32_t row=2; row<இலக்கம்_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    க்கு (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    போது (findDuplicateSafeState(&states)) {
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
int32_t  RBBITableBuilder::getSafeTableSize() மாறிலி {
    int32_t    size = 0;
    int32_t    இலக்கம்_Rows;
    int32_t    இலக்கம்_Cols;
    int32_t    rowSize;

    எனில் (fSafeTable == nullptr) {
        திரும்பு 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    இலக்கம்_Rows = fSafeTable->size();
    இலக்கம்_Cols = fRB->fSetBuilder->get_இலக்கம்_Char_Categories();

    எனில் (use8BitsForSafeTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + அளவு(int8_t)*இலக்கம்_Cols;
    } இல்லையெனில் {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + அளவு(int16_t)*இலக்கம்_Cols;
    }
    size   += இலக்கம்_Rows * rowSize;
    திரும்பு size;
}

தருக்க RBBITableBuilder::use8BitsForSafeTable() மாறிலி {
    திரும்பு fSafeTable->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
வெறுமை RBBITableBuilder::exportSafeTable(வெறுமை *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    முழு                col;

    எனில் (U_FAILURE(*fStatus) || fSafeTable == nullptr) {
        திரும்பு;
    }

    int32_t cat_மொத்தம் = fRB->fSetBuilder->get_இலக்கம்_Char_Categories();
    எனில் (cat_மொத்தம் > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        திரும்பு;
    }

    table->f_இலக்கம்_States = fSafeTable->size();
    table->fFlags     = 0;
    எனில் (use8BitsForSafeTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + அளவு(uint8_t) * cat_மொத்தம்;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } இல்லையெனில் {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + அளவு(int16_t) * cat_மொத்தம்;
    }

    க்கு (state=0; state<table->f_இலக்கம்_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        எனில் (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            க்கு (col=0; col<cat_மொத்தம்; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } இல்லையெனில் {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            க்கு (col=0; col<cat_மொத்தம்; col++) {
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
வெறுமை RBBITableBuilder::printSet(UVector *s) {
    int32_t  i;
    க்கு (i=0; i<s->size(); i++) {
        மாறிலி RBBINode *v = static_cast<மாறிலி RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==nullptr? -1 : v->f_Serial_இலக்கம்);
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
வெறுமை RBBITableBuilder::printStates() {
    முழு     c;    // input "character"
    முழு     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    க்கு (c=0; c<fRB->fSetBuilder->get_இலக்கம்_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    க்கு (c=0; c<fRB->fSetBuilder->get_இலக்கம்_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    க்கு (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        க்கு (c=0; c<fRB->fSetBuilder->get_இலக்கம்_Char_Categories(); c++) {
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
வெறுமை RBBITableBuilder::printReverseTable() {
    முழு     c;    // input "character"
    முழு     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    எனில் (fSafeTable == nullptr) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        திரும்பு;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    க்கு (c=0; c<fRB->fSetBuilder->get_இலக்கம்_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    க்கு (c=0; c<fRB->fSetBuilder->get_இலக்கம்_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    க்கு (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        க்கு (c=0; c<fRB->fSetBuilder->get_இலக்கம்_Char_Categories(); c++) {
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
வெறுமை RBBITableBuilder::printRuleStatusTable() {
    int32_t  thisRecord = 0;
    int32_t  nextRecord = 0;
    முழு      i;
    UVector  *tbl = fRB->fRuleStatusVals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    போது (nextRecord < tbl->size()) {
        thisRecord = nextRecord;
        nextRecord = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        க்கு (i=thisRecord+1; i<nextRecord; i++) {
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

RBBIStateDescriptor::RBBIStateDescriptor(முழு last_உள்ளீடு_குறியீடு, UErrorCode *fStatus) {
    fMarked    = பொய்;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = புதிய UVector32(last_உள்ளீடு_குறியீடு+1, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }
    எனில் (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    fDtran->setSize(last_உள்ளீடு_குறியீடு+1);    // fDtran needs to be pre-sized.
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
