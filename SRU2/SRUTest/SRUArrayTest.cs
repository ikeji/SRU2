using SRU;
using Microsoft.VisualStudio.TestTools.UnitTesting;
namespace SRUTest
{
    
    
    /// <summary>
    ///SRUArrayTest のテスト クラスです。すべての
    ///SRUArrayTest 単体テストをここに含めます
    ///</summary>
    [TestClass()]
    public class SRUArrayTest
    {


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
        //テストを作成するときに、次の追加属性を使用することができます:
        //
        //クラスの最初のテストを実行する前にコードを実行するには、ClassInitialize を使用
        //[ClassInitialize()]
        //public static void MyClassInitialize(TestContext testContext)
        //{
        //}
        //
        //クラスのすべてのテストを実行した後にコードを実行するには、ClassCleanup を使用
        //[ClassCleanup()]
        //public static void MyClassCleanup()
        //{
        //}
        //
        //各テストを実行する前にコードを実行するには、TestInitialize を使用
        //[TestInitialize()]
        //public void MyTestInitialize()
        //{
        //}
        //
        //各テストを実行した後にコードを実行するには、TestCleanup を使用
        //[TestCleanup()]
        //public void MyTestCleanup()
        //{
        //}
        //
        #endregion


        /// <summary>
        ///RegesterArray のテスト
        ///</summary>
        [TestMethod()]
        public void RegesterArrayTest()
        {
            SRUObject Array = new SRUObject();
            SRUArray.RegesterArray(Array);
            Assert.IsTrue(Array.IsExistField("name"));
            Assert.IsTrue(Array.IsExistField("class"));
            Assert.IsTrue(Array.IsExistField("field_missing"));
            Assert.IsTrue(Array.IsExistField("super"));
        }
    }
}
