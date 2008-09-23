using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    public class SRUObject
    {
        private Dictionary<string, SRUObject> objectMap;
        public SRUObject()
        {
            this.objectMap = new Dictionary<string, SRUObject>();
        }

        public SRUObject this[string index]
        {
            get
            {
                if (!objectMap.ContainsKey(index))
                {
                    if (objectMap.ContainsKey("field_missing"))
                    {
                        return this["field_missing"].Call(this,index.ToSRUObject());
                    }
                    return Library.Nil;
                }
                return objectMap[index];
            }
            set { this.objectMap[index] = value; }
        }

        public bool IsExistField(string index)
        {
            return objectMap.ContainsKey(index);
        }

        public override string ToString()
        {
            try
            {
                if (this["inspect"] is SRUProc)
                {
                    return (this["inspect"] as SRUProc).Call(this).ConvertToString();
                }
            }
            catch
            {
            }
            return base.ToString();
        }

        public SRUObject Invoke(string name, params SRUObject[] arg)
        {
            SRUProc p = this[name] as SRUProc;
            // TODO: check 
            SRUObject[] args = new SRUObject[arg.Length + 1];
            args[0] = this;
            Array.Copy(arg, 0, args, 1, arg.Length);
            return p.Call(args);
        }

        public void RegesterMethod(string name, SRUNativeProc.NativeProc proc)
        {
            this.objectMap.Add(name, proc.ToSRUObject());
        }
    }
}
