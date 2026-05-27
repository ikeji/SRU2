# 16 進整数リテラルを使うサンプル。
# 0xN は内部的に Numeric.parse("<decimal>") に脱糖される。
#
# このファイルは csru ではパースできない。rsru でパーサーに 0x... の
# トークンを追加した後に通る想定。

puts 0x10.toS()
puts 0xff.toS()
puts (0x10 + 0x01).toS()
puts (0xff - 0x0f).toS()

# Numeric.parse を再定義しても、0x... は parse 時に決まり済みなので
# 影響を受けないこと（リテラル → 文字列 → parse の脱糖を **コンパイル
# 時** には行わず、**パース時に 10 進文字列を埋め込む** 方針を取る限り、
# 振る舞いは csru の 123 リテラルと一貫する）。
puts 0x100.toS()
