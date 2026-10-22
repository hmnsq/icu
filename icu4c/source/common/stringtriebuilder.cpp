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
hash_String_Trie_تۈگۈن(ثابت UHashTok ئاچقۇچ) {
    أرجع icu::StringTrieBuilder::hash_تۈگۈن(ئاچقۇچ.pointer);
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
    nodes=uhash_openSize(hash_String_Trie_تۈگۈن, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    إذا(U_SUCCESS(errorCode)) {
        إذا(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وإلا {
          uhash_set_ئاچقۇچ_Deleter(nodes, uprv_deleteUObject);
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
        write_تۈگۈن(0, elementsLength, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *يىلتىز=make_تۈگۈن(0, elementsLength, 0, errorCode);
        إذا(U_SUCCESS(errorCode)) {
            يىلتىز->mark_ئوڭ_Edges_First(-1);
            يىلتىز->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_تۈگۈن(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_قىممەت=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_قىممەت(start++);
        إذا(start==limit) {
            أرجع write_قىممەت_And_Final(value, true);  // final-value node
        }
        has_قىممەت=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_تۈگۈن(start, limit, lastUnitIndex);
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
        int32_t length=سانى_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_تۈگۈن(start, limit, unitIndex, length);
        إذا(--length<getMinLinearMatch()) {
            type=length;
        } وإلا {
            اكتب(length);
            type=0;
        }
    }
    أرجع write_قىممەت_And_Type(has_قىممەت, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_تۈگۈن(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t ئوتتۇرا_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_تۈگۈن_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        ئوتتۇرا_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_تۈگۈن(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_تۈگۈن_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_تۈگۈن_Length-1];
    int32_t unit_سان=0;
    افعل {
        int32_t i=starts[unit_سان]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_سان]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } طالما(++unit_سان<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_سان]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_تۈگۈن_Length-1];
    افعل {
        --unit_سان;
        إذا(!isFinal[unit_سان]) {
            jumpTargets[unit_سان]=write_تۈگۈن(starts[unit_سان], starts[unit_سان+1], unitIndex+1);
        }
    } طالما(unit_سان>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_سان=length-1;
    write_تۈگۈن(start, limit, unitIndex+1);
    int32_t offset=اكتب(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_سان>=0) {
        start=starts[unit_سان];
        int32_t value;
        إذا(isFinal[unit_سان]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_قىممەت(start);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_سان];
        }
        write_قىممەت_And_Final(value, isFinal[unit_سان]);
        offset=اكتب(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=اكتب(ئوتتۇرا_Units[ltLength]);
    }
    أرجع offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_تۈگۈن(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    UBool has_قىممەت=خطأ;
    int32_t value=0;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_قىممەت(start++);
        إذا(start==limit) {
            أرجع register_Final_قىممەت(value, errorCode);
        }
        has_قىممەت=true;
    }
    Node *تۈگۈن;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_تۈگۈن=make_تۈگۈن(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        طالما(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            تۈگۈن=create_Linear_Match_تۈگۈن(start, lastUnitIndex, maxLinearMatchLength, next_تۈگۈن);
            next_تۈگۈن=register_تۈگۈن(تۈگۈن, errorCode);
        }
        تۈگۈن=create_Linear_Match_تۈگۈن(start, unitIndex, length, next_تۈگۈن);
    } وإلا {
        // Branch node.
        int32_t length=سانى_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_تۈگۈن=make_Branch_Sub_تۈگۈن(start, limit, unitIndex, length, errorCode);
        تۈگۈن=جديد Branch_Head_تۈگۈن(length, sub_تۈگۈن);
    }
    إذا(has_قىممەت && تۈگۈن!=nullptr) {
        إذا(match_Nodes_Can_Have_قىممەتلەر()) {
            ((قىممەت_تۈگۈن *)تۈگۈن)->set_قىممەت(value);
        } وإلا {
            تۈگۈن=جديد Intermediate_قىممەت_تۈگۈن(value, register_تۈگۈن(تۈگۈن, errorCode));
        }
    }
    أرجع register_تۈگۈن(تۈگۈن, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_تۈگۈن(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    char16_t ئوتتۇرا_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_تۈگۈن_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        ئوتتۇرا_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_تۈگۈن(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    List_Branch_تۈگۈن *list_تۈگۈن=جديد List_Branch_تۈگۈن();
    إذا(list_تۈگۈن==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_سان=0;
    افعل {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        إذا(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_تۈگۈن->add(unit, get_Element_قىممەت(start));
        } وإلا {
            list_تۈگۈن->add(unit, make_تۈگۈن(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } طالما(++unit_سان<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    إذا(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_تۈگۈن->add(unit, get_Element_قىممەت(start));
    } وإلا {
        list_تۈگۈن->add(unit, make_تۈگۈن(start, limit, unitIndex+1, errorCode));
    }
    Node *تۈگۈن=register_تۈگۈن(list_تۈگۈن, errorCode);
    // Create the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        تۈگۈن=register_تۈگۈن(
            جديد Split_Branch_تۈگۈن(ئوتتۇرا_Units[ltLength], lessThan[ltLength], تۈگۈن), errorCode);
    }
    أرجع تۈگۈن;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_تۈگۈن(Node* new_تۈگۈن, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        delete new_تۈگۈن;
        أرجع nullptr;
    }
    إذا(new_تۈگۈن==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    ثابت UHashElement *old=uhash_find(nodes, new_تۈگۈن);
    إذا(old!=nullptr) {
        delete new_تۈگۈن;
        أرجع static_cast<Node*>(old->ئاچقۇچ.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قىممەت=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_تۈگۈن, 1, &errorCode);
    U_ASSERT(old_قىممەت==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_تۈگۈن;
        أرجع nullptr;
    }
    أرجع new_تۈگۈن;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_قىممەت(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    Final_قىممەت_تۈگۈن ئاچقۇچ(value);
    ثابت UHashElement *old=uhash_find(nodes, &ئاچقۇچ);
    إذا(old!=nullptr) {
        أرجع static_cast<Node*>(old->ئاچقۇچ.pointer);
    }
    Node *new_تۈگۈن=جديد Final_قىممەت_تۈگۈن(value);
    إذا(new_تۈگۈن==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قىممەت=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_تۈگۈن, 1, &errorCode);
    U_ASSERT(old_قىممەت==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_تۈگۈن;
        أرجع nullptr;
    }
    أرجع new_تۈگۈن;
}

int32_t
StringTrieBuilder::hash_تۈگۈن(ثابت فراغ *تۈگۈن) {
    أرجع static_cast<ثابت Node*>(تۈگۈن)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *left, ثابت فراغ *ئوڭ) {
    أرجع *static_cast<ثابت Node*>(left) == *static_cast<ثابت Node*>(ئوڭ);
}

منطقي
StringTrieBuilder::Node::operator==(ثابت Node &other) ثابت {
    أرجع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ئوڭ_Edges_First(int32_t edge_سان) {
    إذا(offset==0) {
        offset=edge_سان;
    }
    أرجع edge_سان;
}

منطقي
StringTrieBuilder::Final_قىممەت_تۈگۈن::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Final_قىممەت_تۈگۈن &o=static_cast<ثابت Final_قىممەت_تۈگۈن &>(other);
    أرجع value==o.value;
}

فراغ
StringTrieBuilder::Final_قىممەت_تۈگۈن::اكتب(StringTrieBuilder &builder) {
    offset=builder.write_قىممەت_And_Final(value, true);
}

منطقي
StringTrieBuilder::قىممەت_تۈگۈن::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت قىممەت_تۈگۈن &o=static_cast<ثابت قىممەت_تۈگۈن &>(other);
    أرجع has_قىممەت==o.has_قىممەت && (!has_قىممەت || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_قىممەت_تۈگۈن::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!قىممەت_تۈگۈن::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Intermediate_قىممەت_تۈگۈن &o=static_cast<ثابت Intermediate_قىممەت_تۈگۈن &>(other);
    أرجع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_قىممەت_تۈگۈن::mark_ئوڭ_Edges_First(int32_t edge_سان) {
    إذا(offset==0) {
        offset=edge_سان=next->mark_ئوڭ_Edges_First(edge_سان);
    }
    أرجع edge_سان;
}

فراغ
StringTrieBuilder::Intermediate_قىممەت_تۈگۈن::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    offset=builder.write_قىممەت_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::Linear_Match_تۈگۈن::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!قىممەت_تۈگۈن::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Linear_Match_تۈگۈن &o=static_cast<ثابت Linear_Match_تۈگۈن &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_تۈگۈن::mark_ئوڭ_Edges_First(int32_t edge_سان) {
    إذا(offset==0) {
        offset=edge_سان=next->mark_ئوڭ_Edges_First(edge_سان);
    }
    أرجع edge_سان;
}

منطقي
StringTrieBuilder::List_Branch_تۈگۈن::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت List_Branch_تۈگۈن &o=static_cast<ثابت List_Branch_تۈگۈن &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || قىممەتلەر[i]!=o.قىممەتلەر[i] || equal[i]!=o.equal[i]) {
            أرجع خطأ;
        }
    }
    أرجع true;
}

int32_t
StringTrieBuilder::List_Branch_تۈگۈن::mark_ئوڭ_Edges_First(int32_t edge_سان) {
    إذا(offset==0) {
        first_Edge_سان=edge_سان;
        int32_t step=0;
        int32_t i=length;
        افعل {
            Node *edge=equal[--i];
            إذا(edge!=nullptr) {
                edge_سان=edge->mark_ئوڭ_Edges_First(edge_سان-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } طالما(i>0);
        offset=edge_سان;
    }
    أرجع edge_سان;
}

فراغ
StringTrieBuilder::List_Branch_تۈگۈن::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_سان=length-1;
    Node *ئوڭ_Edge=equal[unit_سان];
    int32_t ئوڭ_Edge_سان= ئوڭ_Edge==nullptr ? first_Edge_سان : ئوڭ_Edge->getOffset();
    افعل {
        --unit_سان;
        إذا(equal[unit_سان]!=nullptr) {
            equal[unit_سان]->write_Unless_Inside_ئوڭ_Edge(first_Edge_سان, ئوڭ_Edge_سان, builder);
        }
    } طالما(unit_سان>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_سان=length-1;
    إذا(ئوڭ_Edge==nullptr) {
        builder.write_قىممەت_And_Final(قىممەتلەر[unit_سان], true);
    } وإلا {
        ئوڭ_Edge->اكتب(builder);
    }
    offset=builder.اكتب(units[unit_سان]);
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_سان>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_سان]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=قىممەتلەر[unit_سان];
            isFinal=true;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_سان]->getOffset()>0);
            value=offset-equal[unit_سان]->getOffset();
            isFinal=خطأ;
        }
        builder.write_قىممەت_And_Final(value, isFinal);
        offset=builder.اكتب(units[unit_سان]);
    }
}

منطقي
StringTrieBuilder::Split_Branch_تۈگۈن::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Split_Branch_تۈگۈن &o=static_cast<ثابت Split_Branch_تۈگۈن &>(other);
    أرجع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_تۈگۈن::mark_ئوڭ_Edges_First(int32_t edge_سان) {
    إذا(offset==0) {
        first_Edge_سان=edge_سان;
        edge_سان=greaterOrEqual->mark_ئوڭ_Edges_First(edge_سان);
        offset=edge_سان=lessThan->mark_ئوڭ_Edges_First(edge_سان-1);
    }
    أرجع edge_سان;
}

فراغ
StringTrieBuilder::Split_Branch_تۈگۈن::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ئوڭ_Edge(first_Edge_سان, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_Head_تۈگۈن::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!قىممەت_تۈگۈن::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Branch_Head_تۈگۈن &o=static_cast<ثابت Branch_Head_تۈگۈن &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_تۈگۈن::mark_ئوڭ_Edges_First(int32_t edge_سان) {
    إذا(offset==0) {
        offset=edge_سان=next->mark_ئوڭ_Edges_First(edge_سان);
    }
    أرجع edge_سان;
}

فراغ
StringTrieBuilder::Branch_Head_تۈگۈن::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.getMinLinearMatch()) {
        offset=builder.write_قىممەت_And_Type(has_قىممەت, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        offset=builder.write_قىممەت_And_Type(has_قىممەت, value, 0);
    }
}

U_NAMESPACE_END
