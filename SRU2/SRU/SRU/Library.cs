using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    public static class Library
    {
        static Library()
        {
            // Make instance
            Nil = new SRUObject();
            Proc = new SRUObject();
            Klass = new SRUObject();
            Object = new SRUObject();
            String = new SRUObject();
            Number = new SRUObject();
            Array = new SRUObject();
            Hash = new SRUObject();
            Boolean = new SRUObject();
            True = new SRUObject();
            False = new SRUObject();
            FieldMissing = new SRUClass.FieldMissingProc();

            // Initialize each instance.
            SRUNil.RegesterNil(Nil);
            SRUProc.RegesterProc(Proc);
            SRUClass.RegesterClass(Klass);
            SRUObjectClass.RegesterObject(Object);
            SRUString.RegesterString(String);
            SRUNumber.RegesterNumber(Number);
            SRUArray.RegesterArray(Array);
            SRUHash.RegesterHash(Hash);
            SRUBoolean.RegesterBoolean(Boolean);
            SRUBoolean.RegesterTrue(True);
            SRUBoolean.RegesterFalse(False);
            FieldMissing.Initialize();

            // Eval
            Expressions = new SRUExpressionsNamespace();
            Expressions.Initialize();
            Evaluator = new SRUEvaluator();
        }

        public static SRUObject Nil;
        public static SRUObject Proc;
        public static SRUObject Klass;
        public static SRUObject Object;
        public static SRUObject String;
        public static SRUObject Number;
        public static SRUObject Array;
        public static SRUObject Hash;
        public static SRUObject Boolean;
        public static SRUObject True;
        public static SRUObject False;
        public static SRUClass.FieldMissingProc FieldMissing;

        public static SRUExpressionsNamespace Expressions;
        public static SRUObject Evaluator;

        public static void RegesterClass(SRUObject env)
        {
            env["Env"] = env;
            env["nil"] = Nil;
            env["Proc"] = Proc;
            env["Class"] = Klass;
            env["Object"] = Object;
            env["String"] = String;
            env["Number"] = Number;
            env["Array"] = Array;
            env["Hash"] = Hash;
            env["Boolean"] = Boolean;
            env["true"] = True;
            env["false"] = False;
            env["Expressions"] = Expressions;
            // TODO: this method called by initializing evaluator;
            //env["Evaluator"] = Evaluator;
        }
    }
}
