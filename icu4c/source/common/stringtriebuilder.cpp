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

#অন্তর্ভাৱ "utypeinfo.h"  // for 'typeid' to work
#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/stringtriebuilder.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "uhash.h"

U_CDECL_BEGIN

স্থির int32_t U_CALLCONV
hash_String_Trie_গ্ৰন্থি(ধ্রুৱ UHashTok চাবি) {
    ঘূৰাই icu::StringTrieBuilder::hash_গ্ৰন্থি(চাবি.pointer);
}

স্থির UBool U_CALLCONV
equalStringTrieNodes(ধ্রুৱ UHashTok key1, ধ্রুৱ UHashTok key2) {
    ঘূৰাই icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

শূন্য
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    nodes=uhash_openSize(hash_String_Trie_গ্ৰন্থি, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    যদি(U_SUCCESS(errorCode)) {
        যদি(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
          uhash_set_চাবি_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

শূন্য
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

শূন্য
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    যদি(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_গ্ৰন্থি(0, elementsLength, 0);
    } অন্যথা /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *মূল=make_গ্ৰন্থি(0, elementsLength, 0, errorCode);
        যদি(U_SUCCESS(errorCode)) {
            মূল->mark_ডান_Edges_First(-1);
            মূল->লিখ(*এতত্);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_গ্ৰন্থি(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_মান=মিছা;
    int32_t value=0;
    int32_t type;
    যদি(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_মান(start++);
        যদি(start==limit) {
            ঘূৰাই write_মান_And_Final(value, সঁচা);  // final-value node
        }
        has_মান=সঁচা;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    যদি(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_গ্ৰন্থি(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        যেতিয়ালৈ(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            লিখ(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } অন্যথা {
        // Branch node.
        int32_t length=গুনতি_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_গ্ৰন্থি(start, limit, unitIndex, length);
        যদি(--length<getMinLinearMatch()) {
            type=length;
        } অন্যথা {
            লিখ(length);
            type=0;
        }
    }
    ঘূৰাই write_মান_And_Type(has_মান, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_গ্ৰন্থি(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t মধ্য_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    যেতিয়ালৈ(length>get_Max_Branch_Linear_Sub_গ্ৰন্থি_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        মধ্য_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_গ্ৰন্থি(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length-1];
    int32_t unit_সংখ্যা=0;
    কৰা {
        int32_t i=starts[unit_সংখ্যা]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_সংখ্যা]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } যেতিয়ালৈ(++unit_সংখ্যা<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_সংখ্যা]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length-1];
    কৰা {
        --unit_সংখ্যা;
        যদি(!isFinal[unit_সংখ্যা]) {
            jumpTargets[unit_সংখ্যা]=write_গ্ৰন্থি(starts[unit_সংখ্যা], starts[unit_সংখ্যা+1], unitIndex+1);
        }
    } যেতিয়ালৈ(unit_সংখ্যা>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_সংখ্যা=length-1;
    write_গ্ৰন্থি(start, limit, unitIndex+1);
    int32_t offset=লিখ(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    যেতিয়ালৈ(--unit_সংখ্যা>=0) {
        start=starts[unit_সংখ্যা];
        int32_t value;
        যদি(isFinal[unit_সংখ্যা]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_মান(start);
        } অন্যথা {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_সংখ্যা];
        }
        write_মান_And_Final(value, isFinal[unit_সংখ্যা]);
        offset=লিখ(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    যেতিয়ালৈ(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=লিখ(মধ্য_Units[ltLength]);
    }
    ঘূৰাই offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_গ্ৰন্থি(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই nullptr;
    }
    UBool has_মান=মিছা;
    int32_t value=0;
    যদি(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_মান(start++);
        যদি(start==limit) {
            ঘূৰাই register_Final_মান(value, errorCode);
        }
        has_মান=সঁচা;
    }
    Node *গ্ৰন্থি;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    যদি(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_গ্ৰন্থি=make_গ্ৰন্থি(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        যেতিয়ালৈ(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            গ্ৰন্থি=create_Linear_Match_গ্ৰন্থি(start, lastUnitIndex, maxLinearMatchLength, next_গ্ৰন্থি);
            next_গ্ৰন্থি=register_গ্ৰন্থি(গ্ৰন্থি, errorCode);
        }
        গ্ৰন্থি=create_Linear_Match_গ্ৰন্থি(start, unitIndex, length, next_গ্ৰন্থি);
    } অন্যথা {
        // Branch node.
        int32_t length=গুনতি_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_গ্ৰন্থি=make_Branch_Sub_গ্ৰন্থি(start, limit, unitIndex, length, errorCode);
        গ্ৰন্থি=নূতন Branch_Head_গ্ৰন্থি(length, sub_গ্ৰন্থি);
    }
    যদি(has_মান && গ্ৰন্থি!=nullptr) {
        যদি(match_Nodes_Can_Have_মানসমূহ()) {
            ((মান_গ্ৰন্থি *)গ্ৰন্থি)->set_মান(value);
        } অন্যথা {
            গ্ৰন্থি=নূতন Intermediate_মান_গ্ৰন্থি(value, register_গ্ৰন্থি(গ্ৰন্থি, errorCode));
        }
    }
    ঘূৰাই register_গ্ৰন্থি(গ্ৰন্থি, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_গ্ৰন্থি(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই nullptr;
    }
    char16_t মধ্য_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    যেতিয়ালৈ(length>get_Max_Branch_Linear_Sub_গ্ৰন্থি_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        মধ্য_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_গ্ৰন্থি(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই nullptr;
    }
    List_Branch_গ্ৰন্থি *list_গ্ৰন্থি=নূতন List_Branch_গ্ৰন্থি();
    যদি(list_গ্ৰন্থি==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_সংখ্যা=0;
    কৰা {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        যদি(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_গ্ৰন্থি->add(unit, get_Element_মান(start));
        } অন্যথা {
            list_গ্ৰন্থি->add(unit, make_গ্ৰন্থি(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } যেতিয়ালৈ(++unit_সংখ্যা<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    যদি(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_গ্ৰন্থি->add(unit, get_Element_মান(start));
    } অন্যথা {
        list_গ্ৰন্থি->add(unit, make_গ্ৰন্থি(start, limit, unitIndex+1, errorCode));
    }
    Node *গ্ৰন্থি=register_গ্ৰন্থি(list_গ্ৰন্থি, errorCode);
    // Create the split-branch nodes.
    যেতিয়ালৈ(ltLength>0) {
        --ltLength;
        গ্ৰন্থি=register_গ্ৰন্থি(
            নূতন Split_Branch_গ্ৰন্থি(মধ্য_Units[ltLength], lessThan[ltLength], গ্ৰন্থি), errorCode);
    }
    ঘূৰাই গ্ৰন্থি;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_গ্ৰন্থি(Node* new_গ্ৰন্থি, UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        delete new_গ্ৰন্থি;
        ঘূৰাই nullptr;
    }
    যদি(new_গ্ৰন্থি==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    ধ্রুৱ UHashElement *old=uhash_find(nodes, new_গ্ৰন্থি);
    যদি(old!=nullptr) {
        delete new_গ্ৰন্থি;
        ঘূৰাই static_cast<Node*>(old->চাবি.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#যদি U_DEBUG
    int32_t old_মান=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_গ্ৰন্থি, 1, &errorCode);
    U_ASSERT(old_মান==0);
    যদি(U_FAILURE(errorCode)) {
        delete new_গ্ৰন্থি;
        ঘূৰাই nullptr;
    }
    ঘূৰাই new_গ্ৰন্থি;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_মান(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই nullptr;
    }
    Final_মান_গ্ৰন্থি চাবি(value);
    ধ্রুৱ UHashElement *old=uhash_find(nodes, &চাবি);
    যদি(old!=nullptr) {
        ঘূৰাই static_cast<Node*>(old->চাবি.pointer);
    }
    Node *new_গ্ৰন্থি=নূতন Final_মান_গ্ৰন্থি(value);
    যদি(new_গ্ৰন্থি==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#যদি U_DEBUG
    int32_t old_মান=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_গ্ৰন্থি, 1, &errorCode);
    U_ASSERT(old_মান==0);
    যদি(U_FAILURE(errorCode)) {
        delete new_গ্ৰন্থি;
        ঘূৰাই nullptr;
    }
    ঘূৰাই new_গ্ৰন্থি;
}

int32_t
StringTrieBuilder::hash_গ্ৰন্থি(ধ্রুৱ শূন্য *গ্ৰন্থি) {
    ঘূৰাই static_cast<ধ্রুৱ Node*>(গ্ৰন্থি)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ধ্রুৱ শূন্য *left, ধ্রুৱ শূন্য *ডান) {
    ঘূৰাই *static_cast<ধ্রুৱ Node*>(left) == *static_cast<ধ্রুৱ Node*>(ডান);
}

তার্কিক
StringTrieBuilder::Node::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    ঘূৰাই এতত্==&other || (typeid(*এতত্)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        offset=edge_সংখ্যা;
    }
    ঘূৰাই edge_সংখ্যা;
}

তার্কিক
StringTrieBuilder::Final_মান_গ্ৰন্থি::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    যদি(এতত্==&other) {
        ঘূৰাই সঁচা;
    }
    যদি(!Node::operator==(other)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ Final_মান_গ্ৰন্থি &o=static_cast<ধ্রুৱ Final_মান_গ্ৰন্থি &>(other);
    ঘূৰাই value==o.value;
}

শূন্য
StringTrieBuilder::Final_মান_গ্ৰন্থি::লিখ(StringTrieBuilder &builder) {
    offset=builder.write_মান_And_Final(value, সঁচা);
}

তার্কিক
StringTrieBuilder::মান_গ্ৰন্থি::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    যদি(এতত্==&other) {
        ঘূৰাই সঁচা;
    }
    যদি(!Node::operator==(other)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ মান_গ্ৰন্থি &o=static_cast<ধ্রুৱ মান_গ্ৰন্থি &>(other);
    ঘূৰাই has_মান==o.has_মান && (!has_মান || value==o.value);
}

তার্কিক
StringTrieBuilder::Intermediate_মান_গ্ৰন্থি::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    যদি(এতত্==&other) {
        ঘূৰাই সঁচা;
    }
    যদি(!মান_গ্ৰন্থি::operator==(other)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ Intermediate_মান_গ্ৰন্থি &o=static_cast<ধ্রুৱ Intermediate_মান_গ্ৰন্থি &>(other);
    ঘূৰাই next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_মান_গ্ৰন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        offset=edge_সংখ্যা=next->mark_ডান_Edges_First(edge_সংখ্যা);
    }
    ঘূৰাই edge_সংখ্যা;
}

শূন্য
StringTrieBuilder::Intermediate_মান_গ্ৰন্থি::লিখ(StringTrieBuilder &builder) {
    next->লিখ(builder);
    offset=builder.write_মান_And_Final(value, মিছা);
}

তার্কিক
StringTrieBuilder::Linear_Match_গ্ৰন্থি::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    যদি(এতত্==&other) {
        ঘূৰাই সঁচা;
    }
    যদি(!মান_গ্ৰন্থি::operator==(other)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ Linear_Match_গ্ৰন্থি &o=static_cast<ধ্রুৱ Linear_Match_গ্ৰন্থি &>(other);
    ঘূৰাই length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_গ্ৰন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        offset=edge_সংখ্যা=next->mark_ডান_Edges_First(edge_সংখ্যা);
    }
    ঘূৰাই edge_সংখ্যা;
}

তার্কিক
StringTrieBuilder::List_Branch_গ্ৰন্থি::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    যদি(এতত্==&other) {
        ঘূৰাই সঁচা;
    }
    যদি(!Node::operator==(other)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ List_Branch_গ্ৰন্থি &o=static_cast<ধ্রুৱ List_Branch_গ্ৰন্থি &>(other);
    বাবে(int32_t i=0; i<length; ++i) {
        যদি(units[i]!=o.units[i] || মানসমূহ[i]!=o.মানসমূহ[i] || equal[i]!=o.equal[i]) {
            ঘূৰাই মিছা;
        }
    }
    ঘূৰাই সঁচা;
}

int32_t
StringTrieBuilder::List_Branch_গ্ৰন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        first_Edge_সংখ্যা=edge_সংখ্যা;
        int32_t step=0;
        int32_t i=length;
        কৰা {
            Node *edge=equal[--i];
            যদি(edge!=nullptr) {
                edge_সংখ্যা=edge->mark_ডান_Edges_First(edge_সংখ্যা-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } যেতিয়ালৈ(i>0);
        offset=edge_সংখ্যা;
    }
    ঘূৰাই edge_সংখ্যা;
}

শূন্য
StringTrieBuilder::List_Branch_গ্ৰন্থি::লিখ(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_সংখ্যা=length-1;
    Node *ডান_Edge=equal[unit_সংখ্যা];
    int32_t ডান_Edge_সংখ্যা= ডান_Edge==nullptr ? first_Edge_সংখ্যা : ডান_Edge->getOffset();
    কৰা {
        --unit_সংখ্যা;
        যদি(equal[unit_সংখ্যা]!=nullptr) {
            equal[unit_সংখ্যা]->write_Unless_Inside_ডান_Edge(first_Edge_সংখ্যা, ডান_Edge_সংখ্যা, builder);
        }
    } যেতিয়ালৈ(unit_সংখ্যা>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_সংখ্যা=length-1;
    যদি(ডান_Edge==nullptr) {
        builder.write_মান_And_Final(মানসমূহ[unit_সংখ্যা], সঁচা);
    } অন্যথা {
        ডান_Edge->লিখ(builder);
    }
    offset=builder.লিখ(units[unit_সংখ্যা]);
    // Write the rest of this node's unit-value pairs.
    যেতিয়ালৈ(--unit_সংখ্যা>=0) {
        int32_t value;
        UBool isFinal;
        যদি(equal[unit_সংখ্যা]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=মানসমূহ[unit_সংখ্যা];
            isFinal=সঁচা;
        } অন্যথা {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_সংখ্যা]->getOffset()>0);
            value=offset-equal[unit_সংখ্যা]->getOffset();
            isFinal=মিছা;
        }
        builder.write_মান_And_Final(value, isFinal);
        offset=builder.লিখ(units[unit_সংখ্যা]);
    }
}

তার্কিক
StringTrieBuilder::Split_Branch_গ্ৰন্থি::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    যদি(এতত্==&other) {
        ঘূৰাই সঁচা;
    }
    যদি(!Node::operator==(other)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ Split_Branch_গ্ৰন্থি &o=static_cast<ধ্রুৱ Split_Branch_গ্ৰন্থি &>(other);
    ঘূৰাই unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_গ্ৰন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        first_Edge_সংখ্যা=edge_সংখ্যা;
        edge_সংখ্যা=greaterOrEqual->mark_ডান_Edges_First(edge_সংখ্যা);
        offset=edge_সংখ্যা=lessThan->mark_ডান_Edges_First(edge_সংখ্যা-1);
    }
    ঘূৰাই edge_সংখ্যা;
}

শূন্য
StringTrieBuilder::Split_Branch_গ্ৰন্থি::লিখ(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ডান_Edge(first_Edge_সংখ্যা, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->লিখ(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.লিখ(unit);
}

তার্কিক
StringTrieBuilder::Branch_Head_গ্ৰন্থি::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    যদি(এতত্==&other) {
        ঘূৰাই সঁচা;
    }
    যদি(!মান_গ্ৰন্থি::operator==(other)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ Branch_Head_গ্ৰন্থি &o=static_cast<ধ্রুৱ Branch_Head_গ্ৰন্থি &>(other);
    ঘূৰাই length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_গ্ৰন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        offset=edge_সংখ্যা=next->mark_ডান_Edges_First(edge_সংখ্যা);
    }
    ঘূৰাই edge_সংখ্যা;
}

শূন্য
StringTrieBuilder::Branch_Head_গ্ৰন্থি::লিখ(StringTrieBuilder &builder) {
    next->লিখ(builder);
    যদি(length<=builder.getMinLinearMatch()) {
        offset=builder.write_মান_And_Type(has_মান, value, length-1);
    } অন্যথা {
        builder.লিখ(length-1);
        offset=builder.write_মান_And_Type(has_মান, value, 0);
    }
}

U_NAMESPACE_END
