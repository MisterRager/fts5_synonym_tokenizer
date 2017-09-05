#include "sqlite3ext.h"

typedef int (*xTokenFn)(void*,int,const char*,int,int,int);

class SynonymTokenizer {
    private:
        sqlite3* m_db;
        const char* m_tableName;
        fts5_tokenizer m_parentTokenizerStruct;
        Fts5Tokenizer* m_parentTokenizerInstance;
        int m_synonymQueryLength;
        char* m_synonymQuery;
    public:
        SynonymTokenizer(
            sqlite3 *db,
            const char *tableName,
            fts5_tokenizer parentTokenizerStruct,
            Fts5Tokenizer* parentTokenizerInstance);
        ~SynonymTokenizer();
        int tokenize(int flags, const char *text, int textLen, xTokenFn xToken);
        int addSynonyms(const char **synonyms, int synonymCount);
        int findSynonyms(void* tokenContext, int flags, const char *token, int start, int end, xTokenFn xtoken);
};

fts5_api *fts5_api_from_db(sqlite3 *db);

extern "C" int xCreate(void *sqlite3, const char **azArg, int nArg, Fts5Tokenizer **ppOut);
extern "C" int xTokenize(Fts5Tokenizer *tokenizer_ptr, void *pCtx, int flags, const char *pText, int nText, xTokenFn xToken);
extern "C" void xDelete(Fts5Tokenizer *tokenizer_ptr);
extern "C" int xtoken_unicode61_callback(void *pCtx, int tflags, const char* pToken, int nToken, int iStart, int iEnd);
