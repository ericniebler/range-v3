# Copyright Louis Dionne 2015
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# Setup compiler flags (more can be set on a per-target basis or in
# subdirectories)

# Compilation flags
include(CheckCXXCompilerFlag)
macro(ranges_append_flag testname flag ${ARGN})
    check_cxx_compiler_flag("${flag} ${ARGN}" ${testname})
    if (${testname})
        add_compile_options(${flag} ${ARGN})
    endif()
endmacro()

# All compilation flags
# Language flag: version of the C++ standard to use
message("[range-v3]: C++ std=${RANGES_CXX_STD}")
ranges_append_flag(RANGES_HAS_CXXSTD "-std=c++${RANGES_CXX_STD}")

# Enable all warnings and make them errors:
ranges_append_flag(RANGES_HAS_WERROR -Werror)
ranges_append_flag(RANGES_HAS_WX -WX)
ranges_append_flag(RANGES_HAS_WALL -Wall)
ranges_append_flag(RANGES_HAS_WALL -Wextra)
ranges_append_flag(RANGES_HAS_WEVERYTHING -Weverything)
ranges_append_flag(RANGES_HAS_PEDANTIC -pedantic)
ranges_append_flag(RANGES_HAS_PEDANTIC_ERRORS -pedantic-errors)

# Selectively disable those warnings that are not worth it:
ranges_append_flag(RANGES_HAS_WNO_CXX98_COMPAT -Wno-c++98-compat)
ranges_append_flag(RANGES_HAS_WNO_CXX98_COMPAT_PEDANTIC -Wno-c++98-compat-pedantic)
ranges_append_flag(RANGES_HAS_WNO_WEAK_VTABLES -Wno-weak-vtables)
ranges_append_flag(RANGES_HAS_WNO_PADDED -Wno-padded)
ranges_append_flag(RANGES_HAS_WNO_MISSING_PROTOTYPES -Wno-missing-prototypes)
ranges_append_flag(RANGES_HAS_WNO_MISSING_VARIABLE_DECLARATIONS -Wno-missing-variable-declarations)
ranges_append_flag(RANGES_HAS_WNO_DOCUMENTATION -Wno-documentation)
ranges_append_flag(RANGES_HAS_WNO_DOCUMENTATION_UNKNOWN_COMMAND -Wno-documentation-unknown-command)
ranges_append_flag(RANGES_HAS_WNO_OLD_STYLE_CAST -Wno-old-style-cast)
ranges_append_flag(RANGES_HAS_WNO_SHADOW -Wno-shadow)

if (RANGES_ENV_MACOSX)
  ranges_append_flag(RANGES_HAS_WNO_GLOBAL_CONSTRUCTORS -Wno-global-constructors)
  ranges_append_flag(RANGES_HAS_WNO_EXIT_TIME_DESTRUCTORS -Wno-exit-time-destructors)
endif()

