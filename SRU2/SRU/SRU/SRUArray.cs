using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    public class SRUArray : SRUObject
    {
        public static void RegesterArray(SRUObject Array)
        {
            Array["name"] = "Array".ToSRUObject();
            Array["class"] = Library.Klass;
            Array["super"] = Library.Object;
            Array["field_missing"] = Library.FieldMissing;
            // TODO: Define method
        }
        private List<SRUObject> list = new List<SRUObject>();

        public SRUArray()
        {
            this["class"] = Library.Array;
            this["field_missing"] = Library.FieldMissing;
        }
    }
}
