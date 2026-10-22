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
hash_String_Trie_غوټه(ثابت UHashTok کلی) {
    أرجع icu::StringTrieBuilder::hash_غوټه(کلی.اشاره_ګر);
}

ساكن UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    أرجع icu::StringTrieBuilder::equalNodes(key1.اشاره_ګر, key2.اشاره_ګر);
}

U_CDECL_پای_ټکی

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(تش_اشاره) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

فراغ
StringTrieBuilder::createCompactBuilder(int32_t اندازه_Guess, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    nodes=uhash_open_اندازه(hash_String_Trie_غوټه, equalStringTrieNodes, تش_اشاره,
                         اندازه_Guess, &error_کوډ);
    إذا(U_SUCCESS(error_کوډ)) {
        إذا(nodes==تش_اشاره) {
          error_کوډ=U_حافظه_ویش_ERROR;
        } وإلا {
          uhash_سېټ_کلی_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

فراغ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=تش_اشاره;
}

فراغ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_اوږدتیا,
                       UError_کوډ &error_کوډ) {
    إذا(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_غوټه(0, elements_اوږدتیا, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_اوږدتیا, error_کوډ);
        Node *ریښه=make_غوټه(0, elements_اوږدتیا, 0, error_کوډ);
        إذا(U_SUCCESS(error_کوډ)) {
            ریښه->mark_ښی_څنډې_لومړی(-1);
            ریښه->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_غوټه(int32_t پیل, int32_t حد, int32_t unit_شاخص) {
    UBool has_ارزښت=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unit_شاخص==get_Element_String_اوږدتیا(پیل)) {
        // An intermediate or final value.
        value=get_Element_ارزښت(پیل++);
        إذا(پیل==حد) {
            أرجع write_ارزښت_And_Final(value, ریښتیا);  // final-value node
        }
        has_ارزښت=ریښتیا;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(پیل, unit_شاخص);
    int32_t maxUnit=getElementUnit(حد-1, unit_شاخص);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t وروستی_Unit_شاخص=get_حد_د_خطي_سمون(پیل, حد-1, unit_شاخص);
        write_غوټه(پیل, حد, وروستی_Unit_شاخص);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=وروستی_Unit_شاخص-unit_شاخص;
        int32_t max_خطي_سمون_اوږدتیا=get_Max_خطي_سمون_اوږدتیا();
        طالما(length>max_خطي_سمون_اوږدتیا) {
            وروستی_Unit_شاخص-=max_خطي_سمون_اوږدتیا;
            length-=max_خطي_سمون_اوږدتیا;
            writeElementUnits(پیل, وروستی_Unit_شاخص, max_خطي_سمون_اوږدتیا);
            اكتب(get_Min_خطي_سمون()+max_خطي_سمون_اوږدتیا-1);
        }
        writeElementUnits(پیل, unit_شاخص, length);
        type=get_Min_خطي_سمون()+length-1;
    } وإلا {
        // Branch node.
        int32_t length=شمېر_Element_Units(پیل, حد, unit_شاخص);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_غوټه(پیل, حد, unit_شاخص, length);
        إذا(--length<get_Min_خطي_سمون()) {
            type=length;
        } وإلا {
            اكتب(length);
            type=0;
        }
    }
    أرجع write_ارزښت_And_Type(has_ارزښت, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_غوټه(int32_t پیل, int32_t حد, int32_t unit_شاخص, int32_t length) {
    char16_t منځ_Units[k_Max_وېشل_Branch_پوړونه];
    int32_t lessThan[k_Max_وېشل_Branch_پوړونه];
    int32_t lt_اوږدتیا=0;
    طالما(length>get_Max_Branch_خطي_Sub_غوټه_اوږدتیا()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(پیل, unit_شاخص, length/2);
        // Encode the less-than branch first.
        منځ_Units[lt_اوږدتیا]=getElementUnit(i, unit_شاخص);  // middle unit
        lessThan[lt_اوږدتیا]=write_Branch_Sub_غوټه(پیل, i, unit_شاخص, length/2);
        ++lt_اوږدتیا;
        // Continue for the greater-or-equal branch.
        پیل=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_خطي_Sub_غوټه_اوږدتیا];
    UBool isFinal[k_Max_Branch_خطي_Sub_غوټه_اوږدتیا-1];
    int32_t unit_شمېره=0;
    افعل {
        int32_t i=starts[unit_شمېره]=پیل;
        char16_t unit=getElementUnit(i++, unit_شاخص);
        i=شاخص_د_Element_With_بل_Unit(i, unit_شاخص, unit);
        isFinal[unit_شمېره]= پیل==i-1 && unit_شاخص+1==get_Element_String_اوږدتیا(پیل);
        پیل=i;
    } طالما(++unit_شمېره<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_شمېره]=پیل;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_خطي_Sub_غوټه_اوږدتیا-1];
    افعل {
        --unit_شمېره;
        إذا(!isFinal[unit_شمېره]) {
            jumpTargets[unit_شمېره]=write_غوټه(starts[unit_شمېره], starts[unit_شمېره+1], unit_شاخص+1);
        }
    } طالما(unit_شمېره>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_شمېره=length-1;
    write_غوټه(پیل, حد, unit_شاخص+1);
    int32_t دننه_واټن=اكتب(getElementUnit(پیل, unit_شاخص));
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_شمېره>=0) {
        پیل=starts[unit_شمېره];
        int32_t value;
        إذا(isFinal[unit_شمېره]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_ارزښت(پیل);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=دننه_واټن-jumpTargets[unit_شمېره];
        }
        write_ارزښت_And_Final(value, isFinal[unit_شمېره]);
        دننه_واټن=اكتب(getElementUnit(پیل, unit_شاخص));
    }
    // Write the split-branch nodes.
    طالما(lt_اوږدتیا>0) {
        --lt_اوږدتیا;
        write_Delta_منزل(lessThan[lt_اوږدتیا]);
        دننه_واټن=اكتب(منځ_Units[lt_اوږدتیا]);
    }
    أرجع دننه_واټن;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_غوټه(int32_t پیل,
                            int32_t حد,
                            int32_t unit_شاخص,
                            UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    UBool has_ارزښت=خطأ;
    int32_t value=0;
    إذا(unit_شاخص==get_Element_String_اوږدتیا(پیل)) {
        // An intermediate or final value.
        value=get_Element_ارزښت(پیل++);
        إذا(پیل==حد) {
            أرجع ثبتوونکی_Final_ارزښت(value, error_کوډ);
        }
        has_ارزښت=ریښتیا;
    }
    Node *غوټه;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(پیل, unit_شاخص);
    int32_t maxUnit=getElementUnit(حد-1, unit_شاخص);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t وروستی_Unit_شاخص=get_حد_د_خطي_سمون(پیل, حد-1, unit_شاخص);
        Node *بل_غوټه=make_غوټه(پیل, حد, وروستی_Unit_شاخص, error_کوډ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=وروستی_Unit_شاخص-unit_شاخص;
        int32_t max_خطي_سمون_اوږدتیا=get_Max_خطي_سمون_اوږدتیا();
        طالما(length>max_خطي_سمون_اوږدتیا) {
            وروستی_Unit_شاخص-=max_خطي_سمون_اوږدتیا;
            length-=max_خطي_سمون_اوږدتیا;
            غوټه=create_خطي_سمون_غوټه(پیل, وروستی_Unit_شاخص, max_خطي_سمون_اوږدتیا, بل_غوټه);
            بل_غوټه=ثبتوونکی_غوټه(غوټه, error_کوډ);
        }
        غوټه=create_خطي_سمون_غوټه(پیل, unit_شاخص, length, بل_غوټه);
    } وإلا {
        // Branch node.
        int32_t length=شمېر_Element_Units(پیل, حد, unit_شاخص);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_غوټه=make_Branch_Sub_غوټه(پیل, حد, unit_شاخص, length, error_کوډ);
        غوټه=جديد Branch_سر_غوټه(length, sub_غوټه);
    }
    إذا(has_ارزښت && غوټه!=تش_اشاره) {
        إذا(سمون_Nodes_Can_Have_ارزښتونه()) {
            ((ارزښت_غوټه *)غوټه)->سېټ_ارزښت(value);
        } وإلا {
            غوټه=جديد Intermediate_ارزښت_غوټه(value, ثبتوونکی_غوټه(غوټه, error_کوډ));
        }
    }
    أرجع ثبتوونکی_غوټه(غوټه, error_کوډ);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_غوټه(int32_t پیل,
                                     int32_t حد,
                                     int32_t unit_شاخص,
                                     int32_t length,
                                     UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    char16_t منځ_Units[k_Max_وېشل_Branch_پوړونه];
    Node *lessThan[k_Max_وېشل_Branch_پوړونه];
    int32_t lt_اوږدتیا=0;
    طالما(length>get_Max_Branch_خطي_Sub_غوټه_اوږدتیا()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(پیل, unit_شاخص, length/2);
        // Create the less-than branch.
        منځ_Units[lt_اوږدتیا]=getElementUnit(i, unit_شاخص);  // middle unit
        lessThan[lt_اوږدتیا]=make_Branch_Sub_غوټه(پیل, i, unit_شاخص, length/2, error_کوډ);
        ++lt_اوږدتیا;
        // Continue for the greater-or-equal branch.
        پیل=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    List_Branch_غوټه *list_غوټه=جديد List_Branch_غوټه();
    إذا(list_غوټه==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_شمېره=0;
    افعل {
        int32_t i=پیل;
        char16_t unit=getElementUnit(i++, unit_شاخص);
        i=شاخص_د_Element_With_بل_Unit(i, unit_شاخص, unit);
        إذا(پیل==i-1 && unit_شاخص+1==get_Element_String_اوږدتیا(پیل)) {
            list_غوټه->جمع_کړه(unit, get_Element_ارزښت(پیل));
        } وإلا {
            list_غوټه->جمع_کړه(unit, make_غوټه(پیل, i, unit_شاخص+1, error_کوډ));
        }
        پیل=i;
    } طالما(++unit_شمېره<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(پیل, unit_شاخص);
    إذا(پیل==حد-1 && unit_شاخص+1==get_Element_String_اوږدتیا(پیل)) {
        list_غوټه->جمع_کړه(unit, get_Element_ارزښت(پیل));
    } وإلا {
        list_غوټه->جمع_کړه(unit, make_غوټه(پیل, حد, unit_شاخص+1, error_کوډ));
    }
    Node *غوټه=ثبتوونکی_غوټه(list_غوټه, error_کوډ);
    // Create the split-branch nodes.
    طالما(lt_اوږدتیا>0) {
        --lt_اوږدتیا;
        غوټه=ثبتوونکی_غوټه(
            جديد وېشل_Branch_غوټه(منځ_Units[lt_اوږدتیا], lessThan[lt_اوږدتیا], غوټه), error_کوډ);
    }
    أرجع غوټه;
}

StringTrieBuilder::Node*
StringTrieBuilder::ثبتوونکی_غوټه(Node* new_غوټه, UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوډ)) {
        ړنګول new_غوټه;
        أرجع تش_اشاره;
    }
    إذا(new_غوټه==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    ثابت UHashElement *old=uhash_پیدا_کړه(nodes, new_غوټه);
    إذا(old!=تش_اشاره) {
        ړنګول new_غوټه;
        أرجع static_cast<Node*>(old->کلی.اشاره_ګر);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_ارزښت=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#د_که_پای
    uhash_puti(nodes, new_غوټه, 1, &error_کوډ);
    U_ASSERT(old_ارزښت==0);
    إذا(U_FAILURE(error_کوډ)) {
        ړنګول new_غوټه;
        أرجع تش_اشاره;
    }
    أرجع new_غوټه;
}

StringTrieBuilder::Node*
StringTrieBuilder::ثبتوونکی_Final_ارزښت(int32_t value, UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    Final_ارزښت_غوټه کلی(value);
    ثابت UHashElement *old=uhash_پیدا_کړه(nodes, &کلی);
    إذا(old!=تش_اشاره) {
        أرجع static_cast<Node*>(old->کلی.اشاره_ګر);
    }
    Node *new_غوټه=جديد Final_ارزښت_غوټه(value);
    إذا(new_غوټه==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_ارزښت=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#د_که_پای
    uhash_puti(nodes, new_غوټه, 1, &error_کوډ);
    U_ASSERT(old_ارزښت==0);
    إذا(U_FAILURE(error_کوډ)) {
        ړنګول new_غوټه;
        أرجع تش_اشاره;
    }
    أرجع new_غوټه;
}

int32_t
StringTrieBuilder::hash_غوټه(ثابت فراغ *غوټه) {
    أرجع static_cast<ثابت Node*>(غوټه)->hash_کوډ();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *کیڼ, ثابت فراغ *ښی) {
    أرجع *static_cast<ثابت Node*>(کیڼ) == *static_cast<ثابت Node*>(ښی);
}

منطقي
StringTrieBuilder::Node::چلوونکی==(ثابت Node &other) ثابت {
    أرجع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) {
    إذا(دننه_واټن==0) {
        دننه_واټن=څنډه_شمېره;
    }
    أرجع څنډه_شمېره;
}

منطقي
StringTrieBuilder::Final_ارزښت_غوټه::چلوونکی==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    إذا(!Node::چلوونکی==(other)) {
        أرجع خطأ;
    }
    ثابت Final_ارزښت_غوټه &o=static_cast<ثابت Final_ارزښت_غوټه &>(other);
    أرجع value==o.value;
}

فراغ
StringTrieBuilder::Final_ارزښت_غوټه::اكتب(StringTrieBuilder &builder) {
    دننه_واټن=builder.write_ارزښت_And_Final(value, ریښتیا);
}

منطقي
StringTrieBuilder::ارزښت_غوټه::چلوونکی==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    إذا(!Node::چلوونکی==(other)) {
        أرجع خطأ;
    }
    ثابت ارزښت_غوټه &o=static_cast<ثابت ارزښت_غوټه &>(other);
    أرجع has_ارزښت==o.has_ارزښت && (!has_ارزښت || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_ارزښت_غوټه::چلوونکی==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    إذا(!ارزښت_غوټه::چلوونکی==(other)) {
        أرجع خطأ;
    }
    ثابت Intermediate_ارزښت_غوټه &o=static_cast<ثابت Intermediate_ارزښت_غوټه &>(other);
    أرجع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_ارزښت_غوټه::mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) {
    إذا(دننه_واټن==0) {
        دننه_واټن=څنډه_شمېره=next->mark_ښی_څنډې_لومړی(څنډه_شمېره);
    }
    أرجع څنډه_شمېره;
}

فراغ
StringTrieBuilder::Intermediate_ارزښت_غوټه::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    دننه_واټن=builder.write_ارزښت_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::خطي_سمون_غوټه::چلوونکی==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    إذا(!ارزښت_غوټه::چلوونکی==(other)) {
        أرجع خطأ;
    }
    ثابت خطي_سمون_غوټه &o=static_cast<ثابت خطي_سمون_غوټه &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::خطي_سمون_غوټه::mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) {
    إذا(دننه_واټن==0) {
        دننه_واټن=څنډه_شمېره=next->mark_ښی_څنډې_لومړی(څنډه_شمېره);
    }
    أرجع څنډه_شمېره;
}

منطقي
StringTrieBuilder::List_Branch_غوټه::چلوونکی==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    إذا(!Node::چلوونکی==(other)) {
        أرجع خطأ;
    }
    ثابت List_Branch_غوټه &o=static_cast<ثابت List_Branch_غوټه &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || ارزښتونه[i]!=o.ارزښتونه[i] || equal[i]!=o.equal[i]) {
            أرجع خطأ;
        }
    }
    أرجع ریښتیا;
}

int32_t
StringTrieBuilder::List_Branch_غوټه::mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) {
    إذا(دننه_واټن==0) {
        لومړی_څنډه_شمېره=څنډه_شمېره;
        int32_t ګام=0;
        int32_t i=length;
        افعل {
            Node *څنډه=equal[--i];
            إذا(څنډه!=تش_اشاره) {
                څنډه_شمېره=څنډه->mark_ښی_څنډې_لومړی(څنډه_شمېره-ګام);
            }
            // For all but the rightmost edge, decrement the edge number.
            ګام=1;
        } طالما(i>0);
        دننه_واټن=څنډه_شمېره;
    }
    أرجع څنډه_شمېره;
}

فراغ
StringTrieBuilder::List_Branch_غوټه::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_شمېره=length-1;
    Node *ښی_څنډه=equal[unit_شمېره];
    int32_t ښی_څنډه_شمېره= ښی_څنډه==تش_اشاره ? لومړی_څنډه_شمېره : ښی_څنډه->get_دننه_واټن();
    افعل {
        --unit_شمېره;
        إذا(equal[unit_شمېره]!=تش_اشاره) {
            equal[unit_شمېره]->write_Unless_Inside_ښی_څنډه(لومړی_څنډه_شمېره, ښی_څنډه_شمېره, builder);
        }
    } طالما(unit_شمېره>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_شمېره=length-1;
    إذا(ښی_څنډه==تش_اشاره) {
        builder.write_ارزښت_And_Final(ارزښتونه[unit_شمېره], ریښتیا);
    } وإلا {
        ښی_څنډه->اكتب(builder);
    }
    دننه_واټن=builder.اكتب(units[unit_شمېره]);
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_شمېره>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_شمېره]==تش_اشاره) {
            // Write the final value for the one string ending with this unit.
            value=ارزښتونه[unit_شمېره];
            isFinal=ریښتیا;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_شمېره]->get_دننه_واټن()>0);
            value=دننه_واټن-equal[unit_شمېره]->get_دننه_واټن();
            isFinal=خطأ;
        }
        builder.write_ارزښت_And_Final(value, isFinal);
        دننه_واټن=builder.اكتب(units[unit_شمېره]);
    }
}

