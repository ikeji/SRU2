class CppHelperBuilder
  def self.to_s(parser)
    self.new.print(parser)
  end
  def print(parser)
    ret = ""
    parser.manipulators.each do|mani|
      ret += <<-EOL
DEFINE_SRU_PROC(#{mani.name}){ // this, src, pos, #{mani.varg_list.map{|m|m.to_s}.join ", "}
  PARGCHK();
      EOL
      if(mani.varg_list.size != 0)
        ret += <<-EOL
  PARGNCHK(#{3 + mani.varg_list.size });
        EOL
      end
      ret += <<-EOL
  LOG << "#{mani.name}";

}

      EOL
    end
    return ret
  end
end
