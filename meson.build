project('datalog', 'cpp')
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

test_files = files('kb.pl')
test('kb1', datalogsh, args: test_files)
