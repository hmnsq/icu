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
hash_String_Trie_نود(ثابت UHashTok کونچي) {
    أرجع icu::StringTrieBuilder::hash_نود(کونچي.pointer);
}

ساكن UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    أرجع icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
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
        أرجع;
    }
    nodes=uhash_openSize(hash_String_Trie_نود, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    إذا(U_SUCCESS(errorCode)) {
        إذا(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وإلا {
          uhash_set_کونچي_Deleter(nodes, uprv_deleteUObject);
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
        write_نود(0, elementsLength, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *اکر=make_نود(0, elementsLength, 0, errorCode);
        إذا(U_SUCCESS(errorCode)) {
            اکر->mark_کانن_Edges_First(-1);
            اکر->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_نود(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_نيلاي=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_نيلاي(start++);
        إذا(start==limit) {
            أرجع write_نيلاي_And_Final(value, true);  // final-value node
        }
        has_نيلاي=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_نود(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        طالما(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            اكتب(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } وإلا {
        // Branch node.
        int32_t length=کيراءن_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_نود(start, limit, unitIndex, length);
        إذا(--length<getMinLinearMatch()) {
            type=length;
        } وإلا {
            اكتب(length);
            type=0;
        }
    }
    أرجع write_نيلاي_And_Type(has_نيلاي, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_نود(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t تڠه_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_نود_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        تڠه_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_نود(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_نود_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_نود_Length-1];
    int32_t unit_نومبور=0;
    افعل {
        int32_t i=starts[unit_نومبور]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_نومبور]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } طالما(++unit_نومبور<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_نومبور]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_نود_Length-1];
    افعل {
        --unit_نومبور;
        إذا(!isFinal[unit_نومبور]) {
            jumpTargets[unit_نومبور]=write_نود(starts[unit_نومبور], starts[unit_نومبور+1], unitIndex+1);
        }
    } طالما(unit_نومبور>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_نومبور=length-1;
    write_نود(start, limit, unitIndex+1);
    int32_t offset=اكتب(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_نومبور>=0) {
        start=starts[unit_نومبور];
        int32_t value;
        إذا(isFinal[unit_نومبور]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_نيلاي(start);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_نومبور];
        }
        write_نيلاي_And_Final(value, isFinal[unit_نومبور]);
        offset=اكتب(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=اكتب(تڠه_Units[ltLength]);
    }
    أرجع offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_نود(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    UBool has_نيلاي=خطأ;
    int32_t value=0;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_نيلاي(start++);
        إذا(start==limit) {
            أرجع register_Final_نيلاي(value, errorCode);
        }
        has_نيلاي=true;
    }
    Node *نود;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_نود=make_نود(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        طالما(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            نود=create_Linear_Match_نود(start, lastUnitIndex, maxLinearMatchLength, next_نود);
            next_نود=register_نود(نود, errorCode);
        }
        نود=create_Linear_Match_نود(start, unitIndex, length, next_نود);
    } وإلا {
        // Branch node.
        int32_t length=کيراءن_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_نود=make_Branch_Sub_نود(start, limit, unitIndex, length, errorCode);
        نود=جديد Branch_Head_نود(length, sub_نود);
    }
    إذا(has_نيلاي && نود!=nullptr) {
        إذا(match_Nodes_Can_Have_نيلاي_سموا()) {
            ((نيلاي_نود *)نود)->set_نيلاي(value);
        } وإلا {
            نود=جديد Intermediate_نيلاي_نود(value, register_نود(نود, errorCode));
        }
    }
    أرجع register_نود(نود, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_نود(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    char16_t تڠه_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_نود_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        تڠه_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_نود(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    List_Branch_نود *list_نود=جديد List_Branch_نود();
    إذا(list_نود==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_نومبور=0;
    افعل {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        إذا(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_نود->add(unit, get_Element_نيلاي(start));
        } وإلا {
            list_نود->add(unit, make_نود(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } طالما(++unit_نومبور<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    إذا(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_نود->add(unit, get_Element_نيلاي(start));
    } وإلا {
        list_نود->add(unit, make_نود(start, limit, unitIndex+1, errorCode));
    }
    Node *نود=register_نود(list_نود, errorCode);
    // Create the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        نود=register_نود(
            جديد Split_Branch_نود(تڠه_Units[ltLength], lessThan[ltLength], نود), errorCode);
    }
    أرجع نود;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_نود(Node* new_نود, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        delete new_نود;
        أرجع nullptr;
    }
    إذا(new_نود==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    ثابت UHashElement *old=uhash_find(nodes, new_نود);
    إذا(old!=nullptr) {
        delete new_نود;
        أرجع static_cast<Node*>(old->کونچي.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_نيلاي=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_نود, 1, &errorCode);
    U_ASSERT(old_نيلاي==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_نود;
        أرجع nullptr;
    }
    أرجع new_نود;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_نيلاي(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    Final_نيلاي_نود کونچي(value);
    ثابت UHashElement *old=uhash_find(nodes, &کونچي);
    إذا(old!=nullptr) {
        أرجع static_cast<Node*>(old->کونچي.pointer);
    }
    Node *new_نود=جديد Final_نيلاي_نود(value);
    إذا(new_نود==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_نيلاي=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_نود, 1, &errorCode);
    U_ASSERT(old_نيلاي==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_نود;
        أرجع nullptr;
    }
    أرجع new_نود;
}

int32_t
StringTrieBuilder::hash_نود(ثابت فراغ *نود) {
    أرجع static_cast<ثابت Node*>(نود)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *left, ثابت فراغ *کانن) {
    أرجع *static_cast<ثابت Node*>(left) == *static_cast<ثابت Node*>(کانن);
}

منطقي
StringTrieBuilder::Node::operator==(ثابت Node &other) ثابت {
    أرجع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_کانن_Edges_First(int32_t edge_نومبور) {
    إذا(offset==0) {
        offset=edge_نومبور;
    }
    أرجع edge_نومبور;
}

منطقي
StringTrieBuilder::Final_نيلاي_نود::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Final_نيلاي_نود &o=static_cast<ثابت Final_نيلاي_نود &>(other);
    أرجع value==o.value;
}

فراغ
StringTrieBuilder::Final_نيلاي_نود::اكتب(StringTrieBuilder &builder) {
    offset=builder.write_نيلاي_And_Final(value, true);
}

منطقي
StringTrieBuilder::نيلاي_نود::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت نيلاي_نود &o=static_cast<ثابت نيلاي_نود &>(other);
    أرجع has_نيلاي==o.has_نيلاي && (!has_نيلاي || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_نيلاي_نود::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!نيلاي_نود::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Intermediate_نيلاي_نود &o=static_cast<ثابت Intermediate_نيلاي_نود &>(other);
    أرجع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_نيلاي_نود::mark_کانن_Edges_First(int32_t edge_نومبور) {
    إذا(offset==0) {
        offset=edge_نومبور=next->mark_کانن_Edges_First(edge_نومبور);
    }
    أرجع edge_نومبور;
}

فراغ
StringTrieBuilder::Intermediate_نيلاي_نود::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    offset=builder.write_نيلاي_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::Linear_Match_نود::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!نيلاي_نود::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Linear_Match_نود &o=static_cast<ثابت Linear_Match_نود &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_نود::mark_کانن_Edges_First(int32_t edge_نومبور) {
    إذا(offset==0) {
        offset=edge_نومبور=next->mark_کانن_Edges_First(edge_نومبور);
    }
    أرجع edge_نومبور;
}

منطقي
StringTrieBuilder::List_Branch_نود::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت List_Branch_نود &o=static_cast<ثابت List_Branch_نود &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || نيلاي_سموا[i]!=o.نيلاي_سموا[i] || equal[i]!=o.equal[i]) {
            أرجع خطأ;
        }
    }
    أرجع true;
}

int32_t
StringTrieBuilder::List_Branch_نود::mark_کانن_Edges_First(int32_t edge_نومبور) {
    إذا(offset==0) {
        first_Edge_نومبور=edge_نومبور;
        int32_t step=0;
        int32_t i=length;
        افعل {
            Node *edge=equal[--i];
            إذا(edge!=nullptr) {
                edge_نومبور=edge->mark_کانن_Edges_First(edge_نومبور-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } طالما(i>0);
        offset=edge_نومبور;
    }
    أرجع edge_نومبور;
}

فراغ
StringTrieBuilder::List_Branch_نود::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_نومبور=length-1;
    Node *کانن_Edge=equal[unit_نومبور];
    int32_t کانن_Edge_نومبور= کانن_Edge==nullptr ? first_Edge_نومبور : کانن_Edge->getOffset();
    افعل {
        --unit_نومبور;
        إذا(equal[unit_نومبور]!=nullptr) {
            equal[unit_نومبور]->write_Unless_Inside_کانن_Edge(first_Edge_نومبور, کانن_Edge_نومبور, builder);
        }
    } طالما(unit_نومبور>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_نومبور=length-1;
    إذا(کانن_Edge==nullptr) {
        builder.write_نيلاي_And_Final(نيلاي_سموا[unit_نومبور], true);
    } وإلا {
        کانن_Edge->اكتب(builder);
    }
    offset=builder.اكتب(units[unit_نومبور]);
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_نومبور>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_نومبور]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=نيلاي_سموا[unit_نومبور];
            isFinal=true;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_نومبور]->getOffset()>0);
            value=offset-equal[unit_نومبور]->getOffset();
            isFinal=خطأ;
        }
        builder.write_نيلاي_And_Final(value, isFinal);
        offset=builder.اكتب(units[unit_نومبور]);
    }
}

منطقي
StringTrieBuilder::Split_Branch_نود::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Split_Branch_نود &o=static_cast<ثابت Split_Branch_نود &>(other);
    أرجع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_نود::mark_کانن_Edges_First(int32_t edge_نومبور) {
    إذا(offset==0) {
        first_Edge_نومبور=edge_نومبور;
        edge_نومبور=greaterOrEqual->mark_کانن_Edges_First(edge_نومبور);
        offset=edge_نومبور=lessThan->mark_کانن_Edges_First(edge_نومبور-1);
    }
    أرجع edge_نومبور;
}

فراغ
StringTrieBuilder::Split_Branch_نود::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_کانن_Edge(first_Edge_نومبور, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_Head_نود::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!نيلاي_نود::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Branch_Head_نود &o=static_cast<ثابت Branch_Head_نود &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_نود::mark_کانن_Edges_First(int32_t edge_نومبور) {
    إذا(offset==0) {
        offset=edge_نومبور=next->mark_کانن_Edges_First(edge_نومبور);
    }
    أرجع edge_نومبور;
}

فراغ
StringTrieBuilder::Branch_Head_نود::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.getMinLinearMatch()) {
        offset=builder.write_نيلاي_And_Type(has_نيلاي, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        offset=builder.write_نيلاي_And_Type(has_نيلاي, value, 0);
    }
}

U_NAMESPACE_END
