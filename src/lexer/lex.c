#include "lex.h"

#include <string.h> // memcpy
#include <assert.h> // assert

#include <util/out.h> // error reporting

int lex(Lexer *l, Token *t) {
    // If there are any tokens waiting in the putback buffer, read from there
    // instead.
    if (l->unlexed_count > 0) {
        l->unlexed_count--;
        memcpy(t, &l->unlexed[l->unlexed_count], sizeof(Token));
        return 0;
    }
    // Now, do the actual lexing: TODO!
    return 0;
}

int unlex(Lexer *l, Token *t) {
    // First, make sure we can actually fit it in the buffer.
    if (l->unlexed_count >= TOKEN_PUTBACKS) {
        PRINT_ERROR(
            "internal: tried to unlex more than %d tokens at a time",
            TOKEN_PUTBACKS
        );
        return -1; // Error return code
    }
    memcpy(&l->unlexed[l->unlexed_count], t, sizeof(Token));
    l->unlexed_count++;
    return 0;
}

int skip_to_token(Lexer *l) {
    char cur, prev;
    int in_block = 0, pass = 0;

    // Read the first character
    if ((cur = fgetc(l->fp)) != EOF) {
        prev = cur;
        if (!(cur == ' ' || cur == '\t' || cur == '/')) {
            fseek(l->fp, -1, SEEK_CUR);
            return 0; // Token begins immediately
        }
    } else {
        return -1; // File done, no more tokens
    }

    // Read each character from the file until EOF
    while ((cur = fgetc(l->fp)) != EOF) {
        if (cur == '/' && prev == '/' && in_block == 0) {
            in_block = 1; // Single line comment
        } else if (cur == '*' && prev == '/' && in_block == 0) {
            in_block = 2; // Block comment
            pass = 2;
        } else if ((in_block == 1 && cur == '\n') || 
                   (in_block == 2 && cur == '/' && prev == '*' && pass <= 0)) {
            in_block = 0; // Out of comment
        } else if (prev == '/' && !(cur == '*' || cur == '/') && in_block == 0) {
            fseek(l->fp, -1, SEEK_CUR);
            return 0; // Token was a slash without a * or / following it
        }

        if (!(cur == ' ' || cur == '\t' || cur == '/') && in_block == 0) {
            fseek(l->fp, -1, SEEK_CUR);
            return 0; // Token is next
        }

        pass -= 1;
        prev = cur;
    }

    return -1; // EOF was reached
}

TokenType ttype_one_char(char c) {
    switch (c) {
    case '(':
        return TT_OPAREN; // (
    case ')':
        return TT_CPAREN; // )
    case '{':
        return TT_OBRACE; // {
    case '}':
        return TT_CBRACE; // }
    case '[':
        return TT_OBRACKET; // [
    case ']':
        return TT_CBRACKET; // ]
    case ';':
        return TT_SEMI; // ;
    }

    return TT_NO_TOKEN;
}

TokenType ttype_many_chars(const char *contents) {
	// TODO: Handle operations

    // Includes only numbers
    int all_numeric = 1;

    int count_fs = 0;
    int count_us = 0;

    int i;

    if (contents == NULL) {
        return TT_NO_TOKEN;
    }

    // Loop through each character
    for (i = 0; contents[i] != '\0'; i++) {
        char c = contents[i];

        // If it has a period, it's a float
        if (c == '.') {
            return TT_LITERAL;
        }

        // Count 'f'
        if (c == 'f') {
            count_fs++;
        }

        // Count 'u'
        if (c == 'u') {
            count_us++;
        }

        // Is it from "0123456789"?
        if (c > '9' || c < '0' && c != 'u') {
            all_numeric = 0;
        }

        if (c == '\'' || c == '"') {
            return TT_LITERAL;
        }
    }

    if (all_numeric) {
        // 100u
        if (count_us == 1 && contents[i - 1] == 'u') {
            return TT_LITERAL;
        }

        // 100
        if (count_us == 0) {
            return TT_LITERAL;
        }
    }

    return TT_IDENTIFIER;
}

TokenType ttype_from_string(const char *contents) {
    int len;

    len = strlen(contents);

    // Single character contents
    if (len == 1) {
        TokenType token = ttype_one_char(contents[0]);

        if (token != TT_NO_TOKEN) {
            return token;
        }
    }

    return ttype_many_chars(contents);
}

int test_ttype_from_string() {
	assert(ttype_from_string("1") == TT_LITERAL);
	assert(ttype_from_string("1.2") == TT_LITERAL);

	assert(ttype_from_string("1u") == TT_LITERAL);
	assert(ttype_from_string("1.2f") == TT_LITERAL);
	assert(ttype_from_string("1.f") == TT_LITERAL);

	assert(ttype_from_string("\"Planck\"") == TT_LITERAL);
	assert(ttype_from_string("'Language'") == TT_LITERAL);

	assert(ttype_from_string("Jaba") == TT_IDENTIFIER);
	assert(ttype_from_string("cat_") == TT_IDENTIFIER);

	assert(ttype_from_string("(") == TT_OPAREN);
	assert(ttype_from_string("}") == TT_CBRACE);

	assert(ttype_from_string(";") == TT_SEMI);

	return 0;
}