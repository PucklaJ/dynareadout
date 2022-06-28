set_project("dynareadout")
add_rules("mode.debug", "mode.release")

add_requires("doctest")

target("binout")
    set_kind("static")
    set_languages("ansi")
    add_files("src/*.c")
    add_headerfiles("src/*.h")

target("binout_test")
    set_kind("binary")
    set_languages("cxx11")
    add_deps("binout")
    add_packages("doctest")
    add_includedirs("src")
    add_files("test/binout_test.cpp")
