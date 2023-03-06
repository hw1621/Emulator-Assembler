#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "translation.h"

// Reads whole text file into a `String`
bool readFile(const char* filename, String* s) {
  FILE* in = NULL;
  long size; // `ftell` only returns `long`... could be a problem with large files

  // The tokenizer will handle '\r' as well as '\n', using binary mode should be fine
  // (it is actually required since we check the file size against the return value of `fread`).
  if ((in = fopen(filename, "rb")) == NULL)      goto fail;
  if (fseek(in, 0, SEEK_END) != 0)               goto fail;
  if ((size = ftell(in)) == -1)                  goto fail;
  StringResize(s, size);
  if (fseek(in, 0, SEEK_SET) != 0)               goto fail;
  if (fread(s->p, 1, size, in) != (size_t) size) goto fail;
  if (fclose(in) != 0)                           goto fail;
  return true;

fail:
  if (in != NULL) fclose(in);
  return false;
}

// Write the contents of `ByteVector` to the destination .bin file
bool writeBinary(ByteVector* a, char* filename) {
  FILE* out = NULL;
  if ((out = fopen(filename, "wb")) == NULL)     goto fail;
  if (fwrite(a->p, 1, a->size, out) != a->size)  goto fail;
  if (fclose(out) != 0)                          goto fail;
  return true;

fail:
  if (out != NULL) fclose(out);
  return false;
}

bool assembleFile(const char* filename, Context* ctx) {
  String s;
  StringInit(&s);
  if (!readFile(filename, &s)) {
    printf("Error: could not read file \"%s\"\n", filename);
    goto fail1;
  }
  // Make sure `s.p` is terminated by a `\0`, which is required by the tokenizer
  // (zero-initialisation is automatic in a `String`).
  StringResize(&s, s.size + 1);

  Parser parser;
  ParserInit(&parser, s.p);

  while (true) {
    if (parser.next.kind == TOK_EOF) break;
    if (parser.next.kind == TOK_IDENTIFIER) {
      if (parser.next2.kind == TOK_STRING) { // TODO: check that the content of `parser.next` is indeed "include"
        // Included file name
        size_t len = parser.next2.end - parser.next2.start - 2;
        char* buf = malloc(len + 1);
        strncpy(buf, parser.next2.start + 1, len);
        buf[len] = '\0';
        // Recursively process files
        bool res = assembleFile(buf, ctx);
        free(buf);
        if (!res) goto fail;
        // Eats the "include" and filename tokens
        ParserAdvance(&parser);
        ParserAdvance(&parser);
        continue;
      }
      if (parser.next2.kind == TOK_COLON) {
        if (!label(&parser, ctx)) goto fail;
        continue;
      }
      if (!instruction(&parser, ctx)) goto fail;
      continue;
    }
    printf("Parsing error: unexpected (%s, %s)\n", tokenName(parser.next.kind), tokenName(parser.next2.kind));
    goto fail;
  }

  ParserFree(&parser);
  StringFree(&s);
  return true;

fail:
  ParserFree(&parser);
  // Fall through
fail1:
  StringFree(&s);
  return false;
}

int main(int argc, char** argv) {
  Context ctx; // Assembled data
  ContextInit(&ctx);
  ByteVector a; // Output container
  ByteVectorInit(&a);

  if (argc != 3) {
    printf("Usage: ./assemble <input-file> <output-file>\n");
    goto fail;
  }
  if (!assembleFile(argv[1], &ctx)) {
    printf("Error: error(s) occurred in assembly\n");
    goto fail;
  }
  if (!link(&ctx, &a)) {
    printf("Error: error(s) occurred in linking\n");
    goto fail;
  }
  if (!writeBinary(&a, argv[2])) {
    printf("Error: could not write destination file \"%s\"\n", argv[2]);
    goto fail;
  }

  ContextFree(&ctx);
  ByteVectorFree(&a);
  return EXIT_SUCCESS;

fail:
  ContextFree(&ctx);
  ByteVectorFree(&a);
  return EXIT_FAILURE;
}
