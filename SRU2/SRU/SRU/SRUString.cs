using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    public class SRUString : SRUObject
    {
        public static void RegesterString(SRUObject obj)
        {
            obj["name"] = "String".ToSRUObject();
            obj["class"] = Library.Klass;
            obj["super"] = Library.Object;
            obj["field_missing"] = Library.FieldMissing;
            // TODO: Define method of String Class
            obj.RegesterInstanceMethod("equals", equals);
        }

        public string Value { get { return value; } }
        private string value;
        public SRUString(string str)
        {
            this["class"] = Library.String;
            this["field_missing"] = Library.FieldMissing;
            this.value = str;
        }

        public static SRUObject equals(params SRUObject[] arg)
        {
            if (arg.Length < 2)
                return Library.False;
            if (arg[0].ConvertToString() == arg[1].ConvertToString())
                return Library.True;
            return Library.False;
        }

        // TODO: please remove after 
        public override bool Equals(object obj)
        {
            if (obj is SRUString)
                return this.value.Equals(((SRUString)obj).value);
            return base.Equals(obj);
        }
        public override int GetHashCode()
        {
            return this.value.GetHashCode();
        }
    }

    public static class SRUStringExtesions
    {

        public static string ConvertToString(this SRUObject obj)
        {
            if (obj is SRUString)
            {
                return ((SRUString)obj).Value;
            }
            // TODO: convert?
            return obj.Invoke("to_s").ConvertToString();
            throw new Exception();
        }

        public static SRUObject ToSRUObject(this string obj)
        {
            return new SRUString(obj);
        }
    }
}
