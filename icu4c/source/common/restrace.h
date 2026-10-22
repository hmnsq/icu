// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#شامل "unicode/utypes.h"

#اگر U_ENABLE_TRACING

ساختار UResourceBundle;

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
عمومی:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_کلید(nullptr),
        fIndex(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_کلید(nullptr),
        fIndex(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, ثابت نویسه* کلید) :
        fResB(resB),
        fParent(nullptr),
        f_کلید(کلید),
        fIndex(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_کلید(nullptr),
        fIndex(index) {}

    ResourceTracer(ثابت ResourceTracer& parent, ثابت نویسه* کلید) :
        fResB(nullptr),
        fParent(&parent),
        f_کلید(کلید),
        fIndex(-1) {}

    ResourceTracer(ثابت ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_کلید(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    تهی trace(ثابت نویسه* type) ثابت;
    تهی traceOpen() ثابت;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    تهی maybeTrace(ثابت نویسه* type) ثابت {
        اگر (fResB || fParent) {
            trace(type);
        }
    }

خصوصی:
    ثابت UResourceBundle* fResB;
    ثابت ResourceTracer* fParent;
    ثابت نویسه* f_کلید;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) ثابت;

    CharString& getResPath(CharString& output, UErrorCode& status) ثابت;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
عمومی:
    ایستا تهی traceOpen(ثابت نویسه* path, ثابت نویسه* type, ثابت نویسه* name);

خصوصی:
    ایستا تهی traceOpenDataFile(ثابت نویسه* path, ثابت نویسه* type, ثابت نویسه* name);
    ایستا تهی traceOpenResFile(ثابت نویسه* path, ثابت نویسه* name);
};

U_NAMESPACE_END

#وگرنه // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
عمومی:
    ResourceTracer() {}

    ResourceTracer(ثابت تهی*) {}

    ResourceTracer(ثابت تهی*, ثابت نویسه*) {}

    ResourceTracer(ثابت تهی*, int32_t) {}

    ResourceTracer(ثابت ResourceTracer&, ثابت نویسه*) {}

    ResourceTracer(ثابت ResourceTracer&, int32_t) {}

    تهی trace(ثابت نویسه*) ثابت {}

    تهی traceOpen() ثابت {}

    تهی maybeTrace(ثابت نویسه*) ثابت {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
عمومی:
    ایستا تهی traceOpen(ثابت نویسه*, ثابت نویسه*, ثابت نویسه*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
