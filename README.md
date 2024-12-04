# PythonStrings for C++

I used https://github.com/imageworks/pystring as a base, updated almost everything to work with std::string_view, 
and made a bunch of functions constexpr. Then I created a class to make the strings work like in Python with method
chaining. All Python methods return copies like Python does! However, most of them have an "in-place brother".
The class has a lot of other useful member functions that are either not available in Python or available in other classes (e.g. textwrap).


Check it out:

```cpp 
#include "pythonstring.h"

int main() {
    const std::string_view line_sep =
        "\n-------------------------------------------\n";

    // Some typedefs to make the code more readable
    using namespace pyindex;
    ///////////////////////////////////////////////////////////////////////////////////////
    // Here are some ways of creating a PythonString object
    std::string about_python =
        "Python is an interpreted, interactive, object-oriented "
        "programming "
        "language. It incorporates modules, exceptions, dynamic "
        "typing, "
        "very "
        "high level dynamic data types, and classes. It supports "
        "multiple "
        "programming paradigms beyond object-oriented programming, "
        "such as "
        "procedural and functional programming. Python combines "
        "remarkable "
        "power with very clear syntax. It has interfaces to many "
        "system "
        "calls "
        "and libraries, as well as to various window systems, and is "
        "extensible in C or C++. It is also usable as an extension "
        "language "
        "for applications that need a programmable interface. Finally, "
        "Python "
        "is portable: it runs on many Unix variants including Linux "
        "and "
        "macOS, "
        "and on Windows";

    PythonString str = about_python;
    const char *iamconstchar = "I am a const char";
    std::cout << "from const char: " << PythonString(iamconstchar) << line_sep;
    std::cout << "from int: " << PythonString(5) << line_sep;
    std::cout
        << "from memory address - especially for Cython - to trick the gil: "
        << PythonString((size_t)(&(about_python.data()[0])),
                        about_python.size())
        << line_sep;
    std::cout << "from float: " << PythonString(12.12) << line_sep;
    std::cout << "from char vector: "
              << PythonString(std::vector<char>{'a', 'b', 'c'}) << line_sep;
    std::cout << "from string_view: "
              << PythonString(std::string_view(about_python)) << line_sep;
    const char *const_chars[] = {"abcd", "efgh", "ijkl", "mnop"};
    std::cout << "from const char** - for Cython: "
              << PythonString(const_chars, 4) << line_sep;

    std::string movetest = "I will be moved";
    std::cout << "from string moved: " << PythonString(std::move(movetest))
              << line_sep;
    std::cout << "moved string is now: " << movetest << line_sep;

    std::cout << "from substring: " << PythonString(about_python.substr(0, 10))
              << line_sep;

    auto pythonstringfrompythonstring = PythonString(str);
    std::cout << "from other PythonString: " << pythonstringfrompythonstring
              << line_sep;
    ;

    auto pythonstringfrompythonstringmoved =
        PythonString(std::move(pythonstringfrompythonstring));
    std::cout << "moved PythonString: " << pythonstringfrompythonstringmoved
              << line_sep;
    ;
    std::cout << "original PythonString: " << pythonstringfrompythonstring
              << line_sep;
    ;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // CLONED PYTHON API STARTS HERE!
    // 
    // Almost 100% cloned Python API
    // The methods always return a new PythonString object;
    // The data is never changed in the original PythonString object (just like
    // in Python). However, there are also _inplace versions of most of the
    //  methods Scroll down to see them.
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.capitalize
    std::cout << "// str.capitalize" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    std::cout << str.capitalize() << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.casefold
    std::cout << "// str.casefold" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    // (a little diffrent from the Python version, removes all accents
    // from latin chars, and does some transformations: ß -> ss) Does
    // not follow the Unicode standard (due to the lack of my Greek, Chinese,
    // etc. knowledge), like Python does:
    // https://www.unicode.org/versions/Unicode15.1.0/ch03.pdf
    // -> 3.13
    //
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    std::cout << PythonString("größer").casefold() << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.center
    std::cout << "// str.center" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    // By the way: methods can be chained like in Python
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("größer").casefold().center(50, 'X') << line_sep;
    std::cout << PythonString("größer").casefold().center(50, "Y") << line_sep;
    std::cout << PythonString("größer").casefold().center(50) << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.count
    std::cout << "// str.count" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\nCounted char 'a': " << str.count('a') << line_sep;
    std::cout << "\nCounted char 'a' from 0 to 10: " << str.count('a', 0, 10)
              << line_sep;

    std::cout << "\nCounted string 'and': " << str.count("and") << line_sep;
    std::cout << "\nCounted string 'and' from 30 to 100: "
              << str.count("and", 30, 100) << line_sep;

    // count multiple strings - not available in Python
    std::cout << "\nCounted words in a map:\n"
              << str.count({"and", "Python", "or", "programming"}) << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.endswith
    std::cout << "// str.endswith" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Ends with 'Windows': " << str.endswith("Windows") << line_sep;
    std::cout << "Ends with 'Mac': " << str.endswith("Mac") << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.expandtabs
    std::cout << "// str.expandtabs" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Expanded tabs: "
              << PythonString("01\t012\t0123\t01234").expandtabs(4) << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.format (uses C++ format strings)
    std::cout << "// str.format (uses C++ format strings, not Python)"
              << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Formated string: "
              << PythonString("%d plus %d is %d").format(1, 2, 3) << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.format_map
    std::cout << "// str.format_map" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    PY_STRING_DICT formatmap{{"name", "Guido"}, {"country", "the Netherlands"}};
    PY_STRING_DICT_UNORDERED formatmap2{{"name", "Guido"},
                                        {"country", "the Netherlands"}};

    std::cout
        << "Format_map string: "
        << PythonString("{name} was born in {country}").format_map(formatmap)
        << PythonString("{name} was born in {country}").format_map(formatmap2)
        << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.index
    std::cout << "// str.index" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Index of 'Python': " << str.index("Python") << line_sep;

    // raise an exception if not found, like in Python
    try {
        std::cout << "Index of 'Cython': " << str.index("Cython") << line_sep;
    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << line_sep;
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.find
    std::cout << "// str.find" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "gst in Angst: " << PythonString("Angst").find("gst")
              << line_sep;
    std::cout << "ba in Angst: " << PythonString("Angst").find("ba")
              << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // in
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "A in Angst: " << PythonString("A").in(PythonString("Angst"))
              << line_sep;
    std::cout << "B in Angst: " << PythonString("B").in(PythonString("Angst"))
              << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.isspace, isalpha, isdigit, isalnum, islower, isupper
    std::cout << "// str.isspace, isalpha, isdigit, isalnum, islower, isupper"
              << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "istitle: " << PythonString("Ana").istitle() << line_sep;

    std::cout << "isprintable: " << PythonString(" ab c").isprintable()
              << line_sep;
    std::cout << "is_ascii: " << PythonString("abc").is_ascii() << line_sep;
    std::cout << "isspace: " << PythonString(" \t \n").isspace() << line_sep;
    std::cout << "isalpha: " << PythonString("abc").isalpha() << line_sep;
    std::cout << "isdigit: " << PythonString("123").isdigit() << line_sep;
    std::cout << "isalnum: " << PythonString("abc123").isalnum() << line_sep;
    std::cout << "islower: " << PythonString("abc").islower() << line_sep;
    std::cout << "isupper: " << PythonString("ABC").isupper() << line_sep;

    std::cout << "istitle: " << PythonString("AnA").istitle() << line_sep;

    std::cout << "isprintable: " << PythonString(" ab c\2").isprintable()
              << line_sep;

    std::cout << "is_ascii: " << PythonString("abç").is_ascii() << line_sep;
    std::cout << "isspace: " << PythonString("xx").isspace() << line_sep;
    std::cout << "isalpha: " << PythonString("!!!").isalpha() << line_sep;
    std::cout << "isdigit: " << PythonString("jjj").isdigit() << line_sep;
    std::cout << "isalnum: " << PythonString("!@#").isalnum() << line_sep;
    std::cout << "islower: " << PythonString("ABC").islower() << line_sep;
    std::cout << "isupper: " << PythonString("abc").isupper() << line_sep;
    std::cout << "isdecimal: " << PythonString("123").isdecimal() << line_sep;
    // alias for isdecimal, different from Python
    std::cout << "isdigit: " << PythonString("123").isdigit() << line_sep;

    std::cout << "isfloat: " << PythonString("123").isfloat() << line_sep;
    std::cout << "isbool: " << PythonString("True").isbool() << line_sep;
    std::cout << "isdecimal: " << PythonString("123.48").isdecimal()
              << line_sep;
    std::cout << "isfloat: " << PythonString("1.2.3").isfloat() << line_sep;
    std::cout << "isbool: " << PythonString("BOOO").isbool() << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.join
    std::cout << "// str.join" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("|").join(PythonString("1,2,3").split(","))
              << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.ljust
    std::cout << "// str.ljust" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("hi, there").ljust(20) << line_sep;
    std::cout << PythonString("hi, there").ljust(20, "X") << line_sep;
    std::cout << PythonString("hi, there").ljust(20, 'Y') << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.lower
    std::cout << "// str.lower" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("hi, there, How Are YOU?").lower() << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.lstrip
    std::cout << "// str.lstrip" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("   spacious   ").lstrip() << line_sep;
    std::cout << PythonString("www.example.com").lstrip("cmowz.") << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    // str.partition
    std::cout << "// str.partition" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////

    std::cout << PythonString("a string to be partitioned at the word be,ok?")
                     .partition("be")
              << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    // str.removeprefix
    std::cout << "// str.removeprefix" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////

    std::cout << PythonString("xyxzxqxgxmystring").removeprefix("xyxzxqxgx")
              << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    // str.removesuffix
    std::cout << "// str.removesuffix" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////

    std::cout << PythonString("mystringxyxzxqxgx").removesuffix("xyxzxqxgx")
              << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    // str.replace
    std::cout << "// str.replace" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////

    std::cout << str.replace("Python", "Cython") << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.rfind
    std::cout << "// str.rfind" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\nfirst index of and: " << str.rfind("and") << line_sep;

    std::cout << "\nif string not found: " << str.rfind("andy") << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.rindex
    std::cout << "// str.rindex" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << str.rindex("and", 150, 180) << line_sep;

    try {

        std::cout << str.rindex("andy", 150, 180) << line_sep;
    } catch (std::exception &e) {
        std::cout << e.what() << line_sep;
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.rjust
    std::cout << "// str.rjust" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////

    std::cout << PythonString("hi, there").rjust(20) << line_sep;
    std::cout << PythonString("hi, there").rjust(20, "X") << line_sep;
    std::cout << PythonString("hi, there").rjust(20, 'Y') << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.rpartition
    std::cout << "// str.rpartition" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("a string to be partitioned at the word be,ok?")
                     .rpartition("be")
              << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.rsplit
    std::cout << "// str.rsplit" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("1,2,3").rsplit(",") << line_sep;
    std::cout << PythonString("1,2,3").rsplit(",", 1) << line_sep;
    std::cout << PythonString("1,2,,3,").rsplit(",", 2) << line_sep;
    std::cout << PythonString("1<>2<>3<4").rsplit("<>", 1) << line_sep;
    std::cout << PythonString("str.rsplit: I will be split at the word be, ok? "
                              "be sure to check that out!")
                     .split("be")
              << line_sep;

    std::cout
        << PythonString(
               "str.rsplit: I will be split at the word be, ok? "
               "be sure to check that out! This time there is a limit of 2")
               .split("be", 2)
        << line_sep;

    std::cout << PythonString(
                     "str.rsplit: I\twill be split at\nwhitespace, ok? ")
                     .split(2)
              << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.split
    std::cout << "// str.split" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    // split at whitespace
    std::vector<PythonString> str_split = str.split();
    std::cout << str_split;
    // same results as
    // https://docs.python.org/3/library/stdtypes.html#str
    std::cout << PythonString("1,2,3").split(",");
    std::cout << PythonString("1,2,3").split(",", 1);
    std::cout << PythonString("1,2,,3,").split(",");
    std::cout << PythonString("1<>2<>3<4").split("<>");
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.splitlines
    std::cout << "// str.splitlines" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("ab c\n\nde fg\rkl\r\n").splitlines();
    std::cout << PythonString("ab c\n\nde fg\rkl\r\n").splitlines(true);
    std::cout << PythonString("One line\n").splitlines();

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.startswith
    std::cout << "// str.startswith" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Startswith with 'Python': " << str.startswith("Python")
              << line_sep;
    std::cout << "Startswith with 'Cython': " << str.startswith("Cython")
              << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.strip
    std::cout << "// str.strip" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    PythonString string2strip{" \t Hello, I will be stripped  !   "};
    std::cout << string2strip.strip() << line_sep;
    std::cout << string2strip.strip("!de\t ") << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.swapcase
    std::cout << "// str.swapcase" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("AbCdEfGjh").swapcase() << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.title
    std::cout << "// str.title" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("AbCdEfGjh").title() << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    // str.translate
    std::cout << "// str.translate" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    PY_TRANSLATION_TABLE mytranslationtable{{'a', 'X'}, {'n', 'Y'}, {'d', 'Z'}};
    std::string delete_these_chars{"rstvwq"};
    auto translatedstring =
        str.translate(mytranslationtable, delete_these_chars);
    std::cout << "\nTranslated string: " << translatedstring << line_sep;

    auto translatedstring2 = str.translate(mytranslationtable, "");
    std::cout << "\nTranslated string 2: " << translatedstring << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.upper
    std::cout << "// str.upper" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("AbCdEfGjh").upper() << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // str.zfill
    std::cout << "// str.zfill" << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << PythonString("6").zfill(6) << "\n";
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // Mostly non Python, but still useful member functions
    // Colored printing, mostly for debugging
    // Can be put between chaining of member functions
    std::cout << PythonString("Some String modiFIcations")
                     .print_bg_cyan(
                         "\nOriginal String: ") // an aditional string can be
                                                // added to facilitate debugging
                     .capitalize()
                     .print_bg_green("\nCapitalized String: ")
                     .lower()
                     .print_bg_red("\nLowered string: ")
                     .upper()
                     .print_bg_yellow("\nUpper string: ")
                     .swapcase()
                     .print_bg_magenta("\nSwaped case: ")
                     .title()
                     .print_bg_green("\nTitle string: ")
              << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // convert_to_base16, to exchange data with other processes, mainly over the
    // stdout, equivalent in Python: base64.b16encode
    //////////////////////////////////////////////////////////////////////////////////////////
    auto base16string =
        PythonString("Hello, my friend, Hello!").convert_to_base16();
    std::cout << base16string << line_sep << line_sep;
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    // from_base16, equivalent in Python: base64.b16decode
    //////////////////////////////////////////////////////////////////////////////////////////
    auto frombase16 = PythonString::from_base16(base16string);
    std::cout << frombase16 << line_sep << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    // wrap string (10 parts), equivalent in Python: textwrap.wrap
    std::cout << str % 10 << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    // remove accents from latin chars, the German letter ß becomes ss

    std::string stringwithaccents = "çÇõä";
    std::cout << PythonString(stringwithaccents).remove_accents() << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    // some useful number checks
    std::cout << PythonString("777")
                     .print_bg_green("\n777 is bin/oct/hex: ")
                     .is_bin_oct_hex_dec()
              << line_sep;
    std::cout
        << PythonString("777").print_bg_green("\n777 is oct: ").is_oct_number()
        << line_sep;

    std::cout << PythonString("0x777")
                     .print_bg_green("\n0x777 is is_hex_number_with_0x: ")
                     .is_hex_number_with_0x()
              << line_sep;

    std::cout << PythonString("777")
                     .print_bg_green("\n777 is is_hex_number_without_0x: ")
                     .is_hex_number_without_0x()
              << line_sep;

    std::cout << PythonString("0b11")
                     .print_bg_green("\n0b11 is_binary_with_0b: ")
                     .is_binary_with_0b()
              << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    // splitting at the strings "and" and "clear", but keeping them, and making
    // sure than the next or previous char is not a forbidden char (kind of like
    // the regex boundary "\b"). We only want to split at words "and" and
    // "clear", not on a word like "band" which includes the word "and"

    std::string forbidden_bound_chars_front =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXY"
        "Z";
    std::string forbidden_bound_chars_back =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXY"
        "Z";
    std::vector<std::string> strings2find{"and", "clear"};
    auto pystrresultssplit = str.split_and_beginning_of_multiple_strings(
        strings2find, forbidden_bound_chars_front, forbidden_bound_chars_back);

    std::cout << pystrresultssplit << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////

    auto randomstring = PythonString::generate_random_alphanumeric_string(100);
    std::cout << randomstring;

    PythonString stringfromfileload =
        PythonString::from_file("C:\\testpytestpoy.txt");
    std::cout << stringfromfileload;
    std::cout << "\n---------------------------------------\n";
    auto stringfromfileloadvec =
        PythonString::read_file_to_string_vector("C:\\testpytestpoy.txt");
    std::cout << stringfromfileloadvec;

    stringfromfileload.save_as_file("c:\\outfiletest.txt");
    stringfromfileload.append_to_file("c:\\outfiletest.txt");
    stringfromfileload.append_to_file("c:\\outfiletest.txt");

    std::cout << PythonString("Hello").find_me_everywhere_in_another_string(
        "Hello, my friend, Hello!");

    std::cout << PythonString("Hello, my friend, Hello!")
                     .find_subststring_everywhere_in_me("Hello");

    ///////////////////////////////////////////////////////////////////////////////////////
    // split strings:
    PythonString string_split_multiple_times =
        "I will be split at the words at be and words, ok?";
    std::vector<std::string> multisplitstring{"at", "be", "words"};
    std::cout << "\nNot keeping strings:\n"
              << string_split_multiple_times.split_at_multi_string(
                     multisplitstring);
    std::cout << "\nKeeping the strings: \n"
              << string_split_multiple_times.split_at_multi_string_keep_strings(
                     multisplitstring);

    std::cout << "Split at single chars: "
              << string_split_multiple_times.split_strtok("atbewords");

    ///////////////////////////////////////////////////////////////////////////////////////
    // str.capitalize_each_word
    PythonString("a string to be capitalized, ok?")
        .capitalize_each_word()
        .print_bg_cyan("This is a capitalized string (each word): ");

    ///////////////////////////////////////////////////////////////////////////////////////
    // insert_strings_at_indices
    std::vector<std::pair<int, std::string>> wordstoinsert{{0, "Zero"},
                                                           {2, "Two"}};
    PythonString("01234")
        .insert_strings_at_indices(wordstoinsert)
        .print_magenta("With inserted words: ");

    ///////////////////////////////////////////////////////////////////////////////////////
    //
    //
    // hashable
    auto muteme = PythonString("I   a m mutable");
    muteme.print_bg_cyan("Original: ");

    muteme[0] = 'Y';
    muteme[1] = 'o';
    muteme[2] = 'u';
    muteme[3] = ' ';
    muteme[4] = 'a';
    muteme[5] = 'r';
    muteme[6] = 'e';
    muteme.print_bg_magenta("1. change: ");
    muteme[PY_SLICE{0, 7, 1}] = "We  are";
    muteme.print_bg_green("2. change: ");
    muteme[PY_SLICE{0, 7, 1}] =
        std::vector<char>{'A', 'l', 'l', ' ', 'a', 'r', 'e'};
    muteme.print_bg_yellow("3. change: ");
    std::cout << muteme[PY_SLICE{0, 7, 1}];
    PythonString(muteme[PY_SLICE{0, 7, 1}])
        .print_bg_yellow("Converted raw pointers to string: ");
    muteme[PY_SLICE{10, 3, -2}] = 'X';
    std::cout << muteme.to_format_BBlue() << line_sep;
    muteme[PY_SLICE{12, 10, -1}] = 'Y';
    std::cout << muteme.to_format_BGreen() << line_sep;

    auto iamnotmuteable = muteme[PY_RANGE(1, 5)];
    iamnotmuteable.print_bg_magenta("Not mutable, no raw pointers: ");

    std::cout << '\n'
              << PythonString("I am a string, and my hash is: ")
                     .print_bg_green()
                     .calculate_hash()
              << "\n "
              << PythonString("I am a string, and my hash is: ")
                     .print_bg_cyan()
                     .calculate_hash()
              << '\n'
              << PythonString("I am another string, and my hash different: ")
                     .print_bg_magenta()
                     .calculate_hash()
              << line_sep;

    PythonString keyvalue1 = std::string("Since I can be hashed");
    std::unordered_map<PythonString, std::string> mymap;
    mymap[keyvalue1] = "I can be used as a key";

    std::cout << mymap[keyvalue1];

    ///////////////////////////////////////////////////////////////////////////////////////
    // compare
    std::cout << "abcdef == abcdef: "
              << (PythonString("abcdef") == PythonString("abcdef")) << line_sep;

    std::cout << "bcdef >= abcdef: "
              << (PythonString("bcdef") >= PythonString("abcdef")) << line_sep;
    std::cout << "bcdef > abcdef: "
              << (PythonString("bcdef") > PythonString("abcdef")) << line_sep;
    std::cout << "bcdef < abcdef: "
              << (PythonString("bcdef") < PythonString("abcdef")) << line_sep;

    std::cout << "bcdef <= abcdef: "
              << (PythonString("bcdef") <= PythonString("abcdef")) << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    // getting single chars and joining them
    auto single_chars =
        str[PY_INT_VEC{0, 1, 2, 3, 4, 5, -10, -9, -8, 7, -6, -5}];
    PythonString("-")
        .join(single_chars)
        .print_bg_green("Single chars joined: ");

    ///////////////////////////////////////////////////////////////////////////////////////
    // reverse string inplace
    str.reverse_inplace().print_bg_cyan("Inplace reverse string: ");

    str.reverse_inplace().print_bg_red(
        "Inplace reverse string->back to normal: ");
    ///////////////////////////////////////////////////////////////////////////////////////
    // reverse string - returns copy
    std::cout << "I am a reversed copy: " << str.reverse().to_format_IBlue()
              << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    // colored debug print
    str.print_bg_cyan();
    str.print_bg_cyan("An optional prefix for debug information: ");

    ///////////////////////////////////////////////////////////////////////////////////////
    // colored debug print
    std::cout << str[PY_RANGE(1, 5)].convert_to_uint8();
    ///////////////////////////////////////////////////////////////////////////////////////
    // operator overloading

    std::string text_string =
        "andPython is an interpreted, interactive, object-oriented programming "
        "language. It incorporates modules, exceptions, dynamic typing, very "
        "high level dynamic data types, and classes. It supports multiple "
        "programming paradigms beyond object-oriented programming, such as "
        "procedural and functional programming. Python combines remarkable "
        "power with very clear syntax. It has interfaces to many system calls "
        "and libraries, as well as to various window systems, and is "
        "extensible in C or C++. It is also usable as an extension language "
        "for applications that need a programmable interface. Finally, Python "
        "is portable: it runs on many Unix variants including Linux and macOS, "
        "and on Windows.and";
    const std::string_view const_str_view = "I am a const string_view";
    PythonString pythonstring_from_string{text_string};
    std::cout << pythonstring_from_string;
    ///////////////////////////////////////////////////////////////////////////////////////
    // from const char*
    std::cout << PythonString(text_string.c_str()).to_format_BIGreen()
              << line_sep;
    std::cout << PythonString(std::string_view(text_string)).to_format_BIRed()
              << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    // from memory address - especially for Cython - byte strings
    size_t memaddress_start = (size_t)(&(text_string.data()[0]));
    std::cout << PythonString(memaddress_start, text_string.size())
                     .to_format_BIPurple()
              << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    // from const char* array - especially for Cython
    const char *testcc[] = {"Hello World!", "Hello World!", "Hello World!"};
    std::cout << PythonString(testcc, 3).to_format_UPurple() << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    // converting
    std::cout << "\nWorked out: " << int(PythonString("12"))
              << "\nDidn't work out: " << int(PythonString("xxx"));
    std::cout << "\nWorked out: " << float(PythonString("12.12"))
              << "\nDidn't work out: " << float(PythonString("1.2.3.4.54.5"));
    std::cout << "\nTrue value: " << bool(PythonString("true"))
              << "\nFalse value: " << bool(PythonString("false")) << line_sep;

    std::vector<std::string> pystring2vector = pythonstring_from_string;
    for (int i = 0; i < pystring2vector.size(); i++) {
        std::cout << pystring2vector[i] << line_sep;
    }
    // std::cout << pythonstring_from_string;
    std::cout << pystring2vector;
    std::cout << "\n------------------------------------------------\n";
    std::list<std::string> pystring2list = pythonstring_from_string;
    std::cout << pystring2list;
    std::cout << "\n------------------------------------------------\n";

    std::set<std::string> pystring2set = pythonstring_from_string;
    std::cout << pystring2set;
    std::cout << "\n------------------------------------------------\n";

    std::unordered_set<std::string> pystring2uset = pythonstring_from_string;
    std::cout << pystring2uset;

    std::cout << "\n------------------------------------------------\n";
    std::unordered_map<int, std::string> pystring2umap =
        pythonstring_from_string;
    std::cout << pystring2umap;
    std::cout << "\n------------------------------------------------\n";
    std::map<int, std::string> pystring2map = pythonstring_from_string;
    std::cout << pystring2map;
    std::cout << "\n------------------------------------------------\n";
    ///////////////////////////////////////////////////////////////////////////////////////
    // operator overloading

    std::cout << (PythonString("First String") +
                  PythonString("\nSecond String")) +
                     " i am a const char, this is a char: " + 'c' +
                     " and this is a substring: " + text_string.substr(0, 4) +
                     " and a const string view: " + const_str_view +
                     "...and also some numbers: " + 110 + " and " + 120 +
                     " and " + 130 + "! we also go floats: " + 1.1f + " and " +
                     1.2f + " and " + 1.3f
              << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    auto newstring2stripinplace = PythonString("Test1123123      ");
    std::cout << newstring2stripinplace << line_sep;
    newstring2stripinplace.strip_inplace();
    std::cout << newstring2stripinplace << line_sep;
    newstring2stripinplace.strip_inplace("123");
    std::cout << newstring2stripinplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    auto newstring2rstripinplace =
        PythonString("   1123123      Test1123123      ");
    std::cout << newstring2rstripinplace << line_sep;
    newstring2rstripinplace.rstrip_inplace();
    std::cout << newstring2rstripinplace << line_sep;
    newstring2rstripinplace.rstrip_inplace("123 ");
    std::cout << newstring2rstripinplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    auto newstring2lstripinplace =
        PythonString("   1123123      Test1123123      ");
    std::cout << newstring2lstripinplace << line_sep;
    newstring2lstripinplace.lstrip_inplace();
    std::cout << newstring2lstripinplace << line_sep;
    newstring2lstripinplace.lstrip_inplace("123 ");
    std::cout << newstring2lstripinplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    PY_TRANSLATION_TABLE mytranslationtable_for_inplace{
        {'a', 'X'}, {'n', 'Y'}, {'d', 'Z'}};
    std::string delete_these_chars_for_inplace{"rstvwq"};
    PythonString strtranlated = str;

    strtranlated.translate_inplace(mytranslationtable_for_inplace);
    std::cout << "\nTranslated string 1: " << strtranlated << line_sep;

    strtranlated.translate_inplace(mytranslationtable_for_inplace,
                                   delete_these_chars_for_inplace);
    std::cout << "\nTranslated string 2:" << strtranlated << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString zfillinplace{6};
    zfillinplace.zfill_inplace(6);
    std::cout << zfillinplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString rjustinplace{"i will be rjusted"};
    std::cout << rjustinplace << line_sep;
    rjustinplace.rjust_inplace(30);
    std::cout << rjustinplace << line_sep;
    rjustinplace.rjust_inplace(50, 'Y');
    std::cout << rjustinplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString ljustinplace{"i will be ljusted"};
    std::cout << ljustinplace << line_sep;
    ljustinplace.ljust_inplace(30);
    std::cout << ljustinplace << line_sep;
    ljustinplace.ljust_inplace(50, 'Y');
    std::cout << ljustinplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString centerinplace{"i will be centered"};
    std::cout << centerinplace << line_sep;
    centerinplace.center_inplace(30);
    std::cout << centerinplace << line_sep;
    centerinplace.center_inplace(50, 'Y');
    std::cout << centerinplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////

    PythonString expandtabsinplace{"i\t\twill\tbe\texpa\tnded\t"};
    std::cout << expandtabsinplace << line_sep;
    expandtabsinplace.expandtabs_inplace(10);
    std::cout << expandtabsinplace << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString replaceinplace{"i will be replaced"};
    std::cout << replaceinplace << line_sep;
    replaceinplace.replace_inplace("will", "have been");
    std::cout << replaceinplace << line_sep;
    replaceinplace.replace_inplace("e", "E", 2);
    std::cout << replaceinplace << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString removeprefixinplace{"i will be removed"};
    std::cout << removeprefixinplace << line_sep;
    removeprefixinplace.removeprefix_inplace("i will");
    std::cout << removeprefixinplace << line_sep;

    removeprefixinplace.removesuffix_inplace("oved");
    std::cout << removeprefixinplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString captilizeeachwordinplace{"i will be capitalized"};
    std::cout << captilizeeachwordinplace << line_sep;
    captilizeeachwordinplace.capitalize_each_word_inplace();
    std::cout << captilizeeachwordinplace << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString capitalize_lower_upper_swapcase_title_inplace{
        "i will be tortured"};
    capitalize_lower_upper_swapcase_title_inplace.capitalize_inplace();
    std::cout << capitalize_lower_upper_swapcase_title_inplace << line_sep;
    capitalize_lower_upper_swapcase_title_inplace.lower_inplace();
    std::cout << capitalize_lower_upper_swapcase_title_inplace << line_sep;
    capitalize_lower_upper_swapcase_title_inplace.upper_inplace();
    std::cout << capitalize_lower_upper_swapcase_title_inplace << line_sep;
    capitalize_lower_upper_swapcase_title_inplace.swapcase_inplace();
    std::cout << capitalize_lower_upper_swapcase_title_inplace << line_sep;
    capitalize_lower_upper_swapcase_title_inplace.title_inplace();
    std::cout << capitalize_lower_upper_swapcase_title_inplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    // insert_strings_at_indices
    std::vector<std::pair<int, std::string>> wordstoinsertinplace{{0, "Zero"},
                                                                  {2, "Two"}};
    auto insert_strings_at_indices_inplace = PythonString("01234");
    insert_strings_at_indices_inplace
        .insert_strings_at_indices_inplace(wordstoinsert)
        .print_magenta("With inserted words, inplace: ");

    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString casefoldinplace{"Größer"};
    casefoldinplace.casefold_inplace();
    std::cout << casefoldinplace << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString removeaccentsinplace{"Größer"};
    removeaccentsinplace.remove_accents_inplace();
    std::cout << removeaccentsinplace << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    auto pythonstring_from_format_inplace = PythonString("%d plus %d is %d");
    pythonstring_from_format_inplace.format_inplace(1, 2, 3);
    std::cout << "Formated string: " << pythonstring_from_format_inplace
              << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    auto formatmap_inplace = PythonString("{name} was born in {country}");

    PY_STRING_DICT formatmap_inplace_test{{"name", "Guido"},
                                          {"country", "the Netherlands"}};
    formatmap_inplace.format_map_inplace(formatmap_inplace_test);

    std::cout << "Format_map string inplace: " << formatmap_inplace << line_sep;

    //////////////////////////////////////////////////////////////////////////////////////////
    auto pythonstring_base16_inplace_test = PythonString("I am just a string");
    pythonstring_base16_inplace_test.convert_to_base16_inplace();
    std::cout << "Base16 inplace encoded: " << pythonstring_base16_inplace_test
              << line_sep;
    pythonstring_base16_inplace_test.from_base16_inplace();
    std::cout << "Base16 inplace decoded: " << pythonstring_base16_inplace_test
              << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////

    std::string forbidden_bound_chars_front_inplace_test =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXY"
        "Z";
    std::string forbidden_bound_chars_back_inplace_test =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXY"
        "Z";
    std::vector<std::string> strings2find_inplace_test{"and", "clear"};
    std::string joinstring_inplace_test = "\n\n";
    auto pystr_inplace_test = str;
    pystr_inplace_test.split_and_beginning_of_multiple_strings_inplace(
        strings2find, forbidden_bound_chars_front, forbidden_bound_chars_back,
        joinstring_inplace_test);
    std::cout << pystr_inplace_test << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    auto not_normalized_whitepace =
        PythonString{"  This  is        a  \t mess!   "};

    auto now_normalized_whitepace =
        not_normalized_whitepace.normalize_whitespaces();
    std::cout << now_normalized_whitepace << line_sep;

    not_normalized_whitepace.normalize_whitespaces_inplace();
    std::cout << not_normalized_whitepace << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    auto not_removed_whitepace_test =
        PythonString{"  This  is        a  \t mess!   "};

    auto now_removed_whitepace_test =
        not_removed_whitepace_test.remove_whitespaces();
    std::cout << now_removed_whitepace_test << line_sep;

    not_removed_whitepace_test.remove_whitespaces_inplace();
    std::cout << not_removed_whitepace_test << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    auto pad_string_test1 = PythonString{"very short string"};
    auto pad_string_test2 = PythonString{
        "very looooooooooooooooooooooooooooooooooooooooong string"};
    std::cout << pad_string_test1.pad_string(30) << line_sep;
    std::cout << pad_string_test2.pad_string(30) << line_sep;
    std::cout << pad_string_test1.pad_string(30, 'x') << line_sep;
    std::cout << pad_string_test2.pad_string(30, 'x') << line_sep;

    pad_string_test1.pad_string_inplace(59, 'y');
    pad_string_test2.pad_string_inplace(59, 'y');
    std::cout << pad_string_test1 << line_sep;
    std::cout << pad_string_test2 << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString str_w_non_alpha_numeric_chars{"\2\3\4!@#$%&*1234ABCDEFG"};
    std::cout
        << str_w_non_alpha_numeric_chars.replace_non_printable_with_spaces()
        << line_sep;
    std::cout
        << str_w_non_alpha_numeric_chars.replace_non_alphanumeric_with_spaces()
        << line_sep;
    std::cout << str_w_non_alpha_numeric_chars.replace_non_decimal_with_spaces()
              << line_sep;

    str_w_non_alpha_numeric_chars.replace_non_printable_with_spaces_inplace()
        .replace_non_alphanumeric_with_spaces_inplace()
        .replace_non_decimal_with_spaces_inplace();

    std::cout << str_w_non_alpha_numeric_chars << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    PythonString forhamming1 = {"hamming"};
    PythonString forhamming2 = {"hemming"};
    PythonString forhamming3 = {"bemmi ghemming"};
    PythonString forhamming4 = {"Lemming"};
    PythonString forhamming5 = {""};
    PythonString forhamming6 = {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"};
    const std::string_view forhamming7 =
        "xxxxxxxxxxxxxxxxxhemmingxxxxxxxxxxxxxxxxxxxx";

    std::cout << forhamming1 << " | " << forhamming1 << " | "
              << forhamming1.hamming_distance_best_fit(forhamming1) << line_sep;

    std::cout << forhamming1 << " | " << forhamming2 << " | "
              << forhamming1.hamming_distance_best_fit(forhamming2) << line_sep;

    std::cout << forhamming1 << " | " << forhamming3 << " | "
              << forhamming1.hamming_distance_best_fit(forhamming3) << line_sep;

    std::cout << forhamming1 << " | " << forhamming4 << " | "
              << forhamming1.hamming_distance_best_fit(forhamming4) << line_sep;

    std::cout << forhamming1 << " | " << forhamming5 << " | "
              << forhamming1.hamming_distance_best_fit(forhamming5) << line_sep;

    std::cout << forhamming1 << " | " << forhamming6 << " | "
              << forhamming1.hamming_distance_best_fit(forhamming6) << line_sep;

    std::cout << forhamming1 << " | "
              << "forhamming6"
              << " | " << forhamming1.hamming_distance_best_fit("forhamming6")
              << line_sep;
    std::cout << forhamming1 << " | " << forhamming7 << " | "
              << forhamming1.hamming_distance_best_fit(forhamming7) << line_sep;

    std::cout << line_sep << line_sep << "substring_distance_best_fit_v1"
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming1 << " | "
              << forhamming1.substring_distance_best_fit_v1(forhamming1)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming2 << " | "
              << forhamming1.substring_distance_best_fit_v1(forhamming2)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming3 << " | "
              << forhamming1.substring_distance_best_fit_v1(forhamming3)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming4 << " | "
              << forhamming1.substring_distance_best_fit_v1(forhamming4)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming5 << " | "
              << forhamming1.substring_distance_best_fit_v1(forhamming5)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming6 << " | "
              << forhamming1.substring_distance_best_fit_v1(forhamming6)
              << line_sep;

    std::cout << forhamming1 << " | "
              << "forhamming6"
              << " | "
              << forhamming1.substring_distance_best_fit_v1("forhamming6")
              << line_sep;
    std::cout << forhamming1 << " | " << forhamming7 << " | "
              << forhamming1.substring_distance_best_fit_v1(forhamming7)
              << line_sep;

    std::cout << line_sep << line_sep << "substring_distance_best_fit_v2"
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming1 << " | "
              << forhamming1.substring_distance_best_fit_v2(forhamming1)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming2 << " | "
              << forhamming1.substring_distance_best_fit_v2(forhamming2)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming3 << " | "
              << forhamming1.substring_distance_best_fit_v2(forhamming3)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming4 << " | "
              << forhamming1.substring_distance_best_fit_v2(forhamming4)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming5 << " | "
              << forhamming1.substring_distance_best_fit_v2(forhamming5)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming6 << " | "
              << forhamming1.substring_distance_best_fit_v2(forhamming6)
              << line_sep;

    std::cout << forhamming1 << " | "
              << "forhamming6"
              << " | "
              << forhamming1.substring_distance_best_fit_v2("forhamming6")
              << line_sep;
    std::cout << forhamming1 << " | " << forhamming7 << " | "
              << forhamming1.substring_distance_best_fit_v2(forhamming7)
              << line_sep;

    std::cout << line_sep << line_sep << "substring_distance_best_fit_v3"
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming1 << " | "
              << forhamming1.substring_distance_best_fit_v3(forhamming1)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming2 << " | "
              << forhamming1.substring_distance_best_fit_v3(forhamming2)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming3 << " | "
              << forhamming1.substring_distance_best_fit_v3(forhamming3)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming4 << " | "
              << forhamming1.substring_distance_best_fit_v3(forhamming4)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming5 << " | "
              << forhamming1.substring_distance_best_fit_v3(forhamming5)
              << line_sep;

    std::cout << forhamming1 << " | " << forhamming6 << " | "
              << forhamming1.substring_distance_best_fit_v3(forhamming6)
              << line_sep;

    std::cout << forhamming1 << " | "
              << "forhamming6"
              << " | "
              << forhamming1.substring_distance_best_fit_v3("forhamming6")
              << line_sep;
    std::cout << forhamming1 << " | " << forhamming7 << " | "
              << forhamming1.substring_distance_best_fit_v3(forhamming7)
              << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    std::cout << "boyer_moore_horspool_searcher_all1\n"
              << str.boyer_moore_horspool_searcher_all("and") << line_sep;

    std::cout << "boyer_moore_horspool_searcher_all2\n"
              << str.boyer_moore_horspool_searcher_all(PythonString("Python"))
              << line_sep;
    ///////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Word frequency\n" << str.count_word_frequency() << line_sep;
    std::cout << "Word frequency\n"
              << str.replace_non_alphanumeric_with_spaces()
                     .lower()
                     .count_word_frequency()
              << line_sep;

    ////////////////////////////////////////////////////////////////////////////////////
    std::cout << "unique words\n" << str.get_unique_words() << line_sep;
    std::cout
        << "unique words\n"
        << str.replace_non_alphanumeric_with_spaces().lower().get_unique_words()
        << line_sep;

    ///////////////////////////////////////////////////////////////////////////////////////
    std::cout << "Keep first occurrence\n"
              << str.remove_duplicated_substrings_keep_first() << line_sep;
    std::cout << "Keep first occurrence\n"
              << str.replace_non_alphanumeric_with_spaces()
                     .lower()
                     .remove_duplicated_substrings_keep_first()
              << line_sep;

    ////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::string> somewordstosort{
        "here",  "HERE", "are",    "ARE",    "some", "SOME", "words",
        "WORDS", "that", "THAT",   "need",   "NEED", "to",   "TO",
        "be",    "BE",   "sorted", "SORTED", "by",   "BY",   "length"};
    std::cout << "PythonString::sort_by_length\n"
              << PythonString::sort_by_length(somewordstosort);
    std::cout << "PythonString::argsort_by_length\n"
              << PythonString::argsort_by_length(somewordstosort);
    std::cout << "PythonString::sort_by_alphabet\n"
              << PythonString::sort_by_alphabet(somewordstosort);
    std::cout << "PythonString::argsort_by_alphabet\n"
              << PythonString::argsort_by_alphabet(somewordstosort);
    std::cout << "PythonString::sort_by_alphabet_reverse\n"
              << PythonString::sort_by_alphabet_reverse(somewordstosort);
    std::cout << "PythonString::argsort_by_alphabet_reverse\n"
              << PythonString::argsort_by_alphabet_reverse(somewordstosort);
    std::cout << "PythonString::sort_by_alphabet_ignore_case\n"
              << PythonString::sort_by_alphabet_ignore_case(somewordstosort);
    std::cout << "PythonString::argsort_by_alphabet_ignore_case\n"
              << PythonString::argsort_by_alphabet_ignore_case(somewordstosort);
    std::cout << "PythonString::sort_by_alphabet_reverse_ignore_case\n"
              << PythonString::sort_by_alphabet_reverse_ignore_case(
                     somewordstosort);
    std::cout << "PythonString::argsort_by_alphabet_reverse_ignore_case\n"
              << PythonString::argsort_by_alphabet_reverse_ignore_case(
                     somewordstosort);
    std::cout << "PythonString::sort_by_alphabet_avarage_ignore_case\n"
              << PythonString::sort_by_alphabet_avarage_ignore_case(
                     somewordstosort);
    std::cout << "PythonString::argsort_by_alphabet_avarage_ignore_case\n"
              << PythonString::argsort_by_alphabet_avarage_ignore_case(
                     somewordstosort);

    ////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    return 0;
}


```








