// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#ضمن "unicode/utypes.h"

#אם U_ENABLE_TRACING

بنية UResourceBundle;

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
عام:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        fKey(nullptr),
        fIndex(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        fKey(nullptr),
        fIndex(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, ثابت حرف* key) :
        fResB(resB),
        fParent(nullptr),
        fKey(key),
        fIndex(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        fKey(nullptr),
        fIndex(index) {}

    ResourceTracer(ثابت ResourceTracer& parent, ثابت حرف* key) :
        fResB(nullptr),
        fParent(&parent),
        fKey(key),
        fIndex(-1) {}

    ResourceTracer(ثابت ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        fKey(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    فراغ trace(ثابت حرف* type) ثابت;
    فراغ traceOpen() ثابت;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    فراغ maybeTrace(ثابت حرف* type) ثابت {
        אם (fResB || fParent) {
            trace(type);
        }
    }

خاص:
    ثابت UResourceBundle* fResB;
    ثابت ResourceTracer* fParent;
    ثابت حرف* fKey;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) ثابت;

    CharString& getResPath(CharString& output, UErrorCode& status) ثابت;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
عام:
    ساكن فراغ traceOpen(ثابت حرف* path, ثابت حرف* type, ثابت حرف* name);

خاص:
    ساكن فراغ traceOpenDataFile(ثابت حرف* path, ثابت حرف* type, ثابت حرف* name);
    ساكن فراغ traceOpenResFile(ثابت حرف* path, ثابت حرف* name);
};

U_NAMESPACE_END

#אחרת // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
عام:
    ResourceTracer() {}

    ResourceTracer(ثابت فراغ*) {}

    ResourceTracer(ثابت فراغ*, ثابت حرف*) {}

    ResourceTracer(ثابت فراغ*, int32_t) {}

    ResourceTracer(ثابت ResourceTracer&, ثابت حرف*) {}

    ResourceTracer(ثابت ResourceTracer&, int32_t) {}

    فراغ trace(ثابت حرف*) ثابت {}

    فراغ traceOpen() ثابت {}

    فراغ maybeTrace(ثابت حرف*) ثابت {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
عام:
    ساكن فراغ traceOpen(ثابت حرف*, ثابت حرف*, ثابت حرف*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
