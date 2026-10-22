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

#जर !UCONFIG_NO_BREAK_ITERATION

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
तार्किक reachableByTransitions(स्थिरांक UVector &states, स्थिरांक int32_t source,
                            स्थिरांक std::function<तार्किक(int32_t)> isSink,
                            स्थिरांक std::function<तार्किक(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        साठी (int32_t चिन्ह = 0; चिन्ह < transitionsFromSource.size(); ++चिन्ह) {
            स्थिरांक int32_t state = transitionsFromSource.elementAti(चिन्ह);
            जर (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    स्थिरांक स्वतः visited = prv::make_unique<तार्किक[]>(states.size(), status);
    पर्यंत (U_SUCCESS(status) && !boundary.empty()) {
        स्थिरांक int32_t s = boundary.popi();
        जर (isSink(s)) {
            परत सत्य;
        }
        जर (excludedState(s) || visited[s]) {
            सुरूठेवा;
        }
        visited[s] = सत्य;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        साठी (int32_t चिन्ह = 0; चिन्ह < transitions.size(); ++चिन्ह) {
            स्थिरांक int32_t t = transitions.elementAti(चिन्ह);
            जर (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    परत असत्य;
}

} // namespace

स्थिरांक int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **मूळ_गाठ, UErrorCode &status) :
        fRB(rb),
        f_वृक्ष(*मूळ_गाठ),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    जर (U_FAILURE(status)) {
        परत;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = नवीन UVector(status);
    जर (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    पूर्णांक i;
    साठी (i=0; i<fDStates->size(); i++) {
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

    जर (U_FAILURE(*fStatus)) {
        परत;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    जर (f_वृक्ष==nullptr) {
        परत;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_वृक्ष = f_वृक्ष->flattenVariables(*fStatus, 0);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
#ifdef RBBI_DEBUG
    जर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_वृक्ष(f_वृक्ष, सत्य);
    }
#endif

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = नवीन RBBINode(RBBINode::opCat, *fStatus);
    // Exit if memory allocation failed.
    जर (cn == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    जर (U_FAILURE(*fStatus)) {
        delete cn;
        परत;
    }
    cn->f_डावा_Child = f_वृक्ष;
    f_वृक्ष->fParent = cn;
    RBBINode *end_Marker_गाठ = cn->f_उजवा_Child = नवीन RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    जर (cn->f_उजवा_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    जर (U_FAILURE(*fStatus)) {
        delete cn;
        परत;
    }
    cn->f_उजवा_Child->fParent = cn;
    f_वृक्ष = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_वृक्ष->flattenSets(*fStatus, 0);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
#ifdef RBBI_DEBUG
    जर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
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
    जर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_वृक्ष);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    जर (fRB->fChainRules) {
        calcChainedFollowPos(f_वृक्ष, end_Marker_गाठ);
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
    जर (n == nullptr) {
        परत;
    }
    जर (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = असत्य;
        परत;
    }

    जर (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = सत्य;
        परत;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_डावा_Child);
    calcNullable(n->f_उजवा_Child);

    // Apply functions from table 3.40 in Aho
    जर (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_डावा_Child->fNullable || n->f_उजवा_Child->fNullable;
    }
    अन्यथा जर (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_डावा_Child->fNullable && n->f_उजवा_Child->fNullable;
    }
    अन्यथा जर (n->fType == RBBINode::opStar || n->fType == RBBINode::opQuestion) {
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
    जर (n == nullptr) {
        परत;
    }
    जर (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fFirstPosSet->addElement(n, *fStatus);
        परत;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_डावा_Child);
    calcFirstPos(n->f_उजवा_Child);

    // Apply functions from table 3.40 in Aho
    जर (n->fType == RBBINode::opOr) {
        setAdd(n->fFirstPosSet, n->f_डावा_Child->fFirstPosSet);
        setAdd(n->fFirstPosSet, n->f_उजवा_Child->fFirstPosSet);
    }
    अन्यथा जर (n->fType == RBBINode::opCat) {
        setAdd(n->fFirstPosSet, n->f_डावा_Child->fFirstPosSet);
        जर (n->f_डावा_Child->fNullable) {
            setAdd(n->fFirstPosSet, n->f_उजवा_Child->fFirstPosSet);
        }
    }
    अन्यथा जर (n->fType == RBBINode::opStar ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fFirstPosSet, n->f_डावा_Child->fFirstPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcLastPos(RBBINode *n) {
    जर (n == nullptr) {
        परत;
    }
    जर (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fLastPosSet->addElement(n, *fStatus);
        परत;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calcLastPos(n->f_डावा_Child);
    calcLastPos(n->f_उजवा_Child);

    // Apply functions from table 3.40 in Aho
    जर (n->fType == RBBINode::opOr) {
        setAdd(n->fLastPosSet, n->f_डावा_Child->fLastPosSet);
        setAdd(n->fLastPosSet, n->f_उजवा_Child->fLastPosSet);
    }
    अन्यथा जर (n->fType == RBBINode::opCat) {
        setAdd(n->fLastPosSet, n->f_उजवा_Child->fLastPosSet);
        जर (n->f_उजवा_Child->fNullable) {
            setAdd(n->fLastPosSet, n->f_डावा_Child->fLastPosSet);
        }
    }
    अन्यथा जर (n->fType == RBBINode::opStar     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fLastPosSet, n->f_डावा_Child->fLastPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcFollowPos(RBBINode *n) {
    जर (n == nullptr ||
        n->fType == RBBINode::leafChar ||
        n->fType == RBBINode::endMark) {
        परत;
    }

    calcFollowPos(n->f_डावा_Child);
    calcFollowPos(n->f_उजवा_Child);

    // Aho rule #1
    जर (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_डावा_Child = n->f_डावा_Child->fLastPosSet;

        साठी (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_डावा_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_डावा_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_उजवा_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    जर (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        साठी (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
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
शून्य RBBITableBuilder::add_Rule_मूळ_Nodes(UVector *dest, RBBINode *गाठ) {
    जर (गाठ == nullptr || U_FAILURE(*fStatus)) {
        परत;
    }
    U_ASSERT(!dest->hasDeleter());
    जर (गाठ->f_Rule_मूळ) {
        dest->addElement(गाठ, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        परत;
    }
    add_Rule_मूळ_Nodes(dest, गाठ->f_डावा_Child);
    add_Rule_मूळ_Nodes(dest, गाठ->f_उजवा_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcChainedFollowPos(RBBINode *वृक्ष, RBBINode *end_Mark_गाठ) {

    UVector         leafNodes(*fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }

    // get a list all leaf nodes
    वृक्ष->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_मूळ_Nodes(*fStatus);
    add_Rule_मूळ_Nodes(&rule_मूळ_Nodes, वृक्ष);

    UVector matchStartNodes(*fStatus);
    साठी (पूर्णांक j=0; j<rule_मूळ_Nodes.size(); ++j) {
        RBBINode *गाठ = static_cast<RBBINode *>(rule_मूळ_Nodes.elementAt(j));
        जर (गाठ->fChainIn) {
            setAdd(&matchStartNodes, गाठ->fFirstPosSet);
        }
    }
    जर (U_FAILURE(*fStatus)) {
        परत;
    }

    int32_t  end_गाठ_Ix;
    int32_t  start_गाठ_Ix;

    साठी (end_गाठ_Ix=0; end_गाठ_Ix<leafNodes.size(); end_गाठ_Ix++) {
        RBBINode* end_गाठ = static_cast<RBBINode*>(leafNodes.elementAt(end_गाठ_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        जर (!end_गाठ->fFollowPos->contains(end_Mark_गाठ)) {
            सुरूठेवा;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_गाठ;
        साठी (start_गाठ_Ix = 0; start_गाठ_Ix<matchStartNodes.size(); start_गाठ_Ix++) {
            start_गाठ = static_cast<RBBINode*>(matchStartNodes.elementAt(start_गाठ_Ix));
            जर (start_गाठ->fType != RBBINode::leafChar) {
                सुरूठेवा;
            }

            जर (end_गाठ->fVal == start_गाठ->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_गाठ->fFollowPos, start_गाठ->fFollowPos);
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

    जर (U_FAILURE(*fStatus)) {
        परत;
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
    RBBINode  *bof_गाठ = f_वृक्ष->f_डावा_Child->f_डावा_Child;
    U_ASSERT(bof_गाठ->fType == RBBINode::leafChar);
    U_ASSERT(bof_गाठ->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_वृक्ष->f_डावा_Child->f_उजवा_Child->fFirstPosSet;

    RBBINode *start_गाठ;
    पूर्णांक       start_गाठ_Ix;
    साठी (start_गाठ_Ix = 0; start_गाठ_Ix<matchStartNodes->size(); start_गाठ_Ix++) {
        start_गाठ = static_cast<RBBINode*>(matchStartNodes->elementAt(start_गाठ_Ix));
        जर (start_गाठ->fType != RBBINode::leafChar) {
            सुरूठेवा;
        }

        जर (start_गाठ->fVal == bof_गाठ->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_गाठ->fFollowPos, start_गाठ->fFollowPos);
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
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    पूर्णांक      last_आदान_चिन्ह = fRB->fSetBuilder->get_अंक_Char_Categories() - 1;
    failState = नवीन RBBIStateDescriptor(last_आदान_चिन्ह, fStatus);
    जर (failState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        जा ExitBuildSTdeleteall;
    }
    failState->fPositions = नवीन UVector(*fStatus);
    जर (failState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    जर (failState->fPositions == nullptr || U_FAILURE(*fStatus)) {
        जा ExitBuildSTdeleteall;
    }
    fDStates->addElement(failState, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        जा ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = नवीन RBBIStateDescriptor(last_आदान_चिन्ह, fStatus);
    जर (initialState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    जर (U_FAILURE(*fStatus)) {
        जा ExitBuildSTdeleteall;
    }
    initialState->fPositions = नवीन UVector(*fStatus);
    जर (initialState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    जर (U_FAILURE(*fStatus)) {
        जा ExitBuildSTdeleteall;
    }
    setAdd(initialState->fPositions, f_वृक्ष->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        जा ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    साठी (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        साठी (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *temp;
            temp = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            जर (temp->fMarked == असत्य) {
                T = temp;
                थांबा;
            }
        }
        जर (T == nullptr) {
            थांबा;
        }

        // mark T;
        T->fMarked = सत्य;

        // for each input symbol a do begin
        int32_t  a;
        साठी (a = 1; a<=last_आदान_चिन्ह; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            साठी (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                जर ((p->fType == RBBINode::leafChar) &&  (p->fVal == a)) {
                    जर (U == nullptr) {
                        U = नवीन UVector(*fStatus);
                        जर (U == nullptr) {
                        	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                        	जा ExitBuildSTdeleteall;
                        }
                    }
                    setAdd(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = असत्य;
            जर (U != nullptr) {
                U_ASSERT(U->size() > 0);
                पूर्णांक  ix;
                साठी (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    जर (setEquals(U, temp2->fPositions)) {
                        delete U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = सत्य;
                        थांबा;
                    }
                }

                // Add U as an unmarked state to Dstates
                जर (!UinDstates)
                {
                    RBBIStateDescriptor *newState = नवीन RBBIStateDescriptor(last_आदान_चिन्ह, fStatus);
                    जर (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    जर (U_FAILURE(*fStatus)) {
                        जा ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    जर (U_FAILURE(*fStatus)) {
                        परत;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    परत;
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
    fLookAheadRuleMap =  नवीन UVector32(fRB->fScanner->अंक_Rules() + 1, *fStatus);
    जर (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    fLookAheadRuleMap->setSize(fRB->fScanner->अंक_Rules() + 1);

    साठी (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        तार्किक saw_Look_Ahead_गाठ = असत्य;
        साठी (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *गाठ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            जर (गाठ->fType != RBBINode::गाठ_Type::lookAhead) {
                सुरूठेवा;
            }
            saw_Look_Ahead_गाठ = सत्य;
            int32_t rule_अंक = गाठ->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_अंक < fLookAheadRuleMap->size());
            U_ASSERT(rule_अंक > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_अंक);
            जर (laSlot != 0) {
                जर (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } अन्यथा {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        जर (!saw_Look_Ahead_गाठ) {
            सुरूठेवा;
        }

        जर (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        साठी (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *गाठ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            जर (गाठ->fType != RBBINode::गाठ_Type::lookAhead) {
                सुरूठेवा;
            }
            int32_t rule_अंक = गाठ->fVal;     // Set when rule was originally parsed.
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
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    जर (U_FAILURE(*fStatus)) {
        परत;
    }

    f_वृक्ष->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }

    साठी (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        साठी (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            जर (sd->fPositions->indexOf(endMarker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                जर (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
                    जर (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                जर (sd->fAccepting==ACCEPTING_UNCONDITIONAL && endMarker->fVal != 0) {
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
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_गाठ;
    int32_t     i;
    int32_t     n;

    f_वृक्ष->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    साठी (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_गाठ = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_गाठ->fType == RBBINode::गाठ_Type::lookAhead);

        साठी (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->indexOf(look_Ahead_गाठ);
            जर (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_गाठ == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_गाठ->fVal);
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
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_गाठ;
    int32_t     i;
    int32_t     n;

    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    f_वृक्ष->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    साठी (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_गाठ = static_cast<RBBINode*>(tagNodes.elementAt(i));

        साठी (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            जर (sd->fPositions->indexOf(tag_गाठ) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_गाठ->fVal);
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
    पूर्णांक i;
    पूर्णांक n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    जर (fRB->fRuleStatusVals->size() == 0) {
        fRB->fRuleStatusVals->addElement(1, *fStatus);  // Num of statuses in group
        fRB->fRuleStatusVals->addElement(static_cast<int32_t>(0), *fStatus); // and our single status of zero
    }

    //    For each state
    साठी (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_मूल्ये = sd->fTagVals;
        जर (this_States_Tag_मूल्ये == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            सुरूठेवा;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  nextTagGroupStart = 0;

        // Loop runs once per group of tags in the global list
        पर्यंत (nextTagGroupStart < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = nextTagGroupStart;
            nextTagGroupStart += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            जर (this_States_Tag_मूल्ये->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                सुरूठेवा;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            साठी (i=0; i<this_States_Tag_मूल्ये->size(); i++) {
                जर (this_States_Tag_मूल्ये->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    थांबा;
                }
            }

            जर (i == this_States_Tag_मूल्ये->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                थांबा;
            }
        }

        जर (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_मूल्ये->size(), *fStatus);
            साठी (i=0; i<this_States_Tag_मूल्ये->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_मूल्ये->elementAti(i), *fStatus);
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
    जर (U_FAILURE(*fStatus)) {
        परत;
    }

#जर UPRV_HAS_SANITIZER
    जर (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        परत;
    }
#endif
    संरचना StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        तार्किक चालक==(StateType स्थिरांक &other) स्थिरांक {
            परत fAccepting == other.fAccepting &&
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
        TypeToStates(स्थिरांक StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](शून्य *p) { delete static_cast<TypeToStates *>(p); });
    साठी (int32_t i = 0; i < fDStates->size(); ++i) {
        स्थिरांक RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        स्थिरांक StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        साठी (; j < initialPartition.size(); ++j) {
            स्वतः &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            जर (type_j == type) {
                states->addElement(i, *fStatus);
                थांबा;
            }
        }
        जर (j == initialPartition.size()) {
            स्वतः newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            जर (U_FAILURE(*fStatus)) {
                परत;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    स्वतः partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    partition->setDeleter(
        [](शून्य *p) { delete static_cast<UVector*>(p); });
    
    साठी (int32_t i = 0; i < initialPartition.size(); ++i) {
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
    स्वतः partition_signature = [&partition, हे](int32_t stateIndex) -> prv::unique_ptr<UVector> {
        स्थिरांक RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(stateIndex));
        स्वतः result = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        जर (U_FAILURE(*fStatus)) {
            परत nullptr;
        }
        साठी (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            साठी (int32_t partIndex = 0; partIndex < partition->size(); ++partIndex) {
                स्थिरांक UVector &part = *static_cast<UVector *>(partition->elementAt(partIndex));
                जर (part.contains(toState)) {
                    result->addElement(partIndex, *fStatus);
                    थांबा;
                }
            }
        }
        result->setComparer([](स्थिरांक UElement left, स्थिरांक UElement उजवा) -> UBool {
            परत left.integer == उजवा.integer;
        });
        परत result;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    साठी (;;) {
        // Π_new.
        स्वतः partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        जर (U_FAILURE(*fStatus)) {
            परत;
        }
        partitionNew->setDeleter(
            [](शून्य *p) { delete static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        तार्किक refined = असत्य;
        साठी (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            स्थिरांक UVector &group = *static_cast<UVector*>(partition->elementAt(i));
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
            साठी (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                स्थिरांक int32_t s = group.elementAti(j);
                स्वतः signature = partition_signature(s);
                int32_t k = 0;
                साठी (; k < groupRefinement.size(); ++k) {
                    स्थिरांक स्वतः &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    जर (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        थांबा;
                    }
                }
                जर (k == groupRefinement.size()) {
                    स्थिरांक स्वतः newEntry =
                        नवीन SignatureToStates(std::move(signature), *fStatus);
                    जर (U_FAILURE(*fStatus)) {
                        परत;
                    }
                    जर (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      परत;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            साठी (int32_t j = 0; j < groupRefinement.size(); ++j) {
                स्वतः &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
            }
        }
        जर (refined) {
            partition = std::move(partitionNew);
        } अन्यथा {
            थांबा;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    साठी (पूर्णांक i = 0; i < partition->size(); ++i) {
        UVector *स्थिरांक part_i = static_cast<UVector *>(partition->elementAt(i));
        जर (part_i->contains(0)) {
            शून्य *स्थिरांक part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            स्थिरांक स्वतः deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } अन्यथा जर (part_i->contains(1)) {
            शून्य *स्थिरांक part_1 = partition->elementAt(1);
            स्थिरांक स्वतः deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    स्वतः oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    साठी (पूर्णांक i = 0; i < partition->size(); ++i) {
        स्थिरांक UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        साठी (पूर्णांक j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    स्वतः स्थिरांक oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    साठी (पूर्णांक i = 0; i < partition->size(); ++i) {
        स्थिरांक UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *स्थिरांक state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        साठी (पूर्णांक j = 0; j < state->fDtran->size(); ++j) {
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

    जर (*vector == nullptr) {
        *vector = नवीन UVector(*fStatus);
    }
    जर (*vector == nullptr || U_FAILURE(*fStatus)) {
        परत;
    }
    UVector *vec = *vector;
    int32_t  vSize = vec->size();
    साठी (i=0; i<vSize; i++) {
        int32_t valAtI = vec->elementAti(i);
        जर (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            परत;
        }
        जर (valAtI > val) {
            थांबा;
        }
    }
    vec->घाला_Element_At(val, i, *fStatus);
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
    Maybe_चळत_Array<शून्य *, 16> destArray, sourceArray;  // Handle small cases without malloc
    शून्य **destPtr, **sourcePtr;
    शून्य **destLim, **sourceLim;

    जर (destOriginalSize > destArray.getCapacity()) {
        जर (destArray.resize(destOriginalSize) == nullptr) {
            परत;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + destOriginalSize;  // destArray.getArrayLimit()?

    जर (sourceSize > sourceArray.getCapacity()) {
        जर (sourceArray.resize(sourceSize) == nullptr) {
            परत;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + sourceSize;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (शून्य) dest->toArray(destPtr);
    (शून्य) source->toArray(sourcePtr);

    dest->setSize(sourceSize+destOriginalSize, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }

    पर्यंत (sourcePtr < sourceLim && destPtr < destLim) {
        जर (*destPtr == *sourcePtr) {
            dest->setElementAt(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        अन्यथा जर (uprv_memcmp(destPtr, sourcePtr, आकार(शून्य *)) < 0) {
            dest->setElementAt(*destPtr++, di++);
        }
        अन्यथा { /* *sourcePtr < *destPtr */
            dest->setElementAt(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    पर्यंत (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    पर्यंत (sourcePtr < sourceLim) {
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
    परत a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
शून्य RBBITableBuilder::printPosSets(RBBINode *n) {
    जर (n==nullptr) {
        परत;
    }
    म_लिखो("\n");
    RBBINode::print_गाठ_Header();
    RBBINode::print_गाठ(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    printSet(n->fFirstPosSet);

    RBBIDebugPrintf("         lastpos:   ");
    printSet(n->fLastPosSet);

    RBBIDebugPrintf("         followpos: ");
    printSet(n->fFollowPos);

    printPosSets(n->f_डावा_Child);
    printPosSets(n->f_उजवा_Child);
}
#endif

//
//    findDuplCharClassFrom()
//
तार्किक RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t अंक_States = fDStates->size();
    int32_t अंक_Cols = fRB->fSetBuilder->get_अंक_Char_Categories();

    साठी (; categories->first < अंक_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        पूर्णांक limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : अंक_Cols;
        साठी (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            साठी (int32_t state=0; state<अंक_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                जर (table_base != table_dupl) {
                    थांबा;
                }
            }
            जर (table_base == table_dupl) {
                परत सत्य;
            }
        }
    }
    परत असत्य;
}


//
//    removeColumn()
//
शून्य RBBITableBuilder::removeColumn(int32_t column) {
    int32_t अंक_States = fDStates->size();
    साठी (int32_t state=0; state<अंक_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


तार्किक RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t अंक_States = fSafeTable->size();

    साठी (; states->first<अंक_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        साठी (states->second=states->first+1; states->second<अंक_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            तार्किक rowsMatch = सत्य;
            int32_t अंक_Cols = firstRow->length();
            साठी (int32_t col=0; col < अंक_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                जर (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = असत्य;
                    थांबा;
                }
            }
            जर (rowsMatch) {
                परत सत्य;
            }
        }
    }
    परत असत्य;
}

शून्य RBBITableBuilder::removeSafeState(IntPair duplStates) {
    स्थिरांक int32_t keepState = duplStates.first;
    स्थिरांक int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t अंक_States = fSafeTable->size();
    साठी (int32_t state=0; state<अंक_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t अंक_Cols = sd->length();
        साठी (int32_t col=0; col<अंक_Cols; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            जर (existingVal == duplState) {
                newVal = keepState;
            } अन्यथा जर (existingVal > duplState) {
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
    स्थिरांक int32_t old_State_मोजणी = fDStates->size();
    minimizeStates();
    परत fDStates->size() - old_State_मोजणी;
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
    जर (fLASlotsInUse == ACCEPTING_UNCONDITIONAL) {
        परत;
    }
    स्थिरांक int32_t lookahead_मोजणी = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    स्थिरांक int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    स्थिरांक स्वतः stateDescriptor = [हे](स्थिरांक int32_t state) -> RBBIStateDescriptor & {
        परत *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    स्थिरांक स्वतः lookaheadReachability =
        prv::make_unique<तार्किक[]>(lookahead_मोजणी * lookahead_मोजणी, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    साठी (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        साठी (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
            जर (k != l) {
                साठी (int32_t source = 1; source < fDStates->size(); ++source) {
                    जर (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        सुरूठेवा;
                    }
                    जर (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](स्थिरांक int32_t state) {
                                परत static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](स्थिरांक int32_t state) {
                                परत static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_मोजणी + k -
                                              firstLookahead] = सत्य;
                    }
                }
            }
        }
    }

    स्थिरांक स्वतः colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_मोजणी, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    साठी (int32_t chromatic_संख्या = 1;; ++chromatic_संख्या) {
        uprv_memset(colours.get(), 0, आकार(int32_t) * lookahead_मोजणी);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        साठी (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            साठी (पूर्णांक source = 0; source < lookahead_मोजणी; ++source) {
                साठी (पूर्णांक sink = 0; sink < lookahead_मोजणी; ++sink) {
                    जर (lookaheadReachability[source * lookahead_मोजणी + sink] &&
                        colours[source] == colours[sink]) {
                        जा nextColouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            साठी (पूर्णांक i = 0; i < fDStates->size(); ++i) {
                जर (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        firstLookahead +
                        colours[stateDescriptor(i).fAccepting - firstLookahead];
                }
                जर (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - firstLookahead];
                }
            }
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_संख्या;
            परत;

          nextColouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            साठी (int32_t i = 0; i < lookahead_मोजणी - 1 && colours[i] == chromatic_संख्या;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            जर (colours[lookahead_मोजणी - 1] == chromatic_संख्या) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                थांबा;
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
int32_t  RBBITableBuilder::getTableSize() स्थिरांक {
    int32_t    size = 0;
    int32_t    अंक_Rows;
    int32_t    अंक_Cols;
    int32_t    rowSize;

    जर (f_वृक्ष == nullptr) {
        परत 0;
    }

    size    = दुरत्व(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    अंक_Rows = fDStates->size();
    अंक_Cols = fRB->fSetBuilder->get_अंक_Char_Categories();

    जर (use8BitsForTable()) {
        rowSize = दुरत्व(RBBIStateTableRow8, fNextState) + आकार(int8_t)*अंक_Cols;
    } अन्यथा {
        rowSize = दुरत्व(RBBIStateTableRow16, fNextState) + आकार(int16_t)*अंक_Cols;
    }
    size   += अंक_Rows * rowSize;
    परत size;
}

तार्किक RBBITableBuilder::use8BitsForTable() स्थिरांक {
    परत fDStates->size() <= kMaxStateFor8BitsTable;
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
    पूर्णांक                col;

    जर (U_FAILURE(*fStatus) || f_वृक्ष == nullptr) {
        परत;
    }

    int32_t cat_मोजणी = fRB->fSetBuilder->get_अंक_Char_Categories();
    जर (cat_मोजणी > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        परत;
    }

    table->f_अंक_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->fLookAheadResultsSize = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    जर (use8BitsForTable()) {
        table->fRowLen    = दुरत्व(RBBIStateTableRow8, fNextState) + आकार(uint8_t) * cat_मोजणी;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } अन्यथा {
        table->fRowLen    = दुरत्व(RBBIStateTableRow16, fNextState) + आकार(int16_t) * cat_मोजणी;
    }
    जर (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    साठी (state=0; state<table->f_अंक_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        जर (use8BitsForTable()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 255);
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->fTagsIdx   = sd->fTagsIdx;
            साठी (col=0; col<cat_मोजणी; col++) {
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
            साठी (col=0; col<cat_मोजणी; col++) {
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
    स्थिरांक स्वतः lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    स्थिरांक स्वतः wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    जर (U_FAILURE(status)) {
        परत;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    साठी (int32_t c1 = 1; c1 < अंक_Char_Classes; ++c1) {
        जर (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          सुरूठेवा;
        }
        साठी (int32_t c2 = 1; c2 < अंक_Char_Classes; ++c2) {
            जर (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                सुरूठेवा;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            तार्किक mustCheckConsistentWithStartC2 = असत्य;
            स्थिरांक int32_t text[2] = {c1, c2};
            साठी (int32_t injectedState = START_STATE; injectedState < अंक_States; ++injectedState) {
                साठी (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    जर (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        स्थिरांक RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        जर (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
                        } अन्यथा जर (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            सुरूठेवा;
                        }
                        जर (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    जा dfaStep;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    साठी (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        साठी (;;) {
                        dfaStep:
                            जर (i == BEFORE_PAIR || i == 2) {
                                जा inspectFinalState;
                            }
                            स्थिरांक int32_t चिन्ह_Ahead = text[i];
                            ++i;
                            स्थिरांक RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            जर (sDescriptor->fDtran->elementAti(चिन्ह_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(चिन्ह_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } अन्यथा {
                                जर (lastBreak != BEFORE_PAIR &&
                                    lastAcceptingPosition == lastBreak) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    जा next_चिन्ह_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                थांबा;
                            }

                            जर (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } अन्यथा जर (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
                                थांबा;
                            }
                            जर (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    जर (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        सुरूठेवा;
                    }
                    जर (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        साठी (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                            जर (lookaheadPositions[l] != LOOKAHEAD_NOT_SET &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wantedLookaheadsInPair[l] = lookaheadPositions[i];
                            } अन्यथा {
                                wantedLookaheadsInPair[l] = LOOKAHEAD_NOT_SET;
                            }
                        }
                    } अन्यथा {
                        जर (wantedEndState != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            जा next_चिन्ह_Pair;
                        }
                        जर (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            जा next_चिन्ह_Pair;
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
                    साठी (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        जर (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            सुरूठेवा;
                        }
                        जर (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            जर (wantedEndState != START_STATE) {
                                जा next_चिन्ह_Pair;
                            }
                        }
                        जर (lookaheadPositions[l] == 1 || wantedLookaheadsInPair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            mustCheckConsistentWithStartC2 = सत्य;
                        }
                    }
                }
            }
            जर (mustCheckConsistentWithStartC2) {
                स्थिरांक RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                स्थिरांक स्वतः c2State = startDescriptor.fDtran->elementAti(c2);
                जर (c2State != wantedEndState) {
                    जा next_चिन्ह_Pair;
                }
                स्थिरांक RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                जर (wantedLastAcceptingPosition != 2) {
                    जा next_चिन्ह_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                साठी (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    जर (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        जा next_चिन्ह_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        next_चिन्ह_Pair:
            सुरूठेवा;
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
        नवीन UVector(uprv_deleteUObject, uhash_compareUnicodeString, अंक_Char_Classes + 2, status), status);
    जर (U_FAILURE(status)) {
        परत;
    }
    fSafeTable = lpSafeTable.orphan();
    साठी (int32_t row=0; row<अंक_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(नवीन UnicodeString(अंक_Char_Classes, 0, अंक_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    जर (U_FAILURE(status)) {
        परत;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    साठी (int32_t charClass=0; charClass < अंक_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    साठी (int32_t row=2; row<अंक_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    साठी (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    पर्यंत (findDuplicateSafeState(&states)) {
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
int32_t  RBBITableBuilder::getSafeTableSize() स्थिरांक {
    int32_t    size = 0;
    int32_t    अंक_Rows;
    int32_t    अंक_Cols;
    int32_t    rowSize;

    जर (fSafeTable == nullptr) {
        परत 0;
    }

    size    = दुरत्व(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    अंक_Rows = fSafeTable->size();
    अंक_Cols = fRB->fSetBuilder->get_अंक_Char_Categories();

    जर (use8BitsForSafeTable()) {
        rowSize = दुरत्व(RBBIStateTableRow8, fNextState) + आकार(int8_t)*अंक_Cols;
    } अन्यथा {
        rowSize = दुरत्व(RBBIStateTableRow16, fNextState) + आकार(int16_t)*अंक_Cols;
    }
    size   += अंक_Rows * rowSize;
    परत size;
}

तार्किक RBBITableBuilder::use8BitsForSafeTable() स्थिरांक {
    परत fSafeTable->size() <= kMaxStateFor8BitsTable;
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
    पूर्णांक                col;

    जर (U_FAILURE(*fStatus) || fSafeTable == nullptr) {
        परत;
    }

    int32_t cat_मोजणी = fRB->fSetBuilder->get_अंक_Char_Categories();
    जर (cat_मोजणी > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        परत;
    }

    table->f_अंक_States = fSafeTable->size();
    table->fFlags     = 0;
    जर (use8BitsForSafeTable()) {
        table->fRowLen    = दुरत्व(RBBIStateTableRow8, fNextState) + आकार(uint8_t) * cat_मोजणी;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } अन्यथा {
        table->fRowLen    = दुरत्व(RBBIStateTableRow16, fNextState) + आकार(int16_t) * cat_मोजणी;
    }

    साठी (state=0; state<table->f_अंक_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        जर (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            साठी (col=0; col<cat_मोजणी; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } अन्यथा {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            साठी (col=0; col<cat_मोजणी; col++) {
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
    साठी (i=0; i<s->size(); i++) {
        स्थिरांक RBBINode *v = static_cast<स्थिरांक RBBINode *>(s->elementAt(i));
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
    पूर्णांक     c;    // input "character"
    पूर्णांक     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    साठी (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    साठी (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    साठी (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        साठी (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
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
    पूर्णांक     c;    // input "character"
    पूर्णांक     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    जर (fSafeTable == nullptr) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        परत;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    साठी (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    साठी (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    साठी (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        साठी (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
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
    पूर्णांक      i;
    UVector  *tbl = fRB->fRuleStatusVals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    पर्यंत (nextRecord < tbl->size()) {
        thisRecord = nextRecord;
        nextRecord = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        साठी (i=thisRecord+1; i<nextRecord; i++) {
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

RBBIStateDescriptor::RBBIStateDescriptor(पूर्णांक last_आदान_चिन्ह, UErrorCode *fStatus) {
    fMarked    = असत्य;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = नवीन UVector32(last_आदान_चिन्ह+1, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        परत;
    }
    जर (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    fDtran->setSize(last_आदान_चिन्ह+1);    // fDtran needs to be pre-sized.
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
