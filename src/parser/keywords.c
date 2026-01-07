#include "keywords.h"

#include "lefthand/keywords.h"
#include "statement/keywords.h"

KeywordTable global_keyword_table = 0;

// TODO: call in main()
void populate_keyword_table() {
    put(&global_keyword_table, String("auto"), (Keyword) { KeywordActionNone, &keyword_auto });
    put(&global_keyword_table, String("int"), (Keyword) { KeywordActionNone, &keyword_int });
    put(&global_keyword_table, String("typeof"), (Keyword) { KeywordActionNone, &keyword_typeof });
    put(&global_keyword_table, String("sizeof"), (Keyword) { KeywordActionNone, &keyword_sizeof });
    put(&global_keyword_table, String("const"), (Keyword) { KeywordActionNone, &keyword_const });
    put(&global_keyword_table, String("extern"), (Keyword) { KeywordActionNone, &keyword_extern });
    put(&global_keyword_table, String("self"), (Keyword) { KeywordActionSelf, &keyword_self });

    put(&global_keyword_table, String("import"), (Keyword) { KeywordActionStatement, &keyword_import });
    put(&global_keyword_table, String("return"), (Keyword) { KeywordActionStatement, &keyword_return });
    put(&global_keyword_table, String("struct"), (Keyword) { KeywordActionStatement, &keyword_struct });
    put(&global_keyword_table, String("if"), (Keyword) { KeywordControlSingleCond, &keywords_control });
    put(&global_keyword_table, String("while"), (Keyword) { KeywordControlSingleCond, &keywords_control });
    put(&global_keyword_table, String("for"), (Keyword) { KeywordControlTripleCond, &keywords_control });
    put(&global_keyword_table, String("type"), (Keyword) { KeywordActionStatement, &keyword_type });
}