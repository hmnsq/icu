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

#ଅନ୍ତର୍ଭାଵ "utypeinfo.h"  // for 'typeid' to work
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringtriebuilder.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"

U_CDECL_BEGIN

ସ୍ଥିର int32_t U_CALLCONV
hash_String_Trie_ଗଣ୍ଠି(ଅଚଳ UHashTok ଚାବି) {
    ଫେରନ୍ତୁ icu::StringTrieBuilder::hash_ଗଣ୍ଠି(ଚାବି.pointer);
}

ସ୍ଥିର UBool U_CALLCONV
equalStringTrieNodes(ଅଚଳ UHashTok key1, ଅଚଳ UHashTok key2) {
    ଫେରନ୍ତୁ icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

ଶୂନ୍ୟ
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    nodes=uhash_openSize(hash_String_Trie_ଗଣ୍ଠି, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    ଯଦି(U_SUCCESS(errorCode)) {
        ଯଦି(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା {
          uhash_set_ଚାବି_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

ଶୂନ୍ୟ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

ଶୂନ୍ୟ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    ଯଦି(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_ଗଣ୍ଠି(0, elementsLength, 0);
    } ଅନ୍ୟଥା /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *ମୂଳ=make_ଗଣ୍ଠି(0, elementsLength, 0, errorCode);
        ଯଦି(U_SUCCESS(errorCode)) {
            ମୂଳ->mark_ଡାହାଣ_Edges_First(-1);
            ମୂଳ->ଲିଖ(*ଏହି);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_ଗଣ୍ଠି(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_ମୂଲ୍ୟ=ମିଥ୍ୟା;
    int32_t value=0;
    int32_t type;
    ଯଦି(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_ମୂଲ୍ୟ(start++);
        ଯଦି(start==limit) {
            ଫେରନ୍ତୁ write_ମୂଲ୍ୟ_And_Final(value, ସତ୍ୟ);  // final-value node
        }
        has_ମୂଲ୍ୟ=ସତ୍ୟ;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    ଯଦି(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_ଗଣ୍ଠି(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ଯେତେବେଳେ(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            ଲିଖ(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } ଅନ୍ୟଥା {
        // Branch node.
        int32_t length=ଗଣନା_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_ଗଣ୍ଠି(start, limit, unitIndex, length);
        ଯଦି(--length<getMinLinearMatch()) {
            type=length;
        } ଅନ୍ୟଥା {
            ଲିଖ(length);
            type=0;
        }
    }
    ଫେରନ୍ତୁ write_ମୂଲ୍ୟ_And_Type(has_ମୂଲ୍ୟ, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_ଗଣ୍ଠି(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t ମଧ୍ୟ_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ଯେତେବେଳେ(length>get_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        ମଧ୍ୟ_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_ଗଣ୍ଠି(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length-1];
    int32_t unit_ସଂଖ୍ୟା=0;
    କର {
        int32_t i=starts[unit_ସଂଖ୍ୟା]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_ସଂଖ୍ୟା]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } ଯେତେବେଳେ(++unit_ସଂଖ୍ୟା<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_ସଂଖ୍ୟା]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length-1];
    କର {
        --unit_ସଂଖ୍ୟା;
        ଯଦି(!isFinal[unit_ସଂଖ୍ୟା]) {
            jumpTargets[unit_ସଂଖ୍ୟା]=write_ଗଣ୍ଠି(starts[unit_ସଂଖ୍ୟା], starts[unit_ସଂଖ୍ୟା+1], unitIndex+1);
        }
    } ଯେତେବେଳେ(unit_ସଂଖ୍ୟା>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ସଂଖ୍ୟା=length-1;
    write_ଗଣ୍ଠି(start, limit, unitIndex+1);
    int32_t offset=ଲିଖ(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    ଯେତେବେଳେ(--unit_ସଂଖ୍ୟା>=0) {
        start=starts[unit_ସଂଖ୍ୟା];
        int32_t value;
        ଯଦି(isFinal[unit_ସଂଖ୍ୟା]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_ମୂଲ୍ୟ(start);
        } ଅନ୍ୟଥା {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_ସଂଖ୍ୟା];
        }
        write_ମୂଲ୍ୟ_And_Final(value, isFinal[unit_ସଂଖ୍ୟା]);
        offset=ଲିଖ(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    ଯେତେବେଳେ(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=ଲିଖ(ମଧ୍ୟ_Units[ltLength]);
    }
    ଫେରନ୍ତୁ offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_ଗଣ୍ଠି(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    UBool has_ମୂଲ୍ୟ=ମିଥ୍ୟା;
    int32_t value=0;
    ଯଦି(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_ମୂଲ୍ୟ(start++);
        ଯଦି(start==limit) {
            ଫେରନ୍ତୁ register_Final_ମୂଲ୍ୟ(value, errorCode);
        }
        has_ମୂଲ୍ୟ=ସତ୍ୟ;
    }
    Node *ଗଣ୍ଠି;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    ଯଦି(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_ଗଣ୍ଠି=make_ଗଣ୍ଠି(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ଯେତେବେଳେ(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            ଗଣ୍ଠି=create_Linear_Match_ଗଣ୍ଠି(start, lastUnitIndex, maxLinearMatchLength, next_ଗଣ୍ଠି);
            next_ଗଣ୍ଠି=register_ଗଣ୍ଠି(ଗଣ୍ଠି, errorCode);
        }
        ଗଣ୍ଠି=create_Linear_Match_ଗଣ୍ଠି(start, unitIndex, length, next_ଗଣ୍ଠି);
    } ଅନ୍ୟଥା {
        // Branch node.
        int32_t length=ଗଣନା_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_ଗଣ୍ଠି=make_Branch_Sub_ଗଣ୍ଠି(start, limit, unitIndex, length, errorCode);
        ଗଣ୍ଠି=ନୂତନ Branch_Head_ଗଣ୍ଠି(length, sub_ଗଣ୍ଠି);
    }
    ଯଦି(has_ମୂଲ୍ୟ && ଗଣ୍ଠି!=nullptr) {
        ଯଦି(match_Nodes_Can_Have_ମୂଲ୍ୟଗୁଡ଼ିକ()) {
            ((ମୂଲ୍ୟ_ଗଣ୍ଠି *)ଗଣ୍ଠି)->set_ମୂଲ୍ୟ(value);
        } ଅନ୍ୟଥା {
            ଗଣ୍ଠି=ନୂତନ Intermediate_ମୂଲ୍ୟ_ଗଣ୍ଠି(value, register_ଗଣ୍ଠି(ଗଣ୍ଠି, errorCode));
        }
    }
    ଫେରନ୍ତୁ register_ଗଣ୍ଠି(ଗଣ୍ଠି, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_ଗଣ୍ଠି(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    char16_t ମଧ୍ୟ_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ଯେତେବେଳେ(length>get_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        ମଧ୍ୟ_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_ଗଣ୍ଠି(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    List_Branch_ଗଣ୍ଠି *list_ଗଣ୍ଠି=ନୂତନ List_Branch_ଗଣ୍ଠି();
    ଯଦି(list_ଗଣ୍ଠି==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_ସଂଖ୍ୟା=0;
    କର {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        ଯଦି(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_ଗଣ୍ଠି->add(unit, get_Element_ମୂଲ୍ୟ(start));
        } ଅନ୍ୟଥା {
            list_ଗଣ୍ଠି->add(unit, make_ଗଣ୍ଠି(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } ଯେତେବେଳେ(++unit_ସଂଖ୍ୟା<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    ଯଦି(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_ଗଣ୍ଠି->add(unit, get_Element_ମୂଲ୍ୟ(start));
    } ଅନ୍ୟଥା {
        list_ଗଣ୍ଠି->add(unit, make_ଗଣ୍ଠି(start, limit, unitIndex+1, errorCode));
    }
    Node *ଗଣ୍ଠି=register_ଗଣ୍ଠି(list_ଗଣ୍ଠି, errorCode);
    // Create the split-branch nodes.
    ଯେତେବେଳେ(ltLength>0) {
        --ltLength;
        ଗଣ୍ଠି=register_ଗଣ୍ଠି(
            ନୂତନ Split_Branch_ଗଣ୍ଠି(ମଧ୍ୟ_Units[ltLength], lessThan[ltLength], ଗଣ୍ଠି), errorCode);
    }
    ଫେରନ୍ତୁ ଗଣ୍ଠି;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_ଗଣ୍ଠି(Node* new_ଗଣ୍ଠି, UErrorCode& errorCode) U_LIFETIME_BOUND {
    ଯଦି(U_FAILURE(errorCode)) {
        delete new_ଗଣ୍ଠି;
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(new_ଗଣ୍ଠି==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ UHashElement *old=uhash_find(nodes, new_ଗଣ୍ଠି);
    ଯଦି(old!=nullptr) {
        delete new_ଗଣ୍ଠି;
        ଫେରନ୍ତୁ static_cast<Node*>(old->ଚାବି.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ଯଦି U_DEBUG
    int32_t old_ମୂଲ୍ୟ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_ଗଣ୍ଠି, 1, &errorCode);
    U_ASSERT(old_ମୂଲ୍ୟ==0);
    ଯଦି(U_FAILURE(errorCode)) {
        delete new_ଗଣ୍ଠି;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ new_ଗଣ୍ଠି;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_ମୂଲ୍ୟ(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    Final_ମୂଲ୍ୟ_ଗଣ୍ଠି ଚାବି(value);
    ଅଚଳ UHashElement *old=uhash_find(nodes, &ଚାବି);
    ଯଦି(old!=nullptr) {
        ଫେରନ୍ତୁ static_cast<Node*>(old->ଚାବି.pointer);
    }
    Node *new_ଗଣ୍ଠି=ନୂତନ Final_ମୂଲ୍ୟ_ଗଣ୍ଠି(value);
    ଯଦି(new_ଗଣ୍ଠି==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ଯଦି U_DEBUG
    int32_t old_ମୂଲ୍ୟ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_ଗଣ୍ଠି, 1, &errorCode);
    U_ASSERT(old_ମୂଲ୍ୟ==0);
    ଯଦି(U_FAILURE(errorCode)) {
        delete new_ଗଣ୍ଠି;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ new_ଗଣ୍ଠି;
}

int32_t
StringTrieBuilder::hash_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *ଗଣ୍ଠି) {
    ଫେରନ୍ତୁ static_cast<ଅଚଳ Node*>(ଗଣ୍ଠି)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    ଫେରନ୍ତୁ *static_cast<ଅଚଳ Node*>(left) == *static_cast<ଅଚଳ Node*>(ଡାହାଣ);
}

ତାର୍କିକ
StringTrieBuilder::Node::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଫେରନ୍ତୁ ଏହି==&other || (typeid(*ଏହି)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) {
    ଯଦି(offset==0) {
        offset=edge_ସଂଖ୍ୟା;
    }
    ଫେରନ୍ତୁ edge_ସଂଖ୍ୟା;
}

ତାର୍କିକ
StringTrieBuilder::Final_ମୂଲ୍ୟ_ଗଣ୍ଠି::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(!Node::operator==(other)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ Final_ମୂଲ୍ୟ_ଗଣ୍ଠି &o=static_cast<ଅଚଳ Final_ମୂଲ୍ୟ_ଗଣ୍ଠି &>(other);
    ଫେରନ୍ତୁ value==o.value;
}

ଶୂନ୍ୟ
StringTrieBuilder::Final_ମୂଲ୍ୟ_ଗଣ୍ଠି::ଲିଖ(StringTrieBuilder &builder) {
    offset=builder.write_ମୂଲ୍ୟ_And_Final(value, ସତ୍ୟ);
}

ତାର୍କିକ
StringTrieBuilder::ମୂଲ୍ୟ_ଗଣ୍ଠି::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(!Node::operator==(other)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ ମୂଲ୍ୟ_ଗଣ୍ଠି &o=static_cast<ଅଚଳ ମୂଲ୍ୟ_ଗଣ୍ଠି &>(other);
    ଫେରନ୍ତୁ has_ମୂଲ୍ୟ==o.has_ମୂଲ୍ୟ && (!has_ମୂଲ୍ୟ || value==o.value);
}

ତାର୍କିକ
StringTrieBuilder::Intermediate_ମୂଲ୍ୟ_ଗଣ୍ଠି::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(!ମୂଲ୍ୟ_ଗଣ୍ଠି::operator==(other)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ Intermediate_ମୂଲ୍ୟ_ଗଣ୍ଠି &o=static_cast<ଅଚଳ Intermediate_ମୂଲ୍ୟ_ଗଣ୍ଠି &>(other);
    ଫେରନ୍ତୁ next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_ମୂଲ୍ୟ_ଗଣ୍ଠି::mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) {
    ଯଦି(offset==0) {
        offset=edge_ସଂଖ୍ୟା=next->mark_ଡାହାଣ_Edges_First(edge_ସଂଖ୍ୟା);
    }
    ଫେରନ୍ତୁ edge_ସଂଖ୍ୟା;
}

ଶୂନ୍ୟ
StringTrieBuilder::Intermediate_ମୂଲ୍ୟ_ଗଣ୍ଠି::ଲିଖ(StringTrieBuilder &builder) {
    next->ଲିଖ(builder);
    offset=builder.write_ମୂଲ୍ୟ_And_Final(value, ମିଥ୍ୟା);
}

ତାର୍କିକ
StringTrieBuilder::Linear_Match_ଗଣ୍ଠି::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(!ମୂଲ୍ୟ_ଗଣ୍ଠି::operator==(other)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ Linear_Match_ଗଣ୍ଠି &o=static_cast<ଅଚଳ Linear_Match_ଗଣ୍ଠି &>(other);
    ଫେରନ୍ତୁ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_ଗଣ୍ଠି::mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) {
    ଯଦି(offset==0) {
        offset=edge_ସଂଖ୍ୟା=next->mark_ଡାହାଣ_Edges_First(edge_ସଂଖ୍ୟା);
    }
    ଫେରନ୍ତୁ edge_ସଂଖ୍ୟା;
}

ତାର୍କିକ
StringTrieBuilder::List_Branch_ଗଣ୍ଠି::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(!Node::operator==(other)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ List_Branch_ଗଣ୍ଠି &o=static_cast<ଅଚଳ List_Branch_ଗଣ୍ଠି &>(other);
    ପାଇଁ(int32_t i=0; i<length; ++i) {
        ଯଦି(units[i]!=o.units[i] || ମୂଲ୍ୟଗୁଡ଼ିକ[i]!=o.ମୂଲ୍ୟଗୁଡ଼ିକ[i] || equal[i]!=o.equal[i]) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

int32_t
StringTrieBuilder::List_Branch_ଗଣ୍ଠି::mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) {
    ଯଦି(offset==0) {
        first_Edge_ସଂଖ୍ୟା=edge_ସଂଖ୍ୟା;
        int32_t step=0;
        int32_t i=length;
        କର {
            Node *edge=equal[--i];
            ଯଦି(edge!=nullptr) {
                edge_ସଂଖ୍ୟା=edge->mark_ଡାହାଣ_Edges_First(edge_ସଂଖ୍ୟା-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } ଯେତେବେଳେ(i>0);
        offset=edge_ସଂଖ୍ୟା;
    }
    ଫେରନ୍ତୁ edge_ସଂଖ୍ୟା;
}

ଶୂନ୍ୟ
StringTrieBuilder::List_Branch_ଗଣ୍ଠି::ଲିଖ(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_ସଂଖ୍ୟା=length-1;
    Node *ଡାହାଣ_Edge=equal[unit_ସଂଖ୍ୟା];
    int32_t ଡାହାଣ_Edge_ସଂଖ୍ୟା= ଡାହାଣ_Edge==nullptr ? first_Edge_ସଂଖ୍ୟା : ଡାହାଣ_Edge->getOffset();
    କର {
        --unit_ସଂଖ୍ୟା;
        ଯଦି(equal[unit_ସଂଖ୍ୟା]!=nullptr) {
            equal[unit_ସଂଖ୍ୟା]->write_Unless_Inside_ଡାହାଣ_Edge(first_Edge_ସଂଖ୍ୟା, ଡାହାଣ_Edge_ସଂଖ୍ୟା, builder);
        }
    } ଯେତେବେଳେ(unit_ସଂଖ୍ୟା>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ସଂଖ୍ୟା=length-1;
    ଯଦି(ଡାହାଣ_Edge==nullptr) {
        builder.write_ମୂଲ୍ୟ_And_Final(ମୂଲ୍ୟଗୁଡ଼ିକ[unit_ସଂଖ୍ୟା], ସତ୍ୟ);
    } ଅନ୍ୟଥା {
        ଡାହାଣ_Edge->ଲିଖ(builder);
    }
    offset=builder.ଲିଖ(units[unit_ସଂଖ୍ୟା]);
    // Write the rest of this node's unit-value pairs.
    ଯେତେବେଳେ(--unit_ସଂଖ୍ୟା>=0) {
        int32_t value;
        UBool isFinal;
        ଯଦି(equal[unit_ସଂଖ୍ୟା]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=ମୂଲ୍ୟଗୁଡ଼ିକ[unit_ସଂଖ୍ୟା];
            isFinal=ସତ୍ୟ;
        } ଅନ୍ୟଥା {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_ସଂଖ୍ୟା]->getOffset()>0);
            value=offset-equal[unit_ସଂଖ୍ୟା]->getOffset();
            isFinal=ମିଥ୍ୟା;
        }
        builder.write_ମୂଲ୍ୟ_And_Final(value, isFinal);
        offset=builder.ଲିଖ(units[unit_ସଂଖ୍ୟା]);
    }
}

ତାର୍କିକ
StringTrieBuilder::Split_Branch_ଗଣ୍ଠି::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(!Node::operator==(other)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ Split_Branch_ଗଣ୍ଠି &o=static_cast<ଅଚଳ Split_Branch_ଗଣ୍ଠି &>(other);
    ଫେରନ୍ତୁ unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_ଗଣ୍ଠି::mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) {
    ଯଦି(offset==0) {
        first_Edge_ସଂଖ୍ୟା=edge_ସଂଖ୍ୟା;
        edge_ସଂଖ୍ୟା=greaterOrEqual->mark_ଡାହାଣ_Edges_First(edge_ସଂଖ୍ୟା);
        offset=edge_ସଂଖ୍ୟା=lessThan->mark_ଡାହାଣ_Edges_First(edge_ସଂଖ୍ୟା-1);
    }
    ଫେରନ୍ତୁ edge_ସଂଖ୍ୟା;
}

ଶୂନ୍ୟ
StringTrieBuilder::Split_Branch_ଗଣ୍ଠି::ଲିଖ(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ଡାହାଣ_Edge(first_Edge_ସଂଖ୍ୟା, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->ଲିଖ(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.ଲିଖ(unit);
}

ତାର୍କିକ
StringTrieBuilder::Branch_Head_ଗଣ୍ଠି::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(!ମୂଲ୍ୟ_ଗଣ୍ଠି::operator==(other)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ Branch_Head_ଗଣ୍ଠି &o=static_cast<ଅଚଳ Branch_Head_ଗଣ୍ଠି &>(other);
    ଫେରନ୍ତୁ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_ଗଣ୍ଠି::mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) {
    ଯଦି(offset==0) {
        offset=edge_ସଂଖ୍ୟା=next->mark_ଡାହାଣ_Edges_First(edge_ସଂଖ୍ୟା);
    }
    ଫେରନ୍ତୁ edge_ସଂଖ୍ୟା;
}

ଶୂନ୍ୟ
StringTrieBuilder::Branch_Head_ଗଣ୍ଠି::ଲିଖ(StringTrieBuilder &builder) {
    next->ଲିଖ(builder);
    ଯଦି(length<=builder.getMinLinearMatch()) {
        offset=builder.write_ମୂଲ୍ୟ_And_Type(has_ମୂଲ୍ୟ, value, length-1);
    } ଅନ୍ୟଥା {
        builder.ଲିଖ(length-1);
        offset=builder.write_ମୂଲ୍ୟ_And_Type(has_ମୂଲ୍ୟ, value, 0);
    }
}

U_NAMESPACE_END
