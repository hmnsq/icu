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
    trả_về icu::StringTrieBuilder::hashNode(key.pointer);
}

static UBool U_CALLCONV
equalStringTrieNodes(const UHashTok key1, const UHashTok key2) {
    trả_về icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

void
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    nếu(U_FAILURE(errorCode)) {
        trả_về;
    }
    nodes=uhash_openSize(hashStringTrieNode, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    nếu(U_SUCCESS(errorCode)) {
        nếu(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } không_thì {
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
    nếu(buildOption==USTRINGTRIE_BUILD_FAST) {
        writeNode(0, elementsLength, 0);
    } không_thì /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *root=makeNode(0, elementsLength, 0, errorCode);
        nếu(U_SUCCESS(errorCode)) {
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
    nếu(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=getElementValue(start++);
        nếu(start==limit) {
            trả_về writeValueAndFinal(value, true);  // final-value node
        }
        hasValue=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    nếu(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        writeNode(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        trong_khi(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            write(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } không_thì {
        // Branch node.
        int32_t length=countElementUnits(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        writeBranchSubNode(start, limit, unitIndex, length);
        nếu(--length<getMinLinearMatch()) {
            type=length;
        } không_thì {
            write(length);
            type=0;
        }
    }
    trả_về writeValueAndType(hasValue, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::writeBranchSubNode(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t middleUnits[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    trong_khi(length>getMaxBranchLinearSubNodeLength()) {
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
    } trong_khi(++unitNumber<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unitNumber]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[kMaxBranchLinearSubNodeLength-1];
    do {
        --unitNumber;
        nếu(!isFinal[unitNumber]) {
            jumpTargets[unitNumber]=writeNode(starts[unitNumber], starts[unitNumber+1], unitIndex+1);
        }
    } trong_khi(unitNumber>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unitNumber=length-1;
    writeNode(start, limit, unitIndex+1);
    int32_t offset=write(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    trong_khi(--unitNumber>=0) {
        start=starts[unitNumber];
        int32_t value;
        nếu(isFinal[unitNumber]) {
            // Write the final value for the one string ending with this unit.
            value=getElementValue(start);
        } không_thì {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unitNumber];
        }
        writeValueAndFinal(value, isFinal[unitNumber]);
        offset=write(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    trong_khi(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=write(middleUnits[ltLength]);
    }
    trả_về offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::makeNode(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    nếu(U_FAILURE(errorCode)) {
        trả_về nullptr;
    }
    UBool hasValue=false;
    int32_t value=0;
    nếu(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=getElementValue(start++);
        nếu(start==limit) {
            trả_về registerFinalValue(value, errorCode);
        }
        hasValue=true;
    }
    Node *node;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    nếu(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *nextNode=makeNode(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        trong_khi(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            node=createLinearMatchNode(start, lastUnitIndex, maxLinearMatchLength, nextNode);
            nextNode=registerNode(node, errorCode);
        }
        node=createLinearMatchNode(start, unitIndex, length, nextNode);
    } không_thì {
        // Branch node.
        int32_t length=countElementUnits(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *subNode=makeBranchSubNode(start, limit, unitIndex, length, errorCode);
        node=new BranchHeadNode(length, subNode);
    }
    nếu(hasValue && node!=nullptr) {
        nếu(matchNodesCanHaveValues()) {
            ((ValueNode *)node)->setValue(value);
        } không_thì {
            node=new IntermediateValueNode(value, registerNode(node, errorCode));
        }
    }
    trả_về registerNode(node, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::makeBranchSubNode(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    nếu(U_FAILURE(errorCode)) {
        trả_về nullptr;
    }
    char16_t middleUnits[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    trong_khi(length>getMaxBranchLinearSubNodeLength()) {
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
    nếu(U_FAILURE(errorCode)) {
        trả_về nullptr;
    }
    ListBranchNode *listNode=new ListBranchNode();
    nếu(listNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        trả_về nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unitNumber=0;
    do {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        nếu(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            listNode->add(unit, getElementValue(start));
        } không_thì {
            listNode->add(unit, makeNode(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } trong_khi(++unitNumber<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    nếu(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        listNode->add(unit, getElementValue(start));
    } không_thì {
        listNode->add(unit, makeNode(start, limit, unitIndex+1, errorCode));
    }
    Node *node=registerNode(listNode, errorCode);
    // Create the split-branch nodes.
    trong_khi(ltLength>0) {
        --ltLength;
        node=registerNode(
            new SplitBranchNode(middleUnits[ltLength], lessThan[ltLength], node), errorCode);
    }
    trả_về node;
}

StringTrieBuilder::Node*
StringTrieBuilder::registerNode(Node* newNode, UErrorCode& errorCode) U_LIFETIME_BOUND {
    nếu(U_FAILURE(errorCode)) {
        delete newNode;
        trả_về nullptr;
    }
    nếu(newNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        trả_về nullptr;
    }
    const UHashElement *old=uhash_find(nodes, newNode);
    nếu(old!=nullptr) {
        delete newNode;
        trả_về static_cast<Node*>(old->key.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#nếu U_DEBUG
    int32_t oldValue=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, newNode, 1, &errorCode);
    U_ASSERT(oldValue==0);
    nếu(U_FAILURE(errorCode)) {
        delete newNode;
        trả_về nullptr;
    }
    trả_về newNode;
}

StringTrieBuilder::Node*
StringTrieBuilder::registerFinalValue(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    nếu(U_FAILURE(errorCode)) {
        trả_về nullptr;
    }
    FinalValueNode key(value);
    const UHashElement *old=uhash_find(nodes, &key);
    nếu(old!=nullptr) {
        trả_về static_cast<Node*>(old->key.pointer);
    }
    Node *newNode=new FinalValueNode(value);
    nếu(newNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        trả_về nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#nếu U_DEBUG
    int32_t oldValue=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, newNode, 1, &errorCode);
    U_ASSERT(oldValue==0);
    nếu(U_FAILURE(errorCode)) {
        delete newNode;
        trả_về nullptr;
    }
    trả_về newNode;
}

int32_t
StringTrieBuilder::hashNode(const void *node) {
    trả_về static_cast<const Node*>(node)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(const void *left, const void *right) {
    trả_về *static_cast<const Node*>(left) == *static_cast<const Node*>(right);
}

bool
StringTrieBuilder::Node::operator==(const Node &other) const {
    trả_về this==&other || (typeid(*this)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::markRightEdgesFirst(int32_t edgeNumber) {
    nếu(offset==0) {
        offset=edgeNumber;
    }
    trả_về edgeNumber;
}

bool
StringTrieBuilder::FinalValueNode::operator==(const Node &other) const {
    nếu(this==&other) {
        trả_về true;
    }
    nếu(!Node::operator==(other)) {
        trả_về false;
    }
    const FinalValueNode &o=static_cast<const FinalValueNode &>(other);
    trả_về value==o.value;
}

void
StringTrieBuilder::FinalValueNode::write(StringTrieBuilder &builder) {
    offset=builder.writeValueAndFinal(value, true);
}

bool
StringTrieBuilder::ValueNode::operator==(const Node &other) const {
    nếu(this==&other) {
        trả_về true;
    }
    nếu(!Node::operator==(other)) {
        trả_về false;
    }
    const ValueNode &o=static_cast<const ValueNode &>(other);
    trả_về hasValue==o.hasValue && (!hasValue || value==o.value);
}

bool
StringTrieBuilder::IntermediateValueNode::operator==(const Node &other) const {
    nếu(this==&other) {
        trả_về true;
    }
    nếu(!ValueNode::operator==(other)) {
        trả_về false;
    }
    const IntermediateValueNode &o=static_cast<const IntermediateValueNode &>(other);
    trả_về next==o.next;
}

int32_t
StringTrieBuilder::IntermediateValueNode::markRightEdgesFirst(int32_t edgeNumber) {
    nếu(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    trả_về edgeNumber;
}

void
StringTrieBuilder::IntermediateValueNode::write(StringTrieBuilder &builder) {
    next->write(builder);
    offset=builder.writeValueAndFinal(value, false);
}

bool
StringTrieBuilder::LinearMatchNode::operator==(const Node &other) const {
    nếu(this==&other) {
        trả_về true;
    }
    nếu(!ValueNode::operator==(other)) {
        trả_về false;
    }
    const LinearMatchNode &o=static_cast<const LinearMatchNode &>(other);
    trả_về length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::LinearMatchNode::markRightEdgesFirst(int32_t edgeNumber) {
    nếu(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    trả_về edgeNumber;
}

bool
StringTrieBuilder::ListBranchNode::operator==(const Node &other) const {
    nếu(this==&other) {
        trả_về true;
    }
    nếu(!Node::operator==(other)) {
        trả_về false;
    }
    const ListBranchNode &o=static_cast<const ListBranchNode &>(other);
    cho(int32_t i=0; i<length; ++i) {
        nếu(units[i]!=o.units[i] || values[i]!=o.values[i] || equal[i]!=o.equal[i]) {
            trả_về false;
        }
    }
    trả_về true;
}

int32_t
StringTrieBuilder::ListBranchNode::markRightEdgesFirst(int32_t edgeNumber) {
    nếu(offset==0) {
        firstEdgeNumber=edgeNumber;
        int32_t step=0;
        int32_t i=length;
        do {
            Node *edge=equal[--i];
            nếu(edge!=nullptr) {
                edgeNumber=edge->markRightEdgesFirst(edgeNumber-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } trong_khi(i>0);
        offset=edgeNumber;
    }
    trả_về edgeNumber;
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
        nếu(equal[unitNumber]!=nullptr) {
            equal[unitNumber]->writeUnlessInsideRightEdge(firstEdgeNumber, rightEdgeNumber, builder);
        }
    } trong_khi(unitNumber>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unitNumber=length-1;
    nếu(rightEdge==nullptr) {
        builder.writeValueAndFinal(values[unitNumber], true);
    } không_thì {
        rightEdge->write(builder);
    }
    offset=builder.write(units[unitNumber]);
    // Write the rest of this node's unit-value pairs.
    trong_khi(--unitNumber>=0) {
        int32_t value;
        UBool isFinal;
        nếu(equal[unitNumber]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=values[unitNumber];
            isFinal=true;
        } không_thì {
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
    nếu(this==&other) {
        trả_về true;
    }
    nếu(!Node::operator==(other)) {
        trả_về false;
    }
    const SplitBranchNode &o=static_cast<const SplitBranchNode &>(other);
    trả_về unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::SplitBranchNode::markRightEdgesFirst(int32_t edgeNumber) {
    nếu(offset==0) {
        firstEdgeNumber=edgeNumber;
        edgeNumber=greaterOrEqual->markRightEdgesFirst(edgeNumber);
        offset=edgeNumber=lessThan->markRightEdgesFirst(edgeNumber-1);
    }
    trả_về edgeNumber;
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
    nếu(this==&other) {
        trả_về true;
    }
    nếu(!ValueNode::operator==(other)) {
        trả_về false;
    }
    const BranchHeadNode &o=static_cast<const BranchHeadNode &>(other);
    trả_về length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::BranchHeadNode::markRightEdgesFirst(int32_t edgeNumber) {
    nếu(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    trả_về edgeNumber;
}

void
StringTrieBuilder::BranchHeadNode::write(StringTrieBuilder &builder) {
    next->write(builder);
    nếu(length<=builder.getMinLinearMatch()) {
        offset=builder.writeValueAndType(hasValue, value, length-1);
    } không_thì {
        builder.write(length-1);
        offset=builder.writeValueAndType(hasValue, value, 0);
    }
}

U_NAMESPACE_END
