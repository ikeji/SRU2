# パーサー拡張: `0x[0-9a-f]+` を整数リテラルとして受理する。
#
# csru の `__parser` はランタイムから書き換え可能なオブジェクトなので、
# 字句規則を SRU 側で差し替えるだけで新リテラルを足せる。rsru 側にも
# 同じ書き換え可能性を保つこと（パーサー本体が slot-dispatch で動く
# 設計にする）。
#
# 仕組み:
# - 文法は `const_literal <= real | number | const_string`。新リテラルは
#   このレベルの選択肢として足したいので、`const_literal` を hook する。
#   先に hex を試し、見つからなければオリジナルへ委譲。
# - AST バリアントは増やさない。整数リテラル `123` は AST 上では
#   `Numeric.parse("123")` の CallExpression で表されている。`0x10` も
#   同様に `Numeric.parse("16")` に脱糖すれば、評価器は何も知らずに動く。
# - csru の SRU 側 API には StringExpression.new しか無いので、
#   CallExpression を SRU から組み立てる手段が無い。代わりに
#   `__parser.parse("16")` を呼んで「`16` を普通にパースした結果」の
#   AST を流用する。

oldCL = __parser.const_literal

def hexval(ch)
  if ch.equal("0")
    return(0)
  end
  if ch.equal("1")
    return(1)
  end
  if ch.equal("2")
    return(2)
  end
  if ch.equal("3")
    return(3)
  end
  if ch.equal("4")
    return(4)
  end
  if ch.equal("5")
    return(5)
  end
  if ch.equal("6")
    return(6)
  end
  if ch.equal("7")
    return(7)
  end
  if ch.equal("8")
    return(8)
  end
  if ch.equal("9")
    return(9)
  end
  if ch.equal("a")
    return(10)
  end
  if ch.equal("b")
    return(11)
  end
  if ch.equal("c")
    return(12)
  end
  if ch.equal("d")
    return(13)
  end
  if ch.equal("e")
    return(14)
  end
  if ch.equal("f")
    return(15)
  end
  return(-1)
end

__parser.const_literal = { |zelf, src, pos:return|
  # 16 進リテラルかどうかを判定。違えばオリジナルに委譲。
  isHex = false
  if (pos + 2) <= src.size()
    if src.get(pos).equal("0")
      if src.get(pos + 1).equal("x")
        isHex = true
      end
    end
  end
  if !isHex
    return(oldCL(zelf, src, pos))
  end

  # 16 進数字をスキャン。
  p = pos + 2
  value = 0
  count = 0
  scanning = true
  while (scanning)
    if (p < src.size())
      v = hexval(src.get(p))
      if v < 0
        scanning = false
      else
        value = value * 16 + v
        count = count + 1
        p = p + 1
      end
    else
      scanning = false
    end
  end
  if count == 0
    return(oldCL(zelf, src, pos))
  end

  # 等価な Numeric.parse("<decimal>") の AST を借りてくる。
  decimal = value.toS()
  inner = __parser.parse(decimal)
  out = Object.new()
  out.status = true
  out.pos = p
  out.ast = inner.ast
  return(out)
}

puts(0x10.toS())
puts(0xff.toS())
puts((0x10 + 0x01).toS())
puts((0xff - 0x0f).toS())
puts(0x100.toS())
