set_project("dynareadout")
add_rules("mode.debug", "mode.release")

target("binout")
    set_kind("static")
    set_languages("ansi")
    add_files("src/binout.c")
    add_headerfiles("src/binout.h")
