using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    public abstract class SRUProc : SRUObject
    {
        public static void RegesterProc(SRUObject obj)
        {
            obj["name"] = "Proc".ToSRUObject();
            obj["class"] = Library.Klass;
            obj["super"] = Library.Object;
            obj["field_missing"] = Library.FieldMissing;
            // TODO: Define method of proc Class
            obj.RegesterInstanceMethod("inspect", inspect);
        }
        public abstract SRUObject Call(params SRUObject[] arg);

        public static SRUObject inspect(params SRUObject[] arg)
        {
            if (arg[0] is SRUNativeProc)
            {
                return "{ -- Native -- }".ToSRUObject();
            }
            if (arg[0] is SRUEvaluator.SRUClosure)
            {
                SRUEvaluator.SRUClosure closure = arg[0] as SRUEvaluator.SRUClosure;
                StringBuilder sb = new StringBuilder();
                foreach(SRUExpression exp in closure.Expressions){
                    sb.Append(exp.ToString() + "\n");
                }
                return ("{|" + String.Join(", ",closure.FormalArgument) + 
                    ((closure.Retname != "")?(": " + closure.Retname):"")+ "| " +
                    sb + " }").ToSRUObject();
            }
            // TODO:impliment closure object inspector
            throw new Exception();
        }
    }

    public class SRUNativeProc : SRUProc
    {
        public delegate SRUObject NativeProc(params SRUObject[] arg);
        private NativeProc proc;
        public SRUNativeProc(NativeProc proc)
        {
            this["class"] = Library.Proc;
            this["field_missing"] = Library.FieldMissing;
            this.proc = proc;
        }
        public override SRUObject Call(params SRUObject[] arg)
        {
            return proc(arg);
        }
    }

    public static class SRUNativeProcExtesions
    {

        public static SRUObject Call(this SRUObject obj,params SRUObject[] arg)
        {
            if (obj is SRUProc)
            {
                return ((SRUProc)obj).Call(arg);
            }
            // TODO: convert?
            throw new Exception();
        }

        public static SRUObject ToSRUObject(this SRUNativeProc.NativeProc obj)
        {
            return new SRUNativeProc(obj);
        }
    }
}
