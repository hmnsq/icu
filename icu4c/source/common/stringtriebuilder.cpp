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

#include "utypeinfo.h"  // for 'typeid' to work
#include "unicode/utypes.h"
#include "unicode/stringtriebuilder.h"
#include "uassert.h"
#include "uhash.h"

U_CDECL_BEGIN

static int32_t U_CALLCONV
hash_String_Trie_बुहोल(const UHashTok सुसि) {
    return icu::StringTrieBuilder::hash_बुहोल(सुसि.pointer);
}

static UBool U_CALLCONV
equalStringTrieNodes(const UHashTok key1, const UHashTok key2) {
    return icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

void
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    if(U_FAILURE(errorCode)) {
        return;
    }
    nodes=uhash_openSize(hash_String_Trie_बुहोल, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    if(U_SUCCESS(errorCode)) {
        if(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } else {
          uhash_set_सुसि_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

void
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

void
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    if(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_बुहोल(0, elementsLength, 0);
    } else /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *उगात=make_बुहोल(0, elementsLength, 0, errorCode);
        if(U_SUCCESS(errorCode)) {
            उगात->mark_कानान_Edges_First(-1);
            उगात->write(*this);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_बुहोल(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_हालागा=false;
    int32_t value=0;
    int32_t type;
    if(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_हालागा(start++);
        if(start==limit) {
            return write_हालागा_And_Final(value, true);  // final-value node
        }
        has_हालागा=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    if(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_बुहोल(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        while(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            write(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } else {
        // Branch node.
        int32_t length=बिलांग_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_बुहोल(start, limit, unitIndex, length);
        if(--length<getMinLinearMatch()) {
            type=length;
        } else {
            write(length);
            type=0;
        }
    }
    return write_हालागा_And_Type(has_हालागा, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_बुहोल(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t गित्ना_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    while(length>get_Max_Branch_Linear_Sub_बुहोल_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        गित्ना_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_बुहोल(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_बुहोल_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_बुहोल_Length-1];
    int32_t unit_नुमेरो=0;
    do {
        int32_t i=starts[unit_नुमेरो]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_नुमेरो]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } while(++unit_नुमेरो<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_नुमेरो]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_बुहोल_Length-1];
    do {
        --unit_नुमेरो;
        if(!isFinal[unit_नुमेरो]) {
            jumpTargets[unit_नुमेरो]=write_बुहोल(starts[unit_नुमेरो], starts[unit_नुमेरो+1], unitIndex+1);
        }
    } while(unit_नुमेरो>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_नुमेरो=length-1;
    write_बुहोल(start, limit, unitIndex+1);
    int32_t offset=write(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    while(--unit_नुमेरो>=0) {
        start=starts[unit_नुमेरो];
        int32_t value;
        if(isFinal[unit_नुमेरो]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_हालागा(start);
        } else {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_नुमेरो];
        }
        write_हालागा_And_Final(value, isFinal[unit_नुमेरो]);
        offset=write(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    while(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=write(गित्ना_Units[ltLength]);
    }
    return offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_बुहोल(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    if(U_FAILURE(errorCode)) {
        return nullptr;
    }
    UBool has_हालागा=false;
    int32_t value=0;
    if(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_हालागा(start++);
        if(start==limit) {
            return register_Final_हालागा(value, errorCode);
        }
        has_हालागा=true;
    }
    Node *बुहोल;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    if(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_बुहोल=make_बुहोल(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        while(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            बुहोल=create_Linear_Match_बुहोल(start, lastUnitIndex, maxLinearMatchLength, next_बुहोल);
            next_बुहोल=register_बुहोल(बुहोल, errorCode);
        }
        बुहोल=create_Linear_Match_बुहोल(start, unitIndex, length, next_बुहोल);
    } else {
        // Branch node.
        int32_t length=बिलांग_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_बुहोल=make_Branch_Sub_बुहोल(start, limit, unitIndex, length, errorCode);
        बुहोल=new Branch_Head_बुहोल(length, sub_बुहोल);
    }
    if(has_हालागा && बुहोल!=nullptr) {
        if(match_Nodes_Can_Have_मगा_हालागा()) {
            ((हालागा_बुहोल *)बुहोल)->set_हालागा(value);
        } else {
            बुहोल=new Intermediate_हालागा_बुहोल(value, register_बुहोल(बुहोल, errorCode));
        }
    }
    return register_बुहोल(बुहोल, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_बुहोल(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    if(U_FAILURE(errorCode)) {
        return nullptr;
    }
    char16_t गित्ना_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    while(length>get_Max_Branch_Linear_Sub_बुहोल_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        गित्ना_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_बुहोल(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    if(U_FAILURE(errorCode)) {
        return nullptr;
    }
    List_Branch_बुहोल *list_बुहोल=new List_Branch_बुहोल();
    if(list_बुहोल==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        return nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_नुमेरो=0;
    do {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        if(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_बुहोल->add(unit, get_Element_हालागा(start));
        } else {
            list_बुहोल->add(unit, make_बुहोल(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } while(++unit_नुमेरो<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    if(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_बुहोल->add(unit, get_Element_हालागा(start));
    } else {
        list_बुहोल->add(unit, make_बुहोल(start, limit, unitIndex+1, errorCode));
    }
    Node *बुहोल=register_बुहोल(list_बुहोल, errorCode);
    // Create the split-branch nodes.
    while(ltLength>0) {
        --ltLength;
        बुहोल=register_बुहोल(
            new Split_Branch_बुहोल(गित्ना_Units[ltLength], lessThan[ltLength], बुहोल), errorCode);
    }
    return बुहोल;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_बुहोल(Node* new_बुहोल, UErrorCode& errorCode) U_LIFETIME_BOUND {
    if(U_FAILURE(errorCode)) {
        delete new_बुहोल;
        return nullptr;
    }
    if(new_बुहोल==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        return nullptr;
    }
    const UHashElement *old=uhash_find(nodes, new_बुहोल);
    if(old!=nullptr) {
        delete new_बुहोल;
        return static_cast<Node*>(old->सुसि.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#if U_DEBUG
    int32_t old_हालागा=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_बुहोल, 1, &errorCode);
    U_ASSERT(old_हालागा==0);
    if(U_FAILURE(errorCode)) {
        delete new_बुहोल;
        return nullptr;
    }
    return new_बुहोल;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_हालागा(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    if(U_FAILURE(errorCode)) {
        return nullptr;
    }
    Final_हालागा_बुहोल सुसि(value);
    const UHashElement *old=uhash_find(nodes, &सुसि);
    if(old!=nullptr) {
        return static_cast<Node*>(old->सुसि.pointer);
    }
    Node *new_बुहोल=new Final_हालागा_बुहोल(value);
    if(new_बुहोल==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        return nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#if U_DEBUG
    int32_t old_हालागा=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_बुहोल, 1, &errorCode);
    U_ASSERT(old_हालागा==0);
    if(U_FAILURE(errorCode)) {
        delete new_बुहोल;
        return nullptr;
    }
    return new_बुहोल;
}

int32_t
StringTrieBuilder::hash_बुहोल(const void *बुहोल) {
    return static_cast<const Node*>(बुहोल)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(const void *left, const void *कानान) {
    return *static_cast<const Node*>(left) == *static_cast<const Node*>(कानान);
}

bool
StringTrieBuilder::Node::operator==(const Node &other) const {
    return this==&other || (typeid(*this)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_कानान_Edges_First(int32_t edge_नुमेरो) {
    if(offset==0) {
        offset=edge_नुमेरो;
    }
    return edge_नुमेरो;
}

bool
StringTrieBuilder::Final_हालागा_बुहोल::operator==(const Node &other) const {
    if(this==&other) {
        return true;
    }
    if(!Node::operator==(other)) {
        return false;
    }
    const Final_हालागा_बुहोल &o=static_cast<const Final_हालागा_बुहोल &>(other);
    return value==o.value;
}

void
StringTrieBuilder::Final_हालागा_बुहोल::write(StringTrieBuilder &builder) {
    offset=builder.write_हालागा_And_Final(value, true);
}

bool
StringTrieBuilder::हालागा_बुहोल::operator==(const Node &other) const {
    if(this==&other) {
        return true;
    }
    if(!Node::operator==(other)) {
        return false;
    }
    const हालागा_बुहोल &o=static_cast<const हालागा_बुहोल &>(other);
    return has_हालागा==o.has_हालागा && (!has_हालागा || value==o.value);
}

bool
StringTrieBuilder::Intermediate_हालागा_बुहोल::operator==(const Node &other) const {
    if(this==&other) {
        return true;
    }
    if(!हालागा_बुहोल::operator==(other)) {
        return false;
    }
    const Intermediate_हालागा_बुहोल &o=static_cast<const Intermediate_हालागा_बुहोल &>(other);
    return next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_हालागा_बुहोल::mark_कानान_Edges_First(int32_t edge_नुमेरो) {
    if(offset==0) {
        offset=edge_नुमेरो=next->mark_कानान_Edges_First(edge_नुमेरो);
    }
    return edge_नुमेरो;
}

void
StringTrieBuilder::Intermediate_हालागा_बुहोल::write(StringTrieBuilder &builder) {
    next->write(builder);
    offset=builder.write_हालागा_And_Final(value, false);
}

bool
StringTrieBuilder::Linear_Match_बुहोल::operator==(const Node &other) const {
    if(this==&other) {
        return true;
    }
    if(!हालागा_बुहोल::operator==(other)) {
        return false;
    }
    const Linear_Match_बुहोल &o=static_cast<const Linear_Match_बुहोल &>(other);
    return length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_बुहोल::mark_कानान_Edges_First(int32_t edge_नुमेरो) {
    if(offset==0) {
        offset=edge_नुमेरो=next->mark_कानान_Edges_First(edge_नुमेरो);
    }
    return edge_नुमेरो;
}

bool
StringTrieBuilder::List_Branch_बुहोल::operator==(const Node &other) const {
    if(this==&other) {
        return true;
    }
    if(!Node::operator==(other)) {
        return false;
    }
    const List_Branch_बुहोल &o=static_cast<const List_Branch_बुहोल &>(other);
    for(int32_t i=0; i<length; ++i) {
        if(units[i]!=o.units[i] || मगा_हालागा[i]!=o.मगा_हालागा[i] || equal[i]!=o.equal[i]) {
            return false;
        }
    }
    return true;
}

int32_t
StringTrieBuilder::List_Branch_बुहोल::mark_कानान_Edges_First(int32_t edge_नुमेरो) {
    if(offset==0) {
        first_Edge_नुमेरो=edge_नुमेरो;
        int32_t step=0;
        int32_t i=length;
        do {
            Node *edge=equal[--i];
            if(edge!=nullptr) {
                edge_नुमेरो=edge->mark_कानान_Edges_First(edge_नुमेरो-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } while(i>0);
        offset=edge_नुमेरो;
    }
    return edge_नुमेरो;
}

void
StringTrieBuilder::List_Branch_बुहोल::write(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_नुमेरो=length-1;
    Node *कानान_Edge=equal[unit_नुमेरो];
    int32_t कानान_Edge_नुमेरो= कानान_Edge==nullptr ? first_Edge_नुमेरो : कानान_Edge->getOffset();
    do {
        --unit_नुमेरो;
        if(equal[unit_नुमेरो]!=nullptr) {
            equal[unit_नुमेरो]->write_Unless_Inside_कानान_Edge(first_Edge_नुमेरो, कानान_Edge_नुमेरो, builder);
        }
    } while(unit_नुमेरो>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_नुमेरो=length-1;
    if(कानान_Edge==nullptr) {
        builder.write_हालागा_And_Final(मगा_हालागा[unit_नुमेरो], true);
    } else {
        कानान_Edge->write(builder);
    }
    offset=builder.write(units[unit_नुमेरो]);
    // Write the rest of this node's unit-value pairs.
    while(--unit_नुमेरो>=0) {
        int32_t value;
        UBool isFinal;
        if(equal[unit_नुमेरो]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=मगा_हालागा[unit_नुमेरो];
            isFinal=true;
        } else {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_नुमेरो]->getOffset()>0);
            value=offset-equal[unit_नुमेरो]->getOffset();
            isFinal=false;
        }
        builder.write_हालागा_And_Final(value, isFinal);
        offset=builder.write(units[unit_नुमेरो]);
    }
}

bool
StringTrieBuilder::Split_Branch_बुहोल::operator==(const Node &other) const {
    if(this==&other) {
        return true;
    }
    if(!Node::operator==(other)) {
        return false;
    }
    const Split_Branch_बुहोल &o=static_cast<const Split_Branch_बुहोल &>(other);
    return unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_बुहोल::mark_कानान_Edges_First(int32_t edge_नुमेरो) {
    if(offset==0) {
        first_Edge_नुमेरो=edge_नुमेरो;
        edge_नुमेरो=greaterOrEqual->mark_कानान_Edges_First(edge_नुमेरो);
        offset=edge_नुमेरो=lessThan->mark_कानान_Edges_First(edge_नुमेरो-1);
    }
    return edge_नुमेरो;
}

void
StringTrieBuilder::Split_Branch_बुहोल::write(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_कानान_Edge(first_Edge_नुमेरो, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->write(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.write(unit);
}

bool
StringTrieBuilder::Branch_Head_बुहोल::operator==(const Node &other) const {
    if(this==&other) {
        return true;
    }
    if(!हालागा_बुहोल::operator==(other)) {
        return false;
    }
    const Branch_Head_बुहोल &o=static_cast<const Branch_Head_बुहोल &>(other);
    return length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_बुहोल::mark_कानान_Edges_First(int32_t edge_नुमेरो) {
    if(offset==0) {
        offset=edge_नुमेरो=next->mark_कानान_Edges_First(edge_नुमेरो);
    }
    return edge_नुमेरो;
}

void
StringTrieBuilder::Branch_Head_बुहोल::write(StringTrieBuilder &builder) {
    next->write(builder);
    if(length<=builder.getMinLinearMatch()) {
        offset=builder.write_हालागा_And_Type(has_हालागा, value, length-1);
    } else {
        builder.write(length-1);
        offset=builder.write_हालागा_And_Type(has_हालागा, value, 0);
    }
}

U_NAMESPACE_END
