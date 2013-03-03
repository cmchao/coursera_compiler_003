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

%%

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
                        if (yyleng == 1 && yytext[0] == '*') {
                            yymore();
                        } else if (std::strcmp(yytext, "*)") == 0) {
                            BEGIN(INITIAL);
                        }

                        if (yytext[0] == '\n') {
                            curr_lineno++;
                        }
                    }

<ST_COMMENT><<EOF>> {
                        BEGIN(INITIAL);
                        cool_yylval.error_msg = "EOF in comment";
                        return ERROR;
                    }

 /*
  * string
  */ 
\"                  {
                        string_buf_ptr = string_buf;
                        cool_yylval.error_msg = NULL;
                        BEGIN(ST_STR);
                    }
<ST_STR>[^\\"]      {
                        if ((string_buf_ptr - string_buf) == 1024) {
                            cool_yylval.error_msg = "String constant too long";
                        } else if (yytext[0] == '\0') {
                            cool_yylval.error_msg = "String contains null character.";
                        } else {
                            *string_buf_ptr++ = yytext[0];
                        }
                    }
<ST_STR>\n          {
                        /* do nothing */
                    }
<ST_STR>\           {
                         char next = yyinput();
                         switch (next) {
                             case 'b' :
                                *string_buf_ptr++ = '\b';
                                break;
                             case 't' :
                                *string_buf_ptr++ = '\t';
                                break;
                             case 'n' :
                                *string_buf_ptr++ = '\n';
                                break;
                             case 'f' :
                                *string_buf_ptr++ = '\f';
                                break;
                             case '0' :
                                *string_buf_ptr++ = '0';
                                break;
                             case '"' :
                                *string_buf_ptr++ = '0';
                                break;
                             case '\n' :
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
                            string_buf_ptr = '\0';
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
                    

 /*
  * string
  */


 /*
  *  The multiple-character operators.
  */
                


 /* general rule */
[ \n]               {
                        /* do nothing */
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


%%