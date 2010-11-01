# Generate by yapp.
symbol :program, :statements, :statement, :let_statement, :flow_statement,
       :if_statement, :while_statement, :class_statement, :def_statement,
       :expression, :if_main, :bool_term, :comp, :bit_sim, :bit_term,
       :bit_shift, :sim, :term, :factor, :instance_method, :method_call,
       :primary, :primitive, :reference, :literal, :closure_literal,
       :closure_varg, :closure_retarg, :const_literal, :array_literal,
       :after_if_statement
# Implimented in C++
symbol :spc_or_lf, :spc, :ident, :number, :real, :const_string, :lf, :eos,
       :spc_or_comment

manipulator :program_end
program <= after_if_statement * spc_or_comment * ( lf | ";" | eos) *
  program_end(:after_if_statement)

manipulator :statements_begin, :statements_statement, :statements_end
statements <= statements_begin *
o(
  spc_or_lf * ~"end" * ~"else" * ~"elsif" * 
  after_if_statement *
  statements_statement(:statements_begin, :after_if_statement) *
  r(
    spc_or_comment * (lf | ";" | eos) *
    spc_or_lf * ~"end" * ~"else" * ~"elsif" * after_if_statement *
    statements_statement(:statements_begin, :after_if_statement)
  )
) * statements_end(:statements_begin)


manipulator :statement_begin, :statement_main, :statement_if, :statement_end
after_if_statement <= statement_begin *
 statement * statement_main(:statement_begin, :statement) *
 o(
  spc * "if" * after_if_statement *
  statement_if(:statement_begin, :after_if_statement)
) * statement_end(:statement_begin)


statement <= spc_or_lf * ( let_statement | flow_statement )


manipulator :let_statement_end
let_statement <= 
flow_statement * # only returns ref
spc * "=" * spc_or_lf * statement *
let_statement_end(:flow_statement, :statement)


flow_statement <=
  if_statement | while_statement | class_statement | def_statement | expression


if_statement <= "if" * if_main


manipulator :if_main_cond, :if_main_then, :if_main_end, :if_main_elsif,
            :if_main_else
if_main <=
spc_or_lf * after_if_statement *
if_main_cond(:after_if_statement) * spc_or_lf * o("then") *
o(statements * if_main_then(:statements)) * spc_or_lf *
(
  "elsif" * if_main *
      if_main_elsif(:if_main_cond, :if_main_then, :if_main) |
  "else" * statements * spc_or_lf * "end" *
      if_main_else(:if_main_cond, :if_main_then, :statements)|
  "end" * if_main_end(:if_main_cond, :if_main_then)
)


manipulator :while_statement_end
while_statement <= "while" * spc_or_lf *
"(" * spc_or_lf * after_if_statement * spc_or_lf * ")" *
spc_or_lf * statements * spc_or_lf *
spc_or_lf * "end" * while_statement_end(:after_if_statement, :statements)


# NOTE: class statement is convert as follow.
# class A < B
#   def x()
#     hoge()
#   end
#   def y(i)
#     fuga(i)
#   end
# end
#       |
#       v
# A = {
#   $$ = B.subclass
#   $$.instanceMethods.x = {|self:return| hoge() }
#   $$.instanceMethods.y = {|self,i:return| fuga(i) }
# }
manipulator :class_statement_begin, :class_statement_method_begin,
            :class_statement_method_varg, :class_statement_method_end,
            :class_statement_end
class_statement <= "class" *
spc_or_lf * ident *
spc_or_lf *
o( "<" * spc_or_lf * after_if_statement) *
class_statement_begin(:ident, :after_if_statement) *
spc_or_lf *
r(
  "def" * spc_or_lf * ident * spc_or_lf *
  class_statement_method_begin(:ident) *
  "(" * spc_or_lf * o(
    ident *
    class_statement_method_varg(:class_statement_method_begin, :ident) *
    spc_or_lf *
    r(
      "," * spc_or_lf * ident *
      class_statement_method_varg(:class_statement_method_begin, :ident)
    )
  ) * ")" *
  spc_or_lf * statements * spc_or_lf * "end" *
  class_statement_method_end(
      :class_statement_begin,
      :class_statement_method_begin,
      :statements)
) * spc_or_lf * "end" *
class_statement_end(:class_statement_begin)


