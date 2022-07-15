set_project("dynareadout")

option("build_test")
    set_default(false)
    set_showmenu(true)

option("build_cpp")
    set_default(true)
    set_showmenu(true)

option("build_python")
    set_default(false)
    set_showmenu(true)
option_end()

add_rules("mode.debug", "mode.release")
target("binout")
    set_kind("$(kind)")
    set_languages("ansi")
    if is_plat("linux") then
        add_cxxflags("-fPIC")
    end
    add_files("src/binout*.c", "src/path.c")
    add_headerfiles("src/binout*.h", "src/path.h")
    if is_kind("shared") then
        add_rules("utils.symbols.export_all")
    end

target("d3plot")
    set_kind("$(kind)")
    set_languages("ansi")
    if is_plat("linux") then
        add_cxxflags("-fPIC")
    end
    add_files("src/d3*.c")
    add_headerfiles("src/d3*.h")
    if is_kind("shared") then
        add_rules("utils.symbols.export_all")
    end
target_end()

if get_config("build_cpp") or get_config("build_python") then
    target("binout_cpp")
        set_kind("$(kind)")
        set_languages("cxx17")
        if is_plat("linux") then
            add_cxxflags("-fPIC")
        end
        add_deps("binout")
        add_includedirs("src")
        add_files("src/cpp/binout*.cpp")
        add_headerfiles("src/cpp/binout*.hpp", "src/cpp/array.hpp", "src/cpp/vec.hpp")
        if is_kind("shared") then
            add_rules("utils.symbols.export_all", {export_classes = true})
        end

    target("d3plot_cpp")
        set_kind("$(kind)")
        set_languages("cxx17")
        if is_plat("linux") then
            add_cxxflags("-fPIC")
        end
        add_deps("d3plot")
        add_includedirs("src")
        add_files("src/cpp/d3*.cpp")
        add_headerfiles("src/cpp/d3*.hpp", "src/cpp/array.hpp")
        if is_kind("shared") then
            add_rules("utils.symbols.export_all", {export_classes = true})
        end
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
            add_includedirs("src/cpp")
        end
        add_packages("doctest")
        add_includedirs("src")
        add_files("test/binout_test.cpp")

    target("d3plot_test")
        set_kind("binary")
        set_languages("cxx17")
        add_deps("d3plot")
        if get_config("build_cpp") then
            add_deps("d3plot_cpp")
            add_defines("D3PLOT_CPP")
            add_includedirs("src/cpp")
        end
        add_packages("doctest")
        add_includedirs("src")
        add_files("test/d3plot_test.cpp")
    target_end()
end

if get_config("build_python") then
    add_requires("python3", "pybind11")
    target("pybind11_module")
        set_kind("shared")
        set_languages("cxx17")
        if is_plat("linux") then
            add_cxxflags("-fPIC")
        end
        add_deps("d3plot_cpp", "binout_cpp")
        add_packages("pybind11")
        add_files("src/python/*.cpp")
        add_headerfiles("src/python/*.hpp")
        add_includedirs("src", "src/cpp")
        add_rpathdirs("@executable_path")

        on_load(function (target)
            os.execv("python3-config", {"--extension-suffix"}, {stdout="/tmp/python_config_name.txt"})
            ext_name = io.readfile("/tmp/python_config_name.txt")
            ext_name = ext_name:gsub("%s+", "")
            target:set("filename", "dynareadout" .. ext_name)
        end)
end
