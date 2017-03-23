namespace Trivio.Migrations
{
	using Models;
	using System;
	using System.Data.Entity;
	using System.Data.Entity.Migrations;
	using System.Linq;

	internal sealed class Configuration : DbMigrationsConfiguration<Trivio.Models.TrivioContext>
    {
        public Configuration()
        {
            AutomaticMigrationsEnabled = false;
        }

        protected override void Seed(Trivio.Models.TrivioContext context)
        {
			context.Users.AddOrUpdate(x => x.Email,
				new User { Name = "Admin", Email = "admin@trivio.com", Password = "admin" },
				new User { Name = "Neil Sanghrajka", Email = "neil@trivio.com", Password = "test1" },
				new User { Name = "Urvashi Mohnani", Email = "urvashi@trivio.com", Password = "test2" }
			);
		}
    }
}
