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
			//  This method will be called after migrating to the latest version.

			//  You can use the DbSet<T>.AddOrUpdate() helper extension method 
			//  to avoid creating duplicate seed data. E.g.
			//

			context.Trivias.AddOrUpdate(x => x.Text,
				new Trivia() { Text = "In Japan, letting a sumo wrestler make your baby cry is considered good luck." },
				new Trivia() { Text = "The actor who was inside R2-D2 hated the guy who played C-3PO, calling him the rudest man I've ever met." },
				new Trivia() { Text = "Sea otters hold hands when they sleep so they don't drift apart" },
				new Trivia() { Text = "In 1986, Apple launched a clothing line." },
				new Trivia() { Text = "Between 1900 and 1920, Tug of War was an Olympic event." },
				new Trivia() { Text = "The word unfriend appeared in print all the way back in 1659." },
				new Trivia() { Text = "A baby can cost new parents 750 hours of sleep in the first year." },
				new Trivia() { Text = "The Code of Hammurabi decreed that bartenders who watered down beer would be executed." },
				new Trivia() { Text = "Google was originally named BackRub." },
				new Trivia() { Text = "In 1999, the U.S. government paid the Zapruder family $16 million for the film of JFK's assassination." }
			);

			context.Users.AddOrUpdate(x => x.Email,
				new User { Name = "Admin", Email = "admin@trivio.com", Password = "admin" },
				new User { Name = "Neil Sanghrajka", Email = "neil@trivio.com", Password = "test1" },
				new User { Name = "Urvashi Mohnani", Email = "urvashi@trivio.com", Password = "test2" }
			);
		}  
    }
}
