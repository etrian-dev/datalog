project('datalog', 'cpp')
datalogpp = library('datalogpp', 'src/lexer.cpp', 'src/parser.cpp')
datalogsh = executable('datalogsh', 'src/main.cpp', link_with: datalogpp)

test_files = files('kb.pl')
test('kb1', datalogsh, args: test_files)