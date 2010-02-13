# Nonterminal symbol
symbol :program, :expression, :list, :define, :lamb, :call, :ref, :literal
# Terminal symbol
symbol :spc, :id, :stringliteral, :numericliteral

program <= expression

expression <= spc * ( list | ref | literal )

list <= define | lamb | call

manipulator :def_creat
define <= "(" * spc * "define" * spc * id * spc * expression * spc * ")" * def_creat(:id, :expression)

manipulator :lamb_begin, :lamb_arg, :lamb_end
lamb <= spc * "(" * spc * "lambda" * lamb_begin * spc * "(" *
  spc * id * lamb_arg(:lamb_begin,:id) * r( spc * "," *spc * id * lamb_arg(:lamb_begin,:id) ) * spc * ")" *
  program * spc * ")" * lamb_end(:lamb_begin, :program)

manipulator :call_begin, :call_rep, :call_end
call <=  spc * "(" * expression * call_begin(:expression) *
  r(spc * expression * call_rep(:call_begin, :expression)) * spc * ")" *
  call_end(:call_begin)

manipulator :ref_ins
ref <= spc * id * '.' * ref * ref_ins(:id,:ref) | id 

literal <= spc * (stringliteral | numericliteral)

