#include "sqlite3.h"

typedef int (*xTokenFn)(void*,int,const char*,int,int,int);
typedef void (*useSynonymFn)(const char*);

class SynonymTokenizer {
    private:
        sqlite3 *m_db;
        const char *m_tableName;
        void findSynonyms(const char *token, useSynonymFn useSynonym);
    public:
        SynonymTokenizer(sqlite3 *db, const char *tableName);
        int tokenize(const char *text, int textLen, void *xToken);
        int addSynonyms(const char **synonyms, int synonymCount);
        ~SynonymTokenizer();
};

extern "C" int xCreateTokenizer(fts5_api *pApi, const char *zName, void *pContext, fts5_tokenizer *pTokenizer, void (*xDestroy)(void*));
extern "C" int xCreate(void *sqlite3, const char **azArg, int nArg, Fts5Tokenizer **ppOut);
extern "C" int xTokenize(Fts5Tokenizer *tokenizer_ptr, void *pCtx, int flags, const char *pText, int nText, xTokenFn xToken);
extern "C" void xDelete(Fts5Tokenizer *tokenizer_ptr);
