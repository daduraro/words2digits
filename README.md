# words2digits

C++11 pet project to convert textual numbers from words to digits.

Fundamentally, the textual numbers are those recognized by the following context-free grammar:

```c
CardNum      -> 'zero' | Millions | AValue

Digit        -> 'one' | 'two' | 'three' | 'four' | 'five' |
                'six' | 'seven' | 'eight' | 'nine'
Teens        -> 'ten' | 'eleven' | 'twelve'  | 'thirteen' |
                'fourteen' | 'fifteen' | 'sixteen' |
                'seventeen' | 'eighteen' | 'nineteen'
SecDig       -> 'twenty' | 'thirty' | 'forty' | 'fifty' |
                'sixty' | 'seventy' | 'eighty' |
                'ninety'
Below100     -> Digit | Teens | SecDig | SecDig '-' Digit

HundredSfx   -> 'hundred' | 'hundred and ' Below100
Hundreds     -> Below100 | Digit ' ' HundredSfx

ThousandSfx  -> 'thousand' | 'thousand ' Hundreds
Thousands    -> Hundreds | Hundreds ' ' ThousandSfx

MillionSfx   -> 'million' | 'million ' Thousands
Millions     -> Thousands | Thousands ' ' MillionSfx

AValue       -> 'a ' HundredSfx |
                'a ' ThousandSfx | 'a hundred ' ThousandSfx |
                'a ' MillionSfx  | 'a hundred ' MillionSfx
```

Which recognizes words such as
```
zero
one
three hundred and forty-one
one thousand four hundred and eighty-seven
eight thousand six hundred and sixty-eight
three hundred and seventy-one thousand two hundred and seventeen
two million six hundred and sixty-two thousand five hundred and two
a hundred and one
a hundred and fifty-nine
a thousand three hundred and eighty-five
a million three hundred and ninety-two
```

Those are textual representation of cardinal numbers up to the millions. However, support for higher-order numbers (billions, trillions, ...) is trivial as they follow the same pattern as millions. Under `tools/` there is a Python script that generates such samples from the grammar using the `nltk` package.

## How to build

To build, use the typical CMake workflow

```sh
mkdir build && cd build
cmake ..
# ninja, make, ...
```

The following cache variables are added
```
VARIABLE        DEFAULT     EXPLANATION
W2D_BUILD_DOC   ON          whether to build the Doxygen docs, fails
                            gracefuly if Doxygen is not found
W2D_TESTS       OFF         whether to build the test files, requires GTest
                            submodule initialized
W2D_COVERAGE    OFF         whether to instrument unittest for coverage, only
                            affects when compiling with gcc (requires gcov)
```

For the tests, this project uses [GTest](https://github.com/google/googletest), which is present as a Git submodule. Just invoke
```sh
git submodule init
git submodule update extern/googletest
```

Note that depending on the `CMakeCache.txt` configuration, it may be needed that GTest is forced to be dynamically linked with the C/C++ runtimes. This can be controlled by the `gtest_force_shared_crt` cache variable, either by invoking
```sh
cmake .. -Dgtest_force_shared_crt=ON
```
or modifying the generated `CMakeCache.txt` file directly.

To execute, just invoke `words2digits` with either an input file or using the standard in. Under `samples/` there is the following example

```
Name a number: forty-two.
She thought —a hundred sheep after— I am not getting any sleep.
A thousand words are worth less than a picture, but even a
 hundred well-said words will get you far enough.
Zero times one million thirty-three thousand nine hundred and seven dot six is still zero.
The words of general 栗林 were clear, we were to attack on the fourth day with
a hundred thousand-man troop.
One hundred and two thousand may be ambiguous, is it a hundred thousand plus two thousand
or one hundred and then two thousand?
Left associative greediness produce some effect:
    "a hundred and two thousand forty-two" versus "a hundred, and two thousand forty-two".
a thousand eight hundred and eighty-three
a hundred and fifty-four
a hundred thousand five hundred and sixteen
a million   ninety thousand   seven hundred   and eighty-one
a hundred million four hundred and six thousand eight hundred and fifty-seven
A
hundred
and
two.
```

which the tool transforms it to

```
Name a number: 42.
She thought —100 sheep after— I am not getting any sleep.
1000 words are worth less than a picture, but even
100 well-said words will get you far enough.
0 times 1033907 dot 6 is still 0.
The words of general 栗林 were clear, we were to attack on the fourth day with
100000-man troop.
102000 may be ambiguous, is it 100000 plus 2000
or 100 and then 2000?
Left associative greediness produce some effect:
    "102 thousand 42" versus "100, and 2042".
1883
154
100516
1090781
100406857

102.
```

## Program architecture

The program is split into two main components, a tokenizer and a grammar parser. As the parsed grammar is an LL(k) grammar, a straight-forward recursive descent parser has been implemented. This means that, at a high level, this parser only requires that, given a specific token, which is the next token in the stream.

As some of the text must be replaced when processed, the tokenizer performs three basic roles:
- Splits the input text into tokens of different categories (text, spaces and other).
- Stores the processed tokens in a transient state in which they might be modified (replaced).
- Dumps the tokens in the same order that they were read from the transient state to an output stream of characters.

For more details see the code [documentation](https://daduraro.github.io/words2digits/).

## Documentation

There are two kind of generated reports:

- [Doxygen generated documentation](https://daduraro.github.io/words2digits/), that can be built from sources directly from a CMake target.

- [gcovr test coverage report](https://daduraro.github.io/words2digits/coverage/), that can be generated, after executing the instrumented unittest (see [How to build](#how-to-build)), using the command:

```sh
gcovr -r source/cli/ -e '.*test.*' build/  --html --html-details -o coverage/index.html
```

## Limitations

### a) Other textual number representations

Language is hard, and it needs context. Any approach to be accurate on the text interpretation based on context-free grammars will unavoidably fail in some cases. For example, texts that refer to dates as "nineteen ninety-two" need context in order to understand they are not two disconnected numbers, "19" and "92", but they rather refer to a single date number: "1992".

Moreover, the current CFG already skips some number representations such as "nil" or "oh" representing 0, and only deals with cardinal numbers up to the millions (although support for billions, trillions, etc. is trivial), being oblivious to fractional numbers, ordinal numbers, etc.

### b) Text encoding

Currently, only UTF-8 / ASCII encodings are supported. See Unicode support limitation.

### c) Unicode support

As all the symbols of the current grammar can be represented as single ASCII/UTF-8 bytes, this first version makes some assumptions on the tokenization. This has the unfortunate effect that multi-byte code points are incorrectly classified as "other", regardless of their actual classification. Regrettably, in a lot of places in the C++ Standard Library it is assumed a single character represent a single code point (see the classification functions in [\<locale\>](https://en.cppreference.com/w/cpp/header/locale)), which is false for variable-length encodings such as UTF-8 or UTF-16. Adding support for languages whose textual numbers representations are not formed by single-byte code points of UTF-8 would require moving the internal character representations to `char16_t` or `char32_t`, or even using some specialized libraries such as [ICU](http://site.icu-project.org/).

### d) CRLF / LF from original file is not preserved

As the application might need to write a newline when a textual number spans multiple line, I made the decision to open files in textual format (which do CRLF/LF conversion) for simplicity and to avoid having inconsistent newline symbols.

