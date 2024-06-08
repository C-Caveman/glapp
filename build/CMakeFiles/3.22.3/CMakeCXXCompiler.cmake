set(CMAKE_CXX_COMPILER "/nix/store/aq67a6bzcjyh6rcjj78miirqjl9rav72-gcc-wrapper-11.3.0/bin/g++")
set(CMAKE_CXX_COMPILER_ARG1 "")
set(CMAKE_CXX_COMPILER_ID "GNU")
set(CMAKE_CXX_COMPILER_VERSION "11.3.0")
set(CMAKE_CXX_COMPILER_VERSION_INTERNAL "")
set(CMAKE_CXX_COMPILER_WRAPPER "")
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "17")
set(CMAKE_CXX_EXTENSIONS_COMPUTED_DEFAULT "ON")
set(CMAKE_CXX_COMPILE_FEATURES "cxx_std_98;cxx_template_template_parameters;cxx_std_11;cxx_alias_templates;cxx_alignas;cxx_alignof;cxx_attributes;cxx_auto_type;cxx_constexpr;cxx_decltype;cxx_decltype_incomplete_return_types;cxx_default_function_template_args;cxx_defaulted_functions;cxx_defaulted_move_initializers;cxx_delegating_constructors;cxx_deleted_functions;cxx_enum_forward_declarations;cxx_explicit_conversions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_final;cxx_func_identifier;cxx_generalized_initializers;cxx_inheriting_constructors;cxx_inline_namespaces;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_noexcept;cxx_nonstatic_member_init;cxx_nullptr;cxx_override;cxx_range_for;cxx_raw_string_literals;cxx_reference_qualified_functions;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_thread_local;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_unrestricted_unions;cxx_user_literals;cxx_variadic_macros;cxx_variadic_templates;cxx_std_14;cxx_aggregate_default_initializers;cxx_attribute_deprecated;cxx_binary_literals;cxx_contextual_conversions;cxx_decltype_auto;cxx_digit_separators;cxx_generic_lambdas;cxx_lambda_init_captures;cxx_relaxed_constexpr;cxx_return_type_deduction;cxx_variable_templates;cxx_std_17;cxx_std_20;cxx_std_23")
set(CMAKE_CXX98_COMPILE_FEATURES "cxx_std_98;cxx_template_template_parameters")
set(CMAKE_CXX11_COMPILE_FEATURES "cxx_std_11;cxx_alias_templates;cxx_alignas;cxx_alignof;cxx_attributes;cxx_auto_type;cxx_constexpr;cxx_decltype;cxx_decltype_incomplete_return_types;cxx_default_function_template_args;cxx_defaulted_functions;cxx_defaulted_move_initializers;cxx_delegating_constructors;cxx_deleted_functions;cxx_enum_forward_declarations;cxx_explicit_conversions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_final;cxx_func_identifier;cxx_generalized_initializers;cxx_inheriting_constructors;cxx_inline_namespaces;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_noexcept;cxx_nonstatic_member_init;cxx_nullptr;cxx_override;cxx_range_for;cxx_raw_string_literals;cxx_reference_qualified_functions;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_thread_local;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_unrestricted_unions;cxx_user_literals;cxx_variadic_macros;cxx_variadic_templates")
set(CMAKE_CXX14_COMPILE_FEATURES "cxx_std_14;cxx_aggregate_default_initializers;cxx_attribute_deprecated;cxx_binary_literals;cxx_contextual_conversions;cxx_decltype_auto;cxx_digit_separators;cxx_generic_lambdas;cxx_lambda_init_captures;cxx_relaxed_constexpr;cxx_return_type_deduction;cxx_variable_templates")
set(CMAKE_CXX17_COMPILE_FEATURES "cxx_std_17")
set(CMAKE_CXX20_COMPILE_FEATURES "cxx_std_20")
set(CMAKE_CXX23_COMPILE_FEATURES "cxx_std_23")

set(CMAKE_CXX_PLATFORM_ID "Linux")
set(CMAKE_CXX_SIMULATE_ID "")
set(CMAKE_CXX_COMPILER_FRONTEND_VARIANT "")
set(CMAKE_CXX_SIMULATE_VERSION "")




set(CMAKE_AR "/nix/store/9izhv7bayzj8sr7m5n7c4qw1qk2fhq9s-binutils-2.38/bin/ar")
set(CMAKE_CXX_COMPILER_AR "/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/bin/gcc-ar")
set(CMAKE_RANLIB "/nix/store/9izhv7bayzj8sr7m5n7c4qw1qk2fhq9s-binutils-2.38/bin/ranlib")
set(CMAKE_CXX_COMPILER_RANLIB "/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/bin/gcc-ranlib")
set(CMAKE_LINKER "/nix/store/aq67a6bzcjyh6rcjj78miirqjl9rav72-gcc-wrapper-11.3.0/bin/ld")
set(CMAKE_MT "")
set(CMAKE_COMPILER_IS_GNUCXX 1)
set(CMAKE_CXX_COMPILER_LOADED 1)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_CXX_ABI_COMPILED TRUE)

