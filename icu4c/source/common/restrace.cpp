// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_ENABLE_TRACING

#ਅਨ੍ਤਰ੍ਭਾਵ "restrace.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utracimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = ਮੂਲ;

ਖਾਲੀ ResourceTracer::trace(ਅਚਲ ਅੱਖਰ* resType) ਅਚਲ {
    U_ASSERT(fResB || f_ਮਾਪਾ);
    UTRACE_ਇੰਦਰਾਜ(UTRACE_UDATA_RESOURCE);
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

    CharString file_ਰਸਤਾ;
    get_File_ਰਸਤਾ(file_ਰਸਤਾ, ਸਥਿਤੀ);

    CharString res_ਰਸਤਾ;
    get_Res_ਰਸਤਾ(res_ਰਸਤਾ, ਸਥਿਤੀ);

    // The longest type ("intvector") is 9 chars
    ਅਚਲ ਅੱਖਰ kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, ਆਕਾਰ(kSpaces) - 1 - uprv_strlen(resType), ਸਥਿਤੀ);
    format.append("(%s) %s @ %s", ਸਥਿਤੀ);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.ਅੰਕੜੇ(),
        resType,
        file_ਰਸਤਾ.ਅੰਕੜੇ(),
        res_ਰਸਤਾ.ਅੰਕੜੇ());
    UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
}

ਖਾਲੀ ResourceTracer::traceOpen() ਅਚਲ {
    U_ASSERT(fResB);
    UTRACE_ਇੰਦਰਾਜ(UTRACE_UDATA_BUNDLE);
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

    CharString file_ਰਸਤਾ;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", get_File_ਰਸਤਾ(file_ਰਸਤਾ, ਸਥਿਤੀ).ਅੰਕੜੇ());
    UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
}

CharString& ResourceTracer::get_File_ਰਸਤਾ(CharString& output, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_ਅੰਕੜੇ->f_ਰਸਤਾ, ਸਥਿਤੀ);
        output.append('/', ਸਥਿਤੀ);
        output.append(fResB->f_ਅੰਕੜੇ->fName, ਸਥਿਤੀ);
        output.append(".res", ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        f_ਮਾਪਾ->get_File_ਰਸਤਾ(output, ਸਥਿਤੀ);
    }
    ਵਾਪਸ output;
}

CharString& ResourceTracer::get_Res_ਰਸਤਾ(CharString& output, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (fResB) {
        output.append('/', ਸਥਿਤੀ);
        output.append(fResB->f_Res_ਰਸਤਾ, ਸਥਿਤੀ);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } ਨਹੀਂਤਾਂ {
        f_ਮਾਪਾ->get_Res_ਰਸਤਾ(output, ਸਥਿਤੀ);
    }
    ਜੇ (f_ਕੁੰਜੀ) {
        output.append('/', ਸਥਿਤੀ);
        output.append(f_ਕੁੰਜੀ, ਸਥਿਤੀ);
    }
    ਜੇ (f_ਸੂਚਕਾਂਕ != -1) {
        output.append('[', ਸਥਿਤੀ);
        UnicodeString ਸੂਚਕਾਂਕ_String;
        ICU_Utility::append_ਸੰਖਿਆ(ਸੂਚਕਾਂਕ_String, f_ਸੂਚਕਾਂਕ);
        output.appendInvariantChars(ਸੂਚਕਾਂਕ_String, ਸਥਿਤੀ);
        output.append(']', ਸਥਿਤੀ);
    }
    ਵਾਪਸ output;
}

ਖਾਲੀ FileTracer::traceOpen(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* type, ਅਚਲ ਅੱਖਰ* name) {
    ਜੇ (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(ਰਸਤਾ, name);
    } ਨਹੀਂਤਾਂ {
        trace_Open_ਅੰਕੜੇ_File(ਰਸਤਾ, type, name);
    }
}

ਖਾਲੀ FileTracer::trace_Open_ਅੰਕੜੇ_File(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* type, ਅਚਲ ਅੱਖਰ* name) {
    UTRACE_ਇੰਦਰਾਜ(UTRACE_UDATA_ਅੰਕੜੇ_FILE);
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

    CharString file_ਰਸਤਾ;
    file_ਰਸਤਾ.append(ਰਸਤਾ, ਸਥਿਤੀ);
    file_ਰਸਤਾ.append('/', ਸਥਿਤੀ);
    file_ਰਸਤਾ.append(name, ਸਥਿਤੀ);
    file_ਰਸਤਾ.append('.', ਸਥਿਤੀ);
    file_ਰਸਤਾ.append(type, ਸਥਿਤੀ);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_ਰਸਤਾ.ਅੰਕੜੇ());
    UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
}

ਖਾਲੀ FileTracer::traceOpenResFile(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, ਅਚਲ ਅੱਖਰ* name) {
    UTRACE_ਇੰਦਰਾਜ(UTRACE_UDATA_RES_FILE);
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

    CharString file_ਰਸਤਾ;
    file_ਰਸਤਾ.append(ਰਸਤਾ, ਸਥਿਤੀ);
    file_ਰਸਤਾ.append('/', ਸਥਿਤੀ);
    file_ਰਸਤਾ.append(name, ਸਥਿਤੀ);
    file_ਰਸਤਾ.append(".res", ਸਥਿਤੀ);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_ਰਸਤਾ.ਅੰਕੜੇ());
    UTRACE_EXIT_ਸਥਿਤੀ(ਸਥਿਤੀ);
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ // U_ENABLE_TRACING
