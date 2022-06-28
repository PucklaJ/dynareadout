set_project("dynareadout")

option("build_test")
    set_default(false)
    set_showmenu(true)
option_end()

add_rules("mode.debug", "mode.release")
target("binout")
    set_kind("static")
    set_languages("ansi")
    add_files("src/*.c")
    add_headerfiles("src/*.h")
target_end()

if get_config("build_test") then
    add_requires("doctest")
    target("binout_test")
        set_kind("binary")
        set_languages("cxx11")
        add_deps("binout")
        add_packages("doctest")
        add_includedirs("src")
        add_files("test/binout_test.cpp")
end