منطقي
StringTrieBuilder::وېشل_Branch_غوټه::چلوونکی==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    إذا(!Node::چلوونکی==(other)) {
        أرجع خطأ;
    }
    ثابت وېشل_Branch_غوټه &o=static_cast<ثابت وېشل_Branch_غوټه &>(other);
    أرجع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::وېشل_Branch_غوټه::mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) {
    إذا(دننه_واټن==0) {
        لومړی_څنډه_شمېره=څنډه_شمېره;
        څنډه_شمېره=greaterOrEqual->mark_ښی_څنډې_لومړی(څنډه_شمېره);
        دننه_واټن=څنډه_شمېره=lessThan->mark_ښی_څنډې_لومړی(څنډه_شمېره-1);
    }
    أرجع څنډه_شمېره;
}

فراغ
StringTrieBuilder::وېشل_Branch_غوټه::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ښی_څنډه(لومړی_څنډه_شمېره, greaterOrEqual->get_دننه_واټن(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->get_دننه_واټن()>0);
    builder.write_Delta_منزل(lessThan->get_دننه_واټن());  // less-than
    دننه_واټن=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_سر_غوټه::چلوونکی==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    إذا(!ارزښت_غوټه::چلوونکی==(other)) {
        أرجع خطأ;
    }
    ثابت Branch_سر_غوټه &o=static_cast<ثابت Branch_سر_غوټه &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_سر_غوټه::mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) {
    إذا(دننه_واټن==0) {
        دننه_واټن=څنډه_شمېره=next->mark_ښی_څنډې_لومړی(څنډه_شمېره);
    }
    أرجع څنډه_شمېره;
}

فراغ
StringTrieBuilder::Branch_سر_غوټه::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.get_Min_خطي_سمون()) {
        دننه_واټن=builder.write_ارزښت_And_Type(has_ارزښت, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        دننه_واټن=builder.write_ارزښت_And_Type(has_ارزښت, value, 0);
    }
}

U_NAMESPACE_پای_ټکی
