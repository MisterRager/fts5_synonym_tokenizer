#include "SynonymTokenizer.h"
#include <cstring>
#include <new>
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1

#define BASE_TOKENIZER "unicode61"

SynonymTokenizer::SynonymTokenizer(
    sqlite3 *db,
    const char *tableName,
    fts5_tokenizer parentTokenizerStruct,
    Fts5Tokenizer* parentTokenizerInstance) :
  m_db(db),
  m_tableName(tableName),
  m_parentTokenizerStruct(parentTokenizerStruct),
  m_parentTokenizerInstance(parentTokenizerInstance) {
}

SynonymTokenizer::~SynonymTokenizer(void) {
  m_parentTokenizerStruct.xDelete(m_parentTokenizerInstance);
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


int xCreate(void *pCtx, const char **azArg, int nArg, Fts5Tokenizer **ppOut) {
  int rc;
  sqlite3 *db = reinterpret_cast<sqlite3*>(pCtx);
  fts5_api *fts5 = fts5_api_from_db(db);
  void *pUnicodeContext = 0;
  fts5_tokenizer unicode;
  Fts5Tokenizer *unicodeInstance;
  void *pTokenizer;
  SynonymTokenizer* tokenizer;

  pTokenizer = (void *) sqlite3_malloc(sizeof(SynonymTokenizer));

  if (pTokenizer) {
    memset(tokenizer, 0, sizeof(SynonymTokenizer));
    rc = fts5->xFindTokenizer(fts5, BASE_TOKENIZER, &pUnicodeContext, &unicode);
  } else {
    rc = SQLITE_NOMEM;
  }

  if (rc == SQLITE_OK) {
    rc = unicode.xCreate(pUnicodeContext, 0, 0, &unicodeInstance);
  }

  if (rc != SQLITE_OK) {
    sqlite3_free(pTokenizer);
  }

  tokenizer = new (pTokenizer) SynonymTokenizer(db, azArg[0], unicode, unicodeInstance);
  *ppOut = (Fts5Tokenizer*) tokenizer;

  return SQLITE_OK;
}

int xTokenize(Fts5Tokenizer *tokenizer_ptr, void *pCtx, int flags, const char *pText, int nText, xTokenFn xToken){
  return 0;
}

void xDelete(Fts5Tokenizer *tokenizer_ptr) {
  SynonymTokenizer *tokenizer = reinterpret_cast<SynonymTokenizer*>(tokenizer_ptr);
  tokenizer->~SynonymTokenizer();
  sqlite3_free(tokenizer_ptr);
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
