project('datalog', 'cpp')

snitch = dependency('snitch')
deps = [snitch]

doxygen = find_program('doxygen')
doxyfile = join_paths('@SOURCE_ROOT@','Doxyfile')
run_target('doxygen', command: [doxygen, doxyfile])

datalogpp = library(
  'datalogpp',
  'src/lexer.cpp',
  'src/parser.cpp',
  'src/ast.cpp',
  'src/parse_error.cpp',
  'src/interpreter.cpp',
  'src/term.cpp',
  'src/eterm.cpp',
  'src/atom.cpp',
  'src/eatom.cpp',
  'src/rule.cpp',
  'src/erule.cpp',
  'src/program.cpp')

datalogsh = executable('datalogsh', 'src/main.cpp', link_with: datalogpp)

subdir('tests')
