#include "SynonymTokenizer.h"
#include <cstring>
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1

SynonymTokenizer::SynonymTokenizer(sqlite3 *db, const char *tableName, Fts5Tokenizer* parentTokenizer) : m_db(db), m_tableName(tableName), m_parentTokenizer(parentTokenizer) {
}

int SynonymTokenizer::tokenize(const char *text, int textLen, void *xToken) {
    return 0;
}

int SynonymTokenizer::addSynonyms(const char **synonyms, int synonymCount) {
    return 0;
}

void SynonymTokenizer::findSynonyms(void* tokenContext, int flags, const char *token, int start, int end, xTokenFn xToken) {
    xToken(tokenContext, flags, token, strlen(token), start, end);
}


int xCreate(void *sqlite3, const char **azArg, int nArg, Fts5Tokenizer **ppOut) {
    int rc;
    SynonymTokenizer* tokenizer = (SynonymTokenizer*) sqlite3_malloc(sizeof(SynonymTokenizer));

    if (tokenizer) {
        memset(tokenizer, 0, sizeof(SynonymTokenizer));
    } else {
        rc = SQLITE_NOMEM;
    }

    return SQLITE_OK;
}

/*
 ** Return a pointer to the fts5_api pointer for database connection db.
 ** If an error occurs, return NULL and leave an error in the database 
 ** handle (accessible using sqlite3_errcode()/errmsg()).
 */
fts5_api *fts5_api_from_db(sqlite3 *db){
    fts5_api *pRet = 0;
    sqlite3_stmt *pStmt = 0;

    if( SQLITE_OK==sqlite3_prepare(db, "SELECT fts5(?)", -1, &pStmt, 0) ){
        sqlite3_bind_pointer(pStmt, 1, (void*)&pRet, "fts5_api_ptr", NULL);
        sqlite3_step(pStmt);
    }

    sqlite3_finalize(pStmt);
    return pRet;
}
