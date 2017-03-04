using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Web;

namespace Trivio.Models
{
    public class TrivioContext : DbContext
    {
        // You can add custom code to this file. Changes will not be overwritten.
        // 
        // If you want Entity Framework to drop and regenerate your database
        // automatically whenever you change your model schema, please use data migrations.
        // For more information refer to the documentation:
        // http://msdn.microsoft.com/en-us/data/jj591621.aspx
    
        public TrivioContext() : base("name=TrivioContext")
        {
        }

		public System.Data.Entity.DbSet<Trivio.Models.Trivia> Trivias { get; set; }

		public System.Data.Entity.DbSet<Trivio.Models.User> Users { get; set; }
	}
}
