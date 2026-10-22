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
तार्किक reachableByTransitions(ध्रुव UVector &states, ध्रुव int32_t source,
                            ध्रुव std::function<तार्किक(int32_t)> isSink,
                            ध्रुव std::function<तार्किक(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        प्रति (int32_t प्रतीक = 0; प्रतीक < transitionsFromSource.size(); ++प्रतीक) {
            ध्रुव int32_t state = transitionsFromSource.elementAti(प्रतीक);
            यदि (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    ध्रुव auto visited = prv::make_unique<तार्किक[]>(states.size(), status);
    यावत् (U_SUCCESS(status) && !boundary.empty()) {
        ध्रुव int32_t s = boundary.popi();
        यदि (isSink(s)) {
            प्रत्यागम सत्य;
        }
        यदि (excludedState(s) || visited[s]) {
            अनुवर्त;
        }
        visited[s] = सत्य;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        प्रति (int32_t प्रतीक = 0; प्रतीक < transitions.size(); ++प्रतीक) {
            ध्रुव int32_t t = transitions.elementAti(प्रतीक);
            यदि (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    प्रत्यागम असत्य;
}

} // namespace

ध्रुव int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **मूल_गाँठ, UErrorCode &status) :
        fRB(rb),
        f_वृक्ष(*मूल_गाँठ),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = नूतन UVector(status);
    यदि (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    पूर्णाङ्क i;
    प्रति (i=0; i<fDStates->size(); i++) {
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
शून्य  RBBITableBuilder::buildForwardTable() {

    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    यदि (f_वृक्ष==nullptr) {
        प्रत्यागम;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_वृक्ष = f_वृक्ष->flattenVariables(*fStatus, 0);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
#ifdef RBBI_DEBUG
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_वृक्ष(f_वृक्ष, सत्य);
    }
#endif

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = नूतन RBBINode(RBBINode::opCat, *fStatus);
    // Exit if memory allocation failed.
    यदि (cn == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(*fStatus)) {
        delete cn;
        प्रत्यागम;
    }
    cn->f_बायाँ_Child = f_वृक्ष;
    f_वृक्ष->fParent = cn;
    RBBINode *end_Marker_गाँठ = cn->f_दायाँ_Child = नूतन RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    यदि (cn->f_दायाँ_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(*fStatus)) {
        delete cn;
        प्रत्यागम;
    }
    cn->f_दायाँ_Child->fParent = cn;
    f_वृक्ष = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_वृक्ष->flattenSets(*fStatus, 0);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
#ifdef RBBI_DEBUG
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_वृक्ष(f_वृक्ष, सत्य);
    }
#endif


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_वृक्ष);
    calcFirstPos(f_वृक्ष);
    calcLastPos(f_वृक्ष);
    calcFollowPos(f_वृक्ष);
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_वृक्ष);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    यदि (fRB->fChainRules) {
        calcChainedFollowPos(f_वृक्ष, end_Marker_गाँठ);
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
शून्य RBBITableBuilder::calcNullable(RBBINode *n) {
    यदि (n == nullptr) {
        प्रत्यागम;
    }
    यदि (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = असत्य;
        प्रत्यागम;
    }

    यदि (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = सत्य;
        प्रत्यागम;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_बायाँ_Child);
    calcNullable(n->f_दायाँ_Child);

    // Apply functions from table 3.40 in Aho
    यदि (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_बायाँ_Child->fNullable || n->f_दायाँ_Child->fNullable;
    }
    अन्यथा यदि (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_बायाँ_Child->fNullable && n->f_दायाँ_Child->fNullable;
    }
    अन्यथा यदि (n->fType == RBBINode::opStar || n->fType == RBBINode::opQuestion) {
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
    यदि (n == nullptr) {
        प्रत्यागम;
    }
    यदि (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fFirstPosSet->addElement(n, *fStatus);
        प्रत्यागम;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_बायाँ_Child);
    calcFirstPos(n->f_दायाँ_Child);

    // Apply functions from table 3.40 in Aho
    यदि (n->fType == RBBINode::opOr) {
        setAdd(n->fFirstPosSet, n->f_बायाँ_Child->fFirstPosSet);
        setAdd(n->fFirstPosSet, n->f_दायाँ_Child->fFirstPosSet);
    }
    अन्यथा यदि (n->fType == RBBINode::opCat) {
        setAdd(n->fFirstPosSet, n->f_बायाँ_Child->fFirstPosSet);
        यदि (n->f_बायाँ_Child->fNullable) {
            setAdd(n->fFirstPosSet, n->f_दायाँ_Child->fFirstPosSet);
        }
    }
    अन्यथा यदि (n->fType == RBBINode::opStar ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fFirstPosSet, n->f_बायाँ_Child->fFirstPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcLastPos(RBBINode *n) {
    यदि (n == nullptr) {
        प्रत्यागम;
    }
    यदि (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fLastPosSet->addElement(n, *fStatus);
        प्रत्यागम;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calcLastPos(n->f_बायाँ_Child);
    calcLastPos(n->f_दायाँ_Child);

    // Apply functions from table 3.40 in Aho
    यदि (n->fType == RBBINode::opOr) {
        setAdd(n->fLastPosSet, n->f_बायाँ_Child->fLastPosSet);
        setAdd(n->fLastPosSet, n->f_दायाँ_Child->fLastPosSet);
    }
    अन्यथा यदि (n->fType == RBBINode::opCat) {
        setAdd(n->fLastPosSet, n->f_दायाँ_Child->fLastPosSet);
        यदि (n->f_दायाँ_Child->fNullable) {
            setAdd(n->fLastPosSet, n->f_बायाँ_Child->fLastPosSet);
        }
    }
    अन्यथा यदि (n->fType == RBBINode::opStar     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fLastPosSet, n->f_बायाँ_Child->fLastPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcFollowPos(RBBINode *n) {
    यदि (n == nullptr ||
        n->fType == RBBINode::leafChar ||
        n->fType == RBBINode::endMark) {
        प्रत्यागम;
    }

    calcFollowPos(n->f_बायाँ_Child);
    calcFollowPos(n->f_दायाँ_Child);

    // Aho rule #1
    यदि (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_बायाँ_Child = n->f_बायाँ_Child->fLastPosSet;

        प्रति (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_बायाँ_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_बायाँ_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_दायाँ_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    यदि (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        प्रति (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
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
शून्य RBBITableBuilder::add_Rule_मूल_Nodes(UVector *dest, RBBINode *गाँठ) {
    यदि (गाँठ == nullptr || U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    U_ASSERT(!dest->hasDeleter());
    यदि (गाँठ->f_Rule_मूल) {
        dest->addElement(गाँठ, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        प्रत्यागम;
    }
    add_Rule_मूल_Nodes(dest, गाँठ->f_बायाँ_Child);
    add_Rule_मूल_Nodes(dest, गाँठ->f_दायाँ_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcChainedFollowPos(RBBINode *वृक्ष, RBBINode *end_Mark_गाँठ) {

    UVector         leafNodes(*fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }

    // get a list all leaf nodes
    वृक्ष->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_मूल_Nodes(*fStatus);
    add_Rule_मूल_Nodes(&rule_मूल_Nodes, वृक्ष);

    UVector matchStartNodes(*fStatus);
    प्रति (पूर्णाङ्क j=0; j<rule_मूल_Nodes.size(); ++j) {
        RBBINode *गाँठ = static_cast<RBBINode *>(rule_मूल_Nodes.elementAt(j));
        यदि (गाँठ->fChainIn) {
            setAdd(&matchStartNodes, गाँठ->fFirstPosSet);
        }
    }
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }

    int32_t  end_गाँठ_Ix;
    int32_t  start_गाँठ_Ix;

    प्रति (end_गाँठ_Ix=0; end_गाँठ_Ix<leafNodes.size(); end_गाँठ_Ix++) {
        RBBINode* end_गाँठ = static_cast<RBBINode*>(leafNodes.elementAt(end_गाँठ_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        यदि (!end_गाँठ->fFollowPos->contains(end_Mark_गाँठ)) {
            अनुवर्त;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_गाँठ;
        प्रति (start_गाँठ_Ix = 0; start_गाँठ_Ix<matchStartNodes.size(); start_गाँठ_Ix++) {
            start_गाँठ = static_cast<RBBINode*>(matchStartNodes.elementAt(start_गाँठ_Ix));
            यदि (start_गाँठ->fType != RBBINode::leafChar) {
                अनुवर्त;
            }

            यदि (end_गाँठ->fVal == start_गाँठ->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_गाँठ->fFollowPos, start_गाँठ->fFollowPos);
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

    यदि (U_FAILURE(*fStatus)) {
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
    RBBINode  *bof_गाँठ = f_वृक्ष->f_बायाँ_Child->f_बायाँ_Child;
    U_ASSERT(bof_गाँठ->fType == RBBINode::leafChar);
    U_ASSERT(bof_गाँठ->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_वृक्ष->f_बायाँ_Child->f_दायाँ_Child->fFirstPosSet;

    RBBINode *start_गाँठ;
    पूर्णाङ्क       start_गाँठ_Ix;
    प्रति (start_गाँठ_Ix = 0; start_गाँठ_Ix<matchStartNodes->size(); start_गाँठ_Ix++) {
        start_गाँठ = static_cast<RBBINode*>(matchStartNodes->elementAt(start_गाँठ_Ix));
        यदि (start_गाँठ->fType != RBBINode::leafChar) {
            अनुवर्त;
        }

        यदि (start_गाँठ->fVal == bof_गाँठ->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_गाँठ->fFollowPos, start_गाँठ->fFollowPos);
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
शून्य RBBITableBuilder::buildStateTable() {
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    पूर्णाङ्क      last_निवेश_प्रतीक = fRB->fSetBuilder->get_अंक_Char_Categories() - 1;
    failState = नूतन RBBIStateDescriptor(last_निवेश_प्रतीक, fStatus);
    यदि (failState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        गच्छ ExitBuildSTdeleteall;
    }
    failState->fPositions = नूतन UVector(*fStatus);
    यदि (failState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (failState->fPositions == nullptr || U_FAILURE(*fStatus)) {
        गच्छ ExitBuildSTdeleteall;
    }
    fDStates->addElement(failState, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        गच्छ ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = नूतन RBBIStateDescriptor(last_निवेश_प्रतीक, fStatus);
    यदि (initialState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(*fStatus)) {
        गच्छ ExitBuildSTdeleteall;
    }
    initialState->fPositions = नूतन UVector(*fStatus);
    यदि (initialState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(*fStatus)) {
        गच्छ ExitBuildSTdeleteall;
    }
    setAdd(initialState->fPositions, f_वृक्ष->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        गच्छ ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    प्रति (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        प्रति (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *temp;
            temp = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            यदि (temp->fMarked == असत्य) {
                T = temp;
                भङ्ग;
            }
        }
        यदि (T == nullptr) {
            भङ्ग;
        }

        // mark T;
        T->fMarked = सत्य;

        // for each input symbol a do begin
        int32_t  a;
        प्रति (a = 1; a<=last_निवेश_प्रतीक; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            प्रति (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                यदि ((p->fType == RBBINode::leafChar) &&  (p->fVal == a)) {
                    यदि (U == nullptr) {
                        U = नूतन UVector(*fStatus);
                        यदि (U == nullptr) {
                        	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                        	गच्छ ExitBuildSTdeleteall;
                        }
                    }
                    setAdd(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = असत्य;
            यदि (U != nullptr) {
                U_ASSERT(U->size() > 0);
                पूर्णाङ्क  ix;
                प्रति (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    यदि (setEquals(U, temp2->fPositions)) {
                        delete U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = सत्य;
                        भङ्ग;
                    }
                }

                // Add U as an unmarked state to Dstates
                यदि (!UinDstates)
                {
                    RBBIStateDescriptor *newState = नूतन RBBIStateDescriptor(last_निवेश_प्रतीक, fStatus);
                    यदि (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    यदि (U_FAILURE(*fStatus)) {
                        गच्छ ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    यदि (U_FAILURE(*fStatus)) {
                        प्रत्यागम;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    प्रत्यागम;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    delete initialState;
    delete failState;
}


/**
 * mapLookAheadRules
 *
 */
शून्य RBBITableBuilder::mapLookAheadRules() {
    fLookAheadRuleMap =  नूतन UVector32(fRB->fScanner->अंक_Rules() + 1, *fStatus);
    यदि (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    fLookAheadRuleMap->setSize(fRB->fScanner->अंक_Rules() + 1);

    प्रति (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        तार्किक saw_Look_Ahead_गाँठ = असत्य;
        प्रति (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *गाँठ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            यदि (गाँठ->fType != RBBINode::गाँठ_Type::lookAhead) {
                अनुवर्त;
            }
            saw_Look_Ahead_गाँठ = सत्य;
            int32_t rule_अंक = गाँठ->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_अंक < fLookAheadRuleMap->size());
            U_ASSERT(rule_अंक > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_अंक);
            यदि (laSlot != 0) {
                यदि (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } अन्यथा {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        यदि (!saw_Look_Ahead_गाँठ) {
            अनुवर्त;
        }

        यदि (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        प्रति (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *गाँठ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            यदि (गाँठ->fType != RBBINode::गाँठ_Type::lookAhead) {
                अनुवर्त;
            }
            int32_t rule_अंक = गाँठ->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = fLookAheadRuleMap->elementAti(rule_अंक);
            (शून्य)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            fLookAheadRuleMap->setElementAt(laSlotForState, rule_अंक);
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
शून्य     RBBITableBuilder::flagAcceptingStates() {
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }

    f_वृक्ष->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }

    प्रति (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        प्रति (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            यदि (sd->fPositions->indexOf(endMarker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                यदि (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
                    यदि (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                यदि (sd->fAccepting==ACCEPTING_UNCONDITIONAL && endMarker->fVal != 0) {
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
शून्य     RBBITableBuilder::flagLookAheadStates() {
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_गाँठ;
    int32_t     i;
    int32_t     n;

    f_वृक्ष->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    प्रति (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_गाँठ = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_गाँठ->fType == RBBINode::गाँठ_Type::lookAhead);

        प्रति (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->indexOf(look_Ahead_गाँठ);
            यदि (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_गाँठ == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_गाँठ->fVal);
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
शून्य     RBBITableBuilder::flagTaggedStates() {
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_गाँठ;
    int32_t     i;
    int32_t     n;

    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    f_वृक्ष->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    प्रति (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_गाँठ = static_cast<RBBINode*>(tagNodes.elementAt(i));

        प्रति (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            यदि (sd->fPositions->indexOf(tag_गाँठ) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_गाँठ->fVal);
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
शून्य  RBBITableBuilder::mergeRuleStatusVals() {
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
    यदि (fRB->fRuleStatusVals->size() == 0) {
        fRB->fRuleStatusVals->addElement(1, *fStatus);  // Num of statuses in group
        fRB->fRuleStatusVals->addElement(static_cast<int32_t>(0), *fStatus); // and our single status of zero
    }

    //    For each state
    प्रति (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_मूल्यावली = sd->fTagVals;
        यदि (this_States_Tag_मूल्यावली == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            अनुवर्त;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  nextTagGroupStart = 0;

        // Loop runs once per group of tags in the global list
        यावत् (nextTagGroupStart < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = nextTagGroupStart;
            nextTagGroupStart += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            यदि (this_States_Tag_मूल्यावली->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                अनुवर्त;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            प्रति (i=0; i<this_States_Tag_मूल्यावली->size(); i++) {
                यदि (this_States_Tag_मूल्यावली->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    भङ्ग;
                }
            }

            यदि (i == this_States_Tag_मूल्यावली->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                भङ्ग;
            }
        }

        यदि (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_मूल्यावली->size(), *fStatus);
            प्रति (i=0; i<this_States_Tag_मूल्यावली->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_मूल्यावली->elementAti(i), *fStatus);
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

शून्य RBBITableBuilder::minimizeStates() {
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }

#यदि UPRV_HAS_SANITIZER
    यदि (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        प्रत्यागम;
    }
#endif
    संरचना StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        तार्किक operator==(StateType ध्रुव &other) ध्रुव {
            प्रत्यागम fAccepting == other.fAccepting &&
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
    संरचना TypeToStates : UMemory {
        TypeToStates(ध्रुव StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](शून्य *p) { delete static_cast<TypeToStates *>(p); });
    प्रति (int32_t i = 0; i < fDStates->size(); ++i) {
        ध्रुव RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ध्रुव StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        प्रति (; j < initialPartition.size(); ++j) {
            auto &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            यदि (type_j == type) {
                states->addElement(i, *fStatus);
                भङ्ग;
            }
        }
        यदि (j == initialPartition.size()) {
            auto newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            यदि (U_FAILURE(*fStatus)) {
                प्रत्यागम;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    auto partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    partition->setDeleter(
        [](शून्य *p) { delete static_cast<UVector*>(p); });
    
    प्रति (int32_t i = 0; i < initialPartition.size(); ++i) {
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
    auto partition_signature = [&partition, एतत्](int32_t stateIndex) -> prv::unique_ptr<UVector> {
        ध्रुव RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(stateIndex));
        auto result = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        यदि (U_FAILURE(*fStatus)) {
            प्रत्यागम nullptr;
        }
        प्रति (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            प्रति (int32_t partIndex = 0; partIndex < partition->size(); ++partIndex) {
                ध्रुव UVector &part = *static_cast<UVector *>(partition->elementAt(partIndex));
                यदि (part.contains(toState)) {
                    result->addElement(partIndex, *fStatus);
                    भङ्ग;
                }
            }
        }
        result->setComparer([](ध्रुव UElement left, ध्रुव UElement दायाँ) -> UBool {
            प्रत्यागम left.integer == दायाँ.integer;
        });
        प्रत्यागम result;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    प्रति (;;) {
        // Π_new.
        auto partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        यदि (U_FAILURE(*fStatus)) {
            प्रत्यागम;
        }
        partitionNew->setDeleter(
            [](शून्य *p) { delete static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        तार्किक refined = असत्य;
        प्रति (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ध्रुव UVector &group = *static_cast<UVector*>(partition->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            संरचना SignatureToStates : UMemory {
                SignatureToStates(prv::unique_ptr<UVector> signature, UErrorCode &status)
                    : signature(std::move(signature)), states(prv::make_unique<UVector>(status, status)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> states;
            };
            UVector groupRefinement(*fStatus);
            groupRefinement.setDeleter([](शून्य *p) { delete static_cast<SignatureToStates*>(p); });
            प्रति (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                ध्रुव int32_t s = group.elementAti(j);
                auto signature = partition_signature(s);
                int32_t k = 0;
                प्रति (; k < groupRefinement.size(); ++k) {
                    ध्रुव auto &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    यदि (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        भङ्ग;
                    }
                }
                यदि (k == groupRefinement.size()) {
                    ध्रुव auto newEntry =
                        नूतन SignatureToStates(std::move(signature), *fStatus);
                    यदि (U_FAILURE(*fStatus)) {
                        प्रत्यागम;
                    }
                    यदि (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      प्रत्यागम;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            प्रति (int32_t j = 0; j < groupRefinement.size(); ++j) {
                auto &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
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
        यदि (part_i->contains(0)) {
            शून्य *ध्रुव part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ध्रुव auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } अन्यथा यदि (part_i->contains(1)) {
            शून्य *ध्रुव part_1 = partition->elementAt(1);
            ध्रुव auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    auto oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    प्रति (पूर्णाङ्क i = 0; i < partition->size(); ++i) {
        ध्रुव UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        प्रति (पूर्णाङ्क j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    auto ध्रुव oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    प्रति (पूर्णाङ्क i = 0; i < partition->size(); ++i) {
        ध्रुव UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *ध्रुव state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        प्रति (पूर्णाङ्क j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->setElementAt(oldStateToPart[state->fDtran->elementAti(j)], j);
        }
        oldStates->setElementAt(nullptr, part.elementAti(0));
    }
    oldStates->setDeleter([](शून्य *p) { delete static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::sortedAdd(UVector **vector, int32_t val) {
    int32_t i;

    यदि (*vector == nullptr) {
        *vector = नूतन UVector(*fStatus);
    }
    यदि (*vector == nullptr || U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    UVector *vec = *vector;
    int32_t  vSize = vec->size();
    प्रति (i=0; i<vSize; i++) {
        int32_t valAtI = vec->elementAti(i);
        यदि (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            प्रत्यागम;
        }
        यदि (valAtI > val) {
            भङ्ग;
        }
    }
    vec->जोड़ो_Element_At(val, i, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::setAdd(UVector *dest, UVector *source) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!source->hasDeleter());
    int32_t destOriginalSize = dest->size();
    int32_t sourceSize       = source->size();
    int32_t di           = 0;
    Maybe_ढेर_Array<शून्य *, 16> destArray, sourceArray;  // Handle small cases without malloc
    शून्य **destPtr, **sourcePtr;
    शून्य **destLim, **sourceLim;

    यदि (destOriginalSize > destArray.getCapacity()) {
        यदि (destArray.resize(destOriginalSize) == nullptr) {
            प्रत्यागम;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + destOriginalSize;  // destArray.getArrayLimit()?

    यदि (sourceSize > sourceArray.getCapacity()) {
        यदि (sourceArray.resize(sourceSize) == nullptr) {
            प्रत्यागम;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + sourceSize;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (शून्य) dest->toArray(destPtr);
    (शून्य) source->toArray(sourcePtr);

    dest->setSize(sourceSize+destOriginalSize, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }

    यावत् (sourcePtr < sourceLim && destPtr < destLim) {
        यदि (*destPtr == *sourcePtr) {
            dest->setElementAt(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        अन्यथा यदि (uprv_memcmp(destPtr, sourcePtr, परिमाण(शून्य *)) < 0) {
            dest->setElementAt(*destPtr++, di++);
        }
        अन्यथा { /* *sourcePtr < *destPtr */
            dest->setElementAt(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    यावत् (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    यावत् (sourcePtr < sourceLim) {
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
    प्रत्यागम a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
शून्य RBBITableBuilder::printPosSets(RBBINode *n) {
    यदि (n==nullptr) {
        प्रत्यागम;
    }
    printf("\n");
    RBBINode::print_गाँठ_Header();
    RBBINode::print_गाँठ(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    printSet(n->fFirstPosSet);

    RBBIDebugPrintf("         lastpos:   ");
    printSet(n->fLastPosSet);

    RBBIDebugPrintf("         followpos: ");
    printSet(n->fFollowPos);

    printPosSets(n->f_बायाँ_Child);
    printPosSets(n->f_दायाँ_Child);
}
#endif

//
//    findDuplCharClassFrom()
//
तार्किक RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t अंक_States = fDStates->size();
    int32_t अंक_Cols = fRB->fSetBuilder->get_अंक_Char_Categories();

    प्रति (; categories->first < अंक_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        पूर्णाङ्क limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : अंक_Cols;
        प्रति (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            प्रति (int32_t state=0; state<अंक_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                यदि (table_base != table_dupl) {
                    भङ्ग;
                }
            }
            यदि (table_base == table_dupl) {
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
    int32_t अंक_States = fDStates->size();
    प्रति (int32_t state=0; state<अंक_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


तार्किक RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t अंक_States = fSafeTable->size();

    प्रति (; states->first<अंक_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        प्रति (states->second=states->first+1; states->second<अंक_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            तार्किक rowsMatch = सत्य;
            int32_t अंक_Cols = firstRow->length();
            प्रति (int32_t col=0; col < अंक_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                यदि (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = असत्य;
                    भङ्ग;
                }
            }
            यदि (rowsMatch) {
                प्रत्यागम सत्य;
            }
        }
    }
    प्रत्यागम असत्य;
}

शून्य RBBITableBuilder::removeSafeState(IntPair duplStates) {
    ध्रुव int32_t keepState = duplStates.first;
    ध्रुव int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t अंक_States = fSafeTable->size();
    प्रति (int32_t state=0; state<अंक_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t अंक_Cols = sd->length();
        प्रति (int32_t col=0; col<अंक_Cols; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            यदि (existingVal == duplState) {
                newVal = keepState;
            } अन्यथा यदि (existingVal > duplState) {
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
    ध्रुव int32_t old_State_गिनती = fDStates->size();
    minimizeStates();
    प्रत्यागम fDStates->size() - old_State_गिनती;
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
    यदि (fLASlotsInUse == ACCEPTING_UNCONDITIONAL) {
        प्रत्यागम;
    }
    ध्रुव int32_t lookahead_गिनती = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ध्रुव int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    ध्रुव auto stateDescriptor = [एतत्](ध्रुव int32_t state) -> RBBIStateDescriptor & {
        प्रत्यागम *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ध्रुव auto lookaheadReachability =
        prv::make_unique<तार्किक[]>(lookahead_गिनती * lookahead_गिनती, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    प्रति (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        प्रति (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
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
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_गिनती + k -
                                              firstLookahead] = सत्य;
                    }
                }
            }
        }
    }

    ध्रुव auto colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_गिनती, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    प्रति (int32_t chromatic_संख्या = 1;; ++chromatic_संख्या) {
        uprv_memset(colours.get(), 0, परिमाण(int32_t) * lookahead_गिनती);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        प्रति (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            प्रति (पूर्णाङ्क source = 0; source < lookahead_गिनती; ++source) {
                प्रति (पूर्णाङ्क sink = 0; sink < lookahead_गिनती; ++sink) {
                    यदि (lookaheadReachability[source * lookahead_गिनती + sink] &&
                        colours[source] == colours[sink]) {
                        गच्छ nextColouring;
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
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_संख्या;
            प्रत्यागम;

          nextColouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            प्रति (int32_t i = 0; i < lookahead_गिनती - 1 && colours[i] == chromatic_संख्या;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            यदि (colours[lookahead_गिनती - 1] == chromatic_संख्या) {
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
int32_t  RBBITableBuilder::getTableSize() ध्रुव {
    int32_t    size = 0;
    int32_t    अंक_Rows;
    int32_t    अंक_Cols;
    int32_t    rowSize;

    यदि (f_वृक्ष == nullptr) {
        प्रत्यागम 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    अंक_Rows = fDStates->size();
    अंक_Cols = fRB->fSetBuilder->get_अंक_Char_Categories();

    यदि (use8BitsForTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + परिमाण(int8_t)*अंक_Cols;
    } अन्यथा {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + परिमाण(int16_t)*अंक_Cols;
    }
    size   += अंक_Rows * rowSize;
    प्रत्यागम size;
}

तार्किक RBBITableBuilder::use8BitsForTable() ध्रुव {
    प्रत्यागम fDStates->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::exportTable(शून्य *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    पूर्णाङ्क                col;

    यदि (U_FAILURE(*fStatus) || f_वृक्ष == nullptr) {
        प्रत्यागम;
    }

    int32_t cat_गिनती = fRB->fSetBuilder->get_अंक_Char_Categories();
    यदि (cat_गिनती > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        प्रत्यागम;
    }

    table->f_अंक_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->fLookAheadResultsSize = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    यदि (use8BitsForTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + परिमाण(uint8_t) * cat_गिनती;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } अन्यथा {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + परिमाण(int16_t) * cat_गिनती;
    }
    यदि (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    प्रति (state=0; state<table->f_अंक_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        यदि (use8BitsForTable()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 255);
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->fTagsIdx   = sd->fTagsIdx;
            प्रति (col=0; col<cat_गिनती; col++) {
                U_ASSERT (sd->fDtran->elementAti(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = sd->fDtran->elementAti(col);
            }
        } अन्यथा {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 0xffff);
            row->r16.fAccepting = sd->fAccepting;
            row->r16.fLookAhead = sd->fLookAhead;
            row->r16.fTagsIdx   = sd->fTagsIdx;
            प्रति (col=0; col<cat_गिनती; col++) {
                row->r16.fNextState[col] = sd->fDtran->elementAti(col);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
शून्य RBBITableBuilder::buildSafeReverseTable(UErrorCode &status) {
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

    int32_t अंक_Char_Classes = fRB->fSetBuilder->get_अंक_Char_Categories();
    int32_t अंक_States = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ध्रुव auto lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    ध्रुव auto wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    प्रति (int32_t c1 = 1; c1 < अंक_Char_Classes; ++c1) {
        यदि (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          अनुवर्त;
        }
        प्रति (int32_t c2 = 1; c2 < अंक_Char_Classes; ++c2) {
            यदि (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                अनुवर्त;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            तार्किक mustCheckConsistentWithStartC2 = असत्य;
            ध्रुव int32_t text[2] = {c1, c2};
            प्रति (int32_t injectedState = START_STATE; injectedState < अंक_States; ++injectedState) {
                प्रति (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    यदि (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ध्रुव RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        यदि (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
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
                    गच्छ dfaStep;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    प्रति (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        प्रति (;;) {
                        dfaStep:
                            यदि (i == BEFORE_PAIR || i == 2) {
                                गच्छ inspectFinalState;
                            }
                            ध्रुव int32_t प्रतीक_Ahead = text[i];
                            ++i;
                            ध्रुव RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            यदि (sDescriptor->fDtran->elementAti(प्रतीक_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(प्रतीक_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } अन्यथा {
                                यदि (lastBreak != BEFORE_PAIR &&
                                    lastAcceptingPosition == lastBreak) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    गच्छ next_प्रतीक_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                भङ्ग;
                            }

                            यदि (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } अन्यथा यदि (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
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
                    यदि (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        प्रति (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                            यदि (lookaheadPositions[l] != LOOKAHEAD_NOT_SET &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wantedLookaheadsInPair[l] = lookaheadPositions[i];
                            } अन्यथा {
                                wantedLookaheadsInPair[l] = LOOKAHEAD_NOT_SET;
                            }
                        }
                    } अन्यथा {
                        यदि (wantedEndState != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            गच्छ next_प्रतीक_Pair;
                        }
                        यदि (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            गच्छ next_प्रतीक_Pair;
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
                    प्रति (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        यदि (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            अनुवर्त;
                        }
                        यदि (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            यदि (wantedEndState != START_STATE) {
                                गच्छ next_प्रतीक_Pair;
                            }
                        }
                        यदि (lookaheadPositions[l] == 1 || wantedLookaheadsInPair[l] == 1) {
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
                ध्रुव auto c2State = startDescriptor.fDtran->elementAti(c2);
                यदि (c2State != wantedEndState) {
                    गच्छ next_प्रतीक_Pair;
                }
                ध्रुव RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                यदि (wantedLastAcceptingPosition != 2) {
                    गच्छ next_प्रतीक_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                प्रति (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    यदि (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        गच्छ next_प्रतीक_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        next_प्रतीक_Pair:
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

    U_ASSERT(fSafeTable == nullptr);
    LocalPointer<UVector> lpSafeTable(
        नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, अंक_Char_Classes + 2, status), status);
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    fSafeTable = lpSafeTable.orphan();
    प्रति (int32_t row=0; row<अंक_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(नूतन UnicodeString(अंक_Char_Classes, 0, अंक_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    प्रति (int32_t charClass=0; charClass < अंक_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    प्रति (int32_t row=2; row<अंक_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    प्रति (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    यावत् (findDuplicateSafeState(&states)) {
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
int32_t  RBBITableBuilder::getSafeTableSize() ध्रुव {
    int32_t    size = 0;
    int32_t    अंक_Rows;
    int32_t    अंक_Cols;
    int32_t    rowSize;

    यदि (fSafeTable == nullptr) {
        प्रत्यागम 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    अंक_Rows = fSafeTable->size();
    अंक_Cols = fRB->fSetBuilder->get_अंक_Char_Categories();

    यदि (use8BitsForSafeTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + परिमाण(int8_t)*अंक_Cols;
    } अन्यथा {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + परिमाण(int16_t)*अंक_Cols;
    }
    size   += अंक_Rows * rowSize;
    प्रत्यागम size;
}

तार्किक RBBITableBuilder::use8BitsForSafeTable() ध्रुव {
    प्रत्यागम fSafeTable->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::exportSafeTable(शून्य *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    पूर्णाङ्क                col;

    यदि (U_FAILURE(*fStatus) || fSafeTable == nullptr) {
        प्रत्यागम;
    }

    int32_t cat_गिनती = fRB->fSetBuilder->get_अंक_Char_Categories();
    यदि (cat_गिनती > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        प्रत्यागम;
    }

    table->f_अंक_States = fSafeTable->size();
    table->fFlags     = 0;
    यदि (use8BitsForSafeTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + परिमाण(uint8_t) * cat_गिनती;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } अन्यथा {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + परिमाण(int16_t) * cat_गिनती;
    }

    प्रति (state=0; state<table->f_अंक_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        यदि (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            प्रति (col=0; col<cat_गिनती; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } अन्यथा {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            प्रति (col=0; col<cat_गिनती; col++) {
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
शून्य RBBITableBuilder::printSet(UVector *s) {
    int32_t  i;
    प्रति (i=0; i<s->size(); i++) {
        ध्रुव RBBINode *v = static_cast<ध्रुव RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==nullptr? -1 : v->f_Serial_अंक);
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
शून्य RBBITableBuilder::printStates() {
    पूर्णाङ्क     c;    // input "character"
    पूर्णाङ्क     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    प्रति (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    प्रति (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    प्रति (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        प्रति (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
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
शून्य RBBITableBuilder::printReverseTable() {
    पूर्णाङ्क     c;    // input "character"
    पूर्णाङ्क     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    यदि (fSafeTable == nullptr) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        प्रत्यागम;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    प्रति (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    प्रति (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    प्रति (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        प्रति (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
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
शून्य RBBITableBuilder::printRuleStatusTable() {
    int32_t  thisRecord = 0;
    int32_t  nextRecord = 0;
    पूर्णाङ्क      i;
    UVector  *tbl = fRB->fRuleStatusVals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    यावत् (nextRecord < tbl->size()) {
        thisRecord = nextRecord;
        nextRecord = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        प्रति (i=thisRecord+1; i<nextRecord; i++) {
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

RBBIStateDescriptor::RBBIStateDescriptor(पूर्णाङ्क last_निवेश_प्रतीक, UErrorCode *fStatus) {
    fMarked    = असत्य;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = नूतन UVector32(last_निवेश_प्रतीक+1, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }
    यदि (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    fDtran->setSize(last_निवेश_प्रतीक+1);    // fDtran needs to be pre-sized.
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
