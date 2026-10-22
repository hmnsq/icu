// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1997-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
* File resbund.cpp
*
* Modification History:
*
*   Date        Name        Description
*   02/05/97    aliu        Fixed bug in chopLocale.  Added scanForLocaleInFile
*                           based on code taken from scanForLocale.  Added
*                           constructor which attempts to read resource bundle
*                           from a specific file, without searching other files.
*   02/11/97    aliu        Added UErrorCode return values to constructors. Fixed
*                           infinite loops in scanForFile and scanForLocale.
*                           Modified getRawResourceData to not delete storage in
*                           localeData and resourceData which it doesn't own.
*                           Added Mac compatibility #ifdefs for tellp() and
*                           ios::nocreate.
*   03/04/97    aliu        Modified to use ExpandingDataSink objects instead of
*                           the highly inefficient ostrstream objects.
*   03/13/97    aliu        Rewrote to load in entire resource bundle and store
*                           it as a Hashtable of ResourceBundleData objects.
*                           Added state table to govern parsing of files.
*                           Modified to load locale index out of new file distinct
*                           from default.txt.
*   03/25/97    aliu        Modified to support 2-d arrays, needed for timezone data.
*                           Added support for custom file suffixes.  Again, needed
*                           to support timezone data.  Improved error handling to
*                           detect duplicate tags and subtags.
*   04/07/97    aliu        Fixed bug in getHashtableForLocale().  Fixed handling
*                           of failing UErrorCode values on entry to API methods.
*                           Fixed bugs in getArrayItem() for negative indices.
*   04/29/97    aliu        Update to use new Hashtable deletion protocol.
*   05/06/97    aliu        Flattened kTransitionTable for HP compiler.
*                           Fixed usage of CharString.
* 06/11/99      stephen     Removed parsing of .txt files.
*                           Reworked to use new binary format.
*                           Cleaned up.
* 06/14/99      stephen     Removed methods taking a filename suffix.
* 06/22/99      stephen     Added missing T_FileStream_close in parse()
* 11/09/99      weiv        Added getLocale(), rewritten constructForLocale()
* March 2000    weiv        complete overhaul.
******************************************************************************
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/resbund.h"

#ضمن "cmemory.h"
#ضمن "mutex.h"
#ضمن "uassert.h"
#ضمن "umutex.h"

#ضمن "uresimp.h"

U_NAMESPACE_BEGIN

