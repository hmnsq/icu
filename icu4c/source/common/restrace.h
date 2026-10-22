// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি U_ENABLE_TRACING

সংরচনা UResourceBundle;

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
সার্ৱজনিক:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_চাবি(nullptr),
        fIndex(-1) {}

    ResourceTracer(ধ্রুৱ UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_চাবি(nullptr),
        fIndex(-1) {}

    ResourceTracer(ধ্রুৱ UResourceBundle* resB, ধ্রুৱ ৱর্ণ* চাবি) :
        fResB(resB),
        fParent(nullptr),
        f_চাবি(চাবি),
        fIndex(-1) {}

    ResourceTracer(ধ্রুৱ UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_চাবি(nullptr),
        fIndex(index) {}

    ResourceTracer(ধ্রুৱ ResourceTracer& parent, ধ্রুৱ ৱর্ণ* চাবি) :
        fResB(nullptr),
        fParent(&parent),
        f_চাবি(চাবি),
        fIndex(-1) {}

    ResourceTracer(ধ্রুৱ ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_চাবি(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    শূন্য trace(ধ্রুৱ ৱর্ণ* type) ধ্রুৱ;
    শূন্য traceOpen() ধ্রুৱ;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    শূন্য maybeTrace(ধ্রুৱ ৱর্ণ* type) ধ্রুৱ {
        যদি (fResB || fParent) {
            trace(type);
        }
    }

গুপ্ত:
    ধ্রুৱ UResourceBundle* fResB;
    ধ্রুৱ ResourceTracer* fParent;
    ধ্রুৱ ৱর্ণ* f_চাবি;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) ধ্রুৱ;

    CharString& getResPath(CharString& output, UErrorCode& status) ধ্রুৱ;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
সার্ৱজনিক:
    স্থির শূন্য traceOpen(ধ্রুৱ ৱর্ণ* path, ধ্রুৱ ৱর্ণ* type, ধ্রুৱ ৱর্ণ* name);

গুপ্ত:
    স্থির শূন্য traceOpenDataFile(ধ্রুৱ ৱর্ণ* path, ধ্রুৱ ৱর্ণ* type, ধ্রুৱ ৱর্ণ* name);
    স্থির শূন্য traceOpenResFile(ধ্রুৱ ৱর্ণ* path, ধ্রুৱ ৱর্ণ* name);
};

U_NAMESPACE_END

#অন্যথা // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
সার্ৱজনিক:
    ResourceTracer() {}

    ResourceTracer(ধ্রুৱ শূন্য*) {}

    ResourceTracer(ধ্রুৱ শূন্য*, ধ্রুৱ ৱর্ণ*) {}

    ResourceTracer(ধ্রুৱ শূন্য*, int32_t) {}

    ResourceTracer(ধ্রুৱ ResourceTracer&, ধ্রুৱ ৱর্ণ*) {}

    ResourceTracer(ধ্রুৱ ResourceTracer&, int32_t) {}

    শূন্য trace(ধ্রুৱ ৱর্ণ*) ধ্রুৱ {}

    শূন্য traceOpen() ধ্রুৱ {}

    শূন্য maybeTrace(ধ্রুৱ ৱর্ণ*) ধ্রুৱ {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
সার্ৱজনিক:
    স্থির শূন্য traceOpen(ধ্রুৱ ৱর্ণ*, ধ্রুৱ ৱর্ণ*, ধ্রুৱ ৱর্ণ*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
