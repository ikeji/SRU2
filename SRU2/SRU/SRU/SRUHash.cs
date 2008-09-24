using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace SRU
{
    public class SRUHash : SRUObject
    {
        public static void RegesterHash(SRUObject Hash)
        {
            Hash["name"] = "Hash".ToSRUObject();
            Hash["class"] = Library.Klass;
            Hash["super"] = Library.Object;
            Hash["field_missing"] = Library.FieldMissing;
            // TODO: Define method
            Hash.RegesterInstanceMethod("get", get);
            Hash.RegesterInstanceMethod("set", set);
        }

        private Dictionary<SRUObject, SRUObject> dict = new Dictionary<SRUObject, SRUObject>();

        public Dictionary<SRUObject, SRUObject> Dict
        {
            get { return dict; }
        }

        public SRUHash()
        {
            this["class"] = Library.Hash;
            this["field_missing"] = Library.FieldMissing;
        }

        public static SRUObject get(SRUObject[] arg)
        {
            if (arg.Length < 2 || !(arg[0] is SRUHash))
                return Library.Nil;
            SRUHash self = arg[0] as SRUHash;
            if (!self.dict.ContainsKey(arg[1]))
                return Library.Nil;
            return self.dict[arg[1]];
        }

        public static SRUObject set(SRUObject[] arg)
        {
            if (arg.Length < 3 || !(arg[0] is SRUHash))
                return Library.Nil;
            SRUHash self = arg[0] as SRUHash;
            return self.dict[arg[1]] = arg[2];
        }
    }
}
