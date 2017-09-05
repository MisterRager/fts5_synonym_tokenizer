#include "SynonymTokenizer.h"
#include <cstring>
#include <cstdio>
#include <new>
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1

#define BASE_TOKENIZER "unicode61"
#define SYNONYM_COLUMN "synonym"
#define ID_COLUMN "id"

#define SYNONYM_QUERY_TEMPLATE (\
  "SELECT tbl." SYNONYM_COLUMN " FROM %1$s tbl " \
  "JOIN %1$s jtbl ON tbl." ID_COLUMN "=tbl2." ID_COLUMN " " \
  "WHERE tbl2." SYNONYM_COLUMN "=?")

const size_t SYNONYM_QUERY_TMPL_LEN = strlen(SYNONYM_QUERY_TEMPLATE);

SynonymTokenizer::SynonymTokenizer(
    sqlite3 *db,
    const char *tableName,
    fts5_tokenizer parentTokenizerStruct,
    Fts5Tokenizer* parentTokenizerInstance) :
  m_db(db),
  m_tableName(tableName),
  m_parentTokenizerStruct(parentTokenizerStruct),
  m_parentTokenizerInstance(parentTokenizerInstance) {
  // Build synonyms query
  this->m_synonymQueryLength = SYNONYM_QUERY_TMPL_LEN + (strlen(this->m_tableName) * 2) + 1;
  this->m_synonymQuery = (char *) sqlite3_malloc(this->m_synonymQueryLength);
  sprintf(this->m_synonymQuery, SYNONYM_QUERY_TEMPLATE, this->m_tableName);
}

SynonymTokenizer::~SynonymTokenizer(void) {
  m_parentTokenizerStruct.xDelete(m_parentTokenizerInstance);
  sqlite3_free(this->m_synonymQuery);
}

int SynonymTokenizer::addSynonyms(const char **synonyms, int synonymCount) {
  return 0;
}

typedef struct xtoken_unicode61_context xtoken_unicode61_context;
struct xtoken_unicode61_context {
  SynonymTokenizer *tokenizer;
  xTokenFn xToken;
};

int xtoken_unicode61_callback(void *pCtx, int tflags, const char* pToken, int nToken, int iStart, int iEnd) {
  xtoken_unicode61_context *context = reinterpret_cast<xtoken_unicode61_context *>(pCtx);
  return context->tokenizer->findSynonyms(pCtx, tflags, pToken, iStart, iEnd, context->xToken);
}

int SynonymTokenizer::tokenize(int flags, const char *text, int textLen, xTokenFn xToken) {
  xtoken_unicode61_context context = {
    this,
    xToken
  };
  return this->m_parentTokenizerStruct.xTokenize(this->m_parentTokenizerInstance, &context, flags, text, textLen, xtoken_unicode61_callback);
}

int SynonymTokenizer::findSynonyms(void* tokenContext, int flags, const char *token, int start, int end, xTokenFn xToken) {
  int status;
  sqlite3_stmt *stmt;

  status = sqlite3_prepare_v2(this->m_db, this->m_synonymQuery, this->m_synonymQueryLength, &stmt, NULL);

  if (status != SQLITE_OK) {
    return status;
  }

  status = sqlite3_bind_text(stmt, 1, token, strlen(token), NULL);

  if (status != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return status;
  }

  for(int status = sqlite3_step(stmt); status == SQLITE_ROW; status = sqlite3_step(stmt)) {
    int bytes = sqlite3_column_bytes(stmt, 1);;
    const unsigned char *text_unsigned = sqlite3_column_text(stmt, 1);
    const char *text = text_unsigned ? reinterpret_cast<const char *>(text_unsigned) : NULL;
    xToken(tokenContext, flags, text, bytes, start, end);
  }

  return sqlite3_finalize(stmt);
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
  SynonymTokenizer *tokenizer = reinterpret_cast<SynonymTokenizer*>(tokenizer_ptr);
  //return tokenizer->tokenize
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
