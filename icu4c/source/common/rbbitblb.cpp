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

#അന്തര്ഭാവ <functional>

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "rbbitblb.h"
#അന്തര്ഭാവ "rbbirb.h"
#അന്തര്ഭാവ "rbbiscan.h"
#അന്തര്ഭാവ "rbbisetb.h"
#അന്തര്ഭാവ "rbbidata.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "uvectr32.h"
#അന്തര്ഭാവ "cmemory.h"

U_NAMESPACE_BEGIN

നാമാകാശ {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
തർക്കം reachableByTransitions(സ്ഥിരം UVector &states, സ്ഥിരം int32_t source,
                            സ്ഥിരം std::function<തർക്കം(int32_t)> isSink,
                            സ്ഥിരം std::function<തർക്കം(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        വേണ്ടി (int32_t ചിഹ്നം = 0; ചിഹ്നം < transitionsFromSource.size(); ++ചിഹ്നം) {
            സ്ഥിരം int32_t state = transitionsFromSource.elementAti(ചിഹ്നം);
            എങ്കിൽ (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    സ്ഥിരം auto visited = prv::make_unique<തർക്കം[]>(states.size(), status);
    ആകുമ്പോൾ (U_SUCCESS(status) && !boundary.empty()) {
        സ്ഥിരം int32_t s = boundary.popi();
        എങ്കിൽ (isSink(s)) {
            തിരികെ സത്യം;
        }
        എങ്കിൽ (excludedState(s) || visited[s]) {
            തുടരുക;
        }
        visited[s] = സത്യം;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        വേണ്ടി (int32_t ചിഹ്നം = 0; ചിഹ്നം < transitions.size(); ++ചിഹ്നം) {
            സ്ഥിരം int32_t t = transitions.elementAti(ചിഹ്നം);
            എങ്കിൽ (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    തിരികെ അസത്യം;
}

} // namespace

സ്ഥിരം int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **വേര്_കണ്ണി, UErrorCode &status) :
        fRB(rb),
        f_വൃക്ഷം(*വേര്_കണ്ണി),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = പുതിയ UVector(status);
    എങ്കിൽ (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി (i=0; i<fDStates->size(); i++) {
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
ശൂന്യം  RBBITableBuilder::buildForwardTable() {

    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    എങ്കിൽ (f_വൃക്ഷം==nullptr) {
        തിരികെ;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_വൃക്ഷം = f_വൃക്ഷം->flattenVariables(*fStatus, 0);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
#ifdef RBBI_DEBUG
    എങ്കിൽ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_വൃക്ഷം(f_വൃക്ഷം, സത്യം);
    }
#endif

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = പുതിയ RBBINode(RBBINode::opCat, *fStatus);
    // Exit if memory allocation failed.
    എങ്കിൽ (cn == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        delete cn;
        തിരികെ;
    }
    cn->f_ഇടത്_Child = f_വൃക്ഷം;
    f_വൃക്ഷം->fParent = cn;
    RBBINode *end_Marker_കണ്ണി = cn->f_വലത്_Child = പുതിയ RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    എങ്കിൽ (cn->f_വലത്_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        delete cn;
        തിരികെ;
    }
    cn->f_വലത്_Child->fParent = cn;
    f_വൃക്ഷം = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_വൃക്ഷം->flattenSets(*fStatus, 0);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
#ifdef RBBI_DEBUG
    എങ്കിൽ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_വൃക്ഷം(f_വൃക്ഷം, സത്യം);
    }
#endif


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_വൃക്ഷം);
    calcFirstPos(f_വൃക്ഷം);
    calcLastPos(f_വൃക്ഷം);
    calcFollowPos(f_വൃക്ഷം);
    എങ്കിൽ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_വൃക്ഷം);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    എങ്കിൽ (fRB->fChainRules) {
        calcChainedFollowPos(f_വൃക്ഷം, end_Marker_കണ്ണി);
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
ശൂന്യം RBBITableBuilder::calcNullable(RBBINode *n) {
    എങ്കിൽ (n == nullptr) {
        തിരികെ;
    }
    എങ്കിൽ (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = അസത്യം;
        തിരികെ;
    }

    എങ്കിൽ (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = സത്യം;
        തിരികെ;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_ഇടത്_Child);
    calcNullable(n->f_വലത്_Child);

    // Apply functions from table 3.40 in Aho
    എങ്കിൽ (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_ഇടത്_Child->fNullable || n->f_വലത്_Child->fNullable;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_ഇടത്_Child->fNullable && n->f_വലത്_Child->fNullable;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n->fType == RBBINode::opStar || n->fType == RBBINode::opQuestion) {
        n->fNullable = സത്യം;
    }
    അല്ലെങ്കിൽ {
        n->fNullable = അസത്യം;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBITableBuilder::calcFirstPos(RBBINode *n) {
    എങ്കിൽ (n == nullptr) {
        തിരികെ;
    }
    എങ്കിൽ (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fFirstPosSet->addElement(n, *fStatus);
        തിരികെ;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_ഇടത്_Child);
    calcFirstPos(n->f_വലത്_Child);

    // Apply functions from table 3.40 in Aho
    എങ്കിൽ (n->fType == RBBINode::opOr) {
        setAdd(n->fFirstPosSet, n->f_ഇടത്_Child->fFirstPosSet);
        setAdd(n->fFirstPosSet, n->f_വലത്_Child->fFirstPosSet);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n->fType == RBBINode::opCat) {
        setAdd(n->fFirstPosSet, n->f_ഇടത്_Child->fFirstPosSet);
        എങ്കിൽ (n->f_ഇടത്_Child->fNullable) {
            setAdd(n->fFirstPosSet, n->f_വലത്_Child->fFirstPosSet);
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n->fType == RBBINode::opStar ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fFirstPosSet, n->f_ഇടത്_Child->fFirstPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBITableBuilder::calcLastPos(RBBINode *n) {
    എങ്കിൽ (n == nullptr) {
        തിരികെ;
    }
    എങ്കിൽ (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fLastPosSet->addElement(n, *fStatus);
        തിരികെ;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calcLastPos(n->f_ഇടത്_Child);
    calcLastPos(n->f_വലത്_Child);

    // Apply functions from table 3.40 in Aho
    എങ്കിൽ (n->fType == RBBINode::opOr) {
        setAdd(n->fLastPosSet, n->f_ഇടത്_Child->fLastPosSet);
        setAdd(n->fLastPosSet, n->f_വലത്_Child->fLastPosSet);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n->fType == RBBINode::opCat) {
        setAdd(n->fLastPosSet, n->f_വലത്_Child->fLastPosSet);
        എങ്കിൽ (n->f_വലത്_Child->fNullable) {
            setAdd(n->fLastPosSet, n->f_ഇടത്_Child->fLastPosSet);
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n->fType == RBBINode::opStar     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fLastPosSet, n->f_ഇടത്_Child->fLastPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBITableBuilder::calcFollowPos(RBBINode *n) {
    എങ്കിൽ (n == nullptr ||
        n->fType == RBBINode::leafChar ||
        n->fType == RBBINode::endMark) {
        തിരികെ;
    }

    calcFollowPos(n->f_ഇടത്_Child);
    calcFollowPos(n->f_വലത്_Child);

    // Aho rule #1
    എങ്കിൽ (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_ഇടത്_Child = n->f_ഇടത്_Child->fLastPosSet;

        വേണ്ടി (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_ഇടത്_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_ഇടത്_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_വലത്_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    എങ്കിൽ (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        വേണ്ടി (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
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
ശൂന്യം RBBITableBuilder::add_Rule_വേര്_Nodes(UVector *dest, RBBINode *കണ്ണി) {
    എങ്കിൽ (കണ്ണി == nullptr || U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    U_ASSERT(!dest->hasDeleter());
    എങ്കിൽ (കണ്ണി->f_Rule_വേര്) {
        dest->addElement(കണ്ണി, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        തിരികെ;
    }
    add_Rule_വേര്_Nodes(dest, കണ്ണി->f_ഇടത്_Child);
    add_Rule_വേര്_Nodes(dest, കണ്ണി->f_വലത്_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBITableBuilder::calcChainedFollowPos(RBBINode *വൃക്ഷം, RBBINode *end_Mark_കണ്ണി) {

    UVector         leafNodes(*fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }

    // get a list all leaf nodes
    വൃക്ഷം->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_വേര്_Nodes(*fStatus);
    add_Rule_വേര്_Nodes(&rule_വേര്_Nodes, വൃക്ഷം);

    UVector matchStartNodes(*fStatus);
    വേണ്ടി (പൂർണ്ണസംഖ്യ j=0; j<rule_വേര്_Nodes.size(); ++j) {
        RBBINode *കണ്ണി = static_cast<RBBINode *>(rule_വേര്_Nodes.elementAt(j));
        എങ്കിൽ (കണ്ണി->fChainIn) {
            setAdd(&matchStartNodes, കണ്ണി->fFirstPosSet);
        }
    }
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }

    int32_t  end_കണ്ണി_Ix;
    int32_t  start_കണ്ണി_Ix;

    വേണ്ടി (end_കണ്ണി_Ix=0; end_കണ്ണി_Ix<leafNodes.size(); end_കണ്ണി_Ix++) {
        RBBINode* end_കണ്ണി = static_cast<RBBINode*>(leafNodes.elementAt(end_കണ്ണി_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        എങ്കിൽ (!end_കണ്ണി->fFollowPos->contains(end_Mark_കണ്ണി)) {
            തുടരുക;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_കണ്ണി;
        വേണ്ടി (start_കണ്ണി_Ix = 0; start_കണ്ണി_Ix<matchStartNodes.size(); start_കണ്ണി_Ix++) {
            start_കണ്ണി = static_cast<RBBINode*>(matchStartNodes.elementAt(start_കണ്ണി_Ix));
            എങ്കിൽ (start_കണ്ണി->fType != RBBINode::leafChar) {
                തുടരുക;
            }

            എങ്കിൽ (end_കണ്ണി->fVal == start_കണ്ണി->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_കണ്ണി->fFollowPos, start_കണ്ണി->fFollowPos);
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
ശൂന്യം RBBITableBuilder::bofFixup() {

    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
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
    RBBINode  *bof_കണ്ണി = f_വൃക്ഷം->f_ഇടത്_Child->f_ഇടത്_Child;
    U_ASSERT(bof_കണ്ണി->fType == RBBINode::leafChar);
    U_ASSERT(bof_കണ്ണി->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_വൃക്ഷം->f_ഇടത്_Child->f_വലത്_Child->fFirstPosSet;

    RBBINode *start_കണ്ണി;
    പൂർണ്ണസംഖ്യ       start_കണ്ണി_Ix;
    വേണ്ടി (start_കണ്ണി_Ix = 0; start_കണ്ണി_Ix<matchStartNodes->size(); start_കണ്ണി_Ix++) {
        start_കണ്ണി = static_cast<RBBINode*>(matchStartNodes->elementAt(start_കണ്ണി_Ix));
        എങ്കിൽ (start_കണ്ണി->fType != RBBINode::leafChar) {
            തുടരുക;
        }

        എങ്കിൽ (start_കണ്ണി->fVal == bof_കണ്ണി->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_കണ്ണി->fFollowPos, start_കണ്ണി->fFollowPos);
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
ശൂന്യം RBBITableBuilder::buildStateTable() {
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    പൂർണ്ണസംഖ്യ      last_നിവേശം_ചിഹ്നം = fRB->fSetBuilder->get_അക്കം_Char_Categories() - 1;
    failState = പുതിയ RBBIStateDescriptor(last_നിവേശം_ചിഹ്നം, fStatus);
    എങ്കിൽ (failState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        പോകുക ExitBuildSTdeleteall;
    }
    failState->fPositions = പുതിയ UVector(*fStatus);
    എങ്കിൽ (failState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ (failState->fPositions == nullptr || U_FAILURE(*fStatus)) {
        പോകുക ExitBuildSTdeleteall;
    }
    fDStates->addElement(failState, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        പോകുക ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = പുതിയ RBBIStateDescriptor(last_നിവേശം_ചിഹ്നം, fStatus);
    എങ്കിൽ (initialState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        പോകുക ExitBuildSTdeleteall;
    }
    initialState->fPositions = പുതിയ UVector(*fStatus);
    എങ്കിൽ (initialState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        പോകുക ExitBuildSTdeleteall;
    }
    setAdd(initialState->fPositions, f_വൃക്ഷം->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        പോകുക ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    വേണ്ടി (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        വേണ്ടി (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *temp;
            temp = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            എങ്കിൽ (temp->fMarked == അസത്യം) {
                T = temp;
                നിർത്തുക;
            }
        }
        എങ്കിൽ (T == nullptr) {
            നിർത്തുക;
        }

        // mark T;
        T->fMarked = സത്യം;

        // for each input symbol a do begin
        int32_t  a;
        വേണ്ടി (a = 1; a<=last_നിവേശം_ചിഹ്നം; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            വേണ്ടി (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                എങ്കിൽ ((p->fType == RBBINode::leafChar) &&  (p->fVal == a)) {
                    എങ്കിൽ (U == nullptr) {
                        U = പുതിയ UVector(*fStatus);
                        എങ്കിൽ (U == nullptr) {
                        	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                        	പോകുക ExitBuildSTdeleteall;
                        }
                    }
                    setAdd(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = അസത്യം;
            എങ്കിൽ (U != nullptr) {
                U_ASSERT(U->size() > 0);
                പൂർണ്ണസംഖ്യ  ix;
                വേണ്ടി (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    എങ്കിൽ (setEquals(U, temp2->fPositions)) {
                        delete U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = സത്യം;
                        നിർത്തുക;
                    }
                }

                // Add U as an unmarked state to Dstates
                എങ്കിൽ (!UinDstates)
                {
                    RBBIStateDescriptor *newState = പുതിയ RBBIStateDescriptor(last_നിവേശം_ചിഹ്നം, fStatus);
                    എങ്കിൽ (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    എങ്കിൽ (U_FAILURE(*fStatus)) {
                        പോകുക ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    എങ്കിൽ (U_FAILURE(*fStatus)) {
                        തിരികെ;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    തിരികെ;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    delete initialState;
    delete failState;
}


/**
 * mapLookAheadRules
 *
 */
ശൂന്യം RBBITableBuilder::mapLookAheadRules() {
    fLookAheadRuleMap =  പുതിയ UVector32(fRB->fScanner->അക്കം_Rules() + 1, *fStatus);
    എങ്കിൽ (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    fLookAheadRuleMap->setSize(fRB->fScanner->അക്കം_Rules() + 1);

    വേണ്ടി (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        തർക്കം saw_Look_Ahead_കണ്ണി = അസത്യം;
        വേണ്ടി (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *കണ്ണി = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            എങ്കിൽ (കണ്ണി->fType != RBBINode::കണ്ണി_Type::lookAhead) {
                തുടരുക;
            }
            saw_Look_Ahead_കണ്ണി = സത്യം;
            int32_t rule_അക്കം = കണ്ണി->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_അക്കം < fLookAheadRuleMap->size());
            U_ASSERT(rule_അക്കം > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_അക്കം);
            എങ്കിൽ (laSlot != 0) {
                എങ്കിൽ (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } അല്ലെങ്കിൽ {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        എങ്കിൽ (!saw_Look_Ahead_കണ്ണി) {
            തുടരുക;
        }

        എങ്കിൽ (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        വേണ്ടി (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *കണ്ണി = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            എങ്കിൽ (കണ്ണി->fType != RBBINode::കണ്ണി_Type::lookAhead) {
                തുടരുക;
            }
            int32_t rule_അക്കം = കണ്ണി->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = fLookAheadRuleMap->elementAti(rule_അക്കം);
            (ശൂന്യം)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            fLookAheadRuleMap->setElementAt(laSlotForState, rule_അക്കം);
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
ശൂന്യം     RBBITableBuilder::flagAcceptingStates() {
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }

    f_വൃക്ഷം->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }

    വേണ്ടി (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        വേണ്ടി (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            എങ്കിൽ (sd->fPositions->indexOf(endMarker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                എങ്കിൽ (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
                    എങ്കിൽ (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                എങ്കിൽ (sd->fAccepting==ACCEPTING_UNCONDITIONAL && endMarker->fVal != 0) {
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
ശൂന്യം     RBBITableBuilder::flagLookAheadStates() {
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_കണ്ണി;
    int32_t     i;
    int32_t     n;

    f_വൃക്ഷം->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    വേണ്ടി (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_കണ്ണി = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_കണ്ണി->fType == RBBINode::കണ്ണി_Type::lookAhead);

        വേണ്ടി (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->indexOf(look_Ahead_കണ്ണി);
            എങ്കിൽ (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_കണ്ണി == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_കണ്ണി->fVal);
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
ശൂന്യം     RBBITableBuilder::flagTaggedStates() {
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_കണ്ണി;
    int32_t     i;
    int32_t     n;

    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    f_വൃക്ഷം->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    വേണ്ടി (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_കണ്ണി = static_cast<RBBINode*>(tagNodes.elementAt(i));

        വേണ്ടി (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            എങ്കിൽ (sd->fPositions->indexOf(tag_കണ്ണി) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_കണ്ണി->fVal);
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
ശൂന്യം  RBBITableBuilder::mergeRuleStatusVals() {
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
    പൂർണ്ണസംഖ്യ i;
    പൂർണ്ണസംഖ്യ n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    എങ്കിൽ (fRB->fRuleStatusVals->size() == 0) {
        fRB->fRuleStatusVals->addElement(1, *fStatus);  // Num of statuses in group
        fRB->fRuleStatusVals->addElement(static_cast<int32_t>(0), *fStatus); // and our single status of zero
    }

    //    For each state
    വേണ്ടി (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_മൂല്യങ്ങൾ = sd->fTagVals;
        എങ്കിൽ (this_States_Tag_മൂല്യങ്ങൾ == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            തുടരുക;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  nextTagGroupStart = 0;

        // Loop runs once per group of tags in the global list
        ആകുമ്പോൾ (nextTagGroupStart < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = nextTagGroupStart;
            nextTagGroupStart += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            എങ്കിൽ (this_States_Tag_മൂല്യങ്ങൾ->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                തുടരുക;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            വേണ്ടി (i=0; i<this_States_Tag_മൂല്യങ്ങൾ->size(); i++) {
                എങ്കിൽ (this_States_Tag_മൂല്യങ്ങൾ->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    നിർത്തുക;
                }
            }

            എങ്കിൽ (i == this_States_Tag_മൂല്യങ്ങൾ->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                നിർത്തുക;
            }
        }

        എങ്കിൽ (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_മൂല്യങ്ങൾ->size(), *fStatus);
            വേണ്ടി (i=0; i<this_States_Tag_മൂല്യങ്ങൾ->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_മൂല്യങ്ങൾ->elementAti(i), *fStatus);
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

ശൂന്യം RBBITableBuilder::minimizeStates() {
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }

#എങ്കിൽ UPRV_HAS_SANITIZER
    എങ്കിൽ (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        തിരികെ;
    }
#endif
    ഘടന StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        തർക്കം operator==(StateType സ്ഥിരം &other) സ്ഥിരം {
            തിരികെ fAccepting == other.fAccepting &&
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
    ഘടന TypeToStates : UMemory {
        TypeToStates(സ്ഥിരം StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](ശൂന്യം *p) { delete static_cast<TypeToStates *>(p); });
    വേണ്ടി (int32_t i = 0; i < fDStates->size(); ++i) {
        സ്ഥിരം RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        സ്ഥിരം StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        വേണ്ടി (; j < initialPartition.size(); ++j) {
            auto &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            എങ്കിൽ (type_j == type) {
                states->addElement(i, *fStatus);
                നിർത്തുക;
            }
        }
        എങ്കിൽ (j == initialPartition.size()) {
            auto newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            എങ്കിൽ (U_FAILURE(*fStatus)) {
                തിരികെ;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    auto partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    partition->setDeleter(
        [](ശൂന്യം *p) { delete static_cast<UVector*>(p); });
    
    വേണ്ടി (int32_t i = 0; i < initialPartition.size(); ++i) {
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
    auto partition_signature = [&partition, ഇത്](int32_t stateIndex) -> prv::unique_ptr<UVector> {
        സ്ഥിരം RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(stateIndex));
        auto result = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        എങ്കിൽ (U_FAILURE(*fStatus)) {
            തിരികെ nullptr;
        }
        വേണ്ടി (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            വേണ്ടി (int32_t partIndex = 0; partIndex < partition->size(); ++partIndex) {
                സ്ഥിരം UVector &part = *static_cast<UVector *>(partition->elementAt(partIndex));
                എങ്കിൽ (part.contains(toState)) {
                    result->addElement(partIndex, *fStatus);
                    നിർത്തുക;
                }
            }
        }
        result->setComparer([](സ്ഥിരം UElement left, സ്ഥിരം UElement വലത്) -> UBool {
            തിരികെ left.integer == വലത്.integer;
        });
        തിരികെ result;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    വേണ്ടി (;;) {
        // Π_new.
        auto partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        എങ്കിൽ (U_FAILURE(*fStatus)) {
            തിരികെ;
        }
        partitionNew->setDeleter(
            [](ശൂന്യം *p) { delete static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        തർക്കം refined = അസത്യം;
        വേണ്ടി (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            സ്ഥിരം UVector &group = *static_cast<UVector*>(partition->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            ഘടന SignatureToStates : UMemory {
                SignatureToStates(prv::unique_ptr<UVector> signature, UErrorCode &status)
                    : signature(std::move(signature)), states(prv::make_unique<UVector>(status, status)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> states;
            };
            UVector groupRefinement(*fStatus);
            groupRefinement.setDeleter([](ശൂന്യം *p) { delete static_cast<SignatureToStates*>(p); });
            വേണ്ടി (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                സ്ഥിരം int32_t s = group.elementAti(j);
                auto signature = partition_signature(s);
                int32_t k = 0;
                വേണ്ടി (; k < groupRefinement.size(); ++k) {
                    സ്ഥിരം auto &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    എങ്കിൽ (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        നിർത്തുക;
                    }
                }
                എങ്കിൽ (k == groupRefinement.size()) {
                    സ്ഥിരം auto newEntry =
                        പുതിയ SignatureToStates(std::move(signature), *fStatus);
                    എങ്കിൽ (U_FAILURE(*fStatus)) {
                        തിരികെ;
                    }
                    എങ്കിൽ (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      തിരികെ;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            വേണ്ടി (int32_t j = 0; j < groupRefinement.size(); ++j) {
                auto &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
            }
        }
        എങ്കിൽ (refined) {
            partition = std::move(partitionNew);
        } അല്ലെങ്കിൽ {
            നിർത്തുക;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < partition->size(); ++i) {
        UVector *സ്ഥിരം part_i = static_cast<UVector *>(partition->elementAt(i));
        എങ്കിൽ (part_i->contains(0)) {
            ശൂന്യം *സ്ഥിരം part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            സ്ഥിരം auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } അല്ലെങ്കിൽ എങ്കിൽ (part_i->contains(1)) {
            ശൂന്യം *സ്ഥിരം part_1 = partition->elementAt(1);
            സ്ഥിരം auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    auto oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < partition->size(); ++i) {
        സ്ഥിരം UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    auto സ്ഥിരം oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < partition->size(); ++i) {
        സ്ഥിരം UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *സ്ഥിരം state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->setElementAt(oldStateToPart[state->fDtran->elementAti(j)], j);
        }
        oldStates->setElementAt(nullptr, part.elementAti(0));
    }
    oldStates->setDeleter([](ശൂന്യം *p) { delete static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBITableBuilder::sortedAdd(UVector **vector, int32_t val) {
    int32_t i;

    എങ്കിൽ (*vector == nullptr) {
        *vector = പുതിയ UVector(*fStatus);
    }
    എങ്കിൽ (*vector == nullptr || U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    UVector *vec = *vector;
    int32_t  vSize = vec->size();
    വേണ്ടി (i=0; i<vSize; i++) {
        int32_t valAtI = vec->elementAti(i);
        എങ്കിൽ (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            തിരികെ;
        }
        എങ്കിൽ (valAtI > val) {
            നിർത്തുക;
        }
    }
    vec->തിരുകുക_Element_At(val, i, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBITableBuilder::setAdd(UVector *dest, UVector *source) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!source->hasDeleter());
    int32_t destOriginalSize = dest->size();
    int32_t sourceSize       = source->size();
    int32_t di           = 0;
    Maybe_അട്ടി_Array<ശൂന്യം *, 16> destArray, sourceArray;  // Handle small cases without malloc
    ശൂന്യം **destPtr, **sourcePtr;
    ശൂന്യം **destLim, **sourceLim;

    എങ്കിൽ (destOriginalSize > destArray.getCapacity()) {
        എങ്കിൽ (destArray.resize(destOriginalSize) == nullptr) {
            തിരികെ;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + destOriginalSize;  // destArray.getArrayLimit()?

    എങ്കിൽ (sourceSize > sourceArray.getCapacity()) {
        എങ്കിൽ (sourceArray.resize(sourceSize) == nullptr) {
            തിരികെ;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + sourceSize;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (ശൂന്യം) dest->toArray(destPtr);
    (ശൂന്യം) source->toArray(sourcePtr);

    dest->setSize(sourceSize+destOriginalSize, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }

    ആകുമ്പോൾ (sourcePtr < sourceLim && destPtr < destLim) {
        എങ്കിൽ (*destPtr == *sourcePtr) {
            dest->setElementAt(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        അല്ലെങ്കിൽ എങ്കിൽ (uprv_memcmp(destPtr, sourcePtr, വലുപ്പം(ശൂന്യം *)) < 0) {
            dest->setElementAt(*destPtr++, di++);
        }
        അല്ലെങ്കിൽ { /* *sourcePtr < *destPtr */
            dest->setElementAt(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    ആകുമ്പോൾ (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    ആകുമ്പോൾ (sourcePtr < sourceLim) {
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
    തിരികെ a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ശൂന്യം RBBITableBuilder::printPosSets(RBBINode *n) {
    എങ്കിൽ (n==nullptr) {
        തിരികെ;
    }
    printf("\n");
    RBBINode::print_കണ്ണി_Header();
    RBBINode::print_കണ്ണി(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    printSet(n->fFirstPosSet);

    RBBIDebugPrintf("         lastpos:   ");
    printSet(n->fLastPosSet);

    RBBIDebugPrintf("         followpos: ");
    printSet(n->fFollowPos);

    printPosSets(n->f_ഇടത്_Child);
    printPosSets(n->f_വലത്_Child);
}
#endif

//
//    findDuplCharClassFrom()
//
തർക്കം RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t അക്കം_States = fDStates->size();
    int32_t അക്കം_Cols = fRB->fSetBuilder->get_അക്കം_Char_Categories();

    വേണ്ടി (; categories->first < അക്കം_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        പൂർണ്ണസംഖ്യ limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : അക്കം_Cols;
        വേണ്ടി (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            വേണ്ടി (int32_t state=0; state<അക്കം_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                എങ്കിൽ (table_base != table_dupl) {
                    നിർത്തുക;
                }
            }
            എങ്കിൽ (table_base == table_dupl) {
                തിരികെ സത്യം;
            }
        }
    }
    തിരികെ അസത്യം;
}


//
//    removeColumn()
//
ശൂന്യം RBBITableBuilder::removeColumn(int32_t column) {
    int32_t അക്കം_States = fDStates->size();
    വേണ്ടി (int32_t state=0; state<അക്കം_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


തർക്കം RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t അക്കം_States = fSafeTable->size();

    വേണ്ടി (; states->first<അക്കം_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        വേണ്ടി (states->second=states->first+1; states->second<അക്കം_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            തർക്കം rowsMatch = സത്യം;
            int32_t അക്കം_Cols = firstRow->length();
            വേണ്ടി (int32_t col=0; col < അക്കം_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                എങ്കിൽ (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = അസത്യം;
                    നിർത്തുക;
                }
            }
            എങ്കിൽ (rowsMatch) {
                തിരികെ സത്യം;
            }
        }
    }
    തിരികെ അസത്യം;
}

ശൂന്യം RBBITableBuilder::removeSafeState(IntPair duplStates) {
    സ്ഥിരം int32_t keepState = duplStates.first;
    സ്ഥിരം int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t അക്കം_States = fSafeTable->size();
    വേണ്ടി (int32_t state=0; state<അക്കം_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t അക്കം_Cols = sd->length();
        വേണ്ടി (int32_t col=0; col<അക്കം_Cols; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            എങ്കിൽ (existingVal == duplState) {
                newVal = keepState;
            } അല്ലെങ്കിൽ എങ്കിൽ (existingVal > duplState) {
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
    സ്ഥിരം int32_t old_State_എണ്ണം = fDStates->size();
    minimizeStates();
    തിരികെ fDStates->size() - old_State_എണ്ണം;
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
ശൂന്യം RBBITableBuilder::minimizeLookaheads() {
    എങ്കിൽ (fLASlotsInUse == ACCEPTING_UNCONDITIONAL) {
        തിരികെ;
    }
    സ്ഥിരം int32_t lookahead_എണ്ണം = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    സ്ഥിരം int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    സ്ഥിരം auto stateDescriptor = [ഇത്](സ്ഥിരം int32_t state) -> RBBIStateDescriptor & {
        തിരികെ *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    സ്ഥിരം auto lookaheadReachability =
        prv::make_unique<തർക്കം[]>(lookahead_എണ്ണം * lookahead_എണ്ണം, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    വേണ്ടി (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        വേണ്ടി (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
            എങ്കിൽ (k != l) {
                വേണ്ടി (int32_t source = 1; source < fDStates->size(); ++source) {
                    എങ്കിൽ (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        തുടരുക;
                    }
                    എങ്കിൽ (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](സ്ഥിരം int32_t state) {
                                തിരികെ static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](സ്ഥിരം int32_t state) {
                                തിരികെ static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_എണ്ണം + k -
                                              firstLookahead] = സത്യം;
                    }
                }
            }
        }
    }

    സ്ഥിരം auto colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_എണ്ണം, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    വേണ്ടി (int32_t chromatic_സംഖ്യ = 1;; ++chromatic_സംഖ്യ) {
        uprv_memset(colours.get(), 0, വലുപ്പം(int32_t) * lookahead_എണ്ണം);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        വേണ്ടി (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            വേണ്ടി (പൂർണ്ണസംഖ്യ source = 0; source < lookahead_എണ്ണം; ++source) {
                വേണ്ടി (പൂർണ്ണസംഖ്യ sink = 0; sink < lookahead_എണ്ണം; ++sink) {
                    എങ്കിൽ (lookaheadReachability[source * lookahead_എണ്ണം + sink] &&
                        colours[source] == colours[sink]) {
                        പോകുക nextColouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < fDStates->size(); ++i) {
                എങ്കിൽ (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        firstLookahead +
                        colours[stateDescriptor(i).fAccepting - firstLookahead];
                }
                എങ്കിൽ (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - firstLookahead];
                }
            }
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_സംഖ്യ;
            തിരികെ;

          nextColouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            വേണ്ടി (int32_t i = 0; i < lookahead_എണ്ണം - 1 && colours[i] == chromatic_സംഖ്യ;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            എങ്കിൽ (colours[lookahead_എണ്ണം - 1] == chromatic_സംഖ്യ) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                നിർത്തുക;
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
int32_t  RBBITableBuilder::getTableSize() സ്ഥിരം {
    int32_t    size = 0;
    int32_t    അക്കം_Rows;
    int32_t    അക്കം_Cols;
    int32_t    rowSize;

    എങ്കിൽ (f_വൃക്ഷം == nullptr) {
        തിരികെ 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    അക്കം_Rows = fDStates->size();
    അക്കം_Cols = fRB->fSetBuilder->get_അക്കം_Char_Categories();

    എങ്കിൽ (use8BitsForTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + വലുപ്പം(int8_t)*അക്കം_Cols;
    } അല്ലെങ്കിൽ {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + വലുപ്പം(int16_t)*അക്കം_Cols;
    }
    size   += അക്കം_Rows * rowSize;
    തിരികെ size;
}

തർക്കം RBBITableBuilder::use8BitsForTable() സ്ഥിരം {
    തിരികെ fDStates->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBITableBuilder::exportTable(ശൂന്യം *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    പൂർണ്ണസംഖ്യ                col;

    എങ്കിൽ (U_FAILURE(*fStatus) || f_വൃക്ഷം == nullptr) {
        തിരികെ;
    }

    int32_t cat_എണ്ണം = fRB->fSetBuilder->get_അക്കം_Char_Categories();
    എങ്കിൽ (cat_എണ്ണം > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        തിരികെ;
    }

    table->f_അക്കം_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->fLookAheadResultsSize = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    എങ്കിൽ (use8BitsForTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + വലുപ്പം(uint8_t) * cat_എണ്ണം;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } അല്ലെങ്കിൽ {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + വലുപ്പം(int16_t) * cat_എണ്ണം;
    }
    എങ്കിൽ (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    വേണ്ടി (state=0; state<table->f_അക്കം_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        എങ്കിൽ (use8BitsForTable()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 255);
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->fTagsIdx   = sd->fTagsIdx;
            വേണ്ടി (col=0; col<cat_എണ്ണം; col++) {
                U_ASSERT (sd->fDtran->elementAti(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = sd->fDtran->elementAti(col);
            }
        } അല്ലെങ്കിൽ {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 0xffff);
            row->r16.fAccepting = sd->fAccepting;
            row->r16.fLookAhead = sd->fLookAhead;
            row->r16.fTagsIdx   = sd->fTagsIdx;
            വേണ്ടി (col=0; col<cat_എണ്ണം; col++) {
                row->r16.fNextState[col] = sd->fDtran->elementAti(col);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
ശൂന്യം RBBITableBuilder::buildSafeReverseTable(UErrorCode &status) {
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

    int32_t അക്കം_Char_Classes = fRB->fSetBuilder->get_അക്കം_Char_Categories();
    int32_t അക്കം_States = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    സ്ഥിരം auto lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    സ്ഥിരം auto wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    വേണ്ടി (int32_t c1 = 1; c1 < അക്കം_Char_Classes; ++c1) {
        എങ്കിൽ (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          തുടരുക;
        }
        വേണ്ടി (int32_t c2 = 1; c2 < അക്കം_Char_Classes; ++c2) {
            എങ്കിൽ (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                തുടരുക;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            തർക്കം mustCheckConsistentWithStartC2 = അസത്യം;
            സ്ഥിരം int32_t text[2] = {c1, c2};
            വേണ്ടി (int32_t injectedState = START_STATE; injectedState < അക്കം_States; ++injectedState) {
                വേണ്ടി (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    എങ്കിൽ (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        സ്ഥിരം RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        എങ്കിൽ (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
                        } അല്ലെങ്കിൽ എങ്കിൽ (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            തുടരുക;
                        }
                        എങ്കിൽ (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    പോകുക dfaStep;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    വേണ്ടി (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        വേണ്ടി (;;) {
                        dfaStep:
                            എങ്കിൽ (i == BEFORE_PAIR || i == 2) {
                                പോകുക inspectFinalState;
                            }
                            സ്ഥിരം int32_t ചിഹ്നം_Ahead = text[i];
                            ++i;
                            സ്ഥിരം RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            എങ്കിൽ (sDescriptor->fDtran->elementAti(ചിഹ്നം_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(ചിഹ്നം_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } അല്ലെങ്കിൽ {
                                എങ്കിൽ (lastBreak != BEFORE_PAIR &&
                                    lastAcceptingPosition == lastBreak) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    പോകുക next_ചിഹ്നം_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                നിർത്തുക;
                            }

                            എങ്കിൽ (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } അല്ലെങ്കിൽ എങ്കിൽ (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
                                നിർത്തുക;
                            }
                            എങ്കിൽ (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    എങ്കിൽ (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        തുടരുക;
                    }
                    എങ്കിൽ (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        വേണ്ടി (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                            എങ്കിൽ (lookaheadPositions[l] != LOOKAHEAD_NOT_SET &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wantedLookaheadsInPair[l] = lookaheadPositions[i];
                            } അല്ലെങ്കിൽ {
                                wantedLookaheadsInPair[l] = LOOKAHEAD_NOT_SET;
                            }
                        }
                    } അല്ലെങ്കിൽ {
                        എങ്കിൽ (wantedEndState != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            പോകുക next_ചിഹ്നം_Pair;
                        }
                        എങ്കിൽ (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            പോകുക next_ചിഹ്നം_Pair;
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
                    വേണ്ടി (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        എങ്കിൽ (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            തുടരുക;
                        }
                        എങ്കിൽ (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            എങ്കിൽ (wantedEndState != START_STATE) {
                                പോകുക next_ചിഹ്നം_Pair;
                            }
                        }
                        എങ്കിൽ (lookaheadPositions[l] == 1 || wantedLookaheadsInPair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            mustCheckConsistentWithStartC2 = സത്യം;
                        }
                    }
                }
            }
            എങ്കിൽ (mustCheckConsistentWithStartC2) {
                സ്ഥിരം RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                സ്ഥിരം auto c2State = startDescriptor.fDtran->elementAti(c2);
                എങ്കിൽ (c2State != wantedEndState) {
                    പോകുക next_ചിഹ്നം_Pair;
                }
                സ്ഥിരം RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                എങ്കിൽ (wantedLastAcceptingPosition != 2) {
                    പോകുക next_ചിഹ്നം_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                വേണ്ടി (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    എങ്കിൽ (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        പോകുക next_ചിഹ്നം_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        next_ചിഹ്നം_Pair:
            തുടരുക;
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
        പുതിയ UVector(uprv_deleteUObject, uhash_compareUnicodeString, അക്കം_Char_Classes + 2, status), status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    fSafeTable = lpSafeTable.orphan();
    വേണ്ടി (int32_t row=0; row<അക്കം_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(പുതിയ UnicodeString(അക്കം_Char_Classes, 0, അക്കം_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    വേണ്ടി (int32_t charClass=0; charClass < അക്കം_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    വേണ്ടി (int32_t row=2; row<അക്കം_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    വേണ്ടി (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    ആകുമ്പോൾ (findDuplicateSafeState(&states)) {
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
int32_t  RBBITableBuilder::getSafeTableSize() സ്ഥിരം {
    int32_t    size = 0;
    int32_t    അക്കം_Rows;
    int32_t    അക്കം_Cols;
    int32_t    rowSize;

    എങ്കിൽ (fSafeTable == nullptr) {
        തിരികെ 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    അക്കം_Rows = fSafeTable->size();
    അക്കം_Cols = fRB->fSetBuilder->get_അക്കം_Char_Categories();

    എങ്കിൽ (use8BitsForSafeTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + വലുപ്പം(int8_t)*അക്കം_Cols;
    } അല്ലെങ്കിൽ {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + വലുപ്പം(int16_t)*അക്കം_Cols;
    }
    size   += അക്കം_Rows * rowSize;
    തിരികെ size;
}

തർക്കം RBBITableBuilder::use8BitsForSafeTable() സ്ഥിരം {
    തിരികെ fSafeTable->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBITableBuilder::exportSafeTable(ശൂന്യം *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    പൂർണ്ണസംഖ്യ                col;

    എങ്കിൽ (U_FAILURE(*fStatus) || fSafeTable == nullptr) {
        തിരികെ;
    }

    int32_t cat_എണ്ണം = fRB->fSetBuilder->get_അക്കം_Char_Categories();
    എങ്കിൽ (cat_എണ്ണം > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        തിരികെ;
    }

    table->f_അക്കം_States = fSafeTable->size();
    table->fFlags     = 0;
    എങ്കിൽ (use8BitsForSafeTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + വലുപ്പം(uint8_t) * cat_എണ്ണം;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } അല്ലെങ്കിൽ {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + വലുപ്പം(int16_t) * cat_എണ്ണം;
    }

    വേണ്ടി (state=0; state<table->f_അക്കം_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        എങ്കിൽ (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            വേണ്ടി (col=0; col<cat_എണ്ണം; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } അല്ലെങ്കിൽ {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            വേണ്ടി (col=0; col<cat_എണ്ണം; col++) {
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
ശൂന്യം RBBITableBuilder::printSet(UVector *s) {
    int32_t  i;
    വേണ്ടി (i=0; i<s->size(); i++) {
        സ്ഥിരം RBBINode *v = static_cast<സ്ഥിരം RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==nullptr? -1 : v->f_Serial_അക്കം);
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
ശൂന്യം RBBITableBuilder::printStates() {
    പൂർണ്ണസംഖ്യ     c;    // input "character"
    പൂർണ്ണസംഖ്യ     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    വേണ്ടി (c=0; c<fRB->fSetBuilder->get_അക്കം_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    വേണ്ടി (c=0; c<fRB->fSetBuilder->get_അക്കം_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    വേണ്ടി (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        വേണ്ടി (c=0; c<fRB->fSetBuilder->get_അക്കം_Char_Categories(); c++) {
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
ശൂന്യം RBBITableBuilder::printReverseTable() {
    പൂർണ്ണസംഖ്യ     c;    // input "character"
    പൂർണ്ണസംഖ്യ     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    എങ്കിൽ (fSafeTable == nullptr) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        തിരികെ;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    വേണ്ടി (c=0; c<fRB->fSetBuilder->get_അക്കം_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    വേണ്ടി (c=0; c<fRB->fSetBuilder->get_അക്കം_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    വേണ്ടി (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        വേണ്ടി (c=0; c<fRB->fSetBuilder->get_അക്കം_Char_Categories(); c++) {
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
ശൂന്യം RBBITableBuilder::printRuleStatusTable() {
    int32_t  thisRecord = 0;
    int32_t  nextRecord = 0;
    പൂർണ്ണസംഖ്യ      i;
    UVector  *tbl = fRB->fRuleStatusVals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    ആകുമ്പോൾ (nextRecord < tbl->size()) {
        thisRecord = nextRecord;
        nextRecord = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        വേണ്ടി (i=thisRecord+1; i<nextRecord; i++) {
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

RBBIStateDescriptor::RBBIStateDescriptor(പൂർണ്ണസംഖ്യ last_നിവേശം_ചിഹ്നം, UErrorCode *fStatus) {
    fMarked    = അസത്യം;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = പുതിയ UVector32(last_നിവേശം_ചിഹ്നം+1, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }
    എങ്കിൽ (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    fDtran->setSize(last_നിവേശം_ചിഹ്നം+1);    // fDtran needs to be pre-sized.
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
