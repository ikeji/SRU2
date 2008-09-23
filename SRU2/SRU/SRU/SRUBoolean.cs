using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    class SRUBoolean
    {
        public static void RegesterBoolean(SRUObject Boolean)
        {
            Boolean["name"] = "Boolean".ToSRUObject();
            Boolean["class"] = Library.Klass;
            Boolean["super"] = Library.Object;
            Boolean["field_missing"] = Library.FieldMissing;
            // TODO: Define method
        }

        public static void RegesterTrue(SRUObject True)
        {
            True["class"] = Library.Boolean;
            True["field_missing"] = Library.FieldMissing;
            True.RegesterMethod("if_true", True_if_true);
        }

        public static SRUObject True_if_true(params SRUObject[] arg)
        {
            return arg[0].Call();
        }

        public static void RegesterFalse(SRUObject False)
        {
            False["class"] = Library.Boolean;
            False["field_missing"] = Library.FieldMissing;
            False.RegesterMethod("if_true", False_if_true);
        }

        public static SRUObject False_if_true(params SRUObject[] arg)
        {
            return Library.Nil;
        }

    }
}