/*-----------------------------------------------------------------------------
 * Implementation Notes
 *
 * Resource bundles are read in once, and thereafter cached.
 * ResourceBundle statically keeps track of which files have been
 * read, so we are guaranteed that each file is read at most once.
 * Resource bundles can be loaded from different data directories and
 * will be treated as distinct, even if they are for the same locale.
 *
 * Resource bundles are lightweight objects, which have pointers to
 * one or more shared Hashtable objects containing all the data.
 * Copying would be cheap, but there is no copy constructor, since
 * there wasn't one in the original API.
 *
 * The ResourceBundle parsing mechanism is implemented as a transition
 * network, for easy maintenance and modification.  The network is
 * implemented as a matrix (instead of in code) to make this even
 * easier.  The matrix contains Transition objects.  Each Transition
 * object describes a destination node and an action to take before
 * moving to the destination node.  The source node is encoded by the
 * index of the object in the array that contains it.  The pieces
 * needed to understand the transition network are the enums for node
 * IDs and actions, the parse() method, which walks through the
 * network and implements the actions, and the network itself.  The
 * network guarantees certain conditions, for example, that a new
 * resource will not be closed until one has been opened first; or
 * that data will not be stored into a TaggedList until a TaggedList
 * has been created.  Nonetheless, the code in parse() does some
 * consistency checks as it runs the network, and fails with an
 * U_INTERNAL_PROGRAM_ERROR if one of these checks fails.  If the input
 * data has a bad format, an U_INVALID_FORMAT_ERROR is returned.  If you
 * see an U_INTERNAL_PROGRAM_ERROR the transition matrix has a bug in
 * it.
 *
 * Old functionality of multiple locales in a single file is still
 * supported.  For this reason, LOCALE names override FILE names.  If
 * data for en_US is located in the en.txt file, once it is loaded,
 * the code will not care where it came from (other than remembering
 * which directory it came from).  However, if there is an en_US
 * resource in en_US.txt, that will take precedence.  There is no
 * limit to the number or type of resources that can be stored in a
 * file, however, files are only searched in a specific way.  If
 * en_US_CA is requested, then first en_US_CA.txt is searched, then
 * en_US.txt, then en.txt, then default.txt.  So it only makes sense
 * to put certain locales in certain files.  In this example, it would
 * be logical to put en_US_CA, en_US, and en into the en.txt file,
 * since they would be found there if asked for.  The extreme example
 * is to place all locale resources into default.txt, which should
 * also work.
 *
 * Inheritance is implemented.  For example, xx_YY_zz inherits as
 * follows: xx_YY_zz, xx_YY, xx, default.  Inheritance is implemented
 * as an array of hashtables.  There will be from 1 to 4 hashtables in
 * the array.
 *
 * Fallback files are implemented.  The fallback pattern is Language
 * Country Variant (LCV) -> LC -> L.  Fallback is first done for the
 * requested locale.  Then it is done for the default locale, as
 * returned by Locale::getDefault().  Then the special file
 * default.txt is searched for the default locale.  The overall FILE
 * fallback path is LCV -> LC -> L -> dLCV -> dLC -> dL -> default.
 *
 * Note that although file name searching includes the default locale,
 * once a ResourceBundle object is constructed, the inheritance path
 * no longer includes the default locale.  The path is LCV -> LC -> L
 * -> default.
 *
 * File parsing is lazy.  Nothing is parsed unless it is called for by
 * someone.  So when a ResourceBundle for xx_YY_zz is constructed,
 * only that locale is parsed (along with anything else in the same
 * file).  Later, if the FooBar tag is asked for, and if it isn't
 * found in xx_YY_zz, then xx_YY.txt will be parsed and checked, and
 * so forth, until the chain is exhausted or the tag is found.
 *
 * Thread-safety is implemented around caches, both the cache that
 * stores all the resource data, and the cache that stores flags
 * indicating whether or not a file has been visited.  These caches
 * delete their storage at static cleanup time, when the process
 * quits.
 *
 * ResourceBundle supports TableCollation as a special case.  This
 * involves having special ResourceBundle objects which DO own their
 * data, since we don't want large collation rule strings in the
 * ResourceBundle cache (these are already cached in the
 * TableCollation cache).  TableCollation files (.ctx files) have the
 * same format as normal resource data files, with a different
 * interpretation, from the standpoint of ResourceBundle.  .ctx files
 * are loaded into otherwise ordinary ResourceBundle objects.  They
 * don't inherit (that's implemented by TableCollation) and they own
 * their data (as mentioned above).  However, they still support
 * possible multiple locales in a single .ctx file.  (This is in
 * practice a bad idea, since you only want the one locale you're
 * looking for, and only one tag will be present
 * ("CollationElements"), so you don't need an inheritance chain of
 * multiple locales.)  Up to 4 locale resources will be loaded from a
 * .ctx file; everything after the first 4 is ignored (parsed and
 * deleted).  (Normal .txt files have no limit.)  Instead of being
 * loaded into the cache, and then looked up as needed, the locale
 * resources are read straight into the ResourceBundle object.
 *
 * The Index, which used to reside in default.txt, has been moved to a
 * new file, index.txt.  This file contains a slightly modified format
 * with the addition of the "InstalledLocales" tag; it looks like:
 *
 * Index {
 *   InstalledLocales {
 *     ar
 *     ..
 *     zh_TW
 *   }
 * }
 */
//-----------------------------------------------------------------------------

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ResourceBundle)

ResourceBundle::ResourceBundle(UError_کوڈ &err)
                                :UObject(), fLocale(خالی_سوچک)
{
    fResource = ures_open(خالی_سوچک, Locale::getDefault().getName(), &err);
}

