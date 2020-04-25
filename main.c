#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// tokenizer
#define PROGRAM_TOTAL_CHARS 1024
#define PROGRAM_TOTAL_TOKENS 256
#define PROGRAM_TOTAL_CHARS_PER_TOKEN 32

// parser
#define PROGRAM_TOTAL_STATEMENT 128

#define TOKEN_IDENTFIER		"IDENTFIER"
#define TOKEN_NUMBER		"NUMBER"
#define TOKEN_LOCAL		 	"LOCAL"
#define TOKEN_FUNCTION		"FUNCTION"
#define TOKEN_RETURN		"RETURN"
#define TOKEN_END			"END"
#define TOKEN_IF			"IF"
#define TOKEN_THEN			"THEN"
#define TOKEN_EQUALS		"EQUALS"
#define TOKEN_LEFT_P		"("
#define TOKEN_RIGHT_P		")"
#define TOKEN_ASSIGNMENT	"="
#define TOKEN_SUM			"+"
#define TOKEN_MUL			"*"
#define TOKEN_SUB			"-"
#define TOKEN_DIV			"/"
#define TOKEN_COMMA			","
#define TOKEN_EOF			"EOF"
#define TOKEN_UNKNOWN		"UNKNOWN"

#define STATEMENT_LOCAL "LOCAL"
#define STATEMENT_IF "IF"


struct Token {
	char* type;
	char* literal;
} typedef Token;

Token token_new(char* literal) {
	Token t;
	t.literal = literal;
	t.type = TOKEN_IDENTFIER;
	if (strcmp(literal, "local") == 0) t.type = TOKEN_LOCAL;
	else if (strcmp(literal, "function") == 0) t.type = TOKEN_FUNCTION;
	else if (strcmp(literal, "return") == 0) t.type = TOKEN_RETURN;
	else if (strcmp(literal, "end") == 0) t.type = TOKEN_END;
	else if (strcmp(literal, "if") == 0) t.type = TOKEN_IF;
	else if (strcmp(literal, "then") == 0) t.type = TOKEN_THEN;
	else if (strcmp(literal, "==") == 0) t.type = TOKEN_EQUALS;
	else if (strcmp(literal, "(") == 0) t.type = TOKEN_LEFT_P;
	else if (strcmp(literal, ")") == 0) t.type = TOKEN_RIGHT_P;
	else if (strcmp(literal, "=") == 0) t.type = TOKEN_ASSIGNMENT;
	else if (strcmp(literal, "+") == 0) t.type = TOKEN_SUM;
	else if (strcmp(literal, "-") == 0) t.type = TOKEN_SUB;
	else if (strcmp(literal, "*") == 0) t.type = TOKEN_MUL;
	else if (strcmp(literal, "/") == 0) t.type = TOKEN_DIV;
	else if (strcmp(literal, ",") == 0) t.type = TOKEN_COMMA;
	else if ((int)literal[0] >= 48 && (int)literal[0] <= 57) t.type = TOKEN_NUMBER;
	return t;
}

struct ASTNode {
	struct ASTNode* parent;
	char* type;
	
	Token identifier;
	Token value;
	Token comparisonOperator;
} typedef ASTNode;

struct Program {
	unsigned int currentTokenIndex;
	unsigned int totalTokens;

	Token tokens[PROGRAM_TOTAL_TOKENS];

	unsigned int currentStatementIndex;
	ASTNode* statements;
} typedef Program;

ASTNode parseStatement(Program* p);

ASTNode parseLocalStatement(Program* p) {
	ASTNode stmt;
	stmt.type = STATEMENT_LOCAL;

	++p->currentTokenIndex;
	stmt.identifier = p->tokens[p->currentTokenIndex];

	++p->currentTokenIndex;
	if (p->tokens[p->currentTokenIndex].type == TOKEN_ASSIGNMENT) {
		++p->currentTokenIndex;
		stmt.value = p->tokens[p->currentTokenIndex];
	}

	return stmt;
}

