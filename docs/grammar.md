# Mythril's grammar definition in BNF

program     = { declaration } ;

declaration = function_decl
            | struct_decl
            | enum_decl
            | impl_decl
            | module_decl
            | import_decl
            | const_decl;

module_decl = "module" IDENTIFIER ";" ;
import_decl = "import" IDENTIFIER { "::" IDENTIFIER } ";" ;

field       = IDENTIFIER ":" type ";" ;
struct_decl = "struct" IDENTIFIER "{" { field } "}" ;

variant     = IDENTIFIER [ "(" type { "," type } ")" ] ";" ;
enum_decl   = "enum" IDENTIFIER "{" { variant } "}" ;

impl_decl   = "impl" IDENTIFIER "{" { function_decl } "}" ;

function_decl = "fn" IDENTIFIER "(" parameters ")" ":" type block ;

parameters  =  [ parameter { "," parameter } ] ;
parameter   = IDENTIFIER ":" type ;

block = "{" { statement } "}" ;

statement   = var_decl
            | const_decl
            | assignment
            | if_stmt 
            | match_stmt
            | for_stmt
            | while_stmt
            | return_stmt
            | expr_stmt;

var_decl    = "let" IDENTIFIER ":" type [ "=" expression ] ";" ;
const_decl  = "const" IDENTIFIER ":" type "=" expression ";" ;

lvalue      = postfix
assignment  = lvalue assign_op expression ";" ;
assign_op   = "=" | "+=" | "-=" | "*=" | "/=" ;

if_stmt     = "if" expression block [ "else" ( if_stmt | block ) ] ;

pattern     = IDENTIFIER
            | literal
            | IDENTIFIER "(" [ pattern { "," pattern } ] ")"
            | "_" ;
match_stmt  = "match" expression "{" { pattern ":" block } [ "_" ":" block ] "}" ;

for_stmt    = "for" IDENTIFIER "in" expression block ;
while_stmt  = "while" expression block ;
return_stmt = "return" [ expression ] ";" ;
expr_stmt   = expression ";" ;

expression  = logical_or
logical_or  = logical_and { "||" logical_and } ;
logical_and = equality { "&&" equality } ;
equality    = comparison { ( "==" | "!=" ) comparison } ;
comparison  = term { ( "<" | "<=" | ">" | ">=" ) term } ;
term        = factor { ( "+" | "-" ) factor } ;
factor      = unary { ( "*" | "/" | "%" ) unary } ;
unary       = ( "!" | "-" | "++" | "--" | "&" | "*" ) unary
            | postfix ;
postfix_op  = "--"
            | "++"
            | call
            | index
            | member;
postfix     = primary { postfix_op } ;
call        = "(" arguments ")" ;
arguments   = [ expression { "," expression } ] ;
index       = "[" expression "]" ;
member      = ( "." | "->" ) IDENTIFIER ;
primary     = literal | IDENTIFIER | "(" expression ")" ;

type        = IDENTIFIER { "*" } ;

literal     = INTEGER | FLOAT | STRING | "true" | "false" | "null" ;
