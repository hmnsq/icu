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
hashStringTrieNode(const UHashTok key) {
    ᮝᮜᮤ icu::StringTrieBuilder::hashNode(key.pointer);
}

static UBool U_CALLCONV
equalStringTrieNodes(const UHashTok key1, const UHashTok key2) {
    ᮝᮜᮤ icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

void
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        ᮝᮜᮤ;
    }
    nodes=uhash_openSize(hashStringTrieNode, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    ᮚᮦᮔ᮪(U_SUCCESS(errorCode)) {
        ᮚᮦᮔ᮪(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ᮜᮤᮚ {
          uhash_setKeyDeleter(nodes, uprv_deleteUObject);
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
    ᮚᮦᮔ᮪(buildOption==USTRINGTRIE_BUILD_FAST) {
        writeNode(0, elementsLength, 0);
    } ᮜᮤᮚ /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *root=makeNode(0, elementsLength, 0, errorCode);
        ᮚᮦᮔ᮪(U_SUCCESS(errorCode)) {
            root->markRightEdgesFirst(-1);
            root->write(*this);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::writeNode(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool hasValue=false;
    int32_t value=0;
    int32_t type;
    ᮚᮦᮔ᮪(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=getElementValue(start++);
        ᮚᮦᮔ᮪(start==limit) {
            ᮝᮜᮤ writeValueAndFinal(value, true);  // final-value node
        }
        hasValue=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    ᮚᮦᮔ᮪(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        writeNode(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ᮞᮨᮓᮨᮜ(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            write(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } ᮜᮤᮚ {
        // Branch node.
        int32_t length=countElementUnits(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        writeBranchSubNode(start, limit, unitIndex, length);
        ᮚᮦᮔ᮪(--length<getMinLinearMatch()) {
            type=length;
        } ᮜᮤᮚ {
            write(length);
            type=0;
        }
    }
    ᮝᮜᮤ writeValueAndType(hasValue, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::writeBranchSubNode(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t middleUnits[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ᮞᮨᮓᮨᮜ(length>getMaxBranchLinearSubNodeLength()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        middleUnits[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=writeBranchSubNode(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[kMaxBranchLinearSubNodeLength];
    UBool isFinal[kMaxBranchLinearSubNodeLength-1];
    int32_t unitNumber=0;
    do {
        int32_t i=starts[unitNumber]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unitNumber]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } ᮞᮨᮓᮨᮜ(++unitNumber<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unitNumber]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[kMaxBranchLinearSubNodeLength-1];
    do {
        --unitNumber;
        ᮚᮦᮔ᮪(!isFinal[unitNumber]) {
            jumpTargets[unitNumber]=writeNode(starts[unitNumber], starts[unitNumber+1], unitIndex+1);
        }
    } ᮞᮨᮓᮨᮜ(unitNumber>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unitNumber=length-1;
    writeNode(start, limit, unitIndex+1);
    int32_t offset=write(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    ᮞᮨᮓᮨᮜ(--unitNumber>=0) {
        start=starts[unitNumber];
        int32_t value;
        ᮚᮦᮔ᮪(isFinal[unitNumber]) {
            // Write the final value for the one string ending with this unit.
            value=getElementValue(start);
        } ᮜᮤᮚ {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unitNumber];
        }
        writeValueAndFinal(value, isFinal[unitNumber]);
        offset=write(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    ᮞᮨᮓᮨᮜ(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=write(middleUnits[ltLength]);
    }
    ᮝᮜᮤ offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::makeNode(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        ᮝᮜᮤ nullptr;
    }
    UBool hasValue=false;
    int32_t value=0;
    ᮚᮦᮔ᮪(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=getElementValue(start++);
        ᮚᮦᮔ᮪(start==limit) {
            ᮝᮜᮤ registerFinalValue(value, errorCode);
        }
        hasValue=true;
    }
    Node *node;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    ᮚᮦᮔ᮪(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *nextNode=makeNode(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ᮞᮨᮓᮨᮜ(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            node=createLinearMatchNode(start, lastUnitIndex, maxLinearMatchLength, nextNode);
            nextNode=registerNode(node, errorCode);
        }
        node=createLinearMatchNode(start, unitIndex, length, nextNode);
    } ᮜᮤᮚ {
        // Branch node.
        int32_t length=countElementUnits(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *subNode=makeBranchSubNode(start, limit, unitIndex, length, errorCode);
        node=new BranchHeadNode(length, subNode);
    }
    ᮚᮦᮔ᮪(hasValue && node!=nullptr) {
        ᮚᮦᮔ᮪(matchNodesCanHaveValues()) {
            ((ValueNode *)node)->setValue(value);
        } ᮜᮤᮚ {
            node=new IntermediateValueNode(value, registerNode(node, errorCode));
        }
    }
    ᮝᮜᮤ registerNode(node, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::makeBranchSubNode(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        ᮝᮜᮤ nullptr;
    }
    char16_t middleUnits[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ᮞᮨᮓᮨᮜ(length>getMaxBranchLinearSubNodeLength()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        middleUnits[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=makeBranchSubNode(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        ᮝᮜᮤ nullptr;
    }
    ListBranchNode *listNode=new ListBranchNode();
    ᮚᮦᮔ᮪(listNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ᮝᮜᮤ nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unitNumber=0;
    do {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        ᮚᮦᮔ᮪(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            listNode->add(unit, getElementValue(start));
        } ᮜᮤᮚ {
            listNode->add(unit, makeNode(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } ᮞᮨᮓᮨᮜ(++unitNumber<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    ᮚᮦᮔ᮪(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        listNode->add(unit, getElementValue(start));
    } ᮜᮤᮚ {
        listNode->add(unit, makeNode(start, limit, unitIndex+1, errorCode));
    }
    Node *node=registerNode(listNode, errorCode);
    // Create the split-branch nodes.
    ᮞᮨᮓᮨᮜ(ltLength>0) {
        --ltLength;
        node=registerNode(
            new SplitBranchNode(middleUnits[ltLength], lessThan[ltLength], node), errorCode);
    }
    ᮝᮜᮤ node;
}

StringTrieBuilder::Node*
StringTrieBuilder::registerNode(Node* newNode, UErrorCode& errorCode) U_LIFETIME_BOUND {
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        delete newNode;
        ᮝᮜᮤ nullptr;
    }
    ᮚᮦᮔ᮪(newNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ᮝᮜᮤ nullptr;
    }
    const UHashElement *old=uhash_find(nodes, newNode);
    ᮚᮦᮔ᮪(old!=nullptr) {
        delete newNode;
        ᮝᮜᮤ static_cast<Node*>(old->key.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ᮚᮦᮔ᮪ U_DEBUG
    int32_t oldValue=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, newNode, 1, &errorCode);
    U_ASSERT(oldValue==0);
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        delete newNode;
        ᮝᮜᮤ nullptr;
    }
    ᮝᮜᮤ newNode;
}

StringTrieBuilder::Node*
StringTrieBuilder::registerFinalValue(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        ᮝᮜᮤ nullptr;
    }
    FinalValueNode key(value);
    const UHashElement *old=uhash_find(nodes, &key);
    ᮚᮦᮔ᮪(old!=nullptr) {
        ᮝᮜᮤ static_cast<Node*>(old->key.pointer);
    }
    Node *newNode=new FinalValueNode(value);
    ᮚᮦᮔ᮪(newNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ᮝᮜᮤ nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ᮚᮦᮔ᮪ U_DEBUG
    int32_t oldValue=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, newNode, 1, &errorCode);
    U_ASSERT(oldValue==0);
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        delete newNode;
        ᮝᮜᮤ nullptr;
    }
    ᮝᮜᮤ newNode;
}

int32_t
StringTrieBuilder::hashNode(const void *node) {
    ᮝᮜᮤ static_cast<const Node*>(node)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(const void *left, const void *right) {
    ᮝᮜᮤ *static_cast<const Node*>(left) == *static_cast<const Node*>(right);
}

bool
StringTrieBuilder::Node::operator==(const Node &other) const {
    ᮝᮜᮤ this==&other || (typeid(*this)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::markRightEdgesFirst(int32_t edgeNumber) {
    ᮚᮦᮔ᮪(offset==0) {
        offset=edgeNumber;
    }
    ᮝᮜᮤ edgeNumber;
}

bool
StringTrieBuilder::FinalValueNode::operator==(const Node &other) const {
    ᮚᮦᮔ᮪(this==&other) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪(!Node::operator==(other)) {
        ᮝᮜᮤ false;
    }
    const FinalValueNode &o=static_cast<const FinalValueNode &>(other);
    ᮝᮜᮤ value==o.value;
}

void
StringTrieBuilder::FinalValueNode::write(StringTrieBuilder &builder) {
    offset=builder.writeValueAndFinal(value, true);
}

bool
StringTrieBuilder::ValueNode::operator==(const Node &other) const {
    ᮚᮦᮔ᮪(this==&other) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪(!Node::operator==(other)) {
        ᮝᮜᮤ false;
    }
    const ValueNode &o=static_cast<const ValueNode &>(other);
    ᮝᮜᮤ hasValue==o.hasValue && (!hasValue || value==o.value);
}

bool
StringTrieBuilder::IntermediateValueNode::operator==(const Node &other) const {
    ᮚᮦᮔ᮪(this==&other) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪(!ValueNode::operator==(other)) {
        ᮝᮜᮤ false;
    }
    const IntermediateValueNode &o=static_cast<const IntermediateValueNode &>(other);
    ᮝᮜᮤ next==o.next;
}

int32_t
StringTrieBuilder::IntermediateValueNode::markRightEdgesFirst(int32_t edgeNumber) {
    ᮚᮦᮔ᮪(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    ᮝᮜᮤ edgeNumber;
}

void
StringTrieBuilder::IntermediateValueNode::write(StringTrieBuilder &builder) {
    next->write(builder);
    offset=builder.writeValueAndFinal(value, false);
}

bool
StringTrieBuilder::LinearMatchNode::operator==(const Node &other) const {
    ᮚᮦᮔ᮪(this==&other) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪(!ValueNode::operator==(other)) {
        ᮝᮜᮤ false;
    }
    const LinearMatchNode &o=static_cast<const LinearMatchNode &>(other);
    ᮝᮜᮤ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::LinearMatchNode::markRightEdgesFirst(int32_t edgeNumber) {
    ᮚᮦᮔ᮪(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    ᮝᮜᮤ edgeNumber;
}

bool
StringTrieBuilder::ListBranchNode::operator==(const Node &other) const {
    ᮚᮦᮔ᮪(this==&other) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪(!Node::operator==(other)) {
        ᮝᮜᮤ false;
    }
    const ListBranchNode &o=static_cast<const ListBranchNode &>(other);
    for(int32_t i=0; i<length; ++i) {
        ᮚᮦᮔ᮪(units[i]!=o.units[i] || values[i]!=o.values[i] || equal[i]!=o.equal[i]) {
            ᮝᮜᮤ false;
        }
    }
    ᮝᮜᮤ true;
}

int32_t
StringTrieBuilder::ListBranchNode::markRightEdgesFirst(int32_t edgeNumber) {
    ᮚᮦᮔ᮪(offset==0) {
        firstEdgeNumber=edgeNumber;
        int32_t step=0;
        int32_t i=length;
        do {
            Node *edge=equal[--i];
            ᮚᮦᮔ᮪(edge!=nullptr) {
                edgeNumber=edge->markRightEdgesFirst(edgeNumber-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } ᮞᮨᮓᮨᮜ(i>0);
        offset=edgeNumber;
    }
    ᮝᮜᮤ edgeNumber;
}

void
StringTrieBuilder::ListBranchNode::write(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unitNumber=length-1;
    Node *rightEdge=equal[unitNumber];
    int32_t rightEdgeNumber= rightEdge==nullptr ? firstEdgeNumber : rightEdge->getOffset();
    do {
        --unitNumber;
        ᮚᮦᮔ᮪(equal[unitNumber]!=nullptr) {
            equal[unitNumber]->writeUnlessInsideRightEdge(firstEdgeNumber, rightEdgeNumber, builder);
        }
    } ᮞᮨᮓᮨᮜ(unitNumber>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unitNumber=length-1;
    ᮚᮦᮔ᮪(rightEdge==nullptr) {
        builder.writeValueAndFinal(values[unitNumber], true);
    } ᮜᮤᮚ {
        rightEdge->write(builder);
    }
    offset=builder.write(units[unitNumber]);
    // Write the rest of this node's unit-value pairs.
    ᮞᮨᮓᮨᮜ(--unitNumber>=0) {
        int32_t value;
        UBool isFinal;
        ᮚᮦᮔ᮪(equal[unitNumber]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=values[unitNumber];
            isFinal=true;
        } ᮜᮤᮚ {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unitNumber]->getOffset()>0);
            value=offset-equal[unitNumber]->getOffset();
            isFinal=false;
        }
        builder.writeValueAndFinal(value, isFinal);
        offset=builder.write(units[unitNumber]);
    }
}

bool
StringTrieBuilder::SplitBranchNode::operator==(const Node &other) const {
    ᮚᮦᮔ᮪(this==&other) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪(!Node::operator==(other)) {
        ᮝᮜᮤ false;
    }
    const SplitBranchNode &o=static_cast<const SplitBranchNode &>(other);
    ᮝᮜᮤ unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::SplitBranchNode::markRightEdgesFirst(int32_t edgeNumber) {
    ᮚᮦᮔ᮪(offset==0) {
        firstEdgeNumber=edgeNumber;
        edgeNumber=greaterOrEqual->markRightEdgesFirst(edgeNumber);
        offset=edgeNumber=lessThan->markRightEdgesFirst(edgeNumber-1);
    }
    ᮝᮜᮤ edgeNumber;
}

void
StringTrieBuilder::SplitBranchNode::write(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->writeUnlessInsideRightEdge(firstEdgeNumber, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->write(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.write(unit);
}

bool
StringTrieBuilder::BranchHeadNode::operator==(const Node &other) const {
    ᮚᮦᮔ᮪(this==&other) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪(!ValueNode::operator==(other)) {
        ᮝᮜᮤ false;
    }
    const BranchHeadNode &o=static_cast<const BranchHeadNode &>(other);
    ᮝᮜᮤ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::BranchHeadNode::markRightEdgesFirst(int32_t edgeNumber) {
    ᮚᮦᮔ᮪(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    ᮝᮜᮤ edgeNumber;
}

void
StringTrieBuilder::BranchHeadNode::write(StringTrieBuilder &builder) {
    next->write(builder);
    ᮚᮦᮔ᮪(length<=builder.getMinLinearMatch()) {
        offset=builder.writeValueAndType(hasValue, value, length-1);
    } ᮜᮤᮚ {
        builder.write(length-1);
        offset=builder.writeValueAndType(hasValue, value, 0);
    }
}

U_NAMESPACE_END
