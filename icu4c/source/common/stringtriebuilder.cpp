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
hash_String_Trie_गाँठो(अचर UHashTok साँचो) {
    फर्काऊ icu::StringTrieBuilder::hash_गाँठो(साँचो.pointer);
}

स्थिर UBool U_CALLCONV
equalStringTrieNodes(अचर UHashTok key1, अचर UHashTok key2) {
    फर्काऊ icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
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
        फर्काऊ;
    }
    nodes=uhash_openSize(hash_String_Trie_गाँठो, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    यदि(U_SUCCESS(errorCode)) {
        यदि(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
          uhash_set_साँचो_Deleter(nodes, uprv_deleteUObject);
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
        write_गाँठो(0, elementsLength, 0);
    } अन्यथा /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *जरा=make_गाँठो(0, elementsLength, 0, errorCode);
        यदि(U_SUCCESS(errorCode)) {
            जरा->mark_दायाँ_Edges_First(-1);
            जरा->लिख(*एतत्);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_गाँठो(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_मान=असत्य;
    int32_t value=0;
    int32_t type;
    यदि(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_मान(start++);
        यदि(start==limit) {
            फर्काऊ write_मान_And_Final(value, सत्य);  // final-value node
        }
        has_मान=सत्य;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    यदि(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_गाँठो(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        जबसम्म(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            लिख(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } अन्यथा {
        // Branch node.
        int32_t length=गन्ती_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_गाँठो(start, limit, unitIndex, length);
        यदि(--length<getMinLinearMatch()) {
            type=length;
        } अन्यथा {
            लिख(length);
            type=0;
        }
    }
    फर्काऊ write_मान_And_Type(has_मान, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_गाँठो(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t बीच_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    जबसम्म(length>get_Max_Branch_Linear_Sub_गाँठो_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        बीच_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_गाँठो(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_गाँठो_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_गाँठो_Length-1];
    int32_t unit_सङ्ख्या=0;
    गर {
        int32_t i=starts[unit_सङ्ख्या]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_सङ्ख्या]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } जबसम्म(++unit_सङ्ख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_सङ्ख्या]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_गाँठो_Length-1];
    गर {
        --unit_सङ्ख्या;
        यदि(!isFinal[unit_सङ्ख्या]) {
            jumpTargets[unit_सङ्ख्या]=write_गाँठो(starts[unit_सङ्ख्या], starts[unit_सङ्ख्या+1], unitIndex+1);
        }
    } जबसम्म(unit_सङ्ख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_सङ्ख्या=length-1;
    write_गाँठो(start, limit, unitIndex+1);
    int32_t offset=लिख(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    जबसम्म(--unit_सङ्ख्या>=0) {
        start=starts[unit_सङ्ख्या];
        int32_t value;
        यदि(isFinal[unit_सङ्ख्या]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_मान(start);
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_सङ्ख्या];
        }
        write_मान_And_Final(value, isFinal[unit_सङ्ख्या]);
        offset=लिख(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    जबसम्म(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=लिख(बीच_Units[ltLength]);
    }
    फर्काऊ offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_गाँठो(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    UBool has_मान=असत्य;
    int32_t value=0;
    यदि(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_मान(start++);
        यदि(start==limit) {
            फर्काऊ register_Final_मान(value, errorCode);
        }
        has_मान=सत्य;
    }
    Node *गाँठो;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    यदि(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_गाँठो=make_गाँठो(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        जबसम्म(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            गाँठो=create_Linear_Match_गाँठो(start, lastUnitIndex, maxLinearMatchLength, next_गाँठो);
            next_गाँठो=register_गाँठो(गाँठो, errorCode);
        }
        गाँठो=create_Linear_Match_गाँठो(start, unitIndex, length, next_गाँठो);
    } अन्यथा {
        // Branch node.
        int32_t length=गन्ती_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_गाँठो=make_Branch_Sub_गाँठो(start, limit, unitIndex, length, errorCode);
        गाँठो=नूतन Branch_Head_गाँठो(length, sub_गाँठो);
    }
    यदि(has_मान && गाँठो!=nullptr) {
        यदि(match_Nodes_Can_Have_मानहरू()) {
            ((मान_गाँठो *)गाँठो)->set_मान(value);
        } अन्यथा {
            गाँठो=नूतन Intermediate_मान_गाँठो(value, register_गाँठो(गाँठो, errorCode));
        }
    }
    फर्काऊ register_गाँठो(गाँठो, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_गाँठो(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    char16_t बीच_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    जबसम्म(length>get_Max_Branch_Linear_Sub_गाँठो_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        बीच_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_गाँठो(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    List_Branch_गाँठो *list_गाँठो=नूतन List_Branch_गाँठो();
    यदि(list_गाँठो==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_सङ्ख्या=0;
    गर {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        यदि(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_गाँठो->add(unit, get_Element_मान(start));
        } अन्यथा {
            list_गाँठो->add(unit, make_गाँठो(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } जबसम्म(++unit_सङ्ख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    यदि(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_गाँठो->add(unit, get_Element_मान(start));
    } अन्यथा {
        list_गाँठो->add(unit, make_गाँठो(start, limit, unitIndex+1, errorCode));
    }
    Node *गाँठो=register_गाँठो(list_गाँठो, errorCode);
    // Create the split-branch nodes.
    जबसम्म(ltLength>0) {
        --ltLength;
        गाँठो=register_गाँठो(
            नूतन Split_Branch_गाँठो(बीच_Units[ltLength], lessThan[ltLength], गाँठो), errorCode);
    }
    फर्काऊ गाँठो;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_गाँठो(Node* new_गाँठो, UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        delete new_गाँठो;
        फर्काऊ nullptr;
    }
    यदि(new_गाँठो==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    अचर UHashElement *old=uhash_find(nodes, new_गाँठो);
    यदि(old!=nullptr) {
        delete new_गाँठो;
        फर्काऊ static_cast<Node*>(old->साँचो.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#यदि U_DEBUG
    int32_t old_मान=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_गाँठो, 1, &errorCode);
    U_ASSERT(old_मान==0);
    यदि(U_FAILURE(errorCode)) {
        delete new_गाँठो;
        फर्काऊ nullptr;
    }
    फर्काऊ new_गाँठो;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_मान(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    Final_मान_गाँठो साँचो(value);
    अचर UHashElement *old=uhash_find(nodes, &साँचो);
    यदि(old!=nullptr) {
        फर्काऊ static_cast<Node*>(old->साँचो.pointer);
    }
    Node *new_गाँठो=नूतन Final_मान_गाँठो(value);
    यदि(new_गाँठो==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#यदि U_DEBUG
    int32_t old_मान=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_गाँठो, 1, &errorCode);
    U_ASSERT(old_मान==0);
    यदि(U_FAILURE(errorCode)) {
        delete new_गाँठो;
        फर्काऊ nullptr;
    }
    फर्काऊ new_गाँठो;
}

int32_t
StringTrieBuilder::hash_गाँठो(अचर शून्य *गाँठो) {
    फर्काऊ static_cast<अचर Node*>(गाँठो)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(अचर शून्य *left, अचर शून्य *दायाँ) {
    फर्काऊ *static_cast<अचर Node*>(left) == *static_cast<अचर Node*>(दायाँ);
}

तार्किक
StringTrieBuilder::Node::चालक==(अचर Node &other) अचर {
    फर्काऊ एतत्==&other || (typeid(*एतत्)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) {
    यदि(offset==0) {
        offset=edge_सङ्ख्या;
    }
    फर्काऊ edge_सङ्ख्या;
}

तार्किक
StringTrieBuilder::Final_मान_गाँठो::चालक==(अचर Node &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    यदि(!Node::चालक==(other)) {
        फर्काऊ असत्य;
    }
    अचर Final_मान_गाँठो &o=static_cast<अचर Final_मान_गाँठो &>(other);
    फर्काऊ value==o.value;
}

शून्य
StringTrieBuilder::Final_मान_गाँठो::लिख(StringTrieBuilder &builder) {
    offset=builder.write_मान_And_Final(value, सत्य);
}

तार्किक
StringTrieBuilder::मान_गाँठो::चालक==(अचर Node &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    यदि(!Node::चालक==(other)) {
        फर्काऊ असत्य;
    }
    अचर मान_गाँठो &o=static_cast<अचर मान_गाँठो &>(other);
    फर्काऊ has_मान==o.has_मान && (!has_मान || value==o.value);
}

तार्किक
StringTrieBuilder::Intermediate_मान_गाँठो::चालक==(अचर Node &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    यदि(!मान_गाँठो::चालक==(other)) {
        फर्काऊ असत्य;
    }
    अचर Intermediate_मान_गाँठो &o=static_cast<अचर Intermediate_मान_गाँठो &>(other);
    फर्काऊ next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_मान_गाँठो::mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) {
    यदि(offset==0) {
        offset=edge_सङ्ख्या=next->mark_दायाँ_Edges_First(edge_सङ्ख्या);
    }
    फर्काऊ edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::Intermediate_मान_गाँठो::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    offset=builder.write_मान_And_Final(value, असत्य);
}

तार्किक
StringTrieBuilder::Linear_Match_गाँठो::चालक==(अचर Node &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    यदि(!मान_गाँठो::चालक==(other)) {
        फर्काऊ असत्य;
    }
    अचर Linear_Match_गाँठो &o=static_cast<अचर Linear_Match_गाँठो &>(other);
    फर्काऊ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_गाँठो::mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) {
    यदि(offset==0) {
        offset=edge_सङ्ख्या=next->mark_दायाँ_Edges_First(edge_सङ्ख्या);
    }
    फर्काऊ edge_सङ्ख्या;
}

तार्किक
StringTrieBuilder::List_Branch_गाँठो::चालक==(अचर Node &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    यदि(!Node::चालक==(other)) {
        फर्काऊ असत्य;
    }
    अचर List_Branch_गाँठो &o=static_cast<अचर List_Branch_गाँठो &>(other);
    लागि(int32_t i=0; i<length; ++i) {
        यदि(units[i]!=o.units[i] || मानहरू[i]!=o.मानहरू[i] || equal[i]!=o.equal[i]) {
            फर्काऊ असत्य;
        }
    }
    फर्काऊ सत्य;
}

int32_t
StringTrieBuilder::List_Branch_गाँठो::mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) {
    यदि(offset==0) {
        first_Edge_सङ्ख्या=edge_सङ्ख्या;
        int32_t step=0;
        int32_t i=length;
        गर {
            Node *edge=equal[--i];
            यदि(edge!=nullptr) {
                edge_सङ्ख्या=edge->mark_दायाँ_Edges_First(edge_सङ्ख्या-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } जबसम्म(i>0);
        offset=edge_सङ्ख्या;
    }
    फर्काऊ edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::List_Branch_गाँठो::लिख(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_सङ्ख्या=length-1;
    Node *दायाँ_Edge=equal[unit_सङ्ख्या];
    int32_t दायाँ_Edge_सङ्ख्या= दायाँ_Edge==nullptr ? first_Edge_सङ्ख्या : दायाँ_Edge->getOffset();
    गर {
        --unit_सङ्ख्या;
        यदि(equal[unit_सङ्ख्या]!=nullptr) {
            equal[unit_सङ्ख्या]->write_Unless_Inside_दायाँ_Edge(first_Edge_सङ्ख्या, दायाँ_Edge_सङ्ख्या, builder);
        }
    } जबसम्म(unit_सङ्ख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_सङ्ख्या=length-1;
    यदि(दायाँ_Edge==nullptr) {
        builder.write_मान_And_Final(मानहरू[unit_सङ्ख्या], सत्य);
    } अन्यथा {
        दायाँ_Edge->लिख(builder);
    }
    offset=builder.लिख(units[unit_सङ्ख्या]);
    // Write the rest of this node's unit-value pairs.
    जबसम्म(--unit_सङ्ख्या>=0) {
        int32_t value;
        UBool isFinal;
        यदि(equal[unit_सङ्ख्या]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=मानहरू[unit_सङ्ख्या];
            isFinal=सत्य;
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_सङ्ख्या]->getOffset()>0);
            value=offset-equal[unit_सङ्ख्या]->getOffset();
            isFinal=असत्य;
        }
        builder.write_मान_And_Final(value, isFinal);
        offset=builder.लिख(units[unit_सङ्ख्या]);
    }
}

तार्किक
StringTrieBuilder::Split_Branch_गाँठो::चालक==(अचर Node &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    यदि(!Node::चालक==(other)) {
        फर्काऊ असत्य;
    }
    अचर Split_Branch_गाँठो &o=static_cast<अचर Split_Branch_गाँठो &>(other);
    फर्काऊ unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_गाँठो::mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) {
    यदि(offset==0) {
        first_Edge_सङ्ख्या=edge_सङ्ख्या;
        edge_सङ्ख्या=greaterOrEqual->mark_दायाँ_Edges_First(edge_सङ्ख्या);
        offset=edge_सङ्ख्या=lessThan->mark_दायाँ_Edges_First(edge_सङ्ख्या-1);
    }
    फर्काऊ edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::Split_Branch_गाँठो::लिख(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_दायाँ_Edge(first_Edge_सङ्ख्या, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->लिख(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.लिख(unit);
}

तार्किक
StringTrieBuilder::Branch_Head_गाँठो::चालक==(अचर Node &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    यदि(!मान_गाँठो::चालक==(other)) {
        फर्काऊ असत्य;
    }
    अचर Branch_Head_गाँठो &o=static_cast<अचर Branch_Head_गाँठो &>(other);
    फर्काऊ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_गाँठो::mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) {
    यदि(offset==0) {
        offset=edge_सङ्ख्या=next->mark_दायाँ_Edges_First(edge_सङ्ख्या);
    }
    फर्काऊ edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::Branch_Head_गाँठो::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    यदि(length<=builder.getMinLinearMatch()) {
        offset=builder.write_मान_And_Type(has_मान, value, length-1);
    } अन्यथा {
        builder.लिख(length-1);
        offset=builder.write_मान_And_Type(has_मान, value, 0);
    }
}

U_NAMESPACE_END
