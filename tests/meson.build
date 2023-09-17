includes = include_directories('../src')

datalog_test = executable(
  'datalog_test',
  'test.cpp',
  include_directories: includes, link_with:datalogpp, dependencies: snitch)

# Unit test
ut0 = executable(
  'ut0',
  'ut0.cpp',
  include_directories: includes, link_with: datalogpp, dependencies: snitch,
  override_options: ['cpp_std=c++20'])
test('ut0', ut0)

# Tests
test0 = files('kb0.pl', 'query0.pl')
test1 = files('kb1.pl', 'query1.pl')

test('find_fact', datalog_test, args: test0)
test('all_facts', datalog_test, args: test1)
