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
hash_String_Trie_गाँठ(ध्रुव UHashTok कुंजी) {
    प्रत्यागम icu::StringTrieBuilder::hash_गाँठ(कुंजी.pointer);
}

स्थिर UBool U_CALLCONV
equalStringTrieNodes(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    प्रत्यागम icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

शून्य
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    nodes=uhash_openSize(hash_String_Trie_गाँठ, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    यदि(U_SUCCESS(errorCode)) {
        यदि(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
          uhash_set_कुंजी_Deleter(nodes, uprv_deleteUObject);
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
    यदि(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_गाँठ(0, elementsLength, 0);
    } अन्यथा /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *मूल=make_गाँठ(0, elementsLength, 0, errorCode);
        यदि(U_SUCCESS(errorCode)) {
            मूल->mark_दायाँ_Edges_First(-1);
            मूल->लिख(*एतत्);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_गाँठ(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_मूल्य=असत्य;
    int32_t value=0;
    int32_t type;
    यदि(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_मूल्य(start++);
        यदि(start==limit) {
            प्रत्यागम write_मूल्य_And_Final(value, सत्य);  // final-value node
        }
        has_मूल्य=सत्य;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    यदि(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_गाँठ(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        यावत्(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            लिख(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } अन्यथा {
        // Branch node.
        int32_t length=गिनती_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_गाँठ(start, limit, unitIndex, length);
        यदि(--length<getMinLinearMatch()) {
            type=length;
        } अन्यथा {
            लिख(length);
            type=0;
        }
    }
    प्रत्यागम write_मूल्य_And_Type(has_मूल्य, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_गाँठ(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t मध्य_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    यावत्(length>get_Max_Branch_Linear_Sub_गाँठ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        मध्य_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_गाँठ(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_गाँठ_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_गाँठ_Length-1];
    int32_t unit_संख्या=0;
    कुरु {
        int32_t i=starts[unit_संख्या]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_संख्या]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } यावत्(++unit_संख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_संख्या]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_गाँठ_Length-1];
    कुरु {
        --unit_संख्या;
        यदि(!isFinal[unit_संख्या]) {
            jumpTargets[unit_संख्या]=write_गाँठ(starts[unit_संख्या], starts[unit_संख्या+1], unitIndex+1);
        }
    } यावत्(unit_संख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_संख्या=length-1;
    write_गाँठ(start, limit, unitIndex+1);
    int32_t offset=लिख(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    यावत्(--unit_संख्या>=0) {
        start=starts[unit_संख्या];
        int32_t value;
        यदि(isFinal[unit_संख्या]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_मूल्य(start);
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_संख्या];
        }
        write_मूल्य_And_Final(value, isFinal[unit_संख्या]);
        offset=लिख(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    यावत्(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=लिख(मध्य_Units[ltLength]);
    }
    प्रत्यागम offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_गाँठ(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    UBool has_मूल्य=असत्य;
    int32_t value=0;
    यदि(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_मूल्य(start++);
        यदि(start==limit) {
            प्रत्यागम register_Final_मूल्य(value, errorCode);
        }
        has_मूल्य=सत्य;
    }
    Node *गाँठ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    यदि(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_गाँठ=make_गाँठ(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        यावत्(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            गाँठ=create_Linear_Match_गाँठ(start, lastUnitIndex, maxLinearMatchLength, next_गाँठ);
            next_गाँठ=register_गाँठ(गाँठ, errorCode);
        }
        गाँठ=create_Linear_Match_गाँठ(start, unitIndex, length, next_गाँठ);
    } अन्यथा {
        // Branch node.
        int32_t length=गिनती_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_गाँठ=make_Branch_Sub_गाँठ(start, limit, unitIndex, length, errorCode);
        गाँठ=नूतन Branch_Head_गाँठ(length, sub_गाँठ);
    }
    यदि(has_मूल्य && गाँठ!=nullptr) {
        यदि(match_Nodes_Can_Have_मूल्यावली()) {
            ((मूल्य_गाँठ *)गाँठ)->set_मूल्य(value);
        } अन्यथा {
            गाँठ=नूतन Intermediate_मूल्य_गाँठ(value, register_गाँठ(गाँठ, errorCode));
        }
    }
    प्रत्यागम register_गाँठ(गाँठ, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_गाँठ(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    char16_t मध्य_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    यावत्(length>get_Max_Branch_Linear_Sub_गाँठ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        मध्य_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_गाँठ(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    List_Branch_गाँठ *list_गाँठ=नूतन List_Branch_गाँठ();
    यदि(list_गाँठ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_संख्या=0;
    कुरु {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        यदि(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_गाँठ->add(unit, get_Element_मूल्य(start));
        } अन्यथा {
            list_गाँठ->add(unit, make_गाँठ(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } यावत्(++unit_संख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    यदि(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_गाँठ->add(unit, get_Element_मूल्य(start));
    } अन्यथा {
        list_गाँठ->add(unit, make_गाँठ(start, limit, unitIndex+1, errorCode));
    }
    Node *गाँठ=register_गाँठ(list_गाँठ, errorCode);
    // Create the split-branch nodes.
    यावत्(ltLength>0) {
        --ltLength;
        गाँठ=register_गाँठ(
            नूतन Split_Branch_गाँठ(मध्य_Units[ltLength], lessThan[ltLength], गाँठ), errorCode);
    }
    प्रत्यागम गाँठ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_गाँठ(Node* new_गाँठ, UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        delete new_गाँठ;
        प्रत्यागम nullptr;
    }
    यदि(new_गाँठ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    ध्रुव UHashElement *old=uhash_find(nodes, new_गाँठ);
    यदि(old!=nullptr) {
        delete new_गाँठ;
        प्रत्यागम static_cast<Node*>(old->कुंजी.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#यदि U_DEBUG
    int32_t old_मूल्य=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_गाँठ, 1, &errorCode);
    U_ASSERT(old_मूल्य==0);
    यदि(U_FAILURE(errorCode)) {
        delete new_गाँठ;
        प्रत्यागम nullptr;
    }
    प्रत्यागम new_गाँठ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_मूल्य(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    Final_मूल्य_गाँठ कुंजी(value);
    ध्रुव UHashElement *old=uhash_find(nodes, &कुंजी);
    यदि(old!=nullptr) {
        प्रत्यागम static_cast<Node*>(old->कुंजी.pointer);
    }
    Node *new_गाँठ=नूतन Final_मूल्य_गाँठ(value);
    यदि(new_गाँठ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#यदि U_DEBUG
    int32_t old_मूल्य=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_गाँठ, 1, &errorCode);
    U_ASSERT(old_मूल्य==0);
    यदि(U_FAILURE(errorCode)) {
        delete new_गाँठ;
        प्रत्यागम nullptr;
    }
    प्रत्यागम new_गाँठ;
}

int32_t
StringTrieBuilder::hash_गाँठ(ध्रुव शून्य *गाँठ) {
    प्रत्यागम static_cast<ध्रुव Node*>(गाँठ)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ध्रुव शून्य *left, ध्रुव शून्य *दायाँ) {
    प्रत्यागम *static_cast<ध्रुव Node*>(left) == *static_cast<ध्रुव Node*>(दायाँ);
}

तार्किक
StringTrieBuilder::Node::operator==(ध्रुव Node &other) ध्रुव {
    प्रत्यागम एतत्==&other || (typeid(*एतत्)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    यदि(offset==0) {
        offset=edge_संख्या;
    }
    प्रत्यागम edge_संख्या;
}

तार्किक
StringTrieBuilder::Final_मूल्य_गाँठ::operator==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!Node::operator==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव Final_मूल्य_गाँठ &o=static_cast<ध्रुव Final_मूल्य_गाँठ &>(other);
    प्रत्यागम value==o.value;
}

शून्य
StringTrieBuilder::Final_मूल्य_गाँठ::लिख(StringTrieBuilder &builder) {
    offset=builder.write_मूल्य_And_Final(value, सत्य);
}

तार्किक
StringTrieBuilder::मूल्य_गाँठ::operator==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!Node::operator==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव मूल्य_गाँठ &o=static_cast<ध्रुव मूल्य_गाँठ &>(other);
    प्रत्यागम has_मूल्य==o.has_मूल्य && (!has_मूल्य || value==o.value);
}

तार्किक
StringTrieBuilder::Intermediate_मूल्य_गाँठ::operator==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!मूल्य_गाँठ::operator==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव Intermediate_मूल्य_गाँठ &o=static_cast<ध्रुव Intermediate_मूल्य_गाँठ &>(other);
    प्रत्यागम next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_मूल्य_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    यदि(offset==0) {
        offset=edge_संख्या=next->mark_दायाँ_Edges_First(edge_संख्या);
    }
    प्रत्यागम edge_संख्या;
}

शून्य
StringTrieBuilder::Intermediate_मूल्य_गाँठ::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    offset=builder.write_मूल्य_And_Final(value, असत्य);
}

तार्किक
StringTrieBuilder::Linear_Match_गाँठ::operator==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!मूल्य_गाँठ::operator==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव Linear_Match_गाँठ &o=static_cast<ध्रुव Linear_Match_गाँठ &>(other);
    प्रत्यागम length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    यदि(offset==0) {
        offset=edge_संख्या=next->mark_दायाँ_Edges_First(edge_संख्या);
    }
    प्रत्यागम edge_संख्या;
}

तार्किक
StringTrieBuilder::List_Branch_गाँठ::operator==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!Node::operator==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव List_Branch_गाँठ &o=static_cast<ध्रुव List_Branch_गाँठ &>(other);
    प्रति(int32_t i=0; i<length; ++i) {
        यदि(units[i]!=o.units[i] || मूल्यावली[i]!=o.मूल्यावली[i] || equal[i]!=o.equal[i]) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

int32_t
StringTrieBuilder::List_Branch_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    यदि(offset==0) {
        first_Edge_संख्या=edge_संख्या;
        int32_t step=0;
        int32_t i=length;
        कुरु {
            Node *edge=equal[--i];
            यदि(edge!=nullptr) {
                edge_संख्या=edge->mark_दायाँ_Edges_First(edge_संख्या-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } यावत्(i>0);
        offset=edge_संख्या;
    }
    प्रत्यागम edge_संख्या;
}

शून्य
StringTrieBuilder::List_Branch_गाँठ::लिख(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_संख्या=length-1;
    Node *दायाँ_Edge=equal[unit_संख्या];
    int32_t दायाँ_Edge_संख्या= दायाँ_Edge==nullptr ? first_Edge_संख्या : दायाँ_Edge->getOffset();
    कुरु {
        --unit_संख्या;
        यदि(equal[unit_संख्या]!=nullptr) {
            equal[unit_संख्या]->write_Unless_Inside_दायाँ_Edge(first_Edge_संख्या, दायाँ_Edge_संख्या, builder);
        }
    } यावत्(unit_संख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_संख्या=length-1;
    यदि(दायाँ_Edge==nullptr) {
        builder.write_मूल्य_And_Final(मूल्यावली[unit_संख्या], सत्य);
    } अन्यथा {
        दायाँ_Edge->लिख(builder);
    }
    offset=builder.लिख(units[unit_संख्या]);
    // Write the rest of this node's unit-value pairs.
    यावत्(--unit_संख्या>=0) {
        int32_t value;
        UBool isFinal;
        यदि(equal[unit_संख्या]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=मूल्यावली[unit_संख्या];
            isFinal=सत्य;
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_संख्या]->getOffset()>0);
            value=offset-equal[unit_संख्या]->getOffset();
            isFinal=असत्य;
        }
        builder.write_मूल्य_And_Final(value, isFinal);
        offset=builder.लिख(units[unit_संख्या]);
    }
}

तार्किक
StringTrieBuilder::Split_Branch_गाँठ::operator==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!Node::operator==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव Split_Branch_गाँठ &o=static_cast<ध्रुव Split_Branch_गाँठ &>(other);
    प्रत्यागम unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    यदि(offset==0) {
        first_Edge_संख्या=edge_संख्या;
        edge_संख्या=greaterOrEqual->mark_दायाँ_Edges_First(edge_संख्या);
        offset=edge_संख्या=lessThan->mark_दायाँ_Edges_First(edge_संख्या-1);
    }
    प्रत्यागम edge_संख्या;
}

शून्य
StringTrieBuilder::Split_Branch_गाँठ::लिख(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_दायाँ_Edge(first_Edge_संख्या, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->लिख(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.लिख(unit);
}

तार्किक
StringTrieBuilder::Branch_Head_गाँठ::operator==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!मूल्य_गाँठ::operator==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव Branch_Head_गाँठ &o=static_cast<ध्रुव Branch_Head_गाँठ &>(other);
    प्रत्यागम length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    यदि(offset==0) {
        offset=edge_संख्या=next->mark_दायाँ_Edges_First(edge_संख्या);
    }
    प्रत्यागम edge_संख्या;
}

शून्य
StringTrieBuilder::Branch_Head_गाँठ::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    यदि(length<=builder.getMinLinearMatch()) {
        offset=builder.write_मूल्य_And_Type(has_मूल्य, value, length-1);
    } अन्यथा {
        builder.लिख(length-1);
        offset=builder.write_मूल्य_And_Type(has_मूल्य, value, 0);
    }
}

U_NAMESPACE_END
