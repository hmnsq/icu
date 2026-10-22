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
    ត្រឡប់ icu::StringTrieBuilder::hashNode(key.pointer);
}

static UBool U_CALLCONV
equalStringTrieNodes(const UHashTok key1, const UHashTok key2) {
    ត្រឡប់ icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

void
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់;
    }
    nodes=uhash_openSize(hashStringTrieNode, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    ប្រសិនបើ(U_SUCCESS(errorCode)) {
        ប្រសិនបើ(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } បើមិនដូច្នេះ {
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
    ប្រសិនបើ(buildOption==USTRINGTRIE_BUILD_FAST) {
        writeNode(0, elementsLength, 0);
    } បើមិនដូច្នេះ /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *root=makeNode(0, elementsLength, 0, errorCode);
        ប្រសិនបើ(U_SUCCESS(errorCode)) {
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
    ប្រសិនបើ(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=getElementValue(start++);
        ប្រសិនបើ(start==limit) {
            ត្រឡប់ writeValueAndFinal(value, true);  // final-value node
        }
        hasValue=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    ប្រសិនបើ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        writeNode(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ខណៈពេល(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            write(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } បើមិនដូច្នេះ {
        // Branch node.
        int32_t length=countElementUnits(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        writeBranchSubNode(start, limit, unitIndex, length);
        ប្រសិនបើ(--length<getMinLinearMatch()) {
            type=length;
        } បើមិនដូច្នេះ {
            write(length);
            type=0;
        }
    }
    ត្រឡប់ writeValueAndType(hasValue, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::writeBranchSubNode(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t middleUnits[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ខណៈពេល(length>getMaxBranchLinearSubNodeLength()) {
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
    } ខណៈពេល(++unitNumber<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unitNumber]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[kMaxBranchLinearSubNodeLength-1];
    do {
        --unitNumber;
        ប្រសិនបើ(!isFinal[unitNumber]) {
            jumpTargets[unitNumber]=writeNode(starts[unitNumber], starts[unitNumber+1], unitIndex+1);
        }
    } ខណៈពេល(unitNumber>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unitNumber=length-1;
    writeNode(start, limit, unitIndex+1);
    int32_t offset=write(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    ខណៈពេល(--unitNumber>=0) {
        start=starts[unitNumber];
        int32_t value;
        ប្រសិនបើ(isFinal[unitNumber]) {
            // Write the final value for the one string ending with this unit.
            value=getElementValue(start);
        } បើមិនដូច្នេះ {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unitNumber];
        }
        writeValueAndFinal(value, isFinal[unitNumber]);
        offset=write(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    ខណៈពេល(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=write(middleUnits[ltLength]);
    }
    ត្រឡប់ offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::makeNode(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ nullptr;
    }
    UBool hasValue=false;
    int32_t value=0;
    ប្រសិនបើ(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=getElementValue(start++);
        ប្រសិនបើ(start==limit) {
            ត្រឡប់ registerFinalValue(value, errorCode);
        }
        hasValue=true;
    }
    Node *node;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    ប្រសិនបើ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *nextNode=makeNode(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        ខណៈពេល(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            node=createLinearMatchNode(start, lastUnitIndex, maxLinearMatchLength, nextNode);
            nextNode=registerNode(node, errorCode);
        }
        node=createLinearMatchNode(start, unitIndex, length, nextNode);
    } បើមិនដូច្នេះ {
        // Branch node.
        int32_t length=countElementUnits(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *subNode=makeBranchSubNode(start, limit, unitIndex, length, errorCode);
        node=new BranchHeadNode(length, subNode);
    }
    ប្រសិនបើ(hasValue && node!=nullptr) {
        ប្រសិនបើ(matchNodesCanHaveValues()) {
            ((ValueNode *)node)->setValue(value);
        } បើមិនដូច្នេះ {
            node=new IntermediateValueNode(value, registerNode(node, errorCode));
        }
    }
    ត្រឡប់ registerNode(node, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::makeBranchSubNode(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ nullptr;
    }
    char16_t middleUnits[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    ខណៈពេល(length>getMaxBranchLinearSubNodeLength()) {
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
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ nullptr;
    }
    ListBranchNode *listNode=new ListBranchNode();
    ប្រសិនបើ(listNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unitNumber=0;
    do {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        ប្រសិនបើ(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            listNode->add(unit, getElementValue(start));
        } បើមិនដូច្នេះ {
            listNode->add(unit, makeNode(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } ខណៈពេល(++unitNumber<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    ប្រសិនបើ(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        listNode->add(unit, getElementValue(start));
    } បើមិនដូច្នេះ {
        listNode->add(unit, makeNode(start, limit, unitIndex+1, errorCode));
    }
    Node *node=registerNode(listNode, errorCode);
    // Create the split-branch nodes.
    ខណៈពេល(ltLength>0) {
        --ltLength;
        node=registerNode(
            new SplitBranchNode(middleUnits[ltLength], lessThan[ltLength], node), errorCode);
    }
    ត្រឡប់ node;
}

StringTrieBuilder::Node*
StringTrieBuilder::registerNode(Node* newNode, UErrorCode& errorCode) U_LIFETIME_BOUND {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        delete newNode;
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ(newNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ nullptr;
    }
    const UHashElement *old=uhash_find(nodes, newNode);
    ប្រសិនបើ(old!=nullptr) {
        delete newNode;
        ត្រឡប់ static_cast<Node*>(old->key.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ប្រសិនបើ U_DEBUG
    int32_t oldValue=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, newNode, 1, &errorCode);
    U_ASSERT(oldValue==0);
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        delete newNode;
        ត្រឡប់ nullptr;
    }
    ត្រឡប់ newNode;
}

StringTrieBuilder::Node*
StringTrieBuilder::registerFinalValue(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ nullptr;
    }
    FinalValueNode key(value);
    const UHashElement *old=uhash_find(nodes, &key);
    ប្រសិនបើ(old!=nullptr) {
        ត្រឡប់ static_cast<Node*>(old->key.pointer);
    }
    Node *newNode=new FinalValueNode(value);
    ប្រសិនបើ(newNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ប្រសិនបើ U_DEBUG
    int32_t oldValue=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, newNode, 1, &errorCode);
    U_ASSERT(oldValue==0);
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        delete newNode;
        ត្រឡប់ nullptr;
    }
    ត្រឡប់ newNode;
}

int32_t
StringTrieBuilder::hashNode(const void *node) {
    ត្រឡប់ static_cast<const Node*>(node)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(const void *left, const void *right) {
    ត្រឡប់ *static_cast<const Node*>(left) == *static_cast<const Node*>(right);
}

bool
StringTrieBuilder::Node::operator==(const Node &other) const {
    ត្រឡប់ this==&other || (typeid(*this)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::markRightEdgesFirst(int32_t edgeNumber) {
    ប្រសិនបើ(offset==0) {
        offset=edgeNumber;
    }
    ត្រឡប់ edgeNumber;
}

bool
StringTrieBuilder::FinalValueNode::operator==(const Node &other) const {
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ប្រសិនបើ(!Node::operator==(other)) {
        ត្រឡប់ false;
    }
    const FinalValueNode &o=static_cast<const FinalValueNode &>(other);
    ត្រឡប់ value==o.value;
}

void
StringTrieBuilder::FinalValueNode::write(StringTrieBuilder &builder) {
    offset=builder.writeValueAndFinal(value, true);
}

bool
StringTrieBuilder::ValueNode::operator==(const Node &other) const {
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ប្រសិនបើ(!Node::operator==(other)) {
        ត្រឡប់ false;
    }
    const ValueNode &o=static_cast<const ValueNode &>(other);
    ត្រឡប់ hasValue==o.hasValue && (!hasValue || value==o.value);
}

bool
StringTrieBuilder::IntermediateValueNode::operator==(const Node &other) const {
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ប្រសិនបើ(!ValueNode::operator==(other)) {
        ត្រឡប់ false;
    }
    const IntermediateValueNode &o=static_cast<const IntermediateValueNode &>(other);
    ត្រឡប់ next==o.next;
}

int32_t
StringTrieBuilder::IntermediateValueNode::markRightEdgesFirst(int32_t edgeNumber) {
    ប្រសិនបើ(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    ត្រឡប់ edgeNumber;
}

void
StringTrieBuilder::IntermediateValueNode::write(StringTrieBuilder &builder) {
    next->write(builder);
    offset=builder.writeValueAndFinal(value, false);
}

bool
StringTrieBuilder::LinearMatchNode::operator==(const Node &other) const {
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ប្រសិនបើ(!ValueNode::operator==(other)) {
        ត្រឡប់ false;
    }
    const LinearMatchNode &o=static_cast<const LinearMatchNode &>(other);
    ត្រឡប់ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::LinearMatchNode::markRightEdgesFirst(int32_t edgeNumber) {
    ប្រសិនបើ(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    ត្រឡប់ edgeNumber;
}

bool
StringTrieBuilder::ListBranchNode::operator==(const Node &other) const {
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ប្រសិនបើ(!Node::operator==(other)) {
        ត្រឡប់ false;
    }
    const ListBranchNode &o=static_cast<const ListBranchNode &>(other);
    សម្រាប់(int32_t i=0; i<length; ++i) {
        ប្រសិនបើ(units[i]!=o.units[i] || values[i]!=o.values[i] || equal[i]!=o.equal[i]) {
            ត្រឡប់ false;
        }
    }
    ត្រឡប់ true;
}

int32_t
StringTrieBuilder::ListBranchNode::markRightEdgesFirst(int32_t edgeNumber) {
    ប្រសិនបើ(offset==0) {
        firstEdgeNumber=edgeNumber;
        int32_t step=0;
        int32_t i=length;
        do {
            Node *edge=equal[--i];
            ប្រសិនបើ(edge!=nullptr) {
                edgeNumber=edge->markRightEdgesFirst(edgeNumber-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } ខណៈពេល(i>0);
        offset=edgeNumber;
    }
    ត្រឡប់ edgeNumber;
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
        ប្រសិនបើ(equal[unitNumber]!=nullptr) {
            equal[unitNumber]->writeUnlessInsideRightEdge(firstEdgeNumber, rightEdgeNumber, builder);
        }
    } ខណៈពេល(unitNumber>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unitNumber=length-1;
    ប្រសិនបើ(rightEdge==nullptr) {
        builder.writeValueAndFinal(values[unitNumber], true);
    } បើមិនដូច្នេះ {
        rightEdge->write(builder);
    }
    offset=builder.write(units[unitNumber]);
    // Write the rest of this node's unit-value pairs.
    ខណៈពេល(--unitNumber>=0) {
        int32_t value;
        UBool isFinal;
        ប្រសិនបើ(equal[unitNumber]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=values[unitNumber];
            isFinal=true;
        } បើមិនដូច្នេះ {
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
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ប្រសិនបើ(!Node::operator==(other)) {
        ត្រឡប់ false;
    }
    const SplitBranchNode &o=static_cast<const SplitBranchNode &>(other);
    ត្រឡប់ unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::SplitBranchNode::markRightEdgesFirst(int32_t edgeNumber) {
    ប្រសិនបើ(offset==0) {
        firstEdgeNumber=edgeNumber;
        edgeNumber=greaterOrEqual->markRightEdgesFirst(edgeNumber);
        offset=edgeNumber=lessThan->markRightEdgesFirst(edgeNumber-1);
    }
    ត្រឡប់ edgeNumber;
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
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ប្រសិនបើ(!ValueNode::operator==(other)) {
        ត្រឡប់ false;
    }
    const BranchHeadNode &o=static_cast<const BranchHeadNode &>(other);
    ត្រឡប់ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::BranchHeadNode::markRightEdgesFirst(int32_t edgeNumber) {
    ប្រសិនបើ(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    ត្រឡប់ edgeNumber;
}

void
StringTrieBuilder::BranchHeadNode::write(StringTrieBuilder &builder) {
    next->write(builder);
    ប្រសិនបើ(length<=builder.getMinLinearMatch()) {
        offset=builder.writeValueAndType(hasValue, value, length-1);
    } បើមិនដូច្នេះ {
        builder.write(length-1);
        offset=builder.writeValueAndType(hasValue, value, 0);
    }
}

U_NAMESPACE_END
