using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SRU.SRU
{
    public static class SRUParser
    {
        public static void RegesterParser(SRUObject parser)
        {
            parser.RegesterMethod("parse", parse);
        }

        public static SRUObject parse(params SRUObject[] obj)
        {
            return new SRUObject();
        }
    }
}
