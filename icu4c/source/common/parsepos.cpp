// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2003-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ParsePosition)

ParsePosition::~ParsePosition() {}

ParsePosition *
ParsePosition::clone() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ ParsePosition(*ಏತತ್);
}

U_NAMESPACE_END
