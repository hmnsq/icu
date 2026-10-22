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

#ضمن "utypeinfo.h"  // for 'typeid' to work
#ضمن "unicode/utypes.h"
#ضمن "unicode/stringtriebuilder.h"
#ضمن "uassert.h"
#ضمن "uhash.h"

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
hashStringTrieNode(ثابت UHashTok key) {
    החזר icu::StringTrieBuilder::hashNode(key.pointer);
}

ساكن UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    החזר icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

فراغ
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר;
    }
    nodes=uhash_openSize(hashStringTrieNode, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    אם(U_SUCCESS(errorCode)) {
        אם(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } אחרת {
          uhash_setKeyDeleter(nodes, uprv_deleteUObject);
        }
    }
}

فراغ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

فراغ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    אם(buildOption==USTRINGTRIE_BUILD_FAST) {
        writeNode(0, elementsLength, 0);
    } אחרת /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *root=makeNode(0, elementsLength, 0, errorCode);
        אם(U_SUCCESS(errorCode)) {
            root->markRightEdgesFirst(-1);
            root->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::writeNode(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool hasValue=שקר;
    int32_t value=0;
    int32_t type;
    אם(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=getElementValue(start++);
        אם(start==limit) {
            החזר writeValueAndFinal(value, אמת);  // final-value node
        }
        hasValue=אמת;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    אם(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        writeNode(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        כלעוד(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            اكتب(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } אחרת {
        // Branch node.
        int32_t length=countElementUnits(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        writeBranchSubNode(start, limit, unitIndex, length);
        אם(--length<getMinLinearMatch()) {
            type=length;
        } אחרת {
            اكتب(length);
            type=0;
        }
    }
    החזר writeValueAndType(hasValue, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::writeBranchSubNode(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t middleUnits[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    כלעוד(length>getMaxBranchLinearSubNodeLength()) {
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
    עשה {
        int32_t i=starts[unitNumber]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unitNumber]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } כלעוד(++unitNumber<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unitNumber]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[kMaxBranchLinearSubNodeLength-1];
    עשה {
        --unitNumber;
        אם(!isFinal[unitNumber]) {
            jumpTargets[unitNumber]=writeNode(starts[unitNumber], starts[unitNumber+1], unitIndex+1);
        }
    } כלעוד(unitNumber>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unitNumber=length-1;
    writeNode(start, limit, unitIndex+1);
    int32_t offset=اكتب(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    כלעוד(--unitNumber>=0) {
        start=starts[unitNumber];
        int32_t value;
        אם(isFinal[unitNumber]) {
            // Write the final value for the one string ending with this unit.
            value=getElementValue(start);
        } אחרת {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unitNumber];
        }
        writeValueAndFinal(value, isFinal[unitNumber]);
        offset=اكتب(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    כלעוד(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=اكتب(middleUnits[ltLength]);
    }
    החזר offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::makeNode(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    אם(U_FAILURE(errorCode)) {
        החזר nullptr;
    }
    UBool hasValue=שקר;
    int32_t value=0;
    אם(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=getElementValue(start++);
        אם(start==limit) {
            החזר registerFinalValue(value, errorCode);
        }
        hasValue=אמת;
    }
    Node *node;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    אם(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *nextNode=makeNode(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        כלעוד(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            node=createLinearMatchNode(start, lastUnitIndex, maxLinearMatchLength, nextNode);
            nextNode=registerNode(node, errorCode);
        }
        node=createLinearMatchNode(start, unitIndex, length, nextNode);
    } אחרת {
        // Branch node.
        int32_t length=countElementUnits(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *subNode=makeBranchSubNode(start, limit, unitIndex, length, errorCode);
        node=جديد BranchHeadNode(length, subNode);
    }
    אם(hasValue && node!=nullptr) {
        אם(matchNodesCanHaveValues()) {
            ((ValueNode *)node)->setValue(value);
        } אחרת {
            node=جديد IntermediateValueNode(value, registerNode(node, errorCode));
        }
    }
    החזר registerNode(node, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::makeBranchSubNode(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    אם(U_FAILURE(errorCode)) {
        החזר nullptr;
    }
    char16_t middleUnits[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    כלעוד(length>getMaxBranchLinearSubNodeLength()) {
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
    אם(U_FAILURE(errorCode)) {
        החזר nullptr;
    }
    ListBranchNode *listNode=جديد ListBranchNode();
    אם(listNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        החזר nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unitNumber=0;
    עשה {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        אם(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            listNode->add(unit, getElementValue(start));
        } אחרת {
            listNode->add(unit, makeNode(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } כלעוד(++unitNumber<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    אם(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        listNode->add(unit, getElementValue(start));
    } אחרת {
        listNode->add(unit, makeNode(start, limit, unitIndex+1, errorCode));
    }
    Node *node=registerNode(listNode, errorCode);
    // Create the split-branch nodes.
    כלעוד(ltLength>0) {
        --ltLength;
        node=registerNode(
            جديد SplitBranchNode(middleUnits[ltLength], lessThan[ltLength], node), errorCode);
    }
    החזר node;
}

StringTrieBuilder::Node*
StringTrieBuilder::registerNode(Node* newNode, UErrorCode& errorCode) U_LIFETIME_BOUND {
    אם(U_FAILURE(errorCode)) {
        delete newNode;
        החזר nullptr;
    }
    אם(newNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        החזר nullptr;
    }
    ثابت UHashElement *old=uhash_find(nodes, newNode);
    אם(old!=nullptr) {
        delete newNode;
        החזר static_cast<Node*>(old->key.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#אם U_DEBUG
    int32_t oldValue=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, newNode, 1, &errorCode);
    U_ASSERT(oldValue==0);
    אם(U_FAILURE(errorCode)) {
        delete newNode;
        החזר nullptr;
    }
    החזר newNode;
}

StringTrieBuilder::Node*
StringTrieBuilder::registerFinalValue(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    אם(U_FAILURE(errorCode)) {
        החזר nullptr;
    }
    FinalValueNode key(value);
    ثابت UHashElement *old=uhash_find(nodes, &key);
    אם(old!=nullptr) {
        החזר static_cast<Node*>(old->key.pointer);
    }
    Node *newNode=جديد FinalValueNode(value);
    אם(newNode==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        החזר nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#אם U_DEBUG
    int32_t oldValue=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, newNode, 1, &errorCode);
    U_ASSERT(oldValue==0);
    אם(U_FAILURE(errorCode)) {
        delete newNode;
        החזר nullptr;
    }
    החזר newNode;
}

int32_t
StringTrieBuilder::hashNode(ثابت فراغ *node) {
    החזר static_cast<ثابت Node*>(node)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *left, ثابت فراغ *right) {
    החזר *static_cast<ثابت Node*>(left) == *static_cast<ثابت Node*>(right);
}

منطقي
StringTrieBuilder::Node::operator==(ثابت Node &other) ثابت {
    החזר هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::markRightEdgesFirst(int32_t edgeNumber) {
    אם(offset==0) {
        offset=edgeNumber;
    }
    החזר edgeNumber;
}

منطقي
StringTrieBuilder::FinalValueNode::operator==(ثابت Node &other) ثابت {
    אם(هذا==&other) {
        החזר אמת;
    }
    אם(!Node::operator==(other)) {
        החזר שקר;
    }
    ثابت FinalValueNode &o=static_cast<ثابت FinalValueNode &>(other);
    החזר value==o.value;
}

فراغ
StringTrieBuilder::FinalValueNode::اكتب(StringTrieBuilder &builder) {
    offset=builder.writeValueAndFinal(value, אמת);
}

منطقي
StringTrieBuilder::ValueNode::operator==(ثابت Node &other) ثابت {
    אם(هذا==&other) {
        החזר אמת;
    }
    אם(!Node::operator==(other)) {
        החזר שקר;
    }
    ثابت ValueNode &o=static_cast<ثابت ValueNode &>(other);
    החזר hasValue==o.hasValue && (!hasValue || value==o.value);
}

منطقي
StringTrieBuilder::IntermediateValueNode::operator==(ثابت Node &other) ثابت {
    אם(هذا==&other) {
        החזר אמת;
    }
    אם(!ValueNode::operator==(other)) {
        החזר שקר;
    }
    ثابت IntermediateValueNode &o=static_cast<ثابت IntermediateValueNode &>(other);
    החזר next==o.next;
}

int32_t
StringTrieBuilder::IntermediateValueNode::markRightEdgesFirst(int32_t edgeNumber) {
    אם(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    החזר edgeNumber;
}

فراغ
StringTrieBuilder::IntermediateValueNode::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    offset=builder.writeValueAndFinal(value, שקר);
}

منطقي
StringTrieBuilder::LinearMatchNode::operator==(ثابت Node &other) ثابت {
    אם(هذا==&other) {
        החזר אמת;
    }
    אם(!ValueNode::operator==(other)) {
        החזר שקר;
    }
    ثابت LinearMatchNode &o=static_cast<ثابت LinearMatchNode &>(other);
    החזר length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::LinearMatchNode::markRightEdgesFirst(int32_t edgeNumber) {
    אם(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    החזר edgeNumber;
}

منطقي
StringTrieBuilder::ListBranchNode::operator==(ثابت Node &other) ثابت {
    אם(هذا==&other) {
        החזר אמת;
    }
    אם(!Node::operator==(other)) {
        החזר שקר;
    }
    ثابت ListBranchNode &o=static_cast<ثابت ListBranchNode &>(other);
    עבור(int32_t i=0; i<length; ++i) {
        אם(units[i]!=o.units[i] || values[i]!=o.values[i] || equal[i]!=o.equal[i]) {
            החזר שקר;
        }
    }
    החזר אמת;
}

int32_t
StringTrieBuilder::ListBranchNode::markRightEdgesFirst(int32_t edgeNumber) {
    אם(offset==0) {
        firstEdgeNumber=edgeNumber;
        int32_t step=0;
        int32_t i=length;
        עשה {
            Node *edge=equal[--i];
            אם(edge!=nullptr) {
                edgeNumber=edge->markRightEdgesFirst(edgeNumber-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } כלעוד(i>0);
        offset=edgeNumber;
    }
    החזר edgeNumber;
}

فراغ
StringTrieBuilder::ListBranchNode::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unitNumber=length-1;
    Node *rightEdge=equal[unitNumber];
    int32_t rightEdgeNumber= rightEdge==nullptr ? firstEdgeNumber : rightEdge->getOffset();
    עשה {
        --unitNumber;
        אם(equal[unitNumber]!=nullptr) {
            equal[unitNumber]->writeUnlessInsideRightEdge(firstEdgeNumber, rightEdgeNumber, builder);
        }
    } כלעוד(unitNumber>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unitNumber=length-1;
    אם(rightEdge==nullptr) {
        builder.writeValueAndFinal(values[unitNumber], אמת);
    } אחרת {
        rightEdge->اكتب(builder);
    }
    offset=builder.اكتب(units[unitNumber]);
    // Write the rest of this node's unit-value pairs.
    כלעוד(--unitNumber>=0) {
        int32_t value;
        UBool isFinal;
        אם(equal[unitNumber]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=values[unitNumber];
            isFinal=אמת;
        } אחרת {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unitNumber]->getOffset()>0);
            value=offset-equal[unitNumber]->getOffset();
            isFinal=שקר;
        }
        builder.writeValueAndFinal(value, isFinal);
        offset=builder.اكتب(units[unitNumber]);
    }
}

منطقي
StringTrieBuilder::SplitBranchNode::operator==(ثابت Node &other) ثابت {
    אם(هذا==&other) {
        החזר אמת;
    }
    אם(!Node::operator==(other)) {
        החזר שקר;
    }
    ثابت SplitBranchNode &o=static_cast<ثابت SplitBranchNode &>(other);
    החזר unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::SplitBranchNode::markRightEdgesFirst(int32_t edgeNumber) {
    אם(offset==0) {
        firstEdgeNumber=edgeNumber;
        edgeNumber=greaterOrEqual->markRightEdgesFirst(edgeNumber);
        offset=edgeNumber=lessThan->markRightEdgesFirst(edgeNumber-1);
    }
    החזר edgeNumber;
}

فراغ
StringTrieBuilder::SplitBranchNode::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->writeUnlessInsideRightEdge(firstEdgeNumber, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::BranchHeadNode::operator==(ثابت Node &other) ثابت {
    אם(هذا==&other) {
        החזר אמת;
    }
    אם(!ValueNode::operator==(other)) {
        החזר שקר;
    }
    ثابت BranchHeadNode &o=static_cast<ثابت BranchHeadNode &>(other);
    החזר length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::BranchHeadNode::markRightEdgesFirst(int32_t edgeNumber) {
    אם(offset==0) {
        offset=edgeNumber=next->markRightEdgesFirst(edgeNumber);
    }
    החזר edgeNumber;
}

فراغ
StringTrieBuilder::BranchHeadNode::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    אם(length<=builder.getMinLinearMatch()) {
        offset=builder.writeValueAndType(hasValue, value, length-1);
    } אחרת {
        builder.اكتب(length-1);
        offset=builder.writeValueAndType(hasValue, value, 0);
    }
}

U_NAMESPACE_END
