# earthorbits


## TODO

- [ ] update CMake locally
- [ ] run through some code and check against style guide sometime
- [ ] `-Werror` should be optionally used, perhaps during CI only as to not hinder the programmer.
- [ ] CMake formatting https://github.com/cheshirekow/cmake_format
- [ ] Get valgrind running in a docker container
- [ ] test installation command with CMake
- [ ] Add pre-commit autoupdate check somehow
- [ ] Create docker images
- [ ] Compile for AppleClang, Clang, gcc 
- [ ] Setup Github pages for this repo
- [ ] Warnings to errors in CI
- [ ] Optionally disable building and installing tests
- [ ] Create a very lightweight docker image
- [ ] Verify clang sanitizers are actually doing something

## Maybe TODO
- [ ] https://github.com/detailyang/pre-commit-shell (if we get more shell scripts)

## Style

* file names all lowercase, no "-" or "_"
* type names are camel case e.g. `UrlShortener`
* variable names all lowercase, snake case (e.g. `std::string table_name;`)'
* class, not `struct` member variables are like variable names but with trailing underscore (e.g. `std::string table_name_`)
* struct members are just like variable names
* constants prefixed with k, e.g. `const int kDaysInAWeek = 7;`
* Functions as camel cased.
* accessors simply use variable name (`int count()` and `void set_count(int c)`)
* enumerator values are done like constants


## useful commands

* `pre-commit run --all-files`
* `otool -L build/tests/earthorbittests` 

## References
* https://google.github.io/styleguide/cppguide.html
* https://github.com/cpp-best-practices/gui_starter_template/blob/main/.clang-tidy 
* https://futuretechforge.com/clang-format-tutorial/
* https://developers.redhat.com/articles/2022/02/25/enforce-code-consistency-clang-format#conclusion
* https://pre-commit.com
* OSs supported by github: https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners/about-github-hosted-runners#supported-runners-and-hardware-resources
* https://clang-analyzer.llvm.org/available_checks.html#core_checkers
* https://opensource.apple.com/source/clang/clang-23/clang/tools/clang/www/StaticAnalysisUsage.html 
* https://github.com/Ericsson/CodeChecker
* https://kristerw.blogspot.com/2018/03/detecting-incorrect-c-stl-usage.html
