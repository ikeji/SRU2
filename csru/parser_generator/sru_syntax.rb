# Generate by yapp.
symbol :program, :statements, :statement, :let_statement, :flow_statement, :if_statement,
       :while_statement, :class_statement, :def_statement, :expression, :if_main,
       :bool_term, :comp, :bit_sim, :bit_term, :bit_shift, :sim, :term, :factor, 
       :instance_method, :instance_call, :method_call, :proc_call,
       :primary, :primitive, :reference, :literal,
       :closure_literal, :closure_varg, :closure_retarg, :const_literal
# Implimented in C++
symbol :spc_or_lf, :spc, :ident, :number, :real, :const_string

program <= statements

#statements <= r( spc_or_lf * ~"end" * ~"else" * ~"elsif" * statement )
statements <= spc_or_lf * statement

#statement <= spc_or_lf * ( let_statement | flow_statement )
statement <= flow_statement

let_statement <= (
  flow_statement  |  # only returns ref
  spc_or_lf * ident
) * spc * "=" * spc_or_lf * statement

#flow_statement <= if_statement | while_statement | class_statement | def_statement | expression
flow_statement <= expression

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

#expression <= bool_term * r( spc * "||" * spc_or_lf * bool_term )
expression <= bool_term

#bool_term <= comp * r( spc * "&&" * spc_or_lf * comp )
bool_term <= comp

#comp <= bit_sim * r( spc * (
#"==" * spc_or_lf * bit_sim |
#"===" * spc_or_lf * bit_sim |
#"!=" * spc_or_lf * bit_sim |
#"<" * spc_or_lf * bit_sim |
#">" * spc_or_lf * bit_sim |
#"<=" * spc_or_lf * bit_sim |
#">=" * spc_or_lf * bit_sim ) 
#)
comp <= bit_sim

#bit_sim <= bit_term * r( spc * "|" * bit_term )
bit_sim <= bit_term

#bit_term <= bit_shift * r(spc * "&" * bit_shift )
bit_term <= bit_shift

#bit_shift <= sim * r( spc * (
#"<<" * spc_or_lf * sim |
#">>" * spc_or_lf * sim )
#)
bit_shift <= sim

#sim <= term * r( spc * (
#"+" * spc_or_lf * term |
#"-" * spc_or_lf * term )
#)
sim <= term

#term <= factor * r( spc * (
#"*" * spc_or_lf * factor |
#"/" * spc_or_lf * factor ) )
term <= factor

#factor <=
#spc * "!" * spc_or_lf * factor |
#spc * "~" * spc_or_lf * factor |
#instance_method
factor <= instance_method

#instance_method <=
#(
#spc * "." * spc_or_lf * instance_call |
#method_call
#) * "." *  spc_or_lf * instance_call
manipulator :instance_method_implicit_self, :instance_method_self
instance_method <=
((
  spc * "." * spc_or_lf * ident *
  instance_method_implicit_self(:ident)
)|(
  method_call
))

instance_call <=
spc * ident *
o(
"(" * spc_or_lf * statement * spc_or_lf *
r( "," * spc_or_lf * statement * spc_or_lf ) *
spc_or_lf * ")"
) *
proc_call

proc_call <=
spc * r(
"(" *
o( statement * r( spc_or_lf * "," * statement ) ) *
spc_or_lf * ")" |
"[" * statement * spc_or_lf * "]" )

manipulator :method_call_primary, :method_call_method_begin, :method_call_method_arg, :method_call_method_end, :method_call_method_index, :method_call_end
method_call <=
primary * method_call_primary(:primary) *
spc * r((
  "(" * method_call_method_begin(:primary) *
  o(
    statement * method_call_method_arg(:method_call_method_begin, :statement) * 
    r(
      spc_or_lf * "," * statement * method_call_method_arg(:method_call_method_begin, :statement)
    )
  ) *
  spc_or_lf * ")" * method_call_method_end(:method_call_primary, :method_call_method_begin)
)|(
  "[" * statement * spc_or_lf * "]" * method_call_method_index(:method_call_primary, :statement)
)) * method_call_end(:method_call_primary)

manipulator :primary_minous, :parent
primary <=
primitive |
spc * (
"-" * primary * primary_minous(:primary) |
"(" * spc_or_lf * statement * spc_or_lf * ")" * parent(:statement)
)

primitive <= reference | literal

literal <= closure_literal | const_literal

reference <= ident

manipulator :closure_begin, :closure_merge_varg, :closure_statement, :closure_end
closure_literal <=
spc * "{" * closure_begin *
o( closure_varg * closure_merge_varg(:closure_begin, :closure_varg)) *
r(
  spc_or_lf * statement * 
  closure_statement(:closure_begin, :statement)
) * spc_or_lf * "}" * closure_end(:closure_begin)

manipulator :closure_varg_begin, :closure_varg_idents,
            :closure_varg_retarg, :closure_varg_end
closure_varg <= spc * o( "|" ) * closure_varg_begin *
o( spc_or_lf * ident * closure_varg_idents(:closure_varg_begin, :ident) *
r(
  spc_or_lf * "," * spc_or_lf * ident *
  closure_varg_idents(:closure_varg_begin, :ident)
) ) *
o( ":" * ident * 
  closure_varg_retarg(:closure_varg_begin, :ident)
) * spc_or_lf * "|" * closure_varg_end(:closure_varg_begin)

closure_retarg <= ":" * ident

const_literal <= real | number | const_string

