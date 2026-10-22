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
hash_String_Trie_كلي(ثابت UHashTok مبودي) {
    أرجع icu::StringTrieBuilder::hash_كلي(مبودي.pointer);
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
    nodes=uhash_openSize(hash_String_Trie_كلي, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    إذا(U_SUCCESS(errorCode)) {
        إذا(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وإلا {
          uhash_set_مبودي_Deleter(nodes, uprv_deleteUObject);
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
        write_كلي(0, elementsLength, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *سيو=make_كلي(0, elementsLength, 0, errorCode);
        إذا(U_SUCCESS(errorCode)) {
            سيو->mark_دام_Edges_First(-1);
            سيو->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_كلي(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_درج=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_درج(start++);
        إذا(start==limit) {
            أرجع write_درج_And_Final(value, true);  // final-value node
        }
        has_درج=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_كلي(start, limit, lastUnitIndex);
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
        int32_t length=كدي_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_كلي(start, limit, unitIndex, length);
        إذا(--length<getMinLinearMatch()) {
            type=length;
        } وإلا {
            اكتب(length);
            type=0;
        }
    }
    أرجع write_درج_And_Type(has_درج, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_كلي(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t ڟكيا_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_كلي_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        ڟكيا_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_كلي(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_كلي_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_كلي_Length-1];
    int32_t unit_لمب=0;
    افعل {
        int32_t i=starts[unit_لمب]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_لمب]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } طالما(++unit_لمب<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_لمب]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_كلي_Length-1];
    افعل {
        --unit_لمب;
        إذا(!isFinal[unit_لمب]) {
            jumpTargets[unit_لمب]=write_كلي(starts[unit_لمب], starts[unit_لمب+1], unitIndex+1);
        }
    } طالما(unit_لمب>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_لمب=length-1;
    write_كلي(start, limit, unitIndex+1);
    int32_t offset=اكتب(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_لمب>=0) {
        start=starts[unit_لمب];
        int32_t value;
        إذا(isFinal[unit_لمب]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_درج(start);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_لمب];
        }
        write_درج_And_Final(value, isFinal[unit_لمب]);
        offset=اكتب(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=اكتب(ڟكيا_Units[ltLength]);
    }
    أرجع offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_كلي(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    UBool has_درج=خطأ;
    int32_t value=0;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_درج(start++);
        إذا(start==limit) {
            أرجع register_Final_درج(value, errorCode);
        }
        has_درج=true;
    }
    Node *كلي;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_كلي=make_كلي(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        طالما(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            كلي=create_Linear_Match_كلي(start, lastUnitIndex, maxLinearMatchLength, next_كلي);
            next_كلي=register_كلي(كلي, errorCode);
        }
        كلي=create_Linear_Match_كلي(start, unitIndex, length, next_كلي);
    } وإلا {
        // Branch node.
        int32_t length=كدي_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_كلي=make_Branch_Sub_كلي(start, limit, unitIndex, length, errorCode);
        كلي=جديد Branch_Head_كلي(length, sub_كلي);
    }
    إذا(has_درج && كلي!=nullptr) {
        إذا(match_Nodes_Can_Have_درجوجي()) {
            ((درج_كلي *)كلي)->set_درج(value);
        } وإلا {
            كلي=جديد Intermediate_درج_كلي(value, register_كلي(كلي, errorCode));
        }
    }
    أرجع register_كلي(كلي, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_كلي(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    char16_t ڟكيا_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_كلي_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        ڟكيا_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_كلي(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    List_Branch_كلي *list_كلي=جديد List_Branch_كلي();
    إذا(list_كلي==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_لمب=0;
    افعل {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        إذا(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_كلي->add(unit, get_Element_درج(start));
        } وإلا {
            list_كلي->add(unit, make_كلي(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } طالما(++unit_لمب<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    إذا(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_كلي->add(unit, get_Element_درج(start));
    } وإلا {
        list_كلي->add(unit, make_كلي(start, limit, unitIndex+1, errorCode));
    }
    Node *كلي=register_كلي(list_كلي, errorCode);
    // Create the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        كلي=register_كلي(
            جديد Split_Branch_كلي(ڟكيا_Units[ltLength], lessThan[ltLength], كلي), errorCode);
    }
    أرجع كلي;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_كلي(Node* new_كلي, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        delete new_كلي;
        أرجع nullptr;
    }
    إذا(new_كلي==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    ثابت UHashElement *old=uhash_find(nodes, new_كلي);
    إذا(old!=nullptr) {
        delete new_كلي;
        أرجع static_cast<Node*>(old->مبودي.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_درج=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_كلي, 1, &errorCode);
    U_ASSERT(old_درج==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_كلي;
        أرجع nullptr;
    }
    أرجع new_كلي;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_درج(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    Final_درج_كلي مبودي(value);
    ثابت UHashElement *old=uhash_find(nodes, &مبودي);
    إذا(old!=nullptr) {
        أرجع static_cast<Node*>(old->مبودي.pointer);
    }
    Node *new_كلي=جديد Final_درج_كلي(value);
    إذا(new_كلي==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_درج=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_كلي, 1, &errorCode);
    U_ASSERT(old_درج==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_كلي;
        أرجع nullptr;
    }
    أرجع new_كلي;
}

int32_t
StringTrieBuilder::hash_كلي(ثابت فراغ *كلي) {
    أرجع static_cast<ثابت Node*>(كلي)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *left, ثابت فراغ *دام) {
    أرجع *static_cast<ثابت Node*>(left) == *static_cast<ثابت Node*>(دام);
}

منطقي
StringTrieBuilder::Node::operator==(ثابت Node &other) ثابت {
    أرجع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_دام_Edges_First(int32_t edge_لمب) {
    إذا(offset==0) {
        offset=edge_لمب;
    }
    أرجع edge_لمب;
}

منطقي
StringTrieBuilder::Final_درج_كلي::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Final_درج_كلي &o=static_cast<ثابت Final_درج_كلي &>(other);
    أرجع value==o.value;
}

فراغ
StringTrieBuilder::Final_درج_كلي::اكتب(StringTrieBuilder &builder) {
    offset=builder.write_درج_And_Final(value, true);
}

منطقي
StringTrieBuilder::درج_كلي::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت درج_كلي &o=static_cast<ثابت درج_كلي &>(other);
    أرجع has_درج==o.has_درج && (!has_درج || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_درج_كلي::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!درج_كلي::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Intermediate_درج_كلي &o=static_cast<ثابت Intermediate_درج_كلي &>(other);
    أرجع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_درج_كلي::mark_دام_Edges_First(int32_t edge_لمب) {
    إذا(offset==0) {
        offset=edge_لمب=next->mark_دام_Edges_First(edge_لمب);
    }
    أرجع edge_لمب;
}

فراغ
StringTrieBuilder::Intermediate_درج_كلي::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    offset=builder.write_درج_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::Linear_Match_كلي::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!درج_كلي::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Linear_Match_كلي &o=static_cast<ثابت Linear_Match_كلي &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_كلي::mark_دام_Edges_First(int32_t edge_لمب) {
    إذا(offset==0) {
        offset=edge_لمب=next->mark_دام_Edges_First(edge_لمب);
    }
    أرجع edge_لمب;
}

منطقي
StringTrieBuilder::List_Branch_كلي::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت List_Branch_كلي &o=static_cast<ثابت List_Branch_كلي &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || درجوجي[i]!=o.درجوجي[i] || equal[i]!=o.equal[i]) {
            أرجع خطأ;
        }
    }
    أرجع true;
}

int32_t
StringTrieBuilder::List_Branch_كلي::mark_دام_Edges_First(int32_t edge_لمب) {
    إذا(offset==0) {
        first_Edge_لمب=edge_لمب;
        int32_t step=0;
        int32_t i=length;
        افعل {
            Node *edge=equal[--i];
            إذا(edge!=nullptr) {
                edge_لمب=edge->mark_دام_Edges_First(edge_لمب-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } طالما(i>0);
        offset=edge_لمب;
    }
    أرجع edge_لمب;
}

فراغ
StringTrieBuilder::List_Branch_كلي::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_لمب=length-1;
    Node *دام_Edge=equal[unit_لمب];
    int32_t دام_Edge_لمب= دام_Edge==nullptr ? first_Edge_لمب : دام_Edge->getOffset();
    افعل {
        --unit_لمب;
        إذا(equal[unit_لمب]!=nullptr) {
            equal[unit_لمب]->write_Unless_Inside_دام_Edge(first_Edge_لمب, دام_Edge_لمب, builder);
        }
    } طالما(unit_لمب>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_لمب=length-1;
    إذا(دام_Edge==nullptr) {
        builder.write_درج_And_Final(درجوجي[unit_لمب], true);
    } وإلا {
        دام_Edge->اكتب(builder);
    }
    offset=builder.اكتب(units[unit_لمب]);
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_لمب>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_لمب]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=درجوجي[unit_لمب];
            isFinal=true;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_لمب]->getOffset()>0);
            value=offset-equal[unit_لمب]->getOffset();
            isFinal=خطأ;
        }
        builder.write_درج_And_Final(value, isFinal);
        offset=builder.اكتب(units[unit_لمب]);
    }
}

منطقي
StringTrieBuilder::Split_Branch_كلي::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Split_Branch_كلي &o=static_cast<ثابت Split_Branch_كلي &>(other);
    أرجع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_كلي::mark_دام_Edges_First(int32_t edge_لمب) {
    إذا(offset==0) {
        first_Edge_لمب=edge_لمب;
        edge_لمب=greaterOrEqual->mark_دام_Edges_First(edge_لمب);
        offset=edge_لمب=lessThan->mark_دام_Edges_First(edge_لمب-1);
    }
    أرجع edge_لمب;
}

فراغ
StringTrieBuilder::Split_Branch_كلي::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_دام_Edge(first_Edge_لمب, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_Head_كلي::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!درج_كلي::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Branch_Head_كلي &o=static_cast<ثابت Branch_Head_كلي &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_كلي::mark_دام_Edges_First(int32_t edge_لمب) {
    إذا(offset==0) {
        offset=edge_لمب=next->mark_دام_Edges_First(edge_لمب);
    }
    أرجع edge_لمب;
}

فراغ
StringTrieBuilder::Branch_Head_كلي::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.getMinLinearMatch()) {
        offset=builder.write_درج_And_Type(has_درج, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        offset=builder.write_درج_And_Type(has_درج, value, 0);
    }
}

U_NAMESPACE_END
