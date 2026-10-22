// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#అన్తర్భావ "utypeinfo.h"  // for 'typeid' to work

#అన్తర్భావ "unicode/uchriter.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ustr_imp.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UCharCharacterIterator)

UCharCharacterIterator::UCharCharacterIterator()
  : CharacterIterator(),
  పాఠ్యం(శూన్యసూచిక)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr పాఠ్యం_Ptr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(పాఠ్యం_Ptr != శూన్యసూచిక ? (length >= 0 ? length : u_strlen(పాఠ్యం_Ptr)) : 0),
  పాఠ్యం(పాఠ్యం_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr పాఠ్యం_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(పాఠ్యం_Ptr != శూన్యసూచిక ? (length >= 0 ? length : u_strlen(పాఠ్యం_Ptr)) : 0, position),
  పాఠ్యం(పాఠ్యం_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr పాఠ్యం_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t పాఠ్యం_Begin,
                                               int32_t పాఠ్యం_చివర,
                                               int32_t position)
  : CharacterIterator(పాఠ్యం_Ptr != శూన్యసూచిక ? (length >= 0 ? length : u_strlen(పాఠ్యం_Ptr)) : 0,
                      పాఠ్యం_Begin, పాఠ్యం_చివర, position),
  పాఠ్యం(పాఠ్యం_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(స్థిరరాశి UCharCharacterIterator& that)
: CharacterIterator(that),
  పాఠ్యం(that.పాఠ్యం)
{
}

UCharCharacterIterator&
UCharCharacterIterator::క్రియాకారకం=(స్థిరరాశి UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::క్రియాకారకం=(that);
    పాఠ్యం = that.పాఠ్యం;
    తిరిగి *ఇది;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

తార్కిక
UCharCharacterIterator::క్రియాకారకం==(స్థిరరాశి ForwardCharacterIterator& that) స్థిరరాశి {
    ఒకవేళ (ఇది == &that) {
        తిరిగి నిజం;
    }
    ఒకవేళ (typeid(*ఇది) != typeid(that)) {
        తిరిగి అబద్ధం;
    }

    స్థిరరాశి UCharCharacterIterator&    realThat = static_cast<స్థిరరాశి UCharCharacterIterator&>(that);

    తిరిగి పాఠ్యం == realThat.పాఠ్యం
        && పాఠ్యం_నిడివి == realThat.పాఠ్యం_నిడివి
        && pos == realThat.pos
        && begin == realThat.begin
        && చివర == realThat.చివర;
}

int32_t
UCharCharacterIterator::hash_సంకేతం() స్థిరరాశి {
    తిరిగి ustr_hashUCharsN(పాఠ్యం, పాఠ్యం_నిడివి) ^ pos ^ begin ^ చివర;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() స్థిరరాశి {
    తిరిగి కొత్త UCharCharacterIterator(*ఇది);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ఒకవేళ(pos < చివర) {
        తిరిగి పాఠ్యం[pos];
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

char16_t
UCharCharacterIterator::మొదటి_Post_Inc() {
    pos = begin;
    ఒకవేళ(pos < చివర) {
        తిరిగి పాఠ్యం[pos++];
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

char16_t
UCharCharacterIterator::చివరి() {
    pos = చివర;
    ఒకవేళ(pos > begin) {
        తిరిగి పాఠ్యం[--pos];
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

char16_t
UCharCharacterIterator::సమితి_సూచీ(int32_t position) {
    ఒకవేళ(position < begin) {
        pos = begin;
    } లేకపోతే ఒకవేళ(position > చివర) {
        pos = చివర;
    } లేకపోతే {
        pos = position;
    }
    ఒకవేళ(pos < చివర) {
        తిరిగి పాఠ్యం[pos];
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

char16_t
UCharCharacterIterator::current() స్థిరరాశి {
    ఒకవేళ (pos >= begin && pos < చివర) {
        తిరిగి పాఠ్యం[pos];
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

char16_t
UCharCharacterIterator::next() {
    ఒకవేళ (pos + 1 < చివర) {
        తిరిగి పాఠ్యం[++pos];
    } లేకపోతే {
        /* make current() return DONE */
        pos = చివర;
        తిరిగి పూర్తి_2;
    }
}

char16_t
UCharCharacterIterator::తదుపరి_Post_Inc() {
    ఒకవేళ (pos < చివర) {
        తిరిగి పాఠ్యం[pos++];
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

UBool
UCharCharacterIterator::has_తదుపరి() {
    తిరిగి pos < చివర;
}

char16_t
UCharCharacterIterator::మునుపటి() {
    ఒకవేళ (pos > begin) {
        తిరిగి పాఠ్యం[--pos];
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

UBool
UCharCharacterIterator::has_మునుపటి() {
    తిరిగి pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ఒకవేళ(pos < చివర) {
        int32_t i = pos;
        UChar32 c;
        U16_తదుపరి(పాఠ్యం, i, చివర, c);
        తిరిగి c;
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ఒకవేళ(pos < చివర) {
        UChar32 c;
        U16_తదుపరి(పాఠ్యం, pos, చివర, c);
        తిరిగి c;
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = చివర;
    ఒకవేళ(pos > begin) {
        UChar32 c;
        U16_PREV(పాఠ్యం, begin, pos, c);
        తిరిగి c;
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

UChar32
UCharCharacterIterator::సమితి_Index32(int32_t position) {
    ఒకవేళ(position < begin) {
        position = begin;
    } లేకపోతే ఒకవేళ(position > చివర) {
        position = చివర;
    }
    ఒకవేళ(position < చివర) {
        U16_సమితి_CP_ప్రారంభం(పాఠ్యం, begin, position);
        int32_t i = ఇది->pos = position;
        UChar32 c;
        U16_తదుపరి(పాఠ్యం, i, చివర, c);
        తిరిగి c;
    } లేకపోతే {
        ఇది->pos = position;
        తిరిగి పూర్తి_2;
    }
}

UChar32
UCharCharacterIterator::current32() స్థిరరాశి {
    ఒకవేళ (pos >= begin && pos < చివర) {
        UChar32 c;
        U16_GET(పాఠ్యం, begin, pos, చివర, c);
        తిరిగి c;
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ఒకవేళ (pos < చివర) {
        U16_FWD_1(పాఠ్యం, pos, చివర);
        ఒకవేళ(pos < చివర) {
            int32_t i = pos;
            UChar32 c;
            U16_తదుపరి(పాఠ్యం, i, చివర, c);
            తిరిగి c;
        }
    }
    /* make current() return DONE */
    pos = చివర;
    తిరిగి పూర్తి_2;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ఒకవేళ (pos < చివర) {
        UChar32 c;
        U16_తదుపరి(పాఠ్యం, pos, చివర, c);
        తిరిగి c;
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ఒకవేళ (pos > begin) {
        UChar32 c;
        U16_PREV(పాఠ్యం, begin, pos, c);
        తిరిగి c;
    } లేకపోతే {
        తిరిగి పూర్తి_2;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    ఎంపిక(origin) {
    స్థితి k_ప్రారంభం:
        pos = begin + delta;
        ఆపు;
    స్థితి kCurrent:
        pos += delta;
        ఆపు;
    స్థితి k_చివర:
        pos = చివర + delta;
        ఆపు;
    అప్రమేయ:
        ఆపు;
    }

    ఒకవేళ(pos < begin) {
        pos = begin;
    } లేకపోతే ఒకవేళ(pos > చివర) {
        pos = చివర;
    }

    తిరిగి pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    ఎంపిక(origin) {
    స్థితి k_ప్రారంభం:
        pos = begin;
        ఒకవేళ(delta > 0) {
            U16_FWD_N(పాఠ్యం, pos, చివర, delta);
        }
        ఆపు;
    స్థితి kCurrent:
        ఒకవేళ(delta > 0) {
            U16_FWD_N(పాఠ్యం, pos, చివర, delta);
        } లేకపోతే {
            U16_వెనుక_N(పాఠ్యం, begin, pos, -delta);
        }
        ఆపు;
    స్థితి k_చివర:
        pos = చివర;
        ఒకవేళ(delta < 0) {
            U16_వెనుక_N(పాఠ్యం, begin, pos, -delta);
        }
        ఆపు;
    అప్రమేయ:
        ఆపు;
    }

    తిరిగి pos;
}

శూన్య UCharCharacterIterator::సమితి_పాఠ్యం(ConstChar16Ptr new_పాఠ్యం,
                                     int32_t      new_పాఠ్యం_నిడివి) {
    పాఠ్యం = new_పాఠ్యం;
    ఒకవేళ (new_పాఠ్యం == శూన్యసూచిక || new_పాఠ్యం_నిడివి < 0) {
        new_పాఠ్యం_నిడివి = 0;
    }
    చివర = పాఠ్యం_నిడివి = new_పాఠ్యం_నిడివి;
    pos = begin = 0;
}

శూన్య
UCharCharacterIterator::get_పాఠ్యం(UnicodeString& ఫలితం) {
    ఫలితం = UnicodeString(పాఠ్యం, పాఠ్యం_నిడివి);
}

U_NAMESPACE_చివర
