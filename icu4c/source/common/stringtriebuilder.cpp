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
hash_String_Trie_عقدة(ثابت UHashTok مفتاح) {
    إرجاع icu::StringTrieBuilder::hash_عقدة(مفتاح.pointer);
}

ساكن UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    إرجاع icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

فراغ
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع;
    }
    nodes=uhash_openSize(hash_String_Trie_عقدة, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    إذا(U_SUCCESS(errorCode)) {
        إذا(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وإلا {
          uhash_set_مفتاح_Deleter(nodes, uprv_deleteUObject);
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
    إذا(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_عقدة(0, elementsLength, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *جذر=make_عقدة(0, elementsLength, 0, errorCode);
        إذا(U_SUCCESS(errorCode)) {
            جذر->mark_يمين_Edges_First(-1);
            جذر->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_عقدة(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_قيمة=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_قيمة(start++);
        إذا(start==limit) {
            إرجاع write_قيمة_And_Final(value, true);  // final-value node
        }
        has_قيمة=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_عقدة(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        بينما(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            اكتب(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } وإلا {
        // Branch node.
        int32_t length=تعداد_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_عقدة(start, limit, unitIndex, length);
        إذا(--length<getMinLinearMatch()) {
            type=length;
        } وإلا {
            اكتب(length);
            type=0;
        }
    }
    إرجاع write_قيمة_And_Type(has_قيمة, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_عقدة(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t وسط_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    بينما(length>get_Max_Branch_Linear_Sub_عقدة_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        وسط_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_عقدة(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_عقدة_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_عقدة_Length-1];
    int32_t unit_العدد=0;
    افعل {
        int32_t i=starts[unit_العدد]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_العدد]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } بينما(++unit_العدد<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_العدد]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_عقدة_Length-1];
    افعل {
        --unit_العدد;
        إذا(!isFinal[unit_العدد]) {
            jumpTargets[unit_العدد]=write_عقدة(starts[unit_العدد], starts[unit_العدد+1], unitIndex+1);
        }
    } بينما(unit_العدد>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_العدد=length-1;
    write_عقدة(start, limit, unitIndex+1);
    int32_t offset=اكتب(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    بينما(--unit_العدد>=0) {
        start=starts[unit_العدد];
        int32_t value;
        إذا(isFinal[unit_العدد]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_قيمة(start);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_العدد];
        }
        write_قيمة_And_Final(value, isFinal[unit_العدد]);
        offset=اكتب(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    بينما(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=اكتب(وسط_Units[ltLength]);
    }
    إرجاع offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_عقدة(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        إرجاع nullptr;
    }
    UBool has_قيمة=خطأ;
    int32_t value=0;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_قيمة(start++);
        إذا(start==limit) {
            إرجاع register_Final_قيمة(value, errorCode);
        }
        has_قيمة=true;
    }
    Node *عقدة;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_عقدة=make_عقدة(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        بينما(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            عقدة=create_Linear_Match_عقدة(start, lastUnitIndex, maxLinearMatchLength, next_عقدة);
            next_عقدة=register_عقدة(عقدة, errorCode);
        }
        عقدة=create_Linear_Match_عقدة(start, unitIndex, length, next_عقدة);
    } وإلا {
        // Branch node.
        int32_t length=تعداد_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_عقدة=make_Branch_Sub_عقدة(start, limit, unitIndex, length, errorCode);
        عقدة=جديد Branch_Head_عقدة(length, sub_عقدة);
    }
    إذا(has_قيمة && عقدة!=nullptr) {
        إذا(match_Nodes_Can_Have_قيم()) {
            ((قيمة_عقدة *)عقدة)->set_قيمة(value);
        } وإلا {
            عقدة=جديد Intermediate_قيمة_عقدة(value, register_عقدة(عقدة, errorCode));
        }
    }
    إرجاع register_عقدة(عقدة, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_عقدة(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        إرجاع nullptr;
    }
    char16_t وسط_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    بينما(length>get_Max_Branch_Linear_Sub_عقدة_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        وسط_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_عقدة(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(errorCode)) {
        إرجاع nullptr;
    }
    List_Branch_عقدة *list_عقدة=جديد List_Branch_عقدة();
    إذا(list_عقدة==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        إرجاع nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_العدد=0;
    افعل {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        إذا(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_عقدة->add(unit, get_Element_قيمة(start));
        } وإلا {
            list_عقدة->add(unit, make_عقدة(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } بينما(++unit_العدد<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    إذا(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_عقدة->add(unit, get_Element_قيمة(start));
    } وإلا {
        list_عقدة->add(unit, make_عقدة(start, limit, unitIndex+1, errorCode));
    }
    Node *عقدة=register_عقدة(list_عقدة, errorCode);
    // Create the split-branch nodes.
    بينما(ltLength>0) {
        --ltLength;
        عقدة=register_عقدة(
            جديد Split_Branch_عقدة(وسط_Units[ltLength], lessThan[ltLength], عقدة), errorCode);
    }
    إرجاع عقدة;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_عقدة(Node* new_عقدة, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        delete new_عقدة;
        إرجاع nullptr;
    }
    إذا(new_عقدة==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        إرجاع nullptr;
    }
    ثابت UHashElement *old=uhash_find(nodes, new_عقدة);
    إذا(old!=nullptr) {
        delete new_عقدة;
        إرجاع static_cast<Node*>(old->مفتاح.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قيمة=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_عقدة, 1, &errorCode);
    U_ASSERT(old_قيمة==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_عقدة;
        إرجاع nullptr;
    }
    إرجاع new_عقدة;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_قيمة(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        إرجاع nullptr;
    }
    Final_قيمة_عقدة مفتاح(value);
    ثابت UHashElement *old=uhash_find(nodes, &مفتاح);
    إذا(old!=nullptr) {
        إرجاع static_cast<Node*>(old->مفتاح.pointer);
    }
    Node *new_عقدة=جديد Final_قيمة_عقدة(value);
    إذا(new_عقدة==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        إرجاع nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قيمة=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_عقدة, 1, &errorCode);
    U_ASSERT(old_قيمة==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_عقدة;
        إرجاع nullptr;
    }
    إرجاع new_عقدة;
}

int32_t
StringTrieBuilder::hash_عقدة(ثابت فراغ *عقدة) {
    إرجاع static_cast<ثابت Node*>(عقدة)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *left, ثابت فراغ *يمين) {
    إرجاع *static_cast<ثابت Node*>(left) == *static_cast<ثابت Node*>(يمين);
}

منطقي
StringTrieBuilder::Node::operator==(ثابت Node &other) ثابت {
    إرجاع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_يمين_Edges_First(int32_t edge_العدد) {
    إذا(offset==0) {
        offset=edge_العدد;
    }
    إرجاع edge_العدد;
}

منطقي
StringTrieBuilder::Final_قيمة_عقدة::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        إرجاع true;
    }
    إذا(!Node::operator==(other)) {
        إرجاع خطأ;
    }
    ثابت Final_قيمة_عقدة &o=static_cast<ثابت Final_قيمة_عقدة &>(other);
    إرجاع value==o.value;
}

فراغ
StringTrieBuilder::Final_قيمة_عقدة::اكتب(StringTrieBuilder &builder) {
    offset=builder.write_قيمة_And_Final(value, true);
}

منطقي
StringTrieBuilder::قيمة_عقدة::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        إرجاع true;
    }
    إذا(!Node::operator==(other)) {
        إرجاع خطأ;
    }
    ثابت قيمة_عقدة &o=static_cast<ثابت قيمة_عقدة &>(other);
    إرجاع has_قيمة==o.has_قيمة && (!has_قيمة || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_قيمة_عقدة::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        إرجاع true;
    }
    إذا(!قيمة_عقدة::operator==(other)) {
        إرجاع خطأ;
    }
    ثابت Intermediate_قيمة_عقدة &o=static_cast<ثابت Intermediate_قيمة_عقدة &>(other);
    إرجاع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_قيمة_عقدة::mark_يمين_Edges_First(int32_t edge_العدد) {
    إذا(offset==0) {
        offset=edge_العدد=next->mark_يمين_Edges_First(edge_العدد);
    }
    إرجاع edge_العدد;
}

فراغ
StringTrieBuilder::Intermediate_قيمة_عقدة::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    offset=builder.write_قيمة_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::Linear_Match_عقدة::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        إرجاع true;
    }
    إذا(!قيمة_عقدة::operator==(other)) {
        إرجاع خطأ;
    }
    ثابت Linear_Match_عقدة &o=static_cast<ثابت Linear_Match_عقدة &>(other);
    إرجاع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_عقدة::mark_يمين_Edges_First(int32_t edge_العدد) {
    إذا(offset==0) {
        offset=edge_العدد=next->mark_يمين_Edges_First(edge_العدد);
    }
    إرجاع edge_العدد;
}

منطقي
StringTrieBuilder::List_Branch_عقدة::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        إرجاع true;
    }
    إذا(!Node::operator==(other)) {
        إرجاع خطأ;
    }
    ثابت List_Branch_عقدة &o=static_cast<ثابت List_Branch_عقدة &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || قيم[i]!=o.قيم[i] || equal[i]!=o.equal[i]) {
            إرجاع خطأ;
        }
    }
    إرجاع true;
}

int32_t
StringTrieBuilder::List_Branch_عقدة::mark_يمين_Edges_First(int32_t edge_العدد) {
    إذا(offset==0) {
        first_Edge_العدد=edge_العدد;
        int32_t step=0;
        int32_t i=length;
        افعل {
            Node *edge=equal[--i];
            إذا(edge!=nullptr) {
                edge_العدد=edge->mark_يمين_Edges_First(edge_العدد-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } بينما(i>0);
        offset=edge_العدد;
    }
    إرجاع edge_العدد;
}

فراغ
StringTrieBuilder::List_Branch_عقدة::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_العدد=length-1;
    Node *يمين_Edge=equal[unit_العدد];
    int32_t يمين_Edge_العدد= يمين_Edge==nullptr ? first_Edge_العدد : يمين_Edge->getOffset();
    افعل {
        --unit_العدد;
        إذا(equal[unit_العدد]!=nullptr) {
            equal[unit_العدد]->write_Unless_Inside_يمين_Edge(first_Edge_العدد, يمين_Edge_العدد, builder);
        }
    } بينما(unit_العدد>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_العدد=length-1;
    إذا(يمين_Edge==nullptr) {
        builder.write_قيمة_And_Final(قيم[unit_العدد], true);
    } وإلا {
        يمين_Edge->اكتب(builder);
    }
    offset=builder.اكتب(units[unit_العدد]);
    // Write the rest of this node's unit-value pairs.
    بينما(--unit_العدد>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_العدد]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=قيم[unit_العدد];
            isFinal=true;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_العدد]->getOffset()>0);
            value=offset-equal[unit_العدد]->getOffset();
            isFinal=خطأ;
        }
        builder.write_قيمة_And_Final(value, isFinal);
        offset=builder.اكتب(units[unit_العدد]);
    }
}

منطقي
StringTrieBuilder::Split_Branch_عقدة::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        إرجاع true;
    }
    إذا(!Node::operator==(other)) {
        إرجاع خطأ;
    }
    ثابت Split_Branch_عقدة &o=static_cast<ثابت Split_Branch_عقدة &>(other);
    إرجاع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_عقدة::mark_يمين_Edges_First(int32_t edge_العدد) {
    إذا(offset==0) {
        first_Edge_العدد=edge_العدد;
        edge_العدد=greaterOrEqual->mark_يمين_Edges_First(edge_العدد);
        offset=edge_العدد=lessThan->mark_يمين_Edges_First(edge_العدد-1);
    }
    إرجاع edge_العدد;
}

فراغ
StringTrieBuilder::Split_Branch_عقدة::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_يمين_Edge(first_Edge_العدد, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_Head_عقدة::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        إرجاع true;
    }
    إذا(!قيمة_عقدة::operator==(other)) {
        إرجاع خطأ;
    }
    ثابت Branch_Head_عقدة &o=static_cast<ثابت Branch_Head_عقدة &>(other);
    إرجاع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_عقدة::mark_يمين_Edges_First(int32_t edge_العدد) {
    إذا(offset==0) {
        offset=edge_العدد=next->mark_يمين_Edges_First(edge_العدد);
    }
    إرجاع edge_العدد;
}

فراغ
StringTrieBuilder::Branch_Head_عقدة::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.getMinLinearMatch()) {
        offset=builder.write_قيمة_And_Type(has_قيمة, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        offset=builder.write_قيمة_And_Type(has_قيمة, value, 0);
    }
}

U_NAMESPACE_END