manipulator :def_statement_begin, :def_statement_varg, :def_statement_end
def_statement <=
"def" * spc_or_lf * ident * def_statement_begin(:ident) *
"(" * spc_or_lf * o(
  ident * def_statement_varg(:def_statement_begin, :ident) *
  spc_or_lf * r(
    "," * spc_or_lf * ident * def_statement_varg(:def_statement_begin, :ident)
  )
) * ")" *
spc_or_lf * statements * spc_or_lf *
"end" * def_statement_end(:def_statement_begin, :statements)


manipulator :expression_begin, :expression_pipepipe, :expression_end
expression <= bool_term * expression_begin(:bool_term) * r(
 spc * "||" * spc_or_lf * bool_term *
 expression_pipepipe(:expression_begin, :bool_term)
) * expression_end(:expression_begin)


manipulator :bool_term_begin, :bool_term_ampamp, :bool_term_end
bool_term <= comp * bool_term_begin(:comp) * r(
 spc * "&&" * spc_or_lf * comp * bool_term_ampamp(:bool_term_begin, :comp)
) * bool_term_end(:bool_term_begin)


manipulator :comp_begin, :comp_equal, :comp_super_equal, :comp_not_equal,
            :comp_greater_than, :comp_less_than, :comp_greater_or_equal,
            :comp_less_or_equal, :comp_end
comp <= bit_sim * comp_begin(:bit_sim) * r(
 spc * (
  "==" * spc_or_lf * bit_sim * comp_equal(:comp_begin, :bit_sim) |
  "===" * spc_or_lf * bit_sim * comp_super_equal(:comp_begin, :bit_sim) |
  "!=" * spc_or_lf * bit_sim * comp_not_equal(:comp_begin, :bit_sim) |
  ">" * spc_or_lf * bit_sim * comp_greater_than(:comp_begin, :bit_sim) |
  "<" * spc_or_lf * bit_sim * comp_less_than(:comp_begin, :bit_sim) |
  ">=" * spc_or_lf * bit_sim * comp_greater_or_equal(:comp_begin, :bit_sim) |
  "<=" * spc_or_lf * bit_sim * comp_less_or_equal(:comp_begin, :bit_sim)
 ) 
) * comp_end(:comp_begin)


manipulator :bit_sim_begin, :bit_sim_pipe, :bit_sim_end
bit_sim <= bit_term * bit_sim_begin(:bit_term) * r(
 spc * "|" * bit_term * bit_sim_pipe(:bit_sim_begin, :bit_term)
) * bit_sim_end(:bit_sim_begin)


manipulator :bit_term_begin, :bit_term_amp, :bit_term_end
bit_term <= bit_shift * bit_term_begin(:bit_shift) * r(
 spc * "&" * bit_shift * bit_term_amp(:bit_term_begin, :bit_shift)
) * bit_term_end(:bit_term_begin)


manipulator :bit_shift_begin, :bit_shift_ltlt, :bit_shift_gtgt, :bit_shift_end
bit_shift <= sim * bit_shift_begin(:sim) * r( spc *
 (
 "<<" * spc_or_lf * sim * bit_shift_ltlt(:bit_shift_begin, :sim) |
 ">>" * spc_or_lf * sim * bit_shift_gtgt(:bit_shift_begin, :sim)
 )
) * bit_shift_end(:bit_shift_begin)


manipulator :sim_begin, :sim_plus, :sim_minus, :sim_end
sim <= term * sim_begin(:term) * r(
 spc * (
  "+" * spc_or_lf * term * sim_plus(:sim_begin, :term) |
  "-" * spc_or_lf * term * sim_minus(:sim_begin, :term)
 )
) * sim_end(:sim_begin)


manipulator :term_begin, :term_asterisk, :term_slash, :term_percent, :term_end
term <= factor * term_begin(:factor) * r(
 spc * (
  "*" * spc_or_lf * factor * term_asterisk(:term_begin, :factor) |
  "/" * spc_or_lf * factor * term_slash(:term_begin, :factor) |
  "%" * spc_or_lf * factor * term_percent(:term_begin, :factor)
 )
) * term_end(:term_begin)


manipulator :factor_exclamation, :factor_tilde
factor <=
spc * "!" * spc_or_lf * factor * factor_exclamation(:factor) |
spc * "~" * spc_or_lf * factor * factor_tilde(:factor) |
instance_method


