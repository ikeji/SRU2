using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    class SRUNil
    {
        public static void RegesterNil(SRUObject Nil)
        {
            // TODO: Define method
            Nil.RegesterMethod("inspect", inspect);
            Nil.RegesterMethod("to_s", to_s);
        }

        public static SRUObject inspect(params SRUObject[] arg)
        {
            return "<Nil>".ToSRUObject();
        }
        public static SRUObject to_s(params SRUObject[] arg)
        {
            return "".ToSRUObject();
        }
    }
}