ResourceBundle::ResourceBundle(ثابت ResourceBundle &other)
                              :UObject(other), fLocale(خالی_سوچک)
{
    UError_کوڈ حالت = U_ZERO_ERROR;

    إذا (other.fResource) {
        fResource = ures_copyResb(خالی_سوچک, other.fResource, &حالت);
    } وإلا {
        /* Copying a bad resource bundle */
        fResource = خالی_سوچک;
    }
}

ResourceBundle::ResourceBundle(UResourceBundle *res, UError_کوڈ& err)
                               :UObject(), fLocale(خالی_سوچک)
{
    إذا (res) {
        fResource = ures_copyResb(خالی_سوچک, res, &err);
    } وإلا {
        /* Copying a bad resource bundle */
        fResource = خالی_سوچک;
    }
}

ResourceBundle::ResourceBundle(ثابت حرف* رستہ, ثابت Locale& locale, UError_کوڈ& err) 
                               :UObject(), fLocale(خالی_سوچک)
{
    fResource = ures_open(رستہ, locale.getName(), &err);
}


ResourceBundle& ResourceBundle::عامل=(ثابت ResourceBundle& other) U_LIFETIME_BOUND
{
    إذا(هذا == &other) {
        أرجع *هذا;
    }
    إذا (fResource != خالی_سوچک) {
        ures_close(fResource);
        fResource = خالی_سوچک;
    }
    إذا (fLocale != خالی_سوچک) {
        مٹاؤ fLocale;
        fLocale = خالی_سوچک;
    }
    UError_کوڈ حالت = U_ZERO_ERROR;
    إذا (other.fResource) {
        fResource = ures_copyResb(خالی_سوچک, other.fResource, &حالت);
    } وإلا {
        /* Copying a bad resource bundle */
        fResource = خالی_سوچک;
    }
    أرجع *هذا;
}

ResourceBundle::~ResourceBundle()
{
    إذا (fResource != خالی_سوچک) {
        ures_close(fResource);
    }
    مٹاؤ fLocale;
}

ResourceBundle *
ResourceBundle::clone() ثابت {
    أرجع جديد ResourceBundle(*هذا);
}

UnicodeString ResourceBundle::getString(UError_کوڈ& حالت) ثابت U_LIFETIME_BOUND {
    int32_t len = 0;
    ثابت char16_t *r = ures_getString(fResource, &len, &حالت);
    أرجع UnicodeString(سچ, r, len);
}

ثابت uint8_t* ResourceBundle::get_ثنائی(int32_t& len, UError_کوڈ& حالت) ثابت U_LIFETIME_BOUND {
    أرجع ures_get_ثنائی(fResource, &len, &حالت);
}

ثابت int32_t* ResourceBundle::get_Int_سمتیہ(int32_t& len,
                                            UError_کوڈ& حالت) ثابت U_LIFETIME_BOUND {
    أرجع ures_get_Int_سمتیہ(fResource, &len, &حالت);
}

uint32_t ResourceBundle::getUInt(UError_کوڈ& حالت) ثابت {
    أرجع ures_getUInt(fResource, &حالت);
}

int32_t ResourceBundle::getInt(UError_کوڈ& حالت) ثابت {
    أرجع ures_getInt(fResource, &حالت);
}

ثابت حرف* ResourceBundle::getName() ثابت U_LIFETIME_BOUND {
    أرجع ures_getName(fResource);
}

ثابت حرف* ResourceBundle::get_کلید() ثابت U_LIFETIME_BOUND {
    أرجع ures_get_کلید(fResource);
}

UResType ResourceBundle::getType() ثابت {
    أرجع ures_getType(fResource);
}

int32_t ResourceBundle::get_ناپ() ثابت {
    أرجع ures_get_ناپ(fResource);
}

UBool ResourceBundle::has_اگلا() ثابت {
    أرجع ures_has_اگلا(fResource);
}

فراغ ResourceBundle::resetIterator() {
    ures_resetIterator(fResource);
}

ResourceBundle ResourceBundle::get_اگلا(UError_کوڈ& حالت) {
    UResourceBundle r;

    ures_init_ڈھیر_Object(&r);
    ures_get_اگلا_Resource(fResource, &r, &حالت);
    ResourceBundle res(&r, حالت);
    إذا (U_SUCCESS(حالت)) {
        ures_close(&r);
    }
    أرجع res;
}

