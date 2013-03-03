/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
#include <cstring>

static bool str_outofbuf(void)
{
    return (string_buf_ptr - string_buf) == MAX_STR_CONST;
}

%}

/*
 * Define names for regular expressions here.
 */

DARROW          =>

/*
 * lex condition
 */

%x ST_COMMENT
%x ST_STR

KType       [A-Z][A-Za-z0-9_]*
KObject     [a-z][A-Za-z0-9_]*
KClass      [Cc][Ll][Aa][Ss]{2,2}
KInherits   [Ii][Nn][Hh][Ee][Rr][Ii][Tt][Ss]
KIf         [Ii][Ff]
KFi         [Ff][Ii]
KThen       [Tt][Hh][Ee][Nn]
KElse       [Ee][Ll][Ss][Ee]
KWhile      [Ww][Hh][Ii][Ll][Ee]
KCase       [Cc][Aa][Ss][Ee]
KEsac       [Ee][Ss][Aa][Cc]
KOf         [Oo][Ff]
KLoop       [Ll][Oo][Oo][Pp]
KPool       [Pp][Oo][Oo][Ll]
KLet        [Ll][Ee][Tt]
KNew        [Nn][Ee][Ww]
KIn         [Ii][Nn]
KIsvoid     [Ii][Ss][Vv][Oo][Ii][Dd]
KNot        [Nn][Oo][Tt]
KTrue       t[Rr][Uu][Ee]
KFalse      f[Aa][Ll][Ss][Ee]

%%
 /*
  * keyword
  */
{KClass}            return CLASS;
{KInherits}         return INHERITS;
{KIf}               return IF;
{KFi}               return FI;
{KThen}             return THEN;
{KElse}             return ELSE;
{KWhile}            return WHILE;
{KCase}             return CASE;
{KEsac}             return ESAC;
{KOf}               return OF;
{KLoop}             return LOOP;
{KPool}             return POOL;
{KLet}              return LET;
{KNew}              return NEW;
{KIn}               return IN;
{KIsvoid}           return ISVOID;
{KNot}              return NOT;
{KTrue}             { cool_yylval.boolean = true; return BOOL_CONST; }
{KFalse}            { cool_yylval.boolean = false; return BOOL_CONST; }
 /*
  *  comments
  */
\(\*                {
                        BEGIN(ST_COMMENT);
                    }
--.*\n              {
                        curr_lineno++;
                        /* do nothing */
                    }

\*\)                {
                        cool_yylval.error_msg = "Unmatched *)";
                        return ERROR;
                    }

<ST_COMMENT>(.|\n)  { 
                        if (yytext[0] == '*') {
                            char next = yyinput();
                            if (next == ')') {
                                BEGIN(INITIAL);
                            } else {
                                unput(next);
                            }
                        } else if (yytext[0] == '\n') {
                            curr_lineno++;
                        }
                    }

<ST_COMMENT><<EOF>> {
                        BEGIN(INITIAL);
                        cool_yylval.error_msg = "EOF in comment";
                        return ERROR;
                    }

 /*
  *  The multiple-character operators.
  */
"*"                 return '*';
"+"                 return '+';
"-"                 return '-';
"/"                 return '/';
"~"                 return '~';
"."                 return '.';
"@"                 return '@';
"<"                 return '<';
"<="                return LE;
"("                 return '(';
")"                 return ')';
"{"                 return '{';
"}"                 return '}';
";"                 return ';';
"="                 return '=';
"<-"                return ASSIGN;
":"                 return ':';
","                 return ',';

{KType}             {
                        cool_yylval.symbol = idtable.add_string(yytext);
                        return TYPEID;
                    }

{KObject}           {
                        cool_yylval.symbol = idtable.add_string(yytext);
                        return OBJECTID;
                    }

[0-9]+              {
                        cool_yylval.symbol = inttable.add_string(yytext);
                        return INT_CONST;
                    }
                    


{DARROW}		{ return (DARROW); }

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */


 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */

\"                  {
                        string_buf_ptr = string_buf;
                        cool_yylval.error_msg = NULL;
                        BEGIN(ST_STR);
                    }
<ST_STR>\n          {
                        cool_yylval.error_msg = "Unterminated string constant";
                        BEGIN(INITIAL);
                        curr_lineno++;
                        return ERROR;
                        /* do nothing */
                    }
<ST_STR>\\          {
                         char next = yyinput();
                         switch (next) {
                             case 'b' :
                                if (!str_outofbuf())
                                    *string_buf_ptr++ = '\b';
                                break;
                             case 't' :
                                if (!str_outofbuf())
                                    *string_buf_ptr++ = '\t';
                                break;
                             case 'n' :
                                if (!str_outofbuf())
                                    *string_buf_ptr++ = '\n';
                                break;
                             case 'f' :
                                if (!str_outofbuf())
                                    *string_buf_ptr++ = '\f';
                                break;
                             case '0' :
                                if (!str_outofbuf())
                                    *string_buf_ptr++ = '0';
                                break;
                             case '"' :
                                if (!str_outofbuf())
                                    *string_buf_ptr++ = '"';
                                break;
                             case '\n' :
                                if (!str_outofbuf())
                                    *string_buf_ptr++ = '\n';
                                curr_lineno++;
                                break;
                             default : 
                                unput(next);
                                break;
                         }
                    }
<ST_STR>\"          {
                        if (cool_yylval.error_msg != NULL) {
                            BEGIN(INITIAL);
                            return ERROR;
                        } else {
                            *string_buf_ptr = '\0';
                            cool_yylval.symbol = stringtable.add_string(string_buf);
                            BEGIN(INITIAL);
                            return STR_CONST;
                        }
                    }

<ST_STR><<EOF>>     {
                        BEGIN(INITIAL);
                        cool_yylval.error_msg = "Unterminated string constant";
                        return ERROR;
                    }

<ST_STR>[^\\"\n]    {
                        //printf("yes : %s\n", yytext);
                        if (str_outofbuf()) {
                            cool_yylval.error_msg = "String constant too long";
                        } else if (yytext[0] == '\0') {
                            cool_yylval.error_msg = "String contains null character.";
                        } else {
                            *string_buf_ptr++ = yytext[0];
                        }
                    }
                    

 /* general rule */
[ \n\t]             {
                        if (yytext[0] == '\n') {
                            curr_lineno++;
                        }
                    }

.                   {
                        cool_yylval.error_msg = yytext;
                        return ERROR;
                    }


%%
