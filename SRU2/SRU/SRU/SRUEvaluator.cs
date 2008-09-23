using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    public class SRUEvaluator :  SRUObject, ExpressionVisitor
    {
        public SRUEvaluator()
        {
            this["current_frame"] = MakeFrame(Library.Nil) ;
            RegesterMethod("eval_expression", eval_expression);
            Library.RegesterClass(this["current_frame"]);
            this["current_frame"]["Evaluator"] = this;
        }

        public static SRUObject eval_expression(params SRUObject[] arg)
        {
            SRUEvaluator evalu = arg[0] as SRUEvaluator;
            SRUExpression exp = arg[1] as SRUExpression;
            return exp.Accept(evalu);
        }

        public static SRUObject MakeFrame(SRUObject currnet_frame)
        {
            SRUObject frame = new SRUObject();
            frame["outside"] = currnet_frame;
            frame.RegesterMethod("field_missing", field_missing);
            return frame;
        }

        public static SRUObject field_missing(params SRUObject[] args)
        {
            if (args[0].IsExistField("outside") && args[0]["outside"] != Library.Nil)
            {
                return args[0]["outside"][args[1].ConvertToString()];
            }
            return Library.Nil;
        }

        #region ExpressionVisitor メンバ

        public SRUObject Visit(SRULetExpression let)
        {
            SRUObject env = null;
            if (let.Env != null)
            {
                env = let.Env.Accept(this);
            }
            if (let.Env == null || let.Env == Library.Nil)
            {
                env = this["current_frame"];
            }
            return env[let.Name] = let.Value.Accept(this);
        }

        public SRUObject Visit(SRURefExpression re)
        {
            SRUObject env = null;
            if (re.Env != null && re.Env != Library.Nil)
            {
                env = re.Env.Accept(this);
            }
            else
            {
                env = this["current_frame"];
            }
            return env[re.Name];
        }

        public SRUObject Visit(SRUCallExpression call)
        {
            SRUObject proc = call.Method.Accept(this);
            SRUObject[] args = new SRUObject[call.Args.Length];
            for (int i = 0; i < args.Length; i++)
            {
                args[i] = call.Args[i].Accept(this);
            }
            return proc.Call(args);
        }

        public SRUObject Visit(SRUClosureExpression clos)
        {
            SRUClosure c = new SRUClosure();
            c.Expressions = clos.Expressions;
            c.FormalArgument = clos.FormalArgument;
            c.Retname = clos.Retname;
            return c;
        }
        public class SRUClosure : SRUProc
        {
            public SRUClosure()
            {
                this["class"] = Library.Proc;
                this["field_missing"] = Library.FieldMissing;
            }
            public string[] FormalArgument = new string[] { };
            public string Retname = "";
            public SRUExpression[] Expressions = new SRUExpression[] { };
            public override SRUObject Call(params SRUObject[] arg)
            {
                SRUObject old_frame = Library.Evaluator["current_frame"];
                SRUObject frame = MakeFrame(old_frame);
                Library.Evaluator["current_frame"] = frame;
                SRUObject retval = Library.Nil;
                for (int i = 0; i < FormalArgument.Length; i++)
                {
                    SRUObject val = Library.Nil;
                    if (arg.Length > i)
                        val = arg[i];
                    frame[FormalArgument[i]] = val;
                }
                foreach (SRUExpression e in Expressions)
                {
                    retval = Library.Evaluator.Invoke("eval_expression", e);
                }
                Library.Evaluator["current_frame"] = old_frame;
                return retval;
            }
        }

        public SRUObject Visit(SRUStringExpression cstr)
        {
            return cstr.Value.ToSRUObject();
        }

        #endregion
    }
}
