// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_ENABLE_TRACING

কাঠামো UResourceBundle;

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
সর্বজনীন:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_চাবি(nullptr),
        f_সূচক(-1) {}

    ResourceTracer(ধ্রুবক UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_চাবি(nullptr),
        f_সূচক(-1) {}

    ResourceTracer(ধ্রুবক UResourceBundle* resB, ধ্রুবক অক্ষর* চাবি) :
        fResB(resB),
        fParent(nullptr),
        f_চাবি(চাবি),
        f_সূচক(-1) {}

    ResourceTracer(ধ্রুবক UResourceBundle* resB, int32_t সূচক) :
        fResB(resB),
        fParent(nullptr),
        f_চাবি(nullptr),
        f_সূচক(সূচক) {}

    ResourceTracer(ধ্রুবক ResourceTracer& parent, ধ্রুবক অক্ষর* চাবি) :
        fResB(nullptr),
        fParent(&parent),
        f_চাবি(চাবি),
        f_সূচক(-1) {}

    ResourceTracer(ধ্রুবক ResourceTracer& parent, int32_t সূচক) :
        fResB(nullptr),
        fParent(&parent),
        f_চাবি(nullptr),
        f_সূচক(সূচক) {}

    ~ResourceTracer();

    শূন্য trace(ধ্রুবক অক্ষর* type) ধ্রুবক;
    শূন্য traceOpen() ধ্রুবক;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    শূন্য maybeTrace(ধ্রুবক অক্ষর* type) ধ্রুবক {
        যদি (fResB || fParent) {
            trace(type);
        }
    }

ব্যক্তিগত:
    ধ্রুবক UResourceBundle* fResB;
    ধ্রুবক ResourceTracer* fParent;
    ধ্রুবক অক্ষর* f_চাবি;
    int32_t f_সূচক;

    CharString& getFilePath(CharString& output, UErrorCode& status) ধ্রুবক;

    CharString& getResPath(CharString& output, UErrorCode& status) ধ্রুবক;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
সর্বজনীন:
    স্থির শূন্য traceOpen(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* type, ধ্রুবক অক্ষর* name);

ব্যক্তিগত:
    স্থির শূন্য trace_Open_তথ্য_File(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* type, ধ্রুবক অক্ষর* name);
    স্থির শূন্য traceOpenResFile(ধ্রুবক অক্ষর* path, ধ্রুবক অক্ষর* name);
};

U_NAMESPACE_END

#অন্যথা // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
সর্বজনীন:
    ResourceTracer() {}

    ResourceTracer(ধ্রুবক শূন্য*) {}

    ResourceTracer(ধ্রুবক শূন্য*, ধ্রুবক অক্ষর*) {}

    ResourceTracer(ধ্রুবক শূন্য*, int32_t) {}

    ResourceTracer(ধ্রুবক ResourceTracer&, ধ্রুবক অক্ষর*) {}

    ResourceTracer(ধ্রুবক ResourceTracer&, int32_t) {}

    শূন্য trace(ধ্রুবক অক্ষর*) ধ্রুবক {}

    শূন্য traceOpen() ধ্রুবক {}

    শূন্য maybeTrace(ধ্রুবক অক্ষর*) ধ্রুবক {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
সর্বজনীন:
    স্থির শূন্য traceOpen(ধ্রুবক অক্ষর*, ধ্রুবক অক্ষর*, ধ্রুবক অক্ষর*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