manipulator :instance_method_begin, :instance_method_self,
            :instance_method_method_begin, :instance_method_method_arg,
            :instance_method_method_end, :instance_method_ref,
            :instance_method_call_begin, :instance_method_call_arg,
            :instance_method_call_end, :instance_method_call_index,
            :instance_method_end
instance_method <=
instance_method_begin *
o(
  method_call * instance_method_self(:instance_method_begin, :method_call)
) *
r( "." * ident * spc *
  ((
    "(" * instance_method_method_begin(:ident) *
    o( 
      spc_or_lf * after_if_statement *
      instance_method_method_arg(:instance_method_method_begin,
                                 :after_if_statement) *
      r(
        spc_or_lf * "," * spc_or_lf * after_if_statement*
        instance_method_method_arg(:instance_method_method_begin,
                                   :after_if_statement) *
        spc_or_lf
      )
    ) *
    spc_or_lf * ")" *
    instance_method_method_end(:instance_method_method_begin,
                               :instance_method_begin)
  )|(
    instance_method_ref(:instance_method_begin, :ident)
  )) *
  r((
    "(" * instance_method_call_begin(:instance_method_begin) *
    o(
      spc_or_lf * after_if_statement *
      instance_method_call_arg(:instance_method_call_begin,
                               :after_if_statement) *
      r(
        spc_or_lf * "," * spc_or_lf * after_if_statement *
        instance_method_call_arg(:instance_method_call_begin,
                                 :after_if_statement)
      )
    ) * spc_or_lf * ")" *
    instance_method_call_end(:instance_method_begin,
        :instance_method_call_begin)
  )|(
    "[" * after_if_statement * spc_or_lf * "]" *
    instance_method_call_index(:instance_method_begin, :after_if_statement)
  ))
) * instance_method_end(:instance_method_begin, :method_call)


manipulator :method_call_primary, :method_call_method_begin,
            :method_call_method_arg, :method_call_method_end,
            :method_call_method_index, :method_call_end
method_call <=
primary * method_call_primary(:primary) *
r((
  spc * "(" * method_call_method_begin(:primary) *
  o(
    after_if_statement * method_call_method_arg(:method_call_method_begin,
                                                :after_if_statement) *
    r(
      spc_or_lf * "," * after_if_statement *
      method_call_method_arg(:method_call_method_begin, :after_if_statement)
    )
  ) *
  spc_or_lf * ")" *
  method_call_method_end(:method_call_primary, :method_call_method_begin)
)|(
  ~"[" * ~"." * spc * ~"if" * ~"end" * ~"elsif" * ~"then" * ~"else" * ~lf *
  ~"|" * ~"&" * ~"=" * ~"!" * ~">" * ~"<" * ~"+" * ~"-" * ~"*" * ~"/" * ~"%" *
  ~"~" *
  method_call_method_begin(:primary) *
  statement * method_call_method_arg(:method_call_method_begin, :statement) *
  r(
    spc * "," * spc_or_lf * ~"if" *
    statement * method_call_method_arg(:method_call_method_begin, :statement)
  ) *
  method_call_method_end(:method_call_primary, :method_call_method_begin)
)|(
  spc * "[" * after_if_statement * spc_or_lf * "]" *
   method_call_method_index(:method_call_primary, :after_if_statement)
)) * method_call_end(:method_call_primary)


manipulator :primary_minus, :parent
primary <=
primitive |
spc * (
"-" * primary * primary_minus(:primary) |
"(" * spc_or_lf * after_if_statement * spc_or_lf * ")" *
parent(:after_if_statement)
)


primitive <= reference | literal


literal <= closure_literal | const_literal | array_literal


reference <= ident


manipulator :closure_begin, :closure_merge_varg, :closure_statements,
            :closure_end
closure_literal <=
spc * "{" * closure_begin *
o( closure_varg * closure_merge_varg(:closure_begin, :closure_varg)) *
statements *
closure_statements(:closure_begin, :statements) *
spc_or_lf * "}" * closure_end(:closure_begin)


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


manipulator :array_literal_begin, :array_literal_item, :array_literal_end
array_literal <= "[" * array_literal_begin *
o( after_if_statement * array_literal_item(:array_literal_begin,
                                           :after_if_statement) *
  r(
    spc_or_lf * "," *
    after_if_statement * array_literal_item(:array_literal_begin,
                                            :after_if_statement)
  ) *
  o( "," )
) * "]" * array_literal_end(:array_literal_begin)


