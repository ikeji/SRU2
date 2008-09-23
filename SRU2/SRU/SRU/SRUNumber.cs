using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    public class SRUNumber : SRUObject
    {
        public static void RegesterNumber(SRUObject obj)
        {
            obj["name"] = "Number".ToSRUObject();
            obj["class"] = Library.Klass;
            obj["super"] = Library.Object;
            obj["field_missing"] = Library.FieldMissing;
            // TODO: Define method of String Class
            obj.RegesterMethod("parse", parse);
            obj.RegesterInstanceMethod("inspect", inspect);
            obj.RegesterInstanceMethod("to_s", to_s);
            obj.RegesterInstanceMethod("add", add);
        }

        public int Value { get { return value; } }
        private int value;
        public SRUNumber(int str)
        {
            this.value = str;
            this["class"] = Library.Number;
            this["field_missing"] = Library.FieldMissing;
        }

        public static SRUObject parse(params SRUObject[] args)
        {
            string num = args[1].ConvertToString();
            return Int32.Parse(num).ToSRUObject();
        }

        public static SRUObject inspect(params SRUObject[] args)
        {
            return ("<Number: " + args[0].ConvertToInt().ToString() + " >").ToSRUObject();
        }
        public static SRUObject to_s(params SRUObject[] args)
        {
            return args[0].ConvertToInt().ToString().ToSRUObject();
        }

        public static SRUObject add(params SRUObject[] args)
        {
            int retval = 0;
            foreach (SRUObject n in args)
            {
                retval += n.ConvertToInt();
            }
            return retval.ToSRUObject();
        }
    }


    public static class SRUNumberExtesions
    {

        public static int ConvertToInt(this SRUObject obj)
        {
            if (obj is SRUNumber)
            {
                return ((SRUNumber)obj).Value;
            }
            // TODO: convert?
            throw new Exception();
        }

        public static SRUObject ToSRUObject(this int obj)
        {
            return new SRUNumber(obj);
        }
    }
}
