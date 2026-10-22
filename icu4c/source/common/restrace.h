// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#که_نه_تعریف_شوی __RESTRACE_H__
#تعریف_کړه __RESTRACE_H__

#ضمن "unicode/utypes.h"

#إذا U_ENABLE_TRACING

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
        f_پلار(nullptr),
        f_کلی(nullptr),
        f_شاخص(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB) :
        fResB(resB),
        f_پلار(nullptr),
        f_کلی(nullptr),
        f_شاخص(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, ثابت حرف* کلی) :
        fResB(resB),
        f_پلار(nullptr),
        f_کلی(کلی),
        f_شاخص(-1) {}

    ResourceTracer(ثابت UResourceBundle* resB, int32_t شاخص) :
        fResB(resB),
        f_پلار(nullptr),
        f_کلی(nullptr),
        f_شاخص(شاخص) {}

    ResourceTracer(ثابت ResourceTracer& پلار, ثابت حرف* کلی) :
        fResB(nullptr),
        f_پلار(&پلار),
        f_کلی(کلی),
        f_شاخص(-1) {}

    ResourceTracer(ثابت ResourceTracer& پلار, int32_t شاخص) :
        fResB(nullptr),
        f_پلار(&پلار),
        f_کلی(nullptr),
        f_شاخص(شاخص) {}

    ~ResourceTracer();

    فراغ trace(ثابت حرف* type) ثابت;
    فراغ traceOpen() ثابت;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    فراغ maybeTrace(ثابت حرف* type) ثابت {
        إذا (fResB || f_پلار) {
            trace(type);
        }
    }

خاص:
    ثابت UResourceBundle* fResB;
    ثابت ResourceTracer* f_پلار;
    ثابت حرف* f_کلی;
    int32_t f_شاخص;

    CharString& get_File_لار(CharString& output, UError_کوډ& حالت) ثابت;

    CharString& get_Res_لار(CharString& output, UError_کوډ& حالت) ثابت;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
عام:
    ساكن فراغ traceOpen(ثابت حرف* لار, ثابت حرف* type, ثابت حرف* name);

خاص:
    ساكن فراغ trace_Open_معلومات_File(ثابت حرف* لار, ثابت حرف* type, ثابت حرف* name);
    ساكن فراغ traceOpenResFile(ثابت حرف* لار, ثابت حرف* name);
};

U_NAMESPACE_پای_ټکی

#وإلا // U_ENABLE_TRACING

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

U_NAMESPACE_پای_ټکی

#د_که_پای // U_ENABLE_TRACING

#د_که_پای //__RESTRACE_H__
