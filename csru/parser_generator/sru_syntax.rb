# Generate by yapp.
symbol :program, :statements, :statement, :let_statement, :flow_statement, :if_statement,
       :while_statement, :class_statement, :def_statement, :expression, :if_main,
       :bool_term, :comp, :bit_sim, :bit_term, :bit_shift, :sim, :term, :factor, 
       :instance_method, :instance_call, :method_call, :proc_call,
       :primary, :primitive, :reference, :literal,
       :closure_literal, :closure_varg, :closure_retarg, :const_literal
# Implimented in C++
symbol :spc_or_lf, :spc, :ident, :number, :real, :const_string

program <= statement

statements <= r( spc_or_lf * ~"end" * ~"else" * ~"elsif" * statement )

statement <= spc_or_lf * ( let_statement | flow_statement )

let_statement <= (
  flow_statement  |  # only returns ref
  spc_or_lf * ident
) * spc * "=" * spc_or_lf * statement

flow_statement <= if_statement | while_statement | class_statement | def_statement | expression

if_statement <= "if" * if_main

if_main <= spc_or_lf * "(" * spc_or_lf * statement * spc_or_lf * ")" *
statements * spc_or_lf *
(
  "elsif" * if_main |
  "else" * statements * spc_or_lf * "end" |
  "end"
)

while_statement <= "while" * spc_or_lf * "(" * spc_or_lf * statement * spc_or_lf * ")" *
spc_or_lf * statements * spc_or_lf *
spc_or_lf * "end"

class_statement <= "class" *
(
  spc_or_lf * statement * "." * ident |
  spc_or_lf * ident
) * o( ":" * spc_or_lf * statement) *
r( "def" * spc_or_lf * ident * spc_or_lf *
  "(" * spc_or_lf * o( ident * spc_or_lf *
  r( "," * spc_or_lf * ident ) ) * ")" *
  spc_or_lf * statements * spc_or_lf * "end"
) * spc_or_lf * "end"

def_statement <=
"def" * spc_or_lf * ident *
"(" * spc_or_lf * o( ident * spc_or_lf *
r( "," * spc_or_lf * ident ) ) * ")" *
spc_or_lf * statements * spc_or_lf * "end"

expression <= bool_term * r( spc * "||" * spc_or_lf * bool_term )

bool_term <= comp * r( spc * "&&" * spc_or_lf * comp )

comp <= bit_sim * r( spc * (
"==" * spc_or_lf * bit_sim |
"===" * spc_or_lf * bit_sim |
"!=" * spc_or_lf * bit_sim |
"<" * spc_or_lf * bit_sim |
">" * spc_or_lf * bit_sim |
"<=" * spc_or_lf * bit_sim |
">=" * spc_or_lf * bit_sim ) 
)

bit_sim <= bit_term * r( spc * "|" * bit_term )

bit_term <= bit_shift * r(spc * "&" * bit_shift )

bit_shift <= sim * r( spc * (
"<<" * spc_or_lf * sim |
">>" * spc_or_lf * sim )
)

sim <= term * r( spc * (
"+" * spc_or_lf * term |
"-" * spc_or_lf * term )
)

term <= factor * r( spc * (
"*" * spc_or_lf * factor |
"/" * spc_or_lf * factor ) )

factor <=
spc * "!" * spc_or_lf * factor |
spc * "~" * spc_or_lf * factor |
instance_method

instance_method <=
(
spc * "." * spc_or_lf * instance_call |
method_call
) * "." *  spc_or_lf * instance_call

instance_call <=
spc * ident *
o(
"(" * spc_or_lf * statement * spc_or_lf *
r( "," * spc_or_lf * statement * spc_or_lf ) *
spc_or_lf * ")"
) *
proc_call

method_call <=
primary * proc_call

proc_call <=
spc * r(
"(" *
o( statement * r( spc_or_lf * "," * statement ) ) *
spc_or_lf * ")" |
"[" * statement * spc_or_lf * "]" )

primary <=
primitive |
spc * "-" * primary |
"(" * statement * spc_or_lf * ")"

primitive <= reference | literal

literal <= closure_literal | const_literal

reference <= ident

closure_literal <=
spc * "{" *
o(closure_varg) *
r( spc_or_lf * statement ) *
 spc_or_lf * "}"

closure_varg <= spc * o( "|" ) *
o( spc_or_lf * ident *
r( spc_or_lf * "," * spc_or_lf * ident ) ) *
o( closure_retarg ) * spc_or_lf * "|"

closure_retarg <= ":" * ident

const_literal <= number | real | const_string

