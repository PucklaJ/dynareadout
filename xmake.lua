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

option("profiling")
    set_default(false)
    set_showmenu(true)
    add_defines("PROFILING")

option("build_gen")
    set_default(false)
    set_showmenu(true)
option_end()

add_rules("mode.debug", "mode.release")
if get_config("build_gen") then
    target("generator")
        set_kind("binary")
        set_languages("ansi")
        if is_plat("linux") then
            add_cflags("-fPIC")
        end
        add_files("gen/*.c")
        add_headerfiles("gen/*.h")

        after_build(function (target)
            os.run(target:targetfile())
        end)
end

target("dynareadout")
    set_kind("$(kind)")
    set_languages("ansi")
    if is_plat("linux") then
        add_cflags("-fPIC")
    end
    add_options("profiling")
    add_files("src/*.c")
    if not get_config("profiling") then
        remove_files("src/profiling.c")
    end
    add_headerfiles("src/*.h")
    if is_kind("shared") then
        add_rules("utils.symbols.export_all")
    end
target_end()

if get_config("build_cpp") or get_config("build_python") then
    target("dynareadout_cpp")
        set_kind("$(kind)")
        set_languages("cxx17")
        if is_plat("linux") then
            add_cxxflags("-fPIC")
            add_syslinks("stdc++fs")
        end
        add_deps("dynareadout")
        add_includedirs("src")
        add_files("src/cpp/*.cpp")
        add_headerfiles("src/cpp/*.hpp")
        if is_kind("shared") then
            add_rules("utils.symbols.export_all", {export_classes = true})
        end
    target_end()
end

if get_config("build_test") then
    add_requires("doctest 2.4.8")
    target("dynareadout_test")
        set_kind("binary")
        set_languages("cxx17")
        add_deps("dynareadout")
        if is_plat("linux") then
            add_cxxflags("-fPIC")
        end
        if get_config("build_cpp") then
            add_deps("dynareadout_cpp")
            add_defines("BUILD_CPP")
            add_includedirs("src/cpp")
        end
        add_packages("doctest")
        add_includedirs("src")
        add_options("profiling")
        add_files("test/*.cpp")
    target_end()
end

if get_config("build_python") then
    add_requires("python >=3.6", "pybind11")
    target("pybind11_module")
        set_kind("shared")
        set_languages("cxx17")
        if is_plat("linux") then
            add_cxxflags("-fPIC")
        end
        add_deps("dynareadout_cpp")
        add_packages("pybind11")
        add_options("profiling")
        add_files("src/python/*.cpp")
        add_headerfiles("src/python/*.hpp")
        add_includedirs("src", "src/cpp")
        add_rpathdirs("@executable_path")

        on_load(function (target)
            import("lib.detect.find_program")
            local ext_name
            local python_config = find_program("python3-config", { check = "--extension-suffix"})
            if python_config then
                local ext_file = os.tmpfile()
                os.execv(python_config, {"--extension-suffix"}, {stdout=ext_file})
                ext_name = io.readfile(ext_file)
                ext_name = ext_name:gsub("%s+", "")
            else
                import("lib.detect.find_programver")
                local version
                local python_version = find_programver("python")
                if python_version then
                    local majorminor = python_version:split(".", {plain = true})
                    version = majorminor[1] .. majorminor[2]
                else
                    version = "310"
                end
                local platform = target:plat()
                if platform == "linux" then
                    platform = platform .. "-gnu"
                end
                local suffix = ".so"
                if target:is_plat("windows", "mingw") then
                    suffix = ".dll"
                end
                ext_name = ".cpython-" .. version .. "-" .. target:arch() .. "-" .. platform .. suffix
            end
            target:set("filename", "dynareadout_c" .. (is_mode("debug") and "_d" or "") .. ext_name)
        end)
end
