# Mythril's grammar definition in BNF

```
program     = { declaration } ;  
  
declaration = function_decl  
            | struct_decl  
            | enum_decl  
            | impl_decl  
            | module_decl  
            | import_decl  
            | const_decl ;  
  
module_decl = "module" path ";" ;  
import_decl = "import" path ";" ;  
path        = IDENTIFIER { "::" IDENTIFIER } ;
  
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
            | break_stmt
            | continue_stmt
            | return_stmt  
            | expr_stmt ;  
  
var_decl    = "let" IDENTIFIER ":" type [ "=" expression ] ";" ;  
const_decl  = "const" IDENTIFIER ":" type "=" expression ";" ;  
  
lvalue          = { lvalue_prefix } IDENTIFIER { lvalue_postfix } ;

lvalue_prefix   = "*" ;

lvalue_postfix  = "[" expression "]"
                | "."  IDENTIFIER 
                | "->" IDENTIFIER ;

assignment  = lvalue assign_op expression ";" ;  
assign_op   = "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" ;
  
if_stmt     = "if" expression block [ "else" ( if_stmt | block ) ] ;  
  
 
pattern     = IDENTIFIER  
            | literal  
            | IDENTIFIER "(" [ pattern { "," pattern } ] ")"  
            | "_" ;  

match_stmt  = "match" expression "{" { pattern ":" block } [ "_" ":" block ] "}" ;  
  
  
loop_stmt     = "loop" [ statement ] [ expression ] [ statement ] block ;  
break_stmt    = "break" ";" ;  
continue_stmt = "continue" ";" ;  
return_stmt   = "return" [ expression ] ";" ;  
expr_stmt     = expression ";" ;  

expression  = logical_or ;
logical_or  = logical_and { "||" logical_and } ;
logical_and = bitwise_or { "&&" bitwise_or } ;
bitwise_or  = bitwise_xor { "|" bitwise_xor } ;
bitwise_xor = bitwise_and { "^" bitwise_and } ;
bitwise_and = equality { "&" equality } ;
equality    = comparison { ( "==" | "!=" ) comparison } ;
comparison  = shift { ( "<" | "<=" | ">" | ">=" ) shift } ;
shift       = term { ( "<<" | ">>" ) term } ;
term        = factor { ( "+" | "-" ) factor } ;
factor      = unary { ( "*" | "/" | "%" ) unary } ;

unary       = ( "!" | "-" | "~" | "&" | "*" ) unary
            | prefix_op unary
            | postfix ;

prefix_op   = "++" | "--" ;

postfix        = primary { postfix_suffix } ;
postfix_suffix = postfix_op
               | call
               | index
               | member ;

postfix_op  = "++" | "--" ;
call        = "(" arguments ")" ;
arguments   = [ expression { "," expression } ] ;
index       = "[" expression "]" ;
member      = ( "." | "->" ) IDENTIFIER ;

primary     = literal
            | IDENTIFIER
            | "(" expression ")" ;

type        = basic_type
            | pointer_type
            | array_type ;

basic_type   = IDENTIFIER ;
pointer_type = type "*" ;
array_type   = type "[" INTEGER "]" ;

literal     = INTEGER | FLOAT | STRING | CHAR | "true" | "false" | "null" ;
```
