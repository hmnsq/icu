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
hash_String_Trie_گرہ(ثابت UHashTok کلید) {
    أرجع icu::StringTrieBuilder::hash_گرہ(کلید.اشارہ_گر);
}

ساكن UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    أرجع icu::StringTrieBuilder::equalNodes(key1.اشارہ_گر, key2.اشارہ_گر);
}

U_CDECL_سرا

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(خالی_سوچک) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

فراغ
StringTrieBuilder::createCompactBuilder(int32_t ناپ_Guess, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    nodes=uhash_open_ناپ(hash_String_Trie_گرہ, equalStringTrieNodes, خالی_سوچک,
                         ناپ_Guess, &error_کوڈ);
    إذا(U_SUCCESS(error_کوڈ)) {
        إذا(nodes==خالی_سوچک) {
          error_کوڈ=U_حافظہ_ونڈ_ERROR;
        } وإلا {
          uhash_سیٹ_کلید_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

فراغ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=خالی_سوچک;
}

فراغ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_لمائی,
                       UError_کوڈ &error_کوڈ) {
    إذا(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_گرہ(0, elements_لمائی, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_لمائی, error_کوڈ);
        Node *جڑ=make_گرہ(0, elements_لمائی, 0, error_کوڈ);
        إذا(U_SUCCESS(error_کوڈ)) {
            جڑ->mark_دایاں_کنارے_پہلا(-1);
            جڑ->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_گرہ(int32_t شروع, int32_t حد_بندی, int32_t unit_اشاریہ) {
    UBool has_قدر=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unit_اشاریہ==get_Element_String_لمائی(شروع)) {
        // An intermediate or final value.
        value=get_Element_قدر(شروع++);
        إذا(شروع==حد_بندی) {
            أرجع write_قدر_And_Final(value, سچ);  // final-value node
        }
        has_قدر=سچ;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(شروع, unit_اشاریہ);
    int32_t maxUnit=getElementUnit(حد_بندی-1, unit_اشاریہ);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t آخری_Unit_اشاریہ=get_حد_بندی_دا_خطی_میل(شروع, حد_بندی-1, unit_اشاریہ);
        write_گرہ(شروع, حد_بندی, آخری_Unit_اشاریہ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=آخری_Unit_اشاریہ-unit_اشاریہ;
        int32_t max_خطی_میل_لمائی=get_Max_خطی_میل_لمائی();
        طالما(length>max_خطی_میل_لمائی) {
            آخری_Unit_اشاریہ-=max_خطی_میل_لمائی;
            length-=max_خطی_میل_لمائی;
            writeElementUnits(شروع, آخری_Unit_اشاریہ, max_خطی_میل_لمائی);
            اكتب(get_Min_خطی_میل()+max_خطی_میل_لمائی-1);
        }
        writeElementUnits(شروع, unit_اشاریہ, length);
        type=get_Min_خطی_میل()+length-1;
    } وإلا {
        // Branch node.
        int32_t length=گنتی_Element_Units(شروع, حد_بندی, unit_اشاریہ);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_گرہ(شروع, حد_بندی, unit_اشاریہ, length);
        إذا(--length<get_Min_خطی_میل()) {
            type=length;
        } وإلا {
            اكتب(length);
            type=0;
        }
    }
    أرجع write_قدر_And_Type(has_قدر, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_گرہ(int32_t شروع, int32_t حد_بندی, int32_t unit_اشاریہ, int32_t length) {
    char16_t درمیان_Units[k_Max_ونڈو_Branch_پدھراں];
    int32_t lessThan[k_Max_ونڈو_Branch_پدھراں];
    int32_t lt_لمائی=0;
    طالما(length>get_Max_Branch_خطی_Sub_گرہ_لمائی()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(شروع, unit_اشاریہ, length/2);
        // Encode the less-than branch first.
        درمیان_Units[lt_لمائی]=getElementUnit(i, unit_اشاریہ);  // middle unit
        lessThan[lt_لمائی]=write_Branch_Sub_گرہ(شروع, i, unit_اشاریہ, length/2);
        ++lt_لمائی;
        // Continue for the greater-or-equal branch.
        شروع=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_خطی_Sub_گرہ_لمائی];
    UBool isFinal[k_Max_Branch_خطی_Sub_گرہ_لمائی-1];
    int32_t unit_نمبر=0;
    افعل {
        int32_t i=starts[unit_نمبر]=شروع;
        char16_t unit=getElementUnit(i++, unit_اشاریہ);
        i=اشاریہ_دا_Element_With_اگلا_Unit(i, unit_اشاریہ, unit);
        isFinal[unit_نمبر]= شروع==i-1 && unit_اشاریہ+1==get_Element_String_لمائی(شروع);
        شروع=i;
    } طالما(++unit_نمبر<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_نمبر]=شروع;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_خطی_Sub_گرہ_لمائی-1];
    افعل {
        --unit_نمبر;
        إذا(!isFinal[unit_نمبر]) {
            jumpTargets[unit_نمبر]=write_گرہ(starts[unit_نمبر], starts[unit_نمبر+1], unit_اشاریہ+1);
        }
    } طالما(unit_نمبر>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_نمبر=length-1;
    write_گرہ(شروع, حد_بندی, unit_اشاریہ+1);
    int32_t وتھ=اكتب(getElementUnit(شروع, unit_اشاریہ));
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_نمبر>=0) {
        شروع=starts[unit_نمبر];
        int32_t value;
        إذا(isFinal[unit_نمبر]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_قدر(شروع);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=وتھ-jumpTargets[unit_نمبر];
        }
        write_قدر_And_Final(value, isFinal[unit_نمبر]);
        وتھ=اكتب(getElementUnit(شروع, unit_اشاریہ));
    }
    // Write the split-branch nodes.
    طالما(lt_لمائی>0) {
        --lt_لمائی;
        write_Delta_منزل(lessThan[lt_لمائی]);
        وتھ=اكتب(درمیان_Units[lt_لمائی]);
    }
    أرجع وتھ;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_گرہ(int32_t شروع,
                            int32_t حد_بندی,
                            int32_t unit_اشاریہ,
                            UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    UBool has_قدر=خطأ;
    int32_t value=0;
    إذا(unit_اشاریہ==get_Element_String_لمائی(شروع)) {
        // An intermediate or final value.
        value=get_Element_قدر(شروع++);
        إذا(شروع==حد_بندی) {
            أرجع رجسٹر_Final_قدر(value, error_کوڈ);
        }
        has_قدر=سچ;
    }
    Node *گرہ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(شروع, unit_اشاریہ);
    int32_t maxUnit=getElementUnit(حد_بندی-1, unit_اشاریہ);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t آخری_Unit_اشاریہ=get_حد_بندی_دا_خطی_میل(شروع, حد_بندی-1, unit_اشاریہ);
        Node *اگلا_گرہ=make_گرہ(شروع, حد_بندی, آخری_Unit_اشاریہ, error_کوڈ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=آخری_Unit_اشاریہ-unit_اشاریہ;
        int32_t max_خطی_میل_لمائی=get_Max_خطی_میل_لمائی();
        طالما(length>max_خطی_میل_لمائی) {
            آخری_Unit_اشاریہ-=max_خطی_میل_لمائی;
            length-=max_خطی_میل_لمائی;
            گرہ=create_خطی_میل_گرہ(شروع, آخری_Unit_اشاریہ, max_خطی_میل_لمائی, اگلا_گرہ);
            اگلا_گرہ=رجسٹر_گرہ(گرہ, error_کوڈ);
        }
        گرہ=create_خطی_میل_گرہ(شروع, unit_اشاریہ, length, اگلا_گرہ);
    } وإلا {
        // Branch node.
        int32_t length=گنتی_Element_Units(شروع, حد_بندی, unit_اشاریہ);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_گرہ=make_Branch_Sub_گرہ(شروع, حد_بندی, unit_اشاریہ, length, error_کوڈ);
        گرہ=جديد Branch_سر_گرہ(length, sub_گرہ);
    }
    إذا(has_قدر && گرہ!=خالی_سوچک) {
        إذا(میل_Nodes_Can_Have_اقدار()) {
            ((قدر_گرہ *)گرہ)->سیٹ_قدر(value);
        } وإلا {
            گرہ=جديد Intermediate_قدر_گرہ(value, رجسٹر_گرہ(گرہ, error_کوڈ));
        }
    }
    أرجع رجسٹر_گرہ(گرہ, error_کوڈ);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_گرہ(int32_t شروع,
                                     int32_t حد_بندی,
                                     int32_t unit_اشاریہ,
                                     int32_t length,
                                     UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    char16_t درمیان_Units[k_Max_ونڈو_Branch_پدھراں];
    Node *lessThan[k_Max_ونڈو_Branch_پدھراں];
    int32_t lt_لمائی=0;
    طالما(length>get_Max_Branch_خطی_Sub_گرہ_لمائی()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(شروع, unit_اشاریہ, length/2);
        // Create the less-than branch.
        درمیان_Units[lt_لمائی]=getElementUnit(i, unit_اشاریہ);  // middle unit
        lessThan[lt_لمائی]=make_Branch_Sub_گرہ(شروع, i, unit_اشاریہ, length/2, error_کوڈ);
        ++lt_لمائی;
        // Continue for the greater-or-equal branch.
        شروع=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    List_Branch_گرہ *list_گرہ=جديد List_Branch_گرہ();
    إذا(list_گرہ==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_نمبر=0;
    افعل {
        int32_t i=شروع;
        char16_t unit=getElementUnit(i++, unit_اشاریہ);
        i=اشاریہ_دا_Element_With_اگلا_Unit(i, unit_اشاریہ, unit);
        إذا(شروع==i-1 && unit_اشاریہ+1==get_Element_String_لمائی(شروع)) {
            list_گرہ->جوڑو(unit, get_Element_قدر(شروع));
        } وإلا {
            list_گرہ->جوڑو(unit, make_گرہ(شروع, i, unit_اشاریہ+1, error_کوڈ));
        }
        شروع=i;
    } طالما(++unit_نمبر<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(شروع, unit_اشاریہ);
    إذا(شروع==حد_بندی-1 && unit_اشاریہ+1==get_Element_String_لمائی(شروع)) {
        list_گرہ->جوڑو(unit, get_Element_قدر(شروع));
    } وإلا {
        list_گرہ->جوڑو(unit, make_گرہ(شروع, حد_بندی, unit_اشاریہ+1, error_کوڈ));
    }
    Node *گرہ=رجسٹر_گرہ(list_گرہ, error_کوڈ);
    // Create the split-branch nodes.
    طالما(lt_لمائی>0) {
        --lt_لمائی;
        گرہ=رجسٹر_گرہ(
            جديد ونڈو_Branch_گرہ(درمیان_Units[lt_لمائی], lessThan[lt_لمائی], گرہ), error_کوڈ);
    }
    أرجع گرہ;
}

StringTrieBuilder::Node*
StringTrieBuilder::رجسٹر_گرہ(Node* new_گرہ, UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوڈ)) {
        مٹاؤ new_گرہ;
        أرجع خالی_سوچک;
    }
    إذا(new_گرہ==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    ثابت UHashElement *old=uhash_لبھو(nodes, new_گرہ);
    إذا(old!=خالی_سوچک) {
        مٹاؤ new_گرہ;
        أرجع static_cast<Node*>(old->کلید.اشارہ_گر);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قدر=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#جے_انت
    uhash_puti(nodes, new_گرہ, 1, &error_کوڈ);
    U_ASSERT(old_قدر==0);
    إذا(U_FAILURE(error_کوڈ)) {
        مٹاؤ new_گرہ;
        أرجع خالی_سوچک;
    }
    أرجع new_گرہ;
}

StringTrieBuilder::Node*
StringTrieBuilder::رجسٹر_Final_قدر(int32_t value, UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    Final_قدر_گرہ کلید(value);
    ثابت UHashElement *old=uhash_لبھو(nodes, &کلید);
    إذا(old!=خالی_سوچک) {
        أرجع static_cast<Node*>(old->کلید.اشارہ_گر);
    }
    Node *new_گرہ=جديد Final_قدر_گرہ(value);
    إذا(new_گرہ==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قدر=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#جے_انت
    uhash_puti(nodes, new_گرہ, 1, &error_کوڈ);
    U_ASSERT(old_قدر==0);
    إذا(U_FAILURE(error_کوڈ)) {
        مٹاؤ new_گرہ;
        أرجع خالی_سوچک;
    }
    أرجع new_گرہ;
}

int32_t
StringTrieBuilder::hash_گرہ(ثابت فراغ *گرہ) {
    أرجع static_cast<ثابت Node*>(گرہ)->hash_کوڈ();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    أرجع *static_cast<ثابت Node*>(کھبے) == *static_cast<ثابت Node*>(سجے);
}

منطقي
StringTrieBuilder::Node::عامل==(ثابت Node &other) ثابت {
    أرجع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) {
    إذا(وتھ==0) {
        وتھ=کنارا_نمبر;
    }
    أرجع کنارا_نمبر;
}

منطقي
StringTrieBuilder::Final_قدر_گرہ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!Node::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت Final_قدر_گرہ &o=static_cast<ثابت Final_قدر_گرہ &>(other);
    أرجع value==o.value;
}

فراغ
StringTrieBuilder::Final_قدر_گرہ::اكتب(StringTrieBuilder &builder) {
    وتھ=builder.write_قدر_And_Final(value, سچ);
}

منطقي
StringTrieBuilder::قدر_گرہ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!Node::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت قدر_گرہ &o=static_cast<ثابت قدر_گرہ &>(other);
    أرجع has_قدر==o.has_قدر && (!has_قدر || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_قدر_گرہ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!قدر_گرہ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت Intermediate_قدر_گرہ &o=static_cast<ثابت Intermediate_قدر_گرہ &>(other);
    أرجع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_قدر_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) {
    إذا(وتھ==0) {
        وتھ=کنارا_نمبر=next->mark_دایاں_کنارے_پہلا(کنارا_نمبر);
    }
    أرجع کنارا_نمبر;
}

فراغ
StringTrieBuilder::Intermediate_قدر_گرہ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    وتھ=builder.write_قدر_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::خطی_میل_گرہ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!قدر_گرہ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت خطی_میل_گرہ &o=static_cast<ثابت خطی_میل_گرہ &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::خطی_میل_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) {
    إذا(وتھ==0) {
        وتھ=کنارا_نمبر=next->mark_دایاں_کنارے_پہلا(کنارا_نمبر);
    }
    أرجع کنارا_نمبر;
}

منطقي
StringTrieBuilder::List_Branch_گرہ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!Node::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت List_Branch_گرہ &o=static_cast<ثابت List_Branch_گرہ &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || اقدار[i]!=o.اقدار[i] || equal[i]!=o.equal[i]) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

int32_t
StringTrieBuilder::List_Branch_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) {
    إذا(وتھ==0) {
        پہلا_کنارا_نمبر=کنارا_نمبر;
        int32_t قدم=0;
        int32_t i=length;
        افعل {
            Node *کنارا=equal[--i];
            إذا(کنارا!=خالی_سوچک) {
                کنارا_نمبر=کنارا->mark_دایاں_کنارے_پہلا(کنارا_نمبر-قدم);
            }
            // For all but the rightmost edge, decrement the edge number.
            قدم=1;
        } طالما(i>0);
        وتھ=کنارا_نمبر;
    }
    أرجع کنارا_نمبر;
}

فراغ
StringTrieBuilder::List_Branch_گرہ::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_نمبر=length-1;
    Node *دایاں_کنارا=equal[unit_نمبر];
    int32_t دایاں_کنارا_نمبر= دایاں_کنارا==خالی_سوچک ? پہلا_کنارا_نمبر : دایاں_کنارا->get_وتھ();
    افعل {
        --unit_نمبر;
        إذا(equal[unit_نمبر]!=خالی_سوچک) {
            equal[unit_نمبر]->write_Unless_Inside_دایاں_کنارا(پہلا_کنارا_نمبر, دایاں_کنارا_نمبر, builder);
        }
    } طالما(unit_نمبر>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_نمبر=length-1;
    إذا(دایاں_کنارا==خالی_سوچک) {
        builder.write_قدر_And_Final(اقدار[unit_نمبر], سچ);
    } وإلا {
        دایاں_کنارا->اكتب(builder);
    }
    وتھ=builder.اكتب(units[unit_نمبر]);
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_نمبر>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_نمبر]==خالی_سوچک) {
            // Write the final value for the one string ending with this unit.
            value=اقدار[unit_نمبر];
            isFinal=سچ;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_نمبر]->get_وتھ()>0);
            value=وتھ-equal[unit_نمبر]->get_وتھ();
            isFinal=خطأ;
        }
        builder.write_قدر_And_Final(value, isFinal);
        وتھ=builder.اكتب(units[unit_نمبر]);
    }
}

منطقي
StringTrieBuilder::ونڈو_Branch_گرہ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!Node::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت ونڈو_Branch_گرہ &o=static_cast<ثابت ونڈو_Branch_گرہ &>(other);
    أرجع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::ونڈو_Branch_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) {
    إذا(وتھ==0) {
        پہلا_کنارا_نمبر=کنارا_نمبر;
        کنارا_نمبر=greaterOrEqual->mark_دایاں_کنارے_پہلا(کنارا_نمبر);
        وتھ=کنارا_نمبر=lessThan->mark_دایاں_کنارے_پہلا(کنارا_نمبر-1);
    }
    أرجع کنارا_نمبر;
}

فراغ
StringTrieBuilder::ونڈو_Branch_گرہ::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_دایاں_کنارا(پہلا_کنارا_نمبر, greaterOrEqual->get_وتھ(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->get_وتھ()>0);
    builder.write_Delta_منزل(lessThan->get_وتھ());  // less-than
    وتھ=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_سر_گرہ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!قدر_گرہ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت Branch_سر_گرہ &o=static_cast<ثابت Branch_سر_گرہ &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_سر_گرہ::mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) {
    إذا(وتھ==0) {
        وتھ=کنارا_نمبر=next->mark_دایاں_کنارے_پہلا(کنارا_نمبر);
    }
    أرجع کنارا_نمبر;
}

فراغ
StringTrieBuilder::Branch_سر_گرہ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.get_Min_خطی_میل()) {
        وتھ=builder.write_قدر_And_Type(has_قدر, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        وتھ=builder.write_قدر_And_Type(has_قدر, value, 0);
    }
}

U_NAMESPACE_سرا
