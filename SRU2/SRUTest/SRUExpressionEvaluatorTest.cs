using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SRU;

namespace SRUTest
{
    /// <summary>
    /// SRUExpressionEvaluatorTest の概要の説明
    /// </summary>
    [TestClass]
    public class SRUExpressionEvaluatorTest
    {
        public SRUExpressionEvaluatorTest()
        {
            //
            // TODO: コンストラクタ ロジックをここに追加します
            //
        }

        private TestContext testContextInstance;

        /// <summary>
        ///現在のテストの実行についての情報および機能を
        ///提供するテスト コンテキストを取得または設定します。
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region 追加のテスト属性
        //
        // テストを作成する際には、次の追加属性を使用できます:
        //
        // クラス内で最初のテストを実行する前に、ClassInitialize を使用してコードを実行してください
        // [ClassInitialize()]
        // public static void MyClassInitialize(TestContext testContext) { }
        //
        // クラス内のテストをすべて実行したら、ClassCleanup を使用してコードを実行してください
        // [ClassCleanup()]
        // public static void MyClassCleanup() { }
        //
        // 各テストを実行する前に、TestInitialize を使用してコードを実行してください
        // [TestInitialize()]
        // public void MyTestInitialize() { }
        //
        // 各テストを実行した後に、TestCleanup を使用してコードを実行してください
        // [TestCleanup()]
        // public void MyTestCleanup() { }
        //
        #endregion

        [TestMethod]
        public void EqualTest()
        {
            SRUObject a = "hoge".ToSRUObject();
            SRUObject b = "hoge".ToSRUObject();
            Assert.AreEqual(a.Invoke("equals", b), Library.True);
        }

        [TestMethod]
        public void InvokeTest()
        {
            SRUObject o = Library.Expressions.ClosureExpression;
            string s = o.ToString();
            Assert.AreEqual(s, "ClosureExpression");
        }

        [TestMethod]
        public void FrameTest()
        {
            SRUObject frame = Library.Evaluator["current_frame"];
            Assert.AreEqual(frame.ToString(), "SRU.SRUObject");
        }

        [TestMethod]
        public void RefTest()
        {
            SRUExpression exp = new SRURefExpression()
            {
                Env = null,
                Name = "Number",
            };
            Assert.AreEqual(Library.Evaluator.Invoke("eval_expression", exp).ToString(),"Number");
        }

        [TestMethod]
        public void RefTest2()
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
            Assert.AreEqual(Library.Evaluator.Invoke("eval_expression", exp).ToString(), "{ -- Native -- }");
        }

        [TestMethod]
        public void CallTest()
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
            Assert.AreEqual(Library.Evaluator.Invoke("eval_expression", exp).ToString(), "<Number: 2 >");
        }

        [TestMethod]
        public  void ClosureTest0()
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
            Assert.AreEqual(Library.Evaluator.Invoke("eval_expression", exp).ToString(), "{|a| a\n }");
        }

        [TestMethod]
        public void ClosureTest()
        {
            // {|a| a }(Number)
            SRUExpression exp = new SRUCallExpression()
            {
                Method = new SRUClosureExpression()
                {
                    FormalArgument = new string[] { "a" },
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
            Assert.AreEqual(Library.Evaluator.Invoke("eval_expression", exp).ToString(), "Number");
        }

        [TestMethod]
        public void ClosureTest2()
        {
            // {|a,b| (a.parse)(a,b) }(Number,"3")
            SRUExpression exp = new SRUCallExpression()
            {
                Method = new SRUClosureExpression()
                {
                    FormalArgument = new string[] { "a", "b" },
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
            Assert.AreEqual(Library.Evaluator.Invoke("eval_expression", exp).ToString(), "<Number: 3 >");
        }

        [TestMethod]
        public void ExpTest()
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
            Assert.AreEqual(Library.Evaluator.Invoke("eval_expression", exp).ToString(), "");
        }

    }
}

