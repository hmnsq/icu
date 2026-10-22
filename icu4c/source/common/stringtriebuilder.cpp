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

#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringtriebuilder.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"

U_CDECL_BEGIN

ಸ್ಥಿರ int32_t U_CALLCONV
hash_String_Trie_ಗಂಟು(ಧ್ರುವ UHashTok ಕೀಲಿ) {
    ಪ್ರತ್ಯಾಗಮ icu::StringTrieBuilder::hash_ಗಂಟು(ಕೀಲಿ.pointer);
}

ಸ್ಥಿರ UBool U_CALLCONV
equalStringTrieNodes(ಧ್ರುವ UHashTok key1, ಧ್ರುವ UHashTok key2) {
    ಪ್ರತ್ಯಾಗಮ icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

ಶೂನ್ಯ
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    nodes=uhash_openSize(hash_String_Trie_ಗಂಟು, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    ಯದಿ(U_SUCCESS(errorCode)) {
        ಯದಿ(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ {
          uhash_set_ಕೀಲಿ_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

ಶೂನ್ಯ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

ಶೂನ್ಯ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    ಯದಿ(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_ಗಂಟು(0, elementsLength, 0);
    } ಅನ್ಯಥಾ /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *ಬೇರು=make_ಗಂಟು(0, elementsLength, 0, errorCode);
        ಯದಿ(U_SUCCESS(errorCode)) {
            ಬೇರು->mark_ಬಲ_Edges_First(-1);
            ಬೇರು->ಲಿಖ(*ಏತತ್);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_ಗಂಟು(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_ಮೌಲ್ಯ=ಅಸತ್ಯ;
    int32_t value=0;
    int32_t type;
    ಯದಿ(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_ಮೌಲ್ಯ(start++);
        ಯದಿ(start==limit) {
            ಪ್ರತ್ಯಾಗಮ write_ಮೌಲ್ಯ_And_Final(value, ಸತ್ಯ);  // final-value node
        }
        has_ಮೌಲ್ಯ=ಸತ್ಯ;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    ಯದಿ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_ಗಂಟು(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ಯಾವತ್(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            ಲಿಖ(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } ಅನ್ಯಥಾ {
        // Branch node.
        int32_t length=ಎಣಿಕೆ_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_ಗಂಟು(start, limit, unitIndex, length);
        ಯದಿ(--length<getMinLinearMatch()) {
            type=length;
        } ಅನ್ಯಥಾ {
            ಲಿಖ(length);
            type=0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ write_ಮೌಲ್ಯ_And_Type(has_ಮೌಲ್ಯ, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_ಗಂಟು(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t ಮಧ್ಯ_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ಯಾವತ್(length>get_Max_Branch_Linear_Sub_ಗಂಟು_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        ಮಧ್ಯ_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_ಗಂಟು(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_ಗಂಟು_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_ಗಂಟು_Length-1];
    int32_t unit_ಸಂಖ್ಯೆ=0;
    ಕುರು {
        int32_t i=starts[unit_ಸಂಖ್ಯೆ]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_ಸಂಖ್ಯೆ]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } ಯಾವತ್(++unit_ಸಂಖ್ಯೆ<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_ಸಂಖ್ಯೆ]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_ಗಂಟು_Length-1];
    ಕುರು {
        --unit_ಸಂಖ್ಯೆ;
        ಯದಿ(!isFinal[unit_ಸಂಖ್ಯೆ]) {
            jumpTargets[unit_ಸಂಖ್ಯೆ]=write_ಗಂಟು(starts[unit_ಸಂಖ್ಯೆ], starts[unit_ಸಂಖ್ಯೆ+1], unitIndex+1);
        }
    } ಯಾವತ್(unit_ಸಂಖ್ಯೆ>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ಸಂಖ್ಯೆ=length-1;
    write_ಗಂಟು(start, limit, unitIndex+1);
    int32_t offset=ಲಿಖ(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    ಯಾವತ್(--unit_ಸಂಖ್ಯೆ>=0) {
        start=starts[unit_ಸಂಖ್ಯೆ];
        int32_t value;
        ಯದಿ(isFinal[unit_ಸಂಖ್ಯೆ]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_ಮೌಲ್ಯ(start);
        } ಅನ್ಯಥಾ {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_ಸಂಖ್ಯೆ];
        }
        write_ಮೌಲ್ಯ_And_Final(value, isFinal[unit_ಸಂಖ್ಯೆ]);
        offset=ಲಿಖ(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    ಯಾವತ್(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=ಲಿಖ(ಮಧ್ಯ_Units[ltLength]);
    }
    ಪ್ರತ್ಯಾಗಮ offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_ಗಂಟು(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UBool has_ಮೌಲ್ಯ=ಅಸತ್ಯ;
    int32_t value=0;
    ಯದಿ(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_ಮೌಲ್ಯ(start++);
        ಯದಿ(start==limit) {
            ಪ್ರತ್ಯಾಗಮ register_Final_ಮೌಲ್ಯ(value, errorCode);
        }
        has_ಮೌಲ್ಯ=ಸತ್ಯ;
    }
    Node *ಗಂಟು;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    ಯದಿ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_ಗಂಟು=make_ಗಂಟು(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ಯಾವತ್(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            ಗಂಟು=create_Linear_Match_ಗಂಟು(start, lastUnitIndex, maxLinearMatchLength, next_ಗಂಟು);
            next_ಗಂಟು=register_ಗಂಟು(ಗಂಟು, errorCode);
        }
        ಗಂಟು=create_Linear_Match_ಗಂಟು(start, unitIndex, length, next_ಗಂಟು);
    } ಅನ್ಯಥಾ {
        // Branch node.
        int32_t length=ಎಣಿಕೆ_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_ಗಂಟು=make_Branch_Sub_ಗಂಟು(start, limit, unitIndex, length, errorCode);
        ಗಂಟು=ನೂತನ Branch_Head_ಗಂಟು(length, sub_ಗಂಟು);
    }
    ಯದಿ(has_ಮೌಲ್ಯ && ಗಂಟು!=nullptr) {
        ಯದಿ(match_Nodes_Can_Have_ಮೌಲ್ಯಗಳು()) {
            ((ಮೌಲ್ಯ_ಗಂಟು *)ಗಂಟು)->set_ಮೌಲ್ಯ(value);
        } ಅನ್ಯಥಾ {
            ಗಂಟು=ನೂತನ Intermediate_ಮೌಲ್ಯ_ಗಂಟು(value, register_ಗಂಟು(ಗಂಟು, errorCode));
        }
    }
    ಪ್ರತ್ಯಾಗಮ register_ಗಂಟು(ಗಂಟು, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_ಗಂಟು(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    char16_t ಮಧ್ಯ_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ಯಾವತ್(length>get_Max_Branch_Linear_Sub_ಗಂಟು_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        ಮಧ್ಯ_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_ಗಂಟು(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    List_Branch_ಗಂಟು *list_ಗಂಟು=ನೂತನ List_Branch_ಗಂಟು();
    ಯದಿ(list_ಗಂಟು==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_ಸಂಖ್ಯೆ=0;
    ಕುರು {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        ಯದಿ(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_ಗಂಟು->add(unit, get_Element_ಮೌಲ್ಯ(start));
        } ಅನ್ಯಥಾ {
            list_ಗಂಟು->add(unit, make_ಗಂಟು(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } ಯಾವತ್(++unit_ಸಂಖ್ಯೆ<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    ಯದಿ(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_ಗಂಟು->add(unit, get_Element_ಮೌಲ್ಯ(start));
    } ಅನ್ಯಥಾ {
        list_ಗಂಟು->add(unit, make_ಗಂಟು(start, limit, unitIndex+1, errorCode));
    }
    Node *ಗಂಟು=register_ಗಂಟು(list_ಗಂಟು, errorCode);
    // Create the split-branch nodes.
    ಯಾವತ್(ltLength>0) {
        --ltLength;
        ಗಂಟು=register_ಗಂಟು(
            ನೂತನ Split_Branch_ಗಂಟು(ಮಧ್ಯ_Units[ltLength], lessThan[ltLength], ಗಂಟು), errorCode);
    }
    ಪ್ರತ್ಯಾಗಮ ಗಂಟು;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_ಗಂಟು(Node* new_ಗಂಟು, UErrorCode& errorCode) U_LIFETIME_BOUND {
    ಯದಿ(U_FAILURE(errorCode)) {
        delete new_ಗಂಟು;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(new_ಗಂಟು==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ UHashElement *old=uhash_find(nodes, new_ಗಂಟು);
    ಯದಿ(old!=nullptr) {
        delete new_ಗಂಟು;
        ಪ್ರತ್ಯಾಗಮ static_cast<Node*>(old->ಕೀಲಿ.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ಯದಿ U_DEBUG
    int32_t old_ಮೌಲ್ಯ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_ಗಂಟು, 1, &errorCode);
    U_ASSERT(old_ಮೌಲ್ಯ==0);
    ಯದಿ(U_FAILURE(errorCode)) {
        delete new_ಗಂಟು;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ new_ಗಂಟು;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_ಮೌಲ್ಯ(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    Final_ಮೌಲ್ಯ_ಗಂಟು ಕೀಲಿ(value);
    ಧ್ರುವ UHashElement *old=uhash_find(nodes, &ಕೀಲಿ);
    ಯದಿ(old!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ static_cast<Node*>(old->ಕೀಲಿ.pointer);
    }
    Node *new_ಗಂಟು=ನೂತನ Final_ಮೌಲ್ಯ_ಗಂಟು(value);
    ಯದಿ(new_ಗಂಟು==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ಯದಿ U_DEBUG
    int32_t old_ಮೌಲ್ಯ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_ಗಂಟು, 1, &errorCode);
    U_ASSERT(old_ಮೌಲ್ಯ==0);
    ಯದಿ(U_FAILURE(errorCode)) {
        delete new_ಗಂಟು;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ new_ಗಂಟು;
}

int32_t
StringTrieBuilder::hash_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *ಗಂಟು) {
    ಪ್ರತ್ಯಾಗಮ static_cast<ಧ್ರುವ Node*>(ಗಂಟು)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    ಪ್ರತ್ಯಾಗಮ *static_cast<ಧ್ರುವ Node*>(left) == *static_cast<ಧ್ರುವ Node*>(ಬಲ);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Node::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ಏತತ್==&other || (typeid(*ಏತತ್)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) {
    ಯದಿ(offset==0) {
        offset=edge_ಸಂಖ್ಯೆ;
    }
    ಪ್ರತ್ಯಾಗಮ edge_ಸಂಖ್ಯೆ;
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Final_ಮೌಲ್ಯ_ಗಂಟು::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(!Node::operator==(other)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ Final_ಮೌಲ್ಯ_ಗಂಟು &o=static_cast<ಧ್ರುವ Final_ಮೌಲ್ಯ_ಗಂಟು &>(other);
    ಪ್ರತ್ಯಾಗಮ value==o.value;
}

ಶೂನ್ಯ
StringTrieBuilder::Final_ಮೌಲ್ಯ_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    offset=builder.write_ಮೌಲ್ಯ_And_Final(value, ಸತ್ಯ);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::ಮೌಲ್ಯ_ಗಂಟು::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(!Node::operator==(other)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ ಮೌಲ್ಯ_ಗಂಟು &o=static_cast<ಧ್ರುವ ಮೌಲ್ಯ_ಗಂಟು &>(other);
    ಪ್ರತ್ಯಾಗಮ has_ಮೌಲ್ಯ==o.has_ಮೌಲ್ಯ && (!has_ಮೌಲ್ಯ || value==o.value);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Intermediate_ಮೌಲ್ಯ_ಗಂಟು::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(!ಮೌಲ್ಯ_ಗಂಟು::operator==(other)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ Intermediate_ಮೌಲ್ಯ_ಗಂಟು &o=static_cast<ಧ್ರುವ Intermediate_ಮೌಲ್ಯ_ಗಂಟು &>(other);
    ಪ್ರತ್ಯಾಗಮ next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_ಮೌಲ್ಯ_ಗಂಟು::mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) {
    ಯದಿ(offset==0) {
        offset=edge_ಸಂಖ್ಯೆ=next->mark_ಬಲ_Edges_First(edge_ಸಂಖ್ಯೆ);
    }
    ಪ್ರತ್ಯಾಗಮ edge_ಸಂಖ್ಯೆ;
}

ಶೂನ್ಯ
StringTrieBuilder::Intermediate_ಮೌಲ್ಯ_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    next->ಲಿಖ(builder);
    offset=builder.write_ಮೌಲ್ಯ_And_Final(value, ಅಸತ್ಯ);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Linear_Match_ಗಂಟು::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(!ಮೌಲ್ಯ_ಗಂಟು::operator==(other)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ Linear_Match_ಗಂಟು &o=static_cast<ಧ್ರುವ Linear_Match_ಗಂಟು &>(other);
    ಪ್ರತ್ಯಾಗಮ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_ಗಂಟು::mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) {
    ಯದಿ(offset==0) {
        offset=edge_ಸಂಖ್ಯೆ=next->mark_ಬಲ_Edges_First(edge_ಸಂಖ್ಯೆ);
    }
    ಪ್ರತ್ಯಾಗಮ edge_ಸಂಖ್ಯೆ;
}

ತಾರ್ಕಿಕ
StringTrieBuilder::List_Branch_ಗಂಟು::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(!Node::operator==(other)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ List_Branch_ಗಂಟು &o=static_cast<ಧ್ರುವ List_Branch_ಗಂಟು &>(other);
    ಪ್ರತಿ(int32_t i=0; i<length; ++i) {
        ಯದಿ(units[i]!=o.units[i] || ಮೌಲ್ಯಗಳು[i]!=o.ಮೌಲ್ಯಗಳು[i] || equal[i]!=o.equal[i]) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

int32_t
StringTrieBuilder::List_Branch_ಗಂಟು::mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) {
    ಯದಿ(offset==0) {
        first_Edge_ಸಂಖ್ಯೆ=edge_ಸಂಖ್ಯೆ;
        int32_t step=0;
        int32_t i=length;
        ಕುರು {
            Node *edge=equal[--i];
            ಯದಿ(edge!=nullptr) {
                edge_ಸಂಖ್ಯೆ=edge->mark_ಬಲ_Edges_First(edge_ಸಂಖ್ಯೆ-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } ಯಾವತ್(i>0);
        offset=edge_ಸಂಖ್ಯೆ;
    }
    ಪ್ರತ್ಯಾಗಮ edge_ಸಂಖ್ಯೆ;
}

ಶೂನ್ಯ
StringTrieBuilder::List_Branch_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_ಸಂಖ್ಯೆ=length-1;
    Node *ಬಲ_Edge=equal[unit_ಸಂಖ್ಯೆ];
    int32_t ಬಲ_Edge_ಸಂಖ್ಯೆ= ಬಲ_Edge==nullptr ? first_Edge_ಸಂಖ್ಯೆ : ಬಲ_Edge->getOffset();
    ಕುರು {
        --unit_ಸಂಖ್ಯೆ;
        ಯದಿ(equal[unit_ಸಂಖ್ಯೆ]!=nullptr) {
            equal[unit_ಸಂಖ್ಯೆ]->write_Unless_Inside_ಬಲ_Edge(first_Edge_ಸಂಖ್ಯೆ, ಬಲ_Edge_ಸಂಖ್ಯೆ, builder);
        }
    } ಯಾವತ್(unit_ಸಂಖ್ಯೆ>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ಸಂಖ್ಯೆ=length-1;
    ಯದಿ(ಬಲ_Edge==nullptr) {
        builder.write_ಮೌಲ್ಯ_And_Final(ಮೌಲ್ಯಗಳು[unit_ಸಂಖ್ಯೆ], ಸತ್ಯ);
    } ಅನ್ಯಥಾ {
        ಬಲ_Edge->ಲಿಖ(builder);
    }
    offset=builder.ಲಿಖ(units[unit_ಸಂಖ್ಯೆ]);
    // Write the rest of this node's unit-value pairs.
    ಯಾವತ್(--unit_ಸಂಖ್ಯೆ>=0) {
        int32_t value;
        UBool isFinal;
        ಯದಿ(equal[unit_ಸಂಖ್ಯೆ]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=ಮೌಲ್ಯಗಳು[unit_ಸಂಖ್ಯೆ];
            isFinal=ಸತ್ಯ;
        } ಅನ್ಯಥಾ {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_ಸಂಖ್ಯೆ]->getOffset()>0);
            value=offset-equal[unit_ಸಂಖ್ಯೆ]->getOffset();
            isFinal=ಅಸತ್ಯ;
        }
        builder.write_ಮೌಲ್ಯ_And_Final(value, isFinal);
        offset=builder.ಲಿಖ(units[unit_ಸಂಖ್ಯೆ]);
    }
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Split_Branch_ಗಂಟು::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(!Node::operator==(other)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ Split_Branch_ಗಂಟು &o=static_cast<ಧ್ರುವ Split_Branch_ಗಂಟು &>(other);
    ಪ್ರತ್ಯಾಗಮ unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_ಗಂಟು::mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) {
    ಯದಿ(offset==0) {
        first_Edge_ಸಂಖ್ಯೆ=edge_ಸಂಖ್ಯೆ;
        edge_ಸಂಖ್ಯೆ=greaterOrEqual->mark_ಬಲ_Edges_First(edge_ಸಂಖ್ಯೆ);
        offset=edge_ಸಂಖ್ಯೆ=lessThan->mark_ಬಲ_Edges_First(edge_ಸಂಖ್ಯೆ-1);
    }
    ಪ್ರತ್ಯಾಗಮ edge_ಸಂಖ್ಯೆ;
}

ಶೂನ್ಯ
StringTrieBuilder::Split_Branch_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ಬಲ_Edge(first_Edge_ಸಂಖ್ಯೆ, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->ಲಿಖ(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.ಲಿಖ(unit);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Branch_Head_ಗಂಟು::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(!ಮೌಲ್ಯ_ಗಂಟು::operator==(other)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ Branch_Head_ಗಂಟು &o=static_cast<ಧ್ರುವ Branch_Head_ಗಂಟು &>(other);
    ಪ್ರತ್ಯಾಗಮ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_ಗಂಟು::mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) {
    ಯದಿ(offset==0) {
        offset=edge_ಸಂಖ್ಯೆ=next->mark_ಬಲ_Edges_First(edge_ಸಂಖ್ಯೆ);
    }
    ಪ್ರತ್ಯಾಗಮ edge_ಸಂಖ್ಯೆ;
}

ಶೂನ್ಯ
StringTrieBuilder::Branch_Head_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    next->ಲಿಖ(builder);
    ಯದಿ(length<=builder.getMinLinearMatch()) {
        offset=builder.write_ಮೌಲ್ಯ_And_Type(has_ಮೌಲ್ಯ, value, length-1);
    } ಅನ್ಯಥಾ {
        builder.ಲಿಖ(length-1);
        offset=builder.write_ಮೌಲ್ಯ_And_Type(has_ಮೌಲ್ಯ, value, 0);
    }
}

U_NAMESPACE_END
