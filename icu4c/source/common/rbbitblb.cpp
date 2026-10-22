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
तार्किक reachableByTransitions(अचर UVector &states, अचर int32_t source,
                            अचर std::function<तार्किक(int32_t)> isSink,
                            अचर std::function<तार्किक(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        लागि (int32_t चिह्न = 0; चिह्न < transitionsFromSource.size(); ++चिह्न) {
            अचर int32_t state = transitionsFromSource.elementAti(चिह्न);
            यदि (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    अचर स्वतः visited = prv::make_unique<तार्किक[]>(states.size(), status);
    जबसम्म (U_SUCCESS(status) && !boundary.empty()) {
        अचर int32_t s = boundary.popi();
        यदि (isSink(s)) {
            फर्काऊ सत्य;
        }
        यदि (excludedState(s) || visited[s]) {
            जारी;
        }
        visited[s] = सत्य;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        लागि (int32_t चिह्न = 0; चिह्न < transitions.size(); ++चिह्न) {
            अचर int32_t t = transitions.elementAti(चिह्न);
            यदि (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    फर्काऊ असत्य;
}

} // namespace

अचर int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **जरा_गाँठो, UErrorCode &status) :
        fRB(rb),
        f_रूख(*जरा_गाँठो),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = नूतन UVector(status);
    यदि (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    पूर्णाङ्क i;
    लागि (i=0; i<fDStates->size(); i++) {
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
        फर्काऊ;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    यदि (f_रूख==nullptr) {
        फर्काऊ;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_रूख = f_रूख->flattenVariables(*fStatus, 0);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
#ifdef RBBI_DEBUG
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_रूख(f_रूख, सत्य);
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
        फर्काऊ;
    }
    cn->f_बायाँ_Child = f_रूख;
    f_रूख->fParent = cn;
    RBBINode *end_Marker_गाँठो = cn->f_दायाँ_Child = नूतन RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    यदि (cn->f_दायाँ_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(*fStatus)) {
        delete cn;
        फर्काऊ;
    }
    cn->f_दायाँ_Child->fParent = cn;
    f_रूख = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_रूख->flattenSets(*fStatus, 0);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
#ifdef RBBI_DEBUG
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_रूख(f_रूख, सत्य);
    }
#endif


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_रूख);
    calcFirstPos(f_रूख);
    calcLastPos(f_रूख);
    calcFollowPos(f_रूख);
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_रूख);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    यदि (fRB->fChainRules) {
        calcChainedFollowPos(f_रूख, end_Marker_गाँठो);
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
        फर्काऊ;
    }
    यदि (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = असत्य;
        फर्काऊ;
    }

    यदि (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = सत्य;
        फर्काऊ;
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
        फर्काऊ;
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
        फर्काऊ;
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
        फर्काऊ;
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
        फर्काऊ;
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
        फर्काऊ;
    }

    calcFollowPos(n->f_बायाँ_Child);
    calcFollowPos(n->f_दायाँ_Child);

    // Aho rule #1
    यदि (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_बायाँ_Child = n->f_बायाँ_Child->fLastPosSet;

        लागि (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_बायाँ_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_बायाँ_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_दायाँ_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    यदि (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        लागि (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
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
शून्य RBBITableBuilder::add_Rule_जरा_Nodes(UVector *dest, RBBINode *गाँठो) {
    यदि (गाँठो == nullptr || U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    U_ASSERT(!dest->hasDeleter());
    यदि (गाँठो->f_Rule_जरा) {
        dest->addElement(गाँठो, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        फर्काऊ;
    }
    add_Rule_जरा_Nodes(dest, गाँठो->f_बायाँ_Child);
    add_Rule_जरा_Nodes(dest, गाँठो->f_दायाँ_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
शून्य RBBITableBuilder::calcChainedFollowPos(RBBINode *रूख, RBBINode *end_Mark_गाँठो) {

    UVector         leafNodes(*fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }

    // get a list all leaf nodes
    रूख->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_जरा_Nodes(*fStatus);
    add_Rule_जरा_Nodes(&rule_जरा_Nodes, रूख);

    UVector matchStartNodes(*fStatus);
    लागि (पूर्णाङ्क j=0; j<rule_जरा_Nodes.size(); ++j) {
        RBBINode *गाँठो = static_cast<RBBINode *>(rule_जरा_Nodes.elementAt(j));
        यदि (गाँठो->fChainIn) {
            setAdd(&matchStartNodes, गाँठो->fFirstPosSet);
        }
    }
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }

    int32_t  end_गाँठो_Ix;
    int32_t  start_गाँठो_Ix;

    लागि (end_गाँठो_Ix=0; end_गाँठो_Ix<leafNodes.size(); end_गाँठो_Ix++) {
        RBBINode* end_गाँठो = static_cast<RBBINode*>(leafNodes.elementAt(end_गाँठो_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        यदि (!end_गाँठो->fFollowPos->contains(end_Mark_गाँठो)) {
            जारी;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_गाँठो;
        लागि (start_गाँठो_Ix = 0; start_गाँठो_Ix<matchStartNodes.size(); start_गाँठो_Ix++) {
            start_गाँठो = static_cast<RBBINode*>(matchStartNodes.elementAt(start_गाँठो_Ix));
            यदि (start_गाँठो->fType != RBBINode::leafChar) {
                जारी;
            }

            यदि (end_गाँठो->fVal == start_गाँठो->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_गाँठो->fFollowPos, start_गाँठो->fFollowPos);
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
        फर्काऊ;
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
    RBBINode  *bof_गाँठो = f_रूख->f_बायाँ_Child->f_बायाँ_Child;
    U_ASSERT(bof_गाँठो->fType == RBBINode::leafChar);
    U_ASSERT(bof_गाँठो->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_रूख->f_बायाँ_Child->f_दायाँ_Child->fFirstPosSet;

    RBBINode *start_गाँठो;
    पूर्णाङ्क       start_गाँठो_Ix;
    लागि (start_गाँठो_Ix = 0; start_गाँठो_Ix<matchStartNodes->size(); start_गाँठो_Ix++) {
        start_गाँठो = static_cast<RBBINode*>(matchStartNodes->elementAt(start_गाँठो_Ix));
        यदि (start_गाँठो->fType != RBBINode::leafChar) {
            जारी;
        }

        यदि (start_गाँठो->fVal == bof_गाँठो->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_गाँठो->fFollowPos, start_गाँठो->fFollowPos);
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
        फर्काऊ;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    पूर्णाङ्क      last_आगत_चिह्न = fRB->fSetBuilder->get_अङ्क_Char_Categories() - 1;
    failState = नूतन RBBIStateDescriptor(last_आगत_चिह्न, fStatus);
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
    initialState = नूतन RBBIStateDescriptor(last_आगत_चिह्न, fStatus);
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
    setAdd(initialState->fPositions, f_रूख->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        गच्छ ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    लागि (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        लागि (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *temp;
            temp = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            यदि (temp->fMarked == असत्य) {
                T = temp;
                रोक;
            }
        }
        यदि (T == nullptr) {
            रोक;
        }

        // mark T;
        T->fMarked = सत्य;

        // for each input symbol a do begin
        int32_t  a;
        लागि (a = 1; a<=last_आगत_चिह्न; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            लागि (px=0; px<T->fPositions->size(); px++) {
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
                लागि (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    यदि (setEquals(U, temp2->fPositions)) {
                        delete U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = सत्य;
                        रोक;
                    }
                }

                // Add U as an unmarked state to Dstates
                यदि (!UinDstates)
                {
                    RBBIStateDescriptor *newState = नूतन RBBIStateDescriptor(last_आगत_चिह्न, fStatus);
                    यदि (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    यदि (U_FAILURE(*fStatus)) {
                        गच्छ ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    यदि (U_FAILURE(*fStatus)) {
                        फर्काऊ;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    फर्काऊ;
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
    fLookAheadRuleMap =  नूतन UVector32(fRB->fScanner->अङ्क_Rules() + 1, *fStatus);
    यदि (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    fLookAheadRuleMap->setSize(fRB->fScanner->अङ्क_Rules() + 1);

    लागि (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        तार्किक saw_Look_Ahead_गाँठो = असत्य;
        लागि (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *गाँठो = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            यदि (गाँठो->fType != RBBINode::गाँठो_Type::lookAhead) {
                जारी;
            }
            saw_Look_Ahead_गाँठो = सत्य;
            int32_t rule_अङ्क = गाँठो->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_अङ्क < fLookAheadRuleMap->size());
            U_ASSERT(rule_अङ्क > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_अङ्क);
            यदि (laSlot != 0) {
                यदि (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } अन्यथा {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        यदि (!saw_Look_Ahead_गाँठो) {
            जारी;
        }

        यदि (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        लागि (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *गाँठो = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            यदि (गाँठो->fType != RBBINode::गाँठो_Type::lookAhead) {
                जारी;
            }
            int32_t rule_अङ्क = गाँठो->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = fLookAheadRuleMap->elementAti(rule_अङ्क);
            (शून्य)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            fLookAheadRuleMap->setElementAt(laSlotForState, rule_अङ्क);
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
        फर्काऊ;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }

    f_रूख->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }

    लागि (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        लागि (n=0; n<fDStates->size(); n++) {
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
        फर्काऊ;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_गाँठो;
    int32_t     i;
    int32_t     n;

    f_रूख->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    लागि (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_गाँठो = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_गाँठो->fType == RBBINode::गाँठो_Type::lookAhead);

        लागि (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->indexOf(look_Ahead_गाँठो);
            यदि (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_गाँठो == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_गाँठो->fVal);
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
        फर्काऊ;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_गाँठो;
    int32_t     i;
    int32_t     n;

    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    f_रूख->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    लागि (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_गाँठो = static_cast<RBBINode*>(tagNodes.elementAt(i));

        लागि (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            यदि (sd->fPositions->indexOf(tag_गाँठो) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_गाँठो->fVal);
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
    लागि (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_मानहरू = sd->fTagVals;
        यदि (this_States_Tag_मानहरू == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            जारी;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  nextTagGroupStart = 0;

        // Loop runs once per group of tags in the global list
        जबसम्म (nextTagGroupStart < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = nextTagGroupStart;
            nextTagGroupStart += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            यदि (this_States_Tag_मानहरू->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                जारी;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            लागि (i=0; i<this_States_Tag_मानहरू->size(); i++) {
                यदि (this_States_Tag_मानहरू->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    रोक;
                }
            }

            यदि (i == this_States_Tag_मानहरू->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                रोक;
            }
        }

        यदि (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_मानहरू->size(), *fStatus);
            लागि (i=0; i<this_States_Tag_मानहरू->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_मानहरू->elementAti(i), *fStatus);
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
        फर्काऊ;
    }

#यदि UPRV_HAS_SANITIZER
    यदि (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        फर्काऊ;
    }
#endif
    संरचना StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        तार्किक चालक==(StateType अचर &other) अचर {
            फर्काऊ fAccepting == other.fAccepting &&
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
        TypeToStates(अचर StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](शून्य *p) { delete static_cast<TypeToStates *>(p); });
    लागि (int32_t i = 0; i < fDStates->size(); ++i) {
        अचर RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        अचर StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        लागि (; j < initialPartition.size(); ++j) {
            स्वतः &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            यदि (type_j == type) {
                states->addElement(i, *fStatus);
                रोक;
            }
        }
        यदि (j == initialPartition.size()) {
            स्वतः newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            यदि (U_FAILURE(*fStatus)) {
                फर्काऊ;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    स्वतः partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    partition->setDeleter(
        [](शून्य *p) { delete static_cast<UVector*>(p); });
    
    लागि (int32_t i = 0; i < initialPartition.size(); ++i) {
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
    स्वतः partition_signature = [&partition, एतत्](int32_t stateIndex) -> prv::unique_ptr<UVector> {
        अचर RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(stateIndex));
        स्वतः result = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        यदि (U_FAILURE(*fStatus)) {
            फर्काऊ nullptr;
        }
        लागि (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            लागि (int32_t partIndex = 0; partIndex < partition->size(); ++partIndex) {
                अचर UVector &part = *static_cast<UVector *>(partition->elementAt(partIndex));
                यदि (part.contains(toState)) {
                    result->addElement(partIndex, *fStatus);
                    रोक;
                }
            }
        }
        result->setComparer([](अचर UElement left, अचर UElement दायाँ) -> UBool {
            फर्काऊ left.integer == दायाँ.integer;
        });
        फर्काऊ result;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    लागि (;;) {
        // Π_new.
        स्वतः partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        यदि (U_FAILURE(*fStatus)) {
            फर्काऊ;
        }
        partitionNew->setDeleter(
            [](शून्य *p) { delete static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        तार्किक refined = असत्य;
        लागि (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            अचर UVector &group = *static_cast<UVector*>(partition->elementAt(i));
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
            लागि (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                अचर int32_t s = group.elementAti(j);
                स्वतः signature = partition_signature(s);
                int32_t k = 0;
                लागि (; k < groupRefinement.size(); ++k) {
                    अचर स्वतः &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    यदि (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        रोक;
                    }
                }
                यदि (k == groupRefinement.size()) {
                    अचर स्वतः newEntry =
                        नूतन SignatureToStates(std::move(signature), *fStatus);
                    यदि (U_FAILURE(*fStatus)) {
                        फर्काऊ;
                    }
                    यदि (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      फर्काऊ;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            लागि (int32_t j = 0; j < groupRefinement.size(); ++j) {
                स्वतः &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
            }
        }
        यदि (refined) {
            partition = std::move(partitionNew);
        } अन्यथा {
            रोक;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    लागि (पूर्णाङ्क i = 0; i < partition->size(); ++i) {
        UVector *अचर part_i = static_cast<UVector *>(partition->elementAt(i));
        यदि (part_i->contains(0)) {
            शून्य *अचर part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            अचर स्वतः deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } अन्यथा यदि (part_i->contains(1)) {
            शून्य *अचर part_1 = partition->elementAt(1);
            अचर स्वतः deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    स्वतः oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    लागि (पूर्णाङ्क i = 0; i < partition->size(); ++i) {
        अचर UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        लागि (पूर्णाङ्क j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    स्वतः अचर oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    लागि (पूर्णाङ्क i = 0; i < partition->size(); ++i) {
        अचर UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *अचर state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        लागि (पूर्णाङ्क j = 0; j < state->fDtran->size(); ++j) {
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
        फर्काऊ;
    }
    UVector *vec = *vector;
    int32_t  vSize = vec->size();
    लागि (i=0; i<vSize; i++) {
        int32_t valAtI = vec->elementAti(i);
        यदि (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            फर्काऊ;
        }
        यदि (valAtI > val) {
            रोक;
        }
    }
    vec->घुसाउ_Element_At(val, i, *fStatus);
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
    Maybe_थाक_Array<शून्य *, 16> destArray, sourceArray;  // Handle small cases without malloc
    शून्य **destPtr, **sourcePtr;
    शून्य **destLim, **sourceLim;

    यदि (destOriginalSize > destArray.getCapacity()) {
        यदि (destArray.resize(destOriginalSize) == nullptr) {
            फर्काऊ;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + destOriginalSize;  // destArray.getArrayLimit()?

    यदि (sourceSize > sourceArray.getCapacity()) {
        यदि (sourceArray.resize(sourceSize) == nullptr) {
            फर्काऊ;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + sourceSize;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (शून्य) dest->toArray(destPtr);
    (शून्य) source->toArray(sourcePtr);

    dest->setSize(sourceSize+destOriginalSize, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }

    जबसम्म (sourcePtr < sourceLim && destPtr < destLim) {
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
    जबसम्म (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    जबसम्म (sourcePtr < sourceLim) {
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
    फर्काऊ a->equals(*b);
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
        फर्काऊ;
    }
    म_लिखो("\n");
    RBBINode::print_गाँठो_Header();
    RBBINode::print_गाँठो(n);
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
    int32_t अङ्क_States = fDStates->size();
    int32_t अङ्क_Cols = fRB->fSetBuilder->get_अङ्क_Char_Categories();

    लागि (; categories->first < अङ्क_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        पूर्णाङ्क limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : अङ्क_Cols;
        लागि (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            लागि (int32_t state=0; state<अङ्क_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                यदि (table_base != table_dupl) {
                    रोक;
                }
            }
            यदि (table_base == table_dupl) {
                फर्काऊ सत्य;
            }
        }
    }
    फर्काऊ असत्य;
}


//
//    removeColumn()
//
शून्य RBBITableBuilder::removeColumn(int32_t column) {
    int32_t अङ्क_States = fDStates->size();
    लागि (int32_t state=0; state<अङ्क_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


तार्किक RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t अङ्क_States = fSafeTable->size();

    लागि (; states->first<अङ्क_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        लागि (states->second=states->first+1; states->second<अङ्क_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            तार्किक rowsMatch = सत्य;
            int32_t अङ्क_Cols = firstRow->length();
            लागि (int32_t col=0; col < अङ्क_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                यदि (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = असत्य;
                    रोक;
                }
            }
            यदि (rowsMatch) {
                फर्काऊ सत्य;
            }
        }
    }
    फर्काऊ असत्य;
}

शून्य RBBITableBuilder::removeSafeState(IntPair duplStates) {
    अचर int32_t keepState = duplStates.first;
    अचर int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t अङ्क_States = fSafeTable->size();
    लागि (int32_t state=0; state<अङ्क_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t अङ्क_Cols = sd->length();
        लागि (int32_t col=0; col<अङ्क_Cols; col++) {
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
    अचर int32_t old_State_गन्ती = fDStates->size();
    minimizeStates();
    फर्काऊ fDStates->size() - old_State_गन्ती;
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
        फर्काऊ;
    }
    अचर int32_t lookahead_गन्ती = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    अचर int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    अचर स्वतः stateDescriptor = [एतत्](अचर int32_t state) -> RBBIStateDescriptor & {
        फर्काऊ *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    अचर स्वतः lookaheadReachability =
        prv::make_unique<तार्किक[]>(lookahead_गन्ती * lookahead_गन्ती, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    लागि (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        लागि (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
            यदि (k != l) {
                लागि (int32_t source = 1; source < fDStates->size(); ++source) {
                    यदि (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        जारी;
                    }
                    यदि (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](अचर int32_t state) {
                                फर्काऊ static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](अचर int32_t state) {
                                फर्काऊ static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_गन्ती + k -
                                              firstLookahead] = सत्य;
                    }
                }
            }
        }
    }

    अचर स्वतः colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_गन्ती, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    लागि (int32_t chromatic_सङ्ख्या = 1;; ++chromatic_सङ्ख्या) {
        uprv_memset(colours.get(), 0, परिमाण(int32_t) * lookahead_गन्ती);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        लागि (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            लागि (पूर्णाङ्क source = 0; source < lookahead_गन्ती; ++source) {
                लागि (पूर्णाङ्क sink = 0; sink < lookahead_गन्ती; ++sink) {
                    यदि (lookaheadReachability[source * lookahead_गन्ती + sink] &&
                        colours[source] == colours[sink]) {
                        गच्छ nextColouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            लागि (पूर्णाङ्क i = 0; i < fDStates->size(); ++i) {
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
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_सङ्ख्या;
            फर्काऊ;

          nextColouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            लागि (int32_t i = 0; i < lookahead_गन्ती - 1 && colours[i] == chromatic_सङ्ख्या;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            यदि (colours[lookahead_गन्ती - 1] == chromatic_सङ्ख्या) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                रोक;
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
int32_t  RBBITableBuilder::getTableSize() अचर {
    int32_t    size = 0;
    int32_t    अङ्क_Rows;
    int32_t    अङ्क_Cols;
    int32_t    rowSize;

    यदि (f_रूख == nullptr) {
        फर्काऊ 0;
    }

    size    = दुरत्व(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    अङ्क_Rows = fDStates->size();
    अङ्क_Cols = fRB->fSetBuilder->get_अङ्क_Char_Categories();

    यदि (use8BitsForTable()) {
        rowSize = दुरत्व(RBBIStateTableRow8, fNextState) + परिमाण(int8_t)*अङ्क_Cols;
    } अन्यथा {
        rowSize = दुरत्व(RBBIStateTableRow16, fNextState) + परिमाण(int16_t)*अङ्क_Cols;
    }
    size   += अङ्क_Rows * rowSize;
    फर्काऊ size;
}

तार्किक RBBITableBuilder::use8BitsForTable() अचर {
    फर्काऊ fDStates->size() <= kMaxStateFor8BitsTable;
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

    यदि (U_FAILURE(*fStatus) || f_रूख == nullptr) {
        फर्काऊ;
    }

    int32_t cat_गन्ती = fRB->fSetBuilder->get_अङ्क_Char_Categories();
    यदि (cat_गन्ती > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        फर्काऊ;
    }

    table->f_अङ्क_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->fLookAheadResultsSize = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    यदि (use8BitsForTable()) {
        table->fRowLen    = दुरत्व(RBBIStateTableRow8, fNextState) + परिमाण(uint8_t) * cat_गन्ती;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } अन्यथा {
        table->fRowLen    = दुरत्व(RBBIStateTableRow16, fNextState) + परिमाण(int16_t) * cat_गन्ती;
    }
    यदि (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    लागि (state=0; state<table->f_अङ्क_States; state++) {
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
            लागि (col=0; col<cat_गन्ती; col++) {
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
            लागि (col=0; col<cat_गन्ती; col++) {
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

    int32_t अङ्क_Char_Classes = fRB->fSetBuilder->get_अङ्क_Char_Categories();
    int32_t अङ्क_States = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    अचर स्वतः lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    अचर स्वतः wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    लागि (int32_t c1 = 1; c1 < अङ्क_Char_Classes; ++c1) {
        यदि (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          जारी;
        }
        लागि (int32_t c2 = 1; c2 < अङ्क_Char_Classes; ++c2) {
            यदि (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                जारी;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            तार्किक mustCheckConsistentWithStartC2 = असत्य;
            अचर int32_t text[2] = {c1, c2};
            लागि (int32_t injectedState = START_STATE; injectedState < अङ्क_States; ++injectedState) {
                लागि (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    यदि (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        अचर RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        यदि (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
                        } अन्यथा यदि (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            जारी;
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
                    लागि (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        लागि (;;) {
                        dfaStep:
                            यदि (i == BEFORE_PAIR || i == 2) {
                                गच्छ inspectFinalState;
                            }
                            अचर int32_t चिह्न_Ahead = text[i];
                            ++i;
                            अचर RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            यदि (sDescriptor->fDtran->elementAti(चिह्न_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(चिह्न_Ahead);
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
                                    गच्छ next_चिह्न_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                रोक;
                            }

                            यदि (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } अन्यथा यदि (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
                                रोक;
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
                        जारी;
                    }
                    यदि (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        लागि (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
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
                            गच्छ next_चिह्न_Pair;
                        }
                        यदि (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            गच्छ next_चिह्न_Pair;
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
                    लागि (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        यदि (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            जारी;
                        }
                        यदि (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            यदि (wantedEndState != START_STATE) {
                                गच्छ next_चिह्न_Pair;
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
                अचर RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                अचर स्वतः c2State = startDescriptor.fDtran->elementAti(c2);
                यदि (c2State != wantedEndState) {
                    गच्छ next_चिह्न_Pair;
                }
                अचर RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                यदि (wantedLastAcceptingPosition != 2) {
                    गच्छ next_चिह्न_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                लागि (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    यदि (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        गच्छ next_चिह्न_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        next_चिह्न_Pair:
            जारी;
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
        नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, अङ्क_Char_Classes + 2, status), status);
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    fSafeTable = lpSafeTable.orphan();
    लागि (int32_t row=0; row<अङ्क_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(नूतन UnicodeString(अङ्क_Char_Classes, 0, अङ्क_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    लागि (int32_t charClass=0; charClass < अङ्क_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    लागि (int32_t row=2; row<अङ्क_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    लागि (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    जबसम्म (findDuplicateSafeState(&states)) {
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
int32_t  RBBITableBuilder::getSafeTableSize() अचर {
    int32_t    size = 0;
    int32_t    अङ्क_Rows;
    int32_t    अङ्क_Cols;
    int32_t    rowSize;

    यदि (fSafeTable == nullptr) {
        फर्काऊ 0;
    }

    size    = दुरत्व(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    अङ्क_Rows = fSafeTable->size();
    अङ्क_Cols = fRB->fSetBuilder->get_अङ्क_Char_Categories();

    यदि (use8BitsForSafeTable()) {
        rowSize = दुरत्व(RBBIStateTableRow8, fNextState) + परिमाण(int8_t)*अङ्क_Cols;
    } अन्यथा {
        rowSize = दुरत्व(RBBIStateTableRow16, fNextState) + परिमाण(int16_t)*अङ्क_Cols;
    }
    size   += अङ्क_Rows * rowSize;
    फर्काऊ size;
}

तार्किक RBBITableBuilder::use8BitsForSafeTable() अचर {
    फर्काऊ fSafeTable->size() <= kMaxStateFor8BitsTable;
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
        फर्काऊ;
    }

    int32_t cat_गन्ती = fRB->fSetBuilder->get_अङ्क_Char_Categories();
    यदि (cat_गन्ती > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        फर्काऊ;
    }

    table->f_अङ्क_States = fSafeTable->size();
    table->fFlags     = 0;
    यदि (use8BitsForSafeTable()) {
        table->fRowLen    = दुरत्व(RBBIStateTableRow8, fNextState) + परिमाण(uint8_t) * cat_गन्ती;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } अन्यथा {
        table->fRowLen    = दुरत्व(RBBIStateTableRow16, fNextState) + परिमाण(int16_t) * cat_गन्ती;
    }

    लागि (state=0; state<table->f_अङ्क_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        यदि (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            लागि (col=0; col<cat_गन्ती; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } अन्यथा {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            लागि (col=0; col<cat_गन्ती; col++) {
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
    लागि (i=0; i<s->size(); i++) {
        अचर RBBINode *v = static_cast<अचर RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==nullptr? -1 : v->f_Serial_अङ्क);
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
    लागि (c=0; c<fRB->fSetBuilder->get_अङ्क_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    लागि (c=0; c<fRB->fSetBuilder->get_अङ्क_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    लागि (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        लागि (c=0; c<fRB->fSetBuilder->get_अङ्क_Char_Categories(); c++) {
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
        फर्काऊ;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    लागि (c=0; c<fRB->fSetBuilder->get_अङ्क_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    लागि (c=0; c<fRB->fSetBuilder->get_अङ्क_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    लागि (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        लागि (c=0; c<fRB->fSetBuilder->get_अङ्क_Char_Categories(); c++) {
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

    जबसम्म (nextRecord < tbl->size()) {
        thisRecord = nextRecord;
        nextRecord = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        लागि (i=thisRecord+1; i<nextRecord; i++) {
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

RBBIStateDescriptor::RBBIStateDescriptor(पूर्णाङ्क last_आगत_चिह्न, UErrorCode *fStatus) {
    fMarked    = असत्य;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = नूतन UVector32(last_आगत_चिह्न+1, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }
    यदि (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    fDtran->setSize(last_आगत_चिह्न+1);    // fDtran needs to be pre-sized.
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
