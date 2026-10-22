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

#அந்தர்பாவ "utypeinfo.h"  // for 'typeid' to work
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/stringtriebuilder.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "uhash.h"

U_CDECL_BEGIN

நிலையான int32_t U_CALLCONV
hash_String_Trie_முனை(மாறிலி UHashTok சாவி) {
    திரும்பு icu::StringTrieBuilder::hash_முனை(சாவி.pointer);
}

நிலையான UBool U_CALLCONV
equalStringTrieNodes(மாறிலி UHashTok key1, மாறிலி UHashTok key2) {
    திரும்பு icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

வெறுமை
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    nodes=uhash_openSize(hash_String_Trie_முனை, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    எனில்(U_SUCCESS(errorCode)) {
        எனில்(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
          uhash_set_சாவி_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

வெறுமை
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

வெறுமை
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    எனில்(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_முனை(0, elementsLength, 0);
    } இல்லையெனில் /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *வேர்=make_முனை(0, elementsLength, 0, errorCode);
        எனில்(U_SUCCESS(errorCode)) {
            வேர்->mark_வலது_Edges_First(-1);
            வேர்->லிக(*இது);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_முனை(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_மதிப்பு=பொய்;
    int32_t value=0;
    int32_t type;
    எனில்(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_மதிப்பு(start++);
        எனில்(start==limit) {
            திரும்பு write_மதிப்பு_And_Final(value, உண்மை);  // final-value node
        }
        has_மதிப்பு=உண்மை;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    எனில்(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_முனை(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        போது(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            லிக(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } இல்லையெனில் {
        // Branch node.
        int32_t length=மொத்தம்_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_முனை(start, limit, unitIndex, length);
        எனில்(--length<getMinLinearMatch()) {
            type=length;
        } இல்லையெனில் {
            லிக(length);
            type=0;
        }
    }
    திரும்பு write_மதிப்பு_And_Type(has_மதிப்பு, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_முனை(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t நடு_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    போது(length>get_Max_Branch_Linear_Sub_முனை_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        நடு_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_முனை(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_முனை_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_முனை_Length-1];
    int32_t unit_எண்=0;
    செய் {
        int32_t i=starts[unit_எண்]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_எண்]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } போது(++unit_எண்<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_எண்]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_முனை_Length-1];
    செய் {
        --unit_எண்;
        எனில்(!isFinal[unit_எண்]) {
            jumpTargets[unit_எண்]=write_முனை(starts[unit_எண்], starts[unit_எண்+1], unitIndex+1);
        }
    } போது(unit_எண்>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_எண்=length-1;
    write_முனை(start, limit, unitIndex+1);
    int32_t offset=லிக(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    போது(--unit_எண்>=0) {
        start=starts[unit_எண்];
        int32_t value;
        எனில்(isFinal[unit_எண்]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_மதிப்பு(start);
        } இல்லையெனில் {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_எண்];
        }
        write_மதிப்பு_And_Final(value, isFinal[unit_எண்]);
        offset=லிக(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    போது(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=லிக(நடு_Units[ltLength]);
    }
    திரும்பு offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_முனை(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    UBool has_மதிப்பு=பொய்;
    int32_t value=0;
    எனில்(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_மதிப்பு(start++);
        எனில்(start==limit) {
            திரும்பு register_Final_மதிப்பு(value, errorCode);
        }
        has_மதிப்பு=உண்மை;
    }
    Node *முனை;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    எனில்(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_முனை=make_முனை(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        போது(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            முனை=create_Linear_Match_முனை(start, lastUnitIndex, maxLinearMatchLength, next_முனை);
            next_முனை=register_முனை(முனை, errorCode);
        }
        முனை=create_Linear_Match_முனை(start, unitIndex, length, next_முனை);
    } இல்லையெனில் {
        // Branch node.
        int32_t length=மொத்தம்_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_முனை=make_Branch_Sub_முனை(start, limit, unitIndex, length, errorCode);
        முனை=புதிய Branch_Head_முனை(length, sub_முனை);
    }
    எனில்(has_மதிப்பு && முனை!=nullptr) {
        எனில்(match_Nodes_Can_Have_மதிப்புகள்()) {
            ((மதிப்பு_முனை *)முனை)->set_மதிப்பு(value);
        } இல்லையெனில் {
            முனை=புதிய Intermediate_மதிப்பு_முனை(value, register_முனை(முனை, errorCode));
        }
    }
    திரும்பு register_முனை(முனை, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_முனை(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    char16_t நடு_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    போது(length>get_Max_Branch_Linear_Sub_முனை_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        நடு_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_முனை(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    List_Branch_முனை *list_முனை=புதிய List_Branch_முனை();
    எனில்(list_முனை==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_எண்=0;
    செய் {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        எனில்(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_முனை->add(unit, get_Element_மதிப்பு(start));
        } இல்லையெனில் {
            list_முனை->add(unit, make_முனை(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } போது(++unit_எண்<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    எனில்(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_முனை->add(unit, get_Element_மதிப்பு(start));
    } இல்லையெனில் {
        list_முனை->add(unit, make_முனை(start, limit, unitIndex+1, errorCode));
    }
    Node *முனை=register_முனை(list_முனை, errorCode);
    // Create the split-branch nodes.
    போது(ltLength>0) {
        --ltLength;
        முனை=register_முனை(
            புதிய Split_Branch_முனை(நடு_Units[ltLength], lessThan[ltLength], முனை), errorCode);
    }
    திரும்பு முனை;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_முனை(Node* new_முனை, UErrorCode& errorCode) U_LIFETIME_BOUND {
    எனில்(U_FAILURE(errorCode)) {
        delete new_முனை;
        திரும்பு nullptr;
    }
    எனில்(new_முனை==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    மாறிலி UHashElement *old=uhash_find(nodes, new_முனை);
    எனில்(old!=nullptr) {
        delete new_முனை;
        திரும்பு static_cast<Node*>(old->சாவி.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#எனில் U_DEBUG
    int32_t old_மதிப்பு=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_முனை, 1, &errorCode);
    U_ASSERT(old_மதிப்பு==0);
    எனில்(U_FAILURE(errorCode)) {
        delete new_முனை;
        திரும்பு nullptr;
    }
    திரும்பு new_முனை;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_மதிப்பு(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    Final_மதிப்பு_முனை சாவி(value);
    மாறிலி UHashElement *old=uhash_find(nodes, &சாவி);
    எனில்(old!=nullptr) {
        திரும்பு static_cast<Node*>(old->சாவி.pointer);
    }
    Node *new_முனை=புதிய Final_மதிப்பு_முனை(value);
    எனில்(new_முனை==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#எனில் U_DEBUG
    int32_t old_மதிப்பு=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_முனை, 1, &errorCode);
    U_ASSERT(old_மதிப்பு==0);
    எனில்(U_FAILURE(errorCode)) {
        delete new_முனை;
        திரும்பு nullptr;
    }
    திரும்பு new_முனை;
}

int32_t
StringTrieBuilder::hash_முனை(மாறிலி வெறுமை *முனை) {
    திரும்பு static_cast<மாறிலி Node*>(முனை)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    திரும்பு *static_cast<மாறிலி Node*>(left) == *static_cast<மாறிலி Node*>(வலது);
}

தருக்க
StringTrieBuilder::Node::operator==(மாறிலி Node &other) மாறிலி {
    திரும்பு இது==&other || (typeid(*இது)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_வலது_Edges_First(int32_t edge_எண்) {
    எனில்(offset==0) {
        offset=edge_எண்;
    }
    திரும்பு edge_எண்;
}

தருக்க
StringTrieBuilder::Final_மதிப்பு_முனை::operator==(மாறிலி Node &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    எனில்(!Node::operator==(other)) {
        திரும்பு பொய்;
    }
    மாறிலி Final_மதிப்பு_முனை &o=static_cast<மாறிலி Final_மதிப்பு_முனை &>(other);
    திரும்பு value==o.value;
}

வெறுமை
StringTrieBuilder::Final_மதிப்பு_முனை::லிக(StringTrieBuilder &builder) {
    offset=builder.write_மதிப்பு_And_Final(value, உண்மை);
}

தருக்க
StringTrieBuilder::மதிப்பு_முனை::operator==(மாறிலி Node &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    எனில்(!Node::operator==(other)) {
        திரும்பு பொய்;
    }
    மாறிலி மதிப்பு_முனை &o=static_cast<மாறிலி மதிப்பு_முனை &>(other);
    திரும்பு has_மதிப்பு==o.has_மதிப்பு && (!has_மதிப்பு || value==o.value);
}

தருக்க
StringTrieBuilder::Intermediate_மதிப்பு_முனை::operator==(மாறிலி Node &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    எனில்(!மதிப்பு_முனை::operator==(other)) {
        திரும்பு பொய்;
    }
    மாறிலி Intermediate_மதிப்பு_முனை &o=static_cast<மாறிலி Intermediate_மதிப்பு_முனை &>(other);
    திரும்பு next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_மதிப்பு_முனை::mark_வலது_Edges_First(int32_t edge_எண்) {
    எனில்(offset==0) {
        offset=edge_எண்=next->mark_வலது_Edges_First(edge_எண்);
    }
    திரும்பு edge_எண்;
}

வெறுமை
StringTrieBuilder::Intermediate_மதிப்பு_முனை::லிக(StringTrieBuilder &builder) {
    next->லிக(builder);
    offset=builder.write_மதிப்பு_And_Final(value, பொய்);
}

தருக்க
StringTrieBuilder::Linear_Match_முனை::operator==(மாறிலி Node &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    எனில்(!மதிப்பு_முனை::operator==(other)) {
        திரும்பு பொய்;
    }
    மாறிலி Linear_Match_முனை &o=static_cast<மாறிலி Linear_Match_முனை &>(other);
    திரும்பு length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_முனை::mark_வலது_Edges_First(int32_t edge_எண்) {
    எனில்(offset==0) {
        offset=edge_எண்=next->mark_வலது_Edges_First(edge_எண்);
    }
    திரும்பு edge_எண்;
}

தருக்க
StringTrieBuilder::List_Branch_முனை::operator==(மாறிலி Node &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    எனில்(!Node::operator==(other)) {
        திரும்பு பொய்;
    }
    மாறிலி List_Branch_முனை &o=static_cast<மாறிலி List_Branch_முனை &>(other);
    க்கு(int32_t i=0; i<length; ++i) {
        எனில்(units[i]!=o.units[i] || மதிப்புகள்[i]!=o.மதிப்புகள்[i] || equal[i]!=o.equal[i]) {
            திரும்பு பொய்;
        }
    }
    திரும்பு உண்மை;
}

int32_t
StringTrieBuilder::List_Branch_முனை::mark_வலது_Edges_First(int32_t edge_எண்) {
    எனில்(offset==0) {
        first_Edge_எண்=edge_எண்;
        int32_t step=0;
        int32_t i=length;
        செய் {
            Node *edge=equal[--i];
            எனில்(edge!=nullptr) {
                edge_எண்=edge->mark_வலது_Edges_First(edge_எண்-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } போது(i>0);
        offset=edge_எண்;
    }
    திரும்பு edge_எண்;
}

வெறுமை
StringTrieBuilder::List_Branch_முனை::லிக(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_எண்=length-1;
    Node *வலது_Edge=equal[unit_எண்];
    int32_t வலது_Edge_எண்= வலது_Edge==nullptr ? first_Edge_எண் : வலது_Edge->getOffset();
    செய் {
        --unit_எண்;
        எனில்(equal[unit_எண்]!=nullptr) {
            equal[unit_எண்]->write_Unless_Inside_வலது_Edge(first_Edge_எண், வலது_Edge_எண், builder);
        }
    } போது(unit_எண்>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_எண்=length-1;
    எனில்(வலது_Edge==nullptr) {
        builder.write_மதிப்பு_And_Final(மதிப்புகள்[unit_எண்], உண்மை);
    } இல்லையெனில் {
        வலது_Edge->லிக(builder);
    }
    offset=builder.லிக(units[unit_எண்]);
    // Write the rest of this node's unit-value pairs.
    போது(--unit_எண்>=0) {
        int32_t value;
        UBool isFinal;
        எனில்(equal[unit_எண்]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=மதிப்புகள்[unit_எண்];
            isFinal=உண்மை;
        } இல்லையெனில் {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_எண்]->getOffset()>0);
            value=offset-equal[unit_எண்]->getOffset();
            isFinal=பொய்;
        }
        builder.write_மதிப்பு_And_Final(value, isFinal);
        offset=builder.லிக(units[unit_எண்]);
    }
}

தருக்க
StringTrieBuilder::Split_Branch_முனை::operator==(மாறிலி Node &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    எனில்(!Node::operator==(other)) {
        திரும்பு பொய்;
    }
    மாறிலி Split_Branch_முனை &o=static_cast<மாறிலி Split_Branch_முனை &>(other);
    திரும்பு unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_முனை::mark_வலது_Edges_First(int32_t edge_எண்) {
    எனில்(offset==0) {
        first_Edge_எண்=edge_எண்;
        edge_எண்=greaterOrEqual->mark_வலது_Edges_First(edge_எண்);
        offset=edge_எண்=lessThan->mark_வலது_Edges_First(edge_எண்-1);
    }
    திரும்பு edge_எண்;
}

வெறுமை
StringTrieBuilder::Split_Branch_முனை::லிக(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_வலது_Edge(first_Edge_எண், greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->லிக(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.லிக(unit);
}

தருக்க
StringTrieBuilder::Branch_Head_முனை::operator==(மாறிலி Node &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    எனில்(!மதிப்பு_முனை::operator==(other)) {
        திரும்பு பொய்;
    }
    மாறிலி Branch_Head_முனை &o=static_cast<மாறிலி Branch_Head_முனை &>(other);
    திரும்பு length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_முனை::mark_வலது_Edges_First(int32_t edge_எண்) {
    எனில்(offset==0) {
        offset=edge_எண்=next->mark_வலது_Edges_First(edge_எண்);
    }
    திரும்பு edge_எண்;
}

வெறுமை
StringTrieBuilder::Branch_Head_முனை::லிக(StringTrieBuilder &builder) {
    next->லிக(builder);
    எனில்(length<=builder.getMinLinearMatch()) {
        offset=builder.write_மதிப்பு_And_Type(has_மதிப்பு, value, length-1);
    } இல்லையெனில் {
        builder.லிக(length-1);
        offset=builder.write_மதிப்பு_And_Type(has_மதிப்பு, value, 0);
    }
}

U_NAMESPACE_END
