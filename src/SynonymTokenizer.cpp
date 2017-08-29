#include "SynonymTokenizer.h"


SynonymTokenizer::SynonymTokenizer(sqlite3 *db, const char *tableName) : m_db(db), m_tableName(tableName) {
}

int SynonymTokenizer::tokenize(const char *text, int textLen, void *xToken) {
    return 0;
}

int SynonymTokenizer::addSynonyms(const char **synonyms, int synonymCount) {
    return 0;
}

void SynonymTokenizer::findSynonyms(const char* token, useSynonymFn useSynonym) {
}

int main() {
    return 0;
}
