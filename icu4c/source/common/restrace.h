// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି U_ENABLE_TRACING

ସଂରଚନା UResourceBundle;

U_NAMESPACE_BEGIN

class CharString;

/**
 * Instances of this class store information used to trace reads from resource
 * bundles when ICU is built with --enable-tracing.
 *
 * All arguments of type const UResourceBundle*, const char*, and
 * const ResourceTracer& are stored as pointers. The caller must retain
 * ownership for the lifetime of this ResourceTracer.
 *
 * Exported as U_COMMON_API for Windows because it is a value field
 * in other exported types.
 */
class U_COMMON_API ResourceTracer {
ସର୍ବସାଧାରଣ:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_ଚାବି(nullptr),
        fIndex(-1) {}

    ResourceTracer(ଅଚଳ UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_ଚାବି(nullptr),
        fIndex(-1) {}

    ResourceTracer(ଅଚଳ UResourceBundle* resB, ଅଚଳ ଅକ୍ଷର* ଚାବି) :
        fResB(resB),
        fParent(nullptr),
        f_ଚାବି(ଚାବି),
        fIndex(-1) {}

    ResourceTracer(ଅଚଳ UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_ଚାବି(nullptr),
        fIndex(index) {}

    ResourceTracer(ଅଚଳ ResourceTracer& parent, ଅଚଳ ଅକ୍ଷର* ଚାବି) :
        fResB(nullptr),
        fParent(&parent),
        f_ଚାବି(ଚାବି),
        fIndex(-1) {}

    ResourceTracer(ଅଚଳ ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_ଚାବି(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    ଶୂନ୍ୟ trace(ଅଚଳ ଅକ୍ଷର* type) ଅଚଳ;
    ଶୂନ୍ୟ traceOpen() ଅଚଳ;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    ଶୂନ୍ୟ maybeTrace(ଅଚଳ ଅକ୍ଷର* type) ଅଚଳ {
        ଯଦି (fResB || fParent) {
            trace(type);
        }
    }

ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ UResourceBundle* fResB;
    ଅଚଳ ResourceTracer* fParent;
    ଅଚଳ ଅକ୍ଷର* f_ଚାବି;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) ଅଚଳ;

    CharString& getResPath(CharString& output, UErrorCode& status) ଅଚଳ;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
ସର୍ବସାଧାରଣ:
    ସ୍ଥିର ଶୂନ୍ୟ traceOpen(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* type, ଅଚଳ ଅକ୍ଷର* name);

ବ୍ୟକ୍ତିଗତ:
    ସ୍ଥିର ଶୂନ୍ୟ traceOpenDataFile(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* type, ଅଚଳ ଅକ୍ଷର* name);
    ସ୍ଥିର ଶୂନ୍ୟ traceOpenResFile(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* name);
};

U_NAMESPACE_END

#ଅନ୍ୟଥା // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
ସର୍ବସାଧାରଣ:
    ResourceTracer() {}

    ResourceTracer(ଅଚଳ ଶୂନ୍ୟ*) {}

    ResourceTracer(ଅଚଳ ଶୂନ୍ୟ*, ଅଚଳ ଅକ୍ଷର*) {}

    ResourceTracer(ଅଚଳ ଶୂନ୍ୟ*, int32_t) {}

    ResourceTracer(ଅଚଳ ResourceTracer&, ଅଚଳ ଅକ୍ଷର*) {}

    ResourceTracer(ଅଚଳ ResourceTracer&, int32_t) {}

    ଶୂନ୍ୟ trace(ଅଚଳ ଅକ୍ଷର*) ଅଚଳ {}

    ଶୂନ୍ୟ traceOpen() ଅଚଳ {}

    ଶୂନ୍ୟ maybeTrace(ଅଚଳ ଅକ୍ଷର*) ଅଚଳ {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
ସର୍ବସାଧାରଣ:
    ସ୍ଥିର ଶୂନ୍ୟ traceOpen(ଅଚଳ ଅକ୍ଷର*, ଅଚଳ ଅକ୍ଷର*, ଅଚଳ ଅକ୍ଷର*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
