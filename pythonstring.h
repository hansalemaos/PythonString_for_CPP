#ifndef PYTHONSTRING_H
#define PYTHONSTRING_H

#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <any>
#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <compare>
#include <cstring>
#include <deque>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <string_view>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
#define MAX_32BIT_INT 2147483647

namespace pystring
{
#if defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS) || defined(_MSC_VER)
#ifndef WINDOWS
#define WINDOWS
#endif
#endif
// Based on https://github.com/imageworks/pystring
// Copyright (c) 2008-present Contributors to the Pystring project.
// All Rights Reserved.

//    Redistribution and use in source and binary forms,
//    with or without modification,
//    are permitted provided that the following conditions are met :

//    1. Redistributions
//        of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.

//    2. Redistributions
//        in binary form must reproduce the above copyright notice,
//    this list of conditions and
//                the following disclaimer in the documentation and /
//        or
//        other materials provided with the distribution.

//        3. Neither the name of the copyright holder nor the names of its
//            contributors may be used to endorse or
//        promote products derived from this software
//            without specific prior written permission.

//        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//        CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//    INCLUDING, BUT NOT LIMITED TO,
//    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//        PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//            CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
//    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
//    OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
//                             PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//                             LOSS OF USE, DATA, OR PROFITS;
//                             OR BUSINESS INTERRUPTION)
//        HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//    WHETHER IN CONTRACT, STRICT LIABILITY,
//    OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
//        ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//    EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
typedef int Py_ssize_t;
static const std::string empty_string = "";
/* helper macro to fixup start/end slice values */

#define ADJUST_INDICES(start, end, len)                                                                                \
    if (end > len)                                                                                                     \
        end = len;                                                                                                     \
    else if (end < 0)                                                                                                  \
    {                                                                                                                  \
        end += len;                                                                                                    \
        if (end < 0)                                                                                                   \
            end = 0;                                                                                                   \
    }                                                                                                                  \
    if (start < 0)                                                                                                     \
    {                                                                                                                  \
        start += len;                                                                                                  \
        if (start < 0)                                                                                                 \
            start = 0;                                                                                                 \
    }
namespace
{
//////////////////////////////////////////////////////////////////////////////////////////////
void reverse_strings(std::vector<std::string> &result)
{
    for (std::vector<std::string>::size_type i = 0; i < result.size() / 2; i++)
    {
        std::swap(result[i], result[result.size() - 1 - i]);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////
void split_whitespace(const std::string_view str, std::vector<std::string> &result, int maxsplit)
{
    std::string::size_type i;
    std::string::size_type j;
    std::string::size_type len = str.size();
    for (i = j = 0; i < len;)
    {
        while (i < len && ::isspace(str[i]))
            i++;
        j = i;
        while (i < len && !::isspace(str[i]))
            i++;
        if (j < i)
        {
            if (maxsplit-- <= 0)
                break;
            result.emplace_back(std::string(str.substr(j, i - j)));
            while (i < len && ::isspace(str[i]))
                i++;
            j = i;
        }
    }
    if (j < len)
    {
        result.emplace_back(std::string(str.substr(j, len - j)));
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////
void rsplit_whitespace(const std::string_view str, std::vector<std::string> &result, int maxsplit)
{
    std::string::size_type len = str.size();
    std::string::size_type i;
    std::string::size_type j;
    for (i = j = len; i > 0;)
    {
        while (i > 0 && ::isspace(str[i - 1]))
            i--;
        j = i;
        while (i > 0 && !::isspace(str[i - 1]))
            i--;
        if (j > i)
        {
            if (maxsplit-- <= 0)
                break;
            result.emplace_back(std::string(str.substr(i, j - i)));
            while (i > 0 && ::isspace(str[i - 1]))
                i--;
            j = i;
        }
    }
    if (j > 0)
    {
        result.emplace_back(std::string(str.substr(0, j)));
    }
    reverse_strings(result);
}
} // anonymous namespace
//////////////////////////////////////////////////////////////////////////////////////////////
static std::vector<size_t> find_all_indices_strviewfind(const std::string_view haystack, const std::string_view needle)
{
    std::vector<size_t> indices;
    size_t offset = 0;
    size_t newoffset = 0;
    size_t maxlen = haystack.size();
    for (;;)
    {
        newoffset = haystack.substr(offset, maxlen).find(needle);
        if (newoffset == std::string::npos)
            break;
        offset += newoffset;
        indices.push_back(offset);
        maxlen = haystack.size() - offset - needle.size();
        offset += needle.size();
    }
    return indices;
}

static void split_at_string(const std::string_view haystack, const std::string_view splitat,
                            std::vector<std::string> &resultssplitsx)
{
    size_t splitatlen = splitat.size();
    auto allresults = find_all_indices_strviewfind(haystack, splitat);
    std::unordered_set<size_t> unorderedsetresults;
    unorderedsetresults.reserve(allresults.size() * splitatlen * 2);
    resultssplitsx.reserve(unorderedsetresults.size());
    size_t resultsplitcounter = 0;
    for (const auto &result : allresults)
    {
        std::string resultsplitsub;
        resultsplitsub.reserve(haystack.size());
        resultssplitsx.emplace_back(std::move(resultsplitsub));
        for (int i = 0; i < splitatlen; i++)
        {
            unorderedsetresults.insert(result + i);
        }
    }
    bool last_was_split = false;
    size_t oldindex = 0;
    size_t vectorindex = 0;
    for (size_t i = 0; i < haystack.size(); i++)
    {
        if (unorderedsetresults.contains(i))
        {
            if ((i - oldindex) > 1)
            {
                vectorindex++;
            }
            oldindex = i;
            continue;
        }
        if (vectorindex >= resultssplitsx.size())
        {
            break;
        }
        resultssplitsx[vectorindex] += haystack[i];
    }
}

static constexpr std::string_view spaces_and_pontuation_view = " \t\n\r\f\v!\"#$%&()*+,./:;<=>?@[\\]^`{|}~";
static std::string spaces_and_pontuation = " \t\n\r\f\v!\"#$%&()*+,./:;<=>?@[\\]^`{|}~";
static void split(const std::string_view str, std::vector<std::string> &result, const std::string_view sep,
                  int maxsplit)
{
    result.clear();
    if (maxsplit < 0)
    {
        maxsplit = MAX_32BIT_INT;
    }
    if (sep.size() == 0)
    {
        split_whitespace(str, result, maxsplit);
        return;
    }
    std::string::size_type i;
    std::string::size_type j;
    std::string::size_type len = str.size();
    std::string::size_type n = sep.size();
    i = j = 0;
    while (i + n <= len)
    {
        if (str[i] == sep[0] && str.substr(i, n) == sep)
        {
            if (maxsplit-- <= 0)
                break;
            result.emplace_back(std::string(str.substr(j, i - j)));
            i = j = i + n;
        }
        else
        {
            i++;
        }
    }
    result.emplace_back(std::string(str.substr(j, len - j)));
}
//////////////////////////////////////////////////////////////////////////////////////////////
static void rsplit(const std::string_view str, std::vector<std::string> &result, const std::string_view sep,
                   int maxsplit)
{
    if (maxsplit < 0)
    {
        split(str, result, sep, maxsplit);
        return;
    }
    result.clear();
    if (sep.size() == 0)
    {
        rsplit_whitespace(str, result, maxsplit);
        return;
    }
    Py_ssize_t i;
    Py_ssize_t j;
    Py_ssize_t len = (Py_ssize_t)str.size();
    Py_ssize_t n = (Py_ssize_t)sep.size();
    i = j = len;
    while (i >= n)
    {
        if (str[i - 1] == sep[n - 1] && str.substr(i - n, n) == sep)
        {
            if (maxsplit-- <= 0)
                break;
            result.emplace_back(std::string(str.substr(i, j - i)));
            i = j = i - n;
        }
        else
        {
            i--;
        }
    }
    result.emplace_back(std::string(str.substr(0, j)));
    reverse_strings(result);
}
//////////////////////////////////////////////////////////////////////////////////////////////
#define LEFTSTRIP 0
#define RIGHTSTRIP 1
#define BOTHSTRIP 2
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string do_strip(const std::string_view str, int strip_type, const std::string_view chars)
{
    bool do_right = false;
    bool do_left = false;
    if (strip_type == BOTHSTRIP)
    {
        do_left = true;
        do_right = true;
    }
    else if (strip_type == LEFTSTRIP)
    {
        do_left = true;
    }
    else if (strip_type == RIGHTSTRIP)
    {
        do_right = true;
    }
    std::vector<uint8_t> indices_left;
    indices_left.resize(str.size(), 0);
    std::vector<uint8_t> indices_right;
    indices_right.resize(str.size(), 0);
    std::vector<uint8_t> indices_both;
    indices_both.resize(str.size(), 0);
    std::unordered_set<char> _chars;
    std::string newstring;
    newstring.reserve(str.size());
    if (chars.size() == 0)
    {
        _chars = {'\n', '\t', '\r', ' ', '\v', '\f'};
    }
    else
    {
        _chars = {chars.begin(), chars.end()};
    }
    size_t indexcounter = 0;
    if (do_left)
    {
        for (auto &chr : str)
        {
            if (_chars.contains(chr))
            {
                indices_left[indexcounter] = 1;
            }
            else
            {
                break;
            }
            indexcounter++;
        }
    }
    if (do_right)
    {
        for (int i = (int)str.size() - 1; i >= 0; i--)
        {
            if (_chars.contains(str[i]))
            {
                indices_right[i] = 1;
            }
            else
            {
                break;
            }
        }
    }
    for (size_t i = 0; i < indices_both.size(); i++)
    {
        if (indices_right[i] || indices_left[i])
        {
            indices_both[i] = 1;
        }
    }
    for (size_t i = 0; i < indices_both.size(); i++)
    {
        if (!indices_both[i])
        {
            newstring += str[i];
        }
    }
    return newstring;
}
//////////////////////////////////////////////////////////////////////////////////////////////

static std::string strip(const std::string_view str, const std::string_view chars)
{
    return do_strip(str, BOTHSTRIP, chars);
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string lstrip(const std::string_view str, const std::string_view chars)
{
    return do_strip(str, LEFTSTRIP, chars);
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string rstrip(const std::string_view str, const std::string_view chars)
{
    return do_strip(str, RIGHTSTRIP, chars);
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string join(const std::string str, const std::vector<std::string> &seq)
{
    std::vector<std::string>::size_type seqlen = seq.size();
    std::vector<std::string>::size_type i;
    if (seqlen == 0)
        return empty_string;
    if (seqlen == 1)
        return seq[0];
    std::string result(seq[0]);
    for (i = 1; i < seqlen; ++i)
    {
        result += str + seq[i];
    }
    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
/* Matches the end (direction >= 0) or start (direction < 0) of self
 * against substr, using the start and end arguments. Returns
 * -1 on error, 0 if not found and 1 if found.
 */
static constexpr int _string_tailmatch(const std::string_view self, const std::string_view substr, Py_ssize_t start,
                                       Py_ssize_t end, int direction)
{
    Py_ssize_t len = (Py_ssize_t)self.size();
    Py_ssize_t slen = (Py_ssize_t)substr.size();
    auto tmpstr1 = std::string(substr);
    auto tmpstr2 = std::string(self);
    const char *sub = tmpstr1.c_str();
    const char *str = tmpstr2.c_str();
    ADJUST_INDICES(start, end, len);
    if (direction < 0)
    {
        // startswith
        if (start + slen > len)
            return 0;
    }
    else
    {
        // endswith
        if (end - start < slen || start > len)
            return 0;
        if (end - slen > start)
            start = end - slen;
    }
    if (end - start >= slen)
        return (!std::memcmp(str + start, sub, slen));
    return 0;
}
} // namespace
static constexpr bool endswith(const std::string_view str, const std::string_view suffix, int start, int end)
{
    int result = _string_tailmatch(str, suffix, (Py_ssize_t)start, (Py_ssize_t)end, +1);
    return static_cast<bool>(result);
}
static constexpr bool _endswith(const std::string_view haystack, const std::string_view needle,
                                [[maybe_unused]] int start, [[maybe_unused]] int end)
{
    if (haystack.size() < needle.size())
    {
        return false;
    }
    int lasthaystackindex = (int)haystack.size() - 1;
    for (int i = needle.size() - 1; i >= 0; i--)
    {
        if (haystack[lasthaystackindex--] != needle[i])
        {
            return false;
        }
    }
    return true;
}
static constexpr bool startswith(const std::string_view str, const std::string_view prefix, int start, int end)
{
    int result = _string_tailmatch(str, prefix, (Py_ssize_t)start, (Py_ssize_t)end, -1);
    return static_cast<bool>(result);
}
static constexpr bool _startswith(const std::string_view haystack, const std::string_view needle,
                                  [[MAYBE_UNUSED]] int start, [[MAYBE_UNUSED]] int end)
{
    if (haystack.size() < needle.size())
    {
        return false;
    }
    for (size_t i = 0; i < needle.size(); i++)
    {
        if (haystack[i] != needle[i])
        {
            return false;
        }
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr char integer_numbers[] = "0123456789";
static constexpr char float_numbers[] = "0123456789.";
static constexpr char hex_upper_letters[] = "0123456789ABCDEF";
static constexpr unsigned int FNV_PRIME = 16777619u;
static constexpr std::string_view bool1 = "True";
static constexpr std::string_view bool2 = "False";
static constexpr std::string_view bool3 = "true";
static constexpr std::string_view bool4 = "false";
static constexpr std::string_view bool5 = "0";
static constexpr std::string_view bool6 = "1";
static constexpr std::string_view bool7 = "TRUE";
static constexpr std::string_view bool8 = "FALSE";
static constexpr std::string_view emptystringview = "";

static constexpr bool is_bool(const std::string_view s)
{

    if (s == bool1 || s == bool2 || s == bool3 || s == bool4 || s == bool5 || s == bool6 || s == bool7 || s == bool8)
    {
        return true;
    }
    return false;
}
static bool is_int_number(const std::string_view s)
{

    if (s.size() == 0)
    {
        return false;
    };
    if ((s.size() > 1) && ((s[0] == '-') || (s[0] == '+')) && ((s[1] != '-') && (s[1] != '+')))
    {
        return is_int_number(s.substr(1, s.size() - 1));
    }
    for (const char &i : s)
    {
        bool isnumber = false;
        for (const char &j : integer_numbers)
        {
            if (i == j)
            {
                isnumber = true;
                break;
            }
        }
        if (!isnumber)
        {
            return false;
        }
    }
    return true;
}
static bool is_float_number(const std::string_view s)
{
    if (s.size() == 0)
    {
        return false;
    };
    if ((s.size() > 1) && ((s[0] == '-') || (s[0] == '+')) && ((s[1] != '-') && (s[1] != '+')))
    {
        return is_float_number(s.substr(1, s.size() - 1));
    }
    size_t dotcounter = 0;
    for (const char &i : s)
    {
        bool isnumber = false;
        for (const char &j : float_numbers)
        {
            if (i == j)
            {
                if (i == '.')
                {
                    dotcounter += 1;
                    if (dotcounter > 1)
                    {
                        return false;
                    }
                }
                isnumber = true;
                break;
            }
        }
        if (!isnumber)
        {
            return false;
        }
    }
    return true;
}

static constexpr bool isalnum(const std::string_view str)
{
    std::string::size_type len = str.size();
    std::string::size_type i;
    if (len == 0)
        return false;
    if (len == 1)
    {
        return ::isalnum(str[0]);
    }
    for (i = 0; i < len; ++i)
    {
        if (!::isalnum(str[i]))
            return false;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr bool isalpha(const std::string_view str)
{
    std::string::size_type len = str.size();
    std::string::size_type i;
    if (len == 0)
        return false;
    if (len == 1)
        return ::isalpha((int)str[0]);
    for (i = 0; i < len; ++i)
    {
        if (!::isalpha((int)str[i]))
            return false;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr bool isdigit(const std::string_view str)
{
    std::string::size_type len = str.size();
    std::string::size_type i;
    if (len == 0)
        return false;
    if (len == 1)
        return ::isdigit(str[0]);
    for (i = 0; i < len; ++i)
    {
        if (!::isdigit(str[i]))
            return false;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr bool islower(const std::string_view str)
{
    std::string::size_type len = str.size();
    std::string::size_type i;
    if (len == 0)
        return false;
    if (len == 1)
        return ::islower(str[0]);
    for (i = 0; i < len; ++i)
    {
        if (!::islower(str[i]))
            return false;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr bool isspace(const std::string_view str)
{
    std::string::size_type len = str.size();
    std::string::size_type i;
    if (len == 0)
        return false;
    if (len == 1)
        return ::isspace(str[0]);
    for (i = 0; i < len; ++i)
    {
        if (!::isspace(str[i]))
            return false;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr bool istitle(const std::string_view str)
{
    std::string::size_type len = str.size();
    std::string::size_type i;
    if (len == 0)
        return false;
    if (len == 1)
        return ::isupper(str[0]);
    bool cased = false;
    bool previous_is_cased = false;
    for (i = 0; i < len; ++i)
    {
        if (::isupper(str[i]))
        {
            if (previous_is_cased)
            {
                return false;
            }
            previous_is_cased = true;
            cased = true;
        }
        else if (::islower(str[i]))
        {
            if (!previous_is_cased)
            {
                return false;
            }
            previous_is_cased = true;
            cased = true;
        }
        else
        {
            previous_is_cased = false;
        }
    }
    return cased;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr bool isupper(const std::string_view str)
{
    std::string::size_type len = str.size();
    std::string::size_type i;
    if (len == 0)
        return false;
    if (len == 1)
        return ::isupper(str[0]);
    for (i = 0; i < len; ++i)
    {
        if (!::isupper(str[i]))
            return false;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string capitalize(const std::string_view str)
{
    std::string s(str);
    std::string::size_type len = s.size();
    std::string::size_type i;
    if (len > 0)
    {
        if (::islower(s[0]))
            s[0] = (char)::toupper(s[0]);
    }
    for (i = 1; i < len; ++i)
    {
        if (::isupper(s[i]))
            s[i] = (char)::tolower(s[i]);
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string lower(const std::string_view str)
{
    std::string s(str);
    std::string::size_type len = s.size();
    std::string::size_type i;
    for (i = 0; i < len; ++i)
    {
        if (::isupper(s[i]))
            s[i] = (char)::tolower(s[i]);
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string upper(const std::string_view str)
{
    std::string s(str);
    std::string::size_type len = s.size();
    std::string::size_type i;
    for (i = 0; i < len; ++i)
    {
        if (::islower(s[i]))
            s[i] = (char)::toupper(s[i]);
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string swapcase(const std::string_view str)
{
    std::string s(str);
    std::string::size_type len = s.size();
    std::string::size_type i;
    for (i = 0; i < len; ++i)
    {
        if (::islower(s[i]))
            s[i] = (char)::toupper(s[i]);
        else if (::isupper(s[i]))
            s[i] = (char)::tolower(s[i]);
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string title(const std::string_view str)
{
    std::string s(str);
    std::string::size_type len = s.size();
    std::string::size_type i;
    bool previous_is_cased = false;
    for (i = 0; i < len; ++i)
    {
        int c = s[i];
        if (::islower(c))
        {
            if (!previous_is_cased)
            {
                s[i] = (char)::toupper(c);
            }
            previous_is_cased = true;
        }
        else if (::isupper(c))
        {
            if (previous_is_cased)
            {
                s[i] = (char)::tolower(c);
            }
            previous_is_cased = true;
        }
        else
        {
            previous_is_cased = false;
        }
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string translate(const std::string_view str, const std::string_view table,
                             const std::string_view deletechars)
{
    std::string s;
    std::string::size_type len = str.size();
    std::string::size_type dellen = deletechars.size();
    if (table.size() != 256)
    {
        return std::string(str);
    }
    if (dellen == 0)
    {
        s = str;
        for (std::string::size_type i = 0; i < len; ++i)
        {
            s[i] = table[s[i]];
        }
        return s;
    }
    int trans_table[256];
    for (int i = 0; i < 256; i++)
    {
        trans_table[i] = table[i];
    }
    for (std::string::size_type i = 0; i < dellen; i++)
    {
        trans_table[(int)deletechars[i]] = -1;
    }
    for (std::string::size_type i = 0; i < len; ++i)
    {
        if (trans_table[(int)str[i]] != -1)
        {
            s += table[str[i]];
        }
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string zfill(const std::string_view str, int width)
{
    int len = (int)str.size();
    if (len >= width)
    {
        return std::string(str);
    }
    std::string s(str);
    int fill = width - len;
    s = std::string(fill, '0') + s;
    if (s[fill] == '+' || s[fill] == '-')
    {
        s[0] = s[fill];
        s[fill] = '0';
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string ljust(const std::string_view str, int width, char fillchar = ' ')
{
    std::string::size_type len = str.size();
    if (((int)len) >= width)
        return std::string(str);
    return std::string(str) + std::string(width - len, fillchar);
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string rjust(const std::string_view str, int width, char fillchar = ' ')
{
    std::string::size_type len = str.size();
    if (((int)len) >= width)
        return std::string(str);
    return std::string(width - len, fillchar) + std::string(str);
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string center(const std::string_view str, int width, char fillchar = ' ')
{
    int len = (int)str.size();
    int marg;
    int left;
    if (len >= width)
        return std::string(str);
    marg = width - len;
    left = marg / 2 + (marg & width & 1);
    return std::string(left, fillchar) + std::string(str) + std::string(marg - left, fillchar);
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string slice(const std::string_view str, int start, int end)
{
    ADJUST_INDICES(start, end, (int)str.size());
    if (start >= end)
        return empty_string;
    return std::string(str).substr(start, end - start);
}
//////////////////////////////////////////////////////////////////////////////////////////////
static int constexpr find(const std::string_view str, const std::string_view sub, int start = 0,
                          int end = MAX_32BIT_INT)
{
    ADJUST_INDICES(start, end, (int)str.size());
    std::string::size_type result = str.substr(0, end).find(sub, start);
    // If we cannot find the string, or if the end-point of our found
    // substring is past the allowed end limit, return that it can't be
    // found.
    if (result == std::string::npos || (result + sub.size() > (std::string::size_type)end))
    {
        return -1;
    }
    return (int)result;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr int index(const std::string_view str, const std::string_view sub, int start, int end)
{
    return find(str, sub, start, end);
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr int rfind(const std::string_view str, const std::string_view sub, int start, int end)
{
    ADJUST_INDICES(start, end, (int)str.size());
    std::string::size_type result = str.rfind(sub, end);
    if (result == std::string::npos || result < (std::string::size_type)start ||
        (result + sub.size() > (std::string::size_type)end))
        return -1;
    return (int)result;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr int rindex(const std::string_view str, const std::string_view sub, int start, int end)
{
    return rfind(str, sub, start, end);
}
static void partition(const std::string_view str, const std::string_view sep, std::vector<std::string> &result)
{

    result.resize(3);
    int index = find(str, sep, 0, MAX_32BIT_INT);
    if (index < 0)
    {
        result[0] = str;
        result[1] = empty_string;
        result[2] = empty_string;
    }
    else
    {
        result[0] = str.substr(0, index);
        result[1] = sep;
        result[2] = str.substr(index + sep.size(), str.size());
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////
static void rpartition(const std::string_view str, const std::string_view sep, std::vector<std::string> &result)
{
    result.resize(3);
    int index = rfind(str, sep, 0, MAX_32BIT_INT);
    if (index < 0)
    {
        result[0] = empty_string;
        result[1] = empty_string;
        result[2] = str;
    }
    else
    {
        result[0] = str.substr(0, index);
        result[1] = sep;
        result[2] = str.substr(index + sep.size(), str.size());
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string expandtabs(const std::string_view str, int tabsize)
{
    std::string s(str);
    std::string::size_type len = str.size();
    std::string::size_type i = 0;
    int offset = 0;
    int j = 0;
    for (i = 0; i < len; ++i)
    {
        if (str[i] == '\t')
        {
            if (tabsize > 0)
            {
                int fillsize = tabsize - (j % tabsize);
                j += fillsize;
                s.replace(i + offset, 1, std::string(fillsize, ' '));
                offset += fillsize - 1;
            }
            else
            {
                s.replace(i + offset, 1, empty_string);
                offset -= 1;
            }
        }
        else
        {
            j++;
            if (str[i] == '\n' || str[i] == '\r')
            {
                j = 0;
            }
        }
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static constexpr int count(const std::string_view str, const std::string_view substr, int start, int end)
{
    int nummatches = 0;
    int cursor = start;
    for (;;)
    {
        cursor = find(str, substr, cursor, end);
        if (cursor < 0)
            break;
        cursor += (int)substr.size();
        nummatches += 1;
    }
    return nummatches;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string replace(const std::string_view str, const std::string_view oldstr, const std::string_view newstr,
                           int count)
{
    int sofar = 0;
    int cursor = 0;
    std::string s(str);
    std::string::size_type oldlen = oldstr.size();
    std::string::size_type newlen = newstr.size();
    cursor = find(s, oldstr, cursor);
    while (cursor != -1 && cursor <= (int)s.size())
    {
        if (count > -1 && sofar >= count)
        {
            break;
        }
        s.replace(cursor, oldlen, newstr);
        cursor += (int)newlen;
        if (oldlen != 0)
        {
            cursor = find(s, oldstr, cursor);
        }
        else
        {
            ++cursor;
        }
        ++sofar;
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////
static void splitlines(const std::string_view str, std::vector<std::string> &result, bool keepends)
{
    result.clear();
    std::string::size_type len = str.size();
    std::string::size_type i;
    std::string::size_type j;
    std::string::size_type eol;
    for (i = j = 0; i < len;)
    {
        while (i < len && str[i] != '\n' && str[i] != '\r')
            i++;
        eol = i;
        if (i < len)
        {
            if (str[i] == '\r' && i + 1 < len && str[i + 1] == '\n')
            {
                i += 2;
            }
            else
            {
                i++;
            }
            if (keepends)
                eol = i;
        }
        result.emplace_back(std::string(str.substr(j, eol - j)));
        j = i;
    }
    if (j < len)
    {
        result.emplace_back(std::string(str.substr(j, len - j)));
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string __mul__(const std::string_view str, int n)
{
    // Early exits
    if (n <= 0)
        return empty_string;
    if (n == 1)
        return std::string(str);
    std::ostringstream os;
    for (int i = 0; i < n; ++i)
    {
        os << str;
    }
    return os.str();
}
//////////////////////////////////////////////////////////////////////////////////////////////
static void generate_random_alphanumstring(char *s, size_t len)
{
    // https://stackoverflow.com/a/440147/15096247
    for (size_t i = 0; i < len; ++i)
    {
        int randomChar = rand() % (26 + 26 + 10);
        if (randomChar < 26)
            s[i] = 'a' + randomChar;
        else if (randomChar < 26 + 26)
            s[i] = 'A' + randomChar - 26;
        else
            s[i] = '0' + randomChar - 26 - 26;
    }
    s[len] = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
static std::string removeprefix(const std::string_view str, const std::string_view prefix)
{
    if (pystring::_startswith(str, prefix, 0, MAX_32BIT_INT))
    {
        return std::string(str).substr(prefix.length());
    }
    return std::string(str);
}
//////////////////////////////////////////////////////////////////////////////////////////////
static std::string removesuffix(const std::string_view str, const std::string_view suffix)
{
    if (pystring::_endswith(str, suffix, 0, MAX_32BIT_INT))
    {
        return std::string(str).substr(0, str.length() - suffix.length());
    }
    return std::string(str);
}
static std::variant<double, int64_t> to_float_or_int(std::string_view s)
{
    std::variant<double, int64_t> returnvalue{};
    std::string tmpstring = std::string(s);
    if (pystring::is_int_number(s))
    {
        returnvalue = std::stoll(tmpstring);
    }
    else if (pystring::is_float_number(s))
    {
        returnvalue = std::stod(tmpstring);
    }
    return returnvalue;
}
bool check_if_string_is_valid_float_zero(const std::string_view str)
{

    if (pystring::is_float_number(str))
    {
        if (atof(str.data()) == 0)
        {
            return true;
        }
    }
    size_t strlen = str.size();
    if ((strlen > 1) && (str[0] == '+') && (pystring::is_float_number(str.substr(1, strlen - 1))))
    {
        return ((atof(&str.data()[1])) == 0);
    }
    if ((strlen > 1) && (str[0] == '-') && (pystring::is_float_number(str.substr(1, strlen - 1))))
    {
        return ((atof(&str.data()[1])) == 0);
    }
    return false;
}
static constexpr std::string_view zero1 = "0";
static constexpr std::string_view zero2 = "0.0";
static constexpr std::string_view zero3 = ".0";
static constexpr std::string_view zero4 = "-0";
static constexpr std::string_view zero5 = "-0.0";
static constexpr std::string_view zero6 = "+0.0";
bool check_if_string_is_valid_int_zero(const std::string_view str)
{

    if ((str == zero1) || (str == zero2) || (str == zero3) || (str == zero4) || (str == zero5) || (str == zero6))
    {
        return true;
    }

    size_t strlen = str.size();

    if (strlen == 0)
    {
        return false;
    }
    if ((strlen > 1) && (str[0] == '+') && (pystring::is_float_number(str.substr(1, strlen - 1))))
    {
        return (int(atof(&str.data()[1])) == 0);
    }
    if ((strlen > 1) && (str[0] == '-') && (pystring::is_float_number(str.substr(1, strlen - 1))))
    {
        return ((atof(&str.data()[1])) == 0);
    }
    if (pystring::is_int_number(str))
    {
        return (int(atof(str.data())) == 0);
    }

    if ((str[0] != '0') && (str[0] != '.'))
    {
        return false;
    }
    if ((str[0] == '.') && (strlen == 1))
    {
        return false;
    }

    if (str[0] == '.')
    {
        return pystring::is_int_number(str.substr(1, strlen - 1));
    }
    if ((str[0] == '0') && (strlen == 1))
    {
        return true;
    }
    if (pystring::is_float_number(str))
    {
        double tempdouble = atof(str.data());
        return (tempdouble < 1) && (tempdouble >= 0);
    }
    return false;
}

// based on https://github.com/p-ranav/structopt

//    MIT License

//    Copyright(c) 2019 Pranav

//    Permission is hereby granted,
//    free of charge,
//    to any person obtaining a copy of
//    this software and associated documentation files(the "Software"),
//    to deal in the Software without restriction,
//    including without limitation the rights to use, copy, modify,
//    merge, publish, distribute, sublicense, and / or sell copies of
//    the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions :

//    The above copyright notice and this permission notice shall be
//    included in all copies or substantial portions of the Software.

//    THE SOFTWARE IS PROVIDED "AS IS",
//    WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT
//    SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
//    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//    TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//    DEALINGS IN THE SOFTWARE.

static constexpr std::string_view integer_numbers_string_view = "0123456789";
static constexpr std::string_view float_numbers_string_view = "0123456789.";
static constexpr std::string_view hex_upper_letters_string_view = "0123456789ABCDEF";
static constexpr std::string_view hex_lower_letters_string_view = "0123456789abcdef";
static constexpr std::string_view prefix_0b = "0b";
static constexpr std::string_view prefix_01 = "01";
static constexpr std::string_view prefix_0x = "0x";
static constexpr std::string_view number_0 = "0";

static constexpr std::string_view octal_numbers_string_view = "01234567";

static constexpr bool is_binary_notation(const std::string_view input)
{
    return input.compare(0, 2, prefix_0b) == 0 && input.size() > 2 &&
           input.find_first_not_of(prefix_01, 2) == std::string_view::npos;
}

static constexpr bool is_hex_notation_upper_with_0x(const std::string_view input)
{
    return input.compare(0, 2, prefix_0x) == 0 && input.size() > 2 &&
           (input.find_first_not_of(hex_upper_letters_string_view, 2) == std::string_view::npos);
}
static constexpr bool is_hex_notation_upper_without_0x(const std::string_view input)
{
    return input.size() > 0 && (input.find_first_not_of(hex_upper_letters_string_view, 2) == std::string_view::npos);
}
static constexpr bool is_hex_notation_lower_with_0x(const std::string_view input)
{
    return input.compare(0, 2, prefix_0x) == 0 && input.size() > 2 &&
           (input.find_first_not_of(hex_lower_letters_string_view, 2) == std::string_view::npos);
}
static constexpr bool is_hex_notation_lower_without_0x(const std::string_view input)
{
    return input.size() > 0 && (input.find_first_not_of(hex_lower_letters_string_view, 2) == std::string_view::npos);
}

static constexpr bool is_octal_notation(const std::string_view input)
{
    return input.size() > 1 && input.find_first_not_of(octal_numbers_string_view, 1) == std::string_view::npos;
}

static constexpr bool is_any_valid_number(const std::string_view input)
{
    if (is_binary_notation(input) || is_hex_notation_lower_with_0x(input) || is_hex_notation_upper_with_0x(input) ||
        is_octal_notation(input))
    {
        return true;
    }

    if (input.empty())
    {
        return false;
    }

    std::size_t i = 0;
    std::size_t j = input.length() - 1;

    // Handling whitespaces
    while (i < input.length() && input[i] == ' ')
        i++;
    while (input[j] == ' ')
        j--;

    if (i > j)
        return false;

    // if string is of length 1 and the only
    // character is not a digit
    if (i == j && !(input[i] >= '0' && input[i] <= '9'))
        return false;

    // If the 1st char is not '+', '-', '.' or digit
    if (input[i] != '.' && input[i] != '+' && input[i] != '-' && !(input[i] >= '0' && input[i] <= '9'))
        return false;

    // To check if a '.' or 'e' is found in given
    // string. We use this flag to make sure that
    // either of them appear only once.
    bool dot_or_exp = false;

    for (; i <= j; i++)
    {
        // If any of the char does not belong to
        // {digit, +, -, ., e}
        if (input[i] != 'e' && input[i] != '.' && input[i] != '+' && input[i] != '-' &&
            !(input[i] >= '0' && input[i] <= '9'))
            return false;

        if (input[i] == '.')
        {
            // checks if the char 'e' has already
            // occurred before '.' If yes, return false;.
            if (dot_or_exp == true)
                return false;

            // If '.' is the last character.
            if (i + 1 > input.length())
                return false;

            // if '.' is not followed by a digit.
            if (!(input[i + 1] >= '0' && input[i + 1] <= '9'))
                return false;
        }

        else if (input[i] == 'e')
        {
            // set dot_or_exp = 1 when e is encountered.
            dot_or_exp = true;

            // if there is no digit before 'e'.
            if (!(input[i - 1] >= '0' && input[i - 1] <= '9'))
                return false;

            // If 'e' is the last Character
            if (i + 1 > input.length())
                return false;

            // if e is not followed either by
            // '+', '-' or a digit
            if (input[i + 1] != '+' && input[i + 1] != '-' && (input[i + 1] >= '0' && input[i] <= '9'))
                return false;
        }
    }

    /* If the string skips all above cases, then
    it is numeric*/
    return true;
}

static std::string normalize_whitespaces(const std::string_view &invecj)
{
    std::string outvecj;
    outvecj.reserve(invecj.size());
    int last_whitespace_counter = 1;
    for (size_t i = 0; i < invecj.size(); i++)
    {
        if ((last_whitespace_counter == 0) && ((invecj[i] == ' ') || (invecj[i] == '\t') || (invecj[i] == '\n') ||
                                               (invecj[i] == '\r') || (invecj[i] == '\v') || (invecj[i] == '\f')))
        {
            outvecj += ' ';
            last_whitespace_counter += 1;
            continue;
        }
        else if ((last_whitespace_counter != 0) && ((invecj[i] == ' ') || (invecj[i] == '\t') || (invecj[i] == '\n') ||
                                                    (invecj[i] == '\r') || (invecj[i] == '\v') || (invecj[i] == '\f')))
        {
            last_whitespace_counter += 1;
            continue;
        }
        last_whitespace_counter = 0;
        outvecj += invecj[i];
    }
    if ((!outvecj.empty()) && (outvecj.back() == ' '))
    {
        outvecj.resize(outvecj.size() - 1);
    }

    return outvecj;
}
std::string remove_whitespaces(const std::string_view invecj)
{
    std::string outvecj;
    outvecj.reserve(invecj.size());
    for (size_t i = 0; i < invecj.size(); i++)
    {
        if ((invecj[i] == ' ') || (invecj[i] == '\t') || (invecj[i] == '\n') || (invecj[i] == '\r') ||
            (invecj[i] == '\v') || (invecj[i] == '\f'))
        {
            continue;
        }
        outvecj += invecj[i];
    }

    return outvecj;
}

std::string to_padded(int length, char pad, const std::string_view inStr)
{
    std::string outstrcpp(length, pad);
    if (inStr.empty())
    {
        return outstrcpp;
    }
    for (int i = 0; i < length; i++)
    {
        if (i >= (int)inStr.length())
        {
            break;
        }
        outstrcpp[i] = inStr[i];
    }
    return outstrcpp;
}

constexpr std::array<uint8_t, 256> array_ascii_replace_non_alphanumeric = {
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  32,  32,  32,  32,  32,  32,  32,  65,
    66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,
    88,  89,  90,  32,  32,  32,  32,  32,  32,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32};

constexpr std::array<uint8_t, 256> array_ascii_replace_non_printable = {
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,
    44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,
    66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,
    88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
    32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,  32};
constexpr std::array<uint8_t, 256> array_ascii_replace_non_numeric = {
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 0-15
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 16-31
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 32-47
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57,                         // 48-57 (digits '0'-'9')
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 58-73
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 74-89
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 90-105
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 106-121
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 122-137
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 138-153
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 154-169
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 170-185
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 186-201
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 202-217
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 218-233
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32  // 234-255
};

std::string replace_fu(const std::string_view s, int op)
{
    std::string outv;
    outv.reserve(s.size());
    auto siterstart = s.begin();
    auto siterend = s.end();
    if (op == 0)
    {
        while (siterstart != siterend)
        {
            outv += (array_ascii_replace_non_alphanumeric[*siterstart++]);
        }
    }
    else if (op == 1)
    {
        while (siterstart != siterend)
        {
            outv += (array_ascii_replace_non_printable[*siterstart++]);
        }
    }
    else if (op == 2)
    {
        while (siterstart != siterend)
        {
            outv += (array_ascii_replace_non_numeric[*siterstart++]);
        }
    }
    return outv;
}
static constexpr int calc_best_substring_distance(const std::string_view strings_height_iter_start,
                                                  const std::string_view strings_width_iter_start, int op = 0)
{
    if ((strings_height_iter_start.size() == 0) && (strings_width_iter_start.size() == 0))
    {
        return 0;
    }
    if ((strings_height_iter_start.size() == 0) && (strings_width_iter_start.size() != 0))
    {
        if (op == 0 || op == 2 || op == 3)
        {
            return strings_width_iter_start.size();
        }
        else if (op == 1)
        {
            return 0;
        }
    }
    if ((strings_width_iter_start.size() == 0) && (strings_height_iter_start.size() != 0))
    {
        if (op == 0 || op == 2 || op == 3)
        {
            return strings_height_iter_start.size();
        }
        else if (op == 1)
        {
            return 0;
        }
    }
    if ((strings_height_iter_start.size() == strings_width_iter_start.size()) &&
        (strings_height_iter_start[0] == strings_width_iter_start[0]) &&
        (strings_height_iter_start == (strings_width_iter_start)))
    {
        if (op == 0 || op == 2 || op == 3)
        {
            return 0;
        }
        else if (op == 1)
        {
            return (int)strings_height_iter_start.size();
        }
    }

    int len_string_width = (int)strings_width_iter_start.size();
    int a_len;
    int b_len;
    int i;
    float a_switched;
    int len_string_height = (int)strings_height_iter_start.size();
    std::string_view astringviewtemp;
    std::string_view bstringviewtemp;
    int result = MAX_32BIT_INT;
    int tmpresult;
    int ii;
    if (len_string_width > len_string_height)
    {
        a_len = len_string_height;
        b_len = len_string_width;
        astringviewtemp = (strings_height_iter_start);
        bstringviewtemp = (strings_width_iter_start);
        a_switched = -1;
    }
    else
    {
        a_len = len_string_width;
        b_len = len_string_height;
        astringviewtemp = (strings_width_iter_start);
        bstringviewtemp = (strings_height_iter_start);
        a_switched = 1;
    }
    if (op == 0)
    {

        result = b_len;
        for (ii = 0; ii <= b_len - a_len; ii++)
        {
            tmpresult = b_len - a_len;
            for (i = 0; i <= b_len; i++)
            {

                if (i >= a_len)
                {
                    break;
                }
                if (bstringviewtemp[i + ii] != astringviewtemp[i])
                {
                    tmpresult++;
                }
            }
            if (tmpresult < result)
            {
                result = tmpresult;
            }
        }
    }
    else if (op == 1)
    {
        result = 0;
        int string_difference = b_len - a_len;
        for (int offsetindex = 0; offsetindex < string_difference; offsetindex++)
        {
            tmpresult = 0;
            for (int stringidex = 0; stringidex < a_len; stringidex++)
            {
                tmpresult += ((astringviewtemp[stringidex] == bstringviewtemp[stringidex + offsetindex]) ? 1 : 0);
            }
            if (tmpresult > result)
            {
                result = tmpresult;
            }
        }
    }
    else if (op == 2 || op == 3)
    {
        result = 0;
        int result1 = 0;

        int string_difference = b_len - a_len;
        int last_best_index = 0;
        int offsetindex;
        int tmpresult_tmp;
        int stringidex;
        for (offsetindex = 0; offsetindex < string_difference; offsetindex++)
        {
            tmpresult = 0;
            tmpresult_tmp = 0;
            for (stringidex = 0; stringidex < a_len; stringidex++)
            {
                if ((astringviewtemp[stringidex] != bstringviewtemp[stringidex + offsetindex]))
                {
                    if (tmpresult_tmp > tmpresult)
                    {
                        tmpresult = tmpresult_tmp;
                    }
                    tmpresult_tmp = 0;
                }
                else
                {
                    tmpresult_tmp++;
                    last_best_index = stringidex + offsetindex;
                }
            }
            if (tmpresult > result)
            {
                result = tmpresult;
            }
            if (result == a_len)
            {
                break;
            }
        }

        if (op == 3)
        {
            // last_best_index1 = 0;
            bool stop_now = false;
            for (offsetindex = 0; offsetindex < string_difference && !stop_now; offsetindex++)
            {
                tmpresult = 0;
                tmpresult_tmp = 0;

                for (stringidex = a_len - 1; stringidex >= 0; stringidex--)
                {

                    if ((astringviewtemp[stringidex] != bstringviewtemp[stringidex + offsetindex]))
                    {
                        if (tmpresult_tmp > tmpresult)
                        {
                            tmpresult = tmpresult_tmp;
                        }
                        tmpresult_tmp = 0;
                    }

                    else
                    {
                        tmpresult_tmp++;
                    }
                    if (stringidex + offsetindex == last_best_index)
                    {
                        stop_now = true;
                        break;
                    }
                }
                if (tmpresult > result1)
                {
                    result1 = tmpresult;
                }
                if (result1 == a_len)
                {
                    break;
                }
            }
        }
        result += result1;
    }
    return result;
}
void static boyer_moore_horspool_searcher_all(const std::string_view in, const std::string_view needle,
                                              std::vector<size_t> &results)
{
    auto in_end = in.end();
    size_t offset = 0;
    for (;;)
    {
        auto it = std::search(in.begin() + offset, in.end(),
                              std::boyer_moore_horspool_searcher(needle.begin(), needle.end()));
        if (it != in.end())
        {
            results.emplace_back(it - in.begin());
            offset = it - in.begin() + 1;
        }
        else
        {
            break;
        }
    }
}

} // namespace pystring
namespace
{
std::string convert_mem_address_to_string(size_t startptr, size_t length)
{
    char *cptr = (char *)startptr;
    char *endptr = cptr + length;
    std::string newstr;
    newstr.reserve(length);
    while (cptr != endptr)
    {
        newstr += *cptr++;
    }
    return newstr;
}
std::string string_from_sv_vector(const std::vector<std::string_view> &v)
{
    size_t totalsize = 0;
    auto begin_iter = v.begin();
    auto end_iter = v.end();
    std::string tmpstr;
    while (begin_iter != end_iter)
    {
        totalsize += begin_iter->size();
        begin_iter++;
    }
    tmpstr.reserve(totalsize);
    begin_iter = v.begin();
    while (begin_iter != end_iter)
    {
        tmpstr += std::string(*begin_iter);
        begin_iter++;
    }
    return tmpstr;
}
std::string string_from_s_vector(const std::vector<std::string> &v)
{
    size_t totalsize = 0;
    auto begin_iter = v.begin();
    auto end_iter = v.end();
    std::string tmpstr;
    while (begin_iter != end_iter)
    {
        totalsize += (*begin_iter).size();
        begin_iter++;
    }
    tmpstr.reserve(totalsize);
    begin_iter = v.begin();
    while (begin_iter != end_iter)
    {
        tmpstr += *begin_iter;
        begin_iter++;
    }
    return tmpstr;
}

std::string string_from_constchar_array(const char *v[], size_t size)
{
    std::string tmpstr;
    tmpstr.reserve(size * 20);
    for (size_t i = 0; i < size; i++)
    {
        tmpstr += std::string(v[i]);
    }
    return tmpstr;
}
std::string string_from_char_vector(const std::vector<char> &v)
{
    auto begin_iter = v.begin();
    auto end_iter = v.end();
    std::string tmpstr;
    tmpstr.reserve(v.size());
    while (begin_iter != end_iter)
    {
        tmpstr += *begin_iter;
        begin_iter++;
    }
    return tmpstr;
}

std::string string_from_char_ptr_vector(const std::vector<char *> &v)
{
    auto begin_iter = v.begin();
    auto end_iter = v.end();
    std::string tmpstr;
    tmpstr.reserve(v.size());
    while (begin_iter != end_iter)
    {
        tmpstr += *(*begin_iter);
        begin_iter++;
    }
    return tmpstr;
}

// Reset
static constexpr std::string_view Color_Off = "\033[0m"; // Text Reset
// Regular Colors
static constexpr std::string_view Black = "\033[0;30m";  // Black
static constexpr std::string_view Red = "\033[0;31m";    // Red
static constexpr std::string_view Green = "\033[0;32m";  // Green
static constexpr std::string_view Yellow = "\033[0;33m"; // Yellow
static constexpr std::string_view Blue = "\033[0;34m";   // Blue
static constexpr std::string_view Purple = "\033[0;35m"; // Purple
static constexpr std::string_view Cyan = "\033[0;36m";   // Cyan
static constexpr std::string_view White = "\033[0;37m";  // White
// Bold
static constexpr std::string_view BBlack = "\033[1;30m";  // Black
static constexpr std::string_view BRed = "\033[1;31m";    // Red
static constexpr std::string_view BGreen = "\033[1;32m";  // Green
static constexpr std::string_view BYellow = "\033[1;33m"; // Yellow
static constexpr std::string_view BBlue = "\033[1;34m";   // Blue
static constexpr std::string_view BPurple = "\033[1;35m"; // Purple
static constexpr std::string_view BCyan = "\033[1;36m";   // Cyan
static constexpr std::string_view BWhite = "\033[1;37m";  // White
// Underline
static constexpr std::string_view UBlack = "\033[4;30m";  // Black
static constexpr std::string_view URed = "\033[4;31m";    // Red
static constexpr std::string_view UGreen = "\033[4;32m";  // Green
static constexpr std::string_view UYellow = "\033[4;33m"; // Yellow
static constexpr std::string_view UBlue = "\033[4;34m";   // Blue
static constexpr std::string_view UPurple = "\033[4;35m"; // Purple
static constexpr std::string_view UCyan = "\033[4;36m";   // Cyan
static constexpr std::string_view UWhite = "\033[4;37m";  // White
// Background
static constexpr std::string_view On_Black = "\033[0;30m\033[40m";  // Black
static constexpr std::string_view On_Red = "\033[0;30m\033[41m";    // Red
static constexpr std::string_view On_Green = "\033[0;30m\033[42m";  // Green
static constexpr std::string_view On_Yellow = "\033[0;30m\033[43m"; // Yellow
static constexpr std::string_view On_Blue = "\033[0;30m\033[44m";   // Blue
static constexpr std::string_view On_Purple = "\033[0;30m\033[45m"; // Purple
static constexpr std::string_view On_Cyan = "\033[0;30m\033[46m";   // Cyan
static constexpr std::string_view On_White = "\033[0;30m\033[47m";  // White
// High Intensity
static constexpr std::string_view IBlack = "\033[0;90m";  // Black
static constexpr std::string_view IRed = "\033[0;91m";    // Red
static constexpr std::string_view IGreen = "\033[0;92m";  // Green
static constexpr std::string_view IYellow = "\033[0;93m"; // Yellow
static constexpr std::string_view IBlue = "\033[0;94m";   // Blue
static constexpr std::string_view IPurple = "\033[0;95m"; // Purple
static constexpr std::string_view ICyan = "\033[0;96m";   // Cyan
static constexpr std::string_view IWhite = "\033[0;97m";  // White
// Bold High Intensty
static constexpr std::string_view BIBlack = "\033[1;90m";  // Black
static constexpr std::string_view BIRed = "\033[1;91m";    // Red
static constexpr std::string_view BIGreen = "\033[1;92m";  // Green
static constexpr std::string_view BIYellow = "\033[1;93m"; // Yellow
static constexpr std::string_view BIBlue = "\033[1;94m";   // Blue
static constexpr std::string_view BIPurple = "\033[1;95m"; // Purple
static constexpr std::string_view BICyan = "\033[1;96m";   // Cyan
static constexpr std::string_view BIWhite = "\033[1;97m";  // White
// High Intensty backgrounds
static constexpr std::string_view On_IBlack = "\033[0;100m";  // Black
static constexpr std::string_view On_IRed = "\033[0;101m";    // Red
static constexpr std::string_view On_IGreen = "\033[0;102m";  // Green
static constexpr std::string_view On_IYellow = "\033[0;103m"; // Yellow
static constexpr std::string_view On_IBlue = "\033[0;104m";   // Blue
static constexpr std::string_view On_IPurple = "\033[10;95m"; // Purple
static constexpr std::string_view On_ICyan = "\033[0;106m";   // Cyan
static constexpr std::string_view On_IWhite = "\033[0;107m";  // White
} // namespace

namespace pyindex
{
using PY_RANGE = std::pair<int, int>;
using PY_INT_VEC = std::vector<int>;
using PY_SIZE_T = size_t;
using PY_CHR_PTR_LIST = std::pair<PY_INT_VEC, std::string>;
using PY_CHAR_VEC = std::vector<char>;
using PY_TRANSLATION_TABLE = std::unordered_map<unsigned char, unsigned char>;
using PY_STRING_VEC = std::vector<std::string>;
using PY_STRING_DICT_UNORDERED = std::unordered_map<std::string, std::string>;
using PY_STRING_DICT = std::map<std::string, std::string>;
struct PY_SLICE
{
    int start;
    int end;
    int step;
};

} // namespace pyindex

namespace strsorters
{
static void length_sorter(std::vector<int> &idx, std::vector<std::string_view> &v)
{
    std::stable_sort(idx.begin(), idx.end(), [&v](int i1, int i2) { return v[i1].size() < v[i2].size(); });
}
static void alphabet_sorter(std::vector<int> &idx, std::vector<std::string_view> &v)
{
    std::stable_sort(idx.begin(), idx.end(), [&v](int i1, int i2) { return v[i1] < v[i2]; });
}

static void alphabet_sorter_ignore_case(std::vector<int> &idx, std::vector<std::string_view> &v)
{
    std::vector<std::string> v2;
    v2.reserve(v.size());
    for_each(v.begin(), v.end(), [&v2](const auto &s) { v2.push_back(pystring::lower(s)); });
    std::stable_sort(idx.begin(), idx.end(), [&v2](int i1, int i2) { return v2[i1] < v2[i2]; });
}

static void alphabet_sorter_last_letter(std::vector<int> &idx, std::vector<std::string_view> &v)
{
    std::vector<std::string> v2{v.begin(), v.end()};
    std::vector<std::string>::iterator strings_height_iter_start = v2.begin();
    std::vector<std::string>::iterator strings_height_iter_end = v2.end();
    while (strings_height_iter_start != strings_height_iter_end)
    {
        std::reverse((char *)&(strings_height_iter_start->data()[0]),
                     (char *)&(strings_height_iter_start->data()[0]) + strings_height_iter_start->size());
        strings_height_iter_start++;
    }
    std::stable_sort(idx.begin(), idx.end(), [&v2](int i1, int i2) { return v2[i1] < v2[i2]; });
}
static void alphabet_sorter_last_letter_ignore_case(std::vector<int> &idx, std::vector<std::string_view> &v)
{
    std::vector<std::string> v2;
    v2.reserve(v.size());
    for_each(v.begin(), v.end(), [&v2](const auto &s) { v2.push_back(pystring::lower(s)); });
    std::vector<std::string>::iterator strings_height_iter_start = v2.begin();
    std::vector<std::string>::iterator strings_height_iter_end = v2.end();
    while (strings_height_iter_start != strings_height_iter_end)
    {
        std::reverse((char *)&(strings_height_iter_start->data()[0]),
                     (char *)&(strings_height_iter_start->data()[0]) + strings_height_iter_start->size());
        strings_height_iter_start++;
    }
    std::stable_sort(idx.begin(), idx.end(), [&v2](int i1, int i2) { return v2[i1] < v2[i2]; });
}

static void alphabet_sorter_ignore_case_average(std::vector<int> &idx, std::vector<std::string_view> &v)
{
    std::vector<float> v2;
    v2.reserve(v.size());
    for_each(v.begin(), v.end(), [&v2](const auto &s) {
        std::string tmp = pystring::lower(s);
        float tmpvalfloat = 0;
        for (int i = 0; i < tmp.size(); i++)
        {
            tmpvalfloat += tmp[i];
        }
        if (tmp.size() > 0)
        {
            tmpvalfloat /= (float)tmp.size();
        }
        v2.push_back(tmpvalfloat);
    });
    std::stable_sort(idx.begin(), idx.end(), [&v2](int i1, int i2) { return v2[i1] < v2[i2]; });
}

} // namespace strsorters

namespace stringmatch
{
static int strpos(char *haystackx, char *needle)
{
    char *p = strstr(haystackx, needle);
    if (p)
        return p - haystackx;
    return -1;
}
static bool is_substring(const std::string &haystack, const std::string &needle)
{
    char *p = ((char *)(haystack.c_str()));
    char *searchfor = ((char *)(needle.c_str()));
    return strpos(p, searchfor) > -1;
}
static std::vector<size_t> find_all_indices_strstr(const std::string &haystack, const std::string &needle)
{
    std::vector<size_t> indices;
    char *p = ((char *)(haystack.c_str()));
    char *searchfor = ((char *)(needle.c_str()));
    int pos = 0;
    int offset = 0;
    for (;;)
    {
        pos = strpos(p + offset, searchfor);
        if (pos == -1)
            break;
        offset += pos + 1;
        indices.emplace_back(offset - 1);
    }
    return indices;
}
static std::unordered_map<std::string, std::vector<size_t>> find_all_indices_multi_strstr(
    const std::string &haystack, const std::vector<std::string> &needles)
{
    std::unordered_map<std::string, std::vector<size_t>> indicesmap;
    indicesmap.reserve(needles.size());
    size_t address_of_haystack = (size_t)(haystack.c_str());
    char *p = (char *)address_of_haystack;
    for (const auto &needle : needles)
    {
        indicesmap[needle].reserve(16);
        p = (char *)address_of_haystack;
        char *searchfor = ((char *)((size_t)(needle.c_str())));
        int pos = 0;
        int offset = 0;
        for (;;)
        {
            pos = strpos(p + offset, searchfor);
            if (pos == -1)
                break;
            offset += pos + 1;
            indicesmap[needle].emplace_back(offset - 1);
        }
    }
    return indicesmap;
}
static std::vector<size_t> find_all_indices_strfind(const std::string &haystack, const std::string &needle)
{
    std::vector<size_t> indices;
    size_t offset = 0;
    size_t newoffset = 0;
    size_t maxlen = haystack.size();
    for (;;)
    {
        newoffset = haystack.substr(offset, maxlen).find(needle);
        if (newoffset == std::string::npos)
            break;
        offset += newoffset;
        indices.push_back(offset);
        maxlen = haystack.size() - offset - needle.size();
        offset += needle.size();
    }
    return indices;
}
static std::unordered_map<std::string, std::vector<size_t>> find_all_indices_multi_strfind(
    const std::string &haystack, const std::vector<std::string> &needles)
{
    std::unordered_map<std::string, std::vector<size_t>> indicesmap;
    indicesmap.reserve(needles.size());
    for (auto &needle : needles)
    {
        indicesmap[needle] = find_all_indices_strfind(haystack, needle);
    }
    return indicesmap;
}
static std::vector<size_t> find_all_indices_strviewfind(const std::string_view haystack, const std::string_view needle)
{
    std::vector<size_t> indices;
    size_t offset = 0;
    size_t newoffset = 0;
    size_t maxlen = haystack.size();
    for (;;)
    {
        newoffset = haystack.substr(offset, maxlen).find(needle);
        if (newoffset == std::string::npos)
            break;
        offset += newoffset;
        indices.push_back(offset);
        maxlen = haystack.size() - offset - needle.size();
        offset += needle.size();
    }
    return indices;
}
static std::unordered_map<std::string, std::vector<size_t>> find_all_indices_multi_strviewfind(
    const std::string_view haystack, const std::vector<std::string> &needles)
{
    std::unordered_map<std::string, std::vector<size_t>> indicesmap;
    indicesmap.reserve(needles.size());
    for (auto &needle : needles)
    {
        indicesmap[needle] = find_all_indices_strviewfind(haystack, needle);
    }
    return indicesmap;
}

} // namespace stringmatch
namespace py_advanced_split_and_insert
{
static std::string insert_strings_at_indices(std::vector<std::pair<int, std::string>> &indicies_and_strings,
                                             const std::string_view mystring)
{
    std::sort(indicies_and_strings.begin(), indicies_and_strings.end());
    std::string resultstring;
    resultstring.reserve(mystring.size() * 2);
    int nextindex = 0;
    for (const auto &item : indicies_and_strings)
    {
        int wheretoinsert = item.first;
        if (wheretoinsert > 0)
        {
            resultstring.append(mystring.substr(nextindex, wheretoinsert - nextindex));
            nextindex = wheretoinsert;
        }
        resultstring.append(item.second);
    }
    if (nextindex < mystring.size() - 1)
    {
        resultstring.append(mystring.substr(nextindex, MAX_32BIT_INT));
    }
    return resultstring;
}
static void split_at_string(const std::string_view haystack, const std::string_view splitat,
                            std::vector<std::string> &resultssplitsx)
{
    size_t splitatlen = splitat.size();
    auto allresults = stringmatch::find_all_indices_strviewfind(haystack, splitat);
    std::unordered_set<size_t> unorderedsetresults;
    unorderedsetresults.reserve(allresults.size() * splitatlen * 2);
    resultssplitsx.reserve(unorderedsetresults.size());
    size_t resultsplitcounter = 0;
    for (const auto &result : allresults)
    {
        std::string resultsplitsub;
        resultsplitsub.reserve(haystack.size());
        resultssplitsx.emplace_back(std::move(resultsplitsub));
        for (int i = 0; i < splitatlen; i++)
        {
            unorderedsetresults.insert(result + i);
        }
    }
    bool last_was_split = false;
    size_t oldindex = 0;
    size_t vectorindex = 0;
    for (size_t i = 0; i < haystack.size(); i++)
    {
        if (unorderedsetresults.contains(i))
        {
            if ((i - oldindex) > 1)
            {
                vectorindex++;
            }
            oldindex = i;
            continue;
        }
        if (vectorindex >= resultssplitsx.size())
        {
            break;
        }
        resultssplitsx[vectorindex] += haystack[i];
    }
}
static void split_at_multi_string(const std::string_view haystack, std::vector<std::string> &splitats,
                                  std::vector<std::string> &resultssplitsx)
{
    std::unordered_set<size_t> unorderedsetresults;
    for (size_t i = 0; i < splitats.size(); i++)
    {
        std::string_view splitat = splitats[i];
        size_t splitatlen = splitat.size();
        auto allresults = stringmatch::find_all_indices_strviewfind(haystack, splitat);
        unorderedsetresults.reserve(allresults.size() * splitatlen * 2);
        resultssplitsx.reserve(unorderedsetresults.size());
        size_t resultsplitcounter = 0;
        for (const auto &result : allresults)
        {
            std::string resultsplitsub;
            resultsplitsub.reserve(haystack.size());
            resultssplitsx.emplace_back(std::move(resultsplitsub));
            for (int ix = 0; ix < splitatlen; ix++)
            {
                unorderedsetresults.insert(result + ix);
            }
        }
    }
    bool last_was_split = false;
    size_t oldindex = 0;
    size_t vectorindex = 0;
    for (size_t i = 0; i < haystack.size(); i++)
    {
        if (unorderedsetresults.contains(i))
        {
            if ((i - oldindex) > 1)
            {
                vectorindex++;
            }
            oldindex = i;
            continue;
        }
        if (vectorindex >= resultssplitsx.size())
        {
            break;
        }
        resultssplitsx[vectorindex] += haystack[i];
    }
    auto itstart = resultssplitsx.begin();
    while (itstart != resultssplitsx.end())
    {
        if (itstart->empty())
        {
            itstart = resultssplitsx.erase(itstart);
        }
        itstart++;
    }
}
static std::map<size_t, std::string> split_at_multi_string_keep_strings(const std::string_view haystack,
                                                                        std::vector<std::string> &splitats)
{
    std::unordered_set<size_t> unorderedsetresults;
    auto mapresults_good = std::map<size_t, uint8_t>();
    auto mapresults_bad = std::map<size_t, uint8_t>();
    size_t oldindex = 0;
    auto mapresults_final = std::map<size_t, std::string>();
    auto mapresults_final_bad = std::map<size_t, std::string>();
    size_t currentindex = 0;
    size_t old_index = 0;
    size_t previous_value = 0;
    size_t insertindex = 0;
    size_t vectorindex = 0;
    for (size_t i = 0; i < splitats.size(); i++)
    {
        std::string_view splitat = splitats[i];
        size_t splitatlen = splitat.size();
        auto allresults = stringmatch::find_all_indices_strviewfind(haystack, splitat);
        unorderedsetresults.reserve(allresults.size() * splitatlen * 2);
        for (const auto &result : allresults)
        {
            for (int ix = 0; ix < splitatlen; ix++)
            {
                unorderedsetresults.insert(result + ix);
            }
        }
    }
    for (size_t i = 0; i < haystack.size(); i++)
    {
        if (unorderedsetresults.contains(i))
        {
            if ((i - oldindex) > 1)
            {
                vectorindex++;
            }
            mapresults_bad[i] = haystack[i];
            oldindex = i;
            continue;
        }
        mapresults_good[i] = haystack[i];
    }
    for (auto &item : mapresults_good)
    {
        currentindex = item.first;
        if (currentindex - old_index == 1)
        {
            mapresults_final[insertindex] += item.second;
        }
        else
        {
            insertindex = currentindex;
            mapresults_final[insertindex] = item.second;
        }
        old_index = currentindex;
    }
    currentindex = 0;
    old_index = 0;
    previous_value = 0;
    insertindex = 0;
    for (auto &item : mapresults_bad)
    {
        currentindex = item.first;
        if (currentindex - old_index == 1)
        {
            mapresults_final_bad[insertindex] += item.second;
        }
        else
        {
            insertindex = currentindex;
            mapresults_final_bad[insertindex] = item.second;
        }
        old_index = currentindex;
    }
    mapresults_final.merge(mapresults_final_bad);
    return mapresults_final;
}
} // namespace py_advanced_split_and_insert

class SliceObject
{
  public:
    SliceObject(std::vector<char *> slice) : _slice(slice), mysize(slice.size())
    {
    }
    std::vector<char *> _slice;
    size_t mysize;

    SliceObject &operator=(std::string other)
    {
        if (other.size() == 0)
        {
            throw std::invalid_argument("Other is empty");
        }
        for (size_t i = 0; i < mysize; i++)
        {
            *(this->_slice[i]) = (other[i % other.size()]);
        }
        return *this;
    }

    SliceObject &operator=(const std::string_view otherchr)
    {
        std::string other;
        other.append(otherchr);
        this->operator=(other);
        return *this;
    }

    SliceObject &operator=(char otherchr)
    {
        std::string other;
        other.append(1, otherchr);
        this->operator=(other);
        return *this;
    }

    SliceObject &operator=(const char *otherchr)
    {
        auto other = std::string(otherchr);
        this->operator=(other);
        return *this;
    }

    SliceObject &operator=(std::vector<char> otherchr)
    {
        std::string other;
        other.reserve(otherchr.size());
        for (size_t i = 0; i < otherchr.size(); i++)
        {
            other += otherchr[i];
        }
        this->operator=(other);
        return *this;
    }
    operator std::string() const
    {
        std::string other;
        other.reserve(mysize);
        for (size_t i = 0; i < mysize; i++)
        {
            other += (*(this->_slice[i]));
        }
        return other;
    }
};

class PythonString
{
  public:
    PythonString(SliceObject &&value) : _str(string_from_char_ptr_vector(value._slice)), _sv(_str)
    {
    }

    explicit PythonString(auto value)
        requires requires {
            std::is_arithmetic<decltype(value)>::value;
            !std::is_pointer<decltype(value)>::value;
        }
        : _str(std::to_string(value))
    {
        _sv = _str;
    }
    PythonString(const std::vector<char *> &charPtrVec)
    {
        size_t totalSize = 0;
        for (const char *c : charPtrVec)
        {
            totalSize += std::strlen(c);
        }
        _str.reserve(totalSize);

        for (const char *c : charPtrVec)
        {
            _str.append(c);
        }
        _sv = _str;
    }

    // Constructor with std::vector<std::unique_ptr<char>>
    PythonString(const std::vector<std::unique_ptr<char>> &uniquePtrVec)
    {
        size_t totalSize = 0;
        for (const auto &uptr : uniquePtrVec)
        {
            totalSize += std::strlen(uptr.get());
        }
        _str.reserve(totalSize);

        for (const auto &uptr : uniquePtrVec)
        {
            _str += *(uptr.get());
        }
        _sv = _str;
    }

    PythonString(const char *v) : _str(v)
    {
        _sv = _str;
    }
    PythonString(const std::string &v) : _str(v.begin(), v.end())
    {
        _sv = _str;
    }
    explicit PythonString(std::string &&v) : _str(std::move(v))
    {
        _sv = _str;
    }
    PythonString(const std::string_view v) : _str(v.begin(), v.end())
    {
        _sv = _str;
    }
    PythonString(pyindex::PY_SIZE_T startptr, pyindex::PY_SIZE_T length)
        : _str(convert_mem_address_to_string(startptr, length))
    {
        _sv = _str;
    }
    PythonString(void *v) : _str((char *)v)
    {
        _sv = _str;
    }
    PythonString(const pyindex::PY_STRING_VEC &stringx) : _str(string_from_s_vector(stringx))
    {
        _sv = _str;
    }
    PythonString(const pyindex::PY_CHAR_VEC &stringx) : _str(string_from_char_vector(stringx))
    {
        _sv = _str;
    }

    PythonString(std::vector<std::string_view> stringx) : _str(string_from_sv_vector(stringx))
    {
        _sv = _str;
    }

    PythonString(const char *v[], pyindex::PY_SIZE_T length) : _str(string_from_constchar_array(v, length))
    {
        _sv = _str;
    }

    PythonString(const PythonString &other) : _str(other._str.begin(), other._str.end())
    {
        _sv = _str;
    }
    PythonString(PythonString &&other)
    {
        _str = std::move(other._str);
        _sv = _str;
    }

    std::string _str;
    std::string_view _sv;

    PythonString &operator=(const std::string_view other)
    {
        this->_str = std::string(other);
        this->_sv = this->_str;
        return *this;
    }
    PythonString &operator=(const char *other)
    {
        this->_str = std::string(other);
        this->_sv = this->_str;
        return *this;
    }
    PythonString &operator=(const std::string &other)
    {
        this->_str = other;
        this->_sv = this->_str;
        return *this;
    }
    PythonString &operator=(const PythonString &other)
    {
        this->_str = other._str;
        this->_sv = this->_str;
        return *this;
    }
    template <typename T> PythonString join(std::vector<T> stringx)
    {
        std::string _tmpstr;
        _tmpstr.reserve(stringx.size() + (_sv.size() * stringx.size()));
        auto iterstart = stringx.begin();
        while (iterstart != (stringx.end() - 1))
        {
            _tmpstr += *iterstart++;
            _tmpstr += _sv;
        }
        _tmpstr += *iterstart;
        return PythonString(std::move(_tmpstr));
    }
    PythonString to_format_Black()
    {
        std::vector<std::string_view> stringx{Black, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_Red()
    {
        std::vector<std::string_view> stringx{Red, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_Green()
    {
        std::vector<std::string_view> stringx{Green, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_Yellow()
    {
        std::vector<std::string_view> stringx{Yellow, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_Blue()
    {
        std::vector<std::string_view> stringx{Blue, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_Purple()
    {
        std::vector<std::string_view> stringx{Purple, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_Cyan()
    {
        std::vector<std::string_view> stringx{Cyan, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_White()
    {
        std::vector<std::string_view> stringx{White, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BBlack()
    {
        std::vector<std::string_view> stringx{BBlack, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BRed()
    {
        std::vector<std::string_view> stringx{BRed, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BGreen()
    {
        std::vector<std::string_view> stringx{BGreen, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BYellow()
    {
        std::vector<std::string_view> stringx{BYellow, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BBlue()
    {
        std::vector<std::string_view> stringx{BBlue, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BPurple()
    {
        std::vector<std::string_view> stringx{BPurple, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BCyan()
    {
        std::vector<std::string_view> stringx{BCyan, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BWhite()
    {
        std::vector<std::string_view> stringx{BWhite, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_UBlack()
    {
        std::vector<std::string_view> stringx{UBlack, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_URed()
    {
        std::vector<std::string_view> stringx{URed, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_UGreen()
    {
        std::vector<std::string_view> stringx{UGreen, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_UYellow()
    {
        std::vector<std::string_view> stringx{UYellow, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_UBlue()
    {
        std::vector<std::string_view> stringx{UBlue, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_UPurple()
    {
        std::vector<std::string_view> stringx{UPurple, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_UCyan()
    {
        std::vector<std::string_view> stringx{UCyan, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_UWhite()
    {
        std::vector<std::string_view> stringx{UWhite, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_Black()
    {
        std::vector<std::string_view> stringx{On_Black, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_Red()
    {
        std::vector<std::string_view> stringx{On_Red, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_Green()
    {
        std::vector<std::string_view> stringx{On_Green, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_Yellow()
    {
        std::vector<std::string_view> stringx{On_Yellow, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_Blue()
    {
        std::vector<std::string_view> stringx{On_Blue, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_Purple()
    {
        std::vector<std::string_view> stringx{On_Purple, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_Cyan()
    {
        std::vector<std::string_view> stringx{On_Cyan, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_White()
    {
        std::vector<std::string_view> stringx{On_White, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_IBlack()
    {
        std::vector<std::string_view> stringx{IBlack, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_IRed()
    {
        std::vector<std::string_view> stringx{IRed, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_IGreen()
    {
        std::vector<std::string_view> stringx{IGreen, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_IYellow()
    {
        std::vector<std::string_view> stringx{IYellow, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_IBlue()
    {
        std::vector<std::string_view> stringx{IBlue, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_IPurple()
    {
        std::vector<std::string_view> stringx{IPurple, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_ICyan()
    {
        std::vector<std::string_view> stringx{ICyan, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_IWhite()
    {
        std::vector<std::string_view> stringx{IWhite, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BIBlack()
    {
        std::vector<std::string_view> stringx{BIBlack, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BIRed()
    {
        std::vector<std::string_view> stringx{BIRed, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BIGreen()
    {
        std::vector<std::string_view> stringx{BIGreen, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BIYellow()
    {
        std::vector<std::string_view> stringx{BIYellow, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BIBlue()
    {
        std::vector<std::string_view> stringx{BIBlue, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BIPurple()
    {
        std::vector<std::string_view> stringx{BIPurple, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BICyan()
    {
        std::vector<std::string_view> stringx{BICyan, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_BIWhite()
    {
        std::vector<std::string_view> stringx{BIWhite, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_IBlack()
    {
        std::vector<std::string_view> stringx{On_IBlack, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_IRed()
    {
        std::vector<std::string_view> stringx{On_IRed, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_IGreen()
    {
        std::vector<std::string_view> stringx{On_IGreen, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_IYellow()
    {
        std::vector<std::string_view> stringx{On_IYellow, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_IBlue()
    {
        std::vector<std::string_view> stringx{On_IBlue, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_IPurple()
    {
        std::vector<std::string_view> stringx{On_IPurple, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_ICyan()
    {
        std::vector<std::string_view> stringx{On_ICyan, _str, Color_Off};
        return PythonString(stringx);
    }
    PythonString to_format_On_IWhite()
    {
        std::vector<std::string_view> stringx{On_IWhite, _str, Color_Off};
        return PythonString(stringx);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // operator overloading +
    PythonString operator+(const PythonString &other)
    {
        std::string temp = std::string(_sv.begin(), _sv.end());
        for (auto &c : other._str)
        {
            temp += c;
        }
        return PythonString(std::move(temp));
    }
    PythonString operator+(const std::string_view other)
    {
        std::string temp = std::string(_sv.begin(), _sv.end());
        temp.append(other);
        return PythonString(std::move(temp));
    }
    PythonString operator+(const std::string &other)
    {
        std::string temp = std::string(_sv.begin(), _sv.end());
        temp.append(other);
        return PythonString(std::move(temp));
    }
    PythonString operator+(const char *other)
    {
        std::string temp = std::string(_sv.begin(), _sv.end());
        temp.append(other);
        return PythonString(std::move(temp));
    }
    PythonString operator+(auto other)
    {
        std::string temp = std::string(_sv.begin(), _sv.end());
        temp.append(PythonString(other)._sv);
        return PythonString(std::move(temp));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    constexpr uint32_t calculate_hash()
    {
        return PythonString::calculate_string_hash_static(_sv);
    }

  private:
    void _check_if_start_end_valid(int start, int end)
    {
        if (start > end)
        {
            throw std::out_of_range("Start index cannot be greater than end index");
        }
    }
    int _get_real_index_or_raise(int index)
    {
        if (index < 0)
        {
            index = (int)_sv.size() + index;
        }
        if (index > (int)_sv.size() - 1)
        {
            throw std::out_of_range("Index out of range");
        }
        return index;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // // operator overloading *
  public:
    PythonString operator*(size_t n)
    {
        std::string tmp;
        tmp.reserve(_sv.size() * n);
        for (size_t i = 0; i < n; i++)
        {
            tmp.append(_sv);
        }
        return PythonString(std::move(tmp));
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    char &operator[](int index)
    {
        return _str[_get_real_index_or_raise(index)];
    }
    PythonString operator[](pyindex::PY_RANGE start_end)
    {
        int _start = _get_real_index_or_raise(start_end.first);
        int _end = _get_real_index_or_raise(start_end.second);
        _check_if_start_end_valid(_start, _end);
        std::string temp;
        temp.reserve(_end - _start);
        for (int i = _start; i < _end; i++)
        {
            temp += _str[i];
        }
        return PythonString(std::move(temp));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    SliceObject operator[](pyindex::PY_SLICE &&pyslice)
    {
        int start = _get_real_index_or_raise(pyslice.start);
        int end = _get_real_index_or_raise(pyslice.end);
        int step = pyslice.step;
        if ((start >= end) && (step > 0))
        {
            throw std::out_of_range("Start index cannot be greater than end index, "
                                    "when step is positive");
        }
        if ((end >= start) && (step < 0))
        {
            throw std::out_of_range("End index cannot be greater than start index, "
                                    "when step is negative");
        }
        if (step == 0)
        {
            step = 1;
        }
        std::vector<char *> ptrvec;
        if (start < end)
        {

            for (int i = start; i < end; i += step)
            {
                ptrvec.push_back((char *)&_str[i]);
            }
        }
        else
        {
            for (int i = start; i > end; i -= ::abs(step))
            {
                ptrvec.push_back((char *)&_str[i]);
            }
        }
        return SliceObject(ptrvec);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // char array
    std::vector<char> operator[](const pyindex::PY_INT_VEC start_end)
    {
        std::vector<char> temp;
        temp.reserve(start_end.size());
        for (const auto strindex : start_end)
        {
            int _index = strindex;
            try
            {
                _index = _get_real_index_or_raise(strindex);
            }
            catch (...)
            {
                continue;
            }
            temp.emplace_back(_str[_index]);
        }
        return temp;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    PythonString &reverse_inplace()
    {
        std::reverse((char *)&(_str.data()[0]), (char *)&(_str.data()[0]) + _str.size());
        return *this;
    }
    PythonString reverse()
    {
        std::string tmp{_sv.begin(), _sv.end()};
        std::reverse((char *)&(tmp.data()[0]), (char *)&(tmp.data()[0]) + tmp.size());
        return PythonString(std::move(tmp));
    }

    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Color print and color string
    PythonString print_green(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_IGreen() + "\n")._sv;
        return PythonString(_sv);
    }
    PythonString print_red(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_IRed() + "\n")._sv;
        return PythonString(_sv);
    }
    PythonString print_yellow(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_IYellow() + "\n")._sv;
        return PythonString(_sv);
    }
    PythonString print_cyan(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_ICyan() + "\n")._sv;
        return PythonString(_sv);
    }
    PythonString print_magenta(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_Purple() + "\n")._sv;
        return PythonString(_sv);
    }
    PythonString print_bg_green(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_On_Green() + "\n")._sv;
        return PythonString(_sv);
    }
    PythonString print_bg_red(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_On_Red() + "\n")._sv;
        return PythonString(_sv);
    }
    PythonString print_bg_yellow(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_On_Yellow() + "\n")._sv;
        return PythonString(_sv);
    }
    PythonString print_bg_cyan(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_On_Cyan() + "\n")._sv;

        return PythonString(_sv);
    }
    PythonString print_bg_magenta(const std::string &prefix = "")
    {
        std::cout << (PythonString(prefix) + to_format_On_Purple() + "\n")._sv;
        return PythonString(_sv);
    }
    void print_to_std_err()
    {
        std::cerr << std::format("{}{}{}{}\n", IWhite, On_Red, _sv, Color_Off);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<PythonString> _return_pystring_vec(std::vector<std::string> &&tmpvec)
    {
        std::vector<PythonString> returnvec;
        returnvec.reserve(tmpvec.size());
        for (size_t i = 0; i < tmpvec.size(); i++)
        {
            returnvec.emplace_back(PythonString{std::move(tmpvec[i])});
        }
        return returnvec;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  str.split(sep, maxsplit)
    std::vector<PythonString> split(const PythonString &sep, int maxsplit)
    {
        return split(sep._sv, maxsplit);
    }
    std::vector<PythonString> split(const std::string &sep, int maxsplit)
    {
        const std::string_view tmpstring_view = std::string_view(sep);
        return split(tmpstring_view, maxsplit);
    }
    std::vector<PythonString> split(std::string &&sep, int maxsplit)
    {
        const std::string_view tmpstring_view = std::string_view(sep);
        return split(tmpstring_view, maxsplit);
    }
    std::vector<PythonString> split(std::string &&sep)
    {
        const std::string_view tmpstring_view = std::string_view(sep);
        return split(tmpstring_view, -1);
    }
    std::vector<PythonString> split(const char *sep, int maxsplit)
    {
        auto tmpstring = std::string(sep);
        const std::string_view tmpstring_view = std::string_view(tmpstring);
        return split(std::string_view(tmpstring), maxsplit);
    }
    std::vector<PythonString> split(const std::string_view sep, int maxsplit)
    {
        std::vector<std::string> result;
        pystring::split(_sv, result, sep, maxsplit);
        return _return_pystring_vec(std::move(result));
    }
    std::vector<PythonString> split(int maxsplit)
    {
        const std::string_view emptystring = "";
        return split(emptystring, maxsplit);
    }
    template <typename T> std::vector<PythonString> split(const T &sep)
    {
        return split(sep, -1);
    }
    std::vector<PythonString> split()
    {
        const std::string_view emptystring = "";
        return split(emptystring, -1);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // operator overloading / split at word
    std::vector<PythonString> operator/(PythonString &&other)
    {
        return (split(other._sv, -1));
    }
    std::vector<PythonString> operator/(const PythonString &other)
    {
        return (split(other._sv, -1));
    }
    std::vector<PythonString> operator/(const std::string_view other)
    {
        return (split(other, -1));
    }
    std::vector<PythonString> operator/(std::string &&other)
    {
        return (split(std::move(other), -1));
    }
    std::vector<PythonString> operator/(const std::string &other)
    {
        return (split(other, -1));
    }
    std::vector<PythonString> operator/(const char *other)
    {
        return (split(other, -1));
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 	////////////////////////////////////////////////////////////////////////////////////////////////////////
    // operator overloading, split at certain string size
    std::vector<PythonString> operator/(int n)
    {
        size_t _n = abs(n);
        if (n == 0)
        {
            _n = 1;
        }
        if (n > _sv.size())
        {
            n = (int)_sv.size();
        }
        std::vector<std::string> temp;
        temp.reserve(n);
        for (size_t i = 0; i < _sv.size(); i++)
        {
            if (i % _n == 0)
            {
                std::string newstring;
                newstring.reserve(_sv.size() / _n);
                temp.emplace_back(std::move(newstring));
            }
            temp[temp.size() - 1] += _sv[i];
        }
        return _return_pystring_vec(std::move(temp));
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator==(const PythonString &other) const
    {
        return _sv == other._sv;
    }
    bool operator!=(const PythonString &other) const
    {
        return _sv != other._sv;
    }
    bool operator<(const PythonString &other) const
    {
        return _sv < other._sv;
    }
    bool operator>(const PythonString &other) const
    {
        return _sv > other._sv;
    }
    bool operator>=(const PythonString &other) const
    {
        return _sv >= other._sv;
    }
    bool operator<=(const PythonString &other) const
    {
        return _sv <= other._sv;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convert to std::vector<uint8_t>, to be able to dump it as member of a
    // struct
    std::vector<uint8_t> convert_to_uint8(int size = -1)
    {
        size_t loopsize;
        std::vector<uint8_t> result;
        if (size <= 0)
        {
            loopsize = _sv.size();
        }
        else
        {
            loopsize = size;
        }
        if (loopsize > _sv.size())
        {
            loopsize = _sv.size();
        }
        result.resize(loopsize);
        for (size_t i = 0; i < loopsize; i++)
        {
            result[i] = (uint8_t)_sv[i];
        }
        return result;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool endswith(const std::string_view suffix, int start, int end)
    {

        auto suffix_size = (int)suffix.size();
        int substring_start = _get_real_index_or_raise(start);
        int substring_end = _get_real_index_or_raise(end);
        _check_if_start_end_valid(substring_start, substring_end);
        if (suffix_size > substring_end - substring_start)
        {
            return false;
        }
        const std::string_view substring = _sv.substr(substring_start, substring_end - substring_start + 1);
        if (substring[substring.size() - 1] != suffix[suffix_size - 1])
        {
            return false;
        }
        int lastcounter = 0;
        for (int i = suffix_size - 1; i >= 0; i--)
        {
            lastcounter++;
            if (substring[substring.size() - lastcounter] != suffix[i])
            {
                return false;
            }
        }
        return true;
    }
    bool endswith(const std::string &suffix, int start, int end)
    {
        return endswith(std::string_view(suffix), start, end);
    }
    bool endswith(const char *suffix, int start, int end)
    {
        return endswith(std::string(suffix), start, end);
    }
    bool endswith(const PythonString &suffix, int start, int end)
    {
        return endswith((suffix._sv), start, end);
    }
    template <typename T> bool endswith(const T &suffix)
    {
        return endswith(suffix, 0, -1);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool startswith(const std::string_view prefix, int start, int end)
    {

        auto prefix_size = (int)prefix.size();
        int substring_start = _get_real_index_or_raise(start);
        int substring_end = _get_real_index_or_raise(end);
        _check_if_start_end_valid(substring_start, substring_end);
        if (prefix_size > substring_end - substring_start)
        {
            return false;
        }
        const std::string_view substring = _sv.substr(substring_start, substring_end - substring_start + 1);
        for (int i = 0; i < prefix_size; i++)
        {
            if (substring[i] != prefix[i])
            {
                return false;
            }
        }
        return true;
    }
    bool startswith(const std::string &prefix, int start, int end)
    {
        return startswith(std::string_view(prefix), start, end);
    }
    bool startswith(const char *prefix, int start, int end)
    {
        return startswith(std::string(prefix), start, end);
    }
    bool startswith(const PythonString &prefix, int start, int end)
    {
        return startswith((prefix._sv), start, end);
    }
    template <typename T> bool startswith(const T &suffix)
    {
        return startswith(suffix, 0, -1);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.isalnum
    constexpr bool isspace()
    {
        return pystring::isspace(_sv);
    }
    constexpr bool isalnum()
    {
        return pystring::isalnum(_sv);
    }
    constexpr bool isalpha()
    {
        return pystring::isalpha(_sv);
    }
    constexpr bool isdigit()
    {
        return pystring::isdigit(_sv);
    }
    constexpr bool islower()
    {
        return pystring::islower(_sv);
    }
    constexpr bool istitle()
    {
        return pystring::istitle(_sv);
    }
    constexpr bool isupper()
    {
        return pystring::isupper(_sv);
    }
    bool isint()
    {
        return pystring::is_int_number(_sv);
    }
    bool isdecimal()
    {
        return pystring::is_int_number(_sv);
    }

    bool isfloat()
    {
        return pystring::is_float_number(_sv);
    }
    bool isbool()
    {
        return pystring::is_bool(_sv);
    }
    std::variant<double, int64_t> convert_to_number()
    {
        return pystring::to_float_or_int(_sv);
    }
    constexpr bool convert_to_bool()
    {
        if (pystring::is_bool(_sv))
        {
            if ((_sv == pystring::bool1) || (_sv == pystring::bool3) || (_sv == pystring::bool6) ||
                (_sv == pystring::bool7))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        return false;
    }

    constexpr bool is_ascii()
    {
        bool isasc = true;
        for (size_t i = 0; i < _sv.size(); i++)
        {
            if ((uint8_t)_sv.data()[i] > 127)
            {
                isasc = false;
                break;
            }
        }
        return isasc;
    }
    constexpr bool isprintable()
    {
        if (_sv.empty())
        {
            return true;
        }
        for (size_t i = 0; i < _sv.size(); i++)
        {
            if (!::isprint((int)_sv.data()[i]))
            {
                return false;
            }
        }
        return true;
    }

    int64_t convert_to_int_at_any_cost(int64_t nan_value)
    {
        std::variant<double, int64_t> tmpvalue = pystring::to_float_or_int(_sv);
        int64_t tmpnumber;
        try
        {
            tmpnumber = std::get<int64_t>(tmpvalue);
        }
        catch (...)
        {
            try
            {
                tmpnumber = (int64_t)(std::get<double>(tmpvalue));
            }
            catch (...)
            {
                // do nothing
            }
        }

        if (tmpnumber != 0)
        {
            return tmpnumber;
        }
        if ((tmpnumber == 0) && (pystring::check_if_string_is_valid_int_zero(_sv)))
        {
            return 0;
        }

        return nan_value;
    }
    double convert_to_double_at_any_cost(double nan_value)
    {
        std::variant<double, int64_t> tmpvalue = pystring::to_float_or_int(_sv);
        double tmpnumber;
        try
        {
            tmpnumber = std::get<double>(tmpvalue);
        }
        catch (...)
        {
            try
            {
                tmpnumber = (double)(std::get<int64_t>(tmpvalue));
            }
            catch (...)
            {
                // do nothing
            }
        }
        if (tmpnumber != 0)
        {
            return tmpnumber;
        }
        if ((tmpnumber == 0) && (pystring::check_if_string_is_valid_float_zero(_sv)))
        {
            return 0;
        }

        return nan_value;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // operator overloading % - wraps string at certain string size
    std::vector<PythonString> operator%(int n)
    {
        size_t _n = ::abs(n);
        return this->operator/(int(::ceil(_sv.size() / _n)));
    }
    std::vector<PythonString> operator%=(int n)
    {
        size_t _n = ::abs(n);
        auto tempvec = this->operator/(int(::ceil(_sv.size() / _n)));
        std::string newstring;
        newstring.reserve(size_t(double(_sv.size()) * 1.1));
        int loopcounter = 0;
        for (auto &i : tempvec)
        {
            newstring += i;
            if (loopcounter < tempvec.size() - 1)
            {
                newstring += '\n';
            }
            loopcounter++;
        }
        this->_str = std::move(newstring);
        this->_sv = this->_str;
        return tempvec;
    }
    explicit operator int64_t()
    {
        return this->convert_to_int_at_any_cost(9223372036854775807);
    }
    explicit operator int32_t()
    {
        return (int32_t)this->convert_to_int_at_any_cost(2147483647);
    }
    explicit operator double()
    {
        return this->convert_to_double_at_any_cost(9223372036854775807.0);
    }
    explicit operator float()
    {
        return (float)this->convert_to_double_at_any_cost(2147483647.0f);
    }

    explicit operator bool()
    {
        return this->convert_to_bool();
    }
    operator std::string() const
    {
        return std::string{_str.begin(), _str.end()};
    }
    operator std::string_view() const
    {
        return _sv;
    }
    operator std::vector<uint8_t>()
    {
        return this->convert_to_uint8(-1);
    }
    operator std::vector<std::string>()
    {
        std::vector<PythonString> tempvec = this->split(-1);
        std::vector<std::string> tempvec2;
        tempvec2.reserve(tempvec.size());
        for (auto &&i : tempvec)
        {
            tempvec2.emplace_back(i._str);
        }
        return tempvec2;
    }
    operator std::map<int, std::string>()
    {
        auto temp = this->split(-1);
        std::map<int, std::string> result;
        int i = 0;
        for (auto &&ii : temp)
        {
            result[i++] = ii._str;
        }
        return result;
    }
    operator std::unordered_map<int, std::string>()
    {
        auto temp = this->split(-1);
        std::unordered_map<int, std::string> result;
        int i = 0;
        for (auto &&ii : temp)
        {
            result[i++] = ii._str;
        }
        return result;
    }
    operator std::list<std::string>()
    {
        auto tempvec = this->split(-1);
        std::list<std::string> tempvec2;
        for (auto &&i : tempvec)
        {
            tempvec2.emplace_back(i._str);
        }
        return tempvec2;
    }
    operator std::set<std::string>()
    {
        auto tempvec = this->split(-1);
        std::set<std::string> tempvec2;
        for (auto &&i : tempvec)
        {
            tempvec2.insert(i._str);
        }
        return tempvec2;
    }
    operator std::unordered_set<std::string>()
    {
        auto tempvec = this->split(-1);
        std::unordered_set<std::string> tempvec2;
        for (auto &&i : tempvec)
        {
            tempvec2.insert(i._str);
        }
        return tempvec2;
    }
    operator const char *()
    {
        return _str.c_str();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  str.splitlines(sep, maxsplit)
    std::vector<PythonString> splitlines(bool keepends)
    {
        std::vector<std::string> result;
        pystring::splitlines(_sv, result, keepends);
        return _return_pystring_vec(std::move(result));
    }

    std::vector<PythonString> splitlines()
    {
        return splitlines(false);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.split(sep, maxsplit)
    std::vector<PythonString> rsplit(const PythonString &sep, int maxsplit)
    {
        return rsplit(sep._sv, maxsplit);
    }
    std::vector<PythonString> rsplit(const std::string &sep, int maxsplit)
    {
        return rsplit(std::string_view(sep), maxsplit);
    }
    std::vector<PythonString> rsplit(const char *sep, int maxsplit)
    {
        return rsplit(std::string_view(std::string(sep)), maxsplit);
    }
    std::vector<PythonString> rsplit(const std::string_view sep, int maxsplit)
    {
        std::vector<std::string> result;
        pystring::rsplit(_sv, result, sep, maxsplit);
        return _return_pystring_vec(std::move(result));
    }
    std::vector<PythonString> rsplit(int maxsplit)
    {
        return rsplit(pystring::emptystringview, maxsplit);
    }
    std::vector<PythonString> rsplit()
    {
        return rsplit(pystring::emptystringview, -1);
    }
    template <typename T> std::vector<PythonString> rsplit(const T &sep)
    {
        return rsplit(sep, -1);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.strip
    PythonString strip(const std::string_view chars)
    {
        return PythonString((pystring::strip(_sv, chars)));
    }
    PythonString strip(const std::string &chars)
    {
        return PythonString((pystring::strip(_sv, std::string_view(chars))));
    }
    PythonString strip(const char *chr)
    {
        auto tmpstring = std::string(chr);
        return PythonString((pystring::strip(_sv, std::string_view(tmpstring))));
    }
    PythonString strip(const PythonString &chars)
    {
        return PythonString((pystring::strip(_sv, chars._sv)));
    }
    PythonString strip(pyindex::PY_CHAR_VEC &chars)
    {
        auto tmppystring = PythonString(chars);
        return PythonString((pystring::strip(_sv, tmppystring._sv)));
    }
    PythonString strip()
    {
        return PythonString((pystring::strip(_sv, pystring::emptystringview)));
    }

    template <typename T> PythonString &strip_inplace(const T &chars)
    {
        this->operator=(strip(chars));
        return *this;
    }
    PythonString &strip_inplace()
    {
        this->operator=(strip(pystring::emptystringview));
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.rstrip
    PythonString rstrip(const std::string_view chars)
    {
        return PythonString((pystring::rstrip(_sv, chars)));
    }
    PythonString rstrip(const std::string &chars)
    {
        return PythonString((pystring::rstrip(_sv, std::string_view(chars))));
    }
    PythonString rstrip(const char *chr)
    {
        auto tmpstring = std::string(chr);
        return PythonString((pystring::rstrip(_sv, std::string_view(tmpstring))));
    }
    PythonString rstrip(const PythonString &chars)
    {
        return PythonString((pystring::rstrip(_sv, chars._sv)));
    }
    PythonString rstrip(pyindex::PY_CHAR_VEC &chars)
    {
        auto tmppystring = PythonString(chars);
        return PythonString((pystring::rstrip(_sv, tmppystring._sv)));
    }
    PythonString rstrip()
    {
        return PythonString((pystring::rstrip(_sv, pystring::emptystringview)));
    }

    template <typename T> PythonString &rstrip_inplace(const T &chars)
    {
        this->operator=(rstrip(chars));
        return *this;
    }
    PythonString &rstrip_inplace()
    {
        this->operator=(rstrip(pystring::emptystringview));
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.lstrip
    PythonString lstrip(const std::string_view chars)
    {
        return PythonString((pystring::lstrip(_sv, chars)));
    }
    PythonString lstrip(const std::string &chars)
    {
        return PythonString((pystring::lstrip(_sv, std::string_view(chars))));
    }
    PythonString lstrip(const char *chr)
    {
        auto tmpstring = std::string(chr);
        return PythonString((pystring::lstrip(_sv, std::string_view(tmpstring))));
    }
    PythonString lstrip(const PythonString &chars)
    {
        return PythonString((pystring::lstrip(_sv, chars._sv)));
    }
    PythonString lstrip(pyindex::PY_CHAR_VEC &chars)
    {
        auto tmppystring = PythonString(chars);
        return PythonString((pystring::lstrip(_sv, tmppystring._sv)));
    }
    PythonString lstrip()
    {
        return PythonString((pystring::lstrip(_sv, pystring::emptystringview)));
    }
    template <typename T> PythonString &lstrip_inplace(const T &chars)
    {
        this->operator=(lstrip(chars));
        return *this;
    }
    PythonString &lstrip_inplace()
    {
        this->operator=(lstrip(pystring::emptystringview));
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // method forwards
    size_t size()
    {
        return _str.size();
    }

    std::string to_cpp_string_copy()
    {
        return std::string(_sv.begin(), _sv.end());
    }

    std::string &to_cpp_string()
    {
        return _str;
    }
    const char *c_str()
    {
        return _str.c_str();
    }

    char *c_str_muteable()
    {
        return (char *)_str.c_str();
    }

    std::string_view::iterator begin()
    {
        return this->_sv.begin();
    }
    std::string_view::iterator end()
    {
        return this->_sv.end();
    }
    std::string_view::const_iterator cbegin() const
    {
        return this->_sv.cbegin();
    }
    std::string_view::const_iterator cend() const
    {
        return this->_sv.cend();
    }
    std::string_view::reverse_iterator rbegin()
    {
        return this->_sv.rbegin();
    }
    std::string_view::reverse_iterator rend()
    {
        return this->_sv.rend();
    }
    std::string_view::const_reverse_iterator crbegin() const
    {
        return this->_sv.rbegin();
    }
    std::string_view::const_reverse_iterator crend() const
    {
        return this->_sv.rend();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.translate
    auto static _create_translation_table(const std::unordered_map<unsigned char, unsigned char> &my_translation_table)
    {
        std::map<unsigned char, unsigned char> translation_table;
        for (unsigned char i = 0;; i++)
        {
            translation_table[i] = i;
            if (i == 255)
            {
                break;
            }
        }
        for (auto &i : my_translation_table)
        {
            translation_table[i.first] = i.second;
        }
        std::string translation_table_str(256, 0);
        for (const auto &i : translation_table)
        {
            translation_table_str[i.first] = i.second;
        }
        return translation_table_str;
    }
    PythonString translate(pyindex::PY_TRANSLATION_TABLE my_translation_table, const std::string &deletechars)
    {
        std::string my_translation_table_string = PythonString::_create_translation_table(my_translation_table);
        std::string resux = (pystring::translate((_sv), my_translation_table_string, deletechars));
        return PythonString(std::move(resux));
    }
    PythonString translate(pyindex::PY_TRANSLATION_TABLE my_translation_table, const std::string_view deletechars)
    {
        std::string my_translation_table_string = PythonString::_create_translation_table(my_translation_table);
        std::string resux = (pystring::translate((_sv), my_translation_table_string, deletechars));
        return PythonString(std::move(resux));
    }
    PythonString translate(pyindex::PY_TRANSLATION_TABLE my_translation_table, const char *deletechars)
    {
        std::string my_translation_table_string = PythonString::_create_translation_table(my_translation_table);
        std::string resux = (pystring::translate((_sv), my_translation_table_string, deletechars));
        return PythonString(std::move(resux));
    }
    PythonString translate(pyindex::PY_TRANSLATION_TABLE my_translation_table)
    {
        std::string my_translation_table_string = PythonString::_create_translation_table(my_translation_table);
        std::string resux = (pystring::translate((_sv), my_translation_table_string, pystring::emptystringview));
        return PythonString(std::move(resux));
    }

    template <typename T>
    PythonString &translate_inplace(pyindex::PY_TRANSLATION_TABLE my_translation_table, const T &deletechars)
    {
        this->operator=(translate(my_translation_table, deletechars));
        return *this;
    }
    PythonString &translate_inplace(pyindex::PY_TRANSLATION_TABLE my_translation_table)
    {
        this->operator=(translate(my_translation_table));
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.zfill
    PythonString zfill(int32_t width)
    {
        std::string resux = (pystring::zfill((_sv), width));
        return PythonString(std::move(resux));
    }

    PythonString &zfill_inplace(int32_t width)
    {
        this->operator=(zfill(width));
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    PythonString rjust(int32_t width, char fillchar)
    {
        std::string resux = (pystring::rjust(_sv, width, fillchar));
        return PythonString(std::move(resux));
    }
    PythonString rjust(int32_t width, const std::string &fillchar)
    {
        return rjust(width, fillchar[0]);
    }
    PythonString rjust(int32_t width)
    {
        return rjust(width, ' ');
    }

    template <typename T> PythonString &rjust_inplace(int32_t width, T fillchar)
    {
        this->operator=(rjust(width, fillchar));
        return *this;
    }
    PythonString &rjust_inplace(int32_t width)
    {
        this->operator=(rjust(width, ' '));
        return *this;
    }

    PythonString ljust(int32_t width)
    {
        std::string resux = (pystring::ljust(_sv, width, ' '));
        return PythonString(std::move(resux));
    }

    PythonString ljust(int32_t width, char fillchar)
    {
        std::string resux = (pystring::ljust(_sv, width, fillchar));
        return PythonString(std::move(resux));
    }
    PythonString ljust(int32_t width, const std::string &fillchar)
    {
        return ljust(width, fillchar[0]);
    }

    template <typename T> PythonString &ljust_inplace(int32_t width, T fillchar)
    {
        this->operator=(ljust(width, fillchar));
        return *this;
    }
    PythonString &ljust_inplace(int32_t width)
    {
        this->operator=(ljust(width, ' '));
        return *this;
    }

    PythonString center(int32_t width, char fillchar)
    {
        std::string resux = (pystring::center(_sv, width, fillchar));
        return PythonString(std::move(resux));
    }
    PythonString center(int32_t width, const std::string &fillchar)
    {
        return center(width, fillchar[0]);
    }
    PythonString center(int32_t width)
    {
        return center(width, ' ');
    }

    template <typename T> PythonString &center_inplace(int32_t width, T fillchar)
    {
        this->operator=(center(width, fillchar));
        return *this;
    }
    PythonString &center_inplace(int32_t width)
    {
        this->operator=(center(width, ' '));
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.find
    int find(const PythonString &sub, int start, int end)
    {
        return (pystring::find(_sv, sub._sv, start, end));
    }
    int find(const std::string_view sub, int start, int end)
    {
        return (pystring::find(_sv, sub, start, end));
    }
    int find(const char *sub, int start, int end)
    {
        return (pystring::find(_sv, std::string(sub), start, end));
    }
    int find(const std::string &sub, int start, int end)
    {
        return (pystring::find(_sv, std::string_view(sub), start, end));
    }
    template <typename T> int find(const T sub)
    {
        return find(sub, 0, MAX_32BIT_INT);
    }

    template <typename T> int index(const T sub, int start, int end)
    {
        int result = find(sub, start, end);
        if (result < 0)
        {
            throw std::runtime_error("Python Value Error");
        }
        return result;
    }
    template <typename T> int index(const T sub)
    {
        return index(sub, 0, MAX_32BIT_INT);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.rfind
    int rfind(const PythonString &sub, int start, int end)
    {
        return (pystring::rfind(_sv, sub._sv, start, end));
    }
    int rfind(const std::string_view sub, int start, int end)
    {
        return (pystring::rfind(_sv, sub, start, end));
    }
    int rfind(const char *sub, int start, int end)
    {
        return (pystring::rfind(_sv, std::string(sub), start, end));
    }
    int rfind(const std::string &sub, int start, int end)
    {
        return (pystring::rfind(_sv, std::string_view(sub), start, end));
    }
    template <typename T> int rfind(const T sub)
    {
        return rfind(sub, 0, MAX_32BIT_INT);
    }
    template <typename T> int rindex(const T sub, int start, int end)
    {
        int result = rfind(sub, start, end);
        if (result < 0)
        {
            throw std::runtime_error("Python Value Error");
        }
        return result;
    }
    template <typename T> int rindex(const T sub)
    {
        return rindex(sub, 0, MAX_32BIT_INT);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.expandtabs
    PythonString expandtabs(int32_t tabsize)
    {
        std::string resux = (pystring::expandtabs(_sv, tabsize));
        return PythonString(std::move(resux));
    }
    PythonString expandtabs()
    {
        return expandtabs(8);
    }

    PythonString &expandtabs_inplace(int32_t tabsize)
    {
        this->operator=(expandtabs(tabsize));
        return *this;
    }
    PythonString &expandtabs_inplace()
    {
        this->operator=(expandtabs(8));
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.count
    int count(const std::string_view substr, int start, int end)
    {
        return (pystring::count(_sv, substr, start, end));
    }
    int count(const char *substr, int start, int end)
    {
        return (pystring::count(_sv, std::string(substr), start, end));
    }
    int count(char substr, int start, int end)
    {
        std::string tmpstr(1, substr);
        return (pystring::count(_sv, tmpstr, start, end));
    }
    int count(const std::string &substr, int start, int end)
    {
        return (pystring::count(_sv, substr, start, end));
    }
    int count(const PythonString &substr, int start, int end)
    {
        return (pystring::count(_sv, substr._sv, start, end));
    }
    std::map<std::string, int> count(const std::vector<std::string> &substr, int start, int end)
    {
        std::map<std::string, int> tmpmap;
        for (const std::string_view sub : substr)
        {
            tmpmap[std::string(sub)] = (pystring::count(_sv, sub, start, end));
        }
        return tmpmap;
    }

    template <typename T> int count(const T substr)
    {
        return count(substr, 0, MAX_32BIT_INT);
    }

    std::map<std::string, int> count(const std::vector<std::string> &substr)
    {
        return count(substr, 0, MAX_32BIT_INT);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.replace
    PythonString replace(const PythonString &oldsub, const PythonString &newsub, int count)
    {
        std::string resux = (pystring::replace(_sv, oldsub._sv, newsub._sv, count));
        return PythonString(std::move(resux));
    }
    PythonString replace(const std::string_view oldsub, const std::string_view newsub, int count)
    {
        std::string resux = (pystring::replace(_sv, oldsub, newsub, count));
        return PythonString(std::move(resux));
    }
    PythonString replace(const char *oldsub, const char *newsub, int count)
    {
        std::string resux = (pystring::replace(_sv, std::string(oldsub), std::string(newsub), count));
        return PythonString(std::move(resux));
    }
    PythonString replace(const std::string &oldsub, const std::string &newsub, int count)
    {
        std::string resux = (pystring::replace(_sv, std::string_view(oldsub), newsub, count));
        return PythonString(std::move(resux));
    }
    template <typename T, typename U> PythonString replace(const T oldsub, const U newsub)
    {
        return replace(oldsub, newsub, -1);
    }

    template <typename T, typename U> PythonString replace_inplace(const T oldsub, const U newsub, int count)
    {
        this->operator=(replace(oldsub, newsub, count));
        return *this;
    }
    template <typename T, typename U> PythonString replace_inplace(const T oldsub, const U newsub)
    {
        this->operator=(replace(oldsub, newsub, -1));
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.removeprefix
    PythonString removeprefix(const PythonString &prefix)
    {
        std::string resux = (pystring::removeprefix(_sv, prefix._sv));
        return PythonString(std::move(resux));
    }
    PythonString removeprefix(const std::string_view prefix)
    {
        std::string resux = (pystring::removeprefix(_sv, prefix));
        return PythonString(std::move(resux));
    }
    PythonString removeprefix(const char *prefix)
    {
        std::string resux = (pystring::removeprefix(_sv, std::string(prefix)));
        return PythonString(std::move(resux));
    }
    PythonString removeprefix(const std::string &prefix)
    {
        std::string resux = (pystring::removeprefix(_sv, std::string_view(prefix)));
        return PythonString(std::move(resux));
    }

    template <typename T> PythonString &removeprefix_inplace(const T prefix)
    {
        this->operator=(removeprefix(prefix));
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.removesuffix
    PythonString removesuffix(const PythonString &suffix)
    {
        std::string resux = (pystring::removesuffix(_sv, suffix._sv));
        return PythonString(std::move(resux));
    }
    PythonString removesuffix(const std::string_view suffix)
    {
        std::string resux = (pystring::removesuffix(_sv, suffix));
        return PythonString(std::move(resux));
    }
    PythonString removesuffix(const char *suffix)
    {
        std::string resux = (pystring::removesuffix(_sv, std::string(suffix)));
        return PythonString(std::move(resux));
    }
    PythonString removesuffix(const std::string &suffix)
    {
        std::string resux = (pystring::removesuffix(_sv, std::string_view(suffix)));
        return PythonString(std::move(resux));
    }

    template <typename T> PythonString &removesuffix_inplace(const T suffix)
    {
        this->operator=(removesuffix(suffix));
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<PythonString> partition(const PythonString &delimiter)
    {
        return partition(delimiter._sv);
    }
    std::vector<PythonString> partition(PythonString &&delimiter)
    {
        return partition(delimiter._sv);
    }
    std::vector<PythonString> partition(const char *delimiter)
    {
        return partition(std::string(delimiter));
    }
    std::vector<PythonString> partition(std::string &&delimiter)
    {
        return partition(std::string_view(delimiter));
    }
    std::vector<PythonString> partition(const std::string &delimiter)
    {
        return partition(std::string_view(delimiter));
    }
    std::vector<PythonString> partition(const std::string_view delimiter)
    {
        std::vector<std::string> result;
        std::vector<PythonString> pyresult;
        result.reserve(3);
        pystring::partition(_sv, delimiter, result);
        for (auto &str : result)
        {
            pyresult.emplace_back(PythonString(std::move(str)));
        }
        return pyresult;
    }
    std::vector<PythonString> partition(char delimiter)
    {
        std::string delimiterstr = std::string(1, delimiter);
        return partition(std::string_view(delimiterstr));
    }
    std::vector<PythonString> rpartition(const PythonString &delimiter)
    {
        return rpartition(delimiter._sv);
    }
    std::vector<PythonString> rpartition(PythonString &&delimiter)
    {
        return rpartition(delimiter._sv);
    }
    std::vector<PythonString> rpartition(const char *delimiter)
    {
        return rpartition(std::string(delimiter));
    }
    std::vector<PythonString> rpartition(std::string &&delimiter)
    {
        return rpartition(std::string_view(delimiter));
    }
    std::vector<PythonString> rpartition(const std::string &delimiter)
    {
        return rpartition(std::string_view(delimiter));
    }
    std::vector<PythonString> rpartition(const std::string_view delimiter)
    {
        std::vector<std::string> result;
        std::vector<PythonString> pyresult;
        result.reserve(3);
        pystring::rpartition(_sv, delimiter, result);
        for (auto &str : result)
        {
            pyresult.emplace_back(PythonString(std::move(str)));
        }
        return pyresult;
    }
    std::vector<PythonString> rpartition(char delimiter)
    {
        std::string delimiterstr = std::string(1, delimiter);
        return rpartition(std::string_view(delimiterstr));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.capitalize_each_word
    PythonString capitalize_each_word()
    {
        std::string newstring = pystring::lower(_sv);
        for (size_t i = 0; i < _sv.size() - 1; i++)
        {
            if (i == 0)
            {
                newstring[i] = toupper(newstring[i]);
            }
            if ((_sv[i] == ' '))
            {
                newstring[i + 1] = toupper(newstring[i + 1]);
            }
        }
        return PythonString(std::move(newstring));
    }

    PythonString &capitalize_each_word_inplace()
    {
        this->operator=(capitalize_each_word());
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.capitalize
    PythonString capitalize()
    {
        return PythonString(std::move(pystring::capitalize(_sv)));
    }
    PythonString &capitalize_inplace()
    {
        this->operator=(capitalize());
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.lower
    PythonString lower()
    {
        return PythonString(std::move(pystring::lower(_sv)));
    }
    PythonString &lower_inplace()
    {
        this->operator=(lower());
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.upper
    PythonString upper()
    {
        return PythonString(std::move(pystring::upper(_sv)));
    }
    PythonString &upper_inplace()
    {
        this->operator=(upper());
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.swapcase
    PythonString swapcase()
    {
        return PythonString(std::move(pystring::swapcase(_sv)));
    }
    PythonString &swapcase_inplace()
    {
        this->operator=(swapcase());
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.title
    PythonString title()
    {
        return PythonString(std::move(pystring::title(_sv)));
    }
    PythonString &title_inplace()
    {
        this->operator=(title());
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    static constexpr size_t calculate_string_hash_static(const std::string_view _sv)
    {
        uint32_t hashnow = 2166136261u;

        for (size_t startiter = 0; startiter < _sv.size(); startiter++)
        {
            hashnow ^= (uint8_t)(_sv.data()[startiter]);
            hashnow *= 16777619u;
        }
        return (size_t)hashnow;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    PythonString insert_strings_at_indices(std::vector<std::pair<int, std::string>> &indicies_and_strings)
    {
        return PythonString(py_advanced_split_and_insert::insert_strings_at_indices(indicies_and_strings, _sv));
    }
    PythonString insert_strings_at_indices(std::vector<std::pair<int, PythonString>> &indicies_and_strings)
    {
        std::vector<std::pair<int, std::string>> indicies_and_stringstmp;
        indicies_and_stringstmp.reserve(indicies_and_strings.size());
        for (auto &item : indicies_and_strings)
        {
            indicies_and_stringstmp.emplace_back(std::make_pair(item.first,
                                                                item.second._str)); // item.first, item.second._str);
        }
        return PythonString(py_advanced_split_and_insert::insert_strings_at_indices(indicies_and_stringstmp, _sv));
    }

    template <typename T> PythonString &insert_strings_at_indices_inplace(T &indicies_and_strings)
    {
        this->operator=(insert_strings_at_indices(indicies_and_strings));
        return *this;
    }

    std::vector<PythonString> split_at_multi_string(const std::vector<PythonString> &splitats)
    {
        std::vector<std::string> resultssplits;
        std::vector<std::string> splitats2;
        splitats2.reserve(splitats.size());
        for (auto &item : splitats)
        {
            splitats2.emplace_back(item._str);
        }
        py_advanced_split_and_insert::split_at_multi_string(_sv, splitats2, resultssplits);
        return _return_pystring_vec(std::move(resultssplits));
    }
    std::map<size_t, PythonString> split_at_multi_string_keep_strings(std::vector<std::string> &splitats)
    {
        std::map<size_t, std::string> mapresults =
            py_advanced_split_and_insert::split_at_multi_string_keep_strings(_sv, splitats);
        std::map<size_t, PythonString> mapresults2;
        for (auto &item : mapresults)
        {
            mapresults2.emplace(item.first, PythonString(std::move(item.second)));
        }
        return mapresults2;
    }
    std::map<size_t, PythonString> split_at_multi_string_keep_strings(std::vector<std::string> &&splitats)
    {
        std::map<size_t, std::string> mapresults =
            py_advanced_split_and_insert::split_at_multi_string_keep_strings(_sv, splitats);
        std::map<size_t, PythonString> mapresults2;
        for (auto &item : mapresults)
        {
            mapresults2.emplace(item.first, PythonString(std::move(item.second)));
        }
        return mapresults2;
    }
    std::map<size_t, PythonString> split_at_multi_string_keep_strings(std::vector<PythonString> &splitats)
    {
        std::vector<std::string> splitats2;
        splitats2.reserve(splitats.size());
        for (auto &item : splitats)
        {
            splitats2.emplace_back(item._str);
        }
        return split_at_multi_string_keep_strings(splitats2);
    }

    size_t len() const
    {
        return _sv.size();
    }

    std::vector<PythonString> split_strtok(const std::string &single_char_delimiters)
    {
        std::string _strc = _str;
        const char *strc = _strc.c_str();
        char *str = (char *)(strc);
        const char *delimitersc = single_char_delimiters.c_str();
        char *delimiters = (char *)(delimitersc);
        std::vector<PythonString> tokensspl;
        char *token = strtok(str, delimiters);
        while (token != NULL)
        {
            tokensspl.emplace_back(PythonString(std::string(token)));
            token = strtok(NULL, delimiters);
        }
        return tokensspl;
    }

    PythonString casefold()
    {
        return remove_accents().lower();
    }

    PythonString &casefold_inplace()
    {
        this->operator=(casefold());
        return *this;
    }

    PythonString remove_accents()
    {
        return ((this->replace("À", "A")
                     .replace("Á", "A")
                     .replace("Â", "A")
                     .replace("Ã", "A")
                     .replace("Ä", "A")
                     .replace("Å", "A")
                     .replace("Æ", "E")
                     .replace("Ç", "C")
                     .replace("È", "E")
                     .replace("É", "E")
                     .replace("Ê", "E")
                     .replace("Ë", "E")
                     .replace("Ì", "I")
                     .replace("Í", "I")
                     .replace("Î", "I")
                     .replace("Ï", "I")
                     .replace("Ð", "D")
                     .replace("Ñ", "N")
                     .replace("Ò", "O")
                     .replace("Ó", "O")
                     .replace("Ô", "O")
                     .replace("Õ", "O")
                     .replace("Ö", "O")
                     .replace("×", "x")
                     .replace("Ø", "0")
                     .replace("Ù", "U")
                     .replace("Ú", "U")
                     .replace("Û", "U")
                     .replace("Ü", "U")
                     .replace("Ý", "Y")
                     .replace("Þ", "P")
                     .replace("ß", "ss")
                     .replace("ẞ", "SS")
                     .replace("à", "a")
                     .replace("á", "a")
                     .replace("â", "a")
                     .replace("ã", "a")
                     .replace("ä", "a")
                     .replace("å", "a")
                     .replace("æ", "e")
                     .replace("ç", "c")
                     .replace("è", "e")
                     .replace("é", "e")
                     .replace("ê", "e")
                     .replace("ë", "e")
                     .replace("ì", "i")
                     .replace("í", "i")
                     .replace("î", "i")
                     .replace("ï", "i")
                     .replace("ð", "O")
                     .replace("ñ", "n")
                     .replace("ò", "o")
                     .replace("ó", "o")
                     .replace("ô", "o")
                     .replace("õ", "o")
                     .replace("ö", "o")
                     .replace("÷", "/")
                     .replace("ø", "0")
                     .replace("ù", "u")
                     .replace("ú", "u")
                     .replace("û", "u")
                     .replace("ü", "u")
                     .replace("ý", "y")
                     .replace("þ", "p")
                     .replace("ÿ", "y")));
    }

    PythonString &remove_accents_inplace()
    {
        this->operator=(remove_accents());
        return *this;
    }
    bool is_bin_oct_hex_dec()
    {
        return pystring::is_any_valid_number(_sv);
    }
    bool is_oct_number()
    {
        return pystring::is_octal_notation(_sv);
    }
    bool is_hex_number_with_0x()
    {

        return (pystring::is_hex_notation_lower_with_0x(_sv)) || (pystring::is_hex_notation_upper_with_0x(_sv));
    }
    bool is_hex_number_without_0x()
    {

        return (pystring::is_hex_notation_lower_without_0x(_sv)) || (pystring::is_hex_notation_upper_without_0x(_sv));
    }
    bool is_binary_with_0b()
    {
        return (pystring::is_binary_notation(_sv));
    }

    static PythonString generate_random_alphanumeric_string(size_t len)
    {
        char *s2 = new char[len + 1];
        pystring::generate_random_alphanumstring(s2, len);
        auto pystr = PythonString(std::string(s2));
        delete[] s2;
        return pystr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // str.removesuffix
    static std::vector<PythonString> read_file_to_string_vector(const char *filename)
    {
        return PythonString::from_file(filename).splitlines(false);
    }
    static std::vector<PythonString> read_file_to_string_vector(const std::string &filename)
    {
        return read_file_to_string_vector(filename.c_str());
    }
    static std::vector<PythonString> read_file_to_string_vector(const std::string_view filename)
    {
        std::string tmpstring = std::string(filename);
        return read_file_to_string_vector(tmpstring.c_str());
    }
    static std::vector<PythonString> read_file_to_string_vector(const PythonString &filename)
    {
        return read_file_to_string_vector(filename._str.c_str());
    }
    static PythonString from_file(const char *filename)
    {
        // fstream causes problems when used with boost::pfr
        FILE *pipe = fopen(filename, "rb");
        if (pipe == NULL)
        {
            throw std::runtime_error("Could not open file");
        }
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        int size_my_buffer = sizeof(buffer);
        std::string cpptmpstring;
        cpptmpstring.reserve(1024);
        while (fgets(buffer, size_my_buffer, pipe) != NULL)
        {
            for (int i = 0; i < size_my_buffer; i++)
            {
                if (buffer[i] == '\0')
                {
                    continue;
                }
                else
                {
                    cpptmpstring += (buffer[i]);
                    buffer[i] = '\0';
                }
            }
        }

        fclose(pipe);
        return PythonString(std::move(cpptmpstring));
    }
    static PythonString from_file(const std::string &filename)
    {
        return from_file(filename.c_str());
    }
    static PythonString from_file(std::string &&filename)
    {
        return from_file(filename.c_str());
    }
    static PythonString from_file(const std::string_view filename)
    {
        std::string tmpstring = std::string(filename);
        return from_file(tmpstring.c_str());
    }
    static PythonString from_file(const PythonString &filename)
    {
        return from_file(filename._str.c_str());
    }
    static PythonString from_file(PythonString &&filename)
    {
        return from_file(filename._str.c_str());
    }
    std::vector<PythonString> split_at_multi_string(pyindex::PY_STRING_VEC &split_at)
    {
        std::vector<std::string> resultssplits;
        py_advanced_split_and_insert::split_at_multi_string(_sv, split_at, resultssplits);
        return _return_pystring_vec(std::move(resultssplits));
    }
    std::vector<PythonString> split_at_multi_string(pyindex::PY_STRING_VEC &&split_at)
    {
        std::vector<std::string> resultssplits;
        py_advanced_split_and_insert::split_at_multi_string(_sv, split_at, resultssplits);
        return _return_pystring_vec(std::move(resultssplits));
    }
    void save_as_file(const char *filename)
    {
        FILE *pipe = fopen(filename, "wb");
        if (pipe == NULL)
        {
            throw std::runtime_error("Could not open file");
        }
        auto startptr = (char *)(_str.c_str());
        auto endptr = startptr + _str.size();
        for (; startptr != endptr;)
        {
            putc(*startptr++, pipe);
        }
        fclose(pipe);
    }
    void append_to_file(const char *filename)
    {
        FILE *pipe = fopen(filename, "ab");
        if (pipe == NULL)
        {
            throw std::runtime_error("Could not open file");
        }
        auto startptr = (char *)(_str.c_str());
        auto endptr = startptr + _str.size();
        for (; startptr != endptr;)
        {
            putc(*startptr++, pipe);
        }
        fclose(pipe);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool in(const std::string_view v)
    {
        return (v.find(_sv) != std::string::npos);
    }
    bool in(const PythonString &v)
    {
        return in(v._sv);
    }
    bool in(const std::string &v)
    {
        return in(std::string_view(v));
    }
    bool in(const char *v)
    {
        return in(std::string(v));
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // some other functions to locate substrings
    std::vector<size_t> find_me_everywhere_in_another_string(const PythonString &value)
    {
        return stringmatch::find_all_indices_strviewfind(value._sv, _sv);
    }
    std::vector<size_t> find_me_everywhere_in_another_string(const std::string_view value)
    {
        return stringmatch::find_all_indices_strviewfind(value, _sv);
    }
    std::vector<size_t> find_me_everywhere_in_another_string(const char *value)
    {
        return stringmatch::find_all_indices_strstr(value, _str.c_str());
    }
    std::vector<size_t> find_me_everywhere_in_another_string(const std::string &value)
    {
        return stringmatch::find_all_indices_strstr(value.c_str(), _str.c_str());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename... Args> PythonString format(Args... args)
    {
        int n = std::snprintf(nullptr, 0, _str.c_str(), args...);
        if (n < 0)
        {
            throw std::runtime_error("Formatting error");
        }
        std::string bufferstring(n + 1, '\0');
        std::snprintf(&bufferstring[0], n + 1, _str.c_str(), args...);
        bufferstring.resize(n);
        return PythonString(std::move(bufferstring));
    }
    template <typename... Args> PythonString &format_inplace(Args... args)
    {
        this->operator=(format(args...));
        return *this;
    }

    template <typename T> PythonString format_map(const T &v)
    {
        PythonString tmpstring = _str;
        for (const auto &kv : v)
        {
            std::string repl1 = "{" + kv.first + "}";
            std::string repl2 = kv.second;
            tmpstring = tmpstring.replace(repl1, repl2);
        }
        return tmpstring;
    }

    template <typename T> PythonString &format_map_inplace(const T &v)
    {
        this->operator=(format_map(v));
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    std::vector<size_t> find_subststring_everywhere_in_me(const PythonString &value)
    {
        return stringmatch::find_all_indices_strviewfind(_sv, value._sv);
    }
    std::vector<size_t> find_subststring_everywhere_in_me(const std::string_view value)
    {
        return stringmatch::find_all_indices_strviewfind(_sv, value);
    }
    std::vector<size_t> find_subststring_everywhere_in_me(const char *value)
    {
        return stringmatch::find_all_indices_strstr(_str.c_str(), value);
    }
    std::vector<size_t> find_subststring_everywhere_in_me(const std::string &value)
    {
        return stringmatch::find_all_indices_strstr(_str.c_str(), value.c_str());
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    std::unordered_map<std::string, std::vector<size_t>> find_subststrings_everywhere_in_me(
        const std::vector<std::string> &needles)
    {
        return stringmatch::find_all_indices_multi_strstr(_str, needles);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convert to base16
    PythonString convert_to_base16()
    {
        std::string cstring2hex((_str.size()) * 2, 48);
        auto buf = cstring2hex.data();
        for (size_t i = 0; i < _sv.size(); i++)
        {
            *buf++ = pystring::hex_upper_letters[_sv[i] >> 4];
            *buf++ = pystring::hex_upper_letters[_sv[i] & 0x0F];
        }
        return PythonString(std::move(cstring2hex));
    }

    PythonString &convert_to_base16_inplace()
    {
        this->operator=(convert_to_base16());
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // string from base16
    static PythonString from_base16(const std::string &str)
    {
        std::string cstring2hex;
        cstring2hex.reserve(str.size() / 2);
        for (size_t i = 0; i < str.size(); i += 2)
        {
            std::string cstri = str.substr(i, 2);
            char result{};
            std::from_chars(str.data() + i, str.data() + i + 2, result, 16);
            cstring2hex += result;
        }
        return PythonString(std::move(cstring2hex));
    }
    PythonString &from_base16_inplace()
    {
        this->operator=(PythonString::from_base16(_str));
        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<PythonString> split_and_beginning_of_multiple_strings(
        const std::vector<std::string> &strings2find, const std::string_view forbidden_bound_chars_front,
        const std::string_view forbidden_bound_chars_back)
    {
        std::vector<size_t> oksplitstrings;
        oksplitstrings.reserve(_sv.size());
        oksplitstrings.push_back(0);
        oksplitstrings.push_back(_sv.size());
        for (const std::string_view str2find1 : strings2find)
        {
            auto firststringresults = this->find_subststring_everywhere_in_me(str2find1);
            size_t length_of_string = str2find1.size();

            for (const auto &start_of_string : firststringresults)
            {
                size_t end_of_string = start_of_string + length_of_string;
                bool front_ok = false;
                bool back_ok = false;
                if (start_of_string == 0)
                {
                    front_ok = true;
                }
                if (end_of_string == _sv.size() - 1)
                {
                    back_ok = true;
                }
                if (!front_ok)
                {

                    size_t index_front2check = start_of_string - 1;
                    front_ok = true;
                    for (size_t chi = 0; chi < forbidden_bound_chars_front.size(); chi++)
                    {
                        if (_sv[index_front2check] == forbidden_bound_chars_front[chi])
                        {
                            front_ok = false;
                            break;
                        }
                    }
                }
                if ((!back_ok))
                {
                    size_t index_back2check = end_of_string;
                    back_ok = true;
                    if ((index_back2check + 1) <= _sv.size())
                    {

                        for (size_t chi = 0; chi < forbidden_bound_chars_back.size(); chi++)
                        {
                            if (_sv[index_back2check] == forbidden_bound_chars_back[chi])
                            {
                                back_ok = false;
                                break;
                            }
                        }
                    }
                }
                if ((front_ok) && (back_ok) && (end_of_string < _sv.size() - 1))
                {
                    oksplitstrings.push_back(start_of_string);
                }
            }
        }
        std::sort(oksplitstrings.begin(), oksplitstrings.end());
        auto last = std::unique(oksplitstrings.begin(), oksplitstrings.end());
        oksplitstrings.erase(last, oksplitstrings.end());
        std::sort(oksplitstrings.begin(), oksplitstrings.end());
        std::vector<std::string> split_results_string;
        split_results_string.resize(oksplitstrings.size() - 1);
        for (size_t chi = 0; chi < oksplitstrings.size() - 1; chi++)
        {
            for (size_t chi2 = oksplitstrings[chi]; chi2 < oksplitstrings[chi + 1]; chi2++)
            {
                split_results_string[chi] += _sv[chi2];
            }
        }
        std::vector<PythonString> split_results_string_final;
        split_results_string_final.reserve(split_results_string.size());
        for (auto &&ii : split_results_string)
        {
            if (!ii.empty())
            {
                split_results_string_final.emplace_back(PythonString(ii));
            }
        }
        return split_results_string_final;
    }

    PythonString &split_and_beginning_of_multiple_strings_inplace(const std::vector<std::string> &strings2find,
                                                                  const std::string_view forbidden_bound_chars_front,
                                                                  const std::string_view forbidden_bound_chars_back,
                                                                  const std::string_view join_string)
    {

        auto interresult = split_and_beginning_of_multiple_strings(strings2find, forbidden_bound_chars_front,
                                                                   forbidden_bound_chars_back);
        *this = PythonString(join_string).join(interresult);
        return *this;
    }

    PythonString normalize_whitespaces()
    {
        std::string result = pystring::normalize_whitespaces(_sv);
        return PythonString(std::move(result));
    }
    PythonString &normalize_whitespaces_inplace()
    {
        this->operator=(this->normalize_whitespaces());
        return *this;
    }

    PythonString remove_whitespaces()
    {
        std::string result = pystring::remove_whitespaces(_sv);
        return PythonString(std::move(result));
    }
    PythonString &remove_whitespaces_inplace()
    {
        this->operator=(this->remove_whitespaces());
        return *this;
    }
    PythonString pad_string(int length)
    {
        std::string tmpstr = pystring::to_padded(length, ' ', _sv);
        return PythonString(std::move(tmpstr));
    }
    PythonString &pad_string_inplace(int length)
    {
        this->operator=(pad_string(length));
        return *this;
    }

    PythonString pad_string(int length, char pad_char)
    {
        std::string tmpstr = pystring::to_padded(length, pad_char, _sv);
        return PythonString(std::move(tmpstr));
    }
    PythonString &pad_string_inplace(int length, char pad_char)
    {
        this->operator=(pad_string(length, pad_char));
        return *this;
    }

    PythonString replace_non_alphanumeric_with_spaces()
    {
        std::string tmpstr = pystring::replace_fu(_sv, 0);
        return PythonString(std::move(tmpstr));
    }
    PythonString replace_non_printable_with_spaces()
    {
        std::string tmpstr = pystring::replace_fu(_sv, 1);
        return PythonString(std::move(tmpstr));
    }
    PythonString replace_non_decimal_with_spaces()
    {
        std::string tmpstr = pystring::replace_fu(_sv, 2);
        return PythonString(std::move(tmpstr));
    }
    PythonString &replace_non_alphanumeric_with_spaces_inplace()
    {

        this->operator=(this->replace_non_alphanumeric_with_spaces());
        return *this;
    }

    PythonString &replace_non_printable_with_spaces_inplace()
    {
        this->operator=(this->replace_non_printable_with_spaces());
        return *this;
    }

    PythonString &replace_non_decimal_with_spaces_inplace()
    {
        this->operator=(this->replace_non_decimal_with_spaces());
        return *this;
    }
    int hamming_distance_best_fit(const PythonString &other)
    {
        return pystring::calc_best_substring_distance(_sv, other._sv, 0);
    }
    constexpr int hamming_distance_best_fit(const std::string_view other)
    {
        return pystring::calc_best_substring_distance(_sv, other, 0);
    }
    int hamming_distance_best_fit(const std::string &other)
    {
        return pystring::calc_best_substring_distance(_sv, std::string_view(other), 0);
    }
    constexpr int hamming_distance_best_fit(const char *other)
    {
        const std::string_view othertmp(other);
        return pystring::calc_best_substring_distance(_sv, othertmp, 0);
    }

    int substring_distance_best_fit_v1(const PythonString &other)
    {
        return pystring::calc_best_substring_distance(_sv, other._sv, 1);
    }
    constexpr int substring_distance_best_fit_v1(const std::string_view other)
    {
        return pystring::calc_best_substring_distance(_sv, other, 1);
    }
    int substring_distance_best_fit_v1(const std::string &other)
    {
        return pystring::calc_best_substring_distance(_sv, std::string_view(other), 1);
    }
    constexpr int substring_distance_best_fit_v1(const char *other)
    {
        const std::string_view othertmp(other);
        return pystring::calc_best_substring_distance(_sv, othertmp, 1);
    }

    int substring_distance_best_fit_v2(const PythonString &other)
    {
        return pystring::calc_best_substring_distance(_sv, other._sv, 2);
    }
    constexpr int substring_distance_best_fit_v2(const std::string_view other)
    {
        return pystring::calc_best_substring_distance(_sv, other, 2);
    }
    int substring_distance_best_fit_v2(const std::string &other)
    {
        return pystring::calc_best_substring_distance(_sv, std::string_view(other), 2);
    }
    constexpr int substring_distance_best_fit_v2(const char *other)
    {
        const std::string_view othertmp(other);
        return pystring::calc_best_substring_distance(_sv, othertmp, 2);
    }

    int substring_distance_best_fit_v3(const PythonString &other)
    {
        return pystring::calc_best_substring_distance(_sv, other._sv, 3);
    }
    constexpr int substring_distance_best_fit_v3(const std::string_view other)
    {
        return pystring::calc_best_substring_distance(_sv, other, 3);
    }
    int substring_distance_best_fit_v3(const std::string &other)
    {
        return pystring::calc_best_substring_distance(_sv, std::string_view(other), 3);
    }
    constexpr int substring_distance_best_fit_v3(const char *other)
    {
        const std::string_view othertmp(other);
        return pystring::calc_best_substring_distance(_sv, othertmp, 3);
    }

    std::vector<size_t> boyer_moore_horspool_searcher_all(const std::string_view needle)
    {
        std::vector<size_t> results;
        results.reserve(128);
        pystring::boyer_moore_horspool_searcher_all(_sv, needle, results);
        return results;
    }

    std::vector<size_t> boyer_moore_horspool_searcher_all(const PythonString &needle)
    {
        return boyer_moore_horspool_searcher_all(needle._sv);
    }

    std::vector<size_t> boyer_moore_horspool_searcher_all(const std::string &needle)
    {
        return boyer_moore_horspool_searcher_all(std::string_view(needle));
    }
    std::vector<size_t> boyer_moore_horspool_searcher_all(const char *needle)
    {
        const std::string_view needletmp(needle);
        return boyer_moore_horspool_searcher_all(needletmp);
    }

    std::unordered_map<std::string, int> count_word_frequency()
    {
        std::unordered_map<std::string, int> results;
        auto wspacesplit = split_strtok(pystring::spaces_and_pontuation);
        results.reserve(wspacesplit.size());
        auto startiter = wspacesplit.begin();
        auto enditer = wspacesplit.end();
        while (startiter != enditer)
        {
            results[startiter->_str]++;
            startiter++;
        }
        return results;
    }

    std::vector<std::string> get_unique_words()
    {
        auto wspacesplit = split_strtok(pystring::spaces_and_pontuation);
        std::unordered_set<std::string> results;
        results.reserve(wspacesplit.size());
        auto startiter = wspacesplit.begin();
        auto enditer = wspacesplit.end();
        while (startiter != enditer)
        {
            results.emplace(startiter->_str);
            startiter++;
        }
        auto asvec = std::vector<std::string>(results.begin(), results.end());
        std::sort(asvec.begin(), asvec.end());
        return asvec;
    }

    PythonString remove_duplicated_substrings_keep_first()
    {
        std::unordered_map<std::string, int> results;
        auto wspacesplit = split_strtok(pystring::spaces_and_pontuation);
        results.reserve(wspacesplit.size());
        auto startiter = wspacesplit.begin();
        auto enditer = wspacesplit.end();
        std::string tmpstr;
        tmpstr.reserve(_str.size());
        while (startiter != enditer)
        {
            results[startiter->_str]++;
            if (results[startiter->_str] == 1)
            {
                tmpstr += startiter->_str;
                tmpstr += ' ';
            }
            startiter++;
        }
        tmpstr.resize(tmpstr.size() - 1);
        return PythonString(std::move(tmpstr));
    }

    static std::vector<int> argsort_by_(std::vector<std::string_view> &v, int sort_algorithm)
    {
        std::vector<int> idx(v.size());
        std::iota(idx.begin(), idx.end(), 0);
        if (sort_algorithm == 0)
        {

            strsorters::length_sorter(idx, v);
        }
        else if (sort_algorithm == 1)
        {
            strsorters::alphabet_sorter(idx, v);
        }
        else if (sort_algorithm == 2)
        {
            strsorters::alphabet_sorter_last_letter(idx, v);
        }
        else if (sort_algorithm == 3)
        {
            strsorters::alphabet_sorter_ignore_case(idx, v);
        }
        else if (sort_algorithm == 4)
        {
            strsorters::alphabet_sorter_last_letter_ignore_case(idx, v);
        }
        else if (sort_algorithm == 5)
        {
            strsorters::alphabet_sorter_ignore_case_average(idx, v);
        }
        return idx;
    }
    static std::vector<int> argsort_by_(const std::vector<std::string> &v, int sort_algorithm)
    {
        std::vector<std::string_view> vsv{v.begin(), v.end()};
        return argsort_by_(vsv, sort_algorithm);
    }
    static std::vector<int> argsort_by_(const std::vector<PythonString> &v, int sort_algorithm)
    {
        std::vector<std::string_view> vsv{v.begin(), v.end()};
        return argsort_by_(vsv, sort_algorithm);
    }

    static std::vector<PythonString> sort_by_(std::vector<std::string_view> &v, int sort_algorithm)
    {
        auto idx = argsort_by_(v, sort_algorithm);
        std::vector<PythonString> retv;
        retv.reserve(v.size());
        auto startiter = idx.begin();
        auto enditer = idx.end();
        while (startiter != enditer)
        {
            retv.emplace_back(v[*startiter++]);
        }
        return retv;
    }
    static std::vector<PythonString> sort_by_(const std::vector<std::string> &v, int sort_algorithm)
    {
        std::vector<std::string_view> vsv{v.begin(), v.end()};
        return sort_by_(vsv, sort_algorithm);
    }
    static std::vector<PythonString> sort_by_(const std::vector<PythonString> &v, int sort_algorithm)
    {
        std::vector<std::string_view> vsv{v.begin(), v.end()};
        return sort_by_(vsv, sort_algorithm);
    }
    template <typename T> static std::vector<PythonString> sort_by_length(const T &v)
    {
        return sort_by_(v, 0);
    }
    template <typename T> static std::vector<int> argsort_by_length(const T &v)
    {
        return argsort_by_(v, 0);
    }

    template <typename T> static std::vector<PythonString> sort_by_alphabet(const T &v)
    {
        return sort_by_(v, 1);
    }
    template <typename T> static std::vector<int> argsort_by_alphabet(const T &v)
    {
        return argsort_by_(v, 1);
    }
    template <typename T> static std::vector<PythonString> sort_by_alphabet_reverse(const T &v)
    {
        return sort_by_(v, 2);
    }
    template <typename T> static std::vector<int> argsort_by_alphabet_reverse(const T &v)
    {
        return argsort_by_(v, 2);
    }
    template <typename T> static std::vector<PythonString> sort_by_alphabet_ignore_case(const T &v)
    {
        return sort_by_(v, 3);
    }
    template <typename T> static std::vector<int> argsort_by_alphabet_ignore_case(const T &v)
    {
        return argsort_by_(v, 3);
    }
    template <typename T> static std::vector<PythonString> sort_by_alphabet_reverse_ignore_case(const T &v)
    {
        return sort_by_(v, 4);
    }
    template <typename T> static std::vector<int> argsort_by_alphabet_reverse_ignore_case(const T &v)
    {
        return argsort_by_(v, 4);
    }

    template <typename T> static std::vector<PythonString> sort_by_alphabet_avarage_ignore_case(const T &v)
    {
        return sort_by_(v, 5);
    }
    template <typename T> static std::vector<int> argsort_by_alphabet_avarage_ignore_case(const T &v)
    {
        return argsort_by_(v, 5);
    }
};

std::ostream &operator<<(std::ostream &os, const PythonString &thestring)
{
    os << thestring._str;
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<PythonString> &thestrings)
{
    for (const auto &thestring : thestrings)
    {
        os << thestring;
        os << '\n';
    }
    return os;
}

template <typename T> std::ostream &operator<<(std::ostream &os, const std::vector<std::vector<T>> &thevec)
{
    for (const auto &thesubvec : thevec)
    {
        for (const auto &theelement : thesubvec)
        {

            os << theelement;
            os << '\t';
        }
        os << '\n';
    }
    return os;
}

template <typename T> std::ostream &operator<<(std::ostream &os, const std::vector<T> &thevec)
{
    for (const auto &theelement : thevec)
    {
        os << theelement;
        os << '\n';
    }
    return os;
}

template <typename T> std::ostream &operator<<(std::ostream &os, const std::set<T> &thevec)
{
    for (const auto &theelement : thevec)
    {
        os << theelement;
        os << '\n';
    }
    return os;
}
template <typename T> std::ostream &operator<<(std::ostream &os, const std::unordered_set<T> &thevec)
{
    for (const auto &theelement : thevec)
    {
        os << theelement;
        os << '\n';
    }
    return os;
}
template <typename T> std::ostream &operator<<(std::ostream &os, const std::list<T> &thevec)
{
    for (const auto &theelement : thevec)
    {
        os << theelement;
        os << '\n';
    }
    return os;
}
template <typename T, size_t N> std::ostream &operator<<(std::ostream &os, const std::array<T, N> &thevec)
{
    for (const auto &theelement : thevec)
    {
        os << theelement;
        os << '\n';
    }
    return os;
}
template <typename T, typename U> std::ostream &operator<<(std::ostream &os, const std::map<T, U> &thevec)
{
    for (const auto &theelement : thevec)
    {
        os << theelement.first;
        os << " : ";
        os << theelement.second;
        os << ",\n";
    }
    return os;
}
template <typename T, typename U> std::ostream &operator<<(std::ostream &os, const std::unordered_map<T, U> &thevec)
{
    for (const auto &theelement : thevec)
    {
        os << theelement.first;
        os << " : ";
        os << theelement.second;
        os << ",\n";
    }
    return os;
}
std::ostream &operator<<(std::ostream &os, const SliceObject &theslice)
{
    os << "I am a slice and I am made of raw pointers! Convert me to a "
          "string! "
          "This is what you'll get: ";
    for (const auto &thestring : theslice._slice)
    {
        os << (*thestring);
    }
    os << '\n';
    return os;
}

void enable_srand()
{
    ::srand(::time(NULL));
}

namespace std
{
template <> struct hash<PythonString>
{
    std::size_t operator()(const PythonString &obj) const
    {
        return (PythonString::calculate_string_hash_static(obj._sv));
    }
};
} // namespace std

#endif
