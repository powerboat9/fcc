#ifndef FC_LIB_TOKEN
#define FC_LIB_TOKEN

// Do not rely on flag outside of pull_token
// See: shenanigans
#define OP_ASSIGN 1

#define OP_ADD    0
#define OP_SUB    2
#define OP_MUL    4
#define OP_DIV    6
#define OP_AND    8
#define OP_OR    10
#define OP_SHL   12
#define OP_SHR   14
#define OP_MOD   16
#define OP_INC   18
#define OP_DEC   20

// shenanigans to make tokenising a bit simpler
#define OP_LT    22
#define OP_LTE   23

#define OP_SET   24
#define OP_EQ    25

#define OP_BONOT 26
#define OP_NEQ   27

#define OP_GTE   29
#define OP_GT    28

#define OP_OPA   30
#define OP_CPA   32
#define OP_BOAND 34
#define OP_BOOR  36
#define OP_NOT   38

#define TOKEN_OPERATOR        0
#define TOKEN_IDENTIFIER      1
#define TOKEN_STRING_LITERAL  2
#define TOKEN_INTEGER_LITERAL 3
#define TOKEN_FP_LITERAL      4
#define TOKEN_SEMICOLON       5
#define TOKEN_EOF             6

struct token {
    char tt;
    union {
        struct {
            char id;
        } operator;
        struct {
            char *ptr;
            size_t len;
        } identifier;
        struct {
            char *ptr;
            size_t len;
            char is_heap;
        } string_literal;
        struct {
            long long value;
        } integer_literal;
        struct {
            double value;
        } fp_literal;
    } inner;
}

int is_digit(char c) {
    switch (c) {
        case '0' ... '9':
            return 1;
        default:
            return 0;
    }
}

#define DECLARE_PARSER(name, out_type) \
    out_type name(\
        struct {\
            char *ptr;\
            size_t len;\
        } *_parser_view\
    )

#define APPLY_PARSER(funct) (funct(_parser_view))

#define STR_IF(s_switch, l, s_const, match_label) \
    do {\
        if (l >= strlen(s_const)) {\
            for (size_t i = 0; i < len; i++) {\
                if (s_const[i] == 0) goto match_label;\
                if (s_const[i] != s_switch[i]) break;\
            }\
        }\
    } while (0)

#define EITHER() \
    do {\
        __label__ _tmp_match, _tmp_skip;\
        STR_IF(s_switch, l, s_const, _tmp_match);\
        goto _tmp_skip;\
        _tmp_match:\
        goto match_label;\
        _tmp_skip:\
    } while (0)

// Returns 0 on success, -1 on error
int pull_token(char **ptr_p, size_t *len_p, struct token *out) {
    char *ptr = *ptr_p;
    size_t len = *len_p;
    if (len == 0) {
        out->tt = TOKEN_EOF;
        return 0;
    }
    size_t new_token_span;
    char op_id;
    switch (ptr[0]) {
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '_':
            goto identifier;
        case '0':
            if (len == 1) {
                new_token_span = 1;
                goto simple_int;
            }
            switch (ptr[1]) {
                case 'x':
                    if ((len == 2)
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
#define _NOASSIGN_OPERATION(c, id) \
        case c:\
            op_id = id;\
            goto operator_noassign;
#define _ASSIGNABLE_OPERATION(c, id) \
        case c:\
            op_id = id;\
            if (len == 1) goto operator_noassign;\
            goto operator_assignable;
        _ASSIGNABLE_OPERATION('*', OP_MUL)
        _ASSIGNABLE_OPERATION('%', OP_MOD)
        _ASSIGNABLE_OPERATION('=', OP_SET)
        _ASSIGNABLE_OPERATION('!', OP_BONOT)
        _NOASSIGN_OPERATION('(', OP_OPA)
        _NOASSIGN_OPERATION(')', CP_OPA)
        _NOASSIGN_OPERATION('~', OP_NOT)
        case '/':
            if (len == 1) {
                op_id = OP_DIV;
                goto operator_noassign;
            }
            switch (ptr[1]) {
                case '*':
                    int nest_cnt = 0;
                    size_t pos = 2;
                    comment_search:
                    if ((pos + 1) >= len) {
                        fprintf(stderr, "unterminated comment");
                        return -1;
                    }
                    switch (ptr[pos]) {
                        case '/':
                            if (ptr[pos + 1] == '*') {
                                nest_cnt += 1;
                                pos += 2;
                            }
                            goto comment_search;
                        case '*':
                            if (ptr[pos + 1] == '/') {
                                if (nest_cnt == 0) {
                                    *ptr_p = ptr + pos + 2;
                                    *len_p = len - pos - 2;
                                    return pull_token(ptr_p, len_p, out);
                                }
                                nest_cnt -= 1;
                                pos += 2;
                            }
                        default:
                            goto comment_search;
                    }
                    // Never reaches this line because of gotos
                case '/':
                    size_t pos = 2;
                    while (1) {
                        if (pos == len) {
                            *ptr_p = ptr + len;
                            *len_p = 0;
                            out->tt = TOKEN_EOF;
                            return 0;
                        }
                        if (ptr[pos] == '\n') {
                            *ptr_p = ptr + pos + 1;
                            *len_p = len - pos - 1;
                            return pull_token(ptr_p, len_p, out);
                        }
                        pos++;
                    }
                    // Never reaches this line because of returns
                case '=':
                    // Handled outside operator_assignable for efficiency and simplicity
                    op_id = OP_DIV | OP_ASSIGN;
                    ptr++;
                    len--;
                    goto operator_noassign;
            }
#define _MULTI_OPERATION(c, low, high, is_high_assignable) \
        case c:\
            if (len == 1) {\
                op_id = low;\
                goto operator_noassign;\
            }\
            if (ptr[1] == c) {\
                op_id = high;\
                ptr++;\
                len--;\
                if (is_high_assignable) goto operator_assignable;\
                goto operator_noassign;\
            } else {\
                op_id = low;\
                goto operator_assignable;\
            }
        _MULTI_OPERATION('&', OP_AND, OP_BOAND, true)
        _MULTI_OPERATION('|', OP_OR, OP_BOOR, true)
        // shenanigans time
        _MULTI_OPERATION('<', OP_LT, OP_SHL, true)
        _MULTI_OPERATION('>', OP_GT, OP_SHR, true)
        _MULTI_OPERATION('+', OP_ADD, OP_INC, false)
        _MULTI_OPERATION('-', OP_SUB, OP_DEC, false)
    }
    operator_assignable:
        // len is verifiably > 1
        if (ptr[1] == '=') {
            op_id |= OP_ASSIGN;
            ptr++;
            len--;
        }
    operator_noassign:
        out->tt = TOKEN_OPERATOR;
        out->operator.id = op_id;
        *ptr_p = ptr + 1;
        *len_p = len + 1;
        return 0;
}

#undef MAX_SIZE
#undef _MAX_SIZE

#endif
