#ifndef TEXT_UTIL_H
#define TEXT_UTIL_H

#include "common_std.h"

#include <ctype.h>
#include <stdio.h>

inline string visualize_escape_codes(const string& str) {
	string ret = "";
    for (char ch : str) {
		if (ch == '\'') {
			ret += "\\\'";
			continue;
		}
		if (ch == '\"') {
			ret += "\\\"";
			continue;
		}
		if (ch == '\\') {
			ret += "\\\\";
			continue;
		}
        if (isprint(ch)) {
            ret += ch;
			continue;
        }
		switch (ch) {
			case '\0': { ret += "\\0"; } break;
			case '\t': { ret += "\\t"; } break;
			case '\n': { ret += "\\n"; } break;
			case '\r': { ret += "\\r"; } break;
			case '\f': { ret += "\\f"; } break;
			case '\v': { ret += "\\v"; } break;
			default: {
				char buf[10];
				snprintf(buf, sizeof(buf), "\\x%02X", ch);
				ret += buf;
			} break;
		}
    }
	return ret;
}

inline char str_retrive(const string& line, size_t index) {
	if (index >= line.size()) {
		return '\0';
	}
	return line[index];
}


inline bool contains_ctype(const string& str, int (&ctype_func)(int)) {
    for (char ch : str) {
        if ((ctype_func)(static_cast<unsigned char>(ch))) {
            return true;
        }
    }
    return false;
}

inline bool case_insensetive_word_compare(const char* str, const char* word) {
	while (*str != '\0' && *word != '\0') {
		if (toupper(*str) != toupper(*word)) {
			return false;
		}
		str++;
		word++;
	}
	if (*str == '\0' && *word == '\0') {
		return true;
	}
	if (*str == '\0') {
		return false;
	}
	if (isalnum(*str) || *str == '.' || *str == '_') {
		return false;
	}
	return true;
}

inline bool find_in_list(size_t& index, const string& str, char const * const str_list[], size_t list_len) {
	for (size_t i = 0; i < list_len; i++) {
		char const * item = str_list[i];
		bool cmp = case_insensetive_word_compare(str.c_str(), item);
		if (cmp) {
			index = i;
			return true;
		}
	}
	return false;
}

#endif /* TEXT_UTIL_H */
