class CppHelperBuilder
  def self.to_s(parser)
    self.new.print(parser)
  end
  def print(parser)
    ret = ""
    parser.manipulators.each do|mani|
      ret += <<-EOL
DEFINE_SRU_PROC(#{mani.name}){ // this, src, pos, #{mani.varg_list.map{|m|m.to_s}.join ", "}
  assert(args.size() >= #{ 3 + mani.varg_list.size });
  LOG << "#{mani.name}";

}

      EOL
    end
    return ret
  end
end