if (RANGES_CXX_COMPILER_GCC)
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6.0")
    ranges_append_flag(RANGES_HAS_WNO_STRICT_OVERFLOW -Wno-strict-overflow)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0")
      ranges_append_flag(RANGES_HAS_WNO_MISSING_FIELD_INITIALIZERS -Wno-missing-field-initializers)
    endif()
  elseif ((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0") OR (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "7.0"))
    ranges_append_flag(RANGES_HAS_WNO_NOEXCEPT_TYPE -Wno-noexcept-type)
  endif()
endif()

if (RANGES_ENV_LINUX AND RANGES_CXX_COMPILER_CLANG)
  # On linux libc++ re-exports the system math headers. The ones from libstdc++
  # use the GCC __extern_always_inline intrinsic which is not supported by clang
  # versions 3.6, 3.7, 3.8, 3.9, 4.0, and current trunk 5.0 (as of 2017.04.13).
  #
  # This works around it by replacing __extern_always_inline with inline using a
  # macro:
  ranges_append_flag(RANGES_HAS_D__EXTERN_ALWAYS_INLINE -D__extern_always_inline=inline)
endif()

# Template diagnostic flags
ranges_append_flag(RANGES_HAS_FDIAGNOSTIC_SHOW_TEMPLATE_TREE -fdiagnostics-show-template-tree)
ranges_append_flag(RANGES_HAS_FTEMPLATE_BACKTRACE_LIMIT "-ftemplate-backtrace-limit=0")

# Clang modules support
if (RANGES_MODULES)
  ranges_append_flag(RANGES_HAS_MODULES -fmodules)
  ranges_append_flag(RANGES_HAS_MODULE_MAP_FILE "-fmodule-map-file=${PROJECT_SOURCE_DIR}/include/module.modulemap")
  ranges_append_flag(RANGES_HAS_MODULE_CACHE_PATH "-fmodules-cache-path=${PROJECT_BINARY_DIR}/module.cache")
  if (RANGES_LIBCXX_MODULE)
    ranges_append_flag(RANGES_HAS_LIBCXX_MODULE_MAP_FILE "-fmodule-map-file=${RANGES_LIBCXX_MODULE}")
  endif()
  if (RANGES_ENV_MACOSX)
    ranges_append_flag(RANGES_HAS_NO_IMPLICIT_MODULE_MAPS -fno-implicit-module-maps)
  endif()
  if (RANGES_DEBUG_BUILD)
    ranges_append_flag(RANGES_HAS_GMODULES -gmodules)
  endif()
endif()

# Sanitizer support: detect incompatible sanitizer combinations
if (RANGES_ASAN AND RANGES_MSAN)
  message(FATAL_ERROR "[range-v3 error]: AddressSanitizer and MemorySanitizer are both enabled at the same time!")
endif()

if (RANGES_MSAN AND RANGES_ENV_MACOSX)
  message(FATAL_ERROR "[range-v3 error]: MemorySanitizer is not supported on MacOSX!")
endif()

# AddressSanitizer support
if (RANGES_ASAN)
  # This policy enables passing the linker flags to the linker when trying to
  # test the features, which is required to successfully link ASan binaries
  cmake_policy(SET CMP0056 NEW)
  set (ASAN_FLAGS "")
  if (RANGES_ENV_MACOSX) # LeakSanitizer not supported on MacOSX
    set (ASAN_FLAGS "-fsanitize=address,integer,undefined,nullability")
  else()
    if (RANGES_CXX_COMPILER_CLANG AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.0")
      set (ASAN_FLAGS "-fsanitize=address")
    else()
      set (ASAN_FLAGS "-fsanitize=address,integer,undefined,leak,nullability")
    endif()
  endif()
  ranges_append_flag(RANGES_HAS_ASAN "${ASAN_FLAGS}")
  if (RANGES_HAS_ASAN) #ASAN flags must be passed to the linker:
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${ASAN_FLAGS}")
  endif()
  ranges_append_flag(RANGES_HAS_SANITIZE_NO_RECOVER "-fno-sanitize-recover=all")
  ranges_append_flag(RANGES_HAS_NO_OMIT_FRAME_POINTER -fno-omit-frame-pointer)
endif()

# MemorySanitizer support
if (RANGES_MSAN)
  # This policy enables passing the linker flags to the linker when trying to
  # compile the examples, which is required to successfully link MSan binaries
  cmake_policy(SET CMP0056 NEW)
  ranges_append_flag(RANGES_HAS_MSAN "-fsanitize=memory")
  ranges_append_flag(RANGES_HAS_MSAN_TRACK_ORIGINS -fsanitize-memory-track-origins)
  ranges_append_flag(RANGES_HAS_SANITIZE_RECOVER_ALL "-fno-sanitize-recover=all")
  ranges_append_flag(RANGES_HAS_NO_OMIT_FRAME_POINTER -fno-omit-frame-pointer)
endif()

# Build types:
if (RANGES_DEBUG_BUILD AND RANGES_RELEASE_BUILD)
  message(FATAL_ERROR "[range-v3 error] Cannot simultaneously generate debug and release builds!")
endif()

if (RANGES_DEBUG_BUILD)
  ranges_append_flag(RANGES_HAS_O0 -O0)
  ranges_append_flag(RANGES_HAS_NO_INLINE -fno-inline)
  ranges_append_flag(RANGES_HAS_STACK_PROTECTOR_ALL -fstack-protector-all)
  ranges_append_flag(RANGES_HAS_G3 -g3)
  # Clang can generate debug info tuned for LLDB or GDB
  if (RANGES_CXX_COMPILER_CLANG)
    if (RANGES_ENV_MACOSX)
      ranges_append_flag(RANGES_HAS_GLLDB -glldb)
    elseif(RANGES_ENV_LINUX)
      ranges_append_flag(RANGES_HAS_GGDB -ggdb)
    endif()
  endif()
endif()

if (RANGES_RELEASE_BUILD)
  if (NOT RANGES_ASSERTIONS)
    ranges_append_flag(RANGES_HAS_DNDEBUG -DNDEBUG)
  endif()
  if (NOT RANGES_ASAN AND NOT RANGES_MSAN)
    # The quality of ASan and MSan error messages suffers if we disable the
    # frame pointer, so leave it enabled when compiling with either of them:
    ranges_append_flag(RANGES_HAS_OMIT_FRAME_POINTER -fomit-frame-pointer)
  endif()

  ranges_append_flag(RANGES_HAS_OFAST -Ofast)
  if (NOT RANGES_HAS_OFAST)
    ranges_append_flag(RANGES_HAS_O2 -O2)
  endif()
  ranges_append_flag(RANGES_HAS_STRICT_ALIASING -fstrict-aliasing)
  if (NOT RANGES_CXX_COMPILER_CLANGC2)
    ranges_append_flag(RANGES_HAS_STRICT_VTABLE_POINTERS -fstrict-vtable-pointers)
  endif()
  ranges_append_flag(RANGES_HAS_FAST_MATH -ffast-math)
  ranges_append_flag(RANGES_HAS_VECTORIZE -fvectorize)

  if (NOT RANGES_ENV_MACOSX)
    # Sized deallocation is not available in MacOSX:
    ranges_append_flag(RANGES_HAS_SIZED_DEALLOCATION -fsized-deallocation)
  endif()

  if (RANGES_LLVM_POLLY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -polly -mllvm -polly-vectorizer=stripmine")
  endif()

  if (RANGES_CXX_COMPILER_CLANG AND (NOT (RANGES_INLINE_THRESHOLD EQUAL -1)))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -inline-threshold=${RANGES_INLINE_THRESHOLD}")
  endif()
endif()

if (RANGES_NATIVE)
  ranges_append_flag(RANGES_HAS_MARCH_NATIVE "-march=native")
  ranges_append_flag(RANGES_HAS_MTUNE_NATIVE "-mtune=native")
endif()

if (RANGES_VERBOSE_BUILD)
  message("[range-v3]: C++ flags: ${CMAKE_CXX_FLAGS}")
  message("[range-v3]: C++ debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
  message("[range-v3]: C++ Release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
  message("[range-v3]: C++ Compile Flags: ${CMAKE_CXX_COMPILE_FLAGS}")
  message("[range-v3]: Compile options: ${COMPILE_OPTIONS_}")
  message("[range-v3]: C Flags: ${CMAKE_C_FLAGS}")
  message("[range-v3]: C Compile Flags: ${CMAKE_C_COMPILE_FLAGS}")
  message("[range-v3]: EXE Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
  message("[range-v3]: C++ Linker flags: ${CMAKE_CXX_LINK_FLAGS}")
  message("[range-v3]: MODULE Linker flags: ${CMAKE_MODULE_LINKER_FLAGS}")
  get_directory_property(CMakeCompDirDefs COMPILE_DEFINITIONS)
  message("[range-v3]: Compile Definitions: ${CmakeCompDirDefs}")
endif()
