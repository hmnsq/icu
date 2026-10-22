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

#अन्तर्भाव "utypeinfo.h"  // for 'typeid' to work
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/stringtriebuilder.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
hash_String_Trie_ग्रन्थिः(ध्रुव UHashTok कुञ्चिका) {
    पटिनिवत्त icu::StringTrieBuilder::hash_ग्रन्थिः(कुञ्चिका.pointer);
}

स्थिर UBool U_CALLCONV
equalStringTrieNodes(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    पटिनिवत्त icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

शून्य
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    nodes=uhash_openSize(hash_String_Trie_ग्रन्थिः, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    सचे(U_SUCCESS(errorCode)) {
        सचे(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अञ्ञथा {
          uhash_set_कुञ्चिका_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

शून्य
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

शून्य
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    सचे(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_ग्रन्थिः(0, elementsLength, 0);
    } अञ्ञथा /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *मूलम्=make_ग्रन्थिः(0, elementsLength, 0, errorCode);
        सचे(U_SUCCESS(errorCode)) {
            मूलम्->mark_दक्षिणः_Edges_First(-1);
            मूलम्->लिख(*एतत्);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_ग्रन्थिः(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_मानम्=मिच्छा;
    int32_t value=0;
    int32_t type;
    सचे(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_मानम्(start++);
        सचे(start==limit) {
            पटिनिवत्त write_मानम्_And_Final(value, सच्च);  // final-value node
        }
        has_मानम्=सच्च;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    सचे(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_ग्रन्थिः(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        याव(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            लिख(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } अञ्ञथा {
        // Branch node.
        int32_t length=गणनम्_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_ग्रन्थिः(start, limit, unitIndex, length);
        सचे(--length<getMinLinearMatch()) {
            type=length;
        } अञ्ञथा {
            लिख(length);
            type=0;
        }
    }
    पटिनिवत्त write_मानम्_And_Type(has_मानम्, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_ग्रन्थिः(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t मध्यः_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    याव(length>get_Max_Branch_Linear_Sub_ग्रन्थिः_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        मध्यः_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_ग्रन्थिः(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_ग्रन्थिः_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_ग्रन्थिः_Length-1];
    int32_t unit_सङ्ख्या=0;
    करोहि {
        int32_t i=starts[unit_सङ्ख्या]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_सङ्ख्या]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } याव(++unit_सङ्ख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_सङ्ख्या]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_ग्रन्थिः_Length-1];
    करोहि {
        --unit_सङ्ख्या;
        सचे(!isFinal[unit_सङ्ख्या]) {
            jumpTargets[unit_सङ्ख्या]=write_ग्रन्थिः(starts[unit_सङ्ख्या], starts[unit_सङ्ख्या+1], unitIndex+1);
        }
    } याव(unit_सङ्ख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_सङ्ख्या=length-1;
    write_ग्रन्थिः(start, limit, unitIndex+1);
    int32_t offset=लिख(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    याव(--unit_सङ्ख्या>=0) {
        start=starts[unit_सङ्ख्या];
        int32_t value;
        सचे(isFinal[unit_सङ्ख्या]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_मानम्(start);
        } अञ्ञथा {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_सङ्ख्या];
        }
        write_मानम्_And_Final(value, isFinal[unit_सङ्ख्या]);
        offset=लिख(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    याव(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=लिख(मध्यः_Units[ltLength]);
    }
    पटिनिवत्त offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_ग्रन्थिः(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    UBool has_मानम्=मिच्छा;
    int32_t value=0;
    सचे(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_मानम्(start++);
        सचे(start==limit) {
            पटिनिवत्त register_Final_मानम्(value, errorCode);
        }
        has_मानम्=सच्च;
    }
    Node *ग्रन्थिः;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    सचे(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_ग्रन्थिः=make_ग्रन्थिः(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        याव(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            ग्रन्थिः=create_Linear_Match_ग्रन्थिः(start, lastUnitIndex, maxLinearMatchLength, next_ग्रन्थिः);
            next_ग्रन्थिः=register_ग्रन्थिः(ग्रन्थिः, errorCode);
        }
        ग्रन्थिः=create_Linear_Match_ग्रन्थिः(start, unitIndex, length, next_ग्रन्थिः);
    } अञ्ञथा {
        // Branch node.
        int32_t length=गणनम्_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_ग्रन्थिः=make_Branch_Sub_ग्रन्थिः(start, limit, unitIndex, length, errorCode);
        ग्रन्थिः=नूतन Branch_Head_ग्रन्थिः(length, sub_ग्रन्थिः);
    }
    सचे(has_मानम् && ग्रन्थिः!=nullptr) {
        सचे(match_Nodes_Can_Have_मानानि()) {
            ((मानम्_ग्रन्थिः *)ग्रन्थिः)->set_मानम्(value);
        } अञ्ञथा {
            ग्रन्थिः=नूतन Intermediate_मानम्_ग्रन्थिः(value, register_ग्रन्थिः(ग्रन्थिः, errorCode));
        }
    }
    पटिनिवत्त register_ग्रन्थिः(ग्रन्थिः, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_ग्रन्थिः(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    char16_t मध्यः_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    याव(length>get_Max_Branch_Linear_Sub_ग्रन्थिः_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        मध्यः_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_ग्रन्थिः(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    List_Branch_ग्रन्थिः *list_ग्रन्थिः=नूतन List_Branch_ग्रन्थिः();
    सचे(list_ग्रन्थिः==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_सङ्ख्या=0;
    करोहि {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        सचे(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_ग्रन्थिः->add(unit, get_Element_मानम्(start));
        } अञ्ञथा {
            list_ग्रन्थिः->add(unit, make_ग्रन्थिः(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } याव(++unit_सङ्ख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    सचे(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_ग्रन्थिः->add(unit, get_Element_मानम्(start));
    } अञ्ञथा {
        list_ग्रन्थिः->add(unit, make_ग्रन्थिः(start, limit, unitIndex+1, errorCode));
    }
    Node *ग्रन्थिः=register_ग्रन्थिः(list_ग्रन्थिः, errorCode);
    // Create the split-branch nodes.
    याव(ltLength>0) {
        --ltLength;
        ग्रन्थिः=register_ग्रन्थिः(
            नूतन Split_Branch_ग्रन्थिः(मध्यः_Units[ltLength], lessThan[ltLength], ग्रन्थिः), errorCode);
    }
    पटिनिवत्त ग्रन्थिः;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_ग्रन्थिः(Node* new_ग्रन्थिः, UErrorCode& errorCode) U_LIFETIME_BOUND {
    सचे(U_FAILURE(errorCode)) {
        delete new_ग्रन्थिः;
        पटिनिवत्त nullptr;
    }
    सचे(new_ग्रन्थिः==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    ध्रुव UHashElement *old=uhash_find(nodes, new_ग्रन्थिः);
    सचे(old!=nullptr) {
        delete new_ग्रन्थिः;
        पटिनिवत्त static_cast<Node*>(old->कुञ्चिका.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#सचे U_DEBUG
    int32_t old_मानम्=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_ग्रन्थिः, 1, &errorCode);
    U_ASSERT(old_मानम्==0);
    सचे(U_FAILURE(errorCode)) {
        delete new_ग्रन्थिः;
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त new_ग्रन्थिः;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_मानम्(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    Final_मानम्_ग्रन्थिः कुञ्चिका(value);
    ध्रुव UHashElement *old=uhash_find(nodes, &कुञ्चिका);
    सचे(old!=nullptr) {
        पटिनिवत्त static_cast<Node*>(old->कुञ्चिका.pointer);
    }
    Node *new_ग्रन्थिः=नूतन Final_मानम्_ग्रन्थिः(value);
    सचे(new_ग्रन्थिः==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#सचे U_DEBUG
    int32_t old_मानम्=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_ग्रन्थिः, 1, &errorCode);
    U_ASSERT(old_मानम्==0);
    सचे(U_FAILURE(errorCode)) {
        delete new_ग्रन्थिः;
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त new_ग्रन्थिः;
}

int32_t
StringTrieBuilder::hash_ग्रन्थिः(ध्रुव शून्य *ग्रन्थिः) {
    पटिनिवत्त static_cast<ध्रुव Node*>(ग्रन्थिः)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    पटिनिवत्त *static_cast<ध्रुव Node*>(left) == *static_cast<ध्रुव Node*>(दक्षिणः);
}

तार्किक
StringTrieBuilder::Node::operator==(ध्रुव Node &other) ध्रुव {
    पटिनिवत्त एतत्==&other || (typeid(*एतत्)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_दक्षिणः_Edges_First(int32_t edge_सङ्ख्या) {
    सचे(offset==0) {
        offset=edge_सङ्ख्या;
    }
    पटिनिवत्त edge_सङ्ख्या;
}

तार्किक
StringTrieBuilder::Final_मानम्_ग्रन्थिः::operator==(ध्रुव Node &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    सचे(!Node::operator==(other)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव Final_मानम्_ग्रन्थिः &o=static_cast<ध्रुव Final_मानम्_ग्रन्थिः &>(other);
    पटिनिवत्त value==o.value;
}

शून्य
StringTrieBuilder::Final_मानम्_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    offset=builder.write_मानम्_And_Final(value, सच्च);
}

तार्किक
StringTrieBuilder::मानम्_ग्रन्थिः::operator==(ध्रुव Node &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    सचे(!Node::operator==(other)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव मानम्_ग्रन्थिः &o=static_cast<ध्रुव मानम्_ग्रन्थिः &>(other);
    पटिनिवत्त has_मानम्==o.has_मानम् && (!has_मानम् || value==o.value);
}

तार्किक
StringTrieBuilder::Intermediate_मानम्_ग्रन्थिः::operator==(ध्रुव Node &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    सचे(!मानम्_ग्रन्थिः::operator==(other)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव Intermediate_मानम्_ग्रन्थिः &o=static_cast<ध्रुव Intermediate_मानम्_ग्रन्थिः &>(other);
    पटिनिवत्त next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_मानम्_ग्रन्थिः::mark_दक्षिणः_Edges_First(int32_t edge_सङ्ख्या) {
    सचे(offset==0) {
        offset=edge_सङ्ख्या=next->mark_दक्षिणः_Edges_First(edge_सङ्ख्या);
    }
    पटिनिवत्त edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::Intermediate_मानम्_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    offset=builder.write_मानम्_And_Final(value, मिच्छा);
}

तार्किक
StringTrieBuilder::Linear_Match_ग्रन्थिः::operator==(ध्रुव Node &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    सचे(!मानम्_ग्रन्थिः::operator==(other)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव Linear_Match_ग्रन्थिः &o=static_cast<ध्रुव Linear_Match_ग्रन्थिः &>(other);
    पटिनिवत्त length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_ग्रन्थिः::mark_दक्षिणः_Edges_First(int32_t edge_सङ्ख्या) {
    सचे(offset==0) {
        offset=edge_सङ्ख्या=next->mark_दक्षिणः_Edges_First(edge_सङ्ख्या);
    }
    पटिनिवत्त edge_सङ्ख्या;
}

तार्किक
StringTrieBuilder::List_Branch_ग्रन्थिः::operator==(ध्रुव Node &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    सचे(!Node::operator==(other)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव List_Branch_ग्रन्थिः &o=static_cast<ध्रुव List_Branch_ग्रन्थिः &>(other);
    अत्थाय(int32_t i=0; i<length; ++i) {
        सचे(units[i]!=o.units[i] || मानानि[i]!=o.मानानि[i] || equal[i]!=o.equal[i]) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त सच्च;
}

int32_t
StringTrieBuilder::List_Branch_ग्रन्थिः::mark_दक्षिणः_Edges_First(int32_t edge_सङ्ख्या) {
    सचे(offset==0) {
        first_Edge_सङ्ख्या=edge_सङ्ख्या;
        int32_t step=0;
        int32_t i=length;
        करोहि {
            Node *edge=equal[--i];
            सचे(edge!=nullptr) {
                edge_सङ्ख्या=edge->mark_दक्षिणः_Edges_First(edge_सङ्ख्या-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } याव(i>0);
        offset=edge_सङ्ख्या;
    }
    पटिनिवत्त edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::List_Branch_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_सङ्ख्या=length-1;
    Node *दक्षिणः_Edge=equal[unit_सङ्ख्या];
    int32_t दक्षिणः_Edge_सङ्ख्या= दक्षिणः_Edge==nullptr ? first_Edge_सङ्ख्या : दक्षिणः_Edge->getOffset();
    करोहि {
        --unit_सङ्ख्या;
        सचे(equal[unit_सङ्ख्या]!=nullptr) {
            equal[unit_सङ्ख्या]->write_Unless_Inside_दक्षिणः_Edge(first_Edge_सङ्ख्या, दक्षिणः_Edge_सङ्ख्या, builder);
        }
    } याव(unit_सङ्ख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_सङ्ख्या=length-1;
    सचे(दक्षिणः_Edge==nullptr) {
        builder.write_मानम्_And_Final(मानानि[unit_सङ्ख्या], सच्च);
    } अञ्ञथा {
        दक्षिणः_Edge->लिख(builder);
    }
    offset=builder.लिख(units[unit_सङ्ख्या]);
    // Write the rest of this node's unit-value pairs.
    याव(--unit_सङ्ख्या>=0) {
        int32_t value;
        UBool isFinal;
        सचे(equal[unit_सङ्ख्या]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=मानानि[unit_सङ्ख्या];
            isFinal=सच्च;
        } अञ्ञथा {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_सङ्ख्या]->getOffset()>0);
            value=offset-equal[unit_सङ्ख्या]->getOffset();
            isFinal=मिच्छा;
        }
        builder.write_मानम्_And_Final(value, isFinal);
        offset=builder.लिख(units[unit_सङ्ख्या]);
    }
}

तार्किक
StringTrieBuilder::Split_Branch_ग्रन्थिः::operator==(ध्रुव Node &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    सचे(!Node::operator==(other)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव Split_Branch_ग्रन्थिः &o=static_cast<ध्रुव Split_Branch_ग्रन्थिः &>(other);
    पटिनिवत्त unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_ग्रन्थिः::mark_दक्षिणः_Edges_First(int32_t edge_सङ्ख्या) {
    सचे(offset==0) {
        first_Edge_सङ्ख्या=edge_सङ्ख्या;
        edge_सङ्ख्या=greaterOrEqual->mark_दक्षिणः_Edges_First(edge_सङ्ख्या);
        offset=edge_सङ्ख्या=lessThan->mark_दक्षिणः_Edges_First(edge_सङ्ख्या-1);
    }
    पटिनिवत्त edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::Split_Branch_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_दक्षिणः_Edge(first_Edge_सङ्ख्या, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->लिख(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.लिख(unit);
}

तार्किक
StringTrieBuilder::Branch_Head_ग्रन्थिः::operator==(ध्रुव Node &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    सचे(!मानम्_ग्रन्थिः::operator==(other)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव Branch_Head_ग्रन्थिः &o=static_cast<ध्रुव Branch_Head_ग्रन्थिः &>(other);
    पटिनिवत्त length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_ग्रन्थिः::mark_दक्षिणः_Edges_First(int32_t edge_सङ्ख्या) {
    सचे(offset==0) {
        offset=edge_सङ्ख्या=next->mark_दक्षिणः_Edges_First(edge_सङ्ख्या);
    }
    पटिनिवत्त edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::Branch_Head_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    सचे(length<=builder.getMinLinearMatch()) {
        offset=builder.write_मानम्_And_Type(has_मानम्, value, length-1);
    } अञ्ञथा {
        builder.लिख(length-1);
        offset=builder.write_मानम्_And_Type(has_मानम्, value, 0);
    }
}

U_NAMESPACE_END
