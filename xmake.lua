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

option("thread_safe")
    set_default(true)
    set_showmenu(true)
option_end()

local use_boost_fs = is_plat("macosx") and (get_config("build_cpp") or get_config("build_python") or get_config("build_test"))

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
    if is_plat("linux", "macosx") then
        add_cflags("-Wno-format")
    end
    if is_plat("linux") then
        add_cflags("-fPIC")
    end
    add_options("profiling", "thread_safe")
    add_files("src/*.c")
    if not get_config("profiling") then
        remove_files("src/profiling.c")
    end
    if not get_config("thread_safe") then
        add_defines("NO_THREAD_SAFETY")
        remove_files("src/sync.c")
    elseif not is_plat("windows") then
        add_syslinks("pthread")
    end
    add_headerfiles("src/*.h")
    if is_kind("shared") then
        add_rules("utils.symbols.export_all")
    end
target_end()

if use_boost_fs then
    add_requires("boost", {configs = {filesystem = true}})
end

if get_config("build_cpp") or get_config("build_python") then
    target("dynareadout_cpp")
        set_kind("$(kind)")
        set_languages("cxx17")
        if is_plat("linux", "macosx") then
            add_cxxflags("-Wno-format")
        end
        if is_plat("linux") then
            add_cxxflags("-fPIC")
            add_syslinks("stdc++fs")
        end
        if use_boost_fs then
            add_packages("boost")
        end
        add_options("thread_safe")
        if not get_config("thread_safe") then
            add_defines("NO_THREAD_SAFETY")
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
        if is_plat("linux", "macosx") then
            add_cxxflags("-Wno-format")
        end
        if is_plat("linux") then
            add_cxxflags("-fPIC")
        end
        if use_boost_fs then
            add_packages("boost")
        end
        if get_config("build_cpp") then
            add_deps("dynareadout_cpp")
            add_defines("BUILD_CPP")
            add_includedirs("src/cpp")
        end
        add_packages("doctest")
        add_includedirs("src")
        add_options("profiling", "thread_safe")
        if not get_config("thread_safe") then
            add_defines("NO_THREAD_SAFETY")
        end
        add_files("test/*.cpp")
    target_end()
end

if get_config("build_python") then
    add_requires("python >=3.6", "pybind11")
    target("pybind11_module")
        set_kind("shared")
        set_languages("cxx17")
        if is_plat("linux", "macosx") then
            add_cxxflags("-Wno-format")
        end
        if is_plat("linux") then
            add_cxxflags("-fPIC")
        end
        if use_boost_fs then
            add_packages("boost")
        end
        add_deps("dynareadout_cpp")
        add_packages("pybind11")
        add_options("profiling", "thread_safe")
        if not get_config("thread_safe") then
            add_defines("NO_THREAD_SAFETY")
        end
        add_files("src/python/*.cpp")
        add_headerfiles("src/python/*.hpp")
        add_includedirs("src", "src/cpp")
        add_rpathdirs("@executable_path")

        on_load(function (target)
            import("lib.detect.find_program")
            local ext_name
            local python = find_program("python")
            if python then
                local ext_file = os.tmpfile()
                os.execv(python, {"src/python/abi-suffix.py"}, {stdout=ext_file})
                ext_name = io.readfile(ext_file)
                ext_name = ext_name:gsub("%s+", "")
            else
                ext_name = ".python-null." .. (is_plat("windows", "mingw") and "dll" or "so")
            end
            target:set("filename", "dynareadout" .. (is_mode("debug") and "_d" or "") .. ext_name)
        end)
end