set(CMAKE_CXX_COMPILER_ENV_VAR "CXX")

set(CMAKE_CXX_COMPILER_ID_RUN 1)
set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS C;M;c++;cc;cpp;cxx;m;mm;mpp;CPP;ixx;cppm)
set(CMAKE_CXX_IGNORE_EXTENSIONS inl;h;hpp;HPP;H;o;O;obj;OBJ;def;DEF;rc;RC)

foreach (lang C OBJC OBJCXX)
  if (CMAKE_${lang}_COMPILER_ID_RUN)
    foreach(extension IN LISTS CMAKE_${lang}_SOURCE_FILE_EXTENSIONS)
      list(REMOVE_ITEM CMAKE_CXX_SOURCE_FILE_EXTENSIONS ${extension})
    endforeach()
  endif()
endforeach()

set(CMAKE_CXX_LINKER_PREFERENCE 30)
set(CMAKE_CXX_LINKER_PREFERENCE_PROPAGATES 1)

# Save compiler ABI information.
set(CMAKE_CXX_SIZEOF_DATA_PTR "8")
set(CMAKE_CXX_COMPILER_ABI "ELF")
set(CMAKE_CXX_BYTE_ORDER "LITTLE_ENDIAN")
set(CMAKE_CXX_LIBRARY_ARCHITECTURE "")

if(CMAKE_CXX_SIZEOF_DATA_PTR)
  set(CMAKE_SIZEOF_VOID_P "${CMAKE_CXX_SIZEOF_DATA_PTR}")
endif()

if(CMAKE_CXX_COMPILER_ABI)
  set(CMAKE_INTERNAL_PLATFORM_ABI "${CMAKE_CXX_COMPILER_ABI}")
endif()

if(CMAKE_CXX_LIBRARY_ARCHITECTURE)
  set(CMAKE_LIBRARY_ARCHITECTURE "")
endif()

set(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX "")
if(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX)
  set(CMAKE_CL_SHOWINCLUDES_PREFIX "${CMAKE_CXX_CL_SHOWINCLUDES_PREFIX}")
endif()





set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "/nix/store/8q06kxhkl4dq3vjd3ai2j8n9kr26c271-gnumake-4.3/include;/nix/store/w18ldwr1k7khrr3zxmffx796r17nik57-glfw-3.3.7/include;/nix/store/ahham50n8mzj4h0bb8a87y6hlhydl58g-libGL-1.4.0-dev/include;/nix/store/7n338lngr6m7n8903d34wrb21ppda6in-simgrid-3.31/include;/nix/store/rcdw0mgn2wjcrnpjxzqr9avfvdigsph8-boost-1.79.0-dev/include;/nix/store/58ai66l83g3j2hzv3h1b60i6dv8bby7c-gdb-12.1/include;/nix/store/k69rmm203rpkrc0q6sxjxn0vln2m5h7x-valgrind-3.19.0-dev/include;/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/include/c++/11.3.0;/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/include/c++/11.3.0/x86_64-unknown-linux-gnu;/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/include/c++/11.3.0/backward;/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/lib/gcc/x86_64-unknown-linux-gnu/11.3.0/include;/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/include;/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/lib/gcc/x86_64-unknown-linux-gnu/11.3.0/include-fixed;/nix/store/1dlzfarjz8g5hx8zhcvsglbp8bqing4r-glibc-2.34-210-dev/include")
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "stdc++;m;gcc_s;gcc;c;gcc_s;gcc")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "/nix/store/w18ldwr1k7khrr3zxmffx796r17nik57-glfw-3.3.7/lib;/nix/store/8pd5vps1mbfp6jrz0vaz88wgj4g00fvv-libGL-1.4.0/lib;/nix/store/vxrnnxvaxj96g9gmlyrvkw7jx5vb9rsz-libglvnd-1.4.0/lib;/nix/store/7n338lngr6m7n8903d34wrb21ppda6in-simgrid-3.31/lib;/nix/store/4djr55cmnfahbg6i6wr8p3ga66gs09gi-boost-1.79.0/lib;/nix/store/58ai66l83g3j2hzv3h1b60i6dv8bby7c-gdb-12.1/lib;/nix/store/d2bpliayddadf6lx6l1i04w265gqw8n6-glibc-2.34-210/lib;/nix/store/q9gfnkp0zaafccpgj4mqngmp9f9y93xh-gcc-11.3.0-lib/lib;/nix/store/aq67a6bzcjyh6rcjj78miirqjl9rav72-gcc-wrapper-11.3.0/bin;/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/lib/gcc/x86_64-unknown-linux-gnu/11.3.0;/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/lib64;/nix/store/jf217yacqd074ypqp2rxs66i29nxi4rn-gcc-11.3.0/lib")
set(CMAKE_CXX_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")
