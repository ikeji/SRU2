using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SRU;

namespace SRU2test
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
            /*
            SRUObject a = "hoge".ToSRUObject();
            SRUObject b = "hoge".ToSRUObject();
            if (a.Invoke("equals", b) == Library.True)
                MessageBox.Show("true");
             */
            /*
            SRUObject o = SRU.SRU.Library.Expressions.ClosureExpression;
            string s = o.ToString2();
            MessageBox.Show(s);
             */
            /*
            SRUObject frame = Library.Evaluator["current_frame"];
            frame.ToString();
             */
            TestCase.ExpTest();
        }
    }

    public class TestCase
    {
        public static void RefTest()
        {
            SRUExpression exp = new SRURefExpression()
            {
                Env = null,
                Name = "Number",
            };
            MessageBox.Show(Library.Evaluator.Invoke("eval_expression", exp).ToString());
        }
        public static void RefTest2()
        {
            SRUExpression exp = new SRURefExpression()
            {
                Env = new SRURefExpression()
                {
                    Env = null,
                    Name = "Number",
                },
                Name = "parse",
            };
            MessageBox.Show(Library.Evaluator.Invoke("eval_expression", exp).ToString());
        }

        public static void CallTest()
        {
            // Number.parse("2")
            // (Number.parse)(Number,"2")
            SRUExpression exp = new SRUCallExpression()
            {
                Method = new SRURefExpression()
                {
                    Env = new SRURefExpression()
                    {
                        Env = null,
                        Name = "Number",
                    },
                    Name = "parse",
                },
                Args = new SRUExpression[]{
                    new SRURefExpression()
                    {
                        Env = null,
                        Name = "Number",
                    },
                    new SRUStringExpression(){
                        Value = "2",
                    },
                },
            };
            MessageBox.Show(Library.Evaluator.Invoke("eval_expression", exp).ToString());
        }

        public static void ClosureTest0()
        {
            // {|a| a }(Number)
            SRUExpression exp = new SRUClosureExpression()
            {
                FormalArgument = new string[] { "a" },
                Expressions = new SRUExpression[]{
                    new SRURefExpression(){
                        Env = null,
                        Name = "a",
                    },
                },
            };
            MessageBox.Show(Library.Evaluator.Invoke("eval_expression", exp).ToString());
        }

        public static void ClosureTest()
        {
            // {|a| a }(Number)
            SRUExpression exp = new SRUCallExpression()
            {
                Method = new SRUClosureExpression()
                {
                    FormalArgument = new string[]{ "a" },
                    Expressions = new SRUExpression[]{
                        new SRURefExpression(){
                            Env = null,
                            Name = "a",
                        },
                    },
                },
                Args = new SRUExpression[]{
                    new SRURefExpression()
                    {
                        Env = null,
                        Name = "Number",
                    },
                },
            };
            MessageBox.Show(Library.Evaluator.Invoke("eval_expression", exp).ToString());
        }

        public static void ClosureTest2()
        {
            // {|a,b| (a.parse)(a,b) }(Number,"3")
            SRUExpression exp = new SRUCallExpression()
            {
                Method = new SRUClosureExpression()
                {
                    FormalArgument = new string[] { "a","b" },
                    Expressions = new SRUExpression[]{
                        new SRUCallExpression(){
                            Method = new SRURefExpression(){
                                Env = new SRURefExpression(){
                                    Env = null,
                                    Name = "a",
                                },
                                Name = "parse",
                            },
                            Args = new SRUExpression[]{
                                new SRURefExpression(){
                                    Env = null,
                                     Name = "a",
                                },
                                new SRURefExpression(){
                                    Env = null,
                                     Name = "b",
                                },
                            },
                        },
                    },
                },
                Args = new SRUExpression[]{
                    new SRURefExpression()
                    {
                        Env = null,
                        Name = "Number",
                    },
                    new SRUStringExpression(){
                        Value = "3",
                    },
                },
            };
            MessageBox.Show(Library.Evaluator.Invoke("eval_expression", exp).ToString());
        }

        public static void ExpTest()
        {
            // 1 + 2
            // Number.parse("1") + Number.parse("2")
            // Number.parse("1").add(Number.parse("2"));
            // {|a,b| (a.add)(a,b))(Number.parse("1"),Number.parse("2"))
            // {|a,b| (a.add)(a,b))({|a,b| (a.parse)(a,b)}(Number,"1"),{|a,b| (a.parse)(a,b)}(Number,"2"))
            SRUExpression exp = new SRUCallExpression()
            {
                // {|a,b| (a.add)(a,b) }
                Method = new SRUClosureExpression()
                {
                    FormalArgument = new string[] { "a", "b" },
                    Expressions = new SRUExpression[]{
                        // (a.add)(a,b)
                        new SRUCallExpression(){
                            //(a.add)
                            Method = new SRURefExpression{
                                Env = new SRURefExpression(){
                                    Env = null,
                                    Name = "a",
                                },
                                Name = "add",
                            },
                            // (a,b)
                            Args = new SRUExpression[]{
                                new SRURefExpression(){
                                    Env = null,
                                    Name = "a",
                                },
                                new SRURefExpression(){
                                    Env = null,
                                    Name = "b",
                                },
                            },
                        },
                    },
                },
                Args = new SRUExpression[]{
                    // {|a,b| (a.parse)(a,b)}(Number,"1")
                    new SRUCallExpression(){
                        // {|a,b| (a.parse)(a,b)}
                        Method = new SRUClosureExpression(){
                            FormalArgument = new string[]{ "a", "b" },
                            Expressions = new SRUExpression[]{
                                // (a.parse)(a,b)
                                new SRUCallExpression(){
                                    // (a.parse)
                                    Method = new SRURefExpression{
                                        Env = new SRURefExpression(){
                                            Env = null,
                                            Name = "a",
                                        },
                                        Name = "parse",
                                    },
                                    // (a,b)
                                    Args = new SRUExpression[]{
                                        new SRURefExpression(){
                                            Env = null,
                                            Name = "a",
                                        },
                                        new SRURefExpression(){
                                            Env = null,
                                            Name = "b",
                                        },
                                    },
                                },
                            },
                        },
                        // (Number,"1")
                        Args = new SRUExpression[]{
                            new SRURefExpression(){
                                Env = null,
                                Name = "Number",
                            },
                            new SRUStringExpression(){
                                Value = "1",
                            },
                        },
                    },
                    // {|a,b| (a.parse)(a,b)}(Number,"2")
                    new SRUCallExpression(){
                        // {|a,b| (a.parse)(a,b)}
                        Method = new SRUClosureExpression(){
                            FormalArgument = new string[]{ "a", "b" },
                            Expressions = new SRUExpression[]{
                                // (a.parse)(a,b)
                                new SRUCallExpression(){
                                    // (a.parse)
                                    Method = new SRURefExpression{
                                        Env = new SRURefExpression(){
                                            Env = null,
                                            Name = "a",
                                        },
                                        Name = "parse",
                                    },
                                    // (a,b)
                                    Args = new SRUExpression[]{
                                        new SRURefExpression(){
                                            Env = null,
                                            Name = "a",
                                        },
                                        new SRURefExpression(){
                                            Env = null,
                                            Name = "b",
                                        },
                                    },
                                },
                            },
                        },
                        // (Number,"2")
                        Args = new SRUExpression[]{
                            new SRURefExpression(){
                                Env = null,
                                Name = "Number",
                            },
                            new SRUStringExpression(){
                                Value = "2",
                            },
                        },
                    },
                },
            };
            MessageBox.Show(Library.Evaluator.Invoke("eval_expression", exp).ConvertToString());
        }
    }
}