ASTNode parseIfStatement(Program* p) {
	ASTNode stmt;
	stmt.type = STATEMENT_IF;

	++p->currentTokenIndex;
	stmt.identifier = p->tokens[p->currentTokenIndex];

	++p->currentTokenIndex;
	if (p->tokens[p->currentTokenIndex].type == TOKEN_EQUALS) {
		stmt.comparisonOperator = p->tokens[p->currentTokenIndex];
		++p->currentTokenIndex;
		stmt.value = p->tokens[p->currentTokenIndex];
		++p->currentTokenIndex;
	} 
	// else if add more comparison operators

	assert(p->tokens[p->currentTokenIndex].type == TOKEN_THEN);
	++p->currentTokenIndex;

	while(1) {
		ASTNode stmt = parseStatement(p);
		assert(strcmp(stmt.type, "EOF") != 0);
		if(strcmp(stmt.type, "UNKNOWN") != 0) {
			p->currentStatementIndex++;
			p->statements[p->currentStatementIndex] = stmt;
		}
		if (strcmp(p->tokens[p->currentTokenIndex].type, "END") == 0) {
			++p->currentTokenIndex;
			break;
		}
		++p->currentTokenIndex;
	}

	return stmt;
}

ASTNode parseStatement(Program* p) {
	ASTNode stmt;
	stmt.type = TOKEN_UNKNOWN;
	if (p->currentTokenIndex >= p->totalTokens) {
		stmt.type = TOKEN_EOF;
		return stmt;
	}
	
	Token currentToken = p->tokens[p->currentTokenIndex];
	if (currentToken.type == TOKEN_LOCAL) return parseLocalStatement(p);
	else if (currentToken.type == TOKEN_IF) return parseIfStatement(p);

	return stmt;
}

int main() {
	char c[256] = "\
local c = 10 \
if x == 10 then \
	print(true) \
end \
function sum(x, y) \
	return x + y \
end;";

	char token[PROGRAM_TOTAL_TOKENS][PROGRAM_TOTAL_CHARS_PER_TOKEN] = {};
	unsigned int idx = -1;
	unsigned int tokenIndex = 0;
	while(1) {
		idx++;
		if (c[idx] == ';') break;
		if (c[idx] == ' ') {
			if (strlen(token[tokenIndex]) > 0) {
				tokenIndex++;
			}
			continue;
		}
		if (c[idx] == '\n' || c[idx] == '\t') {
			if (strlen(token[tokenIndex]) > 0) {
				tokenIndex++;
			}
			continue;
		}
		if (c[idx] == '(' || c[idx] == ')' || c[idx] == ',') {
			tokenIndex++;
			strncat(token[tokenIndex], &c[idx], 1);
			tokenIndex++;
			continue;
		}
		strncat(token[tokenIndex], &c[idx], 1);
	}

	Program p;
	p.currentTokenIndex = -1;
	p.totalTokens = tokenIndex + 1;

	p.statements = malloc(sizeof(ASTNode));
	p.currentStatementIndex = -1;

	// print tokens
	printf("Total tokens: %u\n", p.totalTokens);
	printf("Tokens:\n");
	for (unsigned int i = 0; i < p.totalTokens; i++) {
		p.tokens[i] = token_new(token[i]);
		printf("%u\t%lu\t%s\t\t%s\n", i, strlen(p.tokens[i].literal), p.tokens[i].literal, p.tokens[i].type);
	}

	// parsing..
	while(1) {
		p.currentTokenIndex++;
		ASTNode stmt = parseStatement(&p);
		if(strcmp(stmt.type, "EOF") == 0) break;
		if(strcmp(stmt.type, "UNKNOWN") != 0) {
			p.currentStatementIndex++;
			p.statements[p.currentStatementIndex] = stmt;
		}
	}

	unsigned int totalStatements = p.currentStatementIndex + 1;
	for (unsigned int i = 0; i < totalStatements; i++)
		printf("index=%u type=%s identifier=%s value=%s\n", i, p.statements[i].type, p.statements[i].identifier.literal, p.statements[i].value.literal);

	free(p.statements);

	return 0;
}

