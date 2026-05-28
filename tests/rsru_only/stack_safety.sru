# Regression test: a tight while-loop with 50k iterations used to blow
# the Rust stack via the recursive __while() desugar. TCO in
# Vm::push_sru_frame and the NativeSmash forms of Boolean#ifTrue and
# Proc#call keep both the Rust stack and the SRU upper-chain bounded.
i = 0
while (i < 50000)
  i = i + 1
end
puts i.toS()
