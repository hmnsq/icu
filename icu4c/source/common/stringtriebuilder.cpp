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

ساکن int32_t U_CALLCONV
hash_String_Trie_گرہ(ثابت UHashTok کلید) {
    واپس icu::StringTrieBuilder::hash_گرہ(کلید.اشارہ_گر);
}

ساکن UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    واپس icu::StringTrieBuilder::equalNodes(key1.اشارہ_گر, key2.اشارہ_گر);
}

U_CDECL_انتہا

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(خالی_اشاریہ) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

خالی
StringTrieBuilder::createCompactBuilder(int32_t سائز_Guess, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    nodes=uhash_open_سائز(hash_String_Trie_گرہ, equalStringTrieNodes, خالی_اشاریہ,
                         سائز_Guess, &error_رمز);
    اگر(U_SUCCESS(error_رمز)) {
        اگر(nodes==خالی_اشاریہ) {
          error_رمز=U_حافظہ_تخصیص_ERROR;
        } ورنہ {
          uhash_سیٹ_کلید_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

خالی
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=خالی_اشاریہ;
}

خالی
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_لمبائی,
                       UError_رمز &error_رمز) {
    اگر(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_گرہ(0, elements_لمبائی, 0);
    } ورنہ /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_لمبائی, error_رمز);
        Node *جڑ=make_گرہ(0, elements_لمبائی, 0, error_رمز);
        اگر(U_SUCCESS(error_رمز)) {
            جڑ->mark_دایاں_کنارے_پہلا(-1);
            جڑ->اكتب(*یہ);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_گرہ(int32_t آغاز, int32_t حد_بندی, int32_t unit_اشاریہ) {
    UBool has_قدر=جھوٹ;
    int32_t value=0;
    int32_t type;
    اگر(unit_اشاریہ==get_Element_String_لمبائی(آغاز)) {
        // An intermediate or final value.
        value=get_Element_قدر(آغاز++);
        اگر(آغاز==حد_بندی) {
            واپس write_قدر_And_Final(value, سچ);  // final-value node
        }
        has_قدر=سچ;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(آغاز, unit_اشاریہ);
    int32_t maxUnit=getElementUnit(حد_بندی-1, unit_اشاریہ);
    اگر(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t آخری_Unit_اشاریہ=get_حد_بندی_کا_خطی_مطابقت(آغاز, حد_بندی-1, unit_اشاریہ);
        write_گرہ(آغاز, حد_بندی, آخری_Unit_اشاریہ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=آخری_Unit_اشاریہ-unit_اشاریہ;
        int32_t max_خطی_مطابقت_لمبائی=get_Max_خطی_مطابقت_لمبائی();
        جب(length>max_خطی_مطابقت_لمبائی) {
            آخری_Unit_اشاریہ-=max_خطی_مطابقت_لمبائی;
            length-=max_خطی_مطابقت_لمبائی;
            writeElementUnits(آغاز, آخری_Unit_اشاریہ, max_خطی_مطابقت_لمبائی);
            اكتب(get_Min_خطی_مطابقت()+max_خطی_مطابقت_لمبائی-1);
        }
        writeElementUnits(آغاز, unit_اشاریہ, length);
        type=get_Min_خطی_مطابقت()+length-1;
    } ورنہ {
        // Branch node.
        int32_t length=گنتی_Element_Units(آغاز, حد_بندی, unit_اشاریہ);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_گرہ(آغاز, حد_بندی, unit_اشاریہ, length);
        اگر(--length<get_Min_خطی_مطابقت()) {
            type=length;
        } ورنہ {
            اكتب(length);
            type=0;
        }
    }
    واپس write_قدر_And_Type(has_قدر, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_گرہ(int32_t آغاز, int32_t حد_بندی, int32_t unit_اشاریہ, int32_t length) {
    char16_t درمیان_Units[k_Max_بانٹو_Branch_سطحیں];
    int32_t lessThan[k_Max_بانٹو_Branch_سطحیں];
    int32_t lt_لمبائی=0;
    جب(length>get_Max_Branch_خطی_Sub_گرہ_لمبائی()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(آغاز, unit_اشاریہ, length/2);
        // Encode the less-than branch first.
        درمیان_Units[lt_لمبائی]=getElementUnit(i, unit_اشاریہ);  // middle unit
        lessThan[lt_لمبائی]=write_Branch_Sub_گرہ(آغاز, i, unit_اشاریہ, length/2);
        ++lt_لمبائی;
        // Continue for the greater-or-equal branch.
        آغاز=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_خطی_Sub_گرہ_لمبائی];
    UBool isFinal[k_Max_Branch_خطی_Sub_گرہ_لمبائی-1];
    int32_t unit_نمبر=0;
    کرو {
        int32_t i=starts[unit_نمبر]=آغاز;
        char16_t unit=getElementUnit(i++, unit_اشاریہ);
        i=اشاریہ_کا_Element_With_اگلا_Unit(i, unit_اشاریہ, unit);
        isFinal[unit_نمبر]= آغاز==i-1 && unit_اشاریہ+1==get_Element_String_لمبائی(آغاز);
        آغاز=i;
    } جب(++unit_نمبر<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_نمبر]=آغاز;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_خطی_Sub_گرہ_لمبائی-1];
    کرو {
        --unit_نمبر;
        اگر(!isFinal[unit_نمبر]) {
            jumpTargets[unit_نمبر]=write_گرہ(starts[unit_نمبر], starts[unit_نمبر+1], unit_اشاریہ+1);
        }
    } جب(unit_نمبر>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_نمبر=length-1;
    write_گرہ(آغاز, حد_بندی, unit_اشاریہ+1);
    int32_t انحراف=اكتب(getElementUnit(آغاز, unit_اشاریہ));
    // Write the rest of this node's unit-value pairs.
    جب(--unit_نمبر>=0) {
        آغاز=starts[unit_نمبر];
        int32_t value;
        اگر(isFinal[unit_نمبر]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_قدر(آغاز);
        } ورنہ {
            // Write the delta to the start position of the sub-node.
            value=انحراف-jumpTargets[unit_نمبر];
        }
        write_قدر_And_Final(value, isFinal[unit_نمبر]);
        انحراف=اكتب(getElementUnit(آغاز, unit_اشاریہ));
    }
    // Write the split-branch nodes.
    جب(lt_لمبائی>0) {
        --lt_لمبائی;
        write_Delta_مقصد(lessThan[lt_لمبائی]);
        انحراف=اكتب(درمیان_Units[lt_لمبائی]);
    }
    واپس انحراف;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_گرہ(int32_t آغاز,
                            int32_t حد_بندی,
                            int32_t unit_اشاریہ,
                            UError_رمز& error_رمز) U_LIFETIME_BOUND {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    UBool has_قدر=جھوٹ;
    int32_t value=0;
    اگر(unit_اشاریہ==get_Element_String_لمبائی(آغاز)) {
        // An intermediate or final value.
        value=get_Element_قدر(آغاز++);
        اگر(آغاز==حد_بندی) {
            واپس رجسٹر_Final_قدر(value, error_رمز);
        }
        has_قدر=سچ;
    }
    Node *گرہ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(آغاز, unit_اشاریہ);
    int32_t maxUnit=getElementUnit(حد_بندی-1, unit_اشاریہ);
    اگر(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t آخری_Unit_اشاریہ=get_حد_بندی_کا_خطی_مطابقت(آغاز, حد_بندی-1, unit_اشاریہ);
        Node *اگلا_گرہ=make_گرہ(آغاز, حد_بندی, آخری_Unit_اشاریہ, error_رمز);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=آخری_Unit_اشاریہ-unit_اشاریہ;
        int32_t max_خطی_مطابقت_لمبائی=get_Max_خطی_مطابقت_لمبائی();
        جب(length>max_خطی_مطابقت_لمبائی) {
            آخری_Unit_اشاریہ-=max_خطی_مطابقت_لمبائی;
            length-=max_خطی_مطابقت_لمبائی;
            گرہ=create_خطی_مطابقت_گرہ(آغاز, آخری_Unit_اشاریہ, max_خطی_مطابقت_لمبائی, اگلا_گرہ);
            اگلا_گرہ=رجسٹر_گرہ(گرہ, error_رمز);
        }
        گرہ=create_خطی_مطابقت_گرہ(آغاز, unit_اشاریہ, length, اگلا_گرہ);
    } ورنہ {
        // Branch node.
        int32_t length=گنتی_Element_Units(آغاز, حد_بندی, unit_اشاریہ);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_گرہ=make_Branch_Sub_گرہ(آغاز, حد_بندی, unit_اشاریہ, length, error_رمز);
        گرہ=نیا Branch_سر_گرہ(length, sub_گرہ);
    }
    اگر(has_قدر && گرہ!=خالی_اشاریہ) {
        اگر(مطابقت_Nodes_Can_Have_اقدار()) {
            ((قدر_گرہ *)گرہ)->سیٹ_قدر(value);
        } ورنہ {
            گرہ=نیا Intermediate_قدر_گرہ(value, رجسٹر_گرہ(گرہ, error_رمز));
        }
    }
    واپس رجسٹر_گرہ(گرہ, error_رمز);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_گرہ(int32_t آغاز,
                                     int32_t حد_بندی,
                                     int32_t unit_اشاریہ,
                                     int32_t length,
                                     UError_رمز& error_رمز) U_LIFETIME_BOUND {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    char16_t درمیان_Units[k_Max_بانٹو_Branch_سطحیں];
    Node *lessThan[k_Max_بانٹو_Branch_سطحیں];
    int32_t lt_لمبائی=0;
    جب(length>get_Max_Branch_خطی_Sub_گرہ_لمبائی()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(آغاز, unit_اشاریہ, length/2);
        // Create the less-than branch.
        درمیان_Units[lt_لمبائی]=getElementUnit(i, unit_اشاریہ);  // middle unit
        lessThan[lt_لمبائی]=make_Branch_Sub_گرہ(آغاز, i, unit_اشاریہ, length/2, error_رمز);
        ++lt_لمبائی;
        // Continue for the greater-or-equal branch.
        آغاز=i;
        length=length-length/2;
    }
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    List_Branch_گرہ *list_گرہ=نیا List_Branch_گرہ();
    اگر(list_گرہ==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_نمبر=0;
    کرو {
        int32_t i=آغاز;
        char16_t unit=getElementUnit(i++, unit_اشاریہ);
        i=اشاریہ_کا_Element_With_اگلا_Unit(i, unit_اشاریہ, unit);
        اگر(آغاز==i-1 && unit_اشاریہ+1==get_Element_String_لمبائی(آغاز)) {
            list_گرہ->جوڑو(unit, get_Element_قدر(آغاز));
        } ورنہ {
            list_گرہ->جوڑو(unit, make_گرہ(آغاز, i, unit_اشاریہ+1, error_رمز));
        }
        آغاز=i;
    } جب(++unit_نمبر<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(آغاز, unit_اشاریہ);
    اگر(آغاز==حد_بندی-1 && unit_اشاریہ+1==get_Element_String_لمبائی(آغاز)) {
        list_گرہ->جوڑو(unit, get_Element_قدر(آغاز));
    } ورنہ {
        list_گرہ->جوڑو(unit, make_گرہ(آغاز, حد_بندی, unit_اشاریہ+1, error_رمز));
    }
    Node *گرہ=رجسٹر_گرہ(list_گرہ, error_رمز);
    // Create the split-branch nodes.
    جب(lt_لمبائی>0) {
        --lt_لمبائی;
        گرہ=رجسٹر_گرہ(
            نیا بانٹو_Branch_گرہ(درمیان_Units[lt_لمبائی], lessThan[lt_لمبائی], گرہ), error_رمز);
    }
    واپس گرہ;
}

StringTrieBuilder::Node*
StringTrieBuilder::رجسٹر_گرہ(Node* new_گرہ, UError_رمز& error_رمز) U_LIFETIME_BOUND {
    اگر(U_FAILURE(error_رمز)) {
        حذف new_گرہ;
        واپس خالی_اشاریہ;
    }
    اگر(new_گرہ==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    ثابت UHashElement *old=uhash_ڈھونڈو(nodes, new_گرہ);
    اگر(old!=خالی_اشاریہ) {
        حذف new_گرہ;
        واپس static_cast<Node*>(old->کلید.اشارہ_گر);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#اگر U_DEBUG
    int32_t old_قدر=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#اگر_ختم
    uhash_puti(nodes, new_گرہ, 1, &error_رمز);
    U_ASSERT(old_قدر==0);
    اگر(U_FAILURE(error_رمز)) {
        حذف new_گرہ;
        واپس خالی_اشاریہ;
    }
    واپس new_گرہ;
}

StringTrieBuilder::Node*
StringTrieBuilder::رجسٹر_Final_قدر(int32_t value, UError_رمز& error_رمز) U_LIFETIME_BOUND {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    Final_قدر_گرہ کلید(value);
    ثابت UHashElement *old=uhash_ڈھونڈو(nodes, &کلید);
    اگر(old!=خالی_اشاریہ) {
        واپس static_cast<Node*>(old->کلید.اشارہ_گر);
    }
    Node *new_گرہ=نیا Final_قدر_گرہ(value);
    اگر(new_گرہ==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#اگر U_DEBUG
    int32_t old_قدر=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#اگر_ختم
    uhash_puti(nodes, new_گرہ, 1, &error_رمز);
    U_ASSERT(old_قدر==0);
    اگر(U_FAILURE(error_رمز)) {
        حذف new_گرہ;
        واپس خالی_اشاریہ;
    }
    واپس new_گرہ;
}

int32_t
StringTrieBuilder::hash_گرہ(ثابت خالی *گرہ) {
    واپس static_cast<ثابت Node*>(گرہ)->hash_رمز();
}

UBool
StringTrieBuilder::equalNodes(ثابت خالی *بائیں, ثابت خالی *دائیں) {
    واپس *static_cast<ثابت Node*>(بائیں) == *static_cast<ثابت Node*>(دائیں);
}

منطقی
StringTrieBuilder::Node::عامل==(ثابت Node &other) ثابت {
    واپس یہ==&other || (typeid(*یہ)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_دایاں_کنارے_پہلا(int32_t کنارہ_نمبر) {
    اگر(انحراف==0) {
        انحراف=کنارہ_نمبر;
    }
    واپس کنارہ_نمبر;
}

منطقی
StringTrieBuilder::Final_قدر_گرہ::عامل==(ثابت Node &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    اگر(!Node::عامل==(other)) {
        واپس جھوٹ;
    }
    ثابت Final_قدر_گرہ &o=static_cast<ثابت Final_قدر_گرہ &>(other);
    واپس value==o.value;
}

خالی
StringTrieBuilder::Final_قدر_گرہ::اكتب(StringTrieBuilder &builder) {
    انحراف=builder.write_قدر_And_Final(value, سچ);
}

منطقی
StringTrieBuilder::قدر_گرہ::عامل==(ثابت Node &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    اگر(!Node::عامل==(other)) {
        واپس جھوٹ;
    }
    ثابت قدر_گرہ &o=static_cast<ثابت قدر_گرہ &>(other);
    واپس has_قدر==o.has_قدر && (!has_قدر || value==o.value);
}

منطقی
StringTrieBuilder::Intermediate_قدر_گرہ::عامل==(ثابت Node &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    اگر(!قدر_گرہ::عامل==(other)) {
        واپس جھوٹ;
    }
    ثابت Intermediate_قدر_گرہ &o=static_cast<ثابت Intermediate_قدر_گرہ &>(other);
    واپس next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_قدر_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارہ_نمبر) {
    اگر(انحراف==0) {
        انحراف=کنارہ_نمبر=next->mark_دایاں_کنارے_پہلا(کنارہ_نمبر);
    }
    واپس کنارہ_نمبر;
}

خالی
StringTrieBuilder::Intermediate_قدر_گرہ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    انحراف=builder.write_قدر_And_Final(value, جھوٹ);
}

منطقی
StringTrieBuilder::خطی_مطابقت_گرہ::عامل==(ثابت Node &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    اگر(!قدر_گرہ::عامل==(other)) {
        واپس جھوٹ;
    }
    ثابت خطی_مطابقت_گرہ &o=static_cast<ثابت خطی_مطابقت_گرہ &>(other);
    واپس length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::خطی_مطابقت_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارہ_نمبر) {
    اگر(انحراف==0) {
        انحراف=کنارہ_نمبر=next->mark_دایاں_کنارے_پہلا(کنارہ_نمبر);
    }
    واپس کنارہ_نمبر;
}

منطقی
StringTrieBuilder::List_Branch_گرہ::عامل==(ثابت Node &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    اگر(!Node::عامل==(other)) {
        واپس جھوٹ;
    }
    ثابت List_Branch_گرہ &o=static_cast<ثابت List_Branch_گرہ &>(other);
    برائے(int32_t i=0; i<length; ++i) {
        اگر(units[i]!=o.units[i] || اقدار[i]!=o.اقدار[i] || equal[i]!=o.equal[i]) {
            واپس جھوٹ;
        }
    }
    واپس سچ;
}

int32_t
StringTrieBuilder::List_Branch_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارہ_نمبر) {
    اگر(انحراف==0) {
        پہلا_کنارہ_نمبر=کنارہ_نمبر;
        int32_t قدم=0;
        int32_t i=length;
        کرو {
            Node *کنارہ=equal[--i];
            اگر(کنارہ!=خالی_اشاریہ) {
                کنارہ_نمبر=کنارہ->mark_دایاں_کنارے_پہلا(کنارہ_نمبر-قدم);
            }
            // For all but the rightmost edge, decrement the edge number.
            قدم=1;
        } جب(i>0);
        انحراف=کنارہ_نمبر;
    }
    واپس کنارہ_نمبر;
}

خالی
StringTrieBuilder::List_Branch_گرہ::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_نمبر=length-1;
    Node *دایاں_کنارہ=equal[unit_نمبر];
    int32_t دایاں_کنارہ_نمبر= دایاں_کنارہ==خالی_اشاریہ ? پہلا_کنارہ_نمبر : دایاں_کنارہ->get_انحراف();
    کرو {
        --unit_نمبر;
        اگر(equal[unit_نمبر]!=خالی_اشاریہ) {
            equal[unit_نمبر]->write_Unless_Inside_دایاں_کنارہ(پہلا_کنارہ_نمبر, دایاں_کنارہ_نمبر, builder);
        }
    } جب(unit_نمبر>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_نمبر=length-1;
    اگر(دایاں_کنارہ==خالی_اشاریہ) {
        builder.write_قدر_And_Final(اقدار[unit_نمبر], سچ);
    } ورنہ {
        دایاں_کنارہ->اكتب(builder);
    }
    انحراف=builder.اكتب(units[unit_نمبر]);
    // Write the rest of this node's unit-value pairs.
    جب(--unit_نمبر>=0) {
        int32_t value;
        UBool isFinal;
        اگر(equal[unit_نمبر]==خالی_اشاریہ) {
            // Write the final value for the one string ending with this unit.
            value=اقدار[unit_نمبر];
            isFinal=سچ;
        } ورنہ {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_نمبر]->get_انحراف()>0);
            value=انحراف-equal[unit_نمبر]->get_انحراف();
            isFinal=جھوٹ;
        }
        builder.write_قدر_And_Final(value, isFinal);
        انحراف=builder.اكتب(units[unit_نمبر]);
    }
}

منطقی
StringTrieBuilder::بانٹو_Branch_گرہ::عامل==(ثابت Node &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    اگر(!Node::عامل==(other)) {
        واپس جھوٹ;
    }
    ثابت بانٹو_Branch_گرہ &o=static_cast<ثابت بانٹو_Branch_گرہ &>(other);
    واپس unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::بانٹو_Branch_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارہ_نمبر) {
    اگر(انحراف==0) {
        پہلا_کنارہ_نمبر=کنارہ_نمبر;
        کنارہ_نمبر=greaterOrEqual->mark_دایاں_کنارے_پہلا(کنارہ_نمبر);
        انحراف=کنارہ_نمبر=lessThan->mark_دایاں_کنارے_پہلا(کنارہ_نمبر-1);
    }
    واپس کنارہ_نمبر;
}

خالی
StringTrieBuilder::بانٹو_Branch_گرہ::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_دایاں_کنارہ(پہلا_کنارہ_نمبر, greaterOrEqual->get_انحراف(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->get_انحراف()>0);
    builder.write_Delta_مقصد(lessThan->get_انحراف());  // less-than
    انحراف=builder.اكتب(unit);
}

منطقی
StringTrieBuilder::Branch_سر_گرہ::عامل==(ثابت Node &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    اگر(!قدر_گرہ::عامل==(other)) {
        واپس جھوٹ;
    }
    ثابت Branch_سر_گرہ &o=static_cast<ثابت Branch_سر_گرہ &>(other);
    واپس length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_سر_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارہ_نمبر) {
    اگر(انحراف==0) {
        انحراف=کنارہ_نمبر=next->mark_دایاں_کنارے_پہلا(کنارہ_نمبر);
    }
    واپس کنارہ_نمبر;
}

خالی
StringTrieBuilder::Branch_سر_گرہ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    اگر(length<=builder.get_Min_خطی_مطابقت()) {
        انحراف=builder.write_قدر_And_Type(has_قدر, value, length-1);
    } ورنہ {
        builder.اكتب(length-1);
        انحراف=builder.write_قدر_And_Type(has_قدر, value, 0);
    }
}

U_NAMESPACE_انتہا
