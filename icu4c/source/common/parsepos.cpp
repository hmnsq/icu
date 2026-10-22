// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ParsePosition)

ParsePosition::~ParsePosition() {}

ParsePosition *
ParsePosition::clone() ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ ParsePosition(*ਇਹ);
}

U_NAMESPACE_ਸਿਰਾ
