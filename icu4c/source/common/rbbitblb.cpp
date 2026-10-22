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

#ਅਨ੍ਤਰ੍ਭਾਵ <functional>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbitblb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbiscan.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbisetb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbidata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
ਤਾਰਕਿਕ reachableByTransitions(ਅਚਲ UVector &ਹਾਲਤਾਂ, ਅਚਲ int32_t ਸਰੋਤ,
                            ਅਚਲ std::function<ਤਾਰਕਿਕ(int32_t)> isSink,
                            ਅਚਲ std::function<ਤਾਰਕਿਕ(int32_t)> excludedState, UError_ਕੋਡ &ਸਥਿਤੀ) {
    UStack boundary(ਸਥਿਤੀ);
    {
        UVector32 &transitions_From_ਸਰੋਤ =
            *static_cast<RBBIStateDescriptor *>(ਹਾਲਤਾਂ.elementAt(ਸਰੋਤ))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        ਲਈ (int32_t ਚਿੰਨ੍ਹ = 0; ਚਿੰਨ੍ਹ < transitions_From_ਸਰੋਤ.size(); ++ਚਿੰਨ੍ਹ) {
            ਅਚਲ int32_t state = transitions_From_ਸਰੋਤ.elementAti(ਚਿੰਨ੍ਹ);
            ਜੇ (state != 0) {
                boundary.push(state, ਸਥਿਤੀ);
            }
        }
    }
    ਅਚਲ ਸਵੈਚਾਲਿਤ ਦੇਖਿਆ = prv::make_unique<ਤਾਰਕਿਕ[]>(ਹਾਲਤਾਂ.size(), ਸਥਿਤੀ);
    ਜਦੋਂ (U_SUCCESS(ਸਥਿਤੀ) && !boundary.empty()) {
        ਅਚਲ int32_t s = boundary.popi();
        ਜੇ (isSink(s)) {
            ਵਾਪਸ ਸੱਚ;
        }
        ਜੇ (excludedState(s) || ਦੇਖਿਆ[s]) {
            ਜਾਰੀ;
        }
        ਦੇਖਿਆ[s] = ਸੱਚ;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(ਹਾਲਤਾਂ.elementAt(s))->fDtran;
        ਲਈ (int32_t ਚਿੰਨ੍ਹ = 0; ਚਿੰਨ੍ਹ < transitions.size(); ++ਚਿੰਨ੍ਹ) {
            ਅਚਲ int32_t t = transitions.elementAti(ਚਿੰਨ੍ਹ);
            ਜੇ (t != 0 && !ਦੇਖਿਆ[t]) {
                boundary.push(t, ਸਥਿਤੀ);
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

} // namespace

ਅਚਲ int32_t k_Max_State_For8_ਬਿੱਟ_ਸਾਰਣੀ = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **ਜੜ੍ਹ_ਗੰਢ, UError_ਕੋਡ &ਸਥਿਤੀ) :
        fRB(rb),
        f_ਰੁੱਖ(*ਜੜ੍ਹ_ਗੰਢ),
        f_ਸਥਿਤੀ(&ਸਥਿਤੀ),
        fDStates(ਖਾਲੀ_ਸੂਚਕ),
        f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ(ਖਾਲੀ_ਸੂਚਕ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = ਨਵਾਂ UVector(ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && fDStates == ਖਾਲੀ_ਸੂਚਕ ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    ਪੂਰਨ i;
    ਲਈ (i=0; i<fDStates->size(); i++) {
        ਮਿਟਾਓ static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    ਮਿਟਾਓ fDStates;
    ਮਿਟਾਓ f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ;
    ਮਿਟਾਓ f_Look_Ahead_ਨਿਯਮ_Map;
}


//-----------------------------------------------------------------------------
//
//   RBBITableBuilder::buildForwardTable  -  This is the main function for building
//                               the DFA state transition table from the RBBI rules parse tree.
//
//-----------------------------------------------------------------------------
ਖਾਲੀ  RBBITableBuilder::build_Forward_ਸਾਰਣੀ() {

    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    ਜੇ (f_ਰੁੱਖ==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_ਰੁੱਖ = f_ਰੁੱਖ->flattenVariables(*f_ਸਥਿਤੀ, 0);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਜੇ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_ਰੁੱਖ(f_ਰੁੱਖ, ਸੱਚ);
    }
#ਜੇ_ਅੰਤ

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = ਨਵਾਂ RBBINode(RBBINode::opCat, *f_ਸਥਿਤੀ);
    // Exit if memory allocation failed.
    ਜੇ (cn == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਮਿਟਾਓ cn;
        ਵਾਪਸ;
    }
    cn->f_ਖੱਬਾ_ਬੱਚਾ = f_ਰੁੱਖ;
    f_ਰੁੱਖ->f_ਮਾਪਾ = cn;
    RBBINode *ਸਿਰਾ_Marker_ਗੰਢ = cn->f_ਸੱਜਾ_ਬੱਚਾ = ਨਵਾਂ RBBINode(RBBINode::ਸਿਰਾ_Mark, *f_ਸਥਿਤੀ);
    // Delete and exit if memory allocation failed.
    ਜੇ (cn->f_ਸੱਜਾ_ਬੱਚਾ == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਮਿਟਾਓ cn;
        ਵਾਪਸ;
    }
    cn->f_ਸੱਜਾ_ਬੱਚਾ->f_ਮਾਪਾ = cn;
    f_ਰੁੱਖ = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_ਰੁੱਖ->flattenSets(*f_ਸਥਿਤੀ, 0);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਜੇ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_ਰੁੱਖ(f_ਰੁੱਖ, ਸੱਚ);
    }
#ਜੇ_ਅੰਤ


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_ਰੁੱਖ);
    calc_ਪਹਿਲਾ_Pos(f_ਰੁੱਖ);
    calc_ਆਖਰੀ_Pos(f_ਰੁੱਖ);
    calcFollowPos(f_ਰੁੱਖ);
    ਜੇ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_ਰੁੱਖ);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    ਜੇ (fRB->f_Chain_ਨਿਯਮਾਂ) {
        calcChainedFollowPos(f_ਰੁੱਖ, ਸਿਰਾ_Marker_ਗੰਢ);
    }

    //
    // Build the DFA state transition tables.
    //
    build_State_ਸਾਰਣੀ();
    map_Look_Ahead_ਨਿਯਮਾਂ();
    flag_Accepting_ਹਾਲਤਾਂ();
    flag_Look_Ahead_ਹਾਲਤਾਂ();
    flag_Tagged_ਹਾਲਤਾਂ();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    ਮਿਲਾਓ_ਨਿਯਮ_ਸਥਿਤੀ_Vals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::calcNullable(RBBINode *n) {
    ਜੇ (n == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਜੇ (n->fType == RBBINode::ਸੈੱਟ_Ref ||
        n->fType == RBBINode::ਸਿਰਾ_Mark ) {
        // These are non-empty leaf node types.
        n->fNullable = ਝੂਠ;
        ਵਾਪਸ;
    }

    ਜੇ (n->fType == RBBINode::lookAhead || n->fType == RBBINode::ਟੈਗ) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = ਸੱਚ;
        ਵਾਪਸ;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_ਖੱਬਾ_ਬੱਚਾ);
    calcNullable(n->f_ਸੱਜਾ_ਬੱਚਾ);

    // Apply functions from table 3.40 in Aho
    ਜੇ (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_ਖੱਬਾ_ਬੱਚਾ->fNullable || n->f_ਸੱਜਾ_ਬੱਚਾ->fNullable;
    }
    ਨਹੀਂਤਾਂ ਜੇ (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_ਖੱਬਾ_ਬੱਚਾ->fNullable && n->f_ਸੱਜਾ_ਬੱਚਾ->fNullable;
    }
    ਨਹੀਂਤਾਂ ਜੇ (n->fType == RBBINode::op_ਤਾਰਾ || n->fType == RBBINode::opQuestion) {
        n->fNullable = ਸੱਚ;
    }
    ਨਹੀਂਤਾਂ {
        n->fNullable = ਝੂਠ;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::calc_ਪਹਿਲਾ_Pos(RBBINode *n) {
    ਜੇ (n == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਜੇ (n->fType == RBBINode::ਪੱਤੀ_Char  ||
        n->fType == RBBINode::ਸਿਰਾ_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ਟੈਗ) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ->ਜੋੜੋ_Element(n, *f_ਸਥਿਤੀ);
        ਵਾਪਸ;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calc_ਪਹਿਲਾ_Pos(n->f_ਖੱਬਾ_ਬੱਚਾ);
    calc_ਪਹਿਲਾ_Pos(n->f_ਸੱਜਾ_ਬੱਚਾ);

    // Apply functions from table 3.40 in Aho
    ਜੇ (n->fType == RBBINode::opOr) {
        ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ, n->f_ਖੱਬਾ_ਬੱਚਾ->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);
        ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ, n->f_ਸੱਜਾ_ਬੱਚਾ->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (n->fType == RBBINode::opCat) {
        ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ, n->f_ਖੱਬਾ_ਬੱਚਾ->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);
        ਜੇ (n->f_ਖੱਬਾ_ਬੱਚਾ->fNullable) {
            ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ, n->f_ਸੱਜਾ_ਬੱਚਾ->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ (n->fType == RBBINode::op_ਤਾਰਾ ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ, n->f_ਖੱਬਾ_ਬੱਚਾ->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::calc_ਆਖਰੀ_Pos(RBBINode *n) {
    ਜੇ (n == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਜੇ (n->fType == RBBINode::ਪੱਤੀ_Char  ||
        n->fType == RBBINode::ਸਿਰਾ_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ਟੈਗ) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_ਆਖਰੀ_Pos_ਸੈੱਟ->ਜੋੜੋ_Element(n, *f_ਸਥਿਤੀ);
        ਵਾਪਸ;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calc_ਆਖਰੀ_Pos(n->f_ਖੱਬਾ_ਬੱਚਾ);
    calc_ਆਖਰੀ_Pos(n->f_ਸੱਜਾ_ਬੱਚਾ);

    // Apply functions from table 3.40 in Aho
    ਜੇ (n->fType == RBBINode::opOr) {
        ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਆਖਰੀ_Pos_ਸੈੱਟ, n->f_ਖੱਬਾ_ਬੱਚਾ->f_ਆਖਰੀ_Pos_ਸੈੱਟ);
        ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਆਖਰੀ_Pos_ਸੈੱਟ, n->f_ਸੱਜਾ_ਬੱਚਾ->f_ਆਖਰੀ_Pos_ਸੈੱਟ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (n->fType == RBBINode::opCat) {
        ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਆਖਰੀ_Pos_ਸੈੱਟ, n->f_ਸੱਜਾ_ਬੱਚਾ->f_ਆਖਰੀ_Pos_ਸੈੱਟ);
        ਜੇ (n->f_ਸੱਜਾ_ਬੱਚਾ->fNullable) {
            ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਆਖਰੀ_Pos_ਸੈੱਟ, n->f_ਖੱਬਾ_ਬੱਚਾ->f_ਆਖਰੀ_Pos_ਸੈੱਟ);
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ (n->fType == RBBINode::op_ਤਾਰਾ     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        ਸੈੱਟ_ਜੋੜੋ_2(n->f_ਆਖਰੀ_Pos_ਸੈੱਟ, n->f_ਖੱਬਾ_ਬੱਚਾ->f_ਆਖਰੀ_Pos_ਸੈੱਟ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::calcFollowPos(RBBINode *n) {
    ਜੇ (n == ਖਾਲੀ_ਸੂਚਕ ||
        n->fType == RBBINode::ਪੱਤੀ_Char ||
        n->fType == RBBINode::ਸਿਰਾ_Mark) {
        ਵਾਪਸ;
    }

    calcFollowPos(n->f_ਖੱਬਾ_ਬੱਚਾ);
    calcFollowPos(n->f_ਸੱਜਾ_ਬੱਚਾ);

    // Aho rule #1
    ਜੇ (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *ਆਖਰੀ_Pos_ਦਾ_ਖੱਬਾ_ਬੱਚਾ = n->f_ਖੱਬਾ_ਬੱਚਾ->f_ਆਖਰੀ_Pos_ਸੈੱਟ;

        ਲਈ (ix = 0; ix < static_cast<uint32_t>(ਆਖਰੀ_Pos_ਦਾ_ਖੱਬਾ_ਬੱਚਾ->size()); ix++) {
            i = static_cast<RBBINode*>(ਆਖਰੀ_Pos_ਦਾ_ਖੱਬਾ_ਬੱਚਾ->elementAt(ix));
            ਸੈੱਟ_ਜੋੜੋ_2(i->fFollowPos, n->f_ਸੱਜਾ_ਬੱਚਾ->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);
        }
    }

    // Aho rule #2
    ਜੇ (n->fType == RBBINode::op_ਤਾਰਾ ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        ਲਈ (ix = 0; ix < static_cast<uint32_t>(n->f_ਆਖਰੀ_Pos_ਸੈੱਟ->size()); ix++) {
            i = static_cast<RBBINode*>(n->f_ਆਖਰੀ_Pos_ਸੈੱਟ->elementAt(ix));
            ਸੈੱਟ_ਜੋੜੋ_2(i->fFollowPos, n->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::ਜੋੜੋ_ਨਿਯਮ_ਜੜ੍ਹ_Nodes(UVector *dest, RBBINode *ਗੰਢ) {
    ਜੇ (ਗੰਢ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    U_ASSERT(!dest->hasDeleter());
    ਜੇ (ਗੰਢ->f_ਨਿਯਮ_ਜੜ੍ਹ) {
        dest->ਜੋੜੋ_Element(ਗੰਢ, *f_ਸਥਿਤੀ);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        ਵਾਪਸ;
    }
    ਜੋੜੋ_ਨਿਯਮ_ਜੜ੍ਹ_Nodes(dest, ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ);
    ਜੋੜੋ_ਨਿਯਮ_ਜੜ੍ਹ_Nodes(dest, ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::calcChainedFollowPos(RBBINode *ਰੁੱਖ, RBBINode *ਸਿਰਾ_Mark_ਗੰਢ) {

    UVector         ਪੱਤੀ_Nodes(*f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    // get a list all leaf nodes
    ਰੁੱਖ->ਲੱਭੋ_Nodes(&ਪੱਤੀ_Nodes, RBBINode::ਪੱਤੀ_Char, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector ਨਿਯਮ_ਜੜ੍ਹ_Nodes(*f_ਸਥਿਤੀ);
    ਜੋੜੋ_ਨਿਯਮ_ਜੜ੍ਹ_Nodes(&ਨਿਯਮ_ਜੜ੍ਹ_Nodes, ਰੁੱਖ);

    UVector ਮੇਲ_ਸ਼ੁਰੂ_Nodes(*f_ਸਥਿਤੀ);
    ਲਈ (ਪੂਰਨ j=0; j<ਨਿਯਮ_ਜੜ੍ਹ_Nodes.size(); ++j) {
        RBBINode *ਗੰਢ = static_cast<RBBINode *>(ਨਿਯਮ_ਜੜ੍ਹ_Nodes.elementAt(j));
        ਜੇ (ਗੰਢ->f_Chain_ਵਿੱਚ) {
            ਸੈੱਟ_ਜੋੜੋ_2(&ਮੇਲ_ਸ਼ੁਰੂ_Nodes, ਗੰਢ->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);
        }
    }
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    int32_t  ਸਿਰਾ_ਗੰਢ_Ix;
    int32_t  ਸ਼ੁਰੂ_ਗੰਢ_Ix;

    ਲਈ (ਸਿਰਾ_ਗੰਢ_Ix=0; ਸਿਰਾ_ਗੰਢ_Ix<ਪੱਤੀ_Nodes.size(); ਸਿਰਾ_ਗੰਢ_Ix++) {
        RBBINode* ਸਿਰਾ_ਗੰਢ = static_cast<RBBINode*>(ਪੱਤੀ_Nodes.elementAt(ਸਿਰਾ_ਗੰਢ_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        ਜੇ (!ਸਿਰਾ_ਗੰਢ->fFollowPos->ਸ਼ਾਮਲ_ਹੈ(ਸਿਰਾ_Mark_ਗੰਢ)) {
            ਜਾਰੀ;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *ਸ਼ੁਰੂ_ਗੰਢ;
        ਲਈ (ਸ਼ੁਰੂ_ਗੰਢ_Ix = 0; ਸ਼ੁਰੂ_ਗੰਢ_Ix<ਮੇਲ_ਸ਼ੁਰੂ_Nodes.size(); ਸ਼ੁਰੂ_ਗੰਢ_Ix++) {
            ਸ਼ੁਰੂ_ਗੰਢ = static_cast<RBBINode*>(ਮੇਲ_ਸ਼ੁਰੂ_Nodes.elementAt(ਸ਼ੁਰੂ_ਗੰਢ_Ix));
            ਜੇ (ਸ਼ੁਰੂ_ਗੰਢ->fType != RBBINode::ਪੱਤੀ_Char) {
                ਜਾਰੀ;
            }

            ਜੇ (ਸਿਰਾ_ਗੰਢ->fVal == ਸ਼ੁਰੂ_ਗੰਢ->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                ਸੈੱਟ_ਜੋੜੋ_2(ਸਿਰਾ_ਗੰਢ->fFollowPos, ਸ਼ੁਰੂ_ਗੰਢ->fFollowPos);
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
ਖਾਲੀ RBBITableBuilder::bofFixup() {

    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
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
    RBBINode  *bof_ਗੰਢ = f_ਰੁੱਖ->f_ਖੱਬਾ_ਬੱਚਾ->f_ਖੱਬਾ_ਬੱਚਾ;
    U_ASSERT(bof_ਗੰਢ->fType == RBBINode::ਪੱਤੀ_Char);
    U_ASSERT(bof_ਗੰਢ->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *ਮੇਲ_ਸ਼ੁਰੂ_Nodes = f_ਰੁੱਖ->f_ਖੱਬਾ_ਬੱਚਾ->f_ਸੱਜਾ_ਬੱਚਾ->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ;

    RBBINode *ਸ਼ੁਰੂ_ਗੰਢ;
    ਪੂਰਨ       ਸ਼ੁਰੂ_ਗੰਢ_Ix;
    ਲਈ (ਸ਼ੁਰੂ_ਗੰਢ_Ix = 0; ਸ਼ੁਰੂ_ਗੰਢ_Ix<ਮੇਲ_ਸ਼ੁਰੂ_Nodes->size(); ਸ਼ੁਰੂ_ਗੰਢ_Ix++) {
        ਸ਼ੁਰੂ_ਗੰਢ = static_cast<RBBINode*>(ਮੇਲ_ਸ਼ੁਰੂ_Nodes->elementAt(ਸ਼ੁਰੂ_ਗੰਢ_Ix));
        ਜੇ (ਸ਼ੁਰੂ_ਗੰਢ->fType != RBBINode::ਪੱਤੀ_Char) {
            ਜਾਰੀ;
        }

        ਜੇ (ਸ਼ੁਰੂ_ਗੰਢ->fVal == bof_ਗੰਢ->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            ਸੈੱਟ_ਜੋੜੋ_2(bof_ਗੰਢ->fFollowPos, ਸ਼ੁਰੂ_ਗੰਢ->fFollowPos);
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
ਖਾਲੀ RBBITableBuilder::build_State_ਸਾਰਣੀ() {
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = ਖਾਲੀ_ਸੂਚਕ;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    ਪੂਰਨ      ਆਖਰੀ_ਨਿਵੇਸ਼_ਚਿੰਨ੍ਹ = fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories() - 1;
    failState = ਨਵਾਂ RBBIStateDescriptor(ਆਖਰੀ_ਨਿਵੇਸ਼_ਚਿੰਨ੍ਹ, f_ਸਥਿਤੀ);
    ਜੇ (failState == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਜਾਓ ExitBuildSTdeleteall;
    }
    failState->fPositions = ਨਵਾਂ UVector(*f_ਸਥਿਤੀ);
    ਜੇ (failState->fPositions == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ (failState->fPositions == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਜਾਓ ExitBuildSTdeleteall;
    }
    fDStates->ਜੋੜੋ_Element(failState, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਜਾਓ ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = ਨਵਾਂ RBBIStateDescriptor(ਆਖਰੀ_ਨਿਵੇਸ਼_ਚਿੰਨ੍ਹ, f_ਸਥਿਤੀ);
    ਜੇ (initialState == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਜਾਓ ExitBuildSTdeleteall;
    }
    initialState->fPositions = ਨਵਾਂ UVector(*f_ਸਥਿਤੀ);
    ਜੇ (initialState->fPositions == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਜਾਓ ExitBuildSTdeleteall;
    }
    ਸੈੱਟ_ਜੋੜੋ_2(initialState->fPositions, f_ਰੁੱਖ->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);
    fDStates->ਜੋੜੋ_Element(initialState, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਜਾਓ ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    ਲਈ (;;) {
        RBBIStateDescriptor *T = ਖਾਲੀ_ਸੂਚਕ;
        int32_t              tx;
        ਲਈ (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *ਆਰਜ਼ੀ;
            ਆਰਜ਼ੀ = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            ਜੇ (ਆਰਜ਼ੀ->fMarked == ਝੂਠ) {
                T = ਆਰਜ਼ੀ;
                ਰੋਕੋ;
            }
        }
        ਜੇ (T == ਖਾਲੀ_ਸੂਚਕ) {
            ਰੋਕੋ;
        }

        // mark T;
        T->fMarked = ਸੱਚ;

        // for each input symbol a do begin
        int32_t  a;
        ਲਈ (a = 1; a<=ਆਖਰੀ_ਨਿਵੇਸ਼_ਚਿੰਨ੍ਹ; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = ਖਾਲੀ_ਸੂਚਕ;
            RBBINode   *p;
            int32_t     px;
            ਲਈ (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                ਜੇ ((p->fType == RBBINode::ਪੱਤੀ_Char) &&  (p->fVal == a)) {
                    ਜੇ (U == ਖਾਲੀ_ਸੂਚਕ) {
                        U = ਨਵਾਂ UVector(*f_ਸਥਿਤੀ);
                        ਜੇ (U == ਖਾਲੀ_ਸੂਚਕ) {
                        	*f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        	ਜਾਓ ExitBuildSTdeleteall;
                        }
                    }
                    ਸੈੱਟ_ਜੋੜੋ_2(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = ਝੂਠ;
            ਜੇ (U != ਖਾਲੀ_ਸੂਚਕ) {
                U_ASSERT(U->size() > 0);
                ਪੂਰਨ  ix;
                ਲਈ (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    ਜੇ (ਸੈੱਟ_Equals(U, temp2->fPositions)) {
                        ਮਿਟਾਓ U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = ਸੱਚ;
                        ਰੋਕੋ;
                    }
                }

                // Add U as an unmarked state to Dstates
                ਜੇ (!UinDstates)
                {
                    RBBIStateDescriptor *newState = ਨਵਾਂ RBBIStateDescriptor(ਆਖਰੀ_ਨਿਵੇਸ਼_ਚਿੰਨ੍ਹ, f_ਸਥਿਤੀ);
                    ਜੇ (newState == ਖਾਲੀ_ਸੂਚਕ) {
                    	*f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    }
                    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
                        ਜਾਓ ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->ਜੋੜੋ_Element(newState, *f_ਸਥਿਤੀ);
                    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
                        ਵਾਪਸ;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->ਸੈੱਟ_Element_At(ux, a);
            }
        }
    }
    ਵਾਪਸ;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    ਮਿਟਾਓ initialState;
    ਮਿਟਾਓ failState;
}


/**
 * mapLookAheadRules
 *
 */
ਖਾਲੀ RBBITableBuilder::map_Look_Ahead_ਨਿਯਮਾਂ() {
    f_Look_Ahead_ਨਿਯਮ_Map =  ਨਵਾਂ UVector32(fRB->fScanner->ਅੰਕ_ਨਿਯਮਾਂ() + 1, *f_ਸਥਿਤੀ);
    ਜੇ (f_Look_Ahead_ਨਿਯਮ_Map == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    f_Look_Ahead_ਨਿਯਮ_Map->ਸੈੱਟ_ਮਾਪ(fRB->fScanner->ਅੰਕ_ਨਿਯਮਾਂ() + 1);

    ਲਈ (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t la_ਥਾਂ_For_State = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        ਤਾਰਕਿਕ saw_Look_Ahead_ਗੰਢ = ਝੂਠ;
        ਲਈ (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ਗੰਢ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            ਜੇ (ਗੰਢ->fType != RBBINode::ਗੰਢ_Type::lookAhead) {
                ਜਾਰੀ;
            }
            saw_Look_Ahead_ਗੰਢ = ਸੱਚ;
            int32_t ਨਿਯਮ_ਅੰਕ = ਗੰਢ->fVal;     // Set when rule was originally parsed.
            U_ASSERT(ਨਿਯਮ_ਅੰਕ < f_Look_Ahead_ਨਿਯਮ_Map->size());
            U_ASSERT(ਨਿਯਮ_ਅੰਕ > 0);
            int32_t la_ਥਾਂ = f_Look_Ahead_ਨਿਯਮ_Map->elementAti(ਨਿਯਮ_ਅੰਕ);
            ਜੇ (la_ਥਾਂ != 0) {
                ਜੇ (la_ਥਾਂ_For_State == 0) {
                    la_ਥਾਂ_For_State = la_ਥਾਂ;
                } ਨਹੀਂਤਾਂ {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(la_ਥਾਂ == la_ਥਾਂ_For_State);
                }
            }
        }
        ਜੇ (!saw_Look_Ahead_ਗੰਢ) {
            ਜਾਰੀ;
        }

        ਜੇ (la_ਥਾਂ_For_State == 0) {
            la_ਥਾਂ_For_State = ++f_LASlots_ਵਿੱਚ_Use;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        ਲਈ (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ਗੰਢ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            ਜੇ (ਗੰਢ->fType != RBBINode::ਗੰਢ_Type::lookAhead) {
                ਜਾਰੀ;
            }
            int32_t ਨਿਯਮ_ਅੰਕ = ਗੰਢ->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = f_Look_Ahead_ਨਿਯਮ_Map->elementAti(ਨਿਯਮ_ਅੰਕ);
            (ਖਾਲੀ)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == la_ਥਾਂ_For_State);
            f_Look_Ahead_ਨਿਯਮ_Map->ਸੈੱਟ_Element_At(la_ਥਾਂ_For_State, ਨਿਯਮ_ਅੰਕ);
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
ਖਾਲੀ     RBBITableBuilder::flag_Accepting_ਹਾਲਤਾਂ() {
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    UVector     ਸਿਰਾ_Marker_Nodes(*f_ਸਥਿਤੀ);
    RBBINode    *ਸਿਰਾ_Marker;
    int32_t     i;
    int32_t     n;

    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    f_ਰੁੱਖ->ਲੱਭੋ_Nodes(&ਸਿਰਾ_Marker_Nodes, RBBINode::ਸਿਰਾ_Mark, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    ਲਈ (i=0; i<ਸਿਰਾ_Marker_Nodes.size(); i++) {
        ਸਿਰਾ_Marker = static_cast<RBBINode*>(ਸਿਰਾ_Marker_Nodes.elementAt(i));
        ਲਈ (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            ਜੇ (sd->fPositions->ਸੂਚਕਾਂਕ_ਦਾ(ਸਿਰਾ_Marker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                ਜੇ (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = f_Look_Ahead_ਨਿਯਮ_Map->elementAti(ਸਿਰਾ_Marker->fVal);
                    ਜੇ (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                ਜੇ (sd->fAccepting==ACCEPTING_UNCONDITIONAL && ਸਿਰਾ_Marker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = f_Look_Ahead_ਨਿਯਮ_Map->elementAti(ਸਿਰਾ_Marker->fVal);
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
ਖਾਲੀ     RBBITableBuilder::flag_Look_Ahead_ਹਾਲਤਾਂ() {
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    UVector     lookAheadNodes(*f_ਸਥਿਤੀ);
    RBBINode    *look_Ahead_ਗੰਢ;
    int32_t     i;
    int32_t     n;

    f_ਰੁੱਖ->ਲੱਭੋ_Nodes(&lookAheadNodes, RBBINode::lookAhead, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਲਈ (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_ਗੰਢ = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_ਗੰਢ->fType == RBBINode::ਗੰਢ_Type::lookAhead);

        ਲਈ (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->ਸੂਚਕਾਂਕ_ਦਾ(look_Ahead_ਗੰਢ);
            ਜੇ (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_ਗੰਢ == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookahead_ਥਾਂ = f_Look_Ahead_ਨਿਯਮ_Map->elementAti(look_Ahead_ਗੰਢ->fVal);
                U_ASSERT(sd->fLookAhead == 0 || sd->fLookAhead == lookahead_ਥਾਂ);
                // if (sd->fLookAhead != 0 && sd->fLookAhead != lookaheadSlot) {
                //     printf("%s:%d Bingo. sd->fLookAhead:%d   lookaheadSlot:%d\n",
                //            __FILE__, __LINE__, sd->fLookAhead, lookaheadSlot);
                // }
                sd->fLookAhead = lookahead_ਥਾਂ;
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//    flagTaggedStates
//
//-----------------------------------------------------------------------------
ਖਾਲੀ     RBBITableBuilder::flag_Tagged_ਹਾਲਤਾਂ() {
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    UVector     ਟੈਗ_Nodes(*f_ਸਥਿਤੀ);
    RBBINode    *ਟੈਗ_ਗੰਢ;
    int32_t     i;
    int32_t     n;

    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    f_ਰੁੱਖ->ਲੱਭੋ_Nodes(&ਟੈਗ_Nodes, RBBINode::ਟੈਗ, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਲਈ (i=0; i<ਟੈਗ_Nodes.size(); i++) {                   // For each tag node t (all of 'em)
        ਟੈਗ_ਗੰਢ = static_cast<RBBINode*>(ਟੈਗ_Nodes.elementAt(i));

        ਲਈ (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            ਜੇ (sd->fPositions->ਸੂਚਕਾਂਕ_ਦਾ(ਟੈਗ_ਗੰਢ) >= 0) {  //       if  s include the tag node t
                sorted_ਜੋੜੋ(&sd->f_ਟੈਗ_Vals, ਟੈਗ_ਗੰਢ->fVal);
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
ਖਾਲੀ  RBBITableBuilder::ਮਿਲਾਓ_ਨਿਯਮ_ਸਥਿਤੀ_Vals() {
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
    ਪੂਰਨ i;
    ਪੂਰਨ n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    ਜੇ (fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->size() == 0) {
        fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->ਜੋੜੋ_Element(1, *f_ਸਥਿਤੀ);  // Num of statuses in group
        fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->ਜੋੜੋ_Element(static_cast<int32_t>(0), *f_ਸਥਿਤੀ); // and our single status of zero
    }

    //    For each state
    ਲਈ (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_ਹਾਲਤਾਂ_ਟੈਗ_ਮੁੱਲਾਂ = sd->f_ਟੈਗ_Vals;
        ਜੇ (this_ਹਾਲਤਾਂ_ਟੈਗ_ਮੁੱਲਾਂ == ਖਾਲੀ_ਸੂਚਕ) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->f_ਟੈਗ_ਸੂਚੀ_Idx = 0;
            ਜਾਰੀ;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->f_ਟੈਗ_ਸੂਚੀ_Idx = -1;
        int32_t  this_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ = 0;   // indexes into the global rule status vals list
        int32_t  ਅਗਲਾ_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ = 0;

        // Loop runs once per group of tags in the global list
        ਜਦੋਂ (ਅਗਲਾ_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ < fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->size()) {
            this_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ = ਅਗਲਾ_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ;
            ਅਗਲਾ_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ += fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->elementAti(this_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ) + 1;
            ਜੇ (this_ਹਾਲਤਾਂ_ਟੈਗ_ਮੁੱਲਾਂ->size() != fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->elementAti(this_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                ਜਾਰੀ;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            ਲਈ (i=0; i<this_ਹਾਲਤਾਂ_ਟੈਗ_ਮੁੱਲਾਂ->size(); i++) {
                ਜੇ (this_ਹਾਲਤਾਂ_ਟੈਗ_ਮੁੱਲਾਂ->elementAti(i) !=
                    fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->elementAti(this_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ + 1 + i) ) {
                    // Mismatch.
                    ਰੋਕੋ;
                }
            }

            ਜੇ (i == this_ਹਾਲਤਾਂ_ਟੈਗ_ਮੁੱਲਾਂ->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->f_ਟੈਗ_ਸੂਚੀ_Idx = this_ਟੈਗ_ਸਮੂਹ_ਸ਼ੁਰੂ;
                ਰੋਕੋ;
            }
        }

        ਜੇ (sd->f_ਟੈਗ_ਸੂਚੀ_Idx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->f_ਟੈਗ_ਸੂਚੀ_Idx = fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->size();
            fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->ਜੋੜੋ_Element(this_ਹਾਲਤਾਂ_ਟੈਗ_ਮੁੱਲਾਂ->size(), *f_ਸਥਿਤੀ);
            ਲਈ (i=0; i<this_ਹਾਲਤਾਂ_ਟੈਗ_ਮੁੱਲਾਂ->size(); i++) {
                fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->ਜੋੜੋ_Element(this_ਹਾਲਤਾਂ_ਟੈਗ_ਮੁੱਲਾਂ->elementAti(i), *f_ਸਥਿਤੀ);
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

ਖਾਲੀ RBBITableBuilder::minimize_ਹਾਲਤਾਂ() {
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

#ਜੇ UPRV_HAS_SANITIZER
    ਜੇ (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *f_ਸਥਿਤੀ = U_REGEX_ਨਮੂਨਾ_TOO_BIG;
        ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    ਬਣਤਰ StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t f_ਟੈਗ_ਸੂਚੀ_Idx;
        ਤਾਰਕਿਕ ਸੰਚਾਲਕ==(StateType ਅਚਲ &other) ਅਚਲ {
            ਵਾਪਸ fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   f_ਟੈਗ_ਸੂਚੀ_Idx == other.f_ਟੈਗ_ਸੂਚੀ_Idx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    ਬਣਤਰ Type_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ : UMemory {
        Type_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ(ਅਚਲ StateType &type, UError_ਕੋਡ &ਸਥਿਤੀ)
            : type(type), ਹਾਲਤਾਂ(prv::make_unique<UVector>(ਸਥਿਤੀ, ਸਥਿਤੀ)) {}
        StateType type;
        prv::unique_ptr<UVector> ਹਾਲਤਾਂ;
    };
    UVector initial_ਹਿੱਸੇ_ਕਰੋ(*f_ਸਥਿਤੀ);
    initial_ਹਿੱਸੇ_ਕਰੋ.ਸੈੱਟ_Deleter(
        [](ਖਾਲੀ *p) { ਮਿਟਾਓ static_cast<Type_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ *>(p); });
    ਲਈ (int32_t i = 0; i < fDStates->size(); ++i) {
        ਅਚਲ RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ਅਚਲ StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.f_ਟੈਗ_ਸੂਚੀ_Idx};
        int32_t j = 0;
        ਲਈ (; j < initial_ਹਿੱਸੇ_ਕਰੋ.size(); ++j) {
            ਸਵੈਚਾਲਿਤ &[type_j, ਹਾਲਤਾਂ] =
                *static_cast<Type_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ *>(initial_ਹਿੱਸੇ_ਕਰੋ.elementAt(j));
            ਜੇ (type_j == type) {
                ਹਾਲਤਾਂ->ਜੋੜੋ_Element(i, *f_ਸਥਿਤੀ);
                ਰੋਕੋ;
            }
        }
        ਜੇ (j == initial_ਹਿੱਸੇ_ਕਰੋ.size()) {
            ਸਵੈਚਾਲਿਤ new_ਇੰਦਰਾਜ = prv::make_unique<Type_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ>(type, *f_ਸਥਿਤੀ, *f_ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
                ਵਾਪਸ;
            }
            new_ਇੰਦਰਾਜ->ਹਾਲਤਾਂ->ਜੋੜੋ_Element(i, *f_ਸਥਿਤੀ);
            initial_ਹਿੱਸੇ_ਕਰੋ.adoptElement(new_ਇੰਦਰਾਜ.release(), *f_ਸਥਿਤੀ);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    ਸਵੈਚਾਲਿਤ ਹਿੱਸੇ_ਕਰੋ = prv::make_unique<UVector>(*f_ਸਥਿਤੀ, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਹਿੱਸੇ_ਕਰੋ->ਸੈੱਟ_Deleter(
        [](ਖਾਲੀ *p) { ਮਿਟਾਓ static_cast<UVector*>(p); });
    
    ਲਈ (int32_t i = 0; i < initial_ਹਿੱਸੇ_ਕਰੋ.size(); ++i) {
        ਹਿੱਸੇ_ਕਰੋ->adoptElement(
            static_cast<Type_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ*>(initial_ਹਿੱਸੇ_ਕਰੋ.elementAt(i))->ਹਾਲਤਾਂ.release(),
            *f_ਸਥਿਤੀ);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    ਸਵੈਚਾਲਿਤ ਹਿੱਸੇ_ਕਰੋ_signature = [&ਹਿੱਸੇ_ਕਰੋ, ਇਹ](int32_t state_ਸੂਚਕਾਂਕ) -> prv::unique_ptr<UVector> {
        ਅਚਲ RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state_ਸੂਚਕਾਂਕ));
        ਸਵੈਚਾਲਿਤ ਨਤੀਜਾ = prv::make_unique<UVector>(state.fDtran->size(), *f_ਸਥਿਤੀ, *f_ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਲਈ (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t ਮੰਜ਼ਿਲ_State = state.fDtran->elementAti(i);
            ਲਈ (int32_t part_ਸੂਚਕਾਂਕ = 0; part_ਸੂਚਕਾਂਕ < ਹਿੱਸੇ_ਕਰੋ->size(); ++part_ਸੂਚਕਾਂਕ) {
                ਅਚਲ UVector &part = *static_cast<UVector *>(ਹਿੱਸੇ_ਕਰੋ->elementAt(part_ਸੂਚਕਾਂਕ));
                ਜੇ (part.ਸ਼ਾਮਲ_ਹੈ(ਮੰਜ਼ਿਲ_State)) {
                    ਨਤੀਜਾ->ਜੋੜੋ_Element(part_ਸੂਚਕਾਂਕ, *f_ਸਥਿਤੀ);
                    ਰੋਕੋ;
                }
            }
        }
        ਨਤੀਜਾ->ਸੈੱਟ_Comparer([](ਅਚਲ UElement ਖੱਬੇ, ਅਚਲ UElement ਸੱਜੇ) -> UBool {
            ਵਾਪਸ ਖੱਬੇ.integer == ਸੱਜੇ.integer;
        });
        ਵਾਪਸ ਨਤੀਜਾ;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    ਲਈ (;;) {
        // Π_new.
        ਸਵੈਚਾਲਿਤ ਹਿੱਸੇ_ਕਰੋ_New = prv::make_unique<UVector>(*f_ਸਥਿਤੀ, *f_ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
            ਵਾਪਸ;
        }
        ਹਿੱਸੇ_ਕਰੋ_New->ਸੈੱਟ_Deleter(
            [](ਖਾਲੀ *p) { ਮਿਟਾਓ static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        ਤਾਰਕਿਕ refined = ਝੂਠ;
        ਲਈ (int32_t i = 0; i < ਹਿੱਸੇ_ਕਰੋ->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ਅਚਲ UVector &ਸਮੂਹ = *static_cast<UVector*>(ਹਿੱਸੇ_ਕਰੋ->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            ਬਣਤਰ Signature_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ : UMemory {
                Signature_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ(prv::unique_ptr<UVector> signature, UError_ਕੋਡ &ਸਥਿਤੀ)
                    : signature(std::move(signature)), ਹਾਲਤਾਂ(prv::make_unique<UVector>(ਸਥਿਤੀ, ਸਥਿਤੀ)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> ਹਾਲਤਾਂ;
            };
            UVector ਸਮੂਹ_Refinement(*f_ਸਥਿਤੀ);
            ਸਮੂਹ_Refinement.ਸੈੱਟ_Deleter([](ਖਾਲੀ *p) { ਮਿਟਾਓ static_cast<Signature_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ*>(p); });
            ਲਈ (int32_t j = 0; j < ਸਮੂਹ.size(); ++j) {
                // Index of a state in the group.
                ਅਚਲ int32_t s = ਸਮੂਹ.elementAti(j);
                ਸਵੈਚਾਲਿਤ signature = ਹਿੱਸੇ_ਕਰੋ_signature(s);
                int32_t k = 0;
                ਲਈ (; k < ਸਮੂਹ_Refinement.size(); ++k) {
                    ਅਚਲ ਸਵੈਚਾਲਿਤ &[subgroupSignature, subgroup] =
                        *static_cast<Signature_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ*>(ਸਮੂਹ_Refinement.elementAt(k));
                    ਜੇ (*subgroupSignature == *signature) {
                        subgroup->ਜੋੜੋ_Element(s, *f_ਸਥਿਤੀ);
                        ਰੋਕੋ;
                    }
                }
                ਜੇ (k == ਸਮੂਹ_Refinement.size()) {
                    ਅਚਲ ਸਵੈਚਾਲਿਤ new_ਇੰਦਰਾਜ =
                        ਨਵਾਂ Signature_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ(std::move(signature), *f_ਸਥਿਤੀ);
                    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
                        ਵਾਪਸ;
                    }
                    ਜੇ (new_ਇੰਦਰਾਜ == ਖਾਲੀ_ਸੂਚਕ) {
                      *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                      ਵਾਪਸ;
                    }
                    new_ਇੰਦਰਾਜ->ਹਾਲਤਾਂ->ਜੋੜੋ_Element(s, *f_ਸਥਿਤੀ);
                    ਸਮੂਹ_Refinement.adoptElement(new_ਇੰਦਰਾਜ, *f_ਸਥਿਤੀ);
                }
            }
            refined |= ਸਮੂਹ_Refinement.size() > 1;
            ਲਈ (int32_t j = 0; j < ਸਮੂਹ_Refinement.size(); ++j) {
                ਸਵੈਚਾਲਿਤ &[_, subgroup] = *static_cast<Signature_ਮੰਜ਼ਿਲ_ਹਾਲਤਾਂ*>(
                    ਸਮੂਹ_Refinement.elementAt(j));
                ਹਿੱਸੇ_ਕਰੋ_New->adoptElement(subgroup.release(), *f_ਸਥਿਤੀ);
            }
        }
        ਜੇ (refined) {
            ਹਿੱਸੇ_ਕਰੋ = std::move(ਹਿੱਸੇ_ਕਰੋ_New);
        } ਨਹੀਂਤਾਂ {
            ਰੋਕੋ;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    ਲਈ (ਪੂਰਨ i = 0; i < ਹਿੱਸੇ_ਕਰੋ->size(); ++i) {
        UVector *ਅਚਲ part_i = static_cast<UVector *>(ਹਿੱਸੇ_ਕਰੋ->elementAt(i));
        ਜੇ (part_i->ਸ਼ਾਮਲ_ਹੈ(0)) {
            ਖਾਲੀ *ਅਚਲ part_0 = ਹਿੱਸੇ_ਕਰੋ->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ਅਚਲ ਸਵੈਚਾਲਿਤ deleter = ਹਿੱਸੇ_ਕਰੋ->ਸੈੱਟ_Deleter(ਖਾਲੀ_ਸੂਚਕ);
            ਹਿੱਸੇ_ਕਰੋ->ਸੈੱਟ_Element_At(part_i, 0);
            ਹਿੱਸੇ_ਕਰੋ->ਸੈੱਟ_Element_At(part_0, i);
            ਹਿੱਸੇ_ਕਰੋ->ਸੈੱਟ_Deleter(deleter);
        } ਨਹੀਂਤਾਂ ਜੇ (part_i->ਸ਼ਾਮਲ_ਹੈ(1)) {
            ਖਾਲੀ *ਅਚਲ part_1 = ਹਿੱਸੇ_ਕਰੋ->elementAt(1);
            ਅਚਲ ਸਵੈਚਾਲਿਤ deleter = ਹਿੱਸੇ_ਕਰੋ->ਸੈੱਟ_Deleter(ਖਾਲੀ_ਸੂਚਕ);
            ਹਿੱਸੇ_ਕਰੋ->ਸੈੱਟ_Element_At(part_i, 1);
            ਹਿੱਸੇ_ਕਰੋ->ਸੈੱਟ_Element_At(part_1, i);
            ਹਿੱਸੇ_ਕਰੋ->ਸੈੱਟ_Deleter(deleter);
        }
    }
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਸਵੈਚਾਲਿਤ old_State_ਮੰਜ਼ਿਲ_Part = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਲਈ (ਪੂਰਨ i = 0; i < ਹਿੱਸੇ_ਕਰੋ->size(); ++i) {
        ਅਚਲ UVector &part = *static_cast<UVector*>(ਹਿੱਸੇ_ਕਰੋ->elementAt(i));
        ਲਈ (ਪੂਰਨ j = 0; j < part.size(); ++j) {
            old_State_ਮੰਜ਼ਿਲ_Part[part.elementAti(j)] = i;
        }
    }
    ਸਵੈਚਾਲਿਤ ਅਚਲ old_ਹਾਲਤਾਂ = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*f_ਸਥਿਤੀ, *f_ਸਥਿਤੀ).release();
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਲਈ (ਪੂਰਨ i = 0; i < ਹਿੱਸੇ_ਕਰੋ->size(); ++i) {
        ਅਚਲ UVector &part = *static_cast<UVector*>(ਹਿੱਸੇ_ਕਰੋ->elementAt(i));
        RBBIStateDescriptor *ਅਚਲ state =
            static_cast<RBBIStateDescriptor*>(old_ਹਾਲਤਾਂ->elementAt(part.elementAti(0)));
        fDStates->ਜੋੜੋ_Element(state, *f_ਸਥਿਤੀ);
        ਲਈ (ਪੂਰਨ j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->ਸੈੱਟ_Element_At(old_State_ਮੰਜ਼ਿਲ_Part[state->fDtran->elementAti(j)], j);
        }
        old_ਹਾਲਤਾਂ->ਸੈੱਟ_Element_At(ਖਾਲੀ_ਸੂਚਕ, part.elementAti(0));
    }
    old_ਹਾਲਤਾਂ->ਸੈੱਟ_Deleter([](ਖਾਲੀ *p) { ਮਿਟਾਓ static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::sorted_ਜੋੜੋ(UVector **vector, int32_t val) {
    int32_t i;

    ਜੇ (*vector == ਖਾਲੀ_ਸੂਚਕ) {
        *vector = ਨਵਾਂ UVector(*f_ਸਥਿਤੀ);
    }
    ਜੇ (*vector == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    UVector *vec = *vector;
    int32_t  v_ਮਾਪ = vec->size();
    ਲਈ (i=0; i<v_ਮਾਪ; i++) {
        int32_t valAtI = vec->elementAti(i);
        ਜੇ (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            ਵਾਪਸ;
        }
        ਜੇ (valAtI > val) {
            ਰੋਕੋ;
        }
    }
    vec->ਪਾਓ_Element_At(val, i, *f_ਸਥਿਤੀ);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::ਸੈੱਟ_ਜੋੜੋ_2(UVector *dest, UVector *ਸਰੋਤ) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!ਸਰੋਤ->hasDeleter());
    int32_t dest_Original_ਮਾਪ = dest->size();
    int32_t ਸਰੋਤ_ਮਾਪ       = ਸਰੋਤ->size();
    int32_t di           = 0;
    Maybe_ਢੇਰ_Array<ਖਾਲੀ *, 16> destArray, ਸਰੋਤ_Array;  // Handle small cases without malloc
    ਖਾਲੀ **destPtr, **ਸਰੋਤ_Ptr;
    ਖਾਲੀ **destLim, **ਸਰੋਤ_Lim;

    ਜੇ (dest_Original_ਮਾਪ > destArray.get_ਸਮਰੱਥਾ()) {
        ਜੇ (destArray.resize(dest_Original_ਮਾਪ) == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + dest_Original_ਮਾਪ;  // destArray.getArrayLimit()?

    ਜੇ (ਸਰੋਤ_ਮਾਪ > ਸਰੋਤ_Array.get_ਸਮਰੱਥਾ()) {
        ਜੇ (ਸਰੋਤ_Array.resize(ਸਰੋਤ_ਮਾਪ) == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ;
        }
    }
    ਸਰੋਤ_Ptr = ਸਰੋਤ_Array.getAlias();
    ਸਰੋਤ_Lim = ਸਰੋਤ_Ptr + ਸਰੋਤ_ਮਾਪ;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (ਖਾਲੀ) dest->ਮੰਜ਼ਿਲ_Array(destPtr);
    (ਖਾਲੀ) ਸਰੋਤ->ਮੰਜ਼ਿਲ_Array(ਸਰੋਤ_Ptr);

    dest->ਸੈੱਟ_ਮਾਪ(ਸਰੋਤ_ਮਾਪ+dest_Original_ਮਾਪ, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    ਜਦੋਂ (ਸਰੋਤ_Ptr < ਸਰੋਤ_Lim && destPtr < destLim) {
        ਜੇ (*destPtr == *ਸਰੋਤ_Ptr) {
            dest->ਸੈੱਟ_Element_At(*ਸਰੋਤ_Ptr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        ਨਹੀਂਤਾਂ ਜੇ (uprv_memcmp(destPtr, ਸਰੋਤ_Ptr, ਆਕਾਰ(ਖਾਲੀ *)) < 0) {
            dest->ਸੈੱਟ_Element_At(*destPtr++, di++);
        }
        ਨਹੀਂਤਾਂ { /* *sourcePtr < *destPtr */
            dest->ਸੈੱਟ_Element_At(*ਸਰੋਤ_Ptr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    ਜਦੋਂ (destPtr < destLim) {
        dest->ਸੈੱਟ_Element_At(*destPtr++, di++);
    }
    ਜਦੋਂ (ਸਰੋਤ_Ptr < ਸਰੋਤ_Lim) {
        dest->ਸੈੱਟ_Element_At(*ਸਰੋਤ_Ptr++, di++);
    }

    dest->ਸੈੱਟ_ਮਾਪ(di, *f_ਸਥਿਤੀ);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::ਸੈੱਟ_Equals(UVector *a, UVector *b) {
    ਵਾਪਸ a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBITableBuilder::printPosSets(RBBINode *n) {
    ਜੇ (n==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਲਿਖੋ("\n");
    RBBINode::print_ਗੰਢ_ਸਿਰਲੇਖ();
    RBBINode::print_ਗੰਢ(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    print_ਸੈੱਟ(n->f_ਪਹਿਲਾ_Pos_ਸੈੱਟ);

    RBBIDebugPrintf("         lastpos:   ");
    print_ਸੈੱਟ(n->f_ਆਖਰੀ_Pos_ਸੈੱਟ);

    RBBIDebugPrintf("         followpos: ");
    print_ਸੈੱਟ(n->fFollowPos);

    printPosSets(n->f_ਖੱਬਾ_ਬੱਚਾ);
    printPosSets(n->f_ਸੱਜਾ_ਬੱਚਾ);
}
#ਜੇ_ਅੰਤ

//
//    findDuplCharClassFrom()
//
ਤਾਰਕਿਕ RBBITableBuilder::ਲੱਭੋ_Dupl_Char_Class_From(IntPair *categories) {
    int32_t ਅੰਕ_ਹਾਲਤਾਂ = fDStates->size();
    int32_t ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ = fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories();

    ਲਈ (; categories->first < ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        ਪੂਰਨ ਹੱਦਬੰਦੀ_ਦੂਜਾ = categories->first < fRB->f_ਸੈੱਟ_Builder->get_Dict_Categories_ਸ਼ੁਰੂ() ?
            fRB->f_ਸੈੱਟ_Builder->get_Dict_Categories_ਸ਼ੁਰੂ() : ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ;
        ਲਈ (categories->ਦੂਜਾ=categories->first+1; categories->ਦੂਜਾ < ਹੱਦਬੰਦੀ_ਦੂਜਾ; categories->ਦੂਜਾ++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t ਸਾਰਣੀ_ਅਧਾਰ = 0;
            uint16_t ਸਾਰਣੀ_dupl = 1;
            ਲਈ (int32_t state=0; state<ਅੰਕ_ਹਾਲਤਾਂ; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                ਸਾਰਣੀ_ਅਧਾਰ = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                ਸਾਰਣੀ_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->ਦੂਜਾ));
                ਜੇ (ਸਾਰਣੀ_ਅਧਾਰ != ਸਾਰਣੀ_dupl) {
                    ਰੋਕੋ;
                }
            }
            ਜੇ (ਸਾਰਣੀ_ਅਧਾਰ == ਸਾਰਣੀ_dupl) {
                ਵਾਪਸ ਸੱਚ;
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}


//
//    removeColumn()
//
ਖਾਲੀ RBBITableBuilder::remove_ਕਾਲਮ(int32_t ਕਾਲਮ) {
    int32_t ਅੰਕ_ਹਾਲਤਾਂ = fDStates->size();
    ਲਈ (int32_t state=0; state<ਅੰਕ_ਹਾਲਤਾਂ; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(ਕਾਲਮ < sd->fDtran->size());
        sd->fDtran->removeElementAt(ਕਾਲਮ);
    }
}


ਤਾਰਕਿਕ RBBITableBuilder::ਲੱਭੋ_Duplicate_ਸੁਰੱਖਿਅਤ_State(IntPair *ਹਾਲਤਾਂ) {
    int32_t ਅੰਕ_ਹਾਲਤਾਂ = f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->size();

    ਲਈ (; ਹਾਲਤਾਂ->first<ਅੰਕ_ਹਾਲਤਾਂ-1; ਹਾਲਤਾਂ->first++) {
        UnicodeString *ਪਹਿਲਾ_ਪੰਕਤੀ = static_cast<UnicodeString *>(f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->elementAt(ਹਾਲਤਾਂ->first));
        ਲਈ (ਹਾਲਤਾਂ->ਦੂਜਾ=ਹਾਲਤਾਂ->first+1; ਹਾਲਤਾਂ->ਦੂਜਾ<ਅੰਕ_ਹਾਲਤਾਂ; ਹਾਲਤਾਂ->ਦੂਜਾ++) {
            UnicodeString *dupl_ਪੰਕਤੀ = static_cast<UnicodeString *>(f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->elementAt(ਹਾਲਤਾਂ->ਦੂਜਾ));
            ਤਾਰਕਿਕ ਪੰਕਤੀਆਂ_ਮੇਲ = ਸੱਚ;
            int32_t ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ = ਪਹਿਲਾ_ਪੰਕਤੀ->length();
            ਲਈ (int32_t ਖਾਨਾ=0; ਖਾਨਾ < ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ; ++ਖਾਨਾ) {
                int32_t ਪਹਿਲਾ_Val = ਪਹਿਲਾ_ਪੰਕਤੀ->charAt(ਖਾਨਾ);
                int32_t duplVal = dupl_ਪੰਕਤੀ->charAt(ਖਾਨਾ);
                ਜੇ (!((ਪਹਿਲਾ_Val == duplVal) ||
                        ((ਪਹਿਲਾ_Val == ਹਾਲਤਾਂ->first || ਪਹਿਲਾ_Val == ਹਾਲਤਾਂ->ਦੂਜਾ) &&
                        (duplVal  == ਹਾਲਤਾਂ->first || duplVal  == ਹਾਲਤਾਂ->ਦੂਜਾ)))) {
                    ਪੰਕਤੀਆਂ_ਮੇਲ = ਝੂਠ;
                    ਰੋਕੋ;
                }
            }
            ਜੇ (ਪੰਕਤੀਆਂ_ਮੇਲ) {
                ਵਾਪਸ ਸੱਚ;
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ RBBITableBuilder::remove_ਸੁਰੱਖਿਅਤ_State(IntPair dupl_ਹਾਲਤਾਂ) {
    ਅਚਲ int32_t keepState = dupl_ਹਾਲਤਾਂ.first;
    ਅਚਲ int32_t duplState = dupl_ਹਾਲਤਾਂ.ਦੂਜਾ;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->size());

    f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t ਅੰਕ_ਹਾਲਤਾਂ = f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->size();
    ਲਈ (int32_t state=0; state<ਅੰਕ_ਹਾਲਤਾਂ; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->elementAt(state));
        int32_t ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ = sd->length();
        ਲਈ (int32_t ਖਾਨਾ=0; ਖਾਨਾ<ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ++) {
            int32_t existingVal = sd->charAt(ਖਾਨਾ);
            int32_t newVal = existingVal;
            ਜੇ (existingVal == duplState) {
                newVal = keepState;
            } ਨਹੀਂਤਾਂ ਜੇ (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->ਸੈੱਟ_Char_At(ਖਾਨਾ, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::remove_Duplicate_ਹਾਲਤਾਂ() {
    ਅਚਲ int32_t old_State_ਗਿਣਤੀ = fDStates->size();
    minimize_ਹਾਲਤਾਂ();
    ਵਾਪਸ fDStates->size() - old_State_ਗਿਣਤੀ;
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
ਖਾਲੀ RBBITableBuilder::minimizeLookaheads() {
    ਜੇ (f_LASlots_ਵਿੱਚ_Use == ACCEPTING_UNCONDITIONAL) {
        ਵਾਪਸ;
    }
    ਅਚਲ int32_t lookahead_ਗਿਣਤੀ = f_LASlots_ਵਿੱਚ_Use - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ਅਚਲ int32_t ਪਹਿਲਾ_Lookahead = ACCEPTING_UNCONDITIONAL + 1;
    ਅਚਲ ਸਵੈਚਾਲਿਤ stateDescriptor = [ਇਹ](ਅਚਲ int32_t state) -> RBBIStateDescriptor & {
        ਵਾਪਸ *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ਅਚਲ ਸਵੈਚਾਲਿਤ lookaheadReachability =
        prv::make_unique<ਤਾਰਕਿਕ[]>(lookahead_ਗਿਣਤੀ * lookahead_ਗਿਣਤੀ, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਲਈ (int32_t l = ਪਹਿਲਾ_Lookahead; l <= f_LASlots_ਵਿੱਚ_Use; ++l) {
        ਲਈ (int32_t k = ਪਹਿਲਾ_Lookahead; k <= f_LASlots_ਵਿੱਚ_Use; ++k) {
            ਜੇ (k != l) {
                ਲਈ (int32_t ਸਰੋਤ = 1; ਸਰੋਤ < fDStates->size(); ++ਸਰੋਤ) {
                    ਜੇ (static_cast<int32_t>(stateDescriptor(ਸਰੋਤ).fLookAhead) != l) {
                        ਜਾਰੀ;
                    }
                    ਜੇ (reachableByTransitions(
                            *fDStates, ਸਰੋਤ, /*isSink=*/
                            [&](ਅਚਲ int32_t state) {
                                ਵਾਪਸ static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ਅਚਲ int32_t state) {
                                ਵਾਪਸ static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *f_ਸਥਿਤੀ)) {
                        lookaheadReachability[(l - ਪਹਿਲਾ_Lookahead) * lookahead_ਗਿਣਤੀ + k -
                                              ਪਹਿਲਾ_Lookahead] = ਸੱਚ;
                    }
                }
            }
        }
    }

    ਅਚਲ ਸਵੈਚਾਲਿਤ colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_ਗਿਣਤੀ, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    ਲਈ (int32_t chromatic_ਸੰਖਿਆ = 1;; ++chromatic_ਸੰਖਿਆ) {
        uprv_memset(colours.get(), 0, ਆਕਾਰ(int32_t) * lookahead_ਗਿਣਤੀ);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        ਲਈ (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            ਲਈ (ਪੂਰਨ ਸਰੋਤ = 0; ਸਰੋਤ < lookahead_ਗਿਣਤੀ; ++ਸਰੋਤ) {
                ਲਈ (ਪੂਰਨ sink = 0; sink < lookahead_ਗਿਣਤੀ; ++sink) {
                    ਜੇ (lookaheadReachability[ਸਰੋਤ * lookahead_ਗਿਣਤੀ + sink] &&
                        colours[ਸਰੋਤ] == colours[sink]) {
                        ਜਾਓ ਅਗਲਾ_Colouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            ਲਈ (ਪੂਰਨ i = 0; i < fDStates->size(); ++i) {
                ਜੇ (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        ਪਹਿਲਾ_Lookahead +
                        colours[stateDescriptor(i).fAccepting - ਪਹਿਲਾ_Lookahead];
                }
                ਜੇ (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - ਪਹਿਲਾ_Lookahead];
                }
            }
            f_LASlots_ਵਿੱਚ_Use = ACCEPTING_UNCONDITIONAL + chromatic_ਸੰਖਿਆ;
            ਵਾਪਸ;

          ਅਗਲਾ_Colouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            ਲਈ (int32_t i = 0; i < lookahead_ਗਿਣਤੀ - 1 && colours[i] == chromatic_ਸੰਖਿਆ;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            ਜੇ (colours[lookahead_ਗਿਣਤੀ - 1] == chromatic_ਸੰਖਿਆ) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                ਰੋਕੋ;
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
int32_t  RBBITableBuilder::get_ਸਾਰਣੀ_ਮਾਪ() ਅਚਲ {
    int32_t    size = 0;
    int32_t    ਅੰਕ_ਪੰਕਤੀਆਂ;
    int32_t    ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ;
    int32_t    ਪੰਕਤੀ_ਮਾਪ;

    ਜੇ (f_ਰੁੱਖ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    }

    size    = offsetof(RBBIState_ਸਾਰਣੀ, f_ਸਾਰਣੀ_ਅੰਕੜੇ);    // The header, with no rows to the table.

    ਅੰਕ_ਪੰਕਤੀਆਂ = fDStates->size();
    ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ = fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories();

    ਜੇ (use8_ਬਿੱਟ_For_ਸਾਰਣੀ()) {
        ਪੰਕਤੀ_ਮਾਪ = offsetof(RBBIState_ਸਾਰਣੀ_Row8, f_ਅਗਲਾ_State) + ਆਕਾਰ(int8_t)*ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ;
    } ਨਹੀਂਤਾਂ {
        ਪੰਕਤੀ_ਮਾਪ = offsetof(RBBIState_ਸਾਰਣੀ_Row16, f_ਅਗਲਾ_State) + ਆਕਾਰ(int16_t)*ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ;
    }
    size   += ਅੰਕ_ਪੰਕਤੀਆਂ * ਪੰਕਤੀ_ਮਾਪ;
    ਵਾਪਸ size;
}

ਤਾਰਕਿਕ RBBITableBuilder::use8_ਬਿੱਟ_For_ਸਾਰਣੀ() ਅਚਲ {
    ਵਾਪਸ fDStates->size() <= k_Max_State_For8_ਬਿੱਟ_ਸਾਰਣੀ;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::export_ਸਾਰਣੀ(ਖਾਲੀ *where) {
    RBBIState_ਸਾਰਣੀ* ਸਾਰਣੀ = static_cast<RBBIState_ਸਾਰਣੀ*>(where);
    uint32_t           state;
    ਪੂਰਨ                ਖਾਨਾ;

    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ) || f_ਰੁੱਖ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    int32_t cat_ਗਿਣਤੀ = fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories();
    ਜੇ (cat_ਗਿਣਤੀ > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *f_ਸਥਿਤੀ = U_BRK_ਅੰਦਰੂਨੀ_ERROR;
        ਵਾਪਸ;
    }

    ਸਾਰਣੀ->f_ਅੰਕ_ਹਾਲਤਾਂ = fDStates->size();
    ਸਾਰਣੀ->f_Dict_Categories_ਸ਼ੁਰੂ = fRB->f_ਸੈੱਟ_Builder->get_Dict_Categories_ਸ਼ੁਰੂ();
    ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ = f_LASlots_ਵਿੱਚ_Use == ACCEPTING_UNCONDITIONAL ? 0 : f_LASlots_ਵਿੱਚ_Use + 1;
    ਸਾਰਣੀ->fFlags     = 0;
    ਜੇ (use8_ਬਿੱਟ_For_ਸਾਰਣੀ()) {
        ਸਾਰਣੀ->f_ਪੰਕਤੀ_Len    = offsetof(RBBIState_ਸਾਰਣੀ_Row8, f_ਅਗਲਾ_State) + ਆਕਾਰ(uint8_t) * cat_ਗਿਣਤੀ;
        ਸਾਰਣੀ->fFlags  |= RBBI_8_ਬਿੱਟ_ਪੰਕਤੀਆਂ;
    } ਨਹੀਂਤਾਂ {
        ਸਾਰਣੀ->f_ਪੰਕਤੀ_Len    = offsetof(RBBIState_ਸਾਰਣੀ_Row16, f_ਅਗਲਾ_State) + ਆਕਾਰ(int16_t) * cat_ਗਿਣਤੀ;
    }
    ਜੇ (fRB->fLookAheadHardBreak) {
        ਸਾਰਣੀ->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    ਲਈ (state=0; state<ਸਾਰਣੀ->f_ਅੰਕ_ਹਾਲਤਾਂ; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ* ਪੰਕਤੀ = reinterpret_cast<RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ*>(ਸਾਰਣੀ->f_ਸਾਰਣੀ_ਅੰਕੜੇ + state * ਸਾਰਣੀ->f_ਪੰਕਤੀ_Len);
        ਜੇ (use8_ਬਿੱਟ_For_ਸਾਰਣੀ()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->f_ਟੈਗ_ਸੂਚੀ_Idx && sd->f_ਟੈਗ_ਸੂਚੀ_Idx <= 255);
            RBBIState_ਸਾਰਣੀ_Row8* r8 = reinterpret_cast<RBBIState_ਸਾਰਣੀ_Row8*>(ਪੰਕਤੀ);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->f_ਟੈਗ_ਸੂਚੀ_Idx   = sd->f_ਟੈਗ_ਸੂਚੀ_Idx;
            ਲਈ (ਖਾਨਾ=0; ਖਾਨਾ<cat_ਗਿਣਤੀ; ਖਾਨਾ++) {
                U_ASSERT (sd->fDtran->elementAti(ਖਾਨਾ) <= k_Max_State_For8_ਬਿੱਟ_ਸਾਰਣੀ);
                r8->f_ਅਗਲਾ_State[ਖਾਨਾ] = sd->fDtran->elementAti(ਖਾਨਾ);
            }
        } ਨਹੀਂਤਾਂ {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->f_ਟੈਗ_ਸੂਚੀ_Idx && sd->f_ਟੈਗ_ਸੂਚੀ_Idx <= 0xffff);
            ਪੰਕਤੀ->r16.fAccepting = sd->fAccepting;
            ਪੰਕਤੀ->r16.fLookAhead = sd->fLookAhead;
            ਪੰਕਤੀ->r16.f_ਟੈਗ_ਸੂਚੀ_Idx   = sd->f_ਟੈਗ_ਸੂਚੀ_Idx;
            ਲਈ (ਖਾਨਾ=0; ਖਾਨਾ<cat_ਗਿਣਤੀ; ਖਾਨਾ++) {
                ਪੰਕਤੀ->r16.f_ਅਗਲਾ_State[ਖਾਨਾ] = sd->fDtran->elementAti(ਖਾਨਾ);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
ਖਾਲੀ RBBITableBuilder::build_ਸੁਰੱਖਿਅਤ_ਉਲਟ_ਸਾਰਣੀ(UError_ਕੋਡ &ਸਥਿਤੀ) {
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
    UnicodeString ਸੁਰੱਖਿਅਤ_ਜੋੜੇ;

    int32_t ਅੰਕ_Char_Classes = fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories();
    int32_t ਅੰਕ_ਹਾਲਤਾਂ = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ਅਚਲ ਸਵੈਚਾਲਿਤ lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_ਵਿੱਚ_Use + 1, ਸਥਿਤੀ);
    ਅਚਲ ਸਵੈਚਾਲਿਤ wanted_Lookaheads_ਵਿੱਚ_Pair =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_ਵਿੱਚ_Use + 1, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t ਸ਼ੁਰੂ_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_ਸੈੱਟ = -1;
    ਲਈ (int32_t c1 = 1; c1 < ਅੰਕ_Char_Classes; ++c1) {
        ਜੇ (fRB->f_ਸੈੱਟ_Builder->get_ਪਹਿਲਾ_Char(c1) < 0) {
          ਜਾਰੀ;
        }
        ਲਈ (int32_t c2 = 1; c2 < ਅੰਕ_Char_Classes; ++c2) {
            ਜੇ (fRB->f_ਸੈੱਟ_Builder->get_ਪਹਿਲਾ_Char(c2) < 0) {
                ਜਾਰੀ;
            }
            int32_t wanted_ਸਿਰਾ_State = -1;
            int32_t wanted_ਆਖਰੀ_Accepting_Position;
            ਤਾਰਕਿਕ must_Check_Consistent_With_ਸ਼ੁਰੂ_C2 = ਝੂਠ;
            ਅਚਲ int32_t ਪਾਠ[2] = {c1, c2};
            ਲਈ (int32_t injectedState = ਸ਼ੁਰੂ_STATE; injectedState < ਅੰਕ_ਹਾਲਤਾਂ; ++injectedState) {
                ਲਈ (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= f_LASlots_ਵਿੱਚ_Use; ++injectedLookahead) {
                    int32_t ਆਖਰੀ_Break = BEFORE_PAIR;
                    int32_t ਆਖਰੀ_Accepting_Position = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_ਸੈੱਟ, f_LASlots_ਵਿੱਚ_Use + 1);
                    ਜੇ (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ਅਚਲ RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        ਜੇ (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            ਆਖਰੀ_Accepting_Position = 0;
                        } ਨਹੀਂਤਾਂ ਜੇ (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            ਜਾਰੀ;
                        }
                        ਜੇ (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    ਜਾਓ dfa_ਕਦਮ;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    ਲਈ (;;) {
                        s = ਸ਼ੁਰੂ_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_ਸੈੱਟ, f_LASlots_ਵਿੱਚ_Use + 1);
                        i = ਆਖਰੀ_Break;
                        // Loop Find_Next_Break in L2/26-135.
                        ਲਈ (;;) {
                        dfa_ਕਦਮ:
                            ਜੇ (i == BEFORE_PAIR || i == 2) {
                                ਜਾਓ inspectFinalState;
                            }
                            ਅਚਲ int32_t ਚਿੰਨ੍ਹ_Ahead = ਪਾਠ[i];
                            ++i;
                            ਅਚਲ RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            ਜੇ (sDescriptor->fDtran->elementAti(ਚਿੰਨ੍ਹ_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(ਚਿੰਨ੍ਹ_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } ਨਹੀਂਤਾਂ {
                                ਜੇ (ਆਖਰੀ_Break != BEFORE_PAIR &&
                                    ਆਖਰੀ_Accepting_Position == ਆਖਰੀ_Break) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    ਜਾਓ ਅਗਲਾ_ਚਿੰਨ੍ਹ_Pair;
                                }
                                ਆਖਰੀ_Break = ਆਖਰੀ_Accepting_Position;
                                ਰੋਕੋ;
                            }

                            ਜੇ (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                ਆਖਰੀ_Accepting_Position = i;
                            } ਨਹੀਂਤਾਂ ਜੇ (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_ਸੈੱਟ) {
                                ਆਖਰੀ_Break = lookaheadPositions[sDescriptor->fAccepting];
                                ਰੋਕੋ;
                            }
                            ਜੇ (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    ਜੇ (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        ਜਾਰੀ;
                    }
                    ਜੇ (wanted_ਸਿਰਾ_State < 0) {
                        wanted_ਸਿਰਾ_State = s;
                        wanted_ਆਖਰੀ_Accepting_Position = ਆਖਰੀ_Accepting_Position;
                        ਲਈ (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_ਵਿੱਚ_Use; ++l) {
                            ਜੇ (lookaheadPositions[l] != LOOKAHEAD_NOT_ਸੈੱਟ &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wanted_Lookaheads_ਵਿੱਚ_Pair[l] = lookaheadPositions[i];
                            } ਨਹੀਂਤਾਂ {
                                wanted_Lookaheads_ਵਿੱਚ_Pair[l] = LOOKAHEAD_NOT_ਸੈੱਟ;
                            }
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਜੇ (wanted_ਸਿਰਾ_State != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            ਜਾਓ ਅਗਲਾ_ਚਿੰਨ੍ਹ_Pair;
                        }
                        ਜੇ (wanted_ਆਖਰੀ_Accepting_Position != ਆਖਰੀ_Accepting_Position) {
                            ਜਾਓ ਅਗਲਾ_ਚਿੰਨ੍ਹ_Pair;
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
                    ਲਈ (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_ਵਿੱਚ_Use; ++l) {
                        ਜੇ (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wanted_Lookaheads_ਵਿੱਚ_Pair[l]) {
                            ਜਾਰੀ;
                        }
                        ਜੇ (lookaheadPositions[l] == 2 || wanted_Lookaheads_ਵਿੱਚ_Pair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            ਜੇ (wanted_ਸਿਰਾ_State != ਸ਼ੁਰੂ_STATE) {
                                ਜਾਓ ਅਗਲਾ_ਚਿੰਨ੍ਹ_Pair;
                            }
                        }
                        ਜੇ (lookaheadPositions[l] == 1 || wanted_Lookaheads_ਵਿੱਚ_Pair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            must_Check_Consistent_With_ਸ਼ੁਰੂ_C2 = ਸੱਚ;
                        }
                    }
                }
            }
            ਜੇ (must_Check_Consistent_With_ਸ਼ੁਰੂ_C2) {
                ਅਚਲ RBBIStateDescriptor& ਸ਼ੁਰੂ_Descriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(ਸ਼ੁਰੂ_STATE));
                ਅਚਲ ਸਵੈਚਾਲਿਤ c2State = ਸ਼ੁਰੂ_Descriptor.fDtran->elementAti(c2);
                ਜੇ (c2State != wanted_ਸਿਰਾ_State) {
                    ਜਾਓ ਅਗਲਾ_ਚਿੰਨ੍ਹ_Pair;
                }
                ਅਚਲ RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                ਜੇ (wanted_ਆਖਰੀ_Accepting_Position != 2) {
                    ਜਾਓ ਅਗਲਾ_ਚਿੰਨ੍ਹ_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                ਲਈ (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_ਵਿੱਚ_Use; ++l) {
                    ਜੇ (((wanted_Lookaheads_ਵਿੱਚ_Pair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wanted_ਸਿਰਾ_State != ਸ਼ੁਰੂ_STATE) {
                        ਜਾਓ ਅਗਲਾ_ਚਿੰਨ੍ਹ_Pair;
                    }
                }
            }
            ਸੁਰੱਖਿਅਤ_ਜੋੜੇ.append(static_cast<char16_t>(c1));
            ਸੁਰੱਖਿਅਤ_ਜੋੜੇ.append(static_cast<char16_t>(c2));
        ਅਗਲਾ_ਚਿੰਨ੍ਹ_Pair:
            ਜਾਰੀ;
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

    U_ASSERT(f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ == ਖਾਲੀ_ਸੂਚਕ);
    Local_ਸੂਚਕ<UVector> lp_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ(
        ਨਵਾਂ UVector(uprv_deleteUObject, uhash_compareUnicodeString, ਅੰਕ_Char_Classes + 2, ਸਥਿਤੀ), ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ = lp_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ.orphan();
    ਲਈ (int32_t ਪੰਕਤੀ=0; ਪੰਕਤੀ<ਅੰਕ_Char_Classes + 2; ++ਪੰਕਤੀ) {
        Local_ਸੂਚਕ<UnicodeString> lpString(ਨਵਾਂ UnicodeString(ਅੰਕ_Char_Classes, 0, ਅੰਕ_Char_Classes+4), ਸਥਿਤੀ);
        f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->adoptElement(lpString.orphan(), ਸਥਿਤੀ);
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &ਸ਼ੁਰੂ_State = *static_cast<UnicodeString *>(f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->elementAt(1));
    ਲਈ (int32_t charClass=0; charClass < ਅੰਕ_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        ਸ਼ੁਰੂ_State.ਸੈੱਟ_Char_At(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    ਲਈ (int32_t ਪੰਕਤੀ=2; ਪੰਕਤੀ<ਅੰਕ_Char_Classes+2; ++ਪੰਕਤੀ) {
        UnicodeString &ਪੰਕਤੀ_State = *static_cast<UnicodeString *>(f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->elementAt(ਪੰਕਤੀ));
        ਪੰਕਤੀ_State = ਸ਼ੁਰੂ_State;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    ਲਈ (int32_t pairIdx=0; pairIdx<ਸੁਰੱਖਿਅਤ_ਜੋੜੇ.length(); pairIdx+=2) {
        int32_t c1 = ਸੁਰੱਖਿਅਤ_ਜੋੜੇ.charAt(pairIdx);
        int32_t c2 = ਸੁਰੱਖਿਅਤ_ਜੋੜੇ.charAt(pairIdx + 1);

        UnicodeString &ਪੰਕਤੀ_State = *static_cast<UnicodeString *>(f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->elementAt(c2 + 2));
        ਪੰਕਤੀ_State.ਸੈੱਟ_Char_At(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair ਹਾਲਤਾਂ = {1, 0};
    ਜਦੋਂ (ਲੱਭੋ_Duplicate_ਸੁਰੱਖਿਅਤ_State(&ਹਾਲਤਾਂ)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        remove_ਸੁਰੱਖਿਅਤ_State(ਹਾਲਤਾਂ);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ_ਮਾਪ() ਅਚਲ {
    int32_t    size = 0;
    int32_t    ਅੰਕ_ਪੰਕਤੀਆਂ;
    int32_t    ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ;
    int32_t    ਪੰਕਤੀ_ਮਾਪ;

    ਜੇ (f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    }

    size    = offsetof(RBBIState_ਸਾਰਣੀ, f_ਸਾਰਣੀ_ਅੰਕੜੇ);    // The header, with no rows to the table.

    ਅੰਕ_ਪੰਕਤੀਆਂ = f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->size();
    ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ = fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories();

    ਜੇ (use8_ਬਿੱਟ_For_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ()) {
        ਪੰਕਤੀ_ਮਾਪ = offsetof(RBBIState_ਸਾਰਣੀ_Row8, f_ਅਗਲਾ_State) + ਆਕਾਰ(int8_t)*ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ;
    } ਨਹੀਂਤਾਂ {
        ਪੰਕਤੀ_ਮਾਪ = offsetof(RBBIState_ਸਾਰਣੀ_Row16, f_ਅਗਲਾ_State) + ਆਕਾਰ(int16_t)*ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ;
    }
    size   += ਅੰਕ_ਪੰਕਤੀਆਂ * ਪੰਕਤੀ_ਮਾਪ;
    ਵਾਪਸ size;
}

ਤਾਰਕਿਕ RBBITableBuilder::use8_ਬਿੱਟ_For_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ() ਅਚਲ {
    ਵਾਪਸ f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->size() <= k_Max_State_For8_ਬਿੱਟ_ਸਾਰਣੀ;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBITableBuilder::export_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ(ਖਾਲੀ *where) {
    RBBIState_ਸਾਰਣੀ* ਸਾਰਣੀ = static_cast<RBBIState_ਸਾਰਣੀ*>(where);
    uint32_t           state;
    ਪੂਰਨ                ਖਾਨਾ;

    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ) || f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    int32_t cat_ਗਿਣਤੀ = fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories();
    ਜੇ (cat_ਗਿਣਤੀ > 0x7fff ||
            f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->size() > 0x7fff) {
        *f_ਸਥਿਤੀ = U_BRK_ਅੰਦਰੂਨੀ_ERROR;
        ਵਾਪਸ;
    }

    ਸਾਰਣੀ->f_ਅੰਕ_ਹਾਲਤਾਂ = f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->size();
    ਸਾਰਣੀ->fFlags     = 0;
    ਜੇ (use8_ਬਿੱਟ_For_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ()) {
        ਸਾਰਣੀ->f_ਪੰਕਤੀ_Len    = offsetof(RBBIState_ਸਾਰਣੀ_Row8, f_ਅਗਲਾ_State) + ਆਕਾਰ(uint8_t) * cat_ਗਿਣਤੀ;
        ਸਾਰਣੀ->fFlags  |= RBBI_8_ਬਿੱਟ_ਪੰਕਤੀਆਂ;
    } ਨਹੀਂਤਾਂ {
        ਸਾਰਣੀ->f_ਪੰਕਤੀ_Len    = offsetof(RBBIState_ਸਾਰਣੀ_Row16, f_ਅਗਲਾ_State) + ਆਕਾਰ(int16_t) * cat_ਗਿਣਤੀ;
    }

    ਲਈ (state=0; state<ਸਾਰਣੀ->f_ਅੰਕ_ਹਾਲਤਾਂ; state++) {
        UnicodeString* ਪੰਕਤੀ_String = static_cast<UnicodeString*>(f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->elementAt(state));
        RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ* ਪੰਕਤੀ = reinterpret_cast<RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ*>(ਸਾਰਣੀ->f_ਸਾਰਣੀ_ਅੰਕੜੇ + state * ਸਾਰਣੀ->f_ਪੰਕਤੀ_Len);
        ਜੇ (use8_ਬਿੱਟ_For_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ()) {
            RBBIState_ਸਾਰਣੀ_Row8* r8 = reinterpret_cast<RBBIState_ਸਾਰਣੀ_Row8*>(ਪੰਕਤੀ);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->f_ਟੈਗ_ਸੂਚੀ_Idx    = 0;
            ਲਈ (ਖਾਨਾ=0; ਖਾਨਾ<cat_ਗਿਣਤੀ; ਖਾਨਾ++) {
                U_ASSERT(ਪੰਕਤੀ_String->charAt(ਖਾਨਾ) <= k_Max_State_For8_ਬਿੱਟ_ਸਾਰਣੀ);
                r8->f_ਅਗਲਾ_State[ਖਾਨਾ] = static_cast<uint8_t>(ਪੰਕਤੀ_String->charAt(ਖਾਨਾ));
            }
        } ਨਹੀਂਤਾਂ {
            ਪੰਕਤੀ->r16.fAccepting = 0;
            ਪੰਕਤੀ->r16.fLookAhead = 0;
            ਪੰਕਤੀ->r16.f_ਟੈਗ_ਸੂਚੀ_Idx    = 0;
            ਲਈ (ਖਾਨਾ=0; ਖਾਨਾ<cat_ਗਿਣਤੀ; ਖਾਨਾ++) {
                ਪੰਕਤੀ->r16.f_ਅਗਲਾ_State[ਖਾਨਾ] = ਪੰਕਤੀ_String->charAt(ਖਾਨਾ);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBITableBuilder::print_ਸੈੱਟ(UVector *s) {
    int32_t  i;
    ਲਈ (i=0; i<s->size(); i++) {
        ਅਚਲ RBBINode *v = static_cast<ਅਚਲ RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==ਖਾਲੀ_ਸੂਚਕ? -1 : v->f_Serial_ਅੰਕ);
    }
    RBBIDebugPrintf("\n");
}
#ਜੇ_ਅੰਤ


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBITableBuilder::print_ਹਾਲਤਾਂ() {
    ਪੂਰਨ     c;    // input "character"
    ਪੂਰਨ     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    ਲਈ (c=0; c<fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    ਲਈ (c=0; c<fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    ਲਈ (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->f_ਟੈਗ_ਸੂਚੀ_Idx);
        ਲਈ (c=0; c<fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#ਜੇ_ਅੰਤ


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBITableBuilder::print_ਉਲਟ_ਸਾਰਣੀ() {
    ਪੂਰਨ     c;    // input "character"
    ਪੂਰਨ     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    ਜੇ (f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ == ਖਾਲੀ_ਸੂਚਕ) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        ਵਾਪਸ;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    ਲਈ (c=0; c<fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    ਲਈ (c=0; c<fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    ਲਈ (n=0; n<f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->size(); n++) {
        UnicodeString *ਪੰਕਤੀ_String = (UnicodeString *)f_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        ਲਈ (c=0; c<fRB->f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", ਪੰਕਤੀ_String->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#ਜੇ_ਅੰਤ



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBITableBuilder::print_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ() {
    int32_t  this_ਰਿਕਾਰਡ = 0;
    int32_t  ਅਗਲਾ_ਰਿਕਾਰਡ = 0;
    ਪੂਰਨ      i;
    UVector  *tbl = fRB->f_ਨਿਯਮ_ਸਥਿਤੀ_Vals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    ਜਦੋਂ (ਅਗਲਾ_ਰਿਕਾਰਡ < tbl->size()) {
        this_ਰਿਕਾਰਡ = ਅਗਲਾ_ਰਿਕਾਰਡ;
        ਅਗਲਾ_ਰਿਕਾਰਡ = this_ਰਿਕਾਰਡ + tbl->elementAti(this_ਰਿਕਾਰਡ) + 1;
        RBBIDebugPrintf("%4d   ", this_ਰਿਕਾਰਡ);
        ਲਈ (i=this_ਰਿਕਾਰਡ+1; i<ਅਗਲਾ_ਰਿਕਾਰਡ; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#ਜੇ_ਅੰਤ


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(ਪੂਰਨ ਆਖਰੀ_ਨਿਵੇਸ਼_ਚਿੰਨ੍ਹ, UError_ਕੋਡ *f_ਸਥਿਤੀ) {
    fMarked    = ਝੂਠ;
    fAccepting = 0;
    fLookAhead = 0;
    f_ਟੈਗ_ਸੂਚੀ_Idx   = 0;
    f_ਟੈਗ_Vals   = ਖਾਲੀ_ਸੂਚਕ;
    fPositions = ਖਾਲੀ_ਸੂਚਕ;
    fDtran     = ਖਾਲੀ_ਸੂਚਕ;

    fDtran     = ਨਵਾਂ UVector32(ਆਖਰੀ_ਨਿਵੇਸ਼_ਚਿੰਨ੍ਹ+1, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (fDtran == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    fDtran->ਸੈੱਟ_ਮਾਪ(ਆਖਰੀ_ਨਿਵੇਸ਼_ਚਿੰਨ੍ਹ+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    ਮਿਟਾਓ       fPositions;
    ਮਿਟਾਓ       fDtran;
    ਮਿਟਾਓ       f_ਟੈਗ_Vals;
    fPositions = ਖਾਲੀ_ਸੂਚਕ;
    fDtran     = ਖਾਲੀ_ਸੂਚਕ;
    f_ਟੈਗ_Vals   = ਖਾਲੀ_ਸੂਚਕ;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
