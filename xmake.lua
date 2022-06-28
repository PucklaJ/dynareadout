set_project("dynareadout")

option("build_test")
    set_default(false)
    set_showmenu(true)

option("build_cpp")
    set_default(true)
    set_showmenu(true)
option_end()

add_rules("mode.debug", "mode.release")
target("binout")
    set_kind("static")
    set_languages("ansi")
    add_files("src/*.c")
    add_headerfiles("src/*.h")
target_end()

if get_config("build_cpp") then
    target("binout_cpp")
        set_kind("static")
        set_languages("cxx17")
        add_deps("binout")
        add_includedirs("src")
        add_files("src/cpp/*.cpp")
        add_headerfiles("src/cpp/*.hpp")
    target_end()
end

if get_config("build_test") then
    add_requires("doctest")
    target("binout_test")
        set_kind("binary")
        set_languages("cxx17")
        add_deps("binout")
        if get_config("build_cpp") then
            add_deps("binout_cpp")
            add_defines("BINOUT_CPP")
        end
        add_packages("doctest")
        add_includedirs("src")
        add_files("test/binout_test.cpp")
    target_end()
end
