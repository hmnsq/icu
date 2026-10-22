// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#অন্তর্ভাব "unicode/utypes.h"

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
সার্বজনিক:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_চাবি(nullptr),
        fIndex(-1) {}

    ResourceTracer(ধ্রুব UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_চাবি(nullptr),
        fIndex(-1) {}

    ResourceTracer(ধ্রুব UResourceBundle* resB, ধ্রুব বর্ণ* চাবি) :
        fResB(resB),
        fParent(nullptr),
        f_চাবি(চাবি),
        fIndex(-1) {}

    ResourceTracer(ধ্রুব UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_চাবি(nullptr),
        fIndex(index) {}

    ResourceTracer(ধ্রুব ResourceTracer& parent, ধ্রুব বর্ণ* চাবি) :
        fResB(nullptr),
        fParent(&parent),
        f_চাবি(চাবি),
        fIndex(-1) {}

    ResourceTracer(ধ্রুব ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_চাবি(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    শূন্য trace(ধ্রুব বর্ণ* type) ধ্রুব;
    শূন্য traceOpen() ধ্রুব;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    শূন্য maybeTrace(ধ্রুব বর্ণ* type) ধ্রুব {
        যদি (fResB || fParent) {
            trace(type);
        }
    }

গুপ্ত:
    ধ্রুব UResourceBundle* fResB;
    ধ্রুব ResourceTracer* fParent;
    ধ্রুব বর্ণ* f_চাবি;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) ধ্রুব;

    CharString& getResPath(CharString& output, UErrorCode& status) ধ্রুব;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
সার্বজনিক:
    স্থির শূন্য traceOpen(ধ্রুব বর্ণ* path, ধ্রুব বর্ণ* type, ধ্রুব বর্ণ* name);

গুপ্ত:
    স্থির শূন্য traceOpenDataFile(ধ্রুব বর্ণ* path, ধ্রুব বর্ণ* type, ধ্রুব বর্ণ* name);
    স্থির শূন্য traceOpenResFile(ধ্রুব বর্ণ* path, ধ্রুব বর্ণ* name);
};

U_NAMESPACE_END

#অন্যথা // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
সার্বজনিক:
    ResourceTracer() {}

    ResourceTracer(ধ্রুব শূন্য*) {}

    ResourceTracer(ধ্রুব শূন্য*, ধ্রুব বর্ণ*) {}

    ResourceTracer(ধ্রুব শূন্য*, int32_t) {}

    ResourceTracer(ধ্রুব ResourceTracer&, ধ্রুব বর্ণ*) {}

    ResourceTracer(ধ্রুব ResourceTracer&, int32_t) {}

    শূন্য trace(ধ্রুব বর্ণ*) ধ্রুব {}

    শূন্য traceOpen() ধ্রুব {}

    শূন্য maybeTrace(ধ্রুব বর্ণ*) ধ্রুব {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
সার্বজনিক:
    স্থির শূন্য traceOpen(ধ্রুব বর্ণ*, ধ্রুব বর্ণ*, ধ্রুব বর্ণ*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
