// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_ENABLE_TRACING

#ಅನ್ತರ್ಭಾವ "restrace.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = ಪೂರ್ವನಿಯೋಜಿತ;

ಶೂನ್ಯ ResourceTracer::trace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* resType) ಸ್ಥಿರಾಂಕ {
    U_ASSERT(fResB || f_ಪೋಷಕ);
    UTRACE_ನಮೂದು(UTRACE_UDATA_RESOURCE);
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

    CharString file_ಮಾರ್ಗ;
    get_File_ಮಾರ್ಗ(file_ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);

    CharString res_ಮಾರ್ಗ;
    get_Res_ಮಾರ್ಗ(res_ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);

    // The longest type ("intvector") is 9 chars
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, ಗಾತ್ರ(kSpaces) - 1 - uprv_strlen(resType), ಸ್ಥಿತಿ_2);
    format.append("(%s) %s @ %s", ಸ್ಥಿತಿ_2);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.ದತ್ತಾಂಶ(),
        resType,
        file_ಮಾರ್ಗ.ದತ್ತಾಂಶ(),
        res_ಮಾರ್ಗ.ದತ್ತಾಂಶ());
    UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
}

ಶೂನ್ಯ ResourceTracer::traceOpen() ಸ್ಥಿರಾಂಕ {
    U_ASSERT(fResB);
    UTRACE_ನಮೂದು(UTRACE_UDATA_BUNDLE);
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

    CharString file_ಮಾರ್ಗ;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", get_File_ಮಾರ್ಗ(file_ಮಾರ್ಗ, ಸ್ಥಿತಿ_2).ದತ್ತಾಂಶ());
    UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
}

CharString& ResourceTracer::get_File_ಮಾರ್ಗ(CharString& output, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_ದತ್ತಾಂಶ->f_ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
        output.append('/', ಸ್ಥಿತಿ_2);
        output.append(fResB->f_ದತ್ತಾಂಶ->fName, ಸ್ಥಿತಿ_2);
        output.append(".res", ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        f_ಪೋಷಕ->get_File_ಮಾರ್ಗ(output, ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ output;
}

CharString& ResourceTracer::get_Res_ಮಾರ್ಗ(CharString& output, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (fResB) {
        output.append('/', ಸ್ಥಿತಿ_2);
        output.append(fResB->f_Res_ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        f_ಪೋಷಕ->get_Res_ಮಾರ್ಗ(output, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (f_ಕೀಲಿ) {
        output.append('/', ಸ್ಥಿತಿ_2);
        output.append(f_ಕೀಲಿ, ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (f_ಸೂಚ್ಯಂಕ != -1) {
        output.append('[', ಸ್ಥಿತಿ_2);
        UnicodeString ಸೂಚ್ಯಂಕ_String;
        ICU_Utility::append_ಸಂಖ್ಯೆ(ಸೂಚ್ಯಂಕ_String, f_ಸೂಚ್ಯಂಕ);
        output.appendInvariantChars(ಸೂಚ್ಯಂಕ_String, ಸ್ಥಿತಿ_2);
        output.append(']', ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ output;
}

ಶೂನ್ಯ FileTracer::traceOpen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name) {
    ಆದರೆ (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(ಮಾರ್ಗ, name);
    } ಇಲ್ಲದಿದ್ದರೆ {
        trace_Open_ದತ್ತಾಂಶ_File(ಮಾರ್ಗ, type, name);
    }
}

ಶೂನ್ಯ FileTracer::trace_Open_ದತ್ತಾಂಶ_File(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name) {
    UTRACE_ನಮೂದು(UTRACE_UDATA_ದತ್ತಾಂಶ_FILE);
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

    CharString file_ಮಾರ್ಗ;
    file_ಮಾರ್ಗ.append(ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
    file_ಮಾರ್ಗ.append('/', ಸ್ಥಿತಿ_2);
    file_ಮಾರ್ಗ.append(name, ಸ್ಥಿತಿ_2);
    file_ಮಾರ್ಗ.append('.', ಸ್ಥಿತಿ_2);
    file_ಮಾರ್ಗ.append(type, ಸ್ಥಿತಿ_2);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_ಮಾರ್ಗ.ದತ್ತಾಂಶ());
    UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
}

ಶೂನ್ಯ FileTracer::traceOpenResFile(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name) {
    UTRACE_ನಮೂದು(UTRACE_UDATA_RES_FILE);
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

    CharString file_ಮಾರ್ಗ;
    file_ಮಾರ್ಗ.append(ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
    file_ಮಾರ್ಗ.append('/', ಸ್ಥಿತಿ_2);
    file_ಮಾರ್ಗ.append(name, ಸ್ಥಿತಿ_2);
    file_ಮಾರ್ಗ.append(".res", ಸ್ಥಿತಿ_2);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_ಮಾರ್ಗ.ದತ್ತಾಂಶ());
    UTRACE_EXIT_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_2);
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ // U_ENABLE_TRACING
