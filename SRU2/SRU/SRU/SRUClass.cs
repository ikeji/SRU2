using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    public class SRUClass
    {
        public static void RegesterClass(SRUObject obj)
        {
            obj["name"] = "Class".ToSRUObject();
            obj["class"] = obj;
            obj["super"] = Library.Object;
            obj["field_missing"] = Library.FieldMissing;
            // TODO: Define method
            obj.RegesterInstanceMethod("find_field", find_field);
            obj.RegesterMethod("find_field", find_field);
            obj.RegesterInstanceMethod("inspect", inspect);
        }

        public class FieldMissingProc : SRUProc
        {
            public FieldMissingProc()
            {
            }

            public void Initialize()
            {
                this["class"] = Library.Proc;
                this["field_missing"] = this;
            }

            public override SRUObject Call(params SRUObject[] arg)
            {
                if (arg.Length < 2)
                    return Library.Nil;
                // TODO: Define Class system.
                SRUObject self = arg[0];
                if (self.IsExistField("class"))
                    return self["class"].Invoke("find_field", arg[1]);
                return Library.Nil;
            }
        }

        private static SRUObject find_field(params SRUObject[] arg)
        {
            if (arg.Length < 2)
                return Library.Nil;
            SRUObject self = arg[0];
            if (!self.IsExistField("instance_methods") && !(self["instance_methods"] is SRUHash))
                return Library.Nil;
            if (!(self["instance_methods"] as SRUHash).Dict.ContainsKey(arg[1]))
            {
                if (self.IsExistField("super") && self["super"] != Library.Nil)
                {
                    return self["super"].Invoke("find_field", arg[1]);
                }
                return Library.Nil;
            }
            return ((self["instance_methods"] as SRUHash).Dict[arg[1]]);
        }

        private static SRUObject inspect(params SRUObject[] arg)
        {
            if (arg.Length < 1)
                return Library.Nil;
            SRUObject self = arg[0];
            if (self.IsExistField("name"))
            {
                return self["name"];
            }
            return "Class".ToSRUObject();
        }
    }

    class SRUObjectClass
    {
        public static void RegesterObject(SRUObject obj)
        {
            obj["name"] = "Object".ToSRUObject();
            obj["class"] = Library.Klass;
            obj["supper"] = Library.Nil;
            obj["field_missing"] = Library.FieldMissing;
            // TODO: Define method
            obj.RegesterMethod("new", new_);
            obj.RegesterInstanceMethod("invoke", invoke);
        }

        public static SRUObject new_(params SRUObject[] arg)
        {
            SRUObject retval = new SRUObject();
            retval["class"] = Library.Object;
            retval["field_missing"] = Library.FieldMissing;
            return retval;
        }
        public static SRUObject invoke(params SRUObject[] arg)
        {
            // TODO: relay arg[1..-1]
            // TOOD: error check
            return arg[0].Invoke(arg[1].ConvertToString());
        }

    }

    public static class SRUClassExtension
    {
        public static void RegesterInstanceMethod(this SRUObject self, string name, SRUNativeProc.NativeProc proc)
        {
            if (!self.IsExistField("instance_methods") || !(self["instance_methods"] is SRUHash))
                self["instance_methods"] = new SRUHash();
            (self["instance_methods"] as SRUHash).Dict.Add(name.ToSRUObject(), new SRUNativeProc(proc));
        }
    }
}