UnicodeString ResourceBundle::get_اگلا_String(UError_کوڈ& حالت) U_LIFETIME_BOUND {
    int32_t len = 0;
    ثابت char16_t* r = ures_get_اگلا_String(fResource, &len, خالی_سوچک, &حالت);
    أرجع UnicodeString(سچ, r, len);
}

UnicodeString ResourceBundle::get_اگلا_String(ثابت حرف** کلید, UError_کوڈ& حالت) U_LIFETIME_BOUND {
    int32_t len = 0;
    ثابت char16_t* r = ures_get_اگلا_String(fResource, &len, کلید, &حالت);
    أرجع UnicodeString(سچ, r, len);
}

ResourceBundle ResourceBundle::get(int32_t اشاریہ_R, UError_کوڈ& حالت) ثابت {
    UResourceBundle r;

    ures_init_ڈھیر_Object(&r);
    ures_get_By_اشاریہ(fResource, اشاریہ_R, &r, &حالت);
    ResourceBundle res(&r, حالت);
    إذا (U_SUCCESS(حالت)) {
        ures_close(&r);
    }
    أرجع res;
}

UnicodeString ResourceBundle::getStringEx(int32_t اشاریہ_S,
                                          UError_کوڈ& حالت) ثابت U_LIFETIME_BOUND {
    int32_t len = 0;
    ثابت char16_t* r = ures_get_String_By_اشاریہ(fResource, اشاریہ_S, &len, &حالت);
    أرجع UnicodeString(سچ, r, len);
}

ResourceBundle ResourceBundle::get(ثابت حرف* کلید, UError_کوڈ& حالت) ثابت {
    UResourceBundle r;

    ures_init_ڈھیر_Object(&r);
    ures_get_By_کلید(fResource, کلید, &r, &حالت);
    ResourceBundle res(&r, حالت);
    إذا (U_SUCCESS(حالت)) {
        ures_close(&r);
    }
    أرجع res;
}

ResourceBundle ResourceBundle::getWithFallback(ثابت حرف* کلید, UError_کوڈ& حالت){
    UResourceBundle r;
    ures_init_ڈھیر_Object(&r);
    ures_get_By_کلید_With_Fallback(fResource, کلید, &r, &حالت);
    ResourceBundle res(&r, حالت);
    إذا(U_SUCCESS(حالت)){
        ures_close(&r);
    }
    أرجع res;
}

UnicodeString ResourceBundle::getStringEx(ثابت حرف* کلید,
                                          UError_کوڈ& حالت) ثابت U_LIFETIME_BOUND {
    int32_t len = 0;
    ثابت char16_t* r = ures_get_String_By_کلید(fResource, کلید, &len, &حالت);
    أرجع UnicodeString(سچ, r, len);
}

ثابت حرف* ResourceBundle::get_نسخہ_نمبر() ثابت U_LIFETIME_BOUND {
    أرجع ures_get_نسخہ_نمبر_اندرونی(fResource);
}

فراغ ResourceBundle::get_نسخہ(UVersionInfo نسخہ_Info) ثابت {
    ures_get_نسخہ(fResource, نسخہ_Info);
}

ثابت Locale& ResourceBundle::getLocale() ثابت U_LIFETIME_BOUND {
    ساكن UMutex g_Locale_جندرا;
    Mutex جندرا(&g_Locale_جندرا);
    إذا (fLocale != خالی_سوچک) {
        أرجع *fLocale;
    }
    UError_کوڈ حالت = U_ZERO_ERROR;
    ثابت حرف *localeName = ures_get_Locale_اندرونی(fResource, &حالت);
    ResourceBundle *ncThis = const_cast<ResourceBundle *>(هذا);
    ncThis->fLocale = جديد Locale(localeName);
    أرجع ncThis->fLocale != خالی_سوچک ? *ncThis->fLocale : Locale::getDefault();
}

Locale ResourceBundle::getLocale(ULoc_انکڑے_Locale_Type type, UError_کوڈ &حالت) ثابت
{
  أرجع ures_getLocaleByType(fResource, type, &حالت);
}

U_NAMESPACE_سرا
//eof
