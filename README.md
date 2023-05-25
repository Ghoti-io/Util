# Ghoti.io `shared_string_view`
Ghoti.io (*pronounced [fish](https://en.wikipedia.org/wiki/Ghoti)*) `shared_string_view` is a dynamically-linked, C++ library.

This library extends the ideas of the [`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view) introduced in C++17.  The drawback to the `std::string_view` is that it requires the programmer to ensure that the string remains allocated while the `string_view` itself is in use.  For situations in which strings are passed around long-lived objects, this limitation makes the `std::string_view` useless and its performance advantages tantalizingly out-of-reach.

The features that make this library worth using are:
 - **Memory safety via `std::shared_ptr<>`.**  As long as there is a `Ghoti::shared_string_view` referencing the string pointer, then the string is guaranteed to be present.
 - **Thread safety.** The `Ghoti::shared_string_pointer` object is thread safe.  That is, it is write-only, and the reference counts used by the internal `std::shared_ptr<std::string>` are themselves thread safe.
 - **Minimal but useful methods and operators.**  Immediately useful access methods are supported such as the `[]` operator and forward and reverse iterators (allowing for use in ranged-for expressions).  For more versatile usage, any `shared_string_view` object can be cast to a `std::string_view`, which makes many additional methods available through the STL-provided methods.

## Example Code

Assume the file is called `main.cpp`.  Also assume that the library has been compiled and installed per the instructions at the end of this README.md.

```C++
#include <ghoti.io/shared_string_view.hpp>
#include <string>

using namespace std;

Ghoti::shared_string_view get_ssv() {
  // Create a string in this scope.
  string s{"abc 123"};

  // By returning from the function, the original string will definitely no
  // longer be in scope.
  return Ghoti::shared_string_view{s};
}

/**
 */
int main() {
  // Get a Ghoti::shared_string_view whose source string is now out of scope.
  auto ssv get_ssv();

  // Iterate through a portion of the string.
  for (auto ch : ssv.substr(2,3)) {
    cout << ch << endl;
  }

  return 0;
}
```

### Compile and run the example

```
g++ `pkg-config --libs --cflags ghoti.io-shared_string_view` main.cpp -o main

./main
```

### Sample Output
```
c
 
1
```

## Compiling the code

### Download from Github
```
git clone https://github.com/Ghoti-io/shared_string_view.git
```

### Compile and install the library (Ubuntu 22.04+)
```
make
sudo make install
```

## Make and run the tests
```
make test
```

During development, it is helpful to recompile and run tests every time a file is saved.  This can be done via:
```
make test-watch
```

## Compiling documentation
Documentation can be created using Doxygen.
```
make docs
```
For PDF documentation:
```
make docs-pdf
```

### Viewing the documentation
If using the WSL2 (on Windows), the documentation can be viewed using the following command:
```
wslview docs/html/index.html
```
or
```
wslview docs/latex/refman.pdf
```

