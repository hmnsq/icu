// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2009, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  utracimp.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003aug06
*   created by: Markus W. Scherer
*
*   Internal header for ICU tracing/logging.
*
*
*   Various notes:
*   - using a trace level variable to only call trace functions
*     when the level is sufficient
*   - using the same variable for tracing on/off to never make a function
*     call when off
*   - the function number is put into a local variable by the entry macro
*     and used implicitly to avoid copy&paste/typing mistakes by the developer
*   - the application must call utrace_setFunctions() and pass in
*     implementations for the trace functions
*   - ICU trace macros call ICU functions that route through the function
*     pointers if they have been set;
*     this avoids an indirection at the call site
*     (which would cost more code for another check and for the indirection)
*
*   ### TODO Issues:
*   - Verify that va_list is portable among compilers for the same platform.
*     va_list should be portable because printf() would fail otherwise!
*   - Should enum values like UTraceLevel be passed into int32_t-type arguments,
*     or should enum types be used?
*/

#جيڪڏهن_وصف_نه_ٿيل __UTRACIMP_H__
#وصف_ڪريو __UTRACIMP_H__

#ضمن "unicode/utrace.h"
#ضمن <stdarg.h>

U_CDECL_BEGIN

/** 
 *   Traced Function Exit return types.  
 *   Flags indicating the number and types of varargs included in a call
 *   to a UTraceExit function.
 *   Bits 0-3:  The function return type.  First variable param.
 *   Bit    4:  Flag for presence of U_ErrorCode status param.
 *   @internal
 */
تعريفنوع تعداد UTraceExitVal {
    /** The traced function returns no value  @internal */
    UTRACE_EXITV_NONE   = 0,
    /** The traced function returns an int32_t, or compatible, type.  @internal */
    UTRACE_EXITV_I32    = 1,
    /** The traced function returns a pointer  @internal */
    UTRACE_EXITV_PTR    = 2,
    /** The traced function returns a UBool  @internal */
    UTRACE_EXITV_BOOL   = 3,
    /** Mask to extract the return type values from a UTraceExitVal  @internal */
    UTRACE_EXITV_نقاب   = 0xf,
    /** Bit indicating that the traced function includes a UErrorCode parameter  @internal */
    UTRACE_EXITV_حالت = 0x10
} UTraceExitVal;

/**
 * Trace function for the entry point of a function.
 * Do not use directly, use UTRACE_ENTRY instead.
 * @param fnNumber The UTraceFunctionNumber for the current function.
 * @internal
 */
U_CAPI فراغ U_EXPORT2
utrace_داخلا(int32_t fn_عدد);

/**
 * Trace function for each exit point of a function.
 * Do not use directly, use UTRACE_EXIT* instead.
 * @param fnNumber The UTraceFunctionNumber for the current function.
 * @param returnType The type of the value returned by the function.
 * @param errorCode The UErrorCode value at function exit. See UTRACE_EXIT.
 * @internal
 */
U_CAPI فراغ U_EXPORT2
utrace_exit(int32_t fn_عدد, int32_t returnType, ...);


/**
 * Trace function used inside functions that have a UTRACE_ENTRY() statement.
 * Do not use directly, use UTRACE_DATAX() macros instead.
 *
 * @param utraceFnNumber The number of the current function, from the local
 *        variable of the same name.
 * @param level The trace level for this message.
 * @param fmt The trace format string.
 *
 * @internal
 */
U_CAPI فراغ U_EXPORT2
utrace_معلومات(int32_t utrace_Fn_عدد, int32_t level, ثابت حرف *fmt, ...);

U_CDECL_پڇاڙي

#إذا U_ENABLE_TRACING

/**
 * Boolean expression to see if ICU tracing is turned on
 * to at least the specified level.
 * @internal
 */
#وصف_ڪريو UTRACE_LEVEL(level) (utrace_getLevel()>=(level))

/**
  *  Flag bit in utraceFnNumber, the local variable added to each function 
  *  with tracing code to contains the function number.
  *
  *  Set the flag if the function's entry is traced, which will cause the
  *  function's exit to also be traced.  utraceFnNumber is uncoditionally 
  *  set at entry, whether or not the entry is traced, so that it will
  *  always be available for error trace output.
  *  @internal
  */            
#وصف_ڪريو UTRACE_TRACED_داخلا 0x80000000

/**
 * Trace statement for the entry point of a function.
 * Stores the function number in a local variable.
 * In C code, must be placed immediately after the last variable declaration.
 * Must be matched with UTRACE_EXIT() at all function exit points.
 *
 * Tracing should start with UTRACE_ENTRY after checking for
 * U_FAILURE at function entry, so that if a function returns immediately
 * because of a pre-existing error condition, it does not show up in the trace,
 * consistent with ICU's error handling model.
 *
 * @param fnNumber The UTraceFunctionNumber for the current function.
 * @internal
 */
#وصف_ڪريو UTRACE_داخلا(fn_عدد) \
    int32_t utrace_Fn_عدد=(fn_عدد); \
UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(utrace_getLevel()>=UTRACE_INFO) { \
        utrace_داخلا(fn_عدد); \
        utrace_Fn_عدد |= UTRACE_TRACED_داخلا; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي


/**
 * Trace statement for the entry point of open and close functions.
 * Produces trace output at a less verbose setting than plain UTRACE_ENTRY
 * Stores the function number in a local variable.
 * In C code, must be placed immediately after the last variable declaration.
 * Must be matched with UTRACE_EXIT() at all function exit points.
 *
 * @param fnNumber The UTraceFunctionNumber for the current function.
 * @internal
 */
#وصف_ڪريو UTRACE_داخلا_OC(fn_عدد) \
    int32_t utrace_Fn_عدد=(fn_عدد); \
UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(utrace_getLevel()>=UTRACE_OPEN_CLOSE) { \
        utrace_داخلا(fn_عدد); \
        utrace_Fn_عدد |= UTRACE_TRACED_داخلا; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement for each exit point of a function that has a UTRACE_ENTRY()
 * statement.
 *
 * @param errorCode The function's ICU UErrorCode value at function exit,
 *                  or U_ZERO_ERROR if the function does not use a UErrorCode.
 *                  0==U_ZERO_ERROR indicates success,
 *                  positive values an error (see u_errorName()),
 *                  negative values an informational status.
 *
 * @internal
 */
#وصف_ڪريو UTRACE_EXIT() UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(utrace_Fn_عدد & UTRACE_TRACED_داخلا) { \
        utrace_exit(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, UTRACE_EXITV_NONE); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement for each exit point of a function that has a UTRACE_ENTRY()
 * statement, and that returns a value.
 *
 * @param val       The function's return value, int32_t or compatible type.
 *
 * @internal 
 */
#وصف_ڪريو UTRACE_EXIT_قدر(val) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(utrace_Fn_عدد & UTRACE_TRACED_داخلا) { \
        utrace_exit(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, UTRACE_EXITV_I32, val); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

#وصف_ڪريو UTRACE_EXIT_حالت(حالت) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(utrace_Fn_عدد & UTRACE_TRACED_داخلا) { \
        utrace_exit(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, UTRACE_EXITV_حالت, حالت); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

#وصف_ڪريو UTRACE_EXIT_قدر_حالت(val, حالت) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(utrace_Fn_عدد & UTRACE_TRACED_داخلا) { \
        utrace_exit(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (UTRACE_EXITV_I32 | UTRACE_EXITV_حالت), val, حالت); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

#وصف_ڪريو UTRACE_EXIT_PTR_حالت(ptr, حالت) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(utrace_Fn_عدد & UTRACE_TRACED_داخلا) { \
        utrace_exit(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (UTRACE_EXITV_PTR | UTRACE_EXITV_حالت), ptr, حالت); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes no data arguments.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA0(level, fmt) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (level), (fmt)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes one data argument.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA1(level, fmt, a) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا , (level), (fmt), (a)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes two data arguments.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA2(level, fmt, a, b) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا , (level), (fmt), (a), (b)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes three data arguments.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA3(level, fmt, a, b, c) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (level), (fmt), (a), (b), (c)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes four data arguments.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA4(level, fmt, a, b, c, d) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (level), (fmt), (a), (b), (c), (d)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes five data arguments.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA5(level, fmt, a, b, c, d, e) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (level), (fmt), (a), (b), (c), (d), (e)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes six data arguments.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA6(level, fmt, a, b, c, d, e, f) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (level), (fmt), (a), (b), (c), (d), (e), (f)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes seven data arguments.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA7(level, fmt, a, b, c, d, e, f, g) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (level), (fmt), (a), (b), (c), (d), (e), (f), (g)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes eight data arguments.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA8(level, fmt, a, b, c, d, e, f, g, h) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (level), (fmt), (a), (b), (c), (d), (e), (f), (g), (h)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Trace statement used inside functions that have a UTRACE_ENTRY() statement.
 * Takes nine data arguments.
 * The number of arguments for this macro must match the number of inserts
 * in the format string. Vector inserts count as two arguments.
 * Calls utrace_data() if the level is high enough.
 * @internal
 */
#وصف_ڪريو UTRACE_DATA9(level, fmt, a, b, c, d, e, f, g, h, i) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(UTRACE_LEVEL(level)) { \
        utrace_معلومات(utrace_Fn_عدد & ~UTRACE_TRACED_داخلا, (level), (fmt), (a), (b), (c), (d), (e), (f), (g), (h), (i)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

#وإلا

/*
 * When tracing is disabled, the following macros become empty
 */

#وصف_ڪريو UTRACE_LEVEL(level) 0
#وصف_ڪريو UTRACE_داخلا(fn_عدد)
#وصف_ڪريو UTRACE_داخلا_OC(fn_عدد)
#وصف_ڪريو UTRACE_EXIT()
#وصف_ڪريو UTRACE_EXIT_قدر(val)
#وصف_ڪريو UTRACE_EXIT_حالت(حالت)
#وصف_ڪريو UTRACE_EXIT_قدر_حالت(val, حالت)
#وصف_ڪريو UTRACE_EXIT_PTR_حالت(ptr, حالت)
#وصف_ڪريو UTRACE_DATA0(level, fmt)
#وصف_ڪريو UTRACE_DATA1(level, fmt, a)
#وصف_ڪريو UTRACE_DATA2(level, fmt, a, b)
#وصف_ڪريو UTRACE_DATA3(level, fmt, a, b, c)
#وصف_ڪريو UTRACE_DATA4(level, fmt, a, b, c, d)
#وصف_ڪريو UTRACE_DATA5(level, fmt, a, b, c, d, e)
#وصف_ڪريو UTRACE_DATA6(level, fmt, a, b, c, d, e, f)
#وصف_ڪريو UTRACE_DATA7(level, fmt, a, b, c, d, e, f, g)
#وصف_ڪريو UTRACE_DATA8(level, fmt, a, b, c, d, e, f, g, h)
#وصف_ڪريو UTRACE_DATA9(level, fmt, a, b, c, d, e, f, g, h, i)

#جيڪڏهن_پورو

#جيڪڏهن_پورو
