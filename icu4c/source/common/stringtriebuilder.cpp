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

#شامل "utypeinfo.h"  // for 'typeid' to work
#شامل "unicode/utypes.h"
#شامل "unicode/stringtriebuilder.h"
#شامل "uassert.h"
#شامل "uhash.h"

U_CDECL_BEGIN

ایستا int32_t U_CALLCONV
hash_String_Trie_گره(ثابت UHashTok کلید) {
    بازگشت icu::StringTrieBuilder::hash_گره(کلید.pointer);
}

ایستا UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    بازگشت icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

تهی
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    nodes=uhash_openSize(hash_String_Trie_گره, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    اگر(U_SUCCESS(errorCode)) {
        اگر(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وگرنه {
          uhash_set_کلید_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

تهی
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

تهی
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    اگر(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_گره(0, elementsLength, 0);
    } وگرنه /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *ریشه=make_گره(0, elementsLength, 0, errorCode);
        اگر(U_SUCCESS(errorCode)) {
            ریشه->mark_راست_Edges_First(-1);
            ریشه->اكتب(*این);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_گره(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_مقدار=نادرست;
    int32_t value=0;
    int32_t type;
    اگر(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_مقدار(start++);
        اگر(start==limit) {
            بازگشت write_مقدار_And_Final(value, درست);  // final-value node
        }
        has_مقدار=درست;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    اگر(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_گره(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        وقتی(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            اكتب(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } وگرنه {
        // Branch node.
        int32_t length=شمار_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_گره(start, limit, unitIndex, length);
        اگر(--length<getMinLinearMatch()) {
            type=length;
        } وگرنه {
            اكتب(length);
            type=0;
        }
    }
    بازگشت write_مقدار_And_Type(has_مقدار, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_گره(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t میانه_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    وقتی(length>get_Max_Branch_Linear_Sub_گره_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        میانه_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_گره(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_گره_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_گره_Length-1];
    int32_t unit_عدد=0;
    انجامبده {
        int32_t i=starts[unit_عدد]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_عدد]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } وقتی(++unit_عدد<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_عدد]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_گره_Length-1];
    انجامبده {
        --unit_عدد;
        اگر(!isFinal[unit_عدد]) {
            jumpTargets[unit_عدد]=write_گره(starts[unit_عدد], starts[unit_عدد+1], unitIndex+1);
        }
    } وقتی(unit_عدد>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_عدد=length-1;
    write_گره(start, limit, unitIndex+1);
    int32_t offset=اكتب(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    وقتی(--unit_عدد>=0) {
        start=starts[unit_عدد];
        int32_t value;
        اگر(isFinal[unit_عدد]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_مقدار(start);
        } وگرنه {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_عدد];
        }
        write_مقدار_And_Final(value, isFinal[unit_عدد]);
        offset=اكتب(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    وقتی(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=اكتب(میانه_Units[ltLength]);
    }
    بازگشت offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_گره(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    UBool has_مقدار=نادرست;
    int32_t value=0;
    اگر(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_مقدار(start++);
        اگر(start==limit) {
            بازگشت register_Final_مقدار(value, errorCode);
        }
        has_مقدار=درست;
    }
    Node *گره;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    اگر(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_گره=make_گره(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        وقتی(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            گره=create_Linear_Match_گره(start, lastUnitIndex, maxLinearMatchLength, next_گره);
            next_گره=register_گره(گره, errorCode);
        }
        گره=create_Linear_Match_گره(start, unitIndex, length, next_گره);
    } وگرنه {
        // Branch node.
        int32_t length=شمار_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_گره=make_Branch_Sub_گره(start, limit, unitIndex, length, errorCode);
        گره=جدید Branch_Head_گره(length, sub_گره);
    }
    اگر(has_مقدار && گره!=nullptr) {
        اگر(match_Nodes_Can_Have_مقادیر()) {
            ((مقدار_گره *)گره)->set_مقدار(value);
        } وگرنه {
            گره=جدید Intermediate_مقدار_گره(value, register_گره(گره, errorCode));
        }
    }
    بازگشت register_گره(گره, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_گره(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    char16_t میانه_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    وقتی(length>get_Max_Branch_Linear_Sub_گره_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        میانه_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_گره(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    List_Branch_گره *list_گره=جدید List_Branch_گره();
    اگر(list_گره==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_عدد=0;
    انجامبده {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        اگر(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_گره->add(unit, get_Element_مقدار(start));
        } وگرنه {
            list_گره->add(unit, make_گره(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } وقتی(++unit_عدد<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    اگر(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_گره->add(unit, get_Element_مقدار(start));
    } وگرنه {
        list_گره->add(unit, make_گره(start, limit, unitIndex+1, errorCode));
    }
    Node *گره=register_گره(list_گره, errorCode);
    // Create the split-branch nodes.
    وقتی(ltLength>0) {
        --ltLength;
        گره=register_گره(
            جدید Split_Branch_گره(میانه_Units[ltLength], lessThan[ltLength], گره), errorCode);
    }
    بازگشت گره;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_گره(Node* new_گره, UErrorCode& errorCode) U_LIFETIME_BOUND {
    اگر(U_FAILURE(errorCode)) {
        delete new_گره;
        بازگشت nullptr;
    }
    اگر(new_گره==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    ثابت UHashElement *old=uhash_find(nodes, new_گره);
    اگر(old!=nullptr) {
        delete new_گره;
        بازگشت static_cast<Node*>(old->کلید.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#اگر U_DEBUG
    int32_t old_مقدار=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_گره, 1, &errorCode);
    U_ASSERT(old_مقدار==0);
    اگر(U_FAILURE(errorCode)) {
        delete new_گره;
        بازگشت nullptr;
    }
    بازگشت new_گره;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_مقدار(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    Final_مقدار_گره کلید(value);
    ثابت UHashElement *old=uhash_find(nodes, &کلید);
    اگر(old!=nullptr) {
        بازگشت static_cast<Node*>(old->کلید.pointer);
    }
    Node *new_گره=جدید Final_مقدار_گره(value);
    اگر(new_گره==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#اگر U_DEBUG
    int32_t old_مقدار=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_گره, 1, &errorCode);
    U_ASSERT(old_مقدار==0);
    اگر(U_FAILURE(errorCode)) {
        delete new_گره;
        بازگشت nullptr;
    }
    بازگشت new_گره;
}

int32_t
StringTrieBuilder::hash_گره(ثابت تهی *گره) {
    بازگشت static_cast<ثابت Node*>(گره)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ثابت تهی *left, ثابت تهی *راست) {
    بازگشت *static_cast<ثابت Node*>(left) == *static_cast<ثابت Node*>(راست);
}

منطقی
StringTrieBuilder::Node::operator==(ثابت Node &other) ثابت {
    بازگشت این==&other || (typeid(*این)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_راست_Edges_First(int32_t edge_عدد) {
    اگر(offset==0) {
        offset=edge_عدد;
    }
    بازگشت edge_عدد;
}

منطقی
StringTrieBuilder::Final_مقدار_گره::operator==(ثابت Node &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    اگر(!Node::operator==(other)) {
        بازگشت نادرست;
    }
    ثابت Final_مقدار_گره &o=static_cast<ثابت Final_مقدار_گره &>(other);
    بازگشت value==o.value;
}

تهی
StringTrieBuilder::Final_مقدار_گره::اكتب(StringTrieBuilder &builder) {
    offset=builder.write_مقدار_And_Final(value, درست);
}

منطقی
StringTrieBuilder::مقدار_گره::operator==(ثابت Node &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    اگر(!Node::operator==(other)) {
        بازگشت نادرست;
    }
    ثابت مقدار_گره &o=static_cast<ثابت مقدار_گره &>(other);
    بازگشت has_مقدار==o.has_مقدار && (!has_مقدار || value==o.value);
}

منطقی
StringTrieBuilder::Intermediate_مقدار_گره::operator==(ثابت Node &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    اگر(!مقدار_گره::operator==(other)) {
        بازگشت نادرست;
    }
    ثابت Intermediate_مقدار_گره &o=static_cast<ثابت Intermediate_مقدار_گره &>(other);
    بازگشت next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_مقدار_گره::mark_راست_Edges_First(int32_t edge_عدد) {
    اگر(offset==0) {
        offset=edge_عدد=next->mark_راست_Edges_First(edge_عدد);
    }
    بازگشت edge_عدد;
}

تهی
StringTrieBuilder::Intermediate_مقدار_گره::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    offset=builder.write_مقدار_And_Final(value, نادرست);
}

منطقی
StringTrieBuilder::Linear_Match_گره::operator==(ثابت Node &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    اگر(!مقدار_گره::operator==(other)) {
        بازگشت نادرست;
    }
    ثابت Linear_Match_گره &o=static_cast<ثابت Linear_Match_گره &>(other);
    بازگشت length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_گره::mark_راست_Edges_First(int32_t edge_عدد) {
    اگر(offset==0) {
        offset=edge_عدد=next->mark_راست_Edges_First(edge_عدد);
    }
    بازگشت edge_عدد;
}

منطقی
StringTrieBuilder::List_Branch_گره::operator==(ثابت Node &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    اگر(!Node::operator==(other)) {
        بازگشت نادرست;
    }
    ثابت List_Branch_گره &o=static_cast<ثابت List_Branch_گره &>(other);
    برای(int32_t i=0; i<length; ++i) {
        اگر(units[i]!=o.units[i] || مقادیر[i]!=o.مقادیر[i] || equal[i]!=o.equal[i]) {
            بازگشت نادرست;
        }
    }
    بازگشت درست;
}

int32_t
StringTrieBuilder::List_Branch_گره::mark_راست_Edges_First(int32_t edge_عدد) {
    اگر(offset==0) {
        first_Edge_عدد=edge_عدد;
        int32_t step=0;
        int32_t i=length;
        انجامبده {
            Node *edge=equal[--i];
            اگر(edge!=nullptr) {
                edge_عدد=edge->mark_راست_Edges_First(edge_عدد-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } وقتی(i>0);
        offset=edge_عدد;
    }
    بازگشت edge_عدد;
}

تهی
StringTrieBuilder::List_Branch_گره::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_عدد=length-1;
    Node *راست_Edge=equal[unit_عدد];
    int32_t راست_Edge_عدد= راست_Edge==nullptr ? first_Edge_عدد : راست_Edge->getOffset();
    انجامبده {
        --unit_عدد;
        اگر(equal[unit_عدد]!=nullptr) {
            equal[unit_عدد]->write_Unless_Inside_راست_Edge(first_Edge_عدد, راست_Edge_عدد, builder);
        }
    } وقتی(unit_عدد>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_عدد=length-1;
    اگر(راست_Edge==nullptr) {
        builder.write_مقدار_And_Final(مقادیر[unit_عدد], درست);
    } وگرنه {
        راست_Edge->اكتب(builder);
    }
    offset=builder.اكتب(units[unit_عدد]);
    // Write the rest of this node's unit-value pairs.
    وقتی(--unit_عدد>=0) {
        int32_t value;
        UBool isFinal;
        اگر(equal[unit_عدد]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=مقادیر[unit_عدد];
            isFinal=درست;
        } وگرنه {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_عدد]->getOffset()>0);
            value=offset-equal[unit_عدد]->getOffset();
            isFinal=نادرست;
        }
        builder.write_مقدار_And_Final(value, isFinal);
        offset=builder.اكتب(units[unit_عدد]);
    }
}

منطقی
StringTrieBuilder::Split_Branch_گره::operator==(ثابت Node &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    اگر(!Node::operator==(other)) {
        بازگشت نادرست;
    }
    ثابت Split_Branch_گره &o=static_cast<ثابت Split_Branch_گره &>(other);
    بازگشت unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_گره::mark_راست_Edges_First(int32_t edge_عدد) {
    اگر(offset==0) {
        first_Edge_عدد=edge_عدد;
        edge_عدد=greaterOrEqual->mark_راست_Edges_First(edge_عدد);
        offset=edge_عدد=lessThan->mark_راست_Edges_First(edge_عدد-1);
    }
    بازگشت edge_عدد;
}

تهی
StringTrieBuilder::Split_Branch_گره::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_راست_Edge(first_Edge_عدد, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.اكتب(unit);
}

منطقی
StringTrieBuilder::Branch_Head_گره::operator==(ثابت Node &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    اگر(!مقدار_گره::operator==(other)) {
        بازگشت نادرست;
    }
    ثابت Branch_Head_گره &o=static_cast<ثابت Branch_Head_گره &>(other);
    بازگشت length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_گره::mark_راست_Edges_First(int32_t edge_عدد) {
    اگر(offset==0) {
        offset=edge_عدد=next->mark_راست_Edges_First(edge_عدد);
    }
    بازگشت edge_عدد;
}

تهی
StringTrieBuilder::Branch_Head_گره::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    اگر(length<=builder.getMinLinearMatch()) {
        offset=builder.write_مقدار_And_Type(has_مقدار, value, length-1);
    } وگرنه {
        builder.اكتب(length-1);
        offset=builder.write_مقدار_And_Type(has_مقدار, value, 0);
    }
}

U_NAMESPACE_END
