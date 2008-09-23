using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU
{
    public interface ExpressionVisitor
    {
        SRUObject Visit(SRULetExpression let);
        SRUObject Visit(SRURefExpression re);
        SRUObject Visit(SRUCallExpression call);
        SRUObject Visit(SRUClosureExpression clos);
        SRUObject Visit(SRUStringExpression cstr);
    }

    public class SRUExpressionsNamespace : SRUObject
    {
        public void Initialize()
        {
            Expression = new SRUObject();
            LetExpression = new SRUObject();
            RefExpression = new SRUObject();
            CallExpression = new SRUObject();
            ClosureExpression = new SRUObject();
            StringExpression = new SRUObject();

            this["expression"] = Expression;
            this["LetExpression"] = LetExpression;
            this["RefExpression"] = RefExpression;
            this["CallExpression"] = CallExpression;
            this["ClosureExpression"] = ClosureExpression;
            this["StringExpression"] = StringExpression;

            SRUExpression.RegesterExpression(Expression);
            SRULetExpression.RegesterLetExpression(LetExpression);
            SRURefExpression.RegesterRefExpression(RefExpression);
            SRUCallExpression.RegesterCallExpression(CallExpression);
            SRUClosureExpression.RegesterClosureExpression(ClosureExpression);
            SRUStringExpression.RegesterStringExpression(StringExpression);
        }

        public SRUObject Expression;
        public SRUObject LetExpression;
        public SRUObject RefExpression;
        public SRUObject CallExpression;
        public SRUObject ClosureExpression;
        public SRUObject StringExpression;
    }

    public abstract class SRUExpression : SRUObject
    {
        public abstract SRUObject Accept(ExpressionVisitor v);

        public static void RegesterExpression(SRUObject Expression)
        {
            Expression["name"] = "Expression".ToSRUObject();
            Expression["class"] = Library.Klass;
            Expression["super"] = Library.Object;
            Expression["field_missing"] = Library.FieldMissing;
            // TODO: Define method

            // Make abstract and sield
            Expression["inheritance"] = Library.Nil;
            Expression["new"] = Library.Nil;
        }
    }

    public class SRULetExpression : SRUExpression
    {
        public SRUExpression Env;
        public string Name;
        public SRUExpression Value;

        public SRULetExpression()
        {
            this["class"] = Library.Expressions.LetExpression;
            this["field_missing"] = Library.FieldMissing;
        }

        public static void RegesterLetExpression(SRUObject LetExpression)
        {
            LetExpression["name"] = "LetExpression".ToSRUObject();
            LetExpression["class"] = Library.Klass;
            LetExpression["super"] = Library.Expressions.Expression;
            LetExpression["field_missing"] = Library.FieldMissing;
            // TODO: Define method
        }

        public override SRUObject Accept(ExpressionVisitor v)
        {
            return v.Visit(this);
        }

        // TODO: inspect?
        public override string ToString()
        {
            return Env.ToString() + "." + Name + " = " + Value.ToString();
        }
    }

    public class SRURefExpression : SRUExpression
    {
        public SRUExpression Env;
        public string Name;

        public SRURefExpression()
        {
            this["class"] = Library.Expressions.RefExpression;
            this["field_missing"] = Library.FieldMissing;
        }

        public static void RegesterRefExpression(SRUObject RefExpression)
        {
            RefExpression["name"] = "RefExpression".ToSRUObject();
            RefExpression["class"] = Library.Klass;
            RefExpression["super"] = Library.Expressions.Expression;
            RefExpression["field_missing"] = Library.FieldMissing;
            // TODO: Define method
        }

        public override SRUObject Accept(ExpressionVisitor v)
        {
            return v.Visit(this);
        }
        // TODO: inspect?
        public override string ToString()
        {
            return(((Env!=null)?Env.ToString() + ".":"") + Name);
        }

    }

    public class SRUCallExpression : SRUExpression
    {
        public SRUExpression Method;
        public SRUExpression[] Args = new SRUExpression[] { };

        public SRUCallExpression()
        {
            this["class"] = Library.Expressions.CallExpression;
            this["field_missing"] = Library.FieldMissing;
        }

        internal static void RegesterCallExpression(SRUObject CallExpression)
        {
            CallExpression["name"] = "CallExpression".ToSRUObject();
            CallExpression["class"] = Library.Klass;
            CallExpression["super"] = Library.Expressions.Expression;
            CallExpression["field_missing"] = Library.FieldMissing;
            // TODO: Define method
        }

        public override SRUObject Accept(ExpressionVisitor v)
        {
            return v.Visit(this);
        }
        // TODO: inspect?
        public override string ToString()
        {
            List<string> strarg = new List<string>();
            foreach(SRUExpression arg in Args){
                strarg.Add(arg.ToString());
            }
            return "(" + Method + ")(" + String.Join(", ", strarg.ToArray()) + ")";
        }
    }

    public class SRUClosureExpression : SRUExpression
    {
        public string[] FormalArgument = new string[] { };
        public string Retname = "";
        public SRUExpression[] Expressions = new SRUExpression[] { };
        public SRUClosureExpression()
        {
            this["class"] = Library.Expressions.ClosureExpression;
            this["field_missing"] = Library.FieldMissing;
        }

        public static void RegesterClosureExpression(SRUObject ClosureExpression)
        {
            ClosureExpression["name"] = "ClosureExpression".ToSRUObject();
            ClosureExpression["class"] = Library.Klass;
            ClosureExpression["super"] = Library.Expressions.Expression;
            ClosureExpression["field_missing"] = Library.FieldMissing;
            // TODO: Define method
        }

        public override SRUObject Accept(ExpressionVisitor v)
        {
            return v.Visit(this);
        }

        // TODO: inspect?
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            foreach (SRUExpression exp in Expressions)
            {
                sb.Append(exp.ToString() + "\n");
            }
            return ("{|" + String.Join(", ", FormalArgument) +
                ((Retname != "") ? (": " + Retname) : "") + "| " +
                sb + " }");
        }
    }

    public class SRUStringExpression : SRUExpression
    {
        public string Value;

        public SRUStringExpression()
        {
            this["class"] = Library.Expressions.StringExpression;
            this["field_missing"] = Library.FieldMissing;
        }

        public static void RegesterStringExpression(SRUObject StringExpression)
        {
            StringExpression["name"] = "StringExpression".ToSRUObject();
            StringExpression["class"] = Library.Klass;
            StringExpression["super"] = Library.Expressions.Expression;
            StringExpression["field_missing"] = Library.FieldMissing;
            // TODO: Define method
        }

        public override SRUObject Accept(ExpressionVisitor v)
        {
            return v.Visit(this);
        }
        
        // TODO: inspect?
        public override string ToString()
        {
            return "\"" + Value + "\"";
        }
    }
}
