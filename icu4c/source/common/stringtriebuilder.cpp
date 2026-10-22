// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  stringtriebuilder.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010dec24
*   created by: Markus W. Scherer
*/

#അന്തര്ഭാവ "utypeinfo.h"  // for 'typeid' to work
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/stringtriebuilder.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "uhash.h"

U_CDECL_BEGIN

സ്ഥിതം int32_t U_CALLCONV
hash_String_Trie_കണ്ണി(സ്ഥിരം UHashTok താക്കോൽ) {
    തിരികെ icu::StringTrieBuilder::hash_കണ്ണി(താക്കോൽ.pointer);
}

സ്ഥിതം UBool U_CALLCONV
equalStringTrieNodes(സ്ഥിരം UHashTok key1, സ്ഥിരം UHashTok key2) {
    തിരികെ icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

ശൂന്യം
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    nodes=uhash_openSize(hash_String_Trie_കണ്ണി, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    എങ്കിൽ(U_SUCCESS(errorCode)) {
        എങ്കിൽ(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } അല്ലെങ്കിൽ {
          uhash_set_താക്കോൽ_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

ശൂന്യം
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

ശൂന്യം
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    എങ്കിൽ(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_കണ്ണി(0, elementsLength, 0);
    } അല്ലെങ്കിൽ /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *വേര്=make_കണ്ണി(0, elementsLength, 0, errorCode);
        എങ്കിൽ(U_SUCCESS(errorCode)) {
            വേര്->mark_വലത്_Edges_First(-1);
            വേര്->ലിഖ(*ഇത്);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_കണ്ണി(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_മൂല്യം=അസത്യം;
    int32_t value=0;
    int32_t type;
    എങ്കിൽ(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_മൂല്യം(start++);
        എങ്കിൽ(start==limit) {
            തിരികെ write_മൂല്യം_And_Final(value, സത്യം);  // final-value node
        }
        has_മൂല്യം=സത്യം;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    എങ്കിൽ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_കണ്ണി(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ആകുമ്പോൾ(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            ലിഖ(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } അല്ലെങ്കിൽ {
        // Branch node.
        int32_t length=എണ്ണം_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_കണ്ണി(start, limit, unitIndex, length);
        എങ്കിൽ(--length<getMinLinearMatch()) {
            type=length;
        } അല്ലെങ്കിൽ {
            ലിഖ(length);
            type=0;
        }
    }
    തിരികെ write_മൂല്യം_And_Type(has_മൂല്യം, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_കണ്ണി(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t നടുവ്_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ആകുമ്പോൾ(length>get_Max_Branch_Linear_Sub_കണ്ണി_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        നടുവ്_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_കണ്ണി(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_കണ്ണി_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_കണ്ണി_Length-1];
    int32_t unit_സംഖ്യ=0;
    ചെയ്യുക {
        int32_t i=starts[unit_സംഖ്യ]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_സംഖ്യ]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } ആകുമ്പോൾ(++unit_സംഖ്യ<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_സംഖ്യ]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_കണ്ണി_Length-1];
    ചെയ്യുക {
        --unit_സംഖ്യ;
        എങ്കിൽ(!isFinal[unit_സംഖ്യ]) {
            jumpTargets[unit_സംഖ്യ]=write_കണ്ണി(starts[unit_സംഖ്യ], starts[unit_സംഖ്യ+1], unitIndex+1);
        }
    } ആകുമ്പോൾ(unit_സംഖ്യ>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_സംഖ്യ=length-1;
    write_കണ്ണി(start, limit, unitIndex+1);
    int32_t offset=ലിഖ(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    ആകുമ്പോൾ(--unit_സംഖ്യ>=0) {
        start=starts[unit_സംഖ്യ];
        int32_t value;
        എങ്കിൽ(isFinal[unit_സംഖ്യ]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_മൂല്യം(start);
        } അല്ലെങ്കിൽ {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_സംഖ്യ];
        }
        write_മൂല്യം_And_Final(value, isFinal[unit_സംഖ്യ]);
        offset=ലിഖ(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    ആകുമ്പോൾ(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=ലിഖ(നടുവ്_Units[ltLength]);
    }
    തിരികെ offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_കണ്ണി(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    UBool has_മൂല്യം=അസത്യം;
    int32_t value=0;
    എങ്കിൽ(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_മൂല്യം(start++);
        എങ്കിൽ(start==limit) {
            തിരികെ register_Final_മൂല്യം(value, errorCode);
        }
        has_മൂല്യം=സത്യം;
    }
    Node *കണ്ണി;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    എങ്കിൽ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_കണ്ണി=make_കണ്ണി(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ആകുമ്പോൾ(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            കണ്ണി=create_Linear_Match_കണ്ണി(start, lastUnitIndex, maxLinearMatchLength, next_കണ്ണി);
            next_കണ്ണി=register_കണ്ണി(കണ്ണി, errorCode);
        }
        കണ്ണി=create_Linear_Match_കണ്ണി(start, unitIndex, length, next_കണ്ണി);
    } അല്ലെങ്കിൽ {
        // Branch node.
        int32_t length=എണ്ണം_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_കണ്ണി=make_Branch_Sub_കണ്ണി(start, limit, unitIndex, length, errorCode);
        കണ്ണി=പുതിയ Branch_Head_കണ്ണി(length, sub_കണ്ണി);
    }
    എങ്കിൽ(has_മൂല്യം && കണ്ണി!=nullptr) {
        എങ്കിൽ(match_Nodes_Can_Have_മൂല്യങ്ങൾ()) {
            ((മൂല്യം_കണ്ണി *)കണ്ണി)->set_മൂല്യം(value);
        } അല്ലെങ്കിൽ {
            കണ്ണി=പുതിയ Intermediate_മൂല്യം_കണ്ണി(value, register_കണ്ണി(കണ്ണി, errorCode));
        }
    }
    തിരികെ register_കണ്ണി(കണ്ണി, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_കണ്ണി(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    char16_t നടുവ്_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ആകുമ്പോൾ(length>get_Max_Branch_Linear_Sub_കണ്ണി_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        നടുവ്_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_കണ്ണി(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    List_Branch_കണ്ണി *list_കണ്ണി=പുതിയ List_Branch_കണ്ണി();
    എങ്കിൽ(list_കണ്ണി==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_സംഖ്യ=0;
    ചെയ്യുക {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        എങ്കിൽ(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_കണ്ണി->add(unit, get_Element_മൂല്യം(start));
        } അല്ലെങ്കിൽ {
            list_കണ്ണി->add(unit, make_കണ്ണി(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } ആകുമ്പോൾ(++unit_സംഖ്യ<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    എങ്കിൽ(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_കണ്ണി->add(unit, get_Element_മൂല്യം(start));
    } അല്ലെങ്കിൽ {
        list_കണ്ണി->add(unit, make_കണ്ണി(start, limit, unitIndex+1, errorCode));
    }
    Node *കണ്ണി=register_കണ്ണി(list_കണ്ണി, errorCode);
    // Create the split-branch nodes.
    ആകുമ്പോൾ(ltLength>0) {
        --ltLength;
        കണ്ണി=register_കണ്ണി(
            പുതിയ Split_Branch_കണ്ണി(നടുവ്_Units[ltLength], lessThan[ltLength], കണ്ണി), errorCode);
    }
    തിരികെ കണ്ണി;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_കണ്ണി(Node* new_കണ്ണി, UErrorCode& errorCode) U_LIFETIME_BOUND {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        delete new_കണ്ണി;
        തിരികെ nullptr;
    }
    എങ്കിൽ(new_കണ്ണി==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    സ്ഥിരം UHashElement *old=uhash_find(nodes, new_കണ്ണി);
    എങ്കിൽ(old!=nullptr) {
        delete new_കണ്ണി;
        തിരികെ static_cast<Node*>(old->താക്കോൽ.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#എങ്കിൽ U_DEBUG
    int32_t old_മൂല്യം=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_കണ്ണി, 1, &errorCode);
    U_ASSERT(old_മൂല്യം==0);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        delete new_കണ്ണി;
        തിരികെ nullptr;
    }
    തിരികെ new_കണ്ണി;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_മൂല്യം(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    Final_മൂല്യം_കണ്ണി താക്കോൽ(value);
    സ്ഥിരം UHashElement *old=uhash_find(nodes, &താക്കോൽ);
    എങ്കിൽ(old!=nullptr) {
        തിരികെ static_cast<Node*>(old->താക്കോൽ.pointer);
    }
    Node *new_കണ്ണി=പുതിയ Final_മൂല്യം_കണ്ണി(value);
    എങ്കിൽ(new_കണ്ണി==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#എങ്കിൽ U_DEBUG
    int32_t old_മൂല്യം=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_കണ്ണി, 1, &errorCode);
    U_ASSERT(old_മൂല്യം==0);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        delete new_കണ്ണി;
        തിരികെ nullptr;
    }
    തിരികെ new_കണ്ണി;
}

int32_t
StringTrieBuilder::hash_കണ്ണി(സ്ഥിരം ശൂന്യം *കണ്ണി) {
    തിരികെ static_cast<സ്ഥിരം Node*>(കണ്ണി)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    തിരികെ *static_cast<സ്ഥിരം Node*>(left) == *static_cast<സ്ഥിരം Node*>(വലത്);
}

തർക്കം
StringTrieBuilder::Node::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    തിരികെ ഇത്==&other || (typeid(*ഇത്)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) {
    എങ്കിൽ(offset==0) {
        offset=edge_സംഖ്യ;
    }
    തിരികെ edge_സംഖ്യ;
}

തർക്കം
StringTrieBuilder::Final_മൂല്യം_കണ്ണി::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(!Node::operator==(other)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം Final_മൂല്യം_കണ്ണി &o=static_cast<സ്ഥിരം Final_മൂല്യം_കണ്ണി &>(other);
    തിരികെ value==o.value;
}

ശൂന്യം
StringTrieBuilder::Final_മൂല്യം_കണ്ണി::ലിഖ(StringTrieBuilder &builder) {
    offset=builder.write_മൂല്യം_And_Final(value, സത്യം);
}

തർക്കം
StringTrieBuilder::മൂല്യം_കണ്ണി::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(!Node::operator==(other)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം മൂല്യം_കണ്ണി &o=static_cast<സ്ഥിരം മൂല്യം_കണ്ണി &>(other);
    തിരികെ has_മൂല്യം==o.has_മൂല്യം && (!has_മൂല്യം || value==o.value);
}

തർക്കം
StringTrieBuilder::Intermediate_മൂല്യം_കണ്ണി::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(!മൂല്യം_കണ്ണി::operator==(other)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം Intermediate_മൂല്യം_കണ്ണി &o=static_cast<സ്ഥിരം Intermediate_മൂല്യം_കണ്ണി &>(other);
    തിരികെ next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_മൂല്യം_കണ്ണി::mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) {
    എങ്കിൽ(offset==0) {
        offset=edge_സംഖ്യ=next->mark_വലത്_Edges_First(edge_സംഖ്യ);
    }
    തിരികെ edge_സംഖ്യ;
}

ശൂന്യം
StringTrieBuilder::Intermediate_മൂല്യം_കണ്ണി::ലിഖ(StringTrieBuilder &builder) {
    next->ലിഖ(builder);
    offset=builder.write_മൂല്യം_And_Final(value, അസത്യം);
}

തർക്കം
StringTrieBuilder::Linear_Match_കണ്ണി::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(!മൂല്യം_കണ്ണി::operator==(other)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം Linear_Match_കണ്ണി &o=static_cast<സ്ഥിരം Linear_Match_കണ്ണി &>(other);
    തിരികെ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_കണ്ണി::mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) {
    എങ്കിൽ(offset==0) {
        offset=edge_സംഖ്യ=next->mark_വലത്_Edges_First(edge_സംഖ്യ);
    }
    തിരികെ edge_സംഖ്യ;
}

തർക്കം
StringTrieBuilder::List_Branch_കണ്ണി::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(!Node::operator==(other)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം List_Branch_കണ്ണി &o=static_cast<സ്ഥിരം List_Branch_കണ്ണി &>(other);
    വേണ്ടി(int32_t i=0; i<length; ++i) {
        എങ്കിൽ(units[i]!=o.units[i] || മൂല്യങ്ങൾ[i]!=o.മൂല്യങ്ങൾ[i] || equal[i]!=o.equal[i]) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}

int32_t
StringTrieBuilder::List_Branch_കണ്ണി::mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) {
    എങ്കിൽ(offset==0) {
        first_Edge_സംഖ്യ=edge_സംഖ്യ;
        int32_t step=0;
        int32_t i=length;
        ചെയ്യുക {
            Node *edge=equal[--i];
            എങ്കിൽ(edge!=nullptr) {
                edge_സംഖ്യ=edge->mark_വലത്_Edges_First(edge_സംഖ്യ-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } ആകുമ്പോൾ(i>0);
        offset=edge_സംഖ്യ;
    }
    തിരികെ edge_സംഖ്യ;
}

ശൂന്യം
StringTrieBuilder::List_Branch_കണ്ണി::ലിഖ(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_സംഖ്യ=length-1;
    Node *വലത്_Edge=equal[unit_സംഖ്യ];
    int32_t വലത്_Edge_സംഖ്യ= വലത്_Edge==nullptr ? first_Edge_സംഖ്യ : വലത്_Edge->getOffset();
    ചെയ്യുക {
        --unit_സംഖ്യ;
        എങ്കിൽ(equal[unit_സംഖ്യ]!=nullptr) {
            equal[unit_സംഖ്യ]->write_Unless_Inside_വലത്_Edge(first_Edge_സംഖ്യ, വലത്_Edge_സംഖ്യ, builder);
        }
    } ആകുമ്പോൾ(unit_സംഖ്യ>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_സംഖ്യ=length-1;
    എങ്കിൽ(വലത്_Edge==nullptr) {
        builder.write_മൂല്യം_And_Final(മൂല്യങ്ങൾ[unit_സംഖ്യ], സത്യം);
    } അല്ലെങ്കിൽ {
        വലത്_Edge->ലിഖ(builder);
    }
    offset=builder.ലിഖ(units[unit_സംഖ്യ]);
    // Write the rest of this node's unit-value pairs.
    ആകുമ്പോൾ(--unit_സംഖ്യ>=0) {
        int32_t value;
        UBool isFinal;
        എങ്കിൽ(equal[unit_സംഖ്യ]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=മൂല്യങ്ങൾ[unit_സംഖ്യ];
            isFinal=സത്യം;
        } അല്ലെങ്കിൽ {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_സംഖ്യ]->getOffset()>0);
            value=offset-equal[unit_സംഖ്യ]->getOffset();
            isFinal=അസത്യം;
        }
        builder.write_മൂല്യം_And_Final(value, isFinal);
        offset=builder.ലിഖ(units[unit_സംഖ്യ]);
    }
}

തർക്കം
StringTrieBuilder::Split_Branch_കണ്ണി::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(!Node::operator==(other)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം Split_Branch_കണ്ണി &o=static_cast<സ്ഥിരം Split_Branch_കണ്ണി &>(other);
    തിരികെ unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_കണ്ണി::mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) {
    എങ്കിൽ(offset==0) {
        first_Edge_സംഖ്യ=edge_സംഖ്യ;
        edge_സംഖ്യ=greaterOrEqual->mark_വലത്_Edges_First(edge_സംഖ്യ);
        offset=edge_സംഖ്യ=lessThan->mark_വലത്_Edges_First(edge_സംഖ്യ-1);
    }
    തിരികെ edge_സംഖ്യ;
}

ശൂന്യം
StringTrieBuilder::Split_Branch_കണ്ണി::ലിഖ(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_വലത്_Edge(first_Edge_സംഖ്യ, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->ലിഖ(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.ലിഖ(unit);
}

തർക്കം
StringTrieBuilder::Branch_Head_കണ്ണി::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(!മൂല്യം_കണ്ണി::operator==(other)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം Branch_Head_കണ്ണി &o=static_cast<സ്ഥിരം Branch_Head_കണ്ണി &>(other);
    തിരികെ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_കണ്ണി::mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) {
    എങ്കിൽ(offset==0) {
        offset=edge_സംഖ്യ=next->mark_വലത്_Edges_First(edge_സംഖ്യ);
    }
    തിരികെ edge_സംഖ്യ;
}

ശൂന്യം
StringTrieBuilder::Branch_Head_കണ്ണി::ലിഖ(StringTrieBuilder &builder) {
    next->ലിഖ(builder);
    എങ്കിൽ(length<=builder.getMinLinearMatch()) {
        offset=builder.write_മൂല്യം_And_Type(has_മൂല്യം, value, length-1);
    } അല്ലെങ്കിൽ {
        builder.ലിഖ(length-1);
        offset=builder.write_മൂല്യം_And_Type(has_മൂല്യം, value, 0);
    }
}

U_NAMESPACE_END
