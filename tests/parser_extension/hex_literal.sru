# パーサー拡張: `0x[0-9a-f]+` を整数リテラルとして受理する。
#
# csru の `__parser` はランタイムから書き換え可能なオブジェクトなので、
# 字句規則 (`__parser.real`) を SRU 側で差し替えるだけで新リテラルを
# 足せる。rsru 側にも同じ書き換え可能性を保つこと（パーサー本体が
# slot-dispatch で動く設計にする）。
#
# 仕組み:
# - const_literal は `real | number | const_string` の順に試す。元の
#   `real` は `[0-9.]+` を greedy にマッチするので、整数リテラルでも
#   `real` で吸収される。よって hook するのは `real`。
# - 新規 AST バリアントは増やさず、`Numeric.parse("<decimal>")` の
#   CallExpression に脱糖する。SRU から CallExpression を組み立てる
#   API は無いので、`__parser.parse(<decimal>)` で AST を借りる。

oldReal = __parser.real

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

__parser.real = { |zelf, src, pos:return|
  if (pos + 2) <= src.size()
    if src.get(pos).equal("0")
      if src.get(pos + 1).equal("x")
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
        if count > 0
          decimal = value.toS()
          inner = __parser.parse(decimal)
          out = Object.new()
          out.status = true
          out.pos = p
          out.ast = inner.ast
          return(out)
        end
      end
    end
  end
  return(oldReal(zelf, src, pos))
}

# `puts (x).toS()` は `puts(x).toS()` と解釈されるので中間変数を使う。
a = 0x10.toS();          puts a
b = 0xff.toS();          puts b
c = (0x10 + 0x01).toS(); puts c
d = (0xff - 0x0f).toS(); puts d
e = 0x100.toS();         puts e
