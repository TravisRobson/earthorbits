# earthorbits


## TODO

- [ ] update CMake locally
- [ ] run through some code and check against style guide sometime
- [ ] `-Werror` should be optionally used, perhaps during CI only as to not hinder the programmer.
- [ ] CMake formatting https://github.com/cheshirekow/cmake_format
- [ ] Get valgrind running in a docker container

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




## References
* https://google.github.io/styleguide/cppguide.html
* https://github.com/cpp-best-practices/gui_starter_template/blob/main/.clang-tidy 
* 