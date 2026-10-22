// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*
* File schriter.cpp
*
* Modification History:
*
*   Date        Name        Description
*  05/05/99     stephen     Cleaned up.
******************************************************************************
*/

#అన్తర్భావ "utypeinfo.h"  // for 'typeid' to work

#అన్తర్భావ "unicode/chariter.h"
#అన్తర్భావ "unicode/schriter.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(StringCharacterIterator)

StringCharacterIterator::StringCharacterIterator()
  : UCharCharacterIterator(),
    పాఠ్యం()
{
  // NEVER DEFAULT CONSTRUCT!
}

StringCharacterIterator::StringCharacterIterator(స్థిరరాశి UnicodeString& పాఠ్యం_Str)
  : UCharCharacterIterator(పాఠ్యం_Str.get_బఫరు(), పాఠ్యం_Str.length()),
    పాఠ్యం(పాఠ్యం_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::పాఠ్యం = ఇది->పాఠ్యం.get_బఫరు();
}

StringCharacterIterator::StringCharacterIterator(స్థిరరాశి UnicodeString& పాఠ్యం_Str,
                                                 int32_t పాఠ్యం_Pos)
  : UCharCharacterIterator(పాఠ్యం_Str.get_బఫరు(), పాఠ్యం_Str.length(), పాఠ్యం_Pos),
    పాఠ్యం(పాఠ్యం_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::పాఠ్యం = ఇది->పాఠ్యం.get_బఫరు();
}

StringCharacterIterator::StringCharacterIterator(స్థిరరాశి UnicodeString& పాఠ్యం_Str,
                                                 int32_t పాఠ్యం_Begin,
                                                 int32_t పాఠ్యం_చివర,
                                                 int32_t పాఠ్యం_Pos)
  : UCharCharacterIterator(పాఠ్యం_Str.get_బఫరు(), పాఠ్యం_Str.length(), పాఠ్యం_Begin, పాఠ్యం_చివర, పాఠ్యం_Pos),
    పాఠ్యం(పాఠ్యం_Str)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::పాఠ్యం = ఇది->పాఠ్యం.get_బఫరు();
}

StringCharacterIterator::StringCharacterIterator(స్థిరరాశి StringCharacterIterator& that)
  : UCharCharacterIterator(that),
    పాఠ్యం(that.పాఠ్యం)
{
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::పాఠ్యం = ఇది->పాఠ్యం.get_బఫరు();
}

StringCharacterIterator::~StringCharacterIterator() {
}

StringCharacterIterator&
StringCharacterIterator::క్రియాకారకం=(స్థిరరాశి StringCharacterIterator& that) U_LIFETIME_BOUND {
    UCharCharacterIterator::క్రియాకారకం=(that);
    పాఠ్యం = that.పాఠ్యం;
    // we had set the input parameter's array, now we need to set our copy's array
    UCharCharacterIterator::పాఠ్యం = ఇది->పాఠ్యం.get_బఫరు();
    తిరిగి *ఇది;
}

తార్కిక
StringCharacterIterator::క్రియాకారకం==(స్థిరరాశి ForwardCharacterIterator& that) స్థిరరాశి {
    ఒకవేళ (ఇది == &that) {
        తిరిగి నిజం;
    }

    // do not call UCharCharacterIterator::operator==()
    // because that checks for array pointer equality
    // while we compare UnicodeString objects

    ఒకవేళ (typeid(*ఇది) != typeid(that)) {
        తిరిగి అబద్ధం;
    }

    స్థిరరాశి StringCharacterIterator&    realThat = static_cast<స్థిరరాశి StringCharacterIterator&>(that);

    తిరిగి పాఠ్యం == realThat.పాఠ్యం
        && pos == realThat.pos
        && begin == realThat.begin
        && చివర == realThat.చివర;
}

StringCharacterIterator*
StringCharacterIterator::clone() స్థిరరాశి {
    తిరిగి కొత్త StringCharacterIterator(*ఇది);
}

శూన్య
StringCharacterIterator::సమితి_పాఠ్యం(స్థిరరాశి UnicodeString& new_పాఠ్యం) {
    పాఠ్యం = new_పాఠ్యం;
    UCharCharacterIterator::సమితి_పాఠ్యం(పాఠ్యం.get_బఫరు(), పాఠ్యం.length());
}

శూన్య
StringCharacterIterator::get_పాఠ్యం(UnicodeString& ఫలితం) {
    ఫలితం = పాఠ్యం;
}
U_NAMESPACE_చివర
