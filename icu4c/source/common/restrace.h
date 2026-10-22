// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __RESTRACE_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __RESTRACE_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_ENABLE_TRACING

ਬਣਤਰ UResourceBundle;

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
ਜਨਤਕ:
    ResourceTracer() :
        fResB(nullptr),
        f_ਮਾਪਾ(nullptr),
        f_ਕੁੰਜੀ(nullptr),
        f_ਸੂਚਕਾਂਕ(-1) {}

    ResourceTracer(ਅਚਲ UResourceBundle* resB) :
        fResB(resB),
        f_ਮਾਪਾ(nullptr),
        f_ਕੁੰਜੀ(nullptr),
        f_ਸੂਚਕਾਂਕ(-1) {}

    ResourceTracer(ਅਚਲ UResourceBundle* resB, ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ) :
        fResB(resB),
        f_ਮਾਪਾ(nullptr),
        f_ਕੁੰਜੀ(ਕੁੰਜੀ),
        f_ਸੂਚਕਾਂਕ(-1) {}

    ResourceTracer(ਅਚਲ UResourceBundle* resB, int32_t ਸੂਚਕਾਂਕ) :
        fResB(resB),
        f_ਮਾਪਾ(nullptr),
        f_ਕੁੰਜੀ(nullptr),
        f_ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ) {}

    ResourceTracer(ਅਚਲ ResourceTracer& ਮਾਪਾ, ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ) :
        fResB(nullptr),
        f_ਮਾਪਾ(&ਮਾਪਾ),
        f_ਕੁੰਜੀ(ਕੁੰਜੀ),
        f_ਸੂਚਕਾਂਕ(-1) {}

    ResourceTracer(ਅਚਲ ResourceTracer& ਮਾਪਾ, int32_t ਸੂਚਕਾਂਕ) :
        fResB(nullptr),
        f_ਮਾਪਾ(&ਮਾਪਾ),
        f_ਕੁੰਜੀ(nullptr),
        f_ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ) {}

    ~ResourceTracer();

    ਖਾਲੀ trace(ਅਚਲ ਅੱਖਰ* type) ਅਚਲ;
    ਖਾਲੀ traceOpen() ਅਚਲ;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    ਖਾਲੀ maybeTrace(ਅਚਲ ਅੱਖਰ* type) ਅਚਲ {
        ਜੇ (fResB || f_ਮਾਪਾ) {
            trace(type);
        }
    }

ਨਿਜੀ:
    ਅਚਲ UResourceBundle* fResB;
    ਅਚਲ ResourceTracer* f_ਮਾਪਾ;
    ਅਚਲ ਅੱਖਰ* f_ਕੁੰਜੀ;
    int32_t f_ਸੂਚਕਾਂਕ;

    CharString& get_File_ਰਸਤਾ(CharString& output, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;

    CharString& get_Res_ਰਸਤਾ(CharString& output, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
ਜਨਤਕ:
    ਸਥਿਰ ਖਾਲੀ traceOpen(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* type, ਅਚਲ ਅੱਖਰ* name);

ਨਿਜੀ:
    ਸਥਿਰ ਖਾਲੀ trace_Open_ਅੰਕੜੇ_File(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* type, ਅਚਲ ਅੱਖਰ* name);
    ਸਥਿਰ ਖਾਲੀ traceOpenResFile(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* name);
};

U_NAMESPACE_ਸਿਰਾ

#ਨਹੀਂਤਾਂ // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
ਜਨਤਕ:
    ResourceTracer() {}

    ResourceTracer(ਅਚਲ ਖਾਲੀ*) {}

    ResourceTracer(ਅਚਲ ਖਾਲੀ*, ਅਚਲ ਅੱਖਰ*) {}

    ResourceTracer(ਅਚਲ ਖਾਲੀ*, int32_t) {}

    ResourceTracer(ਅਚਲ ResourceTracer&, ਅਚਲ ਅੱਖਰ*) {}

    ResourceTracer(ਅਚਲ ResourceTracer&, int32_t) {}

    ਖਾਲੀ trace(ਅਚਲ ਅੱਖਰ*) ਅਚਲ {}

    ਖਾਲੀ traceOpen() ਅਚਲ {}

    ਖਾਲੀ maybeTrace(ਅਚਲ ਅੱਖਰ*) ਅਚਲ {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
ਜਨਤਕ:
    ਸਥਿਰ ਖਾਲੀ traceOpen(ਅਚਲ ਅੱਖਰ*, ਅਚਲ ਅੱਖਰ*, ਅਚਲ ਅੱਖਰ*) {}
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ // U_ENABLE_TRACING

#ਜੇ_ਅੰਤ //__RESTRACE_H__
